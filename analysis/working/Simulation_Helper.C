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
//#include "../Cleopatra/InFileCreator.h"
#include "../Armory/Check_Simulation.C"


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
   
public:
   MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
   virtual ~MyMainFrame();
   void Command(int);
   void OpenFile(int);
};
MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {
   // Create a main frame
   fMain = new TGMainFrame(p,w,h);
  
   TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,600,600 );
   fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2,2,2,2));

   TGVerticalFrame *hframe1 = new TGVerticalFrame(fMain,600,600 );
   hframe->AddFrame(hframe1);
   
   TGVerticalFrame *hframe2 = new TGVerticalFrame(fMain,600,600 );
   hframe->AddFrame(hframe2);

   fileName = "reactionConfig.txt";
   
   fileLabel = new TGLabel(hframe2, "");
   fileLabel->SetWidth(600);
   fileLabel->SetHeight(20);
   //fileLabel->SetTextJustify(kTextLeft);
   fileLabel->SetTextColor(kRed);
   fileLabel->ChangeOptions(kFixedSize | kSunkenFrame);
   fileLabel->SetText(fileName);
   hframe2->AddFrame(fileLabel, new TGLayoutHints(kLHintsLeft, 2,2,2,2));
   
  
   editor = new TGTextEdit(hframe2, 600, 600);
   editor->LoadFile(fileName);
   hframe2->AddFrame(editor);
   
   
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
   openRec->SetHeight(30);
   openRec->ChangeOptions( openRec->GetOptions() | kFixedSize );
   openRec->Connect("Clicked()","MyMainFrame",this, "OpenFile(=1)");
   simFrame->AddFrame(openRec,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *openDet = new TGTextButton(simFrame, "detector Geo.");
   openDet->SetWidth(150);
   openDet->SetHeight(30);
   openDet->ChangeOptions( openDet->GetOptions() | kFixedSize );
   openDet->Connect("Clicked()","MyMainFrame",this, "OpenFile(=0)");
   simFrame->AddFrame(openDet,new  TGLayoutHints(kLHintsRight, 5,5,3,4));
   
   TGTextButton *openEx = new TGTextButton(simFrame, "Ex list");
   openEx->SetWidth(150);
   openEx->SetHeight(30);
   openEx->ChangeOptions( openEx->GetOptions() | kFixedSize );
   openEx->Connect("Clicked()","MyMainFrame",this, "OpenFile(=2)");
   simFrame->AddFrame(openEx,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGTextButton *openSimChk = new TGTextButton(simFrame, "Check_Simulation.C");
   openSimChk->SetWidth(150);
   openSimChk->SetHeight(30);
   openSimChk->ChangeOptions( openSimChk->GetOptions() | kFixedSize );
   openSimChk->Connect("Clicked()","MyMainFrame",this, "OpenFile(=4)");
   simFrame->AddFrame(openSimChk,new  TGLayoutHints(kLHintsRight, 5,5,3,4));
   
   TGTextButton *Sim = new TGTextButton(simFrame,"Simulate");
   Sim->SetWidth(150);
   Sim->SetHeight(50);
   Sim->ChangeOptions( Sim->GetOptions() | kFixedSize );
   Sim->Connect("Clicked()","MyMainFrame",this,"Command(=1)");
   simFrame->AddFrame(Sim, new TGLayoutHints(kLHintsRight,5,5,3,4));

   TGTextButton *SimChk = new TGTextButton(simFrame,"Check Simulate");
   SimChk->SetWidth(150);
   SimChk->SetHeight(50);
   SimChk->ChangeOptions( SimChk->GetOptions() | kFixedSize );
   SimChk->Connect("Clicked()","MyMainFrame",this,"Command(=2)");
   simFrame->AddFrame(SimChk, new TGLayoutHints(kLHintsRight,5,5,3,4));

   //================= DWBA group
   TGGroupFrame * DWBAFrame = new TGGroupFrame(fMain, "DWBA calculation", kVerticalFrame);
   hframe1->AddFrame(DWBAFrame, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));

   TGTextButton *openDWBA = new TGTextButton(DWBAFrame, "DWBA setting");
   openDWBA->SetWidth(150);
   openDWBA->SetHeight(30);
   openDWBA->ChangeOptions( openDWBA->GetOptions() | kFixedSize );
   openDWBA->Connect("Clicked()","MyMainFrame",this, "OpenFile(=3)");
   DWBAFrame->AddFrame(openDWBA,new  TGLayoutHints(kLHintsRight, 5,5,3,4));

   TGCheckButton * isInFile = new TGCheckButton(DWBAFrame, "Create inFile");
   isInFile->SetWidth(100);
   isInFile->ChangeOptions(kFixedSize );
   isInFile->SetState(kButtonDown);
   DWBAFrame->AddFrame(isInFile, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));
   
   TGCheckButton * isRun = new TGCheckButton(DWBAFrame, "Run Ptolemy");
   isRun->SetWidth(100);
   isRun->ChangeOptions(kFixedSize );
   isRun->SetState(kButtonDown);
   DWBAFrame->AddFrame(isRun, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));
   
   TGCheckButton * isExtract = new TGCheckButton(DWBAFrame, "Extract Xsec");
   isExtract->SetWidth(100);
   isExtract->ChangeOptions(kFixedSize );
   isExtract->SetState(kButtonDown);
   DWBAFrame->AddFrame(isExtract, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));

   TGCheckButton * isPlot = new TGCheckButton(DWBAFrame, "Plot");
   isPlot->SetWidth(100);
   isPlot->ChangeOptions(kFixedSize );
   isPlot->SetState(kButtonDown);
   DWBAFrame->AddFrame(isPlot, new  TGLayoutHints(kLHintsCenterX, 5,5,3,4));

   TGTextButton *DWBA = new TGTextButton(DWBAFrame, "DWBA");
   DWBA->SetWidth(150);
   DWBA->SetHeight(50);
   DWBA->ChangeOptions( DWBA->GetOptions() | kFixedSize );
   DWBA->Connect("Clicked()","MyMainFrame",this,"Command(=0)");
   DWBAFrame->AddFrame(DWBA,new  TGLayoutHints(kLHintsRight, 5,5,3,4));


   TGTextButton *exit = new TGTextButton(hframe1,"Exit", "gApplication->Terminate(0)");
   exit->SetWidth(150);
   exit->SetHeight(50);
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

