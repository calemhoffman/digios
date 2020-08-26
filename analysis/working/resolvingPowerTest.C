{

   TCanvas * c1 = new TCanvas("c1", "c1", 100, 1500, 600, 400);

   TH1F* h1 = new TH1F("h1", "h1", 150, -5, 10);
   
   //TF1 * g1 = new TF1("g1", "gaus");
   //g1->SetParameter(0, 10);
   //g1->SetParameter(1, 1);
   //g1->SetParameter(2, 2);
   //h1->FillRandom("g1", 1000);

   for( int i = 1; i < 700; i++){
      h1->Fill(gRandom->Gaus(0.0, 0.3));
      h1->Fill(gRandom->Gaus(7.0, 0.3));
   }

   for( int i = 1; i < 1000; i++){
      h1->Fill(gRandom->Gaus(1.0, 0.3));
      h1->Fill(gRandom->Gaus(4.0, 0.3));
   }
   
   for( int i = 1; i < 500; i++){
      h1->Fill(gRandom->Gaus(2.3, 0.3));
      h1->Fill(gRandom->Gaus(5.7, 0.3));
   }

  
   TH1F* h2 = new TH1F("h2", "h2", 150,-5, 10);
   h2->FillRandom("pol0", 2000);
   h2->SetLineColor(2);

   TH1F* h0 = (TH1F*)h1->Clone();
   h0->SetLineColor(4);
   
   h1->Add(h2);
   
   h1->Draw();
   h2->Draw("same");
   
   h0->Draw("same");
   
   

}
