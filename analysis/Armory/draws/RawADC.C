{
  TH2F * hADC = new TH2F("hADC","Raw ADC Data; Energy (channels); ID number",
			 8192,-16384,16384,200,1000,1200);
  
  tree->Draw("id:((post_rise_energy-pre_rise_energy)/100)>>hADC","","colz");

}
