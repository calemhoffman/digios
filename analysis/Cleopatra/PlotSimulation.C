
#include <fstream>
#include <stdlib.h>
#include "../Armory/Check_Simulation.C"

using namespace std;

int main (int argc, char *argv[]) {
   
  printf("=================================================================\n");
  printf("=================== Plot Simulation Canvas ======================\n");
  printf("=================================================================\n");

  if(argc < 2 ) { 
    printf("Usage: ./PlotSimulation input_root_file [config]\n");
    exit(0); 
  }else{
    printf("ROOT file : %s \n", argv[1]);
  }

  string rootFile = argv[1];
  string config = "../Armory/Check_Simulation_Config.txt";
  if( argc >= 3 ) config = argv[2];

  printf("Config File : %s \n", config.c_str());

  Int_t padSize = 500;

  Check_Simulation(rootFile, config, padSize, true);

} 
