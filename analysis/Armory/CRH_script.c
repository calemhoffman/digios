{

Int_t i = 21;
Int_t ab = 1;

const char * AB = "a";
if (ab==1) {AB = "b";}
Int_t cutYN = 0;

TFile *file0 = new TFile ("../root_data/h066_31Si_trace_run025.root", "read"); 
TTree *tree = (TTree*)file0->Get("tree");
printf("=====> is loaded. Total #Entry: %10lld \n", tree->GetEntries());

tree->Draw(Form("e[%d]:x[%d]>>hex(500,-1.1,1.1,500,0,10)",i,i),"coinTime < 20 && x < 0","");

Float_t p0[60][2] = {{3.26484,3.66092},{4.76014,4.77665},{5.7928,5.75805},{6.83909,6.64798},{7.91455,7.91455}}; 
Float_t p1[60][2] = {{0.57155,0.519099},{0.488649,0.463622},{0.452201,0.44516},{0.578821,0.4640},{0.51175,0.51175}};
//Float_t p2[60] = 0.0;
//Float_t p3[60] = 0.0;

TCanvas *cc = new TCanvas("cc","cc",1400,800);cc->ToggleEditor();cc->ToggleToolBar();
cc->Divide(1,2);
cc->cd(1);
tree->SetAlias("ep",Form("e[%d] - (x[%d]*%f)",i,i,p1[i-20][ab]));
if (ab==0) tree->Draw(Form("ep>>hex(600,-1,11)"),"x < 0 && x > -0.95 && x < 0.95 && thetaCM>12","");
if (ab==1) tree->Draw(Form("ep>>hex(600,-1,11)"),"x > 0 && x > -0.95 && x < 0.95 && thetaCM>12","");
cc->cd(2);
if (ab==0) tree->Draw(Form("ep:x[%d]>>hex2(500,-1.1,1.1,500,0,10)",i),"x < 0 && thetaCM>12","");
if (ab==1) tree->Draw(Form("ep:x[%d]>>hex2(500,-1.1,1.1,500,0,10)",i),"x > 0 && thetaCM>12","");


Float_t pp0[10][2] = {{-1.010388,-1.021364},{-1.033074,-1.031451}};
Float_t pp1[10][2] = {{6.8577,6.8571},{8.0326,8.0742}};
// if (cutYN == 1) tree->Draw("ep>>hex(600,-1,11)",gate,"");
cc->Clear(); cc->Divide(1,2);
cc->cd(1);
tree->SetAlias("epp",Form("(e[%d] - (x[%d]*%f)*%f + %f",
i,i,p1[i - 20][ab],pp0[i - 20][ab],pp1[i - 20][ab]));
tree->Draw(Form("ep>>hex(600,-1,11)"),"x > 0 && x > -1 && x < 1","");
cc->cd(2);
tree->Draw(Form("ep:x[%d]>>hex2(500,-1.1,1.1,500,0,10)",i),"x > 0","colz");

cc->Clear();
TH1F * h[60];
TH1F * hSum;
hSum = new TH1F("hSum","hSum",400,-1,7);
for (i=20;i<22;i++) {
for (int j=0;j<2;j++) {
    TString nameis;
    AB = "a";
    if (j==1) {AB = "b";}
    h[i+i*j] = new TH1F(Form("hEx%d%s",i,AB),Form("hEx%d%s",i,AB),400,-1,7);
    tree->SetAlias("epp",Form("(e[%d] - (x[%d]*%f))*%f + %f",
    i,i,p1[i-20][j],pp0[i -20][j],pp1[i -20][j]));
    h[i+i*j]->SetLineColor(i + i*j -19);
    if (j==0) {
        tree->Draw(Form("epp>>hEx%d%s",i,AB),"x < 0 && x > -1 && x < 1 && thetaCM>12","");
    } else {
        tree->Draw(Form("epp>>hEx%d%s",i,AB),"x > 0 && x > -1 && x < 1 && thetaCM>12","same");
    }
    hSum->Add(h[i+i*j]);
}
}
// cc->Clear(); 
hSum->SetLineColor(kBlue);hSum->Draw("");
tree->Draw("Ex>>hEX(400,-1,7)",Form("x > -1 && x < 1 && (detID == 20 || detID ==21)"),"same");





/////////////////////////////////////////
// gen_tree->Draw(Form("e[%d]:ring[%d]>>her(500,-1000,7000,500,0,8000)",i,i),"","colz");
// gate.Form("ring[%d] > -100 && ring[%d] < 100",i,i);
// gen_tree->Draw(Form("xf[%d]:xn[%d]>>hxx(500,400,8000,500,400,8000)",i,i),gate,"colz");
// Float_t xnCorr = 0.96;//0.94; //0.96;//1.0;//1.00;//--//0.9768; //--//0.9589;//--//0.9376; //0.9428; //--//0.947; //0.9478; // -- //0.9732; //0.978508;
// gen_tree->Draw(Form("xf[%d]:xn[%d]*%f>>hxx(500,400,8000,500,400,8000)",i,i,xnCorr),gate,"colz");
// Float_t xfxneCorr1 = 1.11;//1.11;//0.9463;//1.0;//1.0; //--//1.11; //--//1.028;//--//1.035; //1.025; //--//1.02;//1.023;//--//1.02;//1.03;
// Float_t xfxneCorr0 = 0.0;
// gen_tree->Draw(Form("e[%d]:((xf[%d]+xn[%d]*%f)*%f+%f)>>hxsum(500,400,8000,500,400,8000)",i,i,i,xnCorr,xfxneCorr1,xfxneCorr0),gate,"colz");
// gen_tree->SetAlias("xfC",Form("xf[%d]*%f+%f",i,xfxneCorr1,xfxneCorr0));
// gen_tree->SetAlias("xnC",Form("xn[%d]*%f*%f+%f",i,xnCorr,xfxneCorr1,xfxneCorr0));
// gate.Form("ring[%d] > -50 && ring[%d] < 50 && xfC >= e[%d]/2.",i,i,i);
// gen_tree->Draw(Form("e[%d]:(2.0*xfC/e[%d] - 1.0)>>hex1(500,-1.1,1.1,500,400,8000)",i,i),gate,"colz");
// gate.Form("ring[%d] > -50 && ring[%d] < 50 && xnC > e[%d]/2.",i,i,i);
// gen_tree->Draw(Form("e[%d]:(1.0 - 2.0*xnC/e[%d])>>hex2(500,-1.1,1.1,500,400,8000)",i,i),gate,"same colz");

// gate.Form("ring[%d] > -50 && ring[%d] < 50 && xfC >= e[%d]/2.",i,i,i);
// gen_tree->Draw(Form("(2.0*xfC/e[%d] - 1.0)>>hx1(200,-1.1,1.1)",i),gate,"");

// gate.Form("ring[%d] > -50 && ring[%d] < 50 && xnC > e[%d]/2.",i,i,i);
// gen_tree->Draw(Form("(1.0 - 2.0*xnC/e[%d])>>hx2(200,-1.1,1.1)",i),gate,"same");

//det0
// Float_t xnCorr = 0.978508;
// Float_t xfxneCorr1 = 1.03;
// Float_t xfxneCorr0 = 0.0;



}