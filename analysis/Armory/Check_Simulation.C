#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TMacro.h>
#include <TObjArray.h>
#include <fstream>
#include <TCutG.h>
#include "../Armory/AnalysisLibrary.h"

double * FindRange(TString branch, TString gate, TTree * tree, double output[2]);
double ExtractNumber(int index, TMacro * macro);
TString ExtractString(int index, TMacro * macro);
vector<TString> StringToVector(TString str);
vector<int> intConvertor(vector<TString> arr);
vector<double> doubleConvertor(vector<TString> arr);

enum plotID { pEZ,               /// 0
              pRecoilXY,         /// 1
              pRecoilXY1,        /// 2
              pRecoilXY2,        /// 3
              pRecoilRZ,         /// 4
              pRecoilRTR,        /// 5
              pTDiffZ,           /// 6
              pThetaCM,          /// 7
              pThetaCM_Z,        /// 8
              pExCal,            /// 9
              pRecoilRThetaCM,   /// 10
              pArrayXY,          /// 11
              pInfo,             /// 12
              pHitID,            /// 13
              pElum1XY,          /// 14
              pElum1RThetaCM,    /// 15
              pEmpty };          /// 16
plotID StringToPlotID(TString str);
void Check_Simulation(TString filename = "transfer.root", Int_t padSize = 300){
  
  TMacro * config = new TMacro("../Armory/Check_Simulation_Config.txt");
  TString gate = ExtractString(19, config);
  double elumRange = ExtractNumber(20, config);
  vector<double> thetaCMRange = doubleConvertor( StringToVector( ExtractString(21,config) ));
  bool shownKELines = (ExtractString(22, config).Remove(4) == "true" ? true : false);
  bool isOverRideEx = (ExtractString(23, config).Remove(4) == "true" ? true : false);
  vector<double> oExRange = doubleConvertor( StringToVector ( ExtractString(24, config ))); 
  
  vector<TString> plotConfig = StringToVector( ExtractString(18, config));

  vector<plotID> canvas;
  int colCount = 0;
  int colCount_new = 0;
  int rowCount = 1;
  for( int i = 0; i < (int) plotConfig.size(); i++){
     if( plotConfig[i] == "break" ) {
        rowCount ++;
        if( colCount_new > colCount ) colCount = colCount_new;
        colCount_new = 0; 
        continue;
     }
     canvas.push_back( StringToPlotID(plotConfig[i]));
     colCount_new ++;
  }
  
  vector<int> Div = {colCount, rowCount};
  
  
  TFile * file = new TFile(filename, "read");
  TTree * tree = (TTree*) file->Get("tree");
  
  TObjArray * fxList = (TObjArray *) file->FindObjectAny("fxList");
  TObjArray * txList = (TObjArray *) file->FindObjectAny("txList");

  static int nExID = fxList->GetLast()+1;

  TMacro * reactionConfig = (TMacro *) file->FindObjectAny("reactionConfig");
  TString Reaction=reactionConfig->GetName(); ///TODO change to Latex

   gStyle->SetOptStat("");
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);
   

   double eRange[2] = {0, 10};
   double zRange[3] = {400, -1000, 1000}; /// zRange[0] = nBin
   double recoilERange[2];
   vector<double> exList;
   double ExRange[2];

