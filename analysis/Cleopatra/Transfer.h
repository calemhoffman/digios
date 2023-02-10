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

#include "HELIOS_LIB.h"

double exDistFunc(Double_t *x, Double_t * par){
  return par[(int) x[0]];
}

void Transfer(
         string      basicConfig = "reactionConfig.txt", 
         string heliosDetGeoFile = "detectorGeo.txt", 
         string   excitationFile = "Ex.txt", ///when no file, only ground state
         TString     ptolemyRoot = "DWBA.root", /// when no file, use isotropic distribution of thetaCM
         TString    saveFileName = "transfer.root",
         TString        filename = "reaction.dat"){ /// when no file, no output.

  //############################################# Set Reaction
  TransferReaction reaction;
  reaction.SetReactionFromFile(basicConfig);

  printf("*****************************************************************\n");
  printf("*\e[1m\e[33m        %27s                            \e[0m*\n", reaction.GetReactionName().Data());
  printf("*****************************************************************\n");
  printf("----- loading reaction setting from %s. \n", basicConfig.c_str());
  printf("\e[32m#################################### Beam \e[0m\n");

  const ReactionConfig reactionConfig = reaction.GetRectionConfig();

  PrintReactionConfig(reactionConfig);

  vector<float> ExAList = reactionConfig.beamEx;
  int nExA = (int) ExAList.size();

  //############################################# Set HELIOS
  printf("\e[32m#################################### HELIOS configuration\e[0m\n");   
  HELIOS helios;
  helios.SetDetectorGeometry(heliosDetGeoFile);

  const DetGeo detGeo = helios.GetDetectorGeometry();
  
  printf("==================================== E-Z plot slope\n");
  double  betaRect = reaction.GetReactionBeta() ;
  double gamma = reaction.GetReactionGamma();
  double    mb = reaction.GetMass_b();
  double   pCM = reaction.GetMomentumbCM();
  double     q = TMath::Sqrt(mb*mb + pCM*pCM); ///energy of light recoil in center of mass
  double slope = 299.792458 * reaction.GetCharge_b() * abs(helios.GetBField()) / TMath::TwoPi() * betaRect / 1000.; /// MeV/mm
  printf("                       e-z slope : %f MeV/mm\n", slope);   
  double intercept = q/gamma - mb; // MeV
  printf("    e-z intercept (ground state) : %f MeV\n", intercept); 

  //############################################# save reaction.dat
  if( filename != "" ) {
    FILE * keyParaOut;
    keyParaOut = fopen (filename.Data(), "w+");

    printf("=========== save key reaction constants to %s \n", filename.Data());
    fprintf(keyParaOut, "%-15.4f  //%s\n", reaction.GetMass_b(), "mass_b");
    fprintf(keyParaOut, "%-15d  //%s\n",   reaction.GetCharge_b(), "charge_b");
    fprintf(keyParaOut, "%-15.8f  //%s\n", reaction.GetReactionBeta(), "betaCM");
    fprintf(keyParaOut, "%-15.4f  //%s\n", reaction.GetCMTotalEnergy(), "Ecm");
    fprintf(keyParaOut, "%-15.4f  //%s\n", reaction.GetMass_B(), "mass_B");
    fprintf(keyParaOut, "%-15.4f  //%s\n", slope/betaRect, "alpha=slope/betaRect");

    fflush(keyParaOut);
    fclose(keyParaOut);
  }
  
  //############################################# Target scattering, only energy loss
  bool isTargetScattering = reactionConfig.isTargetScattering;
  float           density = reactionConfig.targetDensity;
  float   targetThickness = reactionConfig.targetThickness;
  
  if(isTargetScattering) printf("\e[32m#################################### Target Scattering\e[0m\n");
  TargetScattering msA;
  TargetScattering msB;
  TargetScattering msb;

  if(reactionConfig.isTargetScattering) printf("======== Target : (thickness : %6.2f um) x (density : %6.2f g/cm3) = %6.2f ug/cm2\n", 
                                               targetThickness * 1e+4, 
                                               density, 
                                               targetThickness * density * 1e+6);  

  if( reactionConfig.isTargetScattering ){
    msA.LoadStoppingPower(reactionConfig.beamStoppingPowerFile);
    msb.LoadStoppingPower(reactionConfig.recoilLightStoppingPowerFile);
    msB.LoadStoppingPower(reactionConfig.recoilHeavyStoppingPowerFile);
  }

  //############################################# Decay of particle-B
  Decay decay;
  if(reactionConfig.isDecay) {
    printf("\e[32m#################################### Decay\e[0m\n");
    decay.SetMotherDaugther(reactionConfig.recoilHeavyA,
                            reactionConfig.recoilHeavyZ,
                            reactionConfig.heavyDecayA,
                            reactionConfig.heavyDecayZ);
  }
  //############################################# loading excitation energy
  printf("\e[32m#################################### excitation energies\e[0m\n");
  vector<double> ExKnown;
  vector<double> ExStrength;
  vector<double> ExWidth;
  vector<double> SF;
  vector<double> y0; /// intercept of e-z plot
  vector<double> kCM; /// momentum of b in CM frame
  printf("----- loading excitation energy levels (%s).", excitationFile.c_str());
  ifstream file;
  file.open(excitationFile.c_str());
  string isotopeName;
  if( file.is_open() ){
    string line;
    while( getline(file, line) ){
      ///printf("%s \n", line.c_str());
      if( line.substr(0,2) == "//" ) continue;
      if( line.substr(0,2) == "#=" ) break;

      vector<string> str = SplitStr(line, " ");

      ExKnown.push_back(atof(str[0].c_str()));
      ExStrength.push_back(atof(str[1].c_str()));
      SF.push_back(atof(str[2].c_str()));
      ExWidth.push_back(atof(str[3].c_str()));

    }
    file.close();
    printf("... done.\n");
    int n = (int) ExKnown.size();
    
    printf("%3s | %7s | %5s | %3s | %10s | %5s \n", "", "Ex[MeV]", "Xsec", "SF", "sigma[MeV]", "y0[MeV]");
    printf("----+---------+------+-----+------------+--------\n");
    for(int i = 0; i < n ; i++){
      reaction.SetExB(ExKnown[i]);
      reaction.CalReactionConstant();
      kCM.push_back(reaction.GetMomentumbCM());
      y0.push_back(TMath::Sqrt(mb*mb + kCM[i]*kCM[i])/gamma - mb);
      if( reactionConfig.isDecay ) {
        TLorentzVector temp(0,0,0,0);
        int decayID = decay.CalDecay(temp, ExKnown[i], 0);
        if( decayID == 1) {
          printf("%3d | %7.2f | %5.2f | %3.1f |   %5.3f    | %5.2f --> Decay. \n", i, ExKnown[i], ExStrength[i], SF[i], ExWidth[i], y0[i]);
        }else{
          printf("%3d | %7.2f | %5.2f | %3.1f |   %5.3f    | %5.2f \n", i, ExKnown[i], ExStrength[i], SF[i], ExWidth[i], y0[i]);
        }
      }else{
        printf("%3d | %7.2f | %5.2f | %3.1f |   %5.3f    | %5.2f \n", i, ExKnown[i], ExStrength[i], SF[i], ExWidth[i], y0[i]);
      }
    }
    printf("----+---------+-------+-----+------------+--------\n");
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
  
  //############################################# Load DWBAroot for thetaCM distribution
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

  //############################################# build tree
  printf("\e[32m#################################### building Tree in %s\e[0m\n", saveFileName.Data());
  TFile * saveFile = new TFile(saveFileName, "recreate");
  TTree * tree = new TTree("tree", "tree");

  TMacro config(basicConfig.c_str());
  TMacro detGeoTxt(heliosDetGeoFile.c_str());
  TMacro exList(excitationFile.c_str());
  TMacro reactionData(filename.Data());
  double KEAmean = reactionConfig.beamEnergy;
  TString str;
  str.Form("%s @ %.2f MeV/u", reaction.GetReactionName_Latex().Data(), KEAmean);
  config.SetName(str.Data());
  config.Write("reactionConfig");
  detGeoTxt.Write("detGeo");
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

  int hit; /// the output of Helios.CalHit
  tree->Branch("hit", &hit, "hit/I");
  
  double thetab, phib, Tb;
  double thetaB, phiB, TB;
  tree->Branch("thetab", &thetab, "thetab/D");
  tree->Branch("phib",     &phib, "phib/D");
  tree->Branch("Tb",         &Tb, "Tb/D");
  tree->Branch("thetaB", &thetaB, "thetaB/D");
  tree->Branch("phiB",     &phiB, "phiB/D");
  tree->Branch("TB",         &TB, "TB/D");
  
  double thetaCM;
  tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
  
  double e, z, detX, t, z0, tB;
  tree->Branch("e",    &e, "energy_light/D");
  tree->Branch("x", &detX, "detector_x/D");
  tree->Branch("z",    &z, "array_hit_z/D");
  tree->Branch("z0",  &z0, "z-cycle/D");
  tree->Branch("t",    &t, "cycle_time_light/D");
  tree->Branch("tB",  &tB, "recoil_hit_time/D");   /// hit time for recoil on the recoil detector
  
  int loop, detID, detRowID;
  tree->Branch("detID",       &detID, "detID/I");
  tree->Branch("detRowID", &detRowID, "detRowID/I");
  tree->Branch("loop",         &loop, "loop/I");
  
  double rho, rhoB; ///orbit radius
  tree->Branch("rho",   &rho, "orbit_radius_light/D");
  tree->Branch("rhoB", &rhoB, "orbit_radius_heavy/D");
  
  int ExAID;
  double ExA;
  tree->Branch("ExAID", &ExAID, "ExAID/I");
  tree->Branch("ExA",     &ExA, "ExA/D");

  int ExID;
  double Ex;
  tree->Branch("ExID", &ExID, "ExID/I");
  tree->Branch("Ex",     &Ex, "Ex/D");

  double ExCal, thetaCMCal;
  tree->Branch("ExCal",           &ExCal, "ExCal/D");
  tree->Branch("thetaCMCal", &thetaCMCal, "thetaCMCal/D");

  double KEA, theta, phi;
  tree->Branch("beamTheta", &theta, "beamTheta/D");
  tree->Branch("beamPhi",     &phi, "beamPhi/D");
  tree->Branch("beamKEA",     &KEA, "beamKEA/D");
  
  double TbLoss; /// energy loss of particle-b from target scattering
  double KEAnew; ///beam energy after target scattering
  double depth;  /// reaction depth;
  double Ecm;
  if( reactionConfig.isTargetScattering ){
    tree->Branch("depth",   &depth, "depth/D");
    tree->Branch("TbLoss", &TbLoss, "TbLoss/D");
    tree->Branch("KEAnew", &KEAnew, "KEAnew/D");
    tree->Branch("Ecm",       &Ecm, "Ecm/D");
  }

  double decayTheta; /// the change of thetaB due to decay
  double xRecoil_d, yRecoil_d, rhoRecoil_d, Td;
  if( reactionConfig.isDecay ) {
    tree->Branch("decayTheta",   &decayTheta, "decayTheta/D");
    tree->Branch("xRecoil_d",     &xRecoil_d, "xRecoil_d/D");
    tree->Branch("yRecoil_d",     &yRecoil_d, "yRecoil_d/D");
    tree->Branch("rhoRecoil_d", &rhoRecoil_d, "rhoRecoil_d/D");
    tree->Branch("Td",                   &Td, "Td/D");
  }
  
  double xArray, yArray, rhoArray; ///x, y, rho positon of particle-b on PSD
  tree->Branch("xArray",     &xArray, "xArray/D");
  tree->Branch("yArray",     &yArray, "yArray/D");
  tree->Branch("rhoArray", &rhoArray, "rhoArray/D");
  
  double xRecoil, yRecoil, rhoRecoil; /// x, y, rho position of particle-B on recoil-detector
  tree->Branch("xRecoil",     &xRecoil, "xRecoil/D");
  tree->Branch("yRecoil",     &yRecoil, "yRecoil/D");
  tree->Branch("rhoRecoil", &rhoRecoil, "rhoRecoil/D");
  
  ///in case need ELUM
  double xElum1, yElum1, rhoElum1;
  if( detGeo.elumPos1 != 0 ) {
    tree->Branch("xElum1",     &xElum1, "xElum1/D");
    tree->Branch("yElum1",     &yElum1, "yElum1/D");
    tree->Branch("rhoElum1", &rhoElum1, "rhoElum1/D");
  }
  
  double xElum2, yElum2, rhoElum2;
  if( detGeo.elumPos2 != 0 ) {
    tree->Branch("xElum2",     &xElum2, "xElum2/D");
    tree->Branch("yElum2",     &yElum2, "yElum2/D");
    tree->Branch("rhoElum2", &rhoElum2, "rhoElum2/D");
  }
  
  ///in case need other recoil detector. 
  double xRecoil1, yRecoil1, rhoRecoil1;
  if( detGeo.recoilPos1 != 0 ){
    tree->Branch("xRecoil1",     &xRecoil1, "xRecoil1/D");
    tree->Branch("yRecoil1",     &yRecoil1, "yRecoil1/D");
    tree->Branch("rhoRecoil1", &rhoRecoil1, "rhoRecoil1/D");
  }
  double xRecoil2, yRecoil2, rhoRecoil2;
  if( detGeo.recoilPos2 != 0 ){
    tree->Branch("xRecoil2",     &xRecoil2, "xRecoil2/D");
    tree->Branch("yRecoil2",     &yRecoil2, "yRecoil2/D");
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
    gx[i]->SetParameter(2, TMath::Power(slope/betaRect,2));
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
  vector<double> px, py;
  int countfx = 0;
  for( int j = 0 ; j < n; j++){
    double a = helios.GetDetRadius();
    double q = TMath::Sqrt(mb*mb + kCM[j] * kCM[j] );
    px.clear();
    py.clear();
    countfx = 0;
    for(int i = 0; i < 100; i++){
      double thetacm = TMath::Pi()/TMath::Log(100) * (TMath::Log(100) - TMath::Log(100-i)) ;//using log scale, for more point in small angle.
      double temp = TMath::TwoPi() * slope / betaRect / kCM[j] * a / TMath::Sin(thetacm); 
      double pxTemp = betaRect /slope * (gamma * betaRect * q - gamma * kCM[j] * TMath::Cos(thetacm)) * (1 - TMath::ASin(temp)/TMath::TwoPi()) ;
      double pyTemp = gamma * q - mb - gamma * betaRect * kCM[j] * TMath::Cos(thetacm);   
      if( TMath::IsNaN(pxTemp) || TMath::IsNaN(pyTemp) ) continue;
      px.push_back(pxTemp);
      py.push_back(pyTemp);
      countfx ++;
    }

    fx[j] = new TGraph(countfx, &px[0], &py[0]);
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
    double a = helios.GetDetRadius();
    double q = TMath::Sqrt(mb*mb + kCM[j] * kCM[j] );
    px.clear();
    py.clear();
    countfx = 0;
    for(int i = 0; i < 100; i++){
      double thetacm = (i + 8.) * TMath::DegToRad();
      double temp = TMath::TwoPi() * slope / betaRect / kCM[j] * a / TMath::Sin(thetacm); 
      double pxTemp = betaRect /slope * (gamma * betaRect * q - gamma * kCM[j] * TMath::Cos(thetacm)) * (1 - TMath::ASin(temp)/TMath::TwoPi());
      double pyTemp = thetacm * TMath::RadToDeg();   
      if( TMath::IsNaN(pxTemp) || TMath::IsNaN(pyTemp) ) continue;
      px.push_back(pxTemp);
      py.push_back(pyTemp);
      countfx ++;
    }

    tx[j] = new TGraph(countfx, &px[0], &py[0]);
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
  int numEvent = reactionConfig.numEvents;
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
    if( !reactionConfig.isRedo ) redoFlag = false;
    do{

      //==== Set Ex of A
      ExAID = gRandom->Integer(nExA);
      ExA = ExAList[ExAID];
      reaction.SetExA(ExA);

      //==== Set Ex of B
      if( ExKnown.size() == 1 ) {
        ExID = 0;
        Ex = ExKnown[0] + (ExWidth[0] == 0 ? 0 : gRandom->Gaus(0, ExWidth[0]));
      }else{
        ExID = exDist->GetRandom();
        Ex = ExKnown[ExID]+ (ExWidth[ExID] == 0 ? 0 : gRandom->Gaus(0, ExWidth[ExID]));
      }
      reaction.SetExB(Ex);
  
      //==== Set incident beam
      KEA = reactionConfig.beamEnergy;
      if( reactionConfig.beamEnergySigma == 0 ){
        KEA = reactionConfig.beamEnergy;
      }else{
        KEA = gRandom->Gaus(reactionConfig.beamEnergy, reactionConfig.beamEnergySigma);
      }
      theta = 0.0;
      if( reactionConfig.beamAngleSigma == 0 ){
        theta = reactionConfig.beamAngle;
      }else{
        theta = gRandom->Gaus(reactionConfig.beamAngle, reactionConfig.beamAngleSigma);
      }
      phi = 0.0;
      
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
        KEAnew = msA.GetKE()/reactionConfig.beamA;
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
    int new_zB  = reactionConfig.recoilHeavyZ;
    if( reactionConfig.isDecay){
      
      //decayID = decay.CalDecay(PB, Ex, 0, phiCM + TMath::Pi()/2.); // decay to ground state
      decayID = decay.CalDecay(PB, Ex, 0, phiCM + TMath::Pi()/2); // decay to ground state
      if( decayID == 1 ){
        PB = decay.GetDaugther_D();
        //decayTheta = decay.GetAngleChange();
        decayTheta = decay.GetThetaCM();
        new_zB = reactionConfig.heavyDecayZ;
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
      helios.CalArrayHit(Pb, reaction.GetCharge_b());
      helios.CalRecoilHit(PB, new_zB);
      hit = 2;
      while( hit > 1 ){   hit = helios.DetAcceptance(); } /// while hit > 1, goto next loop;
      
      trajectory orb_b = helios.GetTrajectory_b();
      trajectory orb_B = helios.GetTrajectory_B();
      
      e = helios.GetEnergy() + gRandom->Gaus(0, detGeo.eSigma);

      double ranX = gRandom->Gaus(0, detGeo.zSigma);
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
      if( detGeo.elumPos1 != 0 ){
        xElum1   = helios.GetXPos(detGeo.elumPos1);
        yElum1   = helios.GetYPos(detGeo.elumPos1);
        rhoElum1 = helios.GetR(detGeo.elumPos1);
      }
      if( detGeo.elumPos2 != 0 ){
        xElum2   = helios.GetXPos(detGeo.elumPos2);
        yElum2   = helios.GetYPos(detGeo.elumPos2);
        rhoElum2 = helios.GetR(detGeo.elumPos2);
      }

      //Recoil
      rhoRecoil = orb_B.R;
      tB = orb_B.t;
      xRecoil = orb_B.x;
      yRecoil = orb_B.y;
      rhoB = orb_B.rho;

      //other recoil detectors
      if ( detGeo.recoilPos1 != 0 ){
        xRecoil1 = helios.GetRecoilXPos(detGeo.recoilPos1);
        yRecoil1 = helios.GetRecoilYPos(detGeo.recoilPos1);
        rhoRecoil1 = helios.GetRecoilR(detGeo.recoilPos1);
      }
      if ( detGeo.recoilPos2 != 0 ){
        xRecoil2 = helios.GetRecoilXPos(detGeo.recoilPos2);
        yRecoil2 = helios.GetRecoilYPos(detGeo.recoilPos2);
        rhoRecoil2 = helios.GetRecoilR(detGeo.recoilPos2);
      }
      
      reaction.CalExThetaCM(e, z, helios.GetBField(), helios.GetDetRadius());
      ExCal = reaction.GetEx();
      thetaCMCal = reaction.GetThetaCM();

      //change thetaCM into deg
      thetaCM = thetaCM * TMath::RadToDeg();
      
      //if decay, get the light decay particle on the recoil;
      if( reactionConfig.isDecay ){
        if( decayID == 1 ){
          TLorentzVector Pd = decay.GetDaugther_d();
          
          Td = Pd.E() - Pd.M();
          
          helios.CalRecoilHit(Pd, reactionConfig.heavyDecayZ);
          
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

    if( reactionConfig.isRedo ){
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
