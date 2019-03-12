# digios

## Major Branches
- master : should be the latest branch with latest code
- 4SideArray : the branch for the 4-side array, the main diff is the daq
- ISS631 : the analysis code on 2018-Oct.

## Major Update on Feb 05, 2019, Ryan
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
