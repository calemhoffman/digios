#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"
#include "THStack.h"

#include "../Armory/Cali_gamma.C"

/*********************************************
 * 
 * The script is for temperary analysis,
 * 
 * plot some graphs, apply simple gate or cut.
 * 
 * 
 * ******************************************/
 
void script_Cali_gamma(){

   //========================== input file/cut/simulation   
   TChain * tree = new TChain("gen_tree");   
   ///tree->Add("../root_data/gen_run355.root"); ///137Cs, upstream
   ///tree->Add("../root_data/gen_run356.root"); ///137Cs, downstream
   
   ///tree->Add("../root_data/gen_run357.root"); ///133Ba, upstream
   ///tree->Add("../root_data/gen_run358.root"); ///133Ba, downstream
   
   ///tree->Add("../root_data/gen_run359.root"); ///152Eu, upstream
   ///tree->Add("../root_data/gen_run360.root"); ///152Eu, upstream
   ///tree->Add("../root_data/gen_run361.root"); ///152Eu, downstream
   
   ///tree->Add("../root_data/gen_run362.root"); ///226Ra, upstream
   ///tree->Add("../root_data/gen_run363.root"); ///226Ra, upstream
   ///tree->Add("../root_data/gen_run364.root"); ///226Ra, downstream
   
   ///tree->Add("../root_data/gen_run365.root"); ///207Bi, upstream
   ///tree->Add("../root_data/gen_run366.root"); ///207Bi, upstream
   ///tree->Add("../root_data/gen_run367.root"); ///207Bi, downstream
   
   tree->Add("../root_data/gen_run338.root"); ///16N, 700enA
   
   tree->GetListOfFiles()->Print();
   
   Cali_gamma(tree, 367, 0.6);
   
   /*
   vector<double> ref = {   81.000,
                           121.780,
                           244.699,
                           276.340,
                           302.850,
                           344.280,
                           356.010,
                           383.850,
                           443.960,
                           569.702,
                           778.900,
                           964.050,
                          1063.662,
                          1085.840,
                          1112.090,
                          1408.020,
                          1770.237};
                          
   vector<double> e1 = { 130.020,
                         194.300,
                         391.460,
                         445.670,
                         489.180,
                         554.820,
                         573.990,
                         618.240,
                         714.120,
                         914.690,
                        1249.540,
                        1545.280,
                        1703.440,
                        1739.980,
                        1782.020,
                        2256.230,
                        2836.560};
                        
graph = new TGraph(ref.size(), &e1[0], &ref[0] );
   graph->Draw("A*");
   
   TF1 * fit = new TF1("fit", "pol3" );
   graph->Fit("fit", "");
   
   double ssr = 0;
   
   for( int i = 0; i < ref.size(); i++){
      
      double y1 = fit->Eval(e1[i]);
      
      ssr += (y1 - ref[i]) * (y1 - ref[i]);
      
      printf(" %8.3f | %8.3f | %8.3f | %8.3f \n", ref[i], graph->Eval(e1[i]), y1, y1 - ref[i]);
      
   }
   
   printf("===== rms %f \n", sqrt(ssr/ref.size()));
   
   printf("%f \n", graph->Eval(6000));
   
   graph->Draw("AP");
   
   */
   
}

