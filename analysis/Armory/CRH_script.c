{

Int_t k = 0;
Int_t ab = 0;

const char * AB = "a";
if (ab==1) {AB = "b";}
Int_t cutYN = 0;

TFile *file0 = new TFile ("../root_data/h066_31Si_trace_si31full.root", "read"); 
TTree *tree = (TTree*)file0->Get("tree");
printf("=====> is loaded. Total #Entry: %10lld \n", tree->GetEntries());

Float_t p[30][4];
// = {{0.986768,0.0081,0.977690,0.0292},{1.005557,-0.0561,1.000905,-0.0117},{1.0,0.0,1.0,0.0},{1.0,0.0,1.0,0.0}};
for (int ll=0;ll<30;ll++){
    p[ll][0]=1.0;p[ll][1]=0.0;p[ll][2]=1.0;p[ll][3]=0.0;
}
p[0][0] = 0.985223;p[0][1] = 0.0186;
p[15][0] = 1.003435; p[15][1] = -0.0445;
p[20][0] = 1.001980; p[20][1] =-0.0067;
p[1][0] = 1.045362; p[1][1] = -0.2184;
p[6][0] = 1.036781; p[6][1] = -0.2635;
p[11][0] = 1.017317; p[11][1] = -0.2224;
p[16][0] = 0.988451; p[16][1] = -0.0432;
p[21][0] = 0.995145; p[21][1] = 0.0398;
p[26][0] = 0.996624; p[26][1] = 0.0645;
p[27][0] = 0.992819; p[27][1] = 0.1239;
p[22][0] = 1.008689; p[22][1] = -0.0201;
p[17][0] = 0.988156; p[17][1] = -0.0475;
p[12][0] = 1.018902; p[12][1] = -0.1884;
p[7][0] = 1.010720; p[7][1] = -0.1773;
p[28][0] = 0.958577; p[28][1] = 0.2261;
p[23][0] = 0.977891; p[23][1] = 0.0681;
p[13][0] = 0.970958; p[13][1] = 0.0826;
p[3][0] = 0.966338; p[3][1] = 0.0192;
p[29][0] = 1.013431; p[29][1] = -0.0384;
p[24][0] = 0.994901; p[24][1] = 0.0557;
p[19][0] = 1.014655; p[19][1] = -0.1352;
p[14][0] = 1.029827; p[14][1] = -0.2706;
p[4][0] = 1.005909; p[4][1] = -0.1299;

printf("%f",p[0][0]);

TCanvas *cc = new TCanvas("cc","cc",1400,800);cc->ToggleEditor();cc->ToggleToolBar();
cc->Clear();cc->ToggleEventStatus();
TH1F * h[60];
TH1F * hSum;
hSum = new TH1F("hSum","hSum",300,-1,11);
for (int i=k;i<k+30;i++) {
    for (int j=0;j<1;j++) {
        TString nameis;
        AB = "a";
        if (j==1) {AB = "b";}
        h[2*i+j] = new TH1F(Form("hEx%d%s",i,AB),Form("hEx%d%s",i,AB),300,-1,11);
        tree->SetAlias("Exp",Form("(Ex[%d]*%f + %f)",i,p[i][j*2],p[i][j*2+1]));
        h[2*i+j]->SetLineColor(2*i+j+1);
        if (j==0) {
            tree->Draw(Form("Exp>>hEx%d%s",i,AB),Form("coinTime>-40 && coinTime<20 && x > -1 && x < 1 && thetaCM>5 && detID == %d && e[%d]>1",i,i),"");
            //fitNGauss(h[2*i+j],50);
        } else {
            tree->Draw(Form("Exp>>hEx%d%s",i,AB),Form("coinTime < 15 && x > 0 && x > -1 && x < 1 && thetaCM>5 && detID == %d && e[%d]>1",i,i),"");
            h[2*i+j]->Rebin();
            fitNGauss(h[2*i+j],50);
        }
        hSum->Add(h[2*i+j]);
    }
}
cc->Clear(); 
hSum->SetLineColor(kBlue);hSum->Draw("");
tree->Draw("Ex>>hEX(400,-1,11)",Form("x > -1 && x < 1 && coinTime>-40 && coinTime<20 && thetaCM>15"),"same");

}