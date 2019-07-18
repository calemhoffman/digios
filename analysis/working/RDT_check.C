
{


recoils();

//Count1DH("RDT-5  All", hrdt5, crdtS, 6,    0, 5900, 0);
//Count1DH("RDT-5 29Al", hrdt5, crdtS, 6, 2540, 2840, 2);
//Count1DH("RDT-5 30Si", hrdt5, crdtS, 6, 2900, 3400, 4);
//printf("------------------\n");
//Count1DH("RDT-7  All", hrdt7, crdtS, 8,    0, 5900, 0);
//Count1DH("RDT-7 29Al", hrdt7, crdtS, 8, 2600, 3100, 2);
//Count1DH("RDT-7 30Si", hrdt7, crdtS, 8, 3200, 3600, 4);

TString expression, gate ;

TLatex text;

//================  Get Recoil cuts;
//TFile * fCut2 = new TFile("rdtCuts.30Si_recoilcheck.root");	
//TFile * fCut2 = new TFile("rdtCuts.30Si_recoilcheck_1.root");	 // new cuts after beam tune on Sat 13th (less Si contaminant in beam)
//TFile * fCut2 = new TFile("rdtCutsSi29narrow.root");	
TFile * fCut2 = new TFile("rdtCuts30Sinarrow.root");

bool isCutFile2Open = fCut2->IsOpen();
if(!isCutFile2Open) cout<<"Failed to open cutfile2"<<endl;
int numCut2 = 0 ;
TObjArray * cut2List = NULL;
if( isCutFile2Open ){
   cut2List = (TObjArray *) fCut2->FindObjectAny("cutList");
   numCut2 = cut2List->GetEntries();
   
   printf("=========== found %d cutG in %s \n", numCut2, fCut2->GetName());

   TCutG * cutG2 = new TCutG();
   for(int i = 0; i < numCut2 ; i++){
      cutG2 = (TCutG * ) cut2List->At(i);
      TString name = cutG2->GetName();
      name += "_2";
      cutG2->SetName(name);
      printf(" cut name : %s , VarX: %s, VarY: %s, numPoints: %d \n",
         cut2List->At(i)->GetName(),
         ((TCutG*)cut2List->At(i))->GetVarX(),
         ((TCutG*)cut2List->At(i))->GetVarY(),
         ((TCutG*)cut2List->At(i))->GetN());
   }
}
   
for( int i = 0; i < 4; i++){
   crdt->cd(i+1);
   expression.Form("rdt[%d]:rdt[%d]>>h", 2*i+1, 2*i);
   gate.Form("cut%d", i);
   int count1 = gen_tree->Draw(expression, gate); 

   gate.Form("cut%d_2", i);
   int count2 = gen_tree->Draw(expression, gate); 

   hrdt2D[i]->Draw("colz");

   double x, y;
   ((TCutG*)cutList->At(i))->GetPoint(0, x, y);
   ((TCutG*)cutList->At(i))->Draw("same");
   
   text.SetTextFont(82);
   text.SetTextSize(0.04);
   text.SetTextColor(i+1);
   
   text.DrawLatex(x, y, Form("^{29}Al: %d", count1));
   
   
   ((TCutG*)cut2List->At(i))->GetPoint(0, x, y);
   ((TCutG*)cut2List->At(i))->Draw("same");
   
   text.DrawLatex(x, y, Form("^{30}Si: %d", count2));
   
   int tot = hrdt2D[i]->Integral();
   
   text.SetTextColor(1);
   text.DrawLatex(3000, 0, Form("Total: %d", tot));
}






}
