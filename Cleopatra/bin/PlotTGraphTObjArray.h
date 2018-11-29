/***********************************************************************
 * 
 *  This is PlotResultInRoot.C for ExtractXSec *.root output
 * 
 *  The Xsec are stored in (TObjArray *) gList
 * 
 *  This program is simple get plot all the member in the gList 
 * 
 * -----------------------------------------------------
 *  This program will call the root library and compile in g++
 *  compilation:
 *  g++ PlotResultInROOT.C -o PlotResultInROOT `root-config --cflags --glibs`
 *
 * ------------------------------------------------------
 *  created by Ryan (Tsz Leung) Tang, Nov-18, 2018
 *  email: goluckyryan@gmail.com
 * ********************************************************************/

#include <TROOT.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TLegend.h>

void PlotTGraphTObjArray(TString rootFileName){
  
  TFile * file = new TFile(rootFileName, "READ");
  
  TObjArray * gList = (TObjArray *) file->FindObjectAny("gList");
  
  if( gList == NULL ) {
    printf("No Result was found.\n");
    return;
  }
  
  TCanvas * cPlots = new TCanvas("cPlots", "Ptolemy Results", 0, 0, 800, 600);
  cPlots->SetLogy();
  
  TLegend * legend = new TLegend( 0.6, 0.2, 0.9, 0.4); 
  
  const int n = gList->GetLast() + 1 ; 
  
  TGraph * gr[n];

  //Get minimum, maximum Y
  double maxY = 0;
  double minY = 10000000;
  for ( int i = 0; i < n ; i++){
    
    gr[i] = (TGraph *) gList->At(i);
    gr[i]->SetLineColor(i+1);
    gr[i]->GetXaxis()->SetTitle("#theta_{CM} [deg]");
    gr[i]->GetYaxis()->SetTitle("d#sigma/d#Omega [mb/sr]");
    
    TString name = gr[i]->GetName();
    int pos = name.First("|");
    name.Remove(0, pos+1);
    legend->AddEntry(gr[i], name);
    
    double miny = gr[i]->GetHistogram()->GetMinimum();
    double maxy = gr[i]->GetHistogram()->GetMaximum();
    
    if( miny < minY ) minY = miny;
    if( maxy > maxY ) maxY = maxy;
  }
  
  
  
  for ( int i = 0; i < n ; i++){
    gr[i]->Draw("same");
    
    if( i == 0 ){
      gr[i]->Draw();
      gr[i]->GetYaxis()->SetRangeUser(minY * 0.8, maxY * 1.2);
    }else{
      gr[i]->Draw("same");
    }
  }
  legend->Draw();
  
  cPlots->Update();
  cPlots->Draw();
  
}
