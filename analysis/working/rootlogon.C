{
  printf("rootlogon.C\n");
  gROOT->ProcessLine(".L ../Armory/AutoFit.C");
  //printf("GSUtil_new_cc.so loaded\n");
  gROOT->ProcessLine(".L ../Armory/Check_e_x.C");
  // gROOT->ProcessLine(".L ../Armory/Check_e_z.C");
  // gROOT->ProcessLine(".L ../Armory/Check_alignment.C");
  // gROOT->ProcessLine(".L ../Armory/Check_caliResult.C");
  // gROOT->ProcessLine(".L ../Armory/CRH_cals.C");


  // Set ROOT defaults
  gStyle->SetPalette(1);
  gStyle->SetOptStat("im");
  gStyle->ToggleEditor();
  gStyle->ToggleToolBar();
  /* gStyle->SetLineWidth(2); */
  /* gStyle->SetStatW(0.3); */
  /* gStyle->SetStatH(0.3); */
  /* gStyle->SetTitleW(0.4); */
  /* gStyle->SetTitleH(0.1); */
  /* gStyle->SetCanvasBorderMode(0); */
  /* gStyle->SetPadBorderMode(0); */
  /* gStyle->SetPadColor(0); */
  /* gStyle->SetFrameBorderMode(0); */
  /* gStyle->SetFrameLineWidth(2); */
  /* gStyle->SetCanvasColor(0); */
  /* gStyle->SetOptDate(4); */
  /* gStyle->GetAttDate()->SetTextFont(62); */
  /* gStyle->GetAttDate()->SetTextSize(0.02); */
  /* gStyle->GetAttDate()->SetTextAngle(0); */
  /* gStyle->GetAttDate()->SetTextAlign(11); */
  /* gStyle->GetAttDate()->SetTextColor(1); */
  gStyle->SetDateX(0);
  gStyle->SetDateY(0);

  gROOT->SetStyle("Plain");                       // plain histogram style
  gStyle->SetOptStat("nemruoi");                  // expanded stats box
  gStyle->SetPadTickX(1);                         // tic marks on all axes
  gStyle->SetPadTickY(1);                         //
  gStyle->SetOptFit(1111);                        // the results of the fits
  gStyle->SetPadGridX(kTRUE);                     // draw horizontal and vertical grids
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetPalette(108);                        // pretty and useful palette
  gStyle->SetHistLineWidth(1);                    // a thicker histogram line
  gStyle->SetFrameFillColor(10);                  // a different frame colour
  gStyle->SetTitleFillColor(33);                 // title colour to highlight it
  gStyle->SetTitleW(.76);                         // Title Width
  gStyle->SetTitleH(.07);                        // Title height
  gStyle->SetHistMinimumZero(true);               // Suppresses Histogram Zero Supression


  //Browser *b = new TBrowser()

}
