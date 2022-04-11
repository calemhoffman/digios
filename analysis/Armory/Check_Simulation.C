#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TArc.h>
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
#include "../Cleopatra/Isotope.h"

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
              pEElum1R,          /// 15
              pElum1RThetaCM,    /// 16
              pEmpty };          /// 17
plotID StringToPlotID(TString str);
void Check_Simulation(TString filename = "transfer.root", Int_t padSize = 300){

  printf("=========================== Check_Simulation.C\n");
  
  TMacro * config = new TMacro("../Armory/Check_Simulation_Config.txt");
  int numLine = config->GetListOfLines()->GetSize();
  int startLineNum = 0;
  for( int i = 0; i < numLine ; i++){
      TString haha = config->GetListOfLines()->At(i)->GetName();
      haha.Remove(4);
      if( haha != "////" ) {
         startLineNum = i;
         break;
      }
  }
  
  TString gate = ExtractString(startLineNum+1, config);
  double elumRange = ExtractNumber(startLineNum+2, config);
  vector<double> thetaCMRange = doubleConvertor( StringToVector( ExtractString(startLineNum+3,config) ));
  bool shownKELines = (ExtractString(startLineNum+4, config).Remove(4) == "true" ? true : false);
  bool isOverRideEx = (ExtractString(startLineNum+5, config).Remove(4) == "true" ? true : false);
  vector<double> oExRange = doubleConvertor( StringToVector ( ExtractString(startLineNum+6, config ))); 

   printf("%s \n", gate.Data());


  ///==== config Canvas
  vector<TString> plotConfig = StringToVector( ExtractString(startLineNum, config));
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

  if( colCount == 0 ) colCount = colCount_new;
  ///printf("plot row: %d, col: %d \n", rowCount, colCount);
  
  vector<int> Div = {colCount, rowCount};
  
  
  TFile * file = new TFile(filename, "read");
  TTree * tree = (TTree*) file->Get("tree");
  
  TObjArray * fxList = (TObjArray *) file->FindObjectAny("fxList");
  TObjArray * txList = (TObjArray *) file->FindObjectAny("txList");

  static int nExID = fxList->GetLast()+1;

  //================== reactionConfig
  TMacro * reactionConfigTxt = (TMacro *) file->FindObjectAny("reactionConfig");
  TString Reaction=reactionConfigTxt->GetName(); 

  ReactionConfig reactionConfig = LoadReactionConfig(reactionConfigTxt);

  int nEvent = reactionConfig.numEvents;
   printf("number of events generated : %d \n", nEvent);

   double xBeam = reactionConfig.beamX;
   double yBeam = reactionConfig.beamY; 
   printf("             beam position : (%5.2f, %5.2f) mm \n", xBeam, yBeam); 

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

  //================================== detetcor Geometry
  printf("=================================\n");
  printf(" loading detector Geometry.\n");
  TMacro * detGeoTxt = (TMacro *) file->FindObjectAny("detGeo");  

  DetGeo detGeo = LoadDetectorGeo(detGeoTxt);

  double field = detGeo.Bfield;  
  TString fdmsg = field > 0 ? "out of plan" : "into plan";
  TString msg2;
  msg2.Form("field = %.2f T, %s", field, fdmsg.Data());
  
  double length = detGeo.detLength;
  double posRecoil = detGeo.recoilPos;
  double rhoRecoilIn = detGeo.recoilInnerRadius;
  double rhoRecoilOut = detGeo.recoilOuterRadius;

  double posRecoil1 = detGeo.recoilPos1;
  double posRecoil2 = detGeo.recoilPos2;

  vector<double> pos = detGeo.detPos;

  float firstPos = detGeo.firstPos;
  int rDet = detGeo.nDet;
  int cDet = detGeo.mDet;

  double elum1 = detGeo.elumPos1;
  
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

  zRange[1] = TMath::Min(pos[0], pos[rDet-1]) - 50 ;
  zRange[2] = TMath::Max(pos[0], pos[rDet-1]) + 50 ;

  printf(" zRange : %f - %f \n", zRange[1], zRange[2]);
  printf("=================================\n");
  
   //========================================= Ex List;
   printf(" loading Ex list\n");
   
   TMacro * exListMacro = (TMacro *) file->FindObjectAny("ExList");
   int numEx = exListMacro->GetListOfLines()->GetSize() - 1 ;
   for(int i = 1; i <= numEx ; i++){
     string temp = exListMacro->GetListOfLines()->At(i)->GetName();
     if( temp[0] == '#' ) break;
     if( temp[0] == '/' ) continue;
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
         TH2F * hRecoilXY = new TH2F("hRecoilXY", Form("RecoilXY [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil ), 400, -rhoRecoilOut, rhoRecoilOut, 400,-rhoRecoilOut, rhoRecoilOut);
         tree->Draw("yRecoil:xRecoil>>hRecoilXY", gate, "colz");
         TArc * detArc1 = new TArc(0,0, rhoRecoilOut);
         detArc1->SetLineColor(kBlue-8);
         detArc1->SetFillStyle(0);
         detArc1->Draw("same");  
         TArc * detArc2 = new TArc(0,0, rhoRecoilIn);
         detArc2->SetLineColor(kBlue-8);
         detArc2->SetFillStyle(0);
         detArc2->Draw("same");  
         
         if( xBeam != 0. || yBeam != 0. ){
            TArc * arc = new TArc(xBeam, yBeam, 1);
            arc->SetLineColor(2);
            detArc1->SetFillStyle(0);
            arc->Draw("same");
         }
      }
      if( pID == pRecoilXY1       ){
         TH2F * hRecoilXY1 = new TH2F("hRecoilXY1", Form("RecoilXY-1 [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil1 ), 400, -rhoRecoilOut, rhoRecoilOut, 400,-rhoRecoilOut, rhoRecoilOut);
         tree->Draw("yRecoil1:xRecoil1>>hRecoilXY1", gate, "colz");
      }
      if( pID == pRecoilXY2       ){
         TH2F * hRecoilXY2 = new TH2F("hRecoilXY2", Form("RecoilXY-2 [gated] @ %4.0f mm; X [mm]; Y [mm]", posRecoil2 ), 400, -rhoRecoilOut, rhoRecoilOut, 400,-rhoRecoilOut, rhoRecoilOut);
         tree->Draw("yRecoil2:xRecoil2>>hRecoilXY2", gate, "colz");
      }
      if( pID == pRecoilRZ       ){
         TH2F * hRecoilRZ = new TH2F("hRecoilRZ", "RecoilR - Z [gated]; z [mm]; RecoilR [mm]",  zRange[0], zRange[1], zRange[2], 400,0, rhoRecoilOut);
         tree->Draw("rhoRecoil:z>>hRecoilRZ", gate, "colz");
      }
      if( pID == pRecoilRTR      ){
         FindRange("TB", gate, tree, recoilERange);
         TH2F * hRecoilRTR = new TH2F("hRecoilRTR", "RecoilR - recoilE [gated]; recoil Energy [MeV]; RecoilR [mm]", 500, recoilERange[0], recoilERange[1], 500, 0, rhoRecoilOut);
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
         TH1F * hExCal = new TH1F("hExCal", Form("calculated Ex [gated]; Ex [MeV]; count / %.2f keV",  (ExRange[1]-ExRange[0])/400.*1000),  400, ExRange[0], ExRange[1]);
         tree->Draw("ExCal>>hExCal", gate, "");
         Isotope hRecoil(reactionConfig.recoilHeavyA, reactionConfig.recoilHeavyZ);
         double Sn = hRecoil.CalSp(0,1);
         double Sp = hRecoil.CalSp(1,0);
         double Sa = hRecoil.CalSp(2,2);

         printf("Heavy recoil: %s \n", hRecoil.Name.c_str());
         printf("Sn : %f MeV/u \n", Sn);
         printf("Sp : %f MeV/u \n", Sp);
         printf("Sa : %f MeV/u \n", Sa);

         double yMax = hExCal->GetMaximum();
         TLine * lineSn = new TLine(Sn, 0, Sn, yMax); lineSn->SetLineColor(2); lineSn->Draw("");
         TLine * lineSp = new TLine(Sp, 0, Sp, yMax); lineSp->SetLineColor(4); lineSp->Draw("same");
         TLine * lineSa = new TLine(Sa, 0, Sa, yMax); lineSa->SetLineColor(6); lineSa->Draw("same");

         TLatex * text = new TLatex();
         text->SetTextFont(82);
         text->SetTextSize(0.06);
         text->SetTextColor(2); text->DrawLatex(Sn, yMax*0.9, "S_{n}");
         text->SetTextColor(4); text->DrawLatex(Sp, yMax*0.9, "S_{p}");
         text->SetTextColor(6); text->DrawLatex(Sa, yMax*0.9, "S_{a}");
         
      }
      if( pID == pRecoilRThetaCM ){
        TH2F * hRecoilRThetaCM = new TH2F("hRecoilRThetaCM", "RecoilR - thetaCM [gated]; thetaCM [deg]; RecoilR [mm]", 400, 0, 60, 400,0, rhoRecoilOut);
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

         if( xBeam != 0.0 || yBeam != 0.0 ){
            text.DrawLatex(0.0, 0.1, Form("Bema pos: (%4.1f, %4.1f) mm", xBeam, yBeam));
         }
      }
      
      if( pID == pElum1XY ){
         
         TH2F * hElum1XY = new TH2F("hElum1XY", Form("Elum-1 XY [gated] @ %.0f mm ; X [mm]; Y [mm]", elum1),  400, -elumRange, elumRange, 400, -elumRange, elumRange);
         tree->Draw("yElum1:xElum1>>hElum1XY", gate, "colz");
         
         double count = hElum1XY->GetEntries();
         
         if( count < 2000. ) {
            hElum1XY->SetMarkerStyle(7);
            if( count < 500. ) hElum1XY->SetMarkerStyle(3);
            hElum1XY->Draw("scat");
         }
      }

      if( pID == pEElum1R ){
         TH2F * hEElum1Rho = new TH2F("hEElum1Rho", "Elum-1 E-R [gated]; R[mm]; Energy[MeV]",  400, 0, elumRange, 400, eRange[0], eRange[1]);
         tree->Draw("Tb:rhoElum1>>hEElum1Rho", gate, "colz");
      }
      
      if( pID == pElum1RThetaCM){
         int angBin = 400;
         
         TH2F * hElum1RThetaCM = new TH2F("hElum1RThetaCM", "Elum-1 rho vs ThetaCM [gated]; thatCM [deg]; Elum- rho [mm]", angBin, thetaCMRange[0], thetaCMRange[1],  400, 0, elumRange);
         tree->Draw("rhoElum1:thetaCM>>hElum1RThetaCM", gate, "colz");   
         
         TH1F * htemp = (TH1F *) hElum1RThetaCM->ProjectionX("htemp");  
         
         double rel = (thetaCMRange[1] - thetaCMRange[0])*1.0/angBin;
         printf("angular resolution : %f deg \n", rel);
         
         vector<double> xList;
         double old_y = 0;
         for( int i = 1; i <= angBin; i++){
            double y = htemp->GetBinContent(i);
            if( old_y == 0 && y > 0) xList.push_back(htemp->GetBinCenter(i));
            if( old_y > 0 && y == 0 ) xList.push_back(htemp->GetBinCenter(i));
            old_y = y;
          }
                  
         printf("list of gaps :\n");
         for( int i = 0; i < (int) xList.size(); i+=2){
            printf("%d | %.3f - %.3f deg\n", i, xList[i], xList[i+1]);
         }
         
         TF1 f1("f1", "sin(x)");
         double acceptance = 0;
         double err1 = 0;
         double err2 = 0;
         for( int i = 0; i < (int) xList.size(); i += 2 ){
            acceptance += f1.Integral(xList[i] * TMath::DegToRad(), xList[i+1] * TMath::DegToRad() ) * TMath::TwoPi();
            err1 += f1.Integral((xList[i]-rel) * TMath::DegToRad(), (xList[i+1] + rel) * TMath::DegToRad() ) * TMath::TwoPi();
            err2 += f1.Integral((xList[i]+rel) * TMath::DegToRad(), (xList[i+1] - rel) * TMath::DegToRad() ) * TMath::TwoPi();
         }
         printf("acceptance = %f sr +- %f \n", acceptance, (err1-err2)/2);
         
         TLatex text;
         text.SetTextFont(82);
         text.SetTextSize(0.06);
         text.SetTextColor(2);
         text.SetTextAngle(90);

         for( int i = 0; i < (int) xList.size(); i++){
            text.DrawLatex(xList[i], elumRange/2, Form("%.2f", xList[i]));
         }
         
         text.SetNDC();
         text.SetTextAngle(0);
         text.DrawLatex(0.15, 0.15, Form("accp. = %.2f(%.2f) msr", acceptance * 1000., (err1-err2)*1000./2));         

      }
      
      if( pID == pHitID ){
         printf("=======================meaning of Hit ID\n");
         printf("   1 = light recoil hit array & heavy recoil hit recoil\n");
         printf("   0 = no detector\n");
         printf("  -1 = light recoil go opposite side of array\n");
         printf("  -2 = light recoil hit > det width\n");
         printf("  -3 = light recoil hit > array \n");
         printf("  -4 = light recoil hit blocker \n");
         printf(" -10 = light recoil orbit radius too big  \n");
         printf(" -11 = light recoil orbit radius too small\n");
         printf(" -12 = when reocol at the same side of array, light recoil blocked by recoil detector\n");
         printf(" -13 = more than 3 loops\n");
         printf(" -14 = heavy recoil did not hit recoil  \n");
         printf(" -15 = cannot find hit on array\n");
         printf(" -20 = unknown\n");
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
  if( pos >= 0 ) field.Remove(pos);
  
  return field.Atof();
  
}
TString ExtractString(int index, TMacro * macro){
  
  TString field = macro->GetListOfLines()->At(index)->GetName();

  int pos = field.First('/');
  if( pos >= 0 ) field.Remove(pos);
  
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
   if( str == "pEElum1R" ) return plotID::pEElum1R;           /// 14
   if( str == "pElum1RThetaCM" ) return plotID::pElum1RThetaCM;    /// 15
   if( str == "pEmpty" ) return plotID::pEmpty ;              /// 16
   
   return plotID::pEmpty;
}
