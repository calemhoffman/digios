#include "HELIOS_LIB.h"
#include "TROOT.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TRandom.h"
#include "TGraph.h"
#include "TMacro.h"
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <TObjArray.h>

double exDistFunc(Double_t *x, Double_t * par){
  return par[(int) x[0]];
}

void Transfer(
              string basicConfig = "reactionConfig.txt", 
         string heliosDetGeoFile = "detectorGeo.txt", 
        string    excitationFile = "Ex.txt", //when no file, only ground state
        TString      ptolemyRoot = "DWBA.root", // when no file, use isotropic distribution of thetaCM
        TString     saveFileName = "transfer.root",
        TString         filename = "reaction.dat"){ // when no file, no output.

  //================================================= User Setting
  //---- reaction
  int AA, zA; //beam
  int Aa, za; //target
  int Ab, zb; //recoil-1

  //---- beam
  double KEAmean, KEAsigma; // MeV/u , assume Guassian
  double thetaMean, thetaSigma; // mrad , assume Guassian due to small angle
  double xBeam, yBeam; // mm

  int numEvent;

  //---- HELIOS detector geometry
  double BField , BFieldTheta; // Bfield and direction of B-field in T and deg
  bool isCoincidentWithRecoil = false; 
  double eSigma, zSigma; // detector energy and position resolution in MeV and mm

  //---- excitation of Beam 
  int nExA = 1;
  double ExAList[nExA];
  ExAList[0] = 0.000; // MeV

  bool isFromOutSide = true; //detector facing outside

  //---- target
  bool isTargetScattering = false;
  double density = 0.913; // 0.913 g/cm3
  double targetThickness = 2./2. * 2.2e-4; // 2.2 um = 201 ug/cm2
  string stoppingPowerForA = "25Mg_in_CD2.txt"; // generated by SRIM
  string stoppingPowerForb = "1H_in_CD2.txt";
  string stoppingPowerForB = "26Mg_in_CD2.txt";

  //---- Auxiliary setting
  bool isDecay = false;
  int decayA = 1;
  int decayZ = 0;
  bool isReDo = false; // redo calculation until detected. 

  //---- if basicConfig.txt exist, overide the reaction   
  ifstream cFile;
  cFile.open(basicConfig.c_str());
  if( cFile.is_open() ){
    string line;
    int i = 0;
    while( cFile >> line){
      //printf("%d, %s \n", i,  line.c_str());
      if( line.substr(0,2) == "//" ) continue;
      if( i == 0 ) AA = atoi(line.c_str());
      if( i == 1 ) zA = atoi(line.c_str());
      if( i == 2 ) Aa = atoi(line.c_str());
      if( i == 3 ) za = atoi(line.c_str());
      if( i == 4 ) Ab = atoi(line.c_str());
      if( i == 5 ) zb = atoi(line.c_str());
      if( i == 6 ) KEAmean = atof(line.c_str());
      if( i == 7 ) KEAsigma = atof(line.c_str());
      if( i == 8 ) thetaMean = atof(line.c_str());
      if( i == 9 ) thetaSigma = atof(line.c_str());
      if( i == 10 ) xBeam = atof(line.c_str());
      if( i == 11 ) yBeam = atof(line.c_str());
      if( i == 12 ) numEvent = atoi(line.c_str());
      if( i == 13 ) {
        if( line.compare("false") == 0 ) isTargetScattering = false;
        if( line.compare("true")  == 0 ) isTargetScattering = true;
      }
      if( i == 14 ) density = atof(line.c_str());
      if( i == 15 ) targetThickness = atof(line.c_str());
      if( i == 16 ) stoppingPowerForA = line;
      if( i == 17 ) stoppingPowerForb = line;
      if( i == 18 ) stoppingPowerForB = line;
      if( i == 19 ) {
        if( line.compare("false") == 0 ) isDecay = false;
        if( line.compare("true")  == 0 ) isDecay = true;
      }
      if( i == 20 ) decayA = atoi(line.c_str());
      if( i == 21 ) decayZ = atoi(line.c_str());
      if( i == 22 ) {
        if( line.compare("false") == 0 ) isReDo = false;
        if( line.compare("true" ) == 0 ) isReDo = true;
      }

      if( i == 23) ExAList[0] = atof(line.c_str());
      if( i == 24) {
         isFromOutSide = line.compare("Out") == 0 ? true : false;
      }
      i = i + 1;
    }
    cFile.close();
  }
  //=============================================================
  //=============================================================
  //=============================================================
  //===== Set Reaction
  TransferReaction reaction;
  int AB = AA+Aa-Ab, zB = zA+za-zb;
  reaction.SetA(AA,zA);
  reaction.Seta(Aa,za);
  reaction.Setb(Ab,zb);
  reaction.SetB(AB,zB);
  reaction.SetIncidentEnergyAngle(KEAmean, 0, 0);
  reaction.CalReactionConstant();

  printf("*****************************************************************\n");
  printf("*\e[1m\e[33m        %27s                            \e[0m*\n", reaction.GetReactionName().Data());
  printf("*****************************************************************\n");
  printf("----- loading reaction setting from %s. \n", basicConfig.c_str());
  printf("\e[32m#################################### Beam \e[0m\n");
  if( ExAList[0] != 0 ) printf("    Beam Ex: %7.4f MeV\n", ExAList[0]);
  printf("         KE: %7.4f +- %5.4f MeV/u, dp/p = %5.2f %% \n", KEAmean, KEAsigma, KEAsigma/KEAmean * 50.);
  printf("      theta: %7.4f +- %5.4f MeV/u \n", thetaMean, thetaSigma);
  printf("offset(x,y): %7.4f, %7.4f mm \n", xBeam, yBeam);
  printf("   Q-value : %7.4f MeV \n", reaction.GetQValue() );
  printf("     Max Ex: %7.4f MeV \n", reaction.GetMaxExB() );

  //======== Set HELIOS
  printf("\e[32m#################################### HELIOS configuration\e[0m\n");   
  HELIOS helios;
  bool sethelios = helios.SetDetectorGeometry(heliosDetGeoFile);
  if( !sethelios){
    helios.OverrideMagneticField(BField);
    helios.OverrideMagneticFieldDirection(BFieldTheta);
    printf("======== B-field : %5.2f T, Theta : %6.2f deg\n", BField, BFieldTheta);
  }
  helios.SetDetectorOutside(isFromOutSide);
  //helios.SetCoincidentWithRecoil(isCoincidentWithRecoil);
  int mDet = helios.GetNumberOfDetectorsInSamePos();

  bool coinRecoil = helios.GetCoincidentWithRecoil();
  printf(" is Coincident with Recoil and Array ? %s\n", coinRecoil ? "Yes" : "No");

  helios.SetBeamPosition(xBeam, yBeam);
  
  eSigma = helios.GetDetEnergyResol();
  zSigma = helios.GetDetPositionResol();
  
  printf("  energy resol.: %f MeV\n", eSigma);
  printf("   pos-Z resol.: %f mm \n", zSigma);
  printf("=================================== Auxillary Detectors\n");
  
  double zElum1 = helios.GetElum1Pos();
  double zElum2 = helios.GetElum2Pos();
  double zRecoil1 = helios.GetRecoil1Pos();
  double zRecoil2 = helios.GetRecoil2Pos();
  
  if( zElum1 != 0 )   printf("   Elum 1 pos.: %f mm \n", zElum1);
  if( zElum2 != 0 )   printf("   Elum 2 pos.: %f mm \n", zElum2);
  if( zRecoil1 != 0 ) printf(" Recoil 1 pos.: %f mm \n", zRecoil1);
  if( zRecoil2 != 0 ) printf(" Recoil 2 pos.: %f mm \n", zRecoil2);

  printf("==================================== E-Z plot slope\n");
  double beta = reaction.GetReactionBeta() ;
  double gamma = reaction.GetReactionGamma();
  double mb = reaction.GetMass_b();
  double pCM = reaction.GetMomentumbCM();
  double q = TMath::Sqrt(mb*mb + pCM*pCM);
  double slope = 299.792458 * zb * abs(helios.GetBField()) / TMath::TwoPi() * beta / 1000.; // MeV/mm
  printf("                       e-z slope : %f MeV/mm\n", slope);   
  double intercept = q/gamma - mb; // MeV
  printf("    e-z intercept (ground state) : %f MeV\n", intercept); 

  //============ save reaction.dat
  if( filename != "" ) {
    FILE * keyParaOut;
    keyParaOut = fopen (filename.Data(), "w+");

    printf("=========== save key reaction constants to %s \n", filename.Data());

    fprintf(keyParaOut, "%-15.4f  //%s\n", reaction.GetMass_b(), "mass_b");
    fprintf(keyParaOut, "%-15d  //%s\n", zb, "charge_b");
    fprintf(keyParaOut, "%-15.8f  //%s\n", reaction.GetReactionBeta(), "betaCM");
    fprintf(keyParaOut, "%-15.4f  //%s\n", reaction.GetCMTotalEnergy(), "Ecm");
    fprintf(keyParaOut, "%-15.4f  //%s\n", reaction.GetMass_B(), "mass_B");
    fprintf(keyParaOut, "%-15.4f  //%s\n", slope/beta, "alpha=slope/beta");

    fflush(keyParaOut);
    fclose(keyParaOut);
  }
  //==== Target scattering, only energy loss
  if(isTargetScattering) printf("\e[32m#################################### Target Scattering\e[0m\n");
  TargetScattering msA;
  TargetScattering msB;
  TargetScattering msb;

  if(isTargetScattering) printf("======== Target : (thickness : %6.2f um) x (density : %6.2f g/cm3) = %6.2f ug/cm2\n", 
                                targetThickness * 1e+4, 
                                density, 
                                targetThickness * density * 1e+6);  

  if( isTargetScattering ){
    msA.LoadStoppingPower(stoppingPowerForA);
    msb.LoadStoppingPower(stoppingPowerForb);
    msB.LoadStoppingPower(stoppingPowerForB);
  }

  //======= Decay of particle-B
  Decay decay;
  if(isDecay) {
    printf("\e[32m#################################### Decay\e[0m\n");
    decay.SetMotherDaugther(AB, zB, decayA, decayZ); //decay
  }
  //======= loading excitation energy
  printf("\e[32m#################################### excitation energies\e[0m\n");
  vector<double> ExKnown;
  vector<double> ExStrength;
  vector<double> ExWidth;
  vector<double> SF;
  vector<double> y0; // intercept of e-z plot
  vector<double> kCM; // momentum of b in CM frame
  printf("----- loading excitation energy levels (%s).", excitationFile.c_str());
  ifstream file;
  file.open(excitationFile.c_str());
  string isotopeName;
  if( file.is_open() ){
    string line;
    while( getline(file, line) ){
      //printf("%s \n", line.c_str());
      if( line.substr(0,2) == "//" ) continue;
      if( line.substr(0,2) == "#=" ) break;
      
      ///------- split string
      size_t pos = 0;
      int i = 0;
      do{
        pos = line.find(" "); /// find space
        if( pos == 0 ){ ///check if it is splitter again
          line = line.substr(pos+1);
          continue;
        }

        string haha ;
        if( pos == std::string::npos ){
          haha = line;
        }else{
          haha = line.substr(0, pos);
          line = line.substr(pos);
        }

        ///check if haha is begin with space
        while( haha.substr(0, 1) == " ") haha = haha.substr(1);
        ///check if haha is end with space
        while( haha.back() == ' ') haha = haha.substr(0, haha.size()-1);
  
        if(i == 3 ) {ExWidth.push_back(atof(haha.c_str())); i++;}
        if(i == 2 ) {SF.push_back(atof(haha.c_str())); i++;}
        if(i == 1 ) {ExStrength.push_back(atof(haha.c_str())); i++;}
        if(i == 0 ) {ExKnown.push_back(atof(haha.c_str())); i++;}

      }while(pos != std::string::npos );
    }
    file.close();
    printf("... done.\n");
    int n = ExKnown.size();
    
    printf("%3s | %7s | %4s | %3s | %10s | %5s \n", "", "Ex[MeV]", "Xsec", "SF", "sigma[MeV]", "y0[MeV]");
    printf("----+---------+------+-----+------------+--------\n");
    for(int i = 0; i < n ; i++){
      reaction.SetExB(ExKnown[i]);
      reaction.CalReactionConstant();
      kCM.push_back(reaction.GetMomentumbCM());
      y0.push_back(TMath::Sqrt(mb*mb + kCM[i]*kCM[i])/gamma - mb);
      if( isDecay ) {
        TLorentzVector temp(0,0,0,0);
        int decayID = decay.CalDecay(temp, ExKnown[i], 0);
        if( decayID == 1) {
          printf("%3d | %7.2f | %4.2f | %3.1f |   %5.3f    | %5.2f --> Decay. \n", i, ExKnown[i], ExStrength[i], SF[i], ExWidth[i], y0[i]);
        }else{
          printf("%3d | %7.2f | %4.2f | %3.1f |   %5.3f    | %5.2f \n", i, ExKnown[i], ExStrength[i], SF[i], ExWidth[i], y0[i]);
        }
      }else{
        printf("%3d | %7.2f | %4.2f | %3.1f |   %5.3f    | %5.2f \n", i, ExKnown[i], ExStrength[i], SF[i], ExWidth[i], y0[i]);
      }
    }
    printf("----+---------+------+-----+------------+--------\n");
  }else{
    printf("... fail ------> only ground state.\n");
    ExKnown.push_back(0.0);
    ExStrength.push_back(1.0);
    ExWidth.push_back(0.0);
    reaction.SetExB(ExKnown[0]);
    reaction.CalReactionConstant();
    kCM.push_back(reaction.GetMomentumbCM());
    y0.push_back(TMath::Sqrt(mb*mb + kCM[0]*kCM[0])/gamma - mb);
  }
  
  //---- create Ex-distribution
  TF1 * exDist = NULL;
  if( ExKnown.size() > 1 ) {
    printf("---- creating Ex-distribution \n");
    int exSize = ExKnown.size();
    exDist = new TF1("exDist", exDistFunc, 0, exSize, exSize);
    for(int i = 0; i < exSize; i++){
      exDist->SetParameter(i, ExStrength[i]*SF[i]);
    }
  }
  
  //======== Load DWBAroot for thetaCM distribution
  printf("\e[32m#################################### Load DWBA input : %s  \e[0m\n", ptolemyRoot.Data());
  TF1 * dist = NULL;
  TFile * distFile = new TFile(ptolemyRoot, "read");
  TObjArray * distList = NULL;
  if( distFile->IsOpen() ) {
    distList = (TObjArray *) distFile->FindObjectAny("pList"); // the function List
    int distSize = distList->GetLast() + 1;
    if( distSize != ExKnown.size() ) {
      printf(" The number of distribution from Ptolmey Calculation is not equal to number of Ex input \n"); 
      printf("   --> the Ptolmey calculation is probably not matched with Ex input.\n"); 
      printf(" .... not use DWBA input.  \n"); 
      distFile->Close();
    }
  }else{
    printf("------- no DWBA input. \n");
  }

  //====================== build tree
  printf("\e[32m#################################### building Tree in %s\e[0m\n", saveFileName.Data());
  TFile * saveFile = new TFile(saveFileName, "recreate");
  TTree * tree = new TTree("tree", "tree");

  TMacro config(basicConfig.c_str());
  TMacro detGeo(heliosDetGeoFile.c_str());
  TMacro exList(excitationFile.c_str());
  TMacro reactionData(filename.Data());
  TString str;
  str.Form("%s @ %.2f MeV/u", reaction.GetReactionName_Latex().Data(), KEAmean);
  config.SetName(str.Data());
  config.Write("reactionConfig");
  detGeo.Write("detGeo");
  exList.Write("ExList");
  reactionData.Write("reactionData");
  
  if( distList != NULL ) distList->Write("DWBA", 1);
  
  TMacro hitMeaning;
  str = "=======================meaning of Hit ID\n"; hitMeaning.AddLine(str.Data());
  str = "   1 = light recoil hit array & heavy recoil hit recoil\n"; hitMeaning.AddLine(str.Data());
  str = "   0 = no detector\n"; hitMeaning.AddLine(str.Data());
  str = "  -1 = light recoil go opposite side of array\n"; hitMeaning.AddLine(str.Data());
  str = "  -2 = light recoil hit > det width\n"; hitMeaning.AddLine(str.Data());
  str = "  -3 = light recoil hit > array \n"; hitMeaning.AddLine(str.Data());
  str = "  -4 = light recoil hit blocker \n"; hitMeaning.AddLine(str.Data());
  str = " -10 = light recoil orbit radius too big  \n"; hitMeaning.AddLine(str.Data());
  str = " -11 = light recoil orbit radius too small\n"; hitMeaning.AddLine(str.Data());
  str = " -12 = when reocol at the same side of array, light recoil blocked by recoil detector\n"; hitMeaning.AddLine(str.Data());
  str = " -13 = more than 3 loops\n"; hitMeaning.AddLine(str.Data());
  str = " -14 = heavy recoil did not hit recoil  \n"; hitMeaning.AddLine(str.Data());
  str = " -15 = cannot find hit on array\n"; hitMeaning.AddLine(str.Data());
  str = " -20 = unknown\n"; hitMeaning.AddLine(str.Data());
  str = "===========================================\n"; hitMeaning.AddLine(str.Data());
  
  hitMeaning.Write("hitMeaning");

  int hit; // the output of Helios.CalHit
  tree->Branch("hit", &hit, "hit/I");
  
  double thetab, phib, Tb;
  double thetaB, phiB, TB;
  tree->Branch("thetab", &thetab, "thetab/D");
  tree->Branch("phib", &phib, "phib/D");
  tree->Branch("Tb", &Tb, "Tb/D");
  tree->Branch("thetaB", &thetaB, "thetaB/D");
  tree->Branch("phiB", &phiB, "phiB/D");
  tree->Branch("TB", &TB, "TB/D");
  
  double thetaCM;
  tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
  
  double e, z, detX, t;
  tree->Branch("e", &e, "energy_light/D");
  tree->Branch("x", &detX, "detector_x/D");
  tree->Branch("z", &z, "array_hit_z/D");
  double z0; tree->Branch("z0", &z0, "z-cycle/D");
  tree->Branch("t", &t, "cycle_time_light/D");
  double tB; tree->Branch("tB", &tB, "recoil_hit_time/D");   /// hit time for recoil on the recoil detector
  
  int loop, detID, detRowID;
  tree->Branch("detID", &detID, "detID/I");
  tree->Branch("detRowID", &detRowID, "detRowID/I");
  tree->Branch("loop", &loop, "loop/I");
  
  double rho; //orbit radius
  tree->Branch("rho", &rho, "orbit_radius_light/D");
  
  int ExAID;
  double ExA;
  tree->Branch("ExAID", &ExAID, "ExAID/I");
  tree->Branch("ExA", &ExA, "ExA/D");

  int ExID;
  double Ex;
  tree->Branch("ExID", &ExID, "ExID/I");
  tree->Branch("Ex", &Ex, "Ex/D");

  double ExCal, thetaCMCal;
  tree->Branch("ExCal", &ExCal, "ExCal/D");
  tree->Branch("thetaCMCal", &thetaCMCal, "thetaCMCal/D");

  double KEA, theta, phi;
  tree->Branch("beamTheta", &theta, "beamTheta/D");
  tree->Branch("beamPhi", &phi, "beamPhi/D");
  tree->Branch("beamKEA", &KEA, "beamKEA/D");
  
  double TbLoss; // energy loss of particle-b from target scattering
  double KEAnew; //beam energy after target scattering
  double depth; // reaction depth;
  double Ecm;
  if( isTargetScattering ){
    tree->Branch("depth", &depth, "depth/D");
    tree->Branch("TbLoss", &TbLoss, "TbLoss/D");
    tree->Branch("KEAnew", &KEAnew, "KEAnew/D");
    tree->Branch("Ecm", &Ecm, "Ecm/D");
  }

  double decayTheta; // the change of thetaB due to decay
  double xRecoil_d;
  double yRecoil_d;
  double rhoRecoil_d;
  double Td;
  if( isDecay ) {
    tree->Branch("decayTheta", &decayTheta, "decayTheta/D");
    tree->Branch("xRecoil_d", &xRecoil_d, "xRecoil_d/D");
    tree->Branch("yRecoil_d", &yRecoil_d, "yRecoil_d/D");
    tree->Branch("rhoRecoil_d", &rhoRecoil_d, "rhoRecoil_d/D");
    tree->Branch("Td", &Td, "Td/D");
  }
  
  double xArray, yArray, rhoArray; //x, y, rho positon of particle-b on PSD
  tree->Branch("xArray", &xArray, "xArray/D");
  tree->Branch("yArray", &yArray, "yArray/D");
  tree->Branch("rhoArray", &rhoArray, "rhoArray/D");
  
  double xRecoil, yRecoil, rhoRecoil; // x, y, rho position of particle-B on recoil-detector
  tree->Branch("xRecoil", &xRecoil, "xRecoil/D");
  tree->Branch("yRecoil", &yRecoil, "yRecoil/D");
  tree->Branch("rhoRecoil", &rhoRecoil, "rhoRecoil/D");
  
  //in case need ELUM
  double xElum1, yElum1, rhoElum1;
  if( zElum1 != 0 ) {
    tree->Branch("xElum1", &xElum1, "xElum1/D");
    tree->Branch("yElum1", &yElum1, "yElum1/D");
    tree->Branch("rhoElum1", &rhoElum1, "rhoElum1/D");
  }
  
  double xElum2, yElum2, rhoElum2;
  if( zElum2 != 0 ) {
    tree->Branch("xElum2", &xElum2, "xElum2/D");
    tree->Branch("yElum2", &yElum2, "yElum2/D");
    tree->Branch("rhoElum2", &rhoElum2, "rhoElum2/D");
  }
  
  //in case need other recoil detector. 
  double xRecoil1, yRecoil1, rhoRecoil1;
  if( zRecoil1 != 0 ){
    tree->Branch("xRecoil1", &xRecoil1, "xRecoil1/D");
    tree->Branch("yRecoil1", &yRecoil1, "yRecoil1/D");
    tree->Branch("rhoRecoil1", &rhoRecoil1, "rhoRecoil1/D");
  }
  double xRecoil2, yRecoil2, rhoRecoil2;
  if( zRecoil2 != 0 ){
    tree->Branch("xRecoil2", &xRecoil2, "xRecoil2/D");
    tree->Branch("yRecoil2", &yRecoil2, "yRecoil2/D");
    tree->Branch("rhoRecoil2", &rhoRecoil2, "rhoRecoil2/D");
  }
  //======= function for e-z plot for ideal case
  printf("++++ generate functions\n");
  TObjArray * gList = new TObjArray();
  gList->SetName("Constant thetaCM lines");
  const int gxSize = 50;
  TF1 ** gx = new TF1*[gxSize];
  TString name;
  for( int i = 0; i < gxSize; i++){
    name.Form("g%d", i);     
    gx[i] = new TF1(name, "([0]*TMath::Sqrt([1]+[2]*x*x)+[5]*x)/([3]) - [4]", -1000, 1000);      
    double thetacm = i * TMath::DegToRad();
    double gS2 = TMath::Power(TMath::Sin(thetacm)*gamma,2);
    gx[i]->SetParameter(0, TMath::Cos(thetacm));
    gx[i]->SetParameter(1, mb*mb*(1-gS2));
    gx[i]->SetParameter(2, TMath::Power(slope/beta,2));
    gx[i]->SetParameter(3, 1-gS2);
    gx[i]->SetParameter(4, mb);
    gx[i]->SetParameter(5, -gS2*slope);
    gx[i]->SetNpx(1000);
    gList->Add(gx[i]);
    printf("/");
    if( i > 1 && i % 40 == 0 ) printf("\n");
  }
  gList->Write("gList", TObject::kSingleKey);
  printf(" %d constant thetaCM functions\n", gxSize);

  int n = ExKnown.size();
  TObjArray * fList = new TObjArray();
  TF1** f = new TF1*[n];
  for( int i = 0; i< n ; i++){
    name.Form("f%d", i);     
    f[i] = new TF1(name, "[0] + [1] * x", -1000, 1000);      
    f[i]->SetParameter(0, y0[i]);
    f[i]->SetParameter(1, slope);
    f[i]->SetNpx(1000);
    fList->Add(f[i]);
    printf(".");
  }
  fList->Write("fList", TObject::kSingleKey);
  printf(" %d e-z infinte-small detector functions\n", n);
  
  //--- cal modified f
  TObjArray * fxList = new TObjArray();
  TGraph ** fx = new TGraph*[n];
  for( int j = 0 ; j < n; j++){
    double px[100];
    double py[100];
    double a = helios.GetDetRadius();
    double q = TMath::Sqrt(mb*mb + kCM[j] * kCM[j] );
    for(int i = 0; i < 100; i++){
      double thetacm = TMath::Pi()/TMath::Log(100) * (TMath::Log(100) - TMath::Log(100-i)) ;//using log scale, for more point in small angle.
      double temp = TMath::TwoPi() * slope / beta / kCM[j] * a / TMath::Sin(thetacm); 
      px[i] = beta /slope * (gamma * beta * q - gamma * kCM[j] * TMath::Cos(thetacm)) * (1 - TMath::ASin(temp)/TMath::TwoPi()) ;
      py[i] = gamma * q - mb - gamma * beta * kCM[j] * TMath::Cos(thetacm);   
    }

    fx[j] = new TGraph(100, px, py);
    name.Form("fx%d", j);
    fx[j]->SetName(name);
    fx[j]->SetLineColor(4);
    fxList->Add(fx[j]);
    printf(",");
  }
  fxList->Write("fxList", TObject::kSingleKey);
  printf(" %d e-z finite-size detector functions\n", n);

  //--- cal modified thetaCM vs z
  TObjArray * txList = new TObjArray();
  TGraph ** tx = new TGraph*[n];
  for( int j = 0 ; j < n; j++){
    double px[100];
    double py[100];
    double a = helios.GetDetRadius();
    double q = TMath::Sqrt(mb*mb + kCM[j] * kCM[j] );
    for(int i = 0; i < 100; i++){
      double thetacm = (i + 8.) * TMath::DegToRad();
      double temp = TMath::TwoPi() * slope / beta / kCM[j] * a / TMath::Sin(thetacm); 
      px[i] = beta /slope * (gamma * beta * q - gamma * kCM[j] * TMath::Cos(thetacm)) * (1 - TMath::ASin(temp)/TMath::TwoPi());
      py[i] = thetacm * TMath::RadToDeg();   
    }

    tx[j] = new TGraph(100, px, py);
    name.Form("tx%d", j);
    tx[j]->SetName(name);
    tx[j]->SetLineColor(4);
    txList->Add(tx[j]);
    printf("*");
  }
  txList->Write("txList", TObject::kSingleKey);
  printf(" %d thetaCM-z for finite-size detector functions\n", n);
  
  //========timer
  TBenchmark clock;
  bool shown ;   
  clock.Reset();
  clock.Start("timer");
  shown = false;
  
  //change the number of event into human easy-to-read form
  int digitLen = TMath::Floor(TMath::Log10(numEvent));
  TString numEventStr;
  if( 3 <= digitLen && digitLen < 6 ){
    numEventStr.Form("%5.1f kilo", numEvent/1000.);
  }else if ( 6<= digitLen && digitLen < 9 ){
    numEventStr.Form("%6.2f million", numEvent/1e6);    
  }else if ( 9<= digitLen ){
    numEventStr.Form("%6.2f billion", numEvent/1e9);    
  }
  printf("\e[32m#################################### generating %s events \e[0m\n", numEventStr.Data());

  //====================================================== calculate event
  int count = 0;
  for( int i = 0; i < numEvent; i++){
    bool redoFlag = true;
    if( !isReDo ) redoFlag = false;
    do{

      //==== Set Ex of A
      ExAID = gRandom->Integer(nExA);
      ExA = ExAList[ExAID];
      reaction.SetExA(ExA);

      //==== Set Ex of B
      if( ExKnown.size() == 1 ) {
        ExID = 0;
        Ex = ExKnown[0] + gRandom->Gaus(0, ExWidth[0]);
      }else{
        ExID = exDist->GetRandom();
        Ex = ExKnown[ExID]+ gRandom->Gaus(0, ExWidth[ExID]);
      }
      reaction.SetExB(Ex);
  
      //==== Set incident beam
      if( KEAsigma == 0 ){
        KEA = KEAmean;
      }else{
        KEA = gRandom->Gaus(KEAmean, KEAsigma);
      }
      if( thetaSigma == 0 ){
        theta = thetaMean;
      }else{
        theta = gRandom->Gaus(thetaMean, thetaSigma);
      }
      
      //==== for taregt scattering
      reaction.SetIncidentEnergyAngle(KEA, theta, 0.);
      reaction.CalReactionConstant();
      TLorentzVector PA = reaction.GetPA();            

      //depth = 0;
      if( isTargetScattering ){
        //==== Target scattering, only energy loss
        depth = targetThickness * gRandom->Rndm();
        msA.SetTarget(density, depth); 
        TLorentzVector PAnew = msA.Scattering(PA);
        KEAnew = msA.GetKE()/AA;
        reaction.SetIncidentEnergyAngle(KEAnew, theta, phi);
        reaction.CalReactionConstant();
        Ecm = reaction.GetCMTotalKE();
      }

      //==== Calculate thetaCM, phiCM
      if( distFile->IsOpen()){
        dist = (TF1 *) distList->At(ExID);
        thetaCM = dist->GetRandom() / 180. * TMath::Pi();
      }else{
        thetaCM = TMath::ACos(2 * gRandom->Rndm() - 1) ; 
      }

      double phiCM = TMath::TwoPi() * gRandom->Rndm(); 

      //==== Calculate reaction
      TLorentzVector * output = reaction.Event(thetaCM, phiCM);
      TLorentzVector Pb = output[2];
      TLorentzVector PB = output[3];

    //==== Calculate energy loss of scattered and recoil in target
    if( isTargetScattering ){
      if( Pb.Theta() < TMath::PiOver2() ){
        msb.SetTarget(density, targetThickness - depth);
      }else{
        msb.SetTarget(density, depth);
      }
      Pb = msb.Scattering(Pb);
      TbLoss = msb.GetKELoss();
      msB.SetTarget(density, targetThickness - depth);
      PB = msB.Scattering(PB);
    }else{
      TbLoss = 0;
    }

    //======= Decay of particle-B
    int decayID = 0;
    int new_zB  = decayZ;
    if( isDecay){
      decayID = decay.CalDecay(PB, Ex, 0); // decay to ground state
      if( decayID == 1 ){
        PB = decay.GetDaugther_D();
        decayTheta = decay.GetAngleChange();
        new_zB = decayZ;
      }else{
        decayTheta = TMath::QuietNaN();
      }
    }

    //################################### tree branches
    //===== reaction
    thetab = Pb.Theta() * TMath::RadToDeg();
    thetaB = PB.Theta() * TMath::RadToDeg();

    Tb = Pb.E() - Pb.M();
    TB = PB.E() - PB.M();

    phib = Pb.Phi() * TMath::RadToDeg();
    phiB = PB.Phi() * TMath::RadToDeg();

    //==== Helios
    
    ///printf(" thetaCM : %f \n", thetaCM * TMath::RadToDeg());
    
    if( Tb > 0  || TB > 0 ){
      helios.CalArrayHit(Pb, zb);
      helios.CalRecoilHit(PB, new_zB);
      hit = 2;
      while( hit > 1 ){ hit = helios.DetAcceptance(); }
      
      trajectory orb_b = helios.GetTrajectory_b();
      trajectory orb_B = helios.GetTrajectory_B();
      
      e = helios.GetEnergy() + gRandom->Gaus(0, eSigma);

      double ranX = gRandom->Gaus(0, zSigma);
      z = orb_b.z + ranX;
      detX = helios.GetDetX() + ranX;
 
      z0 = orb_b.z0;
      t = orb_b.t;
      loop = orb_b.loop;
      detID = orb_b.detID;
      detRowID = orb_b.detRowID;
      rho = orb_b.rho;
      rhoArray = orb_b.R;
      
      xArray = orb_b.x;
      yArray = orb_b.y;
      

      //ELUM
      if( zElum1 != 0 ){
        xElum1 = helios.GetXPos(zElum1);
        yElum1 = helios.GetYPos(zElum1);
        rhoElum1 = helios.GetR(zElum1);
      }
      if( zElum2 != 0 ){
        xElum2 = helios.GetXPos(zElum2);
        yElum2 = helios.GetYPos(zElum2);
        rhoElum2 = helios.GetR(zElum2);
      }

      //Recoil
      rhoRecoil = orb_B.R;
      tB = orb_B.t;
      xRecoil = orb_B.x;
      yRecoil = orb_B.y;

      //other recoil detectors
      if ( zRecoil1 != 0 ){
        xRecoil1 = helios.GetRecoilXPos(zRecoil1);
        yRecoil1 = helios.GetRecoilYPos(zRecoil1);
        rhoRecoil1 = helios.GetRecoilR(zRecoil1);
      }
      if ( zRecoil2 != 0 ){
        xRecoil2 = helios.GetRecoilXPos(zRecoil2);
        yRecoil2 = helios.GetRecoilYPos(zRecoil2);
        rhoRecoil2 = helios.GetRecoilR(zRecoil2);
      }
      
      reaction.CalExThetaCM(e, z, helios.GetBField(), helios.GetDetRadius());
      ExCal = reaction.GetEx();
      thetaCMCal = reaction.GetThetaCM();

      //change thetaCM into deg
      thetaCM = thetaCM * TMath::RadToDeg();
      
      //if decay, get the light decay particle on the recoil;
      if( isDecay ){
        if( decayID == 1 ){
          TLorentzVector Pd = decay.GetDaugther_d();
          
          Td = Pd.E() - Pd.M();
          
          helios.CalRecoilHit(Pd, decayZ);
          
          trajectory orb_d = helios.GetTrajectory_B();
          rhoRecoil_d = orb_d.R;
          xRecoil_d = orb_d.x;
          yRecoil_d = orb_d.y;
          
        }else{
          rhoRecoil_d = TMath::QuietNaN();
          xRecoil_d = TMath::QuietNaN();
          yRecoil_d = TMath::QuietNaN();
        }
      }
    
    }else{
      hit = -404;
    }

    if( hit == 1)  count ++;

    if( isReDo ){
      if( hit == 1) {
        redoFlag = false;
      }else{
        redoFlag = true;
        //printf("%d, %2d, thetaCM : %f, theta : %f, z0: %f \n", i, hit, thetaCM * TMath::RadToDeg(), thetab, helios.GetZ0());
      }
    }else{
      redoFlag = false;
    }

    }while( redoFlag );
    tree->Fill();

    //#################################################################### Timer  
    clock.Stop("timer");
    Double_t time = clock.GetRealTime("timer");
    clock.Start("timer");

    if ( !shown ) {
      if (fmod(time, 10) < 1 ){
        printf( "%10d[%2d%%]| %8.2f sec | expect: %5.1f min \n", i, TMath::Nint((i+1)*100./numEvent), time , numEvent*time/(i+1)/60);
        shown = 1;
      }
    }else{
      if (fmod(time, 10) > 9 ){
        shown = 0;
      }
    }
    
  }

  saveFile->Write();
  saveFile->Close();
  
  distFile->Close();

  printf("=============== done. saved as %s. count(hit==1) : %d\n", saveFileName.Data(), count);
  //gROOT->ProcessLine(".q");
}