void MyMainFrame::OpenFile(int ID){
  
  editor->SaveFile(fileName);
  
  if ( ID == 0 ) fileName = "detectorGeo.txt";
  if ( ID == 1 ) fileName = "reactionConfig.txt";
  if ( ID == 2 ) fileName = "Ex.txt";
  if ( ID == 3 ) fileName = "example";
  if ( ID == 4 ) fileName = "../Armory/Check_Simulation.C";
  
  fileLabel->SetText(fileName);
  editor->LoadFile(fileName);
  
}

void MyMainFrame::Command(int ID) {

  editor->SaveFile(fileName);
  
  if( ID == 0 ){
    printf("not implemented.....\n");
    printf("in terminal, use ./Cleopatra.sh \n");
  }
  
  if( ID == 1 ){
    string       basicConfig = "reactionConfig.txt";
    string  heliosDetGeoFile = "detectorGeo.txt";
    string    excitationFile = "Ex.txt"; //when no file, only ground state
    TString      ptolemyRoot = "example.root"; // when no file, use isotropic distribution of thetaCM
    TString     saveFileName = "transfer.root";
    TString         filename = "reaction.dat"; //when no file, no output    
    Transfer( basicConfig, heliosDetGeoFile, excitationFile, ptolemyRoot, saveFileName,  filename);
    Check_Simulation(saveFileName);
  }
  if( ID == 2 ){
    Check_Simulation("transfer.root");
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
