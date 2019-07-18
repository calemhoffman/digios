{
   
   
   TFile *f1 = new TFile("Runs_037.root");
   TH1F *hEx_sum = (TH1F *) gDirectory->Get("hEx");
   hEx_sum->SetDirectory(0); 
   f->Close();
   delete f1;

   TFile *f2 = new TFile("Runs_049.root");
   TH1F *ht = (TH1F *) gDirectory->Get("hEx");
   ht->SetDirectory(0);
   hEx_sum->Add(ht);
   f1->Close();
   delete f2;
   
   
   
   hEx_sum->Draw();
}
