************************************************************************
****        This is analysis working directory      
************************************************************************

Throught this read me, I hope I can help you to understand the basic of 
the analysis.   
                              Nov 29, 2018, Ryan (goluckyryan@gmail.com)

=================================================
========= Pre-requise Knowlegde
=================================================
1) little idea of cern ROOT
2) little bit bash shell script
3) may be some C++

=================================================
========= The folder (analysis) structure is like this
=================================================
analysis
    |
    |-- GEBSort (where the GEBSort code is stored)
    |
    |-- Armory (where all the experimental-independent code is stored)
    |
    |-- root_data (where all Sorted root files should be stored)
    |
    |-- merged_data (where the intermediate files from 
    |                 raw data to sorted root file are stored )
    |
    |-- Simulation (where all kinematics simulation codes are stored)
    |
    |-- working ( This is where you are. 
                  All experimental-dependent codes, parameters, 
                  setting files are stored.)

=================================================
======== From Raw Data to ROOT file
=================================================

Raw Data
   |  
   | GEBSort  (this step is even builder, 
   |            the root tree structure is the digitizer output)
   V
run##.root
   |
   | Armory/GeneralSortTrace.h (this step is 
   |                               Mapping digitizer to detectors)
   V
gen_run##.root

=================================================
======== Initialization 
=================================================
As you can see, there are many files in "working". 
To start with, you need to check/edit few things

1) process_run.sh   

        This is the bash script to retrive the raw data from 
        the DAQ computer, run the GEBsort, and produce root files.
        
        You should check the paths are correct
        
2) detectorGeo.txt
        
        This is the settign for the detector.
        
3) basicReactionConfig.txt
        
        This is for simulation.
        
4) excitation_energies.txt

        This is for simulation as well.
        
5) Chain.C 

        This use the TChain class to process multiple gen_run##.root

=================================================
======== Online analysis
=================================================
To be honest, I don't like to separate online and offline analysis,
in ideal case, the online analysis is not so different then the offline. 

Anyway~~~~~

To sort/process/convert raw data to gen_run##.root. 
Once you set up correctly, you type

>./process_run.sh ##

Then, the gen_run##.root will be appeared in the root_data directory

You can load and "monitor" the gen_run##.root by

>root gen_run##.root
root> gen_tree->Process("Monitor.C++") 

=================================================
======== Calibration & sotre in a new root
=================================================
For Calibration, we have to edit 

dataList.txt

this txt file will tell which gen_run##.root for Calibration

then, we can call 

>root AutoCalibration.C

=================================================
======== Simulation
=================================================
..... to be continue.......

