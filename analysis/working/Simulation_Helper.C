#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>


#include "../Cleopatra/Transfer.h"
#include "../Cleopatra/InFileCreator.h"
#include "../Cleopatra/ExtractXSec.h"
#include "../Cleopatra/PlotTGraphTObjArray.h"
#include "../Armory/Check_Simulation.C"
#include "../Armory/AutoFit.C"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

class MyMainFrame {
   RQ_OBJECT("MyMainFrame")
private:
   TGMainFrame         *fMain;
   TGGroupFrame * reactionframe;
   TRootEmbeddedCanvas *fEcanvas;
  
   ///TGNumberEntry * nb[4][2];
   ///TGNumberEntry * reactionEnergy;
   
   TGTextEdit * editor ;
   TString fileName;
   
   TGLabel * fileLabel;
   TGLabel * statusLabel;
   
   TGNumberEntry * angMin;
   TGNumberEntry * angMax;
   TGNumberEntry * angStep;
   
   TGCheckButton * withDWBA;
   
   TGCheckButton * isInFile;
   TGCheckButton * isRun;
   TGCheckButton * isExtract;
   TGCheckButton * isPlot;
   
   TGCheckButton * isElastic;
   
   
public:
   MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
   virtual ~MyMainFrame();
   void Command(int);
   void OpenFile(int);
   bool IsFileExist(TString filename);
};
MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {
   // Create a main frame
   fMain = new TGMainFrame(p,w,h);
  
   TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,600,600 );
   fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2,2,2,2));

   TGVerticalFrame *hframe1 = new TGVerticalFrame(fMain,600,600 );
   hframe->AddFrame(hframe1);
   
   TGVerticalFrame *hframe2 = new TGVerticalFrame(fMain,600,800 );
   hframe->AddFrame(hframe2);

   fileName = "reactionConfig.txt";
   
   TGHorizontalFrame *hframe00 = new TGHorizontalFrame(hframe2,600,600 );
   hframe2->AddFrame(hframe00, new TGLayoutHints(kLHintsCenterX, 2,2,2,2));
   
   fileLabel = new TGLabel(hframe00, "");
   fileLabel->SetWidth(370);
   fileLabel->SetHeight(20);
   //fileLabel->SetTextJustify(kTextLeft);
   fileLabel->SetTextColor(kRed);
   fileLabel->ChangeOptions(kFixedSize | kSunkenFrame);
   fileLabel->SetText(fileName);
   hframe00->AddFrame(fileLabel, new TGLayoutHints(kLHintsLeft, 2,2,2,2));
   
   
   TGTextButton *save = new TGTextButton(hframe00,"Save");
   save->SetWidth(100);
   save->SetHeight(20);
   save->ChangeOptions( save->GetOptions() | kFixedSize );
   save->Connect("Clicked()","MyMainFrame",this,"Command(=3)");
   hframe00->AddFrame(save, new TGLayoutHints(kLHintsLeft,5,5,3,4));
   
   TGTextButton *help = new TGTextButton(hframe00, "Help");
   help->SetWidth(100);
   help->SetHeight(20);
   help->ChangeOptions( help->GetOptions() | kFixedSize );
   help->Connect("Clicked()","MyMainFrame",this,"Command(=4)");
   hframe00->AddFrame(help,new  TGLayoutHints(kLHintsLeft, 5,5,3,4));
   
  
   editor = new TGTextEdit(hframe2, 600, 700);
   editor->LoadFile(fileName);
   hframe2->AddFrame(editor);
   
   statusLabel = new TGLabel(hframe2, "");
   statusLabel->SetWidth(600);
   statusLabel->SetHeight(20);
   statusLabel->SetTextJustify(kTextLeft);
   statusLabel->SetTextColor(1);
   statusLabel->ChangeOptions(kFixedSize | kSunkenFrame);
   hframe2->AddFrame(statusLabel, new TGLayoutHints(kLHintsLeft, 2,2,2,2));
   
   
   /**
   //============ Reaction setting
   TGGroupFrame * reactionframe = new TGGroupFrame(fMain, "Reaction Setting A(a,b)B", kVerticalFrame);
   hframe->AddFrame(reactionframe, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
   
   
   TGHorizontalFrame * fF[5];
   fF[0] = new TGHorizontalFrame(reactionframe, 170, 30, kFixedSize);
   reactionframe->AddFrame(fF[0]);
   
   TGLabel * lb = new TGLabel(fF[0], "");
   lb->SetWidth(50); lb->ChangeOptions( kFixedSize);
   fF[0]->AddFrame(lb, new TGLayoutHints(kLHintsCenterY, 10, 2, 0, 0));

   TGLabel * lbA = new TGLabel(fF[0], "A");
   lbA->SetWidth(50); lbA->ChangeOptions( kFixedSize);
   fF[0]->AddFrame(lbA, new TGLayoutHints(kLHintsCenterY, 10, 2, 0, 0));
   
   TGLabel * lbZ = new TGLabel(fF[0], "Z");
   lbZ->SetWidth(50); lbZ->ChangeOptions( kFixedSize);
   fF[0]->AddFrame(lbZ, new TGLayoutHints(kLHintsCenterY, 10, 2, 0, 0));
   
   
   for( int pID = 0; pID < 3 ; pID ++){
     
      TString name;
      int defaultA, defaultZ;
      if ( pID == 0 ) {name = "A"; defaultA = 12; defaultZ = 6;}
      if ( pID == 1 ) {name = "a"; defaultA =  2; defaultZ = 1;}
      if ( pID == 2 ) {name = "b"; defaultA =  1; defaultZ = 1;}
       
      fF[pID+1] = new TGHorizontalFrame(reactionframe, 170, 30, kFixedSize);
      reactionframe->AddFrame(fF[pID+1]);

      TGLabel * lb = new TGLabel(fF[pID+1], name + " : ");
      lb->SetWidth(50); lb->ChangeOptions( kFixedSize);
      fF[pID+1]->AddFrame(lb, new TGLayoutHints(kLHintsCenterY, 10, 2, 0, 0));
      
      nb[pID][0] = new TGNumberEntry(fF[pID+1], defaultA, 0, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
      nb[pID][0]->SetWidth(50);
      fF[pID+1]->AddFrame(nb[pID][0]);
      nb[pID][1] = new TGNumberEntry(fF[pID+1], defaultZ, 0, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
      nb[pID][1]->SetWidth(50);
      fF[pID+1]->AddFrame(nb[pID][1]);

   }
   
   fF[4] = new TGHorizontalFrame(reactionframe, 170, 30, kFixedSize);
   reactionframe->AddFrame(fF[4]);

   TGLabel * lbE = new TGLabel(fF[4], "Energy [MeV/u]");
   lbE->SetWidth(100); lbE->ChangeOptions( kFixedSize);
   fF[4]->AddFrame(lbE, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

   reactionEnergy = new TGNumberEntry(fF[4], 10, 0, 0, TGNumberFormat::kNESRealTwo, TGNumberFormat::kNEAPositive);
   reactionEnergy->SetWidth(70);
   fF[4]->AddFrame(reactionEnergy);
   **/
   
   //================= Simulation group
   TGGroupFrame * simFrame = new TGGroupFrame(fMain, "Kinematics Simulation", kVerticalFrame);
   hframe1->AddFrame(simFrame, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));
   
   TGTextButton *openRec = new TGTextButton(simFrame, "reaction Config");
   openRec->SetWidth(150);
   openRec->SetHeight(20);
   openRec->ChangeOptions( openRec->GetOptions() | kFixedSize );
   openRec->Connect("Clicked()","MyMainFrame",this, "OpenFile(=1)");
   simFrame->AddFrame(openRec,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *openDet = new TGTextButton(simFrame, "detector Geo.");
   openDet->SetWidth(150);
   openDet->SetHeight(20);
   openDet->ChangeOptions( openDet->GetOptions() | kFixedSize );
   openDet->Connect("Clicked()","MyMainFrame",this, "OpenFile(=0)");
   simFrame->AddFrame(openDet,new  TGLayoutHints(kLHintsRight, 5,5,3,4));
   
   TGTextButton *openEx = new TGTextButton(simFrame, "Ex List");
   openEx->SetWidth(150);
   openEx->SetHeight(20);
   openEx->ChangeOptions( openEx->GetOptions() | kFixedSize );
   openEx->Connect("Clicked()","MyMainFrame",this, "OpenFile(=2)");
   simFrame->AddFrame(openEx,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   withDWBA = new TGCheckButton(simFrame, "Sim with DWBA\n+example.root\n+example.Ex.txt");
   withDWBA->SetWidth(140);
   withDWBA->ChangeOptions(kFixedSize );
   simFrame->AddFrame(withDWBA, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));

   TGTextButton *Sim = new TGTextButton(simFrame,"Simulate");
   Sim->SetWidth(150);
   Sim->SetHeight(40);
   Sim->ChangeOptions( Sim->GetOptions() | kFixedSize );
   Sim->Connect("Clicked()","MyMainFrame",this,"Command(=1)");
   simFrame->AddFrame(Sim, new TGLayoutHints(kLHintsRight,5,5,3,4));

   TGTextButton *openSimChk = new TGTextButton(simFrame, "Config Simulation Plot");
   openSimChk->SetWidth(150);
   openSimChk->SetHeight(20);
   openSimChk->ChangeOptions( openSimChk->GetOptions() | kFixedSize );
   openSimChk->Connect("Clicked()","MyMainFrame",this, "OpenFile(=4)");
   simFrame->AddFrame(openSimChk,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *SimChk = new TGTextButton(simFrame,"Plot Simulation");
   SimChk->SetWidth(150);
   SimChk->SetHeight(40);
   SimChk->ChangeOptions( SimChk->GetOptions() | kFixedSize );
   SimChk->Connect("Clicked()","MyMainFrame",this,"Command(=2)");
   simFrame->AddFrame(SimChk, new TGLayoutHints(kLHintsRight,5,5,3,4));

   TGTextButton *autoFit = new TGTextButton(simFrame,"AutoFit ExCal");
   autoFit->SetWidth(150);
   autoFit->SetHeight(40);
   autoFit->ChangeOptions( autoFit->GetOptions() | kFixedSize );
   autoFit->Connect("Clicked()","MyMainFrame",this,"Command(=5)");
   simFrame->AddFrame(autoFit, new TGLayoutHints(kLHintsRight,5,5,3,4));

   //================= DWBA group
   TGGroupFrame * DWBAFrame = new TGGroupFrame(fMain, "DWBA calculation", kVerticalFrame);
   hframe1->AddFrame(DWBAFrame, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));

   TGTextButton *openDWBA = new TGTextButton(DWBAFrame, "DWBA setting");
   openDWBA->SetWidth(150);
   openDWBA->SetHeight(20);
   openDWBA->ChangeOptions( openDWBA->GetOptions() | kFixedSize );
   openDWBA->Connect("Clicked()","MyMainFrame",this, "OpenFile(=3)");
   DWBAFrame->AddFrame(openDWBA,new  TGLayoutHints(kLHintsRight, 5,5,3,4));
   
   TGTextButton *openInFile = new TGTextButton(DWBAFrame, "InFile");
   openInFile->SetWidth(150);
   openInFile->SetHeight(20);
   openInFile->ChangeOptions( openDWBA->GetOptions() | kFixedSize );
   openInFile->Connect("Clicked()","MyMainFrame",this, "OpenFile(=5)");
   DWBAFrame->AddFrame(openInFile,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *openOutFile = new TGTextButton(DWBAFrame, "OutFile");
   openOutFile->SetWidth(150);
   openOutFile->SetHeight(20);
   openOutFile->ChangeOptions( openDWBA->GetOptions() | kFixedSize );
   openOutFile->Connect("Clicked()","MyMainFrame",this, "OpenFile(=6)");
   DWBAFrame->AddFrame(openOutFile,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *xsecFile = new TGTextButton(DWBAFrame, "X-Sec");
   xsecFile->SetWidth(150);
   xsecFile->SetHeight(20);
   xsecFile->ChangeOptions( openDWBA->GetOptions() | kFixedSize );
   xsecFile->Connect("Clicked()","MyMainFrame",this, "OpenFile(=7)");
   DWBAFrame->AddFrame(xsecFile,new  TGLayoutHints(kLHintsRight, 5,5,3,4));
   
   //-------- angle setting
   TGHorizontalFrame * hframe000 = new TGHorizontalFrame(DWBAFrame, 150, 30, kFixedSize);
   DWBAFrame->AddFrame(hframe000);
   
   TGLabel * lb1 = new TGLabel(hframe000, "angMin");
   lb1->SetWidth(50); lb1->ChangeOptions( kFixedSize);
   hframe000->AddFrame(lb1, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 5, 0, 0));

   TGLabel * lb2 = new TGLabel(hframe000, "angMax");
   lb2->SetWidth(50); lb2->ChangeOptions( kFixedSize);
   hframe000->AddFrame(lb2, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 5, 0, 0));
   
   TGLabel * lb3 = new TGLabel(hframe000, "angStep");
   lb3->SetWidth(50); lb3->ChangeOptions( kFixedSize);
   hframe000->AddFrame(lb3, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 5, 0, 0));
   
   TGHorizontalFrame * hframe001 = new TGHorizontalFrame(DWBAFrame, 150, 30, kFixedSize);
   DWBAFrame->AddFrame(hframe001);
   
   angMin = new TGNumberEntry(hframe001, 0, 0, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   angMin->SetWidth(50);
   angMin->SetLimitValues(0, 180);
   hframe001->AddFrame(angMin, new TGLayoutHints(kLHintsCenterX , 5, 5, 0, 0));
   
   angMax = new TGNumberEntry(hframe001, 60, 0, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   angMax->SetWidth(50);
   angMax->SetLimitValues(0, 180);
   hframe001->AddFrame(angMax, new TGLayoutHints(kLHintsCenterX , 5, 5, 0, 0));
   
   angStep = new TGNumberEntry(hframe001, 1, 0, 0, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAPositive);
   angStep->SetWidth(50);
   angStep->SetLimitValues(0, 30);
   hframe001->AddFrame(angStep, new TGLayoutHints(kLHintsCenterX, 5, 5, 0, 0));

   //------- Check Boxes
   isInFile = new TGCheckButton(DWBAFrame, "Create inFile");
   isInFile->SetWidth(100);
   isInFile->ChangeOptions(kFixedSize );
   isInFile->SetState(kButtonDown);
   DWBAFrame->AddFrame(isInFile, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));
   
   isRun = new TGCheckButton(DWBAFrame, "Run Ptolemy");
   isRun->SetWidth(100);
   isRun->ChangeOptions(kFixedSize );
   isRun->SetState(kButtonDown);
   DWBAFrame->AddFrame(isRun, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));
   
   isExtract = new TGCheckButton(DWBAFrame, "Extract Xsec");
   isExtract->SetWidth(100);
   isExtract->ChangeOptions(kFixedSize );
   isExtract->SetState(kButtonDown);
   DWBAFrame->AddFrame(isExtract, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));

   isPlot = new TGCheckButton(DWBAFrame, "Plot");
   isPlot->SetWidth(100);
   isPlot->ChangeOptions(kFixedSize );
   isPlot->SetState(kButtonDown);
   DWBAFrame->AddFrame(isPlot, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));

   isElastic = new TGCheckButton(DWBAFrame, "Ratio to RutherFord");
   isElastic->SetWidth(130);
   isElastic->ChangeOptions(kFixedSize );
   DWBAFrame->AddFrame(isElastic, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));


   TGTextButton *DWBA = new TGTextButton(DWBAFrame, "DWBA");
   DWBA->SetWidth(150);
   DWBA->SetHeight(40);
   DWBA->ChangeOptions( DWBA->GetOptions() | kFixedSize );
   DWBA->Connect("Clicked()","MyMainFrame",this,"Command(=0)");
   DWBAFrame->AddFrame(DWBA,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *exit = new TGTextButton(hframe1,"Exit", "gApplication->Terminate(0)");
   exit->SetWidth(150);
   exit->SetHeight(40);
   exit->ChangeOptions( exit->GetOptions() | kFixedSize );
   hframe1->AddFrame(exit, new TGLayoutHints(kLHintsCenterX, 5,5,3,4));



   // Set a name to the main frame
   fMain->SetWindowName("Simulation Helper");

   // Map all subwindows of main frame
   fMain->MapSubwindows();

   // Initialize the layout algorithm
   fMain->Resize(fMain->GetDefaultSize());

   // Map main frame
   fMain->MapWindow();
}

