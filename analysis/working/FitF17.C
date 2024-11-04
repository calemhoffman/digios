#define N 24

double Peak(double *dim, double *par){

    double  x       = dim[0];

    double  area    = par[0];
    double  cent    = par[1];
    double  sigma   = par[2];

    return area/(sigma*TMath::Sqrt(2*TMath::Pi())) * TMath::Gaus(x,cent,sigma);

}

double FitNPeaks(double *dim, double *par){

    double  x       = dim[0];

    double  val     = 0;

    double  sigma   = par[0];
    double  p0      = par[1];
    double  p1      = par[2];

    double  *PeakPar    = new double[3];

    for(int i=0;i<N;i++){
        PeakPar[0]  = par[3+i*2];
        if(i==0)
            PeakPar[1] = par[4];
        else
            PeakPar[1] = par[4+i*2] + par[4];
        PeakPar[2]  = sigma;
        val += Peak(dim,PeakPar);
    }

    val += p0 + p1*x;
    
    return val;

}

void FitData(){

    gStyle->SetOptStat(0);

    TFile   *infile = new TFile("ExcitationSpectra.root","READ");
    TH1F    *h1     = (TH1F*)infile->Get("hExCut1");

    TF1     *f1     = new TF1("Fit",FitNPeaks,-1,7,2*N+3);

    f1->SetParameter(0,0.1);
    f1->SetParLimits(0,0.05,0.15);

    f1->SetParName(0,"sigma");
    f1->SetParName(1,"p0");
    f1->SetParName(2,"p1");
    for(int i=0;i<N;i++){
        f1->SetParName(3+2*i,Form("Area%i",i+1));        
        f1->SetParName(4+2*i,Form("Cent%i",i+1));
    }

    // Peak 1
    f1->SetParameter(3,10);
    f1->SetParLimits(3,0,1000);
    f1->SetParameter(4,0);
    f1->SetParLimits(4,-0.1,0.1);
    // Peak 2
    f1->SetParameter(5,10);
    f1->SetParLimits(5,0,1000);
    //f1->SetParameter(6,1.0);
    //f1->SetParLimits(6,0.9,1.1);
    f1->FixParameter(6,0.9376);
    // Peak 3
    f1->SetParameter(7,10);
    f1->SetParLimits(7,0,1000);
    //f1->SetParameter(8,3.0);
    //f1->SetParLimits(8,2.9,3.1);
    f1->FixParameter(8,1.0416);
    // Peak 4
    f1->SetParameter(9,10);
    f1->SetParLimits(9,0,1000);
    //f1->SetParameter(10,3.7);
    //f1->SetParLimits(10,3.6,3.8);
    f1->FixParameter(10,1.0805);
    // Peak 5
    f1->SetParameter(11,10);
    f1->SetParLimits(11,0,1000);
    //f1->SetParameter(12,4.1);
    //f1->SetParLimits(12,4.0,4.2);
    f1->FixParameter(12,1.1214);
    // Peak 6
    f1->SetParameter(13,10);
    f1->SetParLimits(13,0,1000);
    //f1->SetParameter(14,4.6);
    //f1->SetParLimits(14,4.5,4.7);
    f1->FixParameter(14,1.7008);
    // Peak 7
    f1->SetParameter(15,10);
    f1->SetParLimits(15,0,1000);
    //f1->SetParameter(16,4.9);
    //f1->SetParLimits(16,4.8,5.0);
    f1->FixParameter(16,2.1006);
    // Peak 8
    f1->SetParameter(17,10);
    f1->SetParLimits(17,0,1000);
    //f1->SetParameter(18,5.3);
    //f1->SetParLimits(18,5.1,5.5);
    f1->FixParameter(18,2.5234);
    // Peak 9
    f1->SetParameter(19,10);
    f1->SetParLimits(19,0,1000);
    //f1->SetParameter(20,1.7);
    //f1->SetParLimits(20,1.6,1.8);
    f1->FixParameter(20,3.0618);
    // Peak 10
    f1->SetParameter(21,10);
    f1->SetParLimits(21,0,1000);
    //f1->SetParameter(22,2.1);
    //f1->SetParLimits(22,2.0,2.2);
    f1->FixParameter(22,3.1339);
    // Peak 11
    f1->SetParameter(23,10);
    f1->SetParLimits(23,0,1000);
    //f1->SetParameter(24,0.9);
    //f1->SetParLimits(24,0.8,1.0);
    f1->FixParameter(24,3.3582);
    // Peak 12
    f1->SetParameter(25,10);
    f1->SetParLimits(25,0,1000);
    //f1->SetParameter(26,1.1);
    //f1->SetParLimits(26,1.05,1.2);
    f1->FixParameter(26,3.7242);
    // Peak 13
    f1->SetParameter(27,10);
    f1->SetParLimits(27,0,1000);
    //f1->SetParameter(28,2.5);
    //f1->SetParLimits(28,2.4,2.6);
    f1->FixParameter(28,3.7915);
    // Peak 14
    f1->SetParameter(29,10);
    f1->SetParLimits(29,0,1000);
    //f1->SetParameter(30,3.2);
    //f1->SetParLimits(30,3.0,3.4);
    f1->FixParameter(30,3.8292);
    // Peak 15
    f1->SetParameter(31,10);
    f1->SetParLimits(31,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(32,4.1159);
    // Peak 16
    f1->SetParameter(33,10);
    f1->SetParLimits(33,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(34,4.2258);
    // Peak 17
    f1->SetParameter(35,10);
    f1->SetParLimits(35,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(36,4.3602);
    // Peak 18
    f1->SetParameter(37,10);
    f1->SetParLimits(37,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(38,4.3981);
    // Peak 19
    f1->SetParameter(39,10);
    f1->SetParLimits(39,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(40,4.652);
    // Peak 20
    f1->SetParameter(41,10);
    f1->SetParLimits(41,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(42,4.753);
    // Peak 21
    f1->SetParameter(43,10);
    f1->SetParLimits(43,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(44,4.8483);
    // Peak 22
    f1->SetParameter(45,10);
    f1->SetParLimits(45,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(46,4.860);
    // Peak 23
    f1->SetParameter(47,10);
    f1->SetParLimits(47,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(48,4.9636);
    // Peak 24
    f1->SetParameter(49,10);
    f1->SetParLimits(49,0,1000);
    //f1->SetParameter(32,1.0);
    //f1->SetParLimits(32,0.9,1.1);
    f1->FixParameter(50,5.2976);

    f1->SetNpx(500);
    h1->Fit(f1,"RB0");

    h1->SetBinErrorOption(TH1::kPoisson);

    h1->Draw("e1");
    h1->SetMarkerStyle(20);
    h1->SetMarkerColor(kRed);
    h1->SetMarkerSize(1);
    f1->SetLineWidth(4);
    f1->Draw("same");

    TF1 *indpeak[N];
    for(int i=0;i<N;i++){
        indpeak[i] = new TF1(Form("Peak_%i",i+1),Peak,f1->GetParameter(4+i*2)-0.5,f1->GetParameter(4+i*2)+0.5,3);
        indpeak[i]->SetParameter(0,f1->GetParameter(3+i*2));
        if(i==0)
            indpeak[i]->SetParameter(1,f1->GetParameter(4));
        else
            indpeak[i]->SetParameter(1,f1->GetParameter(4+i*2)+f1->GetParameter(4));
        indpeak[i]->SetParameter(2,f1->GetParameter(0));
        indpeak[i]->SetLineStyle(7);
        indpeak[i]->Draw("same");
    }

}