//========================================= detector Geometry
  //================================== detetcor Geometry
  printf("=================================\n");
  printf(" loading detector Geometry.\n");
  TMacro * detGeo = (TMacro *) file->FindObjectAny("detGeo");  
  double field = ExtractNumber(0, detGeo);
  TString fdmsg = field > 0 ? "out of plan" : "into plan";
  TString msg2;
  msg2.Form("field = %.2f T, %s", field, fdmsg.Data());
  
  double length = ExtractNumber(5, detGeo);
  
  double posRecoil  = ExtractNumber(6, detGeo);
  double rhoRecoil  = ExtractNumber(7, detGeo);
  double posRecoil1 = ExtractNumber(9, detGeo);
  double posRecoil2 = ExtractNumber(10, detGeo);
  double firstPos   = ExtractNumber(14, detGeo);  
   
  vector<double> pos;
  int nLine = detGeo->GetListOfLines()->GetLast() + 1;
  for( int i = 18; i < nLine; i++){
    TString temp = ExtractString(i, detGeo);
    if( temp[0] == '#' ) break;
    pos.push_back(ExtractNumber(i, detGeo));
  }

  int rDet = pos.size(); /// number of detector at different position, row-Det
  int cDet = (int) ExtractNumber(17, detGeo); /// number of detector at same position, column-Det

  vector<double> posTemp = pos;
  for(int id = 0; id < rDet; id++){
    if( firstPos > 0 ) pos[id] = firstPos + posTemp[id];
    if( firstPos < 0 ) pos[id] = firstPos - posTemp[rDet -1 - id];
  }  
  
  printf("number of row-Det : %d \n", rDet);
  printf("number of col-Det : %d \n", cDet);
  
  for(int i = 0; i < rDet ; i++){
     if( firstPos > 0 ){
        printf("%d, %10.2f mm - %10.2f mm \n", i, pos[i], pos[i] + length);
     }else{
        printf("%d, %10.2f mm - %10.2f mm \n", i, pos[i] - length , pos[i]);
     }
  }
  printf("=================================\n");
  
  int numDet = rDet * cDet;

  if( firstPos > 0 ){
    zRange[1] = pos[0]-50;
    zRange[2] = pos[rDet-1] + length + 50;
  }else{
    zRange[1] = pos[0]- length - 50;
    zRange[2] = pos[rDet-1] + 50;
  }

  printf(" zRange : %f - %f \n", zRange[1], zRange[2]);
  printf("=================================\n");
  
   //========================================= Ex List;
   printf(" loading Ex list\n");
   
   TMacro * exListMacro = (TMacro *) file->FindObjectAny("ExList");
   int numEx = exListMacro->GetListOfLines()->GetSize() - 1 ;
   for(int i = 1; i <= numEx ; i++){
     string temp = exListMacro->GetListOfLines()->At(i)->GetName();
     if( temp[0] == '#' ) break;
     vector<string> tempStr = SplitStr(temp, " ");
     printf("%d | %s \n", i, tempStr[0].c_str());
     exList.push_back(atof(tempStr[0].c_str()));
   }
  
   double exSpan = exList.back() - exList[0];
   

   ExRange[0] = exList[0] - exSpan * 0.2;
   ExRange[1] = exList.back() + exSpan * 0.2;
   if( isOverRideEx ) {
      ExRange[0] = oExRange[0];
      ExRange[1] = oExRange[1];
   }
   
   printf("=================================\n");
  
   //========================================= reaction parameters
   
   printf(" loading reaction parameters \n");
   TMacro * reactionData = (TMacro *) file->FindObjectAny("reactionData");
   
   double mass  = ExtractNumber(0, reactionData);
   double q     = ExtractNumber(1, reactionData);
   double beta  = ExtractNumber(2, reactionData);
   double Et    = ExtractNumber(3, reactionData);
   double massB = ExtractNumber(4, reactionData);
   double alpha = ExtractNumber(5, reactionData);
   
   double gamm = 1./TMath::Sqrt(1-beta*beta);
   double slope = alpha * beta;
   printf("\tmass-b    : %f MeV/c2 \n", mass);
   printf("\tcharge-b  : %f \n", q);
   printf("\tE-total   : %f MeV \n", Et);
   printf("\tmass-B    : %f MeV/c2 \n", massB);
   printf("\tbeta      : %f \n", beta);
   printf("\tslope     : %f MeV/mm \n", slope);
   printf("=================================\n");
   
   //=================================== calculate Ranges
   
   //eRange by zRange and exList
   double QQ = (Et*Et + mass*mass - (massB-exList[0])*(massB-exList[0]))/2/Et;
   double intercept = QQ/gamm - mass;   
   eRange[1] = intercept + zRange[2] * slope;
   ///printf("intercept of 0 MeV : %f MeV \n", intercept); 
   ///printf("eRange 0 MeV : %f MeV \n", eRange[1]); 
   
   //thetaCMRange
   ///double momentum = sqrt(( Et*Et - pow(mass + massB - exList[0],2)) * ( Et*Et - pow(mass - massB + exList[0],2)))/2/Et;
   ///double thetaMax = acos( (beta * QQ- alpha / gamm * zRange[2])/momentum) * TMath::RadToDeg();
   ///thetaCMRange[1] = (int) TMath::Ceil(thetaMax/10.)*10;
   ///printf(" momentum    : %f \n", momentum);
   ///printf(" thetaCM Max : %f \n", thetaMax);
   ///printf(" thetaCM Range : %d \n", thetaCMRange[1]);
   
   //===================================================
   printf("============================== Gate\n");
   printf("gate : %s\n", gate.Data());
   printf("====================================\n");

      
   Int_t size[2] = {padSize,padSize}; ///x,y, single Canvas size
   TCanvas * cCheck = new TCanvas("cCheck", "Check For Simulation", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cCheck->GetShowEditor() )cCheck->ToggleEditor();
   if(cCheck->GetShowToolBar() )cCheck->ToggleToolBar();
   cCheck->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck->cd(i);
      cCheck->cd(i)->SetGrid();
      if( canvas[i-1] == pThetaCM ) {
         cCheck->cd(i)->SetGrid(0,0);
         cCheck->cd(i)->SetLogy();
      }
      if( canvas[i-1] == pHitID ){
         cCheck->cd(i)->SetLogy();
      }
      plotID pID = canvas[i-1];
      ///########################################
      if( pID == pEZ){
         TH2F * hez = new TH2F("hez", Form("e-z [gated] @ %5.0f mm; z [mm]; e [MeV]", firstPos), zRange[0], zRange[1], zRange[2], 400, eRange[0], eRange[1]);
         tree->Draw("e:z>>hez", gate, "colz");
         if( shownKELines){
            for( int i = 0; i < nExID ; i++){
               fxList->At(i)->Draw("same");
            }
         }
      }
      if( pID == pRecoilXY       ){
         TH2F * hRecoilXY = new TH2F("hRecoilXY", Form("RecoilXY [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil ), 400, -rhoRecoil, rhoRecoil, 400,-rhoRecoil, rhoRecoil);
         tree->Draw("yRecoil:xRecoil>>hRecoilXY", gate, "colz");
      }
      if( pID == pRecoilXY1       ){
         TH2F * hRecoilXY1 = new TH2F("hRecoilXY1", Form("RecoilXY-1 [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil1 ), 400, -rhoRecoil, rhoRecoil, 400,-rhoRecoil, rhoRecoil);
         tree->Draw("yRecoil1:xRecoil1>>hRecoilXY1", gate, "colz");
      }
      if( pID == pRecoilXY2       ){
         TH2F * hRecoilXY2 = new TH2F("hRecoilXY2", Form("RecoilXY-2 [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil2 ), 400, -rhoRecoil, rhoRecoil, 400,-rhoRecoil, rhoRecoil);
         tree->Draw("yRecoil2:xRecoil2>>hRecoilXY2", gate, "colz");
      }
      if( pID == pRecoilRZ       ){
         TH2F * hRecoilRZ = new TH2F("hRecoilRZ", "RecoilR - Z [gated]; z [mm]; RecoilR [mm]",  zRange[0], zRange[1], zRange[2], 400,0, rhoRecoil);
         tree->Draw("rhoRecoil:z>>hRecoilRZ", gate, "colz");
      }
      if( pID == pRecoilRTR      ){
         FindRange("TB", gate, tree, recoilERange);
         TH2F * hRecoilRTR = new TH2F("hRecoilRTR", "RecoilR - recoilE [gated]; recoil Energy [MeV]; RecoilR [mm]", 500, recoilERange[0], recoilERange[1], 500, 0, rhoRecoil);
         tree->Draw("rhoRecoil:TB>>hRecoilRTR", gate, "colz");
      }
      if( pID == pTDiffZ         ){
        double tDiffRange [2];
        FindRange("t-tB", gate, tree, tDiffRange);
        TH2F * hTDiffZ = new TH2F("hTDiffZ", "time(Array) - time(Recoil) vs Z [gated]; z [mm]; time diff [ns]", zRange[0], zRange[1], zRange[2],  500, tDiffRange[0], tDiffRange[1]);
        tree->Draw("t - tB : z >> hTDiffZ", gate, "colz");
      }
      if( pID == pThetaCM        ){
        TH1F * hThetaCM[nExID];
        TLegend * legend = new TLegend(0.8,0.2,0.99,0.8);
        double maxCount = 0;
        int startID = 0; // set the start ExID
        for( int i = startID; i < nExID; i++){
          hThetaCM[i] = new TH1F(Form("hThetaCM%d", i), Form("thetaCM [gated] (ExID=%d); thetaCM [deg]; count", i), 200, thetaCMRange[0], thetaCMRange[1]);
          hThetaCM[i]->SetLineColor(i+1-startID);
          hThetaCM[i]->SetFillColor(i+1-startID);
          hThetaCM[i]->SetFillStyle(3000+i-startID);
          tree->Draw(Form("thetaCM>>hThetaCM%d", i), gate + Form("&& ExID==%d", i), "");
          legend->AddEntry(hThetaCM[i], Form("Ex=%5.1f MeV", exList[i]));
          double max = hThetaCM[i]->GetMaximum();
          if( max > maxCount ) maxCount = max;
        }

        for( int i = startID; i < nExID; i++){
          hThetaCM[i]->GetYaxis()->SetRangeUser(1, maxCount * 1.2);
          if( i == startID ) {
            hThetaCM[i]->Draw();
          }else{
            hThetaCM[i]->Draw("same");
          }
        }   
        legend->Draw();
      }
      if( pID == pThetaCM_Z      ){
         TH2F *hThetaCM_Z = new TH2F("hThetaCM_Z","ThetaCM vs Z ; Z [mm]; thetaCM [deg]",zRange[0], zRange[1], zRange[2], 200, thetaCMRange[0], thetaCMRange[1]);
         tree->Draw("thetaCM:z>>hThetaCM_Z",gate,"col");
         if( shownKELines){
            for( int i = 0; i < nExID ; i++){
               txList->At(i)->Draw("same");
            }
         }

      }
      if( pID == pExCal          ){
         TH1F * hExCal = new TH1F("hExCal", "calculated Ex [gated]; Ex [MeV]; count",  400, ExRange[0], ExRange[1]);
         tree->Draw("ExCal>>hExCal", gate, "");
      }
      if( pID == pRecoilRThetaCM ){
        TH2F * hRecoilRThetaCM = new TH2F("hRecoilRThetaCM", "RecoilR - thetaCM [gated]; thetaCM [deg]; RecoilR [mm]", 400, 0, 60, 400,0, rhoRecoil);
        tree->Draw("rhoRecoil:thetaCM>>hRecoilRThetaCM", gate, "colz");
      }
      if( pID == pArrayXY ){
        TH2F * hArrayXY = new TH2F("hArrayXY", "Array-XY [gated]; X [mm]; Y [mm]", 400, -20, 20, 400, -20, 20);
        tree->Draw("yArray:xArray>>hArrayXY", gate, "colz");
      }
      if( pID == pInfo           ){
         TLatex text;
         text.SetNDC();
         text.SetTextFont(82);
         text.SetTextSize(0.06);
         text.SetTextColor(2);

         text.DrawLatex(0., 0.9, Reaction);
         text.DrawLatex(0., 0.8, msg2);
         text.SetTextColor(1);
         text.DrawLatex(0., 0.7, "gate:");
         
         text.SetTextColor(2);
         //check gate text length, if > 30, break by "&&" 
         int ll = gate.Length();
         if( ll > 30 ) {
           vector<string> strList = SplitStr( (string) gate.Data(), "&&");
           for( int i = 0; i < strList.size(); i++){
              text.DrawLatex(0., 0.6 - 0.05*i, (TString) strList[i]);
           }
         }else{
            text.DrawLatex(0., 0.6, gate);
         }
      }
      
      if( pID == pElum1XY ){
         TH2F * hElum1XY = new TH2F("hElum1XY", "Elum-1 XY [gated]; X [mm]; Y [mm]",  400, -elumRange, elumRange, 400, -elumRange, elumRange);
         tree->Draw("yElum1:xElum1>>hElum1XY", gate, "colz");
      }
      
      if( pID == pElum1RThetaCM){
         TH2F * hElum1RThetaCM = new TH2F("hElum1RThetaCM", "Elum-1 rho vs ThetaCM [gated]; thatCM [deg]; Elum- rho [mm]", 400, thetaCMRange[0], thetaCMRange[1],  400, 0, elumRange);
         tree->Draw("rhoElum1:thetaCM>>hElum1RThetaCM", gate, "colz");   
         
         TH1F * htemp = (TH1F *) hElum1RThetaCM->ProjectionX("htemp");  
         
         double xMin = 0 , xMax = 0;
         for( int i = 1; i <= 400; i++){
           double y = htemp->GetBinContent(i);
           if( y > 0 && xMin == 0 ) xMin = htemp->GetBinCenter(i);
         }
         
         for( int i = 400; i >= 0; i--){
           double y = htemp->GetBinContent(i);
           if( y > 0 && xMax == 0 ) xMax = htemp->GetBinCenter(i);
         }
         
         printf("xMin : %f deg \n", xMin);
         printf("xMax : %f deg \n", xMax);
         
         TF1 f1("f1", "sin(x)");
         double acceptance = f1.Integral(xMin * TMath::DegToRad(), xMax * TMath::DegToRad() ) * TMath::TwoPi();
         
         printf("acceptance = %f sr \n", acceptance);
         
         TLatex text;
         text.SetTextFont(82);
         text.SetTextSize(0.06);
         text.SetTextColor(2);
         text.SetTextAngle(90);

         text.DrawLatex(xMin, elumRange/2, Form("%.2f", xMin));
         text.DrawLatex(xMax, elumRange/2, Form("%.2f", xMax));
      }
      
      if( pID == pHitID ){
         printf("=======================meaning of Hit ID\n");
         printf("  1 = light recoil hit array & heavy recoil hit recoil\n");
         printf("  0 = when no detector\n");
         printf(" -1 = light recoil blocked by recoil detector\n");
         printf(" -2 = heavy recoil missed recoil detector\n");
         printf(" -3 = light recoil looped over 10 loops\n");
         printf(" -4 = light recoil over shoot array \n");
         printf(" -5 = light recoil hit array from inside, becasue of math\n");
         printf(" -6 = light recoil blocked by the structure between target and array detector\n");
         printf(" -7 = light recoil hit the gap between array detectors (along z)\n");
         printf(" -8 = light recoil hit the gap between array detectors (along xy)\n");
         printf(" -9 = light recoil don't hit any array detectors (exclude above hit-ID)\n");
         printf("-10 = light recoil hit HELIOS wall or go to opposite direction.\n");
         printf("            (e.g. upstream array, light particle go downstream)\n");
         printf("===========================================\n");
         
         TH1F * hHit = new TH1F("hHit", "hit; hit-ID; count", 13, -11, 2);
         tree->Draw("hit>>hHit", "", "");
      }
      ///#######################################################

   }
   
   cCheck->Modified();
   cCheck->Update();
   
}

///============================================================
///============================================================

double * FindRange(TString branch, TString gate, TTree * tree, double output[2]){
   tree->Draw(Form("%s>>temp1", branch.Data()), gate);
   TH1F * temp1 = (TH1F *) gROOT->FindObjectAny("temp1");

   output[1] = temp1->GetXaxis()->GetXmax();
   output[0] = temp1->GetXaxis()->GetXmin();

   delete temp1;
   return output;
}

double ExtractNumber(int index, TMacro * macro){
  
  TString field = macro->GetListOfLines()->At(index)->GetName();
  
  int pos = field.First('/');
  
  if( pos > -1 ) field.Remove(pos);
  
  return field.Atof();
  
}
TString ExtractString(int index, TMacro * macro){
  
  TString field = macro->GetListOfLines()->At(index)->GetName();
  
  int pos = field.First('/');
  
  if( pos > -1 ) field.Remove(pos);
  
  return field;
  
}

vector<TString> StringToVector(TString str){
   
   vector<TString> temp;
   
   bool startFlag = false;
   bool endFlag = false;
   string jaja="";
   for(int i = 0; i < str.Length(); i++){
         
      if( str[i] == '{' ) {
         startFlag = true;
         continue;
      }
      if( str[i] == ' '){
         continue;
      }
      if( startFlag && !endFlag){
         
         if( str[i] == ',' ){
            temp.push_back(jaja);
            jaja="";
            continue;
         }
         
         if( str[i] == '}') {
            temp.push_back(jaja);
            endFlag = true;
            continue;
         }
         jaja += str[i];
         
      }
   }
   return temp;
}

vector<int> intConvertor(vector<TString> arr){
   vector<int> out ;
   for( int i = 0 ; i < (int) arr.size(); i++){
      out.push_back( arr[i].Atoi());
   }
   return out;
}
vector<double> doubleConvertor(vector<TString> arr){
   vector<double> out ;
   for( int i = 0 ; i < (int) arr.size(); i++){
      out.push_back( arr[i].Atof());
   }
   return out;
}

plotID StringToPlotID(TString str){
   
   if( str == "pEZ") return plotID::pEZ;                      ///0
   if( str == "pRecoilXY") return plotID::pRecoilXY;                /// 1
   if( str == "pRecoilXY1" ) return plotID::pRecoilXY1;       /// 2
   if( str == "pRecoilXY2" ) return plotID::pRecoilXY2;       /// 3
   if( str == "pRecoilRZ" ) return plotID::pRecoilRZ;         /// 4
   if( str == "pRecoilRTR" ) return plotID::pRecoilRTR;       /// 5
   if( str == "pTDiffZ" ) return plotID::pTDiffZ;             /// 6
   if( str == "pThetaCM" ) return plotID::pThetaCM;           /// 7
   if( str == "pThetaCM_Z" ) return plotID::pThetaCM_Z;       /// 8
   if( str == "pExCal" ) return plotID::pExCal;               /// 9
   if( str == "pRecoilRThetaCM" ) return plotID::pRecoilRThetaCM;   /// 10
   if( str == "pArrayXY" ) return plotID::pArrayXY;           /// 11
   if( str == "pInfo" ) return plotID::pInfo;                 /// 12
   if( str == "pHitID" ) return plotID::pHitID;               /// 13
   if( str == "pElum1XY" ) return plotID::pElum1XY;           /// 14
   if( str == "pElum1RThetaCM" ) return plotID::pElum1RThetaCM;    /// 15
   if( str == "pEmpty" ) return plotID::pEmpty ;              /// 16
   
   return plotID::pEmpty;
}
