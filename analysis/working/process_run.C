void
process_run(Int_t RUNNUM=5, Int_t SORTNUM=0)
{
  if (SORTNUM==0) {
    TString name;
    name.Form("../root_data/run%0*d.root", 3, RUNNUM);
    TFile f(name);
    TTree *t1 = (TTree*)f.Get("tree");
    t1->Process("../Armory/GeneralSort.C++");
    f.Close();
  }

  else if (SORTNUM==1) {
    TString name("gen.root");
    TFile ff(name);
    TTree *t2 = (TTree*)ff.Get("gen_tree");
    t2->Process("../Armory/Monitors.C++");
  }
}
