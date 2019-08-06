How to use the codes

====================== WSCal
WSCal is a simple calculation of the WS levels.
It only required
1) the depth (in negative)
2) radius ( not the reduced radius )
3) diffusiveness 

===================== WSFit
WSFit is a simple comparison of the WS levels
It will not do parameter search 


===================== WSSearch 
WSSearch will search parameters

===================== How to use WSSearch with parallel computation in LCRC
using command 
 > sbatch ProofSlurm.sh

to estimate the run time, 

 >root ProofWSSearch.C+

===================== Code dependency

RK4.h (core class using RK4 to solve Schrodinger equation)
  |
  |
  V
WS.h  (inheritage from RK4, added A, and redused radius)


1) WSCal.cpp , use WS.h class to calculate WS level
2) WSFit.cpp , use WS.h class to compare 
3) WSSearch.cpp 
4) WSProof.C , Proof version of WSSearch
5) ProofWSSearch.C, code for running WSProof.C
