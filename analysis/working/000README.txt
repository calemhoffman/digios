************************************************************************
****        This is analysis working directory      
************************************************************************

Throught this read me, I hope I can help you to understand the basic of 
the analysis.   
                              Nov 29, 2018, Ryan (goluckyryan@gmail.com)
                 last update: Feb 05, 2019, Ryan        
                              
----------- context 
    1) Pre-request Knowlegde
    2) The folder (analysis) structure 
    3) From Raw Data to ROOT file
    4) Initialization 
    5) Online analysis
    6) Calibration & sotre in a new root
    7) Simulation
    8) Cleopatra for DWBA calculation using Ptolemy
    9) Off-line analysis

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
    |-- Cleopatra (where all Ptolemy and new Transfer simulation codes are stored)
    |
    |-- Simulation (obsolete)
    |
    |-- working ( This is where you are. 
                  All experimental-dependent codes, parameters, 
                  setting files are stored.)

=================================================
======== 3) From Raw Data to ROOT file
=================================================

Raw Data
   |
   | GEBMerge (this step will merge all the raw files to a single file)
   |
merged_run##
   |  
   | GEBSort  (this step is even builder, 
   |            the root tree structure is the digitizer output)
   V
run##.root
   |
   | Armory/GeneralSort.h (this step is 
   |                               Mapping digitizer to detectors)
   V
gen_run##.root
   |
   | Monitors.C
   V
online analysis Canvas

another way is create another root for calibrated, physical data

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

2) process_run   

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

    >./process_run RUNID
    
          step 1) get the raw data from DAQ server
          
          step 2) merge and sort --> created root_data/run##.root
          
          step 3) GeneralSort using process.C --> root_data/gen_run##.root
          
          step 4) Monitors.C using ChainMonitors.C

After step 3), gen_run##.root will be appeared in the root_data directory
You can load and "monitor" the gen_run##.root by

    >root gen_run##.root
    root> gen_tree->Process("Monitor.C++") 
    
To see more for the usage of ./process_run

    >./process_run -help

=================================================
======== 6) Calibration & sotre in a new root
=================================================
For Calibration, we have to edit 

     runsList.txt
     detectorGeo.txt

these txt files will tell which gen_run##.root for Calibration

then, we can call 

    >root AutoCalibrationTrace.C

=================================================
======== 7) Simulation
=================================================

To simulate a transfer reaction, Two input files 

    1) reactionConfig.txt
    2) detectorGeo.txt
    3) Ex.txt
    
Must be edited to suit the reaction. If no Ex.txt, 
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
 
    >../Cleopatra/Transfer
    
To see more

    >../Cleopatra/Transfer -help

=================================================
======== 8) Cleopatra for DWBA calculation using Ptolemy
=================================================

The calculation is using Ptolemy. 
It requires an *.in file for setting of optical potenital.

Cleopatra is a user interface for Ptolemy.
It do following things:

1) create *in file (InFileCreator)
2) do Ptolemy calculation
3) extract cross section distributions from *.out file (ExtractXsec)
4) plot the distributions (PlotTGraphObjArray)
5) (optional) simulate transfer reaction.

The input files:

      DWBA : for simple Ptolemy setting, the file name can be changed.

The output files are:

    1) DWBA.in  (in file for Ptolemy)
    2) DWBA.out (out file for Ptolemy)
    3) DWBA.Ex.txt (excitation energies with Xsec)
    4) DWBA.Xsec.txt (Xsec distributions in txt format)
    5) DWBA.root ( Xsec distributions in TObjArray)

For simulating transfer reaction with thetaCM distribution. 
need the reactionConfig.txt and detectorGeo.txt. same as part 7).
But instead of using the Ex.txt, use DWBA.Ex.txt. 


=================================================
======== 9) Off-line analysis
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
