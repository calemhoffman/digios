[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3517238.svg)](https://doi.org/10.5281/zenodo.3517238)

# digios

Since Aug 19, 2025, the DAQ computer can no longer update from github (probably due to SSL too old). The daq folder will not update as often as the analysis folder. 

Also, GEBSort/Merge will no longer be used. Instead EventBuilder_S is being used.

## Note
- This repository mainly used in 4 places: HELIOS-DAQ, HELIOS-MAC, HELIOS-DB, and LCRC(/lcrc/project/HELIOS)
- In HELIOS-DAQ, MAC, and DB, things are the same, can switch to other branches.
- In LCRC, PLEASE only use the master branch at /lcrc/project/HELIOS/digios/
-- To analysis different experiment, edit digios/expName.sh, and use MakeDataLinks.
- After clone the repository, edit .bashrc, add:
-- source ~/path_to_repository/heliosrc.sh

## Major Branches
- master    : should be the latest branch with latest code
- ISS631    : the analysis code on 2018-Oct.
- h072_16N  : the analysis code for (d,3He)
- h073_82Kr : the analysis code for 82Kr(d,p), 82Se(d,p)

## Obsolte Branches
- 4SideArray : the branch for the 4-side array, the main diff is the edm panels

## Update on Feb 5, 2021, Ryan
- Latest verison update at the middle of h074_238U
- Clean up the GEBSort directory
-- GEBSort simplified to EventBuilder
-- no more TS.list and map.dat output
- Clean up daq/edm/scripts
-- put the not-used scripts into obsolete
- Clean up daq/edm/screens
-- put the not-used scripts into obsolete
- New fitting method using mouse click
-- clickFitNGaussPol
-- clickFitNGaussPolSub
- add heliosrc.sh
-- user need to add "source path_to_repository/heliosrc.sh" in ~/.bashrc
- add working/Simulation_Helper.C
-- GUI interface for Ptlomey and Kinematic simulation
- add working/generatHistogram.C
-- a code to simulated any histogram, can be use for resolution estimation
- add HVMonitor.py
-- for reading and control Iseg HV module and put to database
- add HELIOSMain_4sidesArray.edl
-- this edl also modiflied alot 
- add HELIOSArraySettings_4sidesArray.edl
- Remove Simulation
-- all programs are adapted in Cleopatra
- move process_XXXX to Armory
-- in heliosrc.sh, the Armory will be in PATH, can call Process_RUN from anywhere, but should be called in working/
- added ptolemy_mac
-- please read install_ptolemy_mac.txt for installation
- many tiny bugs fix.


## Update on July 27, 2020, Ryan
- Latest version update from H075_20Ne.
- with auto-analysis that can push Monitors.C canvas to web server.
- H075_20Ne use 4-side array, can update edm panels.
- the auto-elog contains more informations.
- with Slack
- with Grafana
- many bugs fix and improvements
- various new tools in Armory, e.g. Check_Simulation, Check_rdtGate, Cali_hamma
- renew the alpha-calibration code, so that it will find the most possible peaks to fit (AnalysisLibrary.h)
- with slurm for LCRC parllel computing
- better trace analysis
- GEBSort and GEBSort_trace, so that no need to change the bin_rcnp.c
- more Woods-Saxon tools
- separated process_run into many sub-process. so that the code is more managible, and can run sub-process separately.
- Cleopatra include potentials for A=3 and alpha
- improved the edm panels for indivial VME, can change threshold directly. 
- added AutoStartSop 
- added read vacuum gauage
- Set VME Trigger by bash command
- added globus_in.py and globus_out.py using globus cli. 

## Update on June 11, 2019, Ryan
- the code was updated and improved base on h064_15N (d,p) run 013.
- the physical dimension of the new array is not known, assume old array.

## Minor update on March 25, 2019, Ryan
- the code is tested with alpha source run, with or without trace is OK.
- AutoCalibration should be OK.
- Simluation becomes obsolete.

## Major update on March 13, 2019, Ryan (tag = v1.5)
- reorginize of the strcuture of directory
- made the daq for new 6-side array
- made the expName be solely depend on expName.sh 
- tested on Mac and DAQ, GeneralSort/Trace.C is working well
- need more test on AutoCalibration

## Major update on Feb 05, 2019, Ryan
- Move Cleopatra into analysis.
- Make a new Transfer.C/h for simulation included Ptolmey Xsec distribution.

## Major  update on Nov 29, 2018, Ryan
Basic Structure:
1. daq
2. analysis
   - oragnize the files, put all experimentals independent files in Armory.  
   - added ryan's code for stardard analysis method. 

3. Cleopatra
   - This is a user interface for Ptolemy.
 
## Initialization
All things digital data for HELIOS, ISS, etc. Including both daq and analysis software
1. Cp from svn repo
