# digios

## Major Branches
- master : should be the latest branch with latest code
- 4SideArray : the branch for the 4-side array, the main diff is the daq
- ISS631 : the analysis code on 2018-Oct.

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
