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

enum plotID { pEZ,               /// 0
              pRecoilXY,         /// 1
              pRecoilRZ,         /// 2
              pRecoilRTR,        /// 3
              pTDiffZ,           /// 4
              pThetaCM,          /// 5
              pThetaCM_Z,        /// 6
              pExCal,            /// 7
              pRecoilRThetaCM,   /// 8
              pInfo,             /// 9
              pHitID,            /// 10
              pEmpty };          /// 11

//======================================== User Setting

const int Div[2] = {3,2}; /// x,y
plotID canvas[6] = { pEZ, pRecoilXY, pThetaCM,
                     pThetaCM_Z, pExCal, pInfo};

//TString gate = "hit == 1 && rhoRecoil > 10 && rhoElum1 > 72.6 && loop == 1";
//TString gate = "hit == 1 && loop <= 1 && rhoRecoil > 10 ";
TString gate = "hit == 1 && loop <= 1 && thetaCM > 10";

//override ExRange;
bool isOverRideEx = false;
double oExRange[2] = {-2, 6};
   
int thetaCMRange[2] = {0, 60}; 
double zRange[3] = {400, -1000, 1000}; /// zRange[0] = nBin

bool shownKELines = false;

//============================================== end of user setting

double * FindRange(TString branch, TString gate, TTree * tree, double output[2]);
vector<string> SplitStr(string tempLine, string splitter, int shift = 0);
void Plot(plotID pID);

TFile * file;
TTree * tree; 
TString Reaction;
TString msg2; 
TObjArray * fxList;
TObjArray * txList;

double length = 50.5;
double firstPos = -110;
double rhoRecoil = 50;
double posRecoil = 400;

double eRange[2] = {0, 10};
double recoilERange[2];
vector<double> exList;
double ExRange[2];
static int nExID;

void Check_Simulation(TString filename = "transfer.root", Int_t padSize = 300){
  
  file = new TFile(filename, "read");
  tree = (TTree*) file->Get("tree");
  
  TMacro * reactionConfig = (TMacro *) file->FindObjectAny("reactionConfig");
  Reaction=reactionConfig->GetName(); ///TODO change to Latex
  
  TMacro * detGeo = (TMacro *) file->FindObjectAny("detGeo");
  TString field = detGeo->GetListOfLines()->At(0)->GetName();
  int pp = field.First('/');
  field.Remove(pp);
  msg2.Form("field = %.2f T", field.Atof());

  fxList = (TObjArray *) file->FindObjectAny("fxList");
  txList = (TObjArray *) file->FindObjectAny("txList");

  nExID = fxList->GetLast()+1;

   gStyle->SetOptStat("");
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);


//========================================= detector Geometry
   string detGeoFileName = "detectorGeo.txt";
   int numDet;
   int rDet = 5; /// number of detector at different position, row-Det
   int cDet = 6; /// number of detector at same position, column-Det
   vector<double> pos;
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream detFile;
   detFile.open(detGeoFileName.c_str());
   int i = 0;
   if( detFile.is_open() ){
      string x;
      while( detFile >> x){
         ///printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 5 ) length   = atof(x.c_str());
         if( i == 6 ) posRecoil = atof(x.c_str());
         if( i == 7 ) rhoRecoil = atof(x.c_str());
         if( i == 14 ) firstPos = atof(x.c_str());
         if( i == 17 ) cDet = atoi(x.c_str());
         if( i >= 18 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      rDet = pos.size();
      detFile.close();
      printf("... done.\n");
      
      vector<double> posTemp = pos;
      for(int id = 0; id < rDet; id++){
        if( firstPos > 0 ) pos[id] = firstPos + posTemp[id];
        if( firstPos < 0 ) pos[id] = firstPos - posTemp[rDet -1 - id];
      }
      
      for(int i = 0; i < rDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %10.2f mm - %10.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %10.2f mm - %10.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");

          
   }else{
       printf("... fail\n");
       return;
   }
   
   numDet = rDet * cDet;
   
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
   printf(" loading reaction parameters");
   ifstream reactionfile;
   reactionfile.open("reaction.dat");
   double mass, q, beta, Et, massB;
   double alpha, gamm, slope;
   
   if( reactionfile.is_open() ){
      string x;
      int i = 0;
      while( reactionfile >> x ){
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) mass = atof(x.c_str());
         if( i == 1 ) q    = atof(x.c_str());
         if( i == 2 ) beta = atof(x.c_str()); 
         if( i == 3 ) Et   = atof(x.c_str()); 
         if( i == 4 ) massB = atof(x.c_str()); 
         if( i == 5 ) alpha = atof(x.c_str()); 
         i = i + 1;
      }
      printf("... done.\n");

      gamm = 1./TMath::Sqrt(1-beta*beta);
      slope = alpha * beta;
      printf("\tmass-b    : %f MeV/c2 \n", mass);
      printf("\tcharge-b  : %f \n", q);
      printf("\tE-total   : %f MeV \n", Et);
      printf("\tmass-B    : %f MeV/c2 \n", massB);		 
      printf("\tbeta      : %f \n", beta);
      printf("\tslope     : %f MeV/mm \n", slope);
      printf("=================================\n");

   }else{
      printf("... fail.\n");
   }
   reactionfile.close();
   
   
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
      Plot(canvas[i-1]);
   }
   
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

vector<string> SplitStr(string tempLine, string splitter, int shift = 0){

  vector<string> output;

  size_t pos;
  do{
    pos = tempLine.find(splitter); // fine splitter
    if( pos == 0 ){ //check if it is splitter again
      tempLine = tempLine.substr(pos+1);
      continue;
    }

    string secStr;
    if( pos == string::npos ){
      secStr = tempLine;
    }else{
      secStr = tempLine.substr(0, pos+shift);
      tempLine = tempLine.substr(pos+shift);
    }

    //check if secStr is begin with space
    if( secStr.substr(0, 1) == " "){
      secStr = secStr.substr(1);
    }

    output.push_back(secStr);
    //printf(" |%s---\n", secStr.c_str());
    
  }while(pos != string::npos );

  return output;
}

void Plot(plotID pID) {
   
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
      TH1F * hExCal = new TH1F("hExCal", "calculated Ex [gated]; Ex [MeV]; count",  200, ExRange[0], ExRange[1]);
      tree->Draw("ExCal>>hExCal", gate, "");
   }
   if( pID == pRecoilRThetaCM ){
     TH2F * hRecoilRThetaCM = new TH2F("hRecoilRThetaCM", "RecoilR - thetaCM [gated]; thetaCM [deg]; RecoilR [mm]", 400, 0, 60, 400,0, rhoRecoil);
     tree->Draw("rhoRecoil:thetaCM>>hRecoilRThetaCM", gate, "colz");
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
   
   
}
