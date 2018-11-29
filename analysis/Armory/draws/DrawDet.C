{
  TCanvas *cc = new TCanvas("cc","Det 19");
  //Det 19
   psd_tree->SetAlias("xncal","xn[19]*0.975596");
   psd_tree->SetAlias("ecalx","e[19]*1.03803");
   psd_tree->SetAlias("ecal","e[19]*1.0+0.0");
   psd_tree->SetAlias("x1","xf[19]/ecalx");
   psd_tree->SetAlias("x2","1-xncal/ecalx");
   psd_tree->SetAlias("x","(xf[19]-xn[19])/(xf[19]+xn[19])");
   psd_tree->SetAlias("xe2","(ecal-(x2*307.303+748.749))");//3.9 MeV
   //psd_tree->SetAlias("xe2","(ecal-(x2*x2*x2*x2*10688.9+x2*x2*x2*(-10578.7)+x2*x2*3285.72-x2*124.456+1062.1))");//2.9 MeV
   //   psd_tree->SetAlias("ex2","(xe2*(-0.00388)+2.92593)"); //2.9
   psd_tree->SetAlias("ex2","(xe2*(-0.003962)+4.05575)"); //3.9
   
   psd_tree->SetAlias("xe1","(ecal-(x1*199.836+796.372))");//lin 3.9?
   //psd_tree->SetAlias("xe1","(ecal-(x1*x1*x1*x1*(-2411.29)+x1*x1*x1*(6662.4)+x1*x1*(-6589.51)+x1*2937.86+398.757))");//3.9?
   //   psd_tree->SetAlias("ex2","(xe2*(-0.00388)+2.92593)"); //
   psd_tree->SetAlias("ex1","(xe1*(-0.0039471)+3.936)"); //3.9?
   
   psd_tree->SetAlias("tac1","(e_t[19]-rdt_t[1])");
   psd_tree->SetAlias("t","(t5[0]-(x*x*(542.719)+x*(-2.484)+2930.79))");

  
  cc->Clear();
  cc->Divide(2,2);
  cc->cd(2);
  psd_tree->Draw("ecal:x1>>heVx1(500,0,1,500,0,3000)","xf[19]>xncal","");
  cc->cd(1);
  psd_tree->Draw("ecal:x2>>heVx2(500,0,1,500,0,3000)","xf[19]<=xncal","");
  cc->cd(4);
  psd_tree->Draw("ex1>>hxe1(400,-1,7)","xf[19]>xncal&&ecal>100","");
  cc->cd(3);
  psd_tree->Draw("ex2>>hxe2(400,-1,7)","xf[19]<=xncal&&ecal>100","");

  TCanvas *cc20 = new TCanvas("cc20","Det 20");
  cc20->Clear();cc20->SetTitle("Det 20");

  psd_tree->SetAlias("xncal","xn[20]*0.945164");
   psd_tree->SetAlias("ecalx","e[20]*0.975706");
   psd_tree->SetAlias("ecal","e[20]*1.0+0.0");
   psd_tree->SetAlias("x1","xf[20]/ecalx");
   psd_tree->SetAlias("x2","1-xncal/ecalx");
   psd_tree->SetAlias("x","(xf[20]-xn[20])/(xf[20]+xn[20])");
   psd_tree->SetAlias("xe2","(ecal-(x2*196.431+947.905))");//
   psd_tree->SetAlias("ex2","(xe2*(-0.00395492)+4.377866)"); // 
   psd_tree->SetAlias("xe1","(ecal-(x1*316.642+371.746))");//
   psd_tree->SetAlias("ex1","(xe1*(-0.00390155)+6.2854)"); //
   psd_tree->SetAlias("tac1","(e_t[20]-rdt_t[1])");
   psd_tree->SetAlias("t","(t5[0]-(x*x*(0)+x*(0)+0))");

  
  cc20->Divide(2,2);
  cc20->cd(2);
  psd_tree->Draw("ecal:x1>>heVx1(500,0,1,500,0,3000)","xf[20]>xncal","");
  cc20->cd(1);
  psd_tree->Draw("ecal:x2>>heVx2(500,0,1,500,0,3000)","xf[20]<=xncal","");
  cc20->cd(4);
  psd_tree->Draw("ex1>>hxe1(400,-1,7)","xf[20]>xncal&&ecal>100","");
  cc20->cd(3);
  psd_tree->Draw("ex2>>hxe2(400,-1,7)","xf[20]<=xncal&&ecal>100","");



  

   TCanvas *cc21 = new TCanvas("cc21","Det 21");
  cc21->Clear();cc21->SetTitle("Det 21");
 psd_tree->SetAlias("xncal","xn[21]*0.987393");
   psd_tree->SetAlias("ecalx","e[21]*1.11636");
   psd_tree->SetAlias("ecal","e[21]*1.0+0.0");
   psd_tree->SetAlias("x1","xf[21]/ecalx");
   psd_tree->SetAlias("x2","1-xncal/ecalx");
   psd_tree->SetAlias("x","(xf[21]-xn[21])/(xf[21]+xn[21])");

   psd_tree->SetAlias("xe1","(ecal-(x1*223.942+355.609))");//
   psd_tree->SetAlias("ex1","(xe1*(-0.00425644)+7.42905)"); //
   
   psd_tree->SetAlias("xe2","(ecal-(x2*272.162+445.235))");//
   psd_tree->SetAlias("ex2","(xe2*(-0.00424458)+7.04477)"); //
   
   psd_tree->SetAlias("tac1","(e_t[21]-rdt_t[1])");
   psd_tree->SetAlias("t","(t5[0]-(x*x*(0)+x*(0)+0))");
  
  cc21->Divide(2,2);
  cc21->cd(2);
  psd_tree->Draw("ecal:x1>>heVx1(500,0,1,500,0,3000)","xf[21]>xncal","");
  cc21->cd(1);
  psd_tree->Draw("ecal:x2>>heVx2(500,0,1,500,0,3000)","xf[21]<=xncal","");
  cc21->cd(4);
  psd_tree->Draw("ex1>>hxe1(500,-1,9)","xf[21]>xncal&&ecal>100","");
  cc21->cd(3);
  psd_tree->Draw("ex2>>hxe2(500,-1,9)","xf[21]<=xncal&&ecal>100","");

     TCanvas *cc22 = new TCanvas("cc22","Det 22");
  cc22->Clear();cc22->SetTitle("Det 22");

 psd_tree->SetAlias("xncal","xn[22]*1.0057");
   psd_tree->SetAlias("ecalx","e[22]*0.865252");
   psd_tree->SetAlias("ecal","e[22]*1.0+0.0");
   psd_tree->SetAlias("x1","xf[22]/ecalx");
   psd_tree->SetAlias("x2","1-xncal/ecalx");
   psd_tree->SetAlias("x","(xf[22]-xn[22])/(xf[22]+xn[22])");

   psd_tree->SetAlias("xe1","(ecal-(x1*x1*x1*x1*(-2421.59)+x1*x1*x1*8121.18+x1*x1*(-9878.51)+x1*5386.25-257.44))");//
   psd_tree->SetAlias("ex1","(xe1*(-0.0035193)+7.25776)"); //

   psd_tree->SetAlias("xe2","(ecal-(x2*x2*x2*x2*(-4202.25)+x2*x2*x2*4619.85+x2*x2*(-1754.1)+x2*484.873+722.953))");//
   psd_tree->SetAlias("ex2","(xe2*(-0.00354301)+7.29875)"); //
   
   psd_tree->SetAlias("tac1","(e_t[21]-rdt_t[1])");
   psd_tree->SetAlias("t","(t5[0]-(x*x*(0)+x*(0)+0))");
  
  cc22->Divide(2,2);
  cc22->cd(2);
  psd_tree->Draw("ecal:x1>>heVx1(500,0,1,500,0,3000)","xf[22]>xncal","");
  cc22->cd(1);
  psd_tree->Draw("ecal:x2>>heVx2(500,0,1,500,0,3000)","xf[22]<=xncal","");
  cc22->cd(4);
  psd_tree->Draw("ex1>>hxe1(600,-1,11)","xf[22]>xncal&&ecal>100","");
  cc22->cd(3);
  psd_tree->Draw("ex2>>hxe2(600,-1,11)","xf[22]<=xncal&&ecal>100","");
}
