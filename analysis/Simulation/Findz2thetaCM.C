{   
/**///======================================================== initial input
   const char* rootfile="~/ANALYSIS/Simulation/transfer.root"; const char* treeName="tree";
   
   double zRange[3] = { 400, -500, -100};
   string excitationFile = "excitation_energies.txt";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,1};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }
   cScript->cd(1);

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= Analysis

   printf("############################################## excitation energies\n");
   vector<double> ExKnown;
   int numEx = 0;
   printf("----- loading excitation ExKnown levels.");
   ifstream file;
   file.open(excitationFile.c_str());
   string isotopeName;
   if( file.is_open() ){
      string exline;
      int i = 0;
      while( file >> exline){
         if( exline.substr(0,2) == "//" ) continue;
         if( i == 0 ) isotopeName = exline; 
         if ( i >= 1 ){
            ExKnown.push_back(atof(exline.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      numEx = ExKnown.size();

//      printf("========== %s\n", isotopeName.c_str());
//      for(int i = 0; i < numEx ; i++){
//         printf("%d, Ex: %6.2f MeV \n", i, ExKnown[i]);
//      }
   }else{
       printf("... fail\n");
       return;
   }

   //=============== plotting cos(thetaCM) vs z
   TH2F * k = new TH2F("k", "k", zRange[0], zRange[0], zRange[0], 400, 0.65, 1.0); 
   double c0[numEx], c1[numEx];
   for(int ExID = 0; ExID < numEx ; ExID ++){
      
      TString gate;
      gate.Form("loop == 1 && thetaCM > 10 &&  ExID == %d", ExID);
      tree->Draw("TMath::Cos(thetaCM*TMath::DegToRad()):z >> k", gate, "")   ;
      // fit pol1
      k->Fit("pol1", "q");
      
      c0[ExID] = pol1->GetParameter(0);
      c1[ExID] = pol1->GetParameter(1);
      
      printf("%d, Ex:%f -- c[0]: %9.6f, c[1]: %20.15f \n", ExID, ExKnown[ExID], c0[ExID], c1[ExID]); 
      
   }
   
   //============ Find the ExKnown vs c0, ExKnown vs c1;
   TGraph * g0 = new TGraph(numEx, &ExKnown[0], &c0[0] );
   g0->Draw("*ap");
   g0->Fit("pol2", "q");
   double e00 = pol2->GetParameter(0);
   double e01 = pol2->GetParameter(1);
   double e02 = pol2->GetParameter(2);
   printf("====  e00:%12.6f, e01:%12.9f, e02:%12.9f \n", e00, e01, e02);
   
   TF1 * e2c0 = new TF1("e2c0", "[0] + [1]*x + [2]*x*x", -10, 20);
   e2c0->SetParameter(0, e00);
   e2c0->SetParameter(1, e01);
   e2c0->SetParameter(2, e02);

   TGraph * g1 = new TGraph(numEx, &ExKnown[0], &c1[0] );
   g1->Draw("*ap");
   g1->Fit("pol2", "q");
   double e10 = pol2->GetParameter(0);
   double e11 = pol2->GetParameter(1);
   double e12 = pol2->GetParameter(2);
   printf("====  e10:%12.6f, e11:%12.9f, e12:%12.9f \n", e10, e11, e12);
   
   TF1 * e2c1 = new TF1("e2c1", "[0] + [1]*x + [2]*x*x", -10, 20);
   e2c1->SetParameter(0, e10);
   e2c1->SetParameter(1, e11);
   e2c1->SetParameter(2, e12);
   
   
   //============ test
   tree->Draw("TMath::Cos(thetaCM*TMath::DegToRad()):z >> ezt", "loop==1 && thetaCM > 10 && 5 > detID && detID > 0 && hit == 1");
   
   // for Ex = 0;
   double ex = ExKnown[2];
   TF1 * line = new TF1("line", "[0] + [1]*x", zRange[1], zRange[2]);
   double p0 = e2c0->Eval(ex);
   line->SetParameter(0, p0);
   double p1 = e2c1->Eval(ex);
   line->SetParameter(1, p1);
   
   line->Draw("same");
}

