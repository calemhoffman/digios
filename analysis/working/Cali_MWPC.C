{

   TChain * chain = new TChain("gen_tree");
   chain->Add("../root_data/gen_run03[0-9].root");
   chain->Add("../root_data/gen_run04[1-5].root");


   ///--------- Calibraing the slope
   TCanvas * c1 = new TCanvas("c1", "MWPC slope calibration", 800, 1600);
   c1->Divide(2,4);

   TH2F * h[4][2];
   
   double slope[4][2];
   for( int i = 0; i < 4; i ++){
      for( int j = 0; j < 2; j++){
         
         TString title = (j ==  0 ? "x": "y");
         
         h[i][j] = new TH2F(Form("h%d%d", i,j ), Form("MWPC-%s; %s-1;%s-2)", title.Data(), title.Data(), title.Data()), 500, 0, 7000, 500, 0, 7000);
         
         int id1 = 10*(i+1) + 2*j;
         int id2 = 10*(i+1) + 2*j + 1;
         
         TString express = Form("ezero[%d]:ezero[%d] >> h%d%d", id2, id1, i,j );

         c1->cd(2*i+j+1);
         chain->Draw(express, "", "colz");
         
         c1->Update();
         c1->Draw();
         gSystem->ProcessEvents();
         
         
         TProfile * temp = h[i][j]->ProfileX();
         temp->SetLineColor(kYellow);
         temp->Draw("same");
         
         TF1 * fit = new TF1("fit", "pol1");
         fit->SetLineColor(kRed);
         
         temp->Fit("fit", "Rq", "", 1000, 5000);
         
         slope[i][j] = fit->GetParameter(1);
         
         //printf("%d-%d | slope = %f \n", i, j, slope[i][j]);
                  
         delete fit;
      }  
   }
   
   printf("=================================\n");
   for( int i = 0; i < 4; i ++){
      printf(" { %f, %f }, \n", slope[i][0], slope[i][1]);
   }


   printf("#####################################################\n");

                                 
   TCanvas * c2 = new TCanvas("c2", "MWPC sum Calibration", 800, 1600);
   c2->Divide(2,4);

   TH1F * g[4][2];
   
   double a[4][2];
   
   for( int i = 0; i < 4; i ++){
      for( int j = 0; j < 2; j++){
         TString title = (j ==  0 ? "x": "y");
         g[i][j] = new TH1F(Form("g%d%d", i,j ), Form("MWPC %s1+%s2; %s1+%s2; count/ 3 ch ", title.Data(), title.Data(), title.Data(), title.Data()), 500, 5500, 7000);
         
         int id1 = 10*(i+1) + 2*j;
         int id2 = 10*(i+1) + 2*j + 1;
         
         TString express = Form("ezero[%d] + %f*ezero[%d] >> g%d%d", id2, abs(slope[i][j]), id1, i,j );
         
         c2->cd(2*i+j+1);
         chain->Draw(express);
         
         c2->Update();
         c2->Draw();
         gSystem->ProcessEvents();
         
         
         TF1 * fit = new TF1("fit", "gaus");
         fit->SetNpx(1000);
         fit->SetLineColor(kRed);
         
         g[i][j]->Fit("fit", "q");
         
         a[i][j] = fit->GetParameter(1);
         
         //printf("%d-%d | mean = %f \n", i, j, a[i][j]);
         
         
         c2->Update();
         c2->Draw();
         gSystem->ProcessEvents();
         
         delete fit;
      }
   }
   
   printf("=================================\n");
   for( int i = 0; i < 4; i ++){
      printf(" { %f, %f },\n", a[i][0], a[i][1]);
   }
   
   
   


}
