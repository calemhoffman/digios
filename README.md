[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3517238.svg)](https://doi.org/10.5281/zenodo.3517238)

# digios

## Note
- This repository mainly used in 4 places: HELIOS-DAQ, HELIOS-MAC, HELIOS-DB, and LCRC(/lcrc/project/HELIOS)
- In HELIOS-DAQ, MAC, and DB, things are the same, can switch to other branches.
- In LCRC, PLEASE only use the master branch at /lcrc/project/HELIOS/digios/
-- To analysis different experiment, edit digios/expName.sh, and use MakeDataLinks.

## Major Branches
- master : should be the latest branch with latest code
- ISS631 : the analysis code on 2018-Oct.
- h072_16N : the analysis code for (d,3He)

## Obsolte Branches
- 4SideArray : the branch for the 4-side array, the main diff is the edm panels

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