bool MyMainFrame::IsFileExist(TString filename){
  ifstream file (filename.Data());  
  return file.is_open();
}

void MyMainFrame::OpenFile(int ID){
  
  editor->SaveFile(fileName);
  
  TString oldFileName = fileName;
    
  if ( ID == 0 ) fileName = "detectorGeo.txt";
  if ( ID == 1 ) fileName = "reactionConfig.txt";
  if ( ID == 2 ) fileName = "Ex.txt";
  if ( ID == 3 ) fileName = "example";
  if ( ID == 4 ) fileName = "../Armory/Check_Simulation.C";
  if ( ID == 5 ) fileName = "example.in";
  if ( ID == 6 ) fileName = "example.out";
  if ( ID == 7 ) fileName = "example.Xsec.txt";
  
  //test if file exist
  if ( IsFileExist(fileName) ){
    
    fileLabel->SetText(fileName);
    editor->LoadFile(fileName);
    statusLabel->SetText(fileName + "   opened.");
    
  }else{
  
    statusLabel->SetText(fileName + "   not exist.");    
    fileName = oldFileName;

  }
  
}

void MyMainFrame::Command(int ID) {

  editor->SaveFile(fileName);
  
  if( ID == 0 ){
    
    if( isInFile->GetState()) {
      double aMin = angMin->GetNumber();
      double aMax = angMax->GetNumber();
      double aStep = angStep->GetNumber();
      statusLabel->SetText("Creating example.in.....");
      InFileCreator("example", "example.in", aMin, aMax, aStep);
      statusLabel->SetText("in-file created.");
    }
    
    bool isRunOK = true;
    if( isRun->GetState() && IsFileExist("example.in") ) {
      //printf("run ptolemy...........\n");
      
      statusLabel->SetText("Running Ptolemy.....");
      int output = system("../Cleopatra/ptolemy <example.in> example.out");
      statusLabel->SetText("Check terminal, if no massage, Ptolemy run well.");
      
      printf("Ptolemy exist code : %d\n", output);
      if( output == 0 ) {
         isRunOK = true;
      }else{
         isRunOK = false;
         statusLabel->SetText("Ptolemy exist with problems.");
      }
    }
    
    if( isRunOK && isExtract->GetState() && IsFileExist("example.out")){
       int ElasticFlag = 0; // 1 for ratio to Rutherford, 2 for total Xsec
       if (isElastic->GetState()) ElasticFlag = 1;
       statusLabel->SetText("Extracting X-sec.....");
       ExtractXSec("example.out", ElasticFlag);
       statusLabel->SetText("X-sec Extracted.");
    }
    
    if( isRunOK && isPlot->GetState() && IsFileExist("example.root")){
       statusLabel->SetText("Plot X-sec.....");
       PlotTGraphTObjArray("example.root");
       statusLabel->SetText("Plotted X-sec.");
    }
  }
  
  if( ID == 1 ){
    string       basicConfig = "reactionConfig.txt";
    string  heliosDetGeoFile = "detectorGeo.txt";
    string    excitationFile = "Ex.txt"; //when no file, only ground state
    TString      ptolemyRoot = ""; // when no file, use isotropic distribution of thetaCM
    TString     saveFileName = "transfer.root";
    TString         filename = "reaction.dat"; //when no file, no output    
    
    if( withDWBA->GetState() ) {
       ptolemyRoot = "example.root";
       excitationFile = "example.Ex.txt";
    }
    statusLabel->SetText("Running simulation.......");
    Transfer( basicConfig, heliosDetGeoFile, excitationFile, ptolemyRoot, saveFileName,  filename);
    //gROOT->ProcessLine(".x ../Armory/Check_Simulation('transfer.root')");
    
    statusLabel->SetText("Plotting simulation.......");
    Check_Simulation(saveFileName);
    statusLabel->SetText("Plotted Simulation result");
  }
  if( ID == 2 ){
    //gROOT->ProcessLine(".x ../Armory/Check_Simulation('transfer.root')");
    statusLabel->SetText("Plotting simulation.......");
    Check_Simulation("transfer.root");
    statusLabel->SetText(" Plotted Simulation result");
  }
  
  if( ID == 3 ){
     if( fileName != "" ){
        statusLabel->SetText(fileName + "   saved.");
        if( fileName == "../Armory/Check_Simulation.C") {
           statusLabel->SetText(fileName + "   saved. PLEAE close and reOPEN Simulation_Helper.C for taking effect.");
        }
      }else{
         statusLabel->SetText("cannot save HELP page.");
      }
  }
  
  if( ID == 4 ){
     fileName = "";
     statusLabel->SetText("Help Page.");
     editor->LoadBuffer("===================== For Simulation");
     editor->AddLine("1) Make sure you check :");
     editor->AddLine("      a) reaction Config");
     editor->AddLine("      b) detector Geo.");
     editor->AddLine("      c) Ex List");
     editor->AddLine("");
     editor->AddLine("2) Not need to save file, fiel save when any button (except the Exit) is pressed.");
     editor->AddLine("");
     editor->AddLine("3) There is a checkbox for simulation with DWBA");
     editor->AddLine("      This requires the existance of example.root and example.Ex.txt");
     editor->AddLine("      These files can be generated by DWBA calculation.");
     editor->AddLine("      Please change the angMin = 0 and angMax = 180.");
     editor->AddLine("");
     editor->AddLine("4) After simulation, it will plot the result.");
     editor->AddLine("      To change the plotting, Click on the Config Simulation Plot.");
     editor->AddLine("      Please save, and exit, reOpen the Simulation_Helper to make the change effective.");
     editor->AddLine("");
     editor->AddLine("5) If the transfer.root is already here, simply Plot Simulation.");
     editor->AddLine("");
     editor->AddLine("===================== For DWBA (only for linux)");
     editor->AddLine("1) Only need to change the DWBA setting.");
     editor->AddLine("");
     editor->AddLine("2) The GUI offer a view on the infile and outfile.");
     editor->AddLine("");
     editor->AddLine("3) For elastics scattering, there is a checkbox for plotting the ratio to RutherFord.");
     editor->AddLine("");
     editor->AddLine("4) The flow of the DWBA calculation is like this:");
     editor->AddLine("      a) read the example file and convert to example.in");
     editor->AddLine("      b) run Ptolemy from example.in, and the output is example.out");
     editor->AddLine("      c) extract the cross section from the example.out, and save :");
     editor->AddLine("              * example.Xsec.txt");
     editor->AddLine("              * example.Ex.txt");
     editor->AddLine("              * example.root");
     editor->AddLine("      d) Plot the cross section from the example.root.");
     
  }
  
  if( ID == 5) {
     
     TH1F * temp = (TH1F*) gROOT->FindObjectAny("hExCal");
     
     if( temp != NULL ){
        fitAuto(temp, -1);
        statusLabel->SetText("Auto Fit hExCal");
     }else{
        statusLabel->SetText("Cannot find historgram hExCal. Please Run Plot Simulation first.");        
     }
      
     //gROOT->ProcessLine("fitAuto(hExCal, -1)");
     
  }
  
}

MyMainFrame::~MyMainFrame() {
   // Clean up used widgets: frames, buttons, layout hints
   fMain->Cleanup();
   delete fMain;
}

void Simulation_Helper() {

   new MyMainFrame(gClient->GetRoot(),800,600);
}
