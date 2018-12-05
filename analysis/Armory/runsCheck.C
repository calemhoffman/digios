{
   TString folderPath ="../root_data/gen_run*.root";
   const char* treeName="gen_tree";
   
   //==============================================
   //TString rootFile;
   
   TFile * f = NULL;
   TTree * tree = NULL;
   
   ULong64_t e_t[100];
   TBranch  *b_EnergyTimestamp;
   
   double time1, time2, dTime;
   ULong64_t firstTime, lastTime;
   
   TString cmd;
   cmd.Form(".!ls -1 %s | sort > runList.txt", folderPath.Data());
   gROOT->ProcessLine(cmd);
   
   ifstream file;
   file.open("runList.txt");
   vector<TString> rootFileName;
   if( file.is_open() ){
      string x;
      while( file >> x){
         rootFileName.push_back(x);
      }
   }else{
       printf("... fail\n");
       gROOT->ProcessLine(".!rm -f runList.txt");
       return;
   }
   
   gROOT->ProcessLine(".!rm -f runList.txt"); 
   
   
   FILE * paraOut;
   TString filename;
   filename.Form("run_Summary.dat");
   paraOut = fopen (filename, "w+");
   
   int numFile = rootFileName.size();
   for( int i = 0; i < numFile ; i++){
      
      //printf("%s \n", rootFileName[i].Data());
      
      f = new TFile (rootFileName[i], "read");
      
      if( !f->IsOpen()) continue; 
      printf("%15s is loaded.", rootFileName[i].Data());
      
      tree = (TTree*)f->Get(treeName);

      tree->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
      
      int totalEvent = tree->GetEntries();
      
      time1 = 0;
      time2 = 0;
      dTime = 0;
      firstTime = 0;
      lastTime = 0;
      
      //find the first event has time_recored
      
      bool breakFlag = false;
      for(int event = 1; event < totalEvent; event++){
         tree->GetEntry(event);
         for(int j = 0; j < 24; j++){
            if( e_t[j] > 0 ) {
               firstTime = e_t[j];
               //printf("%d ", event);
               breakFlag = true;
               break;
            }
         }
         if( breakFlag ) break;      
      }
      
      //find the last event time_recored
      breakFlag = false;
      for(int event = totalEvent-1; event > 0; event--){
         tree->GetEntry(event);
         for(int j = 0; j < 24; j++){
            if( e_t[j] > 0 ) {
               lastTime = e_t[j];
               //printf(", %d \n", event);
               breakFlag = true;
               break;
            }
         }
         if( breakFlag ) break;      
      }
      
      
      time1 = firstTime/1e8;
      time2 = lastTime/1e8;
      dTime = (lastTime - firstTime)/1e8;
      //printf("%11f sec, %11f sec, %11f sec \n", time1, time2, dTime);
      
      double size = f->GetSize(); // in byte
      printf("#Entry: %10d, size: %6.1f MB, duration: %10.2f sec = %7.2f min = %7.2f hr\n", 
               totalEvent, size/1024/1024, dTime, dTime/60., dTime/60./60.);
      
      
      if( i == 0 ) {
         fprintf(paraOut, "%50s, %10s, %8s, %11s, %11s, %11s\n", "file",  "#event", "size[MB]", "duration[s]", "[min]", "[hour]");   
      
      }   
      fprintf(paraOut, "%50s, %10d, %8.2f, %11.3f, %11.3f, %11.3f\n", rootFileName[i].Data(),  totalEvent, size/1024/1024, dTime, dTime/60., dTime/60./60.);   
      fflush(paraOut);   
      
      
   }
      
   fclose(paraOut);
   
   printf("=========== saved summery to %s.\n", filename.Data());
   
   gROOT->ProcessLine(".q");
   
}
