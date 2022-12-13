CC=g++

all: Isotope InFileCreator ExtractXSec ExtractXSecFromText PlotTGraphTObjArray FindThetaCM Transfer PlotSimulation IsotopeShort

#Cleopatra: Cleopatra.C ../Simulation/Isotope.h ../Simulation/constant.h potentials.h InFileCreator.h ExtractXSec.h PlotTGraphTObjArray.h
#	$(CC) Cleopatra.C -o Cleopatra `root-config --cflags --glibs`

InFileCreator: InFileCreator.C InFileCreator.h ../Cleopatra/Isotope.h ../Cleopatra/constant.h potentials.h
	$(CC) InFileCreator.C -o InFileCreator `root-config --cflags --glibs`

ExtractXSec: ExtractXSec.C ExtractXSec.h
	$(CC) ExtractXSec.C -o ExtractXSec `root-config --cflags --glibs`

ExtractXSecFromText: ExtractXSecFromText.C ExtractXSec.h
	$(CC) ExtractXSecFromText.C -o ExtractXSecFromText `root-config --cflags --glibs`

PlotTGraphTObjArray: PlotTGraphTObjArray.C PlotTGraphTObjArray.h
	$(CC) PlotTGraphTObjArray.C -o PlotTGraphTObjArray `root-config --cflags --glibs`

FindThetaCM: FindThetaCM.C FindThetaCM.h ../Cleopatra/HELIOS_LIB.h ../Cleopatra/Isotope.h ../Cleopatra/constant.h
	$(CC) FindThetaCM.C -o FindThetaCM `root-config --cflags --glibs`

Transfer: Transfer.C Transfer.h ../Cleopatra/HELIOS_LIB.h ../Cleopatra/Isotope.h ../Cleopatra/constant.h
	$(CC) Transfer.C -o Transfer `root-config --cflags --glibs`

PlotSimulation: PlotSimulation.C ../Armory/Check_Simulation.C
	$(CC) PlotSimulation.C -o PlotSimulation `root-config --cflags --glibs`

Isotope: ../Cleopatra/Isotope.h ../Cleopatra/Isotope.C
	$(CC) Isotope.C -o Isotope
	
IsotopeShort: ../Cleopatra/Isotope.h ../Cleopatra/IsotopeShort.C
	$(CC) IsotopeShort.C -o IsotopeShort
