{ 


//gROOT->ProcessLine(".x t0a.C");
//gROOT->ProcessLine(".x t1a.C");
//gROOT->ProcessLine(".x t2a.C");
//gROOT->ProcessLine(".x t3a.C");
//gROOT->ProcessLine(".x t4a.C");
//gROOT->ProcessLine(".x t5a.C");
//
//gROOT->ProcessLine(".x t6a.C");
//gROOT->ProcessLine(".x t7a.C");
//gROOT->ProcessLine(".x t8a.C");
//gROOT->ProcessLine(".x t9a.C");
//gROOT->ProcessLine(".x t10a.C");
//gROOT->ProcessLine(".x t11a.C");

//gROOT->ProcessLine(".x t12a.C");
//gROOT->ProcessLine(".x t13a.C");
//gROOT->ProcessLine(".x t14a.C");
//gROOT->ProcessLine(".x t15a.C");
//gROOT->ProcessLine(".x t16a.C");
//gROOT->ProcessLine(".x t17a.C");
//
//gROOT->ProcessLine(".x t18a.C");
//gROOT->ProcessLine(".x t19a.C");
//gROOT->ProcessLine(".x t20a.C");
//gROOT->ProcessLine(".x t21a.C");
//gROOT->ProcessLine(".x t22a.C");
//gROOT->ProcessLine(".x t23a.C");

gROOT->ProcessLine(".x cut0.C");
gROOT->ProcessLine(".x cut1.C");
gROOT->ProcessLine(".x cut2.C");
gROOT->ProcessLine(".x cut3.C");
gROOT->ProcessLine(".x cut4.C");
gROOT->ProcessLine(".x cut5.C");
//
gROOT->ProcessLine(".x cut6.C");
gROOT->ProcessLine(".x cut7.C");
gROOT->ProcessLine(".x cut8.C");
gROOT->ProcessLine(".x cut9.C");
////gROOT->ProcessLine(".x cut10.C");
////gROOT->ProcessLine(".x cut11.C");
//
gROOT->ProcessLine(".x cut12.C");
gROOT->ProcessLine(".x cut13.C");
gROOT->ProcessLine(".x cut14.C");
gROOT->ProcessLine(".x cut15.C");
gROOT->ProcessLine(".x cut16.C");
gROOT->ProcessLine(".x cut17.C");
//
gROOT->ProcessLine(".x cut18.C");
gROOT->ProcessLine(".x cut19.C");
gROOT->ProcessLine(".x cut20.C");
gROOT->ProcessLine(".x cut21.C");
gROOT->ProcessLine(".x cut22.C");
gROOT->ProcessLine(".x cut23.C");
//
//gROOT->ProcessLine(".x rec1.C");
//gROOT->ProcessLine(".x rec2.C");
//gROOT->ProcessLine(".x rec3.C");
//gROOT->ProcessLine(".x rec4.C");
//
//gROOT->ProcessLine(".x rec1a.C");
//gROOT->ProcessLine(".x rec2a.C");
//

gROOT->ProcessLine(".x rec1C.C");
gROOT->ProcessLine(".x rec2C.C");
gROOT->ProcessLine(".x rec3C.C");
gROOT->ProcessLine(".x rec4C.C");
/*
gROOT->ProcessLine(".x rec1b.C");
gROOT->ProcessLine(".x rec2b.C");
gROOT->ProcessLine(".x rec3b.C");
gROOT->ProcessLine(".x rec4b.C");
*/
//

//
//gROOT->ProcessLine(".x rec1s.C");
//gROOT->ProcessLine(".x rec2s.C");
//gROOT->ProcessLine(".x rec3s.C");
//gROOT->ProcessLine(".x rec4s.C");
//
//gROOT->ProcessLine(".x rec1pa.C");
//gROOT->ProcessLine(".x rec2pa.C");
//gROOT->ProcessLine(".x rec3pa.C");
//gROOT->ProcessLine(".x rec4pa.C");

//gROOT->ProcessLine(".x r01all.C");
//gROOT->ProcessLine(".x r23all.C");
//gROOT->ProcessLine(".x r45all.C");
//gROOT->ProcessLine(".x r67all.C");

/*
gStyle->SetOptStat("i"); 
c1=new TCanvas();
c1->SetWindowSize(1400,800);
c1->Divide(6,4);
c1->cd(1);tree->Draw("coinTime[0]:x[0]>>tx0(200,-2,2,200,-400,400)","x[0]>-0.95 && x[0]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(2);tree->Draw("coinTime[1]:x[1]>>tx1(200,-2,2,200,-400,400)","x[1]>-0.95 && x[1]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(3);tree->Draw("coinTime[2]:x[2]>>tx2(200,-2,2,200,-400,400)","x[2]>-0.95 && x[2]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(4);tree->Draw("coinTime[3]:x[3]>>tx3(200,-2,2,200,-400,400)","x[3]>-0.95 && x[3]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(5);tree->Draw("coinTime[4]:x[4]>>tx4(200,-2,2,200,-400,400)","x[4]>-0.95 && x[4]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(6);tree->Draw("coinTime[5]:x[5]>>tx5(200,-2,2,200,-400,400)","x[5]>-0.95 && x[5]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");

c1->cd(7);tree->Draw("coinTime[6]:x[6]>>tx6(200,-2,2,200,-400,400)","x[6]>-0.95 && x[6]<0.95 &&  (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(8);tree->Draw("coinTime[7]:x[7]>>tx7(200,-2,2,200,-400,400)","x[7]>-0.95 && x[7]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(9);tree->Draw("coinTime[8]:x[8]>>tx8(200,-2,2,200,-400,400)","x[8]>-0.95 && x[8]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(10);tree->Draw("coinTime[9]:x[9]>>tx9(200,-2,2,200,-400,400)","x[9]>-0.95 && x[9]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(11);tree->Draw("coinTime[10]:x[10]>>tx10(200,-2,2,200,-400,400)","x[10]>-0.95 && x[10]<0.95","");
c1->cd(12);tree->Draw("coinTime[11]:x[11]>>tx11(200,-2,2,200,-400,400)","x[11]>-0.95 && x[11]<0.95","");

c1->cd(13);tree->Draw("coinTime[12]:x[12]>>tx12(200,-2,2,200,-400,400)","x[12]>-0.95 && x[12]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(14);tree->Draw("coinTime[13]:x[13]>>tx13(200,-2,2,200,-400,400)","x[13]>-0.95 && x[13]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(15);tree->Draw("coinTime[14]:x[14]>>tx14(200,-2,2,200,-400,400)","x[14]>-0.95 && x[14]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(16);tree->Draw("coinTime[15]:x[15]>>tx15(200,-2,2,200,-400,400)","x[15]>-0.95 && x[15]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(17);tree->Draw("coinTime[16]:x[16]>>tx16(200,-2,2,200,-400,400)","x[16]>-0.95 && x[16]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(18);tree->Draw("coinTime[17]:x[17]>>tx17(200,-2,2,200,-400,400)","x[17]>-0.95 && x[17]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");

c1->cd(19);tree->Draw("coinTime[18]:x[18]>>tx18(200,-2,2,200,-400,400)","x[18]>-0.95 && x[18]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(20);tree->Draw("coinTime[19]:x[19]>>tx19(200,-2,2,200,-400,400)","x[19]>-0.95 && x[19]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(21);tree->Draw("coinTime[20]:x[20]>>tx20(200,-2,2,200,-400,400)","x[20]>-0.95 && x[20]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(22);tree->Draw("coinTime[21]:x[21]>>tx21(200,-2,2,200,-400,400)","x[21]>-0.95 && x[21]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(23);tree->Draw("coinTime[22]:x[22]>>tx22(200,-2,2,200,-400,400)","x[22]>-0.95 && x[22]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
c1->cd(24);tree->Draw("coinTime[23]:x[23]>>tx23(200,-2,2,200,-400,400)","x[23]>-0.95 && x[23]<0.95 && (rec1b||rec2b||rec3b||rec4b)","");
*/

/*
c1 = new TCanvas();
c1->SetWindowSize(1400,800);
c1->Divide(2,2);
c1->cd(1);tree->Draw("trdt[1]:trdt[0]>>h1(1000,0,5000,1000,0,7000)","run>15");
c1->cd(2);tree->Draw("trdt[3]:trdt[2]>>h1(1000,0,5000,1000,0,7000)","run>15");
c1->cd(3);tree->Draw("trdt[5]:trdt[4]>>h1(1000,0,5000,1000,0,7000)","run>15");
c1->cd(4);tree->Draw("trdt[7]:trdt[6]>>h1(1000,0,5000,1000,0,7000)","run>15");
*/
/*c1->cd(1);tree->Draw("trdt[1]:trdt[0]>>h1(1000,0,5000,1000,0,7000)"," (cut0|| cut1||cut2||cut3||cut4||cut5||cut6||cut7||cut8||cut9||cut12||cut13||cut14||cut15||cut16||cut17||cut18||cut19||cut20||cut21||cut22||cut23)");
c1->cd(2);tree->Draw("trdt[3]:trdt[2]>>h2(1000,0,5000,1000,0,7000)"," (cut0|| cut1||cut2||cut3||cut4||cut5||cut6||cut7||cut8||cut9||cut12||cut13||cut14||cut15||cut16||cut17||cut18||cut19||cut20||cut21||cut22||cut23)");
c1->cd(3);tree->Draw("trdt[5]:trdt[4]>>h3(1000,0,5000,1000,0,7000)"," (cut0|| cut1||cut2||cut3||cut4||cut5||cut6||cut7||cut8||cut9||cut12||cut13||cut14||cut15||cut16||cut17||cut18||cut19||cut20||cut21||cut22||cut23)");
c1->cd(4);tree->Draw("trdt[7]:trdt[6]>>h4(1000,0,5000,1000,0,7000)"," (cut0|| cut1||cut2||cut3||cut4||cut5||cut6||cut7||cut8||cut9||cut12||cut13||cut14||cut15||cut16||cut17||cut18||cut19||cut20||cut21||cut22||cut23)");
*/


gStyle->SetOptStat("i"); 
c1=new TCanvas();
c1->SetWindowSize(1400,800);
c1->Divide(6,4);
c1->cd(1);tree->Draw("Ex>>Ex0(150,-2,12)","detID==0 && x[0]>-0.95 && x[0]<0.95 && cut0 && (rec1C||rec2C||rec3C||rec4C) && e[0]>3 && thetaCM[0]>20","");
c1->cd(2);tree->Draw("Ex>>Ex1(150,-2,12)","detID==1 && x[1]>-0.95 && x[1]<0.95 && cut1 && (rec1C||rec2C||rec3C||rec4C) && e[1]>3 && thetaCM[1]>20","");
c1->cd(3);tree->Draw("Ex>>Ex2(150,-2,12)","detID==2 && x[2]>-0.95 && x[2]<0.95 && cut2 && (rec1C||rec2C||rec3C||rec4C) && e[2]>3 && thetaCM[2]>20","");
c1->cd(4);tree->Draw("Ex>>Ex3(150,-2,12)","detID==3 && x[3]>-0.95 && x[3]<0.95 && cut3 && (rec1C||rec2C||rec3C||rec4C) && e[3]>3 && thetaCM[3]>20","");
c1->cd(5);tree->Draw("Ex>>Ex4(150,-2,12)","detID==4 && x[4]>-0.95 && x[4]<0.95 && cut4 && (rec1C||rec2C||rec3C||rec4C) && e[4]>3 && thetaCM[4]>20","");
c1->cd(6);tree->Draw("Ex>>Ex5(150,-2,12)","detID==5 && x[5]>-0.95 && x[5]<0.95 && cut5 && (rec1C||rec2C||rec3C||rec4C) && e[5]>3 && thetaCM[5]>20","");

c1->cd(7);tree->Draw("Ex>>Ex6(150,-2,12)","detID==6 && x[6]>-0.95 && x[6]<0.95 && cut6 && (rec1C||rec2C||rec3C||rec4C) && e[6]>3 && thetaCM[6]>20","");
c1->cd(8);tree->Draw("Ex>>Ex7(150,-2,12)","detID==7 && x[7]>-0.95 && x[7]<0.95 && cut7 && (rec1C||rec2C||rec3C||rec4C) && e[7]>3 && thetaCM[7]>20","");
c1->cd(9);tree->Draw("Ex>>Ex8(150,-2,12)","detID==8 && x[8]>-0.95 && x[8]<0.95 && cut8 && (rec1C||rec2C||rec3C||rec4C) && e[8]>3 && thetaCM[8]>20","");
c1->cd(10);tree->Draw("Ex>>Ex9(150,-2,12)","detID==9 && x[9]>-0.95 && x[9]<0.95 && cut9 && (rec1C||rec2C||rec3C||rec4C) && e[9]>3 && thetaCM[9]>20","");
c1->cd(11);tree->Draw("Ex>>Ex10(150,-2,12)","detID==10 && x[10]>-0.95 && x[10]<0.95","");
c1->cd(12);tree->Draw("Ex>>Ex11(150,-2,12)","detID==11 && x[11]>-0.95 && x[11]<0.950","");

c1->cd(13);tree->Draw("Ex>>Ex12(150,-2,12)","detID==12 && x[12]>-0.95 && x[12]<0.95 && cut12 && (rec1C||rec2C||rec3C||rec4C) && e[12]>3 && thetaCM[12]>20","");
c1->cd(14);tree->Draw("Ex>>Ex13(150,-2,12)","detID==13 && x[13]>-0.95 && x[13]<0.95 && cut13 && (rec1C||rec2C||rec3C||rec4C) && e[13]>3 && thetaCM[13]>20","");
c1->cd(15);tree->Draw("Ex>>Ex14(150,-2,12)","detID==14 && x[14]>-0.95 && x[14]<0.95 && cut14 && (rec1C||rec2C||rec3C||rec4C) && e[14]>3 && thetaCM[14]>20","");
c1->cd(16);tree->Draw("Ex>>Ex15(150,-2,12)","detID==15 && x[15]>-0.95 && x[15]<0.95 && cut15 && (rec1C||rec2C||rec3C||rec4C) && e[15]>3 && thetaCM[15]>20","");
c1->cd(17);tree->Draw("Ex>>Ex16(150,-2,12)","detID==16 && x[16]>-0.95 && x[16]<0.95 && cut16 && (rec1C||rec2C||rec3C||rec4C) && e[16]>3 && thetaCM[16]>20","");
c1->cd(18);tree->Draw("Ex>>Ex17(150,-2,12)","detID==17 && x[17]>-0.95 && x[17]<0.95 && cut17 && (rec1C||rec2C||rec3C||rec4C) && e[17]>3 && thetaCM[17]>20","");

c1->cd(19);tree->Draw("Ex>>Ex18(150,-2,12)","detID==18 && x[18]>-0.95 && x[18]<0.95 && cut18 && (rec1C||rec2C||rec3C||rec4C) && e[18]>3 && thetaCM[18]>20","");
c1->cd(20);tree->Draw("Ex>>Ex19(150,-2,12)","detID==19 && x[19]>-0.95 && x[19]<0.95 && cut19 && (rec1C||rec2C||rec3C||rec4C) && e[19]>3 && thetaCM[19]>20","");
c1->cd(21);tree->Draw("Ex>>Ex20(150,-2,12)","detID==20 && x[20]>-0.95 && x[20]<0.95 && cut20 && (rec1C||rec2C||rec3C||rec4C) && e[20]>3 && thetaCM[20]>20","");
c1->cd(22);tree->Draw("Ex>>Ex21(150,-2,12)","detID==21 && x[21]>-0.95 && x[21]<0.95 && cut21 && (rec1C||rec2C||rec3C||rec4C) && e[21]>3 && thetaCM[21]>20","");
c1->cd(23);tree->Draw("Ex>>Ex22(150,-2,12)","detID==22 && x[22]>-0.95 && x[22]<0.95 && cut22 && (rec1C||rec2C||rec3C||rec4C) && e[22]>3 && thetaCM[22]>20","");
c1->cd(24);tree->Draw("Ex>>Ex23(150,-2,12)","detID==23 && x[23]>-0.95 && x[23]<0.95 && cut23 && (rec1C||rec2C||rec3C||rec4C) && e[23]>3 && thetaCM[23]>20","");

Ex0->Add(Ex1);
Ex0->Add(Ex2);
Ex0->Add(Ex3);
Ex0->Add(Ex4);
Ex0->Add(Ex5);

Ex0->Add(Ex6);
Ex0->Add(Ex7);
Ex0->Add(Ex9);

Ex0->Add(Ex12);
Ex0->Add(Ex13);
Ex0->Add(Ex14);
Ex0->Add(Ex15);
Ex0->Add(Ex16);
Ex0->Add(Ex17);

Ex0->Add(Ex18);
Ex0->Add(Ex19);
Ex0->Add(Ex20);
Ex0->Add(Ex21);
Ex0->Add(Ex22);
Ex0->Add(Ex23);


//gStyle->SetOptStat("i"); 
//c1=new TCanvas();
//c1->SetWindowSize(1400,800);
//c1->Divide(6,4);
//c1->cd(1);tree->Draw("Ex>>Exs0(160,-2,14)","detID==0 && x[0]>-0.95 && x[0]<0.95 && t0a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[0]>2","");
//c1->cd(2);tree->Draw("Ex>>Exs1(160,-2,14)","detID==1 && x[1]>-0.95 && x[1]<0.95 && t1a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[1]>2","");
//c1->cd(3);tree->Draw("Ex>>Exs2(160,-2,14)","detID==2 && x[2]>-0.95 && x[2]<0.95 && t2a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[2]>2","");
//c1->cd(4);tree->Draw("Ex>>Exs3(160,-2,14)","detID==3 && x[3]>-0.95 && x[3]<0.95 && t3a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[3]>2","");
//c1->cd(5);tree->Draw("Ex>>Exs4(160,-2,14)","detID==4 && x[4]>-0.95 && x[4]<0.95 && t4a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[4]>2","");
//c1->cd(6);tree->Draw("Ex>>Exs5(160,-2,14)","detID==5 && x[5]>-0.95 && x[5]<0.95 && t5a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[5]>2","");
//
//c1->cd(7);tree->Draw("Ex>>Exs6(160,-2,14)","detID==6 && x[6]>-0.95 && x[6]<0.95 && t6a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[6]>2","");
//c1->cd(8);tree->Draw("Ex>>Exs7(160,-2,14)","detID==7 && x[7]>-0.95 && x[7]<0.95 && t7a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[7]>2","");
//c1->cd(9);tree->Draw("Ex>>Exs8(160,-2,14)","detID==8 && x[8]>-0.95 && x[8]<0.95 && t8a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[8]>2","");
//c1->cd(10);tree->Draw("Ex>>Exs9(160,-2,14)","detID==9 && x[9]>-0.95 && x[9]<0.95 && t9a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[9]>2","");
//c1->cd(11);tree->Draw("Ex>>Exs10(160,-2,14)","detID==10 && x[10]>-0.95 && x[10]<0.95","");
//c1->cd(12);tree->Draw("Ex>>Exs11(160,-2,14)","detID==11 && x[11]>-0.95 && x[11]<0.950","");
//
//c1->cd(13);tree->Draw("Ex>>Exs12(160,-2,14)","detID==12 && x[12]>-0.95 && x[12]<0.95 && t12a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[12]>2","");
//c1->cd(14);tree->Draw("Ex>>Exs13(160,-2,14)","detID==13 && x[13]>-0.95 && x[13]<0.95 && t13a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[13]>2","");
//c1->cd(15);tree->Draw("Ex>>Exs14(160,-2,14)","detID==14 && x[14]>-0.95 && x[14]<0.95 && t14a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[14]>2","");
//c1->cd(16);tree->Draw("Ex>>Exs15(160,-2,14)","detID==15 && x[15]>-0.95 && x[15]<0.95 && t15a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[15]>2","");
//c1->cd(17);tree->Draw("Ex>>Exs16(160,-2,14)","detID==16 && x[16]>-0.95 && x[16]<0.95 && t16a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[16]>2","");
//c1->cd(18);tree->Draw("Ex>>Exs17(160,-2,14)","detID==17 && x[17]>-0.95 && x[17]<0.95 && t17a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[17]>2","");
//
//c1->cd(19);tree->Draw("Ex>>Exs18(160,-2,14)","detID==18 && x[18]>-0.95 && x[18]<0.95 && t18a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[18]>2","");
//c1->cd(3);tree->Draw("Ex>>Exs19(160,-2,14)","detID==19 && x[19]>-0.95 && x[19]<0.95 && t19a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[19]>2","");
//c1->cd(21);tree->Draw("Ex>>Exs20(160,-2,14)","detID==20 && x[20]>-0.95 && x[20]<0.95 && t20a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[20]>2","");
//c1->cd(22);tree->Draw("Ex>>Exs21(160,-2,14)","detID==21 && x[21]>-0.95 && x[21]<0.95 && t21a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[21]>2","");
//c1->cd(23);tree->Draw("Ex>>Exs22(160,-2,14)","detID==22 && x[22]>-0.95 && x[22]<0.95 && t22a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[22]>2","");
//c1->cd(3);tree->Draw("Ex>>Exs23(160,-2,14)","detID==23 && x[23]>-0.95 && x[23]<0.95 && t23a && (rec1sb || rec2sb || rec3sb || rec4sb) && e[23]>2","");
//
///*
//Exs0->Add(Exs1);
//Exs0->Add(Exs2);
//Exs0->Add(Exs3);
//Exs0->Add(Exs4);
//Exs0->Add(Exs5);
//
//Exs0->Add(Exs6);
//Exs0->Add(Exs7);
//Exs0->Add(Exs9);
//
//Exs0->Add(Exs12);
//Exs0->Add(Exs13);
//Exs0->Add(Exs14);
//Exs0->Add(Exs15);
//Exs0->Add(Exs16);
//Exs0->Add(Exs17);
//
//Exs0->Add(Exs18);
//Exs0->Add(Exs19);
//Exs0->Add(Exs20);
//Exs0->Add(Exs21);
//Exs0->Add(Exs22);
//Exs0->Add(Exs23);
//*/
//
//gStyle->SetOptStat("i"); 
//c1=new TCanvas();
//c1->SetWindowSize(1400,800);
//c1->Divide(6,4);
//c1->cd(1);tree->Draw("Ex>>Exp0(160,-2,14)","detID==0 && x[0]>-0.95 && x[0]<0.95 && t0a && (rec1pab || rec2pab || rec3pab || rec4pa b) && e[0]>2","");
//c1->cd(2);tree->Draw("Ex>>Exp1(160,-2,14)","detID==1 && x[1]>-0.95 && x[1]<0.95 && t1a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[1]>2","");
//c1->cd(3);tree->Draw("Ex>>Exp2(160,-2,14)","detID==2 && x[2]>-0.95 && x[2]<0.95 && t2a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[2]>2","");
//c1->cd(4);tree->Draw("Ex>>Exp3(160,-2,14)","detID==3 && x[3]>-0.95 && x[3]<0.95 && t3a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[3]>2","");
//c1->cd(5);tree->Draw("Ex>>Exp4(160,-2,14)","detID==4 && x[4]>-0.95 && x[4]<0.95 && t4a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[4]>2","");
//c1->cd(6);tree->Draw("Ex>>Exp5(160,-2,14)","detID==5 && x[5]>-0.95 && x[5]<0.95 && t5a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[5]>2","");
//
//c1->cd(7);tree->Draw("Ex>>Exp6(160,-2,14)","detID==6 && x[6]>-0.95 && x[6]<0.95 && t6a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[6]>2","");
//c1->cd(8);tree->Draw("Ex>>Exp7(160,-2,14)","detID==7 && x[7]>-0.95 && x[7]<0.95 && t7a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[7]>2","");
//c1->cd(9);tree->Draw("Ex>>Exp8(160,-2,14)","detID==8 && x[8]>-0.95 && x[8]<0.95 && t8a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[8]>2","");
//c1->cd(10);tree->Draw("Ex>>Exp9(160,-2,14)","detID==9 && x[9]>-0.95 && x[9]<0.95 && t9a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[9]>2","");
//c1->cd(11);tree->Draw("Ex>>Exp10(160,-2,14)","detID==10 && x[10]>-0.95 && x[10]<0.95","");
//c1->cd(12);tree->Draw("Ex>>Exp11(160,-2,14)","detID==11 && x[11]>-0.95 && x[11]<0.950","");
//
//c1->cd(13);tree->Draw("Ex>>Exp12(160,-2,14)","detID==12 && x[12]>-0.95 && x[12]<0.95 && t12a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[12]>2","");
//c1->cd(14);tree->Draw("Ex>>Exp13(160,-2,14)","detID==13 && x[13]>-0.95 && x[13]<0.95 && t13a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[13]>2","");
//c1->cd(15);tree->Draw("Ex>>Exp14(160,-2,14)","detID==14 && x[14]>-0.95 && x[14]<0.95 && t14a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[14]>2","");
//c1->cd(16);tree->Draw("Ex>>Exp15(160,-2,14)","detID==15 && x[15]>-0.95 && x[15]<0.95 && t15a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[15]>2","");
//c1->cd(17);tree->Draw("Ex>>Exp16(160,-2,14)","detID==16 && x[16]>-0.95 && x[16]<0.95 && t16a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[16]>2","");
//c1->cd(18);tree->Draw("Ex>>Exp17(160,-2,14)","detID==17 && x[17]>-0.95 && x[17]<0.95 && t17a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[17]>2","");
//
//c1->cd(19);tree->Draw("Ex>>Exp18(160,-2,14)","detID==18 && x[18]>-0.95 && x[18]<0.95 && t18a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[18]>2","");
//c1->cd(3);tree->Draw("Ex>>Exp19(160,-2,14)","detID==19 && x[19]>-0.95 && x[19]<0.95 && t19a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[19]>2","");
//c1->cd(21);tree->Draw("Ex>>Exp20(160,-2,14)","detID==20 && x[20]>-0.95 && x[20]<0.95 && t20a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[20]>2","");
//c1->cd(22);tree->Draw("Ex>>Exp21(160,-2,14)","detID==21 && x[21]>-0.95 && x[21]<0.95 && t21a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[21]>2","");
//c1->cd(23);tree->Draw("Ex>>Exp22(160,-2,14)","detID==22 && x[22]>-0.95 && x[22]<0.95 && t22a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[22]>2","");
//c1->cd(3);tree->Draw("Ex>>Exp23(160,-2,14)","detID==23 && x[23]>-0.95 && x[23]<0.95 && t23a && (rec1pab || rec2pab || rec3pab || rec4pab) && e[23]>2","");
//
//Exp0->Add(Exp1);
//Exp0->Add(Exp2);
//Exp0->Add(Exp3);
//Exp0->Add(Exp4);
//Exp0->Add(Exp5);
//
//Exp0->Add(Exp6);
//Exp0->Add(Exp7);
//Exp0->Add(Exp9);
//
//Exp0->Add(Exp12);
//Exp0->Add(Exp13);
//Exp0->Add(Exp14);
//Exp0->Add(Exp15);
//Exp0->Add(Exp16);
//Exp0->Add(Exp17);
//
//Exp0->Add(Exp18);
//Exp0->Add(Exp19);
//Exp0->Add(Exp20);
//Exp0->Add(Exp21);
//Exp0->Add(Exp22);
//Exp0->Add(Exp23);
//
//gStyle->SetOptStat("i"); 
//c1=new TCanvas();
//c1->SetWindowSize(1400,800);
//c1->Divide(6,4);
//c1->cd(1);tree->Draw("Ex>>Extot0(160,-2,14)","detID==0 && x[0]>-0.95 && x[0]<0.95 && t0a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[0]>2","");
//c1->cd(2);tree->Draw("Ex>>Extot1(160,-2,14)","detID==1 && x[1]>-0.95 && x[1]<0.95 && t1a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[1]>2","");
//c1->cd(3);tree->Draw("Ex>>Extot2(160,-2,14)","detID==2 && x[2]>-0.95 && x[2]<0.95 && t2a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[2]>2","");
//c1->cd(4);tree->Draw("Ex>>Extot3(160,-2,14)","detID==3 && x[3]>-0.95 && x[3]<0.95 && t3a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[3]>2","");
//c1->cd(5);tree->Draw("Ex>>Extot4(160,-2,14)","detID==4 && x[4]>-0.95 && x[4]<0.95 && t4a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[4]>2","");
//c1->cd(6);tree->Draw("Ex>>Extot5(160,-2,14)","detID==5 && x[5]>-0.95 && x[5]<0.95 && t5a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[5]>2","");
//
//c1->cd(7);tree->Draw("Ex>>Extot6(160,-2,14)","detID==6 && x[6]>-0.95 && x[6]<0.95 && t6a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[6]>2","");
//c1->cd(8);tree->Draw("Ex>>Extot7(160,-2,14)","detID==7 && x[7]>-0.95 && x[7]<0.95 && t7a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[7]>2","");
//c1->cd(9);tree->Draw("Ex>>Extot8(160,-2,14)","detID==8 && x[8]>-0.95 && x[8]<0.95 && t8a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[8]>2","");
//c1->cd(10);tree->Draw("Ex>>Extot9(160,-2,14)","detID==9 && x[9]>-0.95 && x[9]<0.95 && t9a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[9]>2","");
//c1->cd(11);tree->Draw("Ex>>Extot10(160,-2,14)","detID==10 && x[10]>-0.95 && x[10]<0.95","");
//c1->cd(12);tree->Draw("Ex>>Extot11(160,-2,14)","detID==11 && x[11]>-0.95 && x[11]<0.950","");
//
//c1->cd(13);tree->Draw("Ex>>Extot12(160,-2,14)","detID==12 && x[12]>-0.95 && x[12]<0.95 && t12a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[12]>2","");
//c1->cd(14);tree->Draw("Ex>>Extot13(160,-2,14)","detID==13 && x[13]>-0.95 && x[13]<0.95 && t13a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[13]>2","");
//c1->cd(15);tree->Draw("Ex>>Extot14(160,-2,14)","detID==14 && x[14]>-0.95 && x[14]<0.95 && t14a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[14]>2","");
//c1->cd(16);tree->Draw("Ex>>Extot15(160,-2,14)","detID==15 && x[15]>-0.95 && x[15]<0.95 && t15a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[15]>2","");
//c1->cd(17);tree->Draw("Ex>>Extot16(160,-2,14)","detID==16 && x[16]>-0.95 && x[16]<0.95 && t16a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[16]>2","");
//c1->cd(18);tree->Draw("Ex>>Extot17(160,-2,14)","detID==17 && x[17]>-0.95 && x[17]<0.95 && t17a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[17]>2","");
//
//c1->cd(19);tree->Draw("Ex>>Extot18(160,-2,14)","detID==18 && x[18]>-0.95 && x[18]<0.95 && t18a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[18]>2","");
//c1->cd(3);tree->Draw("Ex>>Extot19(160,-2,14)","detID==19 && x[19]>-0.95 && x[19]<0.95 && t19a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[19]>2","");
//c1->cd(21);tree->Draw("Ex>>Extot20(160,-2,14)","detID==20 && x[20]>-0.95 && x[20]<0.95 && t20a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[20]>2","");
//c1->cd(22);tree->Draw("Ex>>Extot21(160,-2,14)","detID==21 && x[21]>-0.95 && x[21]<0.95 && t21a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[21]>2","");
//c1->cd(23);tree->Draw("Ex>>Extot22(160,-2,14)","detID==22 && x[22]>-0.95 && x[22]<0.95 && t22a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[22]>2","");
//c1->cd(3);tree->Draw("Ex>>Extot23(160,-2,14)","detID==23 && x[23]>-0.95 && x[23]<0.95 && t23a && (rec1pab || rec2pab || rec3pab || rec4pa b || rec1b || rec2b || rec3b || rec4b || rec1sb || rec2sb || rec3sb || rec4sb) && e[23]>2","");
//
//
//Exs0->Add(Exs6);
//Exs0->Add(Exs12);
//Exs0->Add(Exs18);
//
//Exs1->Add(Exs7);
//Exs1->Add(Exs13);
//Exs1->Add(Exs19);
//
//Exs2->Add(Exs8);
//Exs2->Add(Exs14);
//Exs2->Add(Exs20);
//
//Exs3->Add(Exs9);
//Exs3->Add(Exs15);
//Exs3->Add(Exs21);
//
////Exs4->Add(Exs10);
//Exs4->Add(Exs16);
//Exs4->Add(Exs22);
//
////Exs5->Add(Exs11);
//Exs5->Add(Exs17);
//Exs5->Add(Exs23);
//


/*
Extot0->Add(Extot1);
Extot0->Add(Extot2);
Extot0->Add(Extot3);
Extot0->Add(Extot4);
Extot0->Add(Extot5);

Extot0->Add(Extot6);
Extot0->Add(Extot7);
Extot0->Add(Extot9);

Extot0->Add(Extot12);
Extot0->Add(Extot13);
Extot0->Add(Extot14);
Extot0->Add(Extot15);
Extot0->Add(Extot16);
Extot0->Add(Extot17);

Extot0->Add(Extot18);
Extot0->Add(Extot19);
Extot0->Add(Extot20);
Extot0->Add(Extot21);
Extot0->Add(Extot22);
Extot0->Add(Extot23);
*/




//gStyle->SetOptStat("i"); 
//c1=new TCanvas();
//c1->SetWindowSize(1400,400);
//c1->Divide(6,1);
//c1->cd(1);Exs0->Draw();
//c1->cd(2);Exs1->Draw();
//c1->cd(3);Exs2->Draw();
//c1->cd(4);Exs3->Draw();
//c1->cd(5);Exs4->Draw();
//c1->cd(6);Exs5->Draw();
//


/*

tree->Draw("Ex>>Ex0(240,-2,10)","detID==0 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[0]>1.5","");
tree->Draw("Ex>>Ex1(240,-2,10)","detID==1 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[1]>1.5","");
//tree->Draw("Ex>>Ex2(240,-2,10)","detID==2 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[2]>1.5","");
tree->Draw("Ex>>Ex3(240,-2,10)","detID==3 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[3]>1.5","");
tree->Draw("Ex>>Ex4(240,-2,10)","detID==4 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[4]>1.5","");
//tree->Draw("Ex>>Ex5(240,-2,10)","detID==5 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[5]>1.5","");

tree->Draw("Ex>>Ex6(240,-2,10)","detID==6 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[6]>1.5","");
tree->Draw("Ex>>Ex7(240,-2,10)","detID==7 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[7]>1.5","");
//tree->Draw("Ex>>Ex8(240,-2,10)","detID==8 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[8]>1.5","");
tree->Draw("Ex>>Ex9(240,-2,10)","detID==9 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[9]>1.5","");
//tree->Draw("Ex>>Ex10(240,-2,10)","detID==10 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[10]>1.5","");
//tree->Draw("Ex>>Ex11(240,-2,10)","detID==11 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[11]>1.5","");

//tree->Draw("Ex>>Ex12(240,-2,10)","detID==12 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[12]>1.5","");
tree->Draw("Ex>>Ex13(240,-2,10)","detID==13 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[13]>1.5","");
tree->Draw("Ex>>Ex14(240,-2,10)","detID==14 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[14]>1.5","");
tree->Draw("Ex>>Ex15(240,-2,10)","detID==15 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[15]>1.5","");
tree->Draw("Ex>>Ex16(240,-2,10)","detID==16 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[16]>1.5","");
tree->Draw("Ex>>Ex17(240,-2,10)","detID==17 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[17]>1.5","");

tree->Draw("Ex>>Ex18(240,-2,10)","detID==18 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[18]>1.5","");
tree->Draw("Ex>>Ex19(240,-2,10)","detID==19 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[19]>1.5","");
tree->Draw("Ex>>Ex20(240,-2,10)","detID==20 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[20]>1.5","");
tree->Draw("Ex>>Ex21(240,-2,10)","detID==21 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[21]>1.5","");
tree->Draw("Ex>>Ex22(240,-2,10)","detID==22 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[22]>1.5","");
tree->Draw("Ex>>Ex23(240,-2,10)","detID==23 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01ab || r23ab || r45ab || r67ab) && e[23]>1.5","");

Ex0->Add(Ex1);
//Ex0->Add(Ex2);
Ex0->Add(Ex3);
Ex0->Add(Ex4);
//Ex0->Add(Ex5);

Ex0->Add(Ex6);
Ex0->Add(Ex7);
Ex0->Add(Ex9);

//Ex0->Add(Ex12);
Ex0->Add(Ex13);
Ex0->Add(Ex14);
Ex0->Add(Ex15);
Ex0->Add(Ex16);
Ex0->Add(Ex17);

Ex0->Add(Ex18);
Ex0->Add(Ex19);
Ex0->Add(Ex20);
Ex0->Add(Ex21);
Ex0->Add(Ex22);
Ex0->Add(Ex23);

//Ex0->Draw();

tree->Draw("Ex>>Ex0b(240,-2,10)","detID==0 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[0]>1.5","");
tree->Draw("Ex>>Ex1b(240,-2,10)","detID==1 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[1]>1.5","");
//tree->Draw("Ex>>Ex2b(240,-2,10)","detID==2 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[2]>1.5","");
tree->Draw("Ex>>Ex3b(240,-2,10)","detID==3 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[3]>1.5","");
tree->Draw("Ex>>Ex4b(240,-2,10)","detID==4 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[4]>1.5","");
//tree->Draw("Ex>>Ex5b(240,-2,10)","detID==5 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[5]>1.5","");

tree->Draw("Ex>>Ex6b(240,-2,10)","detID==6 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[6]>1.5","");
tree->Draw("Ex>>Ex7b(240,-2,10)","detID==7 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[7]>1.5","");
//tree->Draw("Ex>>Ex8b(240,-2,10)","detID==8 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[8]>1.5","");
tree->Draw("Ex>>Ex9b(240,-2,10)","detID==9 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[9]>1.5","");
//tree->Draw("Ex>>Ex10b(240,-2,10)","detID==10 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[10]>1.5","");
//tree->Draw("Ex>>Ex11b(240,-2,10)","detID==11 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[11]>1.5","");

//tree->Draw("Ex>>Ex12b(240,-2,10)","detID==12 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[12]>1.5","");
tree->Draw("Ex>>Ex13b(240,-2,10)","detID==13 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[13]>1.5","");
tree->Draw("Ex>>Ex14b(240,-2,10)","detID==14 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[14]>1.5","");
tree->Draw("Ex>>Ex15b(240,-2,10)","detID==15 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[15]>1.5","");
tree->Draw("Ex>>Ex16b(240,-2,10)","detID==16 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[16]>1.5","");
tree->Draw("Ex>>Ex17b(240,-2,10)","detID==17 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[17]>1.5","");

tree->Draw("Ex>>Ex18b(240,-2,10)","detID==18 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[18]>1.5","");
tree->Draw("Ex>>Ex19b(240,-2,10)","detID==19 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[19]>1.5","");
tree->Draw("Ex>>Ex20b(240,-2,10)","detID==20 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[20]>1.5","");
tree->Draw("Ex>>Ex21b(240,-2,10)","detID==21 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[21]>1.5","");
tree->Draw("Ex>>Ex22b(240,-2,10)","detID==22 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[22]>1.5","");
tree->Draw("Ex>>Ex23b(240,-2,10)","detID==23 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01bb || r23bb || r45bb || r67bb) && e[23]>1.5","");

Ex0b->Add(Ex1b);
//Ex0b->Add(Ex2b);
Ex0b->Add(Ex3b);
Ex0b->Add(Ex4b);
//Ex0b->Add(Ex5b);

Ex0b->Add(Ex6b);
Ex0b->Add(Ex7b);
Ex0b->Add(Ex9b);

//Ex0b->Add(Ex12b);
Ex0b->Add(Ex13b);
Ex0b->Add(Ex14b);
Ex0b->Add(Ex15b);
Ex0b->Add(Ex16b);
Ex0b->Add(Ex17b);

Ex0b->Add(Ex18b);
Ex0b->Add(Ex19b);
Ex0b->Add(Ex20b);
Ex0b->Add(Ex21b);
Ex0b->Add(Ex22b);
Ex0b->Add(Ex23b);

//Ex0b->Draw();

tree->Draw("Ex>>Ex0c(240,-2,10)","detID==0 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[0]>1.5","");
tree->Draw("Ex>>Ex1c(240,-2,10)","detID==1 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[1]>1.5","");
//tree->Draw("Ex>>Ex2c(240,-2,10)","detID==2 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[2]>1.5","");
tree->Draw("Ex>>Ex3c(240,-2,10)","detID==3 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[3]>1.5","");
tree->Draw("Ex>>Ex4c(240,-2,10)","detID==4 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[4]>1.5","");
//tree->Draw("Ex>>Ex5c(240,-2,10)","detID==5 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[5]>1.5","");

tree->Draw("Ex>>Ex6c(240,-2,10)","detID==6 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[6]>1.5","");
tree->Draw("Ex>>Ex7c(240,-2,10)","detID==7 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[7]>1.5","");
//tree->Draw("Ex>>Ex8c(240,-2,10)","detID==8 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[8]>1.5","");
tree->Draw("Ex>>Ex9c(240,-2,10)","detID==9 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[9]>1.5","");
//tree->Draw("Ex>>Ex10c(240,-2,10)","detID==10 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[10]>1.5","");
//tree->Draw("Ex>>Ex11c(240,-2,10)","detID==11 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[11]>1.5","");

//tree->Draw("Ex>>Ex12c(240,-2,10)","detID==12 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[12]>1.5","");
tree->Draw("Ex>>Ex13c(240,-2,10)","detID==13 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[13]>1.5","");
tree->Draw("Ex>>Ex14c(240,-2,10)","detID==14 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[14]>1.5","");
tree->Draw("Ex>>Ex15c(240,-2,10)","detID==15 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[15]>1.5","");
tree->Draw("Ex>>Ex16c(240,-2,10)","detID==16 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[16]>1.5","");
tree->Draw("Ex>>Ex17c(240,-2,10)","detID==17 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[17]>1.5","");

tree->Draw("Ex>>Ex18c(240,-2,10)","detID==18 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[18]>1.5","");
tree->Draw("Ex>>Ex19c(240,-2,10)","detID==19 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[19]>1.5","");
tree->Draw("Ex>>Ex20c(240,-2,10)","detID==20 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[20]>1.5","");
tree->Draw("Ex>>Ex21c(240,-2,10)","detID==21 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[21]>1.5","");
tree->Draw("Ex>>Ex22c(240,-2,10)","detID==22 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[22]>1.5","");
tree->Draw("Ex>>Ex23c(240,-2,10)","detID==23 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01cb || r23cb || r45cb || r67cb) && e[23]>1.5","");

Ex0c->Add(Ex1c);
//Ex0c->Add(Ex2c);
Ex0c->Add(Ex3c);
Ex0c->Add(Ex4c);
//Ex0c->Add(Ex5c);

Ex0c->Add(Ex6c);
Ex0c->Add(Ex7c);
Ex0c->Add(Ex9c);

//Ex0c->Add(Ex12c);
Ex0c->Add(Ex13c);
Ex0c->Add(Ex14c);
Ex0c->Add(Ex15c);
Ex0c->Add(Ex16c);
Ex0c->Add(Ex17c);

Ex0c->Add(Ex18c);
Ex0c->Add(Ex19c);
Ex0c->Add(Ex20c);
Ex0c->Add(Ex21c);
Ex0c->Add(Ex22c);
Ex0c->Add(Ex23c);

//Ex0c->Draw();

tree->Draw("Ex>>Ex0all(240,-2,10)","detID==0 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[0]>1.5","");
tree->Draw("Ex>>Ex1all(240,-2,10)","detID==1 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[1]>1.5","");
//tree->Draw("Ex>>Ex2all(240,-2,10)","detID==2 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[2]>1.5","");
tree->Draw("Ex>>Ex3all(240,-2,10)","detID==3 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[3]>1.5","");
tree->Draw("Ex>>Ex4all(240,-2,10)","detID==4 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[4]>1.5","");
//tree->Draw("Ex>>Ex5all(240,-2,10)","detID==5 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[5]>1.5","");

tree->Draw("Ex>>Ex6all(240,-2,10)","detID==6 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[6]>1.5","");
tree->Draw("Ex>>Ex7all(240,-2,10)","detID==7 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[7]>1.5","");
//tree->Draw("Ex>>Ex8all(240,-2,10)","detID==8 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[8]>1.5","");
tree->Draw("Ex>>Ex9all(240,-2,10)","detID==9 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[9]>1.5","");
//tree->Draw("Ex>>Ex10all(240,-2,10)","detID==10 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[10]>1.5","");
//tree->Draw("Ex>>Ex11all(240,-2,10)","detID==11 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[11]>1.5","");

//tree->Draw("Ex>>Ex12all(240,-2,10)","detID==12 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[12]>1.5","");
tree->Draw("Ex>>Ex13all(240,-2,10)","detID==13 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[13]>1.5","");
tree->Draw("Ex>>Ex14all(240,-2,10)","detID==14 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[14]>1.5","");
tree->Draw("Ex>>Ex15all(240,-2,10)","detID==15 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[15]>1.5","");
tree->Draw("Ex>>Ex16all(240,-2,10)","detID==16 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[16]>1.5","");
tree->Draw("Ex>>Ex17all(240,-2,10)","detID==17 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[17]>1.5","");

tree->Draw("Ex>>Ex18all(240,-2,10)","detID==18 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[18]>1.5","");
tree->Draw("Ex>>Ex19all(240,-2,10)","detID==19 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[19]>1.5","");
tree->Draw("Ex>>Ex20all(240,-2,10)","detID==20 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[20]>1.5","");
tree->Draw("Ex>>Ex21all(240,-2,10)","detID==21 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[21]>1.5","");
tree->Draw("Ex>>Ex22all(240,-2,10)","detID==22 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[22]>1.5","");
tree->Draw("Ex>>Ex23all(240,-2,10)","detID==23 && coin_t>-5 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.9 && x<0.9 && (r01allb || r23allb || r45allb || r67allb) && e[23]>1.5","");

Ex0all->Add(Ex1all);
//Ex0all->Add(Ex2all);
Ex0all->Add(Ex3all);
Ex0all->Add(Ex4all);
//Ex0all->Add(Ex5all);

Ex0all->Add(Ex6all);
Ex0all->Add(Ex7all);
Ex0all->Add(Ex9all);

//Ex0all->Add(Ex12all);
Ex0all->Add(Ex13all);
Ex0all->Add(Ex14all);
Ex0all->Add(Ex15all);
Ex0all->Add(Ex16all);
Ex0all->Add(Ex17all);

Ex0all->Add(Ex18all);
Ex0all->Add(Ex19all);
Ex0all->Add(Ex20all);
Ex0all->Add(Ex21all);
Ex0all->Add(Ex22all);
Ex0all->Add(Ex23all);

//Ex0all->Draw();

gStyle->SetOptStat("i"); 
c1=new TCanvas();
c1->SetWindowSize(600,1200);
c1->Divide(1,4);
c1->cd(1);Ex0all->Draw();
c1->cd(2);Ex0->Draw();
c1->cd(3);Ex0b->Draw();
c1->cd(4);Ex0c->Draw();

*/

////c1->cd(1);tree->Draw("-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460>>all(375,2,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM>15 && thetaCM<40 && e>1"); //perfect ungated all
//c1->cd(1);tree->Draw("Ex>>all(375,2,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM>15 && thetaCM<40 && e>1"); //perfect ungated all
//c1->cd(1);tree->Draw("Ex>>alpha(375,2,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM>15 && thetaCM<40 && e>1 && (rdt1_alpha1ab || rdt2_alpha1ab || rdt3_alpha1ab || rdt4_alpha1ab || rdt1_alpha1bb || rdt2_alpha1bb || rdt3_alpha1bb || rdt4_alpha1b)"); //perfect ungated all
//c1->cd(1);tree->Draw("Ex>>neut(375,2,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM>15 && thetaCM<40 && e>1 && (rdt1_neut1b || rdt2_neut1b || rdt3_neut1b || rdt4_neut1)"); //perfect ungated all
//c1->cd(1);tree->Draw("Ex>>prot(375,2,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM>15 && thetaCM<40 && e>1 && (rdt1_pb || rdt2_pb || rdt3_pb || rdt4_p)"); //perfect ungated all
//
//c1->cd(2);tree->Draw("-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460>>ball(200,6,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM > 10 && e>1");
//c1->cd(3);tree->Draw("-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460>>b10b11(200,6,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM > 10 && e>1 && (rdt1_ballb || rdt3_ballb || rdt4_ball)");
//c1->cd(4);tree->Draw("-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460>>alpha(200,6,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM > 10 && e>1 && (rdt1_alpha1ab || rdt2_alpha1ab || rdt3_alpha1ab || rdt4_alpha1ab || rdt1_alpha1bb || rdt2_alpha1bb || rdt3_alpha1bb || rdt4_alpha1b)");
//c1->cd(5);tree->Draw("-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460>>neut(200,6,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM > 10 && e>1  && (rdt1_neut1b || rdt3_neut1b || rdt4_neut1)");
//c1->cd(6);tree->Draw("-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460>>prot(200,6,14)","x > -0.95 && x < 0.95 && coin_t > -10 && coin_t < 20 && thetaCM > 10 && e>1  && (rdt1_p1b || rdt3_p1b || rdt4_p1)")

//tree->Draw("rdt[1]:rdt[0]>>haha(500,0,5000,500,0,5000)","coin_t > -8 && coin_t < 8 && (-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460)>11.2 && (-(0.03782428*Ex**2)+(1.60922045*Ex)-2.43758460) < 14","colz")

//tree->Draw("e:z>>haha1(500,-500,0,500,0,10)","coin_t>-10 && coin_t<20 && thetaCM>15 && thetaCM<40 && x>-0.95 && x<0.95","colz")

//gStyle->SetOptStat("i"); 
//c1=new TCanvas();
//c1->SetWindowSize(600,1200);
//c1->Divide(1,4);
////c1->cd(1);tree->Draw("Ex>>all(50,10,14)","x > -0.9 && x < 0.9 && coin_t > -10 && coin_t < 15");
////c1->cd(1);tree->Draw("Ex>>b10b11(50,10,14)","x > -0.9 && x < 0.9 && coin_t > -10 && coin_t < 15 && (rdt1_ballb || rdt2_ballb || rdt3_ballb || rdt4_ball)");
//c1->cd(1);tree->Draw("Ex>>na(200,10,14)","x > -0.9 && x < 0.9 && coin_t > -8 && coin_t < 8 && (rdt1_neut1b || rdt1_alpha1ab || rdt1_alpha1bb || rdt2_neut1b || rdt2_alpha1ab || rdt2_alpha1bb || rdt3_neut1b || rdt3_alpha1ab || rdt3_alpha1bb || rdt4_neut1b || rdt4_alpha1ab || rdt4_alpha1b)");
//c1->cd(2);tree->Draw("Ex>>alpha(200,10,14)","x > -0.9 && x < 0.9 && coin_t > -8 && coin_t < 8  && (rdt1_alpha1ab || rdt1_alpha1bb || rdt2_alpha1ab || rdt2_alpha1bb || rdt3_alpha1ab || rdt3_alpha1bb || rdt4_alpha1ab || rdt4_alpha1b)");
//c1->cd(3);tree->Draw("Ex>>neut(200,10,14)","x > -0.9 && x < 0.9 && coin_t > -8 && coin_t < 8  && (rdt1_neut1b || rdt2_neut1b || rdt3_neut1b || rdt4_neut1)");
//c1->cd(4);tree->Draw("Ex>>be(200,10,14)","x > -0.9 && x < 0.9 && coin_t > -8 && coin_t < 8  && (rdt1_pb || rdt2_pb || rdt3_pb || rdt4_p)");



//gStyle->SetOptStat("i"); 
//c1=new TCanvas();
//c1->SetWindowSize(1400,800);
//c1->Divide(6,4);
//c1->cd(1);tree->Draw("coinTime[0]:x[0]>>tx0(500,-2,2,500,-400,400)","x[0]>-0.95 && x[0]<0.95","colz");
//c1->cd(2);tree->Draw("coinTime[1]:x[1]>>tx1(500,-2,2,500,-400,400)","x[1]>-0.95 && x[1]<0.95","colz");
//c1->cd(3);tree->Draw("coinTime[2]:x[2]>>tx2(500,-2,2,500,-400,400)","x[2]>-0.95 && x[2]<0.95","colz");
//c1->cd(4);tree->Draw("coinTime[3]:x[3]>>tx3(500,-2,2,500,-400,400)","x[3]>-0.95 && x[3]<0.95","colz");
//c1->cd(5);tree->Draw("coinTime[4]:x[4]>>tx4(500,-2,2,500,-400,400)","x[4]>-0.95 && x[4]<0.95","colz");
//c1->cd(6);tree->Draw("coinTime[5]:x[5]>>tx5(500,-2,2,500,-400,400)","x[5]>-0.95 && x[5]<0.95","colz");
//
//c1->cd(7);tree->Draw("coinTime[6]:x[6]>>tx6(500,-2,2,500,-400,400)","x[6]>-0.95 && x[6]<0.95","colz");
//c1->cd(8);tree->Draw("coinTime[7]:x[7]>>tx7(500,-2,2,500,-400,400)","x[7]>-0.95 && x[7]<0.95","colz");
//c1->cd(9);tree->Draw("coinTime[8]:x[8]>>tx8(500,-2,2,500,-400,400)","x[8]>-0.95 && x[8]<0.95","colz");
//c1->cd(10);tree->Draw("coinTime[9]:x[9]>>tx9(500,-2,2,500,-400,400)","x[9]>-0.95 && x[9]<0.95","colz");
//c1->cd(11);tree->Draw("coinTime[10]:x[10]>>tx10(500,-2,2,500,-400,400)","x[10]>-0.95 && x[10]<0.95","colz");
//c1->cd(12);tree->Draw("coinTime[11]:x[11]>>tx11(500,-2,2,500,-400,400)","x[11]>-0.95 && x[11]<0.95","colz");
//
//c1->cd(13);tree->Draw("coinTime[12]:x[12]>>tx12(500,-2,2,500,-400,400)","x[12]>-0.95 && x[12]<0.95","colz");
//c1->cd(14);tree->Draw("coinTime[13]:x[13]>>tx13(500,-2,2,500,-400,400)","x[13]>-0.95 && x[13]<0.95","colz");
//c1->cd(15);tree->Draw("coinTime[14]:x[14]>>tx14(500,-2,2,500,-400,400)","x[14]>-0.95 && x[14]<0.95","colz");
//c1->cd(16);tree->Draw("coinTime[15]:x[15]>>tx15(500,-2,2,500,-400,400)","x[15]>-0.95 && x[15]<0.95","colz");
//c1->cd(17);tree->Draw("coinTime[16]:x[16]>>tx16(500,-2,2,500,-400,400)","x[16]>-0.95 && x[16]<0.95","colz");
//c1->cd(18);tree->Draw("coinTime[17]:x[17]>>tx17(500,-2,2,500,-400,400)","x[17]>-0.95 && x[17]<0.95","colz");
//
//c1->cd(19);tree->Draw("coinTime[18]:x[18]>>tx18(500,-2,2,500,-400,400)","x[18]>-0.95 && x[18]<0.95","colz");
//c1->cd(3);tree->Draw("coinTime[19]:x[19]>>tx19(500,-2,2,500,-400,400)","x[19]>-0.95 && x[19]<0.95","colz");
//c1->cd(21);tree->Draw("coinTime[20]:x[20]>>tx20(500,-2,2,500,-400,400)","x[20]>-0.95 && x[20]<0.95","colz");
//c1->cd(22);tree->Draw("coinTime[21]:x[21]>>tx21(500,-2,2,500,-400,400)","x[21]>-0.95 && x[21]<0.95","colz");
//c1->cd(23);tree->Draw("coinTime[22]:x[22]>>tx22(500,-2,2,500,-400,400)","x[22]>-0.95 && x[22]<0.95","colz");
//c1->cd(3);tree->Draw("coinTime[23]:x[23]>>tx23(500,-2,2,500,-400,400)","x[23]>-0.95 && x[23]<0.95","colz");

////tree->SetAlias("tcc0","coinTime[3]+(4.09438*x[3])+(82.4161*x[3]**2)-(11.4832*x[3]**3)-(66.5726*x[3]**4)-25.4912");

//tree->Draw("coinTime[0]+(0.833555*x[0])+(93.8753*x[0]**2)+(0.595455*x[0]**3)-(75.4543*x[0]**4)-21.4228:x[0]>>tx0a(500,-2,2,500,-400,400)","x[0]>-0.95 && x[0]<0.95","colz")

//c1->cd(10);tree->Draw("coinTime[9]:x[9]>>tx9(500,-2,2,500,300,1000)","x[9]>-0.95 && x[9]<0.95","colz");
//tree->Draw("coinTime[9]-(2.14106*x[9])+(101.126*x[9]**2)+(4.50877*x[9]**3)-(85.3259*x[9]**4)-781.835:x[9]>>tx9a(500,-2,2,500,-500,200)","x[9]>-0.95 && x[9]<0.95","colz")


/*

gStyle->SetOptStat("i"); 
c1=new TCanvas();
c1->SetWindowSize(1600,400);
c1->Divide(6,1);
c1->cd(1);
tree->Draw("Ex>>Ex0(50,10,14)","coin_t > -10 && coin_t < 15 && detID==0 && x[0]>-0.9 && x[0]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[0]>9 ");
tree->Draw("Ex>>Ex6(50,10,14)","coin_t > -10 && coin_t < 15 && detID==6 && x[6]>-0.9 && x[6]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[6]>9 ");
tree->Draw("Ex>>Ex12(50,10,14)","coin_t > -10 && coin_t < 15 && detID==12 && x[12]>-0.9 && x[12]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[12]>9 ");
tree->Draw("Ex>>Ex18(50,10,14)","coin_t > -10 && coin_t < 15 && detID==18 && x[18]>-0.9 && x[18]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[18]>9 ");
//Ex0p->Add(Ex6p); Ex0p->Add(Ex12p); Ex0p->Add(Ex18p); Ex0p->Draw("");
Ex0->Add(Ex6); Ex0->Add(Ex12); Ex0->Add(Ex18); Ex0->Draw("");

c1->cd(2);
tree->Draw("Ex>>Ex1(50,10,14)","coin_t > -10 && coin_t < 15 && detID==1 && x[1]>-0.9 && x[1]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[1]>9 ");
tree->Draw("Ex>>Ex7(50,10,14)","coin_t > -10 && coin_t < 15 && detID==7 && x[7]>-0.9 && x[7]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[7]>9");
tree->Draw("Ex>>Ex13(50,10,14)","coin_t > -10 && coin_t < 15 && detID==13 && x[13]>-0.9 && x[13]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[13]>9 ");
tree->Draw("Ex>>Ex19(50,10,14)","coin_t > -10 && coin_t < 15 && detID==19 && x[19]>-0.9 && x[19]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[19]>9 ");
Ex1->Add(Ex7); Ex1->Add(Ex13); Ex1->Add(Ex19); Ex1->Draw("");

c1->cd(3);
tree->Draw("Ex>>Ex2(50,10,14)","coin_t > -10 && coin_t < 15 && detID==2 && x[2]>-0.9 && x[2]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[2]>9 ");
tree->Draw("Ex>>Ex8(50,10,14)","coin_t > -10 && coin_t < 15 && detID==8 && x[8]>-0.9 && x[8]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[8]>9 ");
tree->Draw("Ex>>Ex14(50,10,14)","coin_t > -10 && coin_t < 15 && detID==14 && x[14]>-0.9 && x[14]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[14]>9 ");
tree->Draw("Ex>>Ex20(50,10,14)","coin_t > -10 && coin_t < 15 && detID==20 && x[20]>-0.9 && x[20]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[20]>9 ");
Ex2->Add(Ex8); Ex2->Add(Ex14); Ex2->Add(Ex20); Ex2->Draw("");
//Ex8->Add(Ex14); Ex8->Add(Ex20); Ex8->Draw("");

c1->cd(4);
tree->Draw("Ex>>Ex3(50,10,14)","coin_t > -10 && coin_t < 15 && detID==3 && x[3]>-0.9 && x[3]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[3]>9 ");
tree->Draw("Ex>>Ex9(50,10,14)","coin_t > -10 && coin_t < 15 && detID==9 && x[9]>-0.9 && x[9]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[9]>9 ");
tree->Draw("Ex>>Ex15(50,10,14)","coin_t > -10 && coin_t < 15 && detID==15 && x[15]>-0.9 && x[15]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[15]>9 ");
tree->Draw("Ex>>Ex21(50,10,14)","coin_t > -10 && coin_t < 15 && detID==21 && x[21]>-0.9 && x[21]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[21]>9 ");
Ex3->Add(Ex9); Ex3->Add(Ex15); Ex3->Add(Ex21); Ex3->Draw("");

c1->cd(5);
tree->Draw("Ex>>Ex4(50,10,14)","coin_t > -10 && coin_t < 15 && detID==4 && x[4]>-0.9 && x[4]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[4]>9 ");
tree->Draw("Ex>>Ex10(50,10,14)","coin_t > -10 && coin_t < 15 && detID==10 && x[10]>-0.9 && x[10]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[10]>9 ");
tree->Draw("Ex>>Ex16(50,10,14)","coin_t > -10 && coin_t < 15 && detID==16 && x[16]>-0.9 && x[16]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[16]>9 ");
tree->Draw("Ex>>Ex22(50,10,14)","coin_t > -10 && coin_t < 15 && detID==22 && x[22]>-0.9 && x[22]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[22]>9 ");
Ex4->Add(Ex10); Ex4->Add(Ex16); Ex4->Add(Ex22); Ex4->Draw("");
//Ex4p->Add(Ex10p); Ex4p->Draw("");

c1->cd(6);
tree->Draw("Ex>>Ex5(50,10,14)","coin_t > -10 && coin_t < 15 && detID==5 && x[5]>-0.9 && x[5]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[5]>9 ");
tree->Draw("Ex>>Ex11(50,10,14)","coin_t > -10 && coin_t < 15 && detID==11 && x[11]>-0.9 && x[11]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[11]>9 ");
tree->Draw("Ex>>Ex17(50,10,14)","coin_t > -10 && coin_t < 15 && detID==17 && x[17]>-0.9 && x[17]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[17]>9 ");
tree->Draw("Ex>>Ex23(50,10,14)","coin_t > -10 && coin_t < 15 && detID==23 && x[23]>-0.9 && x[23]<0.9 && (rdt1_neutb || rdt3_neutb || rdt4_neutb) && thetaCM[23]>9 ");
Ex5->Add(Ex11); Ex5->Add(Ex17); Ex5->Add(Ex23); Ex5->Draw("");
//Ex5p->Add(Ex23p); Ex5p->Draw("");


gStyle->SetOptStat("i"); 
c1=new TCanvas();
c1->SetWindowSize(1400,800);
c1->Divide(6,4);
c1->cd(1);tree->Draw("Ex>>Ex0a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==0 && x[0]>-0.95 && x[0]<0.95 && thetaCM[0]>10 ");Ex0a->Draw("");
c1->cd(2);tree->Draw("Ex>>Ex1a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==1 && x[1]>-0.95 && x[1]<0.95 && thetaCM[1]>10 ");Ex1a->Draw("");
c1->cd(3);tree->Draw("Ex>>Ex2a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==2 && x[2]>-0.95 && x[2]<0.95 && thetaCM[2]>10 ");Ex2a->Draw("");
c1->cd(4);tree->Draw("Ex>>Ex3a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==3 && x[3]>-0.95 && x[3]<0.95 && thetaCM[3]>10 ");Ex3a->Draw("");
c1->cd(5);tree->Draw("Ex>>Ex4a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==4 && x[4]>-0.95 && x[4]<0.95 && thetaCM[4]>10 ");Ex4a->Draw("");
c1->cd(6);tree->Draw("Ex>>Ex5a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==5 && x[5]>-0.95 && x[5]<0.95 && thetaCM[5]>10 ");Ex5a->Draw("");

c1->cd(7);tree->Draw("Ex>>Ex6a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==6 && x[6]>-0.95 && x[6]<0.95 && thetaCM[6]>10 ");Ex6a->Draw("");
c1->cd(8);tree->Draw("Ex>>Ex7a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==7 && x[7]>-0.95 && x[7]<0.95 && thetaCM[7]>10 ");Ex7a->Draw("");
c1->cd(9);tree->Draw("Ex>>Ex8a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==8 && x[8]>-0.95 && x[8]<0.95 && thetaCM[8]>10 ");Ex8a->Draw("");
c1->cd(10);tree->Draw("Ex>>Ex9a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==9 && x[9]>-0.95 && x[9]<0.95 && thetaCM[9]>10 ");Ex9a->Draw("");
c1->cd(11);tree->Draw("Ex>>Ex10a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==10 && x[10]>-0.95 && x[10]<0.95 && thetaCM[10]>10 ");Ex10a->Draw("");
c1->cd(12);tree->Draw("Ex>>Ex11a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==11 && x[11]>-0.95 && x[11]<0.95 && thetaCM[11]>10 ");Ex11a->Draw("");

c1->cd(13);tree->Draw("Ex>>Ex12a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==12 && x[12]>-0.95 && x[12]<0.95 && thetaCM[12]>10 ");Ex12a->Draw("");
c1->cd(14);tree->Draw("Ex>>Ex13a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==14 && x[14]>-0.95 && x[14]<0.95 && thetaCM[14]>10");Ex14a->Draw("");
c1->cd(16);tree->Draw("Ex>>Ex15a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==15 && x[15]>-0.95 && x[15]<0.95 && thetaCM[15]>10");Ex15a->Draw("");
c1->cd(17);tree->Draw("Ex>>Ex16a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==16 && x[16]>-0.95 && x[16]<0.95 && thetaCM[16]>10 ");Ex16a->Draw("");
c1->cd(18);tree->Draw("Ex>>Ex17a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==17 && x[17]>-0.95 && x[17]<0.95 && thetaCM[17]>10 ");Ex17a->Draw("");

c1->cd(19);tree->Draw("Ex>>Ex18a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==18 && x[18]>-0.95 && x[18]<0.95 && thetaCM[18]>10 ");Ex18a->Draw("");
c1->cd(3);tree->Draw("Ex>>Ex19a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==19 && x[19]>-0.95 && x[19]<0.95 && thetaCM[19]>10 ");Ex19a->Draw("");
c1->cd(21);tree->Draw("Ex>>Ex20a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==20 && x[20]>-0.95 && x[20]<0.95 && thetaCM[20]>10 ");Ex20a->Draw("");
c1->cd(22);tree->Draw("Ex>>Ex21a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==21 && x[21]>-0.95 && x[21]<0.95 && thetaCM[21]>10 ");Ex21a->Draw("");
c1->cd(23);tree->Draw("Ex>>Ex22a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==22 && x[22]>-0.95 && x[22]<0.95 && thetaCM[22]>10 ");Ex22a->Draw("");
c1->cd(3);tree->Draw("Ex>>Ex23a(150,4,14)","coin_t > -10 && coin_t < 15 && detID==23 && x[23]>-0.95 && x[23]<0.95 && thetaCM[23]>10");Ex23a->Draw("");

*/
}
