

TGraph * TrapezoidFilter(TGraph * trace){
   ///Trapezoid filter https://doi.org/10.1016/0168-9002(94)91652-7

   int baseLineEnd = 80;
   
   int riseTime = 20; //ch
   int flatTop = 20;
   float decayTime = 2000;
   
   TGraph *  trapezoid = new TGraph();
   trapezoid->Clear();
   
   ///find baseline;
   double baseline = 0;
   for( int i = 0; i < baseLineEnd; i++){
      baseline += trace->Eval(i);
   }
   baseline = baseline*1./baseLineEnd;
   
   int length = trace->GetN();
   
   double pn = 0.;
   double sn = 0.;
   for( int i = 0; i < length ; i++){
   
      double dlk = trace->Eval(i) - baseline;
      if( i - riseTime >= 0            ) dlk -= trace->Eval(i - riseTime)             - baseline;
      if( i - flatTop - riseTime >= 0  ) dlk -= trace->Eval(i - flatTop - riseTime)   - baseline;
      if( i - flatTop - 2*riseTime >= 0) dlk += trace->Eval(i - flatTop - 2*riseTime) - baseline;
      
      if( i == 0 ){
         pn = dlk;
         sn = pn + dlk*decayTime;
      }else{
         pn = pn + dlk;
         sn = sn + pn + dlk*decayTime;
      }    
      
      trapezoid->SetPoint(i, i, sn / decayTime / riseTime);
    
   }
   
   return trapezoid;
   
}

TGraph * g;
TGraph * t;

TH1F * haha0 = new TH1F("haha0", "IC-0 energy", 400, 0, 100);
TH1F * haha1 = new TH1F("haha1", "IC-1 energy", 400, 0, 100);


TH2F * haha01 = new TH2F("haha01", "IC  PID", 400, 0, 100, 400, 0, 100);



void test(){


   TFile * file = new TFile("../root_data/trace_run028.root");
   TTree * tree = (TTree *) file->Get("gen_tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 1500, 800, 800);
   cRead->Divide(1,2);
   for( int i = 1; i <= 3 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
   TClonesArray * arr = new TClonesArray("TGraph");
   tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("trace", &arr);

   
   vector<double> peak0;
   vector<double> peak1;
   //for( int ev = 34; ev < totnumEntry; ev++){
   for( int ev = 34; ev < 4000; ev++){
      tree->GetEntry(ev);
      
      peak0.clear();
      peak1.clear();
      
      if( ev%1000 == 0 ) printf("-------- ev = %d \n", ev);
      
      for( int j = 0; j < arr->GetEntriesFast() ; j++){

         g  = (TGraph*) arr->At(j);
         
         if( g->GetN() == 0 ) continue;
         
         TString idStr = g->GetTitle();
         idStr.Remove(0,3);
         int detID =  atoi(idStr.Data());
         
         t = TrapezoidFilter(g);
         
         //find peaks 
         //printf("-------------\n");
         int nPeak = 0;
         double threshold = 10;
         double yMax = 0;
         for( int p = 0; p < t->GetN(); p++){
            double y = t -> Eval(p);
            if( y < threshold ) {
               
               if( yMax > 0 ) {
                  //nPeak ++;
                  //printf(" %d , peak : %f \n", nPeak, yMax);
                  if( detID == 300 ) haha0->Fill(yMax);
                  if( detID == 301 ) haha1->Fill(yMax);
                  //if( detID == 300 ) peak0.push_back(yMax);
                  //if( detID == 301 ) peak1.push_back(yMax);
               }
               yMax = 0;
               continue;
            }
            
            if( y > yMax ) yMax = y;
            
         }
         
         
         //cRead->cd(1);
         //g->Draw("Al");
         //
         //cRead->cd(2);
         //t->Draw("Al");
         //
         //cRead->Update();
         //gSystem->ProcessEvents();
         //
         //gPad->WaitPrimitive();
      }
      
      //for( int k = 0; k < (int) peak0.size()
      
      
      
   }
   
   
   cRead->cd(1);
   haha0->Draw();
         
         
   cRead->cd(2);
   haha1->Draw();
         

}


