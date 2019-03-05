CC=g++

all: InFileCreator ExtractXSec PlotTGraphTObjArray FindThetaCM Transfer

#Cleopatra: Cleopatra.C ../Simulation/Isotope.h ../Simulation/constant.h potentials.h InFileCreator.h ExtractXSec.h PlotTGraphTObjArray.h
#	$(CC) Cleopatra.C -o Cleopatra `root-config --cflags --glibs`

InFileCreator: InFileCreator.C InFileCreator.h ../Simulation/Isotope.h ../Simulation/constant.h potentials.h
	$(CC) InFileCreator.C -o InFileCreator `root-config --cflags --glibs`

ExtractXSec: ExtractXSec.C ExtractXSec.h
	$(CC) ExtractXSec.C -o ExtractXSec `root-config --cflags --glibs`

PlotTGraphTObjArray: PlotTGraphTObjArray.C PlotTGraphTObjArray.h
	$(CC) PlotTGraphTObjArray.C -o PlotTGraphTObjArray `root-config --cflags --glibs`

FindThetaCM: FindThetaCM.C FindThetaCM.h ../../analysis/Simulation/HELIOS_LIB.h ../Simulation/Isotope.h ../Simulation/constant.h
	$(CC) FindThetaCM.C -o FindThetaCM `root-config --cflags --glibs`

Transfer: Transfer.C Transfer.h ../../analysis/Simulation/HELIOS_LIB.h ../Simulation/Isotope.h ../Simulation/constant.h
	$(CC) Transfer.C -o Transfer `root-config --cflags --glibs`
