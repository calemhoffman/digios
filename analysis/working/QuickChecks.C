#include <TTree.h>
#include <TFile.h>

void QuickChecks() {
    TFile * file = new TFile("root_data/s005_32Si_trace_tp.root");
    TTree * tree = (TTree *) file->Get("tree");
    //tree->Print();

    Float_t xcorr8[3] = {-12.5515,1.3713,-26.5162};
    tree->SetAlias("dt7","coinTimeUC-37.8491-2.4986*x[7]+45.908*x[7]*x[7]");
    tree->SetAlias("dt8","coinTimeUC+12.5515-1.3713*x[8]+26.5162*x[8]*x[8]");
    tree->SetAlias("dt14","coinTimeUC+17.3969-2.19879*x[14]+17.177*x[14]*x[14]");

TCanvas *cc = new TCanvas("cc","cc",800,600);

tree->Draw("coinTimeUC:x[7]>>htxgg(100,-1.3,1.3,100,-100,100)","detID==7&&CUTG","");
tree->Draw("dt7:x[7]>>htx(100,-1.3,1.3,100,-100,100)","detID==7","");

// //     tree->Draw("dt8>>hdta(100,-100,100)","","");
// //     tree->Draw("dt8>>hdtb(100,-100,100)","x[8]>-0.95&&x[8]<0.95&&e[8]<5.0","same");
// //     tree->Draw("dt8>>hdtc(100,-100,100)","rdt[4]>1000&&rdt[4]<1800","same");
// //     tree->Draw("dt8>>hdtd(100,-100,100)","x[8]>-0.95&&x[8]<0.95&&rdt[4]>1000&&rdt[4]<1800","same");

//     tree->Draw("dt14>>hdta(100,-100,100)","","");
//     tree->Draw("dt14>>hdtb(100,-100,100)","x[14]>-1&&x[14]<1&&e[14]<5.0","same");
//     tree->Draw("dt14>>hdtc(100,-100,100)","rdt[4]>1000&&rdt[4]<1800","same");
//     tree->Draw("dt14>>hdtd(100,-100,100)","x[14]>-1&&x[14]<1&&rdt[4]>1000&&rdt[4]<1800","same");

// //     tree->Draw("e[8]:dt8>>hdtexd(100,-100,100,100,-1,9)","x[8]>-0.95&&x[8]<0.95&&rdt[4]>1000&&rdt[4]<1800","");
tree->Draw("Ex:dt14>>hdtexd14(100,-100,100,100,-1,9)","x[14]>-1&&x[14]<1&&rdt[4]>1000&&rdt[4]<1800","");
tree->Draw("Ex:dt7>>hdtexd7(100,-100,100,100,-1,9)","x[7]>-0.95&&x[7]<0.95&&rdt[4]>1000&&rdt[4]<1800","");
//     tree->Draw("e[14]:dt14>>hdtexd(100,-100,100,100,-1,9)","x[14]>-1&&x[14]<1&&rdt[4]>1000&&rdt[4]<1800","");


cc->Clear();
TH1F * he[24];
TH1F * heb[24];
for (int i=0;i<24;i++) 
{
    he[i] = new TH1F(Form("he%d",i),Form("he%d",i),100,0,10);
    heb[i] = new TH1F(Form("heb%d",i),Form("heb%d",i),100,0,10);
}
TH1F * hEx = new TH1F("hEx","hEx",100,-1,9);
TH1F * hex7 = new TH1F("hex7","hex7",100,-1,9);
TH1F * hex8 = new TH1F("hex8","hex8",100,-1,9);
TH1F * hex14 = new TH1F("hex14","hex14",100,-1,9);


he[7]->SetLineColor(kRed);
tree->Draw("e[7]>>he7","x[7]>-0.95&&x[7]<0.95&&rdt[4]>1000&&rdt[4]<1800","");
he[7]->SetLineColor(4);
tree->Draw("e[7]>>heb7","dt7>10&&dt7<30&&x[7]>-0.95&&x[7]<0.95&&rdt[4]>500&&rdt[4]<1800","same");
tree->Draw("Ex>>hex7","dt7>15&&dt7<30&&x[7]>-0.95&&x[7]<0.95&&rdt[4]>500&&rdt[4]<1600","");

he[8]->SetLineColor(kRed);
tree->Draw("e[8]>>he8","x[8]>-0.95&&x[8]<0.95&&rdt[4]>1000&&rdt[4]<1800","");
he[8]->SetLineColor(4);
tree->Draw("e[8]>>heb8","dt8>10&&dt8<30&&x[8]>-0.95&&x[8]<0.95&&rdt[4]>500&&rdt[4]<1800","same");
tree->Draw("Ex>>hex8","dt8>15&&dt8<30&&x[8]>-0.95&&x[8]<0.95&&rdt[4]>500&&rdt[4]<1600","");

he[14]->SetLineColor(kRed);
tree->Draw("e[14]>>he14","x[14]>-0.95&&x[14]<0.95&&rdt[4]>1000&&rdt[4]<1800","");
he[14]->SetLineColor(4);
tree->Draw("e[14]>>heb14","dt14>10&&dt14<35&&x[14]>-0.95&&x[14]<0.95&&rdt[4]>500&&rdt[4]<1800","same");
tree->Draw("Ex>>hex14","dt14>10&&dt14<30&&x[14]>-0.95&&x[14]<0.95&&rdt[4]>500&&rdt[4]<1600","");

hEx->Add(hex7);hEx->Add(hex8);hEx->Add(hex14);
hEx->Draw();

he[0]->Add(he[7]);he[0]->Add(he[8]); he[0]->Add(he[14]);
heb[0]->Add(heb[7]);heb[0]->Add(heb[8]); heb[0]->Add(heb[14]);
he[0]->Draw();heb[0]->SetLineColor(3); he[0]->SetTitle("energy det7+det8+det14");heb[0]->Draw("same");

cc->Clear();
tree->Draw("-dt14>>hdta(100,-100,100)","","");
// tree->Draw("-dt14>>hdtb(100,-100,100)","x[14]>-1&&x[14]<1&&e[14]<5.0","same");
// tree->Draw("-dt14>>hdtc(100,-100,100)","rdt[4]>1000&&rdt[4]<1800","same");
tree->Draw("-dt14>>hdtd(100,-100,100)","x[14]>-1&&x[14]<1&&rdt[4]>1000&&rdt[4]<1800","same");

/*
cc->Clear();cc->Divide(2,2);
cc->cd(1);
tree->Draw("-rdt[0]:-rdt[6]>>hrdt0(1000,0,1500,1000,0,2200)","","colz");
cc->cd(2);
tree->Draw("-rdt[1]:-rdt[6]>>hrdt1(1000,0,1500,1000,0,2200)","","colz");
cc->cd(3);
tree->Draw("-rdt[2]:-rdt[6]>>hrdt2(1000,0,1500,1000,0,2200)","","colz");
cc->cd(4);
tree->Draw("-rdt[3]:-rdt[6]>>hrdt3(1000,0,1500,1000,0,2200)","","colz");
*/

// cc->Clear();cc->Divide(2,2);
// cc->cd(1);
// tree->Draw("-rdt[0]>>hrdt0(1000,0,2500)","","");
// tree->Draw("-rdt[0]>>hrdt0a(1000,0,2500)","rdt[4]>500","same");
// cc->cd(2);
// tree->Draw("-rdt[1]>>hrdt1(1000,0,2500)","","");
// tree->Draw("-rdt[1]>>hrdt1a(1000,0,2500)","rdt[4]>500","same");
// cc->cd(3);
// tree->Draw("-rdt[2]>>hrdt2(1000,0,2500)","","");
// tree->Draw("-rdt[2]>>hrdt2a(1000,0,2500)","rdt[4]>500","same");
// cc->cd(4);
// tree->Draw("-rdt[3]>>hrdt3(1000,0,2500)","","");
// tree->Draw("-rdt[3]>>hrdt3a(1000,0,2500)","rdt[4]>500","same");
}