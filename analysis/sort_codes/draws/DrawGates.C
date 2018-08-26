{

  TCanvas *c1 = new TCanvas("c1","Applied Gates",1000,800);

  TH1F *hex1 = new TH1F("hex1","Excitation Energy 1;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex1g = new TH1F("hex1g","Ex Energy 1 Array-RF;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex1gg = new TH1F("hex1gg","Ex Energy 1 TAC;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex1ggg = new TH1F("hex1ggg","Ex Energy 1 RDT Energy;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex1G = new TH1F("hex1G","Ex Energy 1 All Gates;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);

 psd_tree->Draw("ex1>>hex1","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)","");
 psd_tree->Draw("ex1>>hex1g","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(ttt>-220&&ttt<280)","");
 psd_tree->Draw("ex1>>hex1gg","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(tac1>-5.5&&tac1<6.5)","");
 psd_tree->Draw("ex1>>hex1ggg","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(rdt[1]>5300&&rdt[1]<5850)","");
 psd_tree->Draw("ex1>>hex1G","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(ttt>-220&&ttt<280)&&(tac1>-5.5&&tac1<6.5)&&(rdt[1]>5300&&rdt[1]<5850)","");

 TH2F *heVx1 = new TH2F("heVx1","Energy VS X 1; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x1>>heVx1","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)","");
 
  TH2F *heVx1g = new TH2F("heVx1g","Energy VS X 1 Array-RF; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x1>>heVx1g","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(ttt>-220&&ttt<280)","");

 
  TH2F *heVx1gg = new TH2F("heVx1gg","Energy VS X 1 TAC; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x1>>heVx1gg","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(tac1>-5.5&&tac1<6.5)","");

 TH2F *heVx1ggg = new TH2F("heVx1ggg","Energy VS X 1 RDT Energy; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x1>>heVx1ggg","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(rdt[1]>5300&&rdt[1]<5850)","");

 TH2F *heVx1G = new TH2F("heVx1G","Energy VS X 1 All Gates; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x1>>heVx1G","(e[22]>100)&&(xf[22]>xncal)&&(x1<0.95)&&(ttt>-220&&ttt<280)&&(tac1>-5.5&&tac1<6.5)&&(rdt[1]>5300&&rdt[1]<5850)","");
 
  TH1F *hex2 = new TH1F("hex2","Excitation Energy 2;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex2g = new TH1F("hex2g","Ex Energy 2 Array-RF;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex2gg = new TH1F("hex2gg","Ex Energy 2 TAC;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex2ggg = new TH1F("hex2ggg","Ex Energy 2 RDT Energy;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);
 TH1F *hex2G = new TH1F("hex2G","Ex Energy 2 All Gates;Excitation Energy [MeV]; Counts / 25 keV",480,-1,11);

 psd_tree->Draw("ex2>>hex2","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)","");
 psd_tree->Draw("ex2>>hex2g","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(ttt>-220&&ttt<280)","");
 psd_tree->Draw("ex2>>hex2gg","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(tac1>-5.5&&tac1<6.5)","");
 psd_tree->Draw("ex2>>hex2ggg","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(rdt[1]>5300&&rdt[1]<5850)","");
 psd_tree->Draw("ex2>>hex2G","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(ttt>-220&&ttt<280)&&(tac1>-5.5&&tac1<6.5)&&(rdt[1]>5300&&rdt[1]<5850)","");

 TH2F *heVx2 = new TH2F("heVx2","Energy VS X 2; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x2>>heVx2","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)","");
 
  TH2F *heVx2g = new TH2F("heVx2g","Energy VS X 2 Array-RF; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x2>>heVx2g","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(ttt>-220&&ttt<280)","");

 
  TH2F *heVx2gg = new TH2F("heVx2gg","Energy VS X 2 TAC; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x2>>heVx2gg","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(tac1>-5.5&&tac1<6.5)","");

 TH2F *heVx2ggg = new TH2F("heVx2ggg","Energy VS X 2 RDT Energy; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x2>>heVx2ggg","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(rdt[1]>5300&&rdt[1]<5850)","");

 TH2F *heVx2G = new TH2F("heVx2G","Energy VS X 2 All Gates; X; Energy",500,0,1,500,0,3000);
 psd_tree->Draw("e[22]:x2>>heVx2G","(e[22]>100)&&(xf[22]<=xncal)&&(x2>0.05)&&(ttt>-220&&ttt<280)&&(tac1>-5.5&&tac1<6.5)&&(rdt[1]>5300&&rdt[1]<5850)","");


 c1->Clear();c1->Divide(3,5);
 c1->cd(1); hex2->Draw(); c1->cd(2); hex1->Draw(); c1->cd(3); heVx1->Draw(); heVx2->Draw("same");
 c1->cd(4); hex2g->Draw(); c1->cd(5); hex1g->Draw(); c1->cd(6); heVx1g->Draw(); heVx2g->Draw("same");
 c1->cd(7); hex2gg->Draw(); c1->cd(8); hex1gg->Draw(); c1->cd(9); heVx1gg->Draw(); heVx2gg->Draw("same");
 c1->cd(10); hex2ggg->Draw(); c1->cd(11); hex1ggg->Draw(); c1->cd(12); heVx1ggg->Draw(); heVx2ggg->Draw("same");
 c1->cd(13); hex2G->Draw(); c1->cd(14); hex1G->Draw(); c1->cd(15); heVx1G->Draw(); heVx2G->Draw("same");

}
