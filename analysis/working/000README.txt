************************************************************************
****        This is analysis working directory      
************************************************************************

Throught this read me, I hope I can help you to understand the basic of 
the analysis.   
                              Nov 29, 2018, Ryan (goluckyryan@gmail.com)
                              
                              
----------- context 
    1) Pre-request Knowlegde
    2) The folder (analysis) structure 
    3) From Raw Data to ROOT file
    4) Initialization 
    5) Online analysis
    6) Calibration & sotre in a new root
    7) Simulation
    8) Off-line analysis

=================================================
======== 1) Pre-request Knowlegde
=================================================
1) little idea of cern ROOT
2) little bit bash shell script
3) may be some C++

=================================================
========= 2) The folder (analysis) structure 
=================================================
analysis
    |
    |-- GEBSort (where the GEBSort code is stored)
    |
    |-- Armory (where all the experimental-independent code is stored)
    |
    |-- data  ( where the DAQ raw data stored )
    |
    |-- merged_data (where intermediate file during GEBSort)
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
======== 3) From Raw Data to ROOT file
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
   |
   | after Calibration for e, xf, xn, etc.. Armory/Cali_e_trace.h
   V
A_gen_run##.root (this can be condensation for many runs)
   

=================================================
======== 4) Initialization 
=================================================
As you can see, there are many files in "working". 
To start with, you need to check/edit few things

0) the symbolic links / folder

    /anlysis/data
    /anlysis/merged_data
    /anlysis/root_data

        These are data will be stored. 
        ===== YOU NEED TO CREATE YOURSELF ======
        
1) GeneralSortMapping.h
        
        This is the detector Mapping.

2) process_run.sh   

        This is the bash script to retrive the raw data from 
        the DAQ computer, run the GEBsort, and produce root files.
        
        You should check the paths are correct
        
2) process_run.C   

        This macro is for calling GeneralSort.C
        
        You should check the paths are correct
        
4) detectorGeo.txt
        
        This is the settign for the detector. This file is widely used
        in Calibration.
        
5) Chain.C 

        This use the TChain class to process multiple gen_run##.root

=================================================
======== 5) Online analysis
=================================================
To be honest, I don't like to separate online and offline analysis,
in ideal case, the online analysis is not so different then the offline. 

Anyway~~~~~

To sort/process/convert raw data to gen_run##.root. 
Once you set up correctly, you type

    >./process_run.sh ##
    
          step 1) get the raw data from DAQ server
          
          step 2) merge and sort --> created root_data/run##.root
          
          step 3) GeneralSort using process.C --> root_data/gen_run##.root
          
          step 4) Monitors.C using ChainMonitors.C

After step 3), gen_run##.root will be appeared in the root_data directory
You can load and "monitor" the gen_run##.root by

    >root gen_run##.root
    root> gen_tree->Process("Monitor.C++") 

=================================================
======== 6) Calibration & sotre in a new root
=================================================
For Calibration, we have to edit 

     runsList.txt

this txt file will tell which gen_run##.root for Calibration

then, we can call 

    >root AutoCalibrationTrace.C

=================================================
======== 7) Simulation
=================================================

To simulate a transfer reaction, Two input files 

    1) basicReactionConfig.txt
    2) detectorGeo.txt
    3) excitation_energy.txt
    
Must be edited to suit the reaction. If no excitation_energy.txt, 
only ground state will be calculated. 

In the output are 

    1) transfer.root
          
       constain a) tree   - constain simulated e, z, thetaCM, Ex
                b) gList  - TObjArray of TGraph, gList->At(i)
                                  is the constant thetaCM=ideg lines
                c) fList  - TObjArray of TGraph, infinite detector e-z lines
                d) fxList - TObjArray of TGraph, finite detector e-z lines
                c) txList - TObjArray of TGraph, thetaCM vs z lines
          
    2) reaction.dat
      
       essential parameter for mapping (e,z) -> (Ex, thetaCM)
       
       
To run a simualtion
 
    >root ../Simulation/transfer.C+

=================================================
======== 8) Off-line analysis
=================================================

Once the Calibration are done. It is better to generate a new root file.
Remeber to edit 

runsList.txt
    
Then run

    >root AutoCalibrationTrace.C+
    
    option 5
    
This will generate A_gen_run##.root.

A further gate selection, detail Ex offset, etc... can be done with 

    * Analyzer.h

=================================================
========   end of README  
=================================================
