#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGTextEditor.h>
#include <TGNumberEntry.h>
#include <TGComboBox.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>


#include "../Cleopatra/Transfer.h"
#include "../Cleopatra/InFileCreator.h"
#include "../Cleopatra/ExtractXSec.h"
#include "../Cleopatra/PlotTGraphTObjArray.h"
#include "../Armory/AutoFit.C"
#include "../Armory/Check_Simulation.C"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#ifdef __linux__
  #define OS_Type 1
#elif __APPLE__
  #define OS_Type 0
#endif

class MyMainFrame {
   RQ_OBJECT("MyMainFrame")
private:
   TGMainFrame *fMain;
  
   TGTextEdit * editor;
   
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
   
   TGComboBox    * extractFlag;
   
   
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

   withDWBA = new TGCheckButton(simFrame, "Sim with DWBA\n+DWBA.root\n+DWBA.Ex.txt");
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
   angMin->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 180);
   hframe001->AddFrame(angMin, new TGLayoutHints(kLHintsCenterX , 5, 5, 0, 0));
   
   angMax = new TGNumberEntry(hframe001, 60, 0, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   angMax->SetWidth(50);
   angMin->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 180);
   hframe001->AddFrame(angMax, new TGLayoutHints(kLHintsCenterX , 5, 5, 0, 0));
   
   angStep = new TGNumberEntry(hframe001, 1, 0, 0, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEAPositive);
   angStep->SetWidth(50);
   angMin->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 30);
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

   extractFlag = new TGComboBox(DWBAFrame, 100);
   extractFlag->SetWidth(130);
   extractFlag->SetHeight(30);
   
   extractFlag->AddEntry("Xsec.", 2);
   extractFlag->AddEntry("Ratio to Ruth.", 1);
   extractFlag->AddEntry("(n,n) Xsec.", 3);
   extractFlag->Select(2);
   DWBAFrame->AddFrame(extractFlag, new  TGLayoutHints(kLHintsLeft, 5,5,3,4));

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
  if ( ID == 3 ) fileName = "DWBA";
  if ( ID == 4 ) fileName = "../Armory/Check_Simulation_Config.txt";
  if ( ID == 5 ) fileName = "DWBA.in";
  if ( ID == 6 ) fileName = "DWBA.out";
  if ( ID == 7 ) fileName = "DWBA.Xsec.txt";
  
  //test if file exist
  if ( IsFileExist(fileName) ){
    
    fileLabel->SetText(fileName);

    editor->LoadFile(fileName);
   
    if( ID >= 5 ) {
       editor->SetReadOnly(true);
    }else{
      editor->SetReadOnly(false);
    }
    
    editor->ShowBottom();

    if( ID < 5){
      statusLabel->SetText(fileName + " opened.");
    }else{
      statusLabel->SetText(fileName + " opened. (READ ONLY)");
    }
  }else{
  
    statusLabel->SetText(fileName + " not exist.");    
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
      statusLabel->SetText("Creating DWBA.in.....");
      InFileCreator("DWBA", "DWBA.in", aMin, aMax, aStep);
      statusLabel->SetText("in-file created.");
    }
    
    bool isRunOK = true;
    if( isRun->GetState() && IsFileExist("DWBA.in") ) {
      //printf("run ptolemy...........\n");
      
      statusLabel->SetText("Running Ptolemy.....");
      int output = 1; 
      if( OS_Type == 1 ){
        output = system("../Cleopatra/ptolemy <DWBA.in> DWBA.out");
      }else{
        output = system("../Cleopatra/ptolemy_mac <DWBA.in> DWBA.out");
      }
      
      statusLabel->SetText("Check terminal, if no massage, Ptolemy run well.");
      
      printf("Ptolemy exist code : %d\n", output);
      if( output == 0 ) {
         isRunOK = true;
      }else{
         isRunOK = false;
         statusLabel->SetText("Ptolemy exist with problems.");
      }
    }
    
    if( isRunOK && isExtract->GetState() && IsFileExist("DWBA.out")){
       int ElasticFlag = 0; // 1 for ratio to Rutherford, 2 for total Xsec, 3 for (n,n) Xsec
       ElasticFlag = extractFlag->GetSelected();
       statusLabel->SetText("Extracting X-sec.....");
       ExtractXSec("DWBA.out", ElasticFlag);
       statusLabel->SetText("X-sec Extracted.");
    }
    
    if( isRunOK && isPlot->GetState() && IsFileExist("DWBA.root")){
       statusLabel->SetText("Plot X-sec.....");
       PlotTGraphTObjArray("DWBA.root");
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
       ptolemyRoot = "DWBA.root";
       excitationFile = "DWBA.Ex.txt";
    }
    statusLabel->SetText("Running simulation.......");
    
    Transfer( basicConfig, heliosDetGeoFile, excitationFile, ptolemyRoot, saveFileName,  filename);
    
    statusLabel->SetText("Plotting simulation.......");
    Check_Simulation();
    
    statusLabel->SetText("Plotted Simulation result");
  }
  if( ID == 2 ){
     Check_Simulation();
     statusLabel->SetText(" Run Simulation first.");
  }
  
  if( ID == 3 ){
     if( fileName != "" ){
        statusLabel->SetText(fileName + "   saved.");
      }else{
         statusLabel->SetText("cannot save HELP page.");
      }
  }
  
  if( ID == 4 ){
     fileName = "";
     statusLabel->SetText("Help Page.");
     editor->LoadBuffer("==================== For Simulation");
     editor->AddLine("");
     editor->AddLine("1) Make sure you check :");
     editor->AddLine("      a) reaction Config");
     editor->AddLine("      b) detector Geo.");
     editor->AddLine("      c) Ex List");
     editor->AddLine("");
     editor->AddLine("2) Not need to save file, fiel save when any button (except the Exit) is pressed.");
     editor->AddLine("");
     editor->AddLine("3) There is a checkbox for simulation with DWBA");
     editor->AddLine("      This requires the existance of DWBA.root and DWBA.Ex.txt");
     editor->AddLine("      These files can be generated by DWBA calculation.");
     editor->AddLine("      Please change the angMin = 0 and angMax = 180.");
     editor->AddLine("");
     editor->AddLine("4) After simulation, it will plot the result.");
     editor->AddLine("      To change the plotting, Click on the Config Simulation Plot.");
     editor->AddLine("");
     editor->AddLine("5) If the transfer.root is already here, simply Plot Simulation.");
     editor->AddLine("");
     editor->AddLine("========================= For DWBA ");
     editor->AddLine("");
     editor->AddLine("1) Only need to change the DWBA setting.");
     editor->AddLine("");
     editor->AddLine("2) The GUI offer a view on the infile and outfile.");
     editor->AddLine("");
     editor->AddLine("3) For elastics scattering, there is a checkbox for plotting the ratio to RutherFord.");
     editor->AddLine("");
     editor->AddLine("4) The flow of the DWBA calculation is like this:");
     editor->AddLine("      a) read the DWBA file and convert to DWBA.in");
     editor->AddLine("      b) run Ptolemy from DWBA.in, and the output is DWBA.out");
     editor->AddLine("      c) extract the cross section from the DWBA.out, and save :");
     editor->AddLine("              * DWBA.Xsec.txt");
     editor->AddLine("              * DWBA.Ex.txt");
     editor->AddLine("              * DWBA.root");
     editor->AddLine("      d) Plot the cross section from the DWBA.root.");
     editor->AddLine("");
     editor->AddLine("================ Tips for using the editor, both MAC or LINUX");
     editor->AddLine("");
     editor->AddLine("DO NOT PRESS SHIFT");
     editor->AddLine("DO NOT PRESS SHIFT");
     editor->AddLine("DO NOT PRESS SHIFT");
     editor->AddLine("DO NOT PRESS SHIFT");
     editor->AddLine("");
     editor->AddLine("Ctrl+U      | Delete current line.  ");
     editor->AddLine("Ctrl+C      | Copy            ");
     editor->AddLine("Ctrl+V      | Paste            ");
     editor->AddLine("=================================================== eof");

     TString osTypeStr;
     osTypeStr.Form("OS type is %s", (OS_Type == 0 ? "Mac" : "Linux"));

     editor->AddLine(osTypeStr);
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
