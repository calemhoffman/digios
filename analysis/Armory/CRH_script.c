{
TString gate;
Int_t i = 11;

gen_tree->Draw(Form("e[%d]:ring[%d]>>her(500,-1000,7000,500,0,8000)",i,i),"","colz");


gate.Form("ring[%d] > -100 && ring[%d] < 100",i,i);

gen_tree->Draw(Form("xf[%d]:xn[%d]>>hxx(500,400,8000,500,400,8000)",i,i),gate,"colz");

Float_t xnCorr = 0.96;//0.94; //0.96;//1.0;//1.00;//--//0.9768; //--//0.9589;//--//0.9376; //0.9428; //--//0.947; //0.9478; // -- //0.9732; //0.978508;
gen_tree->Draw(Form("xf[%d]:xn[%d]*%f>>hxx(500,400,8000,500,400,8000)",i,i,xnCorr),gate,"colz");


Float_t xfxneCorr1 = 1.11;//1.11;//0.9463;//1.0;//1.0; //--//1.11; //--//1.028;//--//1.035; //1.025; //--//1.02;//1.023;//--//1.02;//1.03;
Float_t xfxneCorr0 = 0.0;

gen_tree->Draw(Form("e[%d]:((xf[%d]+xn[%d]*%f)*%f+%f)>>hxsum(500,400,8000,500,400,8000)",i,i,i,xnCorr,xfxneCorr1,xfxneCorr0),gate,"colz");

gen_tree->SetAlias("xfC",Form("xf[%d]*%f+%f",i,xfxneCorr1,xfxneCorr0));
gen_tree->SetAlias("xnC",Form("xn[%d]*%f*%f+%f",i,xnCorr,xfxneCorr1,xfxneCorr0));

gate.Form("ring[%d] > -50 && ring[%d] < 50 && xfC >= e[%d]/2.",i,i,i);
gen_tree->Draw(Form("e[%d]:(2.0*xfC/e[%d] - 1.0)>>hex1(500,-1.1,1.1,500,400,8000)",i,i),gate,"colz");

gate.Form("ring[%d] > -50 && ring[%d] < 50 && xnC > e[%d]/2.",i,i,i);
gen_tree->Draw(Form("e[%d]:(1.0 - 2.0*xnC/e[%d])>>hex2(500,-1.1,1.1,500,400,8000)",i,i),gate,"same colz");

// gate.Form("ring[%d] > -50 && ring[%d] < 50 && xfC >= e[%d]/2.",i,i,i);
// gen_tree->Draw(Form("(2.0*xfC/e[%d] - 1.0)>>hx1(200,-1.1,1.1)",i),gate,"");

// gate.Form("ring[%d] > -50 && ring[%d] < 50 && xnC > e[%d]/2.",i,i,i);
// gen_tree->Draw(Form("(1.0 - 2.0*xnC/e[%d])>>hx2(200,-1.1,1.1)",i),gate,"same");





//det0
// Float_t xnCorr = 0.978508;
// Float_t xfxneCorr1 = 1.03;
// Float_t xfxneCorr0 = 0.0;



}