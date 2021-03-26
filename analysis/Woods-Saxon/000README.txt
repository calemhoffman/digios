How to use the codes

====================== WSCal
WSCal is a simple calculation of the WS levels.
It only required
1) the depth (in negative)
2) radius ( not the reduced radius )
3) diffusiveness 

===================== WSCompare
WSCompare is a simple comparison of the WS levels
It will not do parameter search 

===================== WSFit 
WSSearch will search parameters and find the best fit 

===================== WSRange 
WSRange generate range of WS energy. useful fo plotting

===================== WSSearchTree
WSMakeTree will output result in tree.

===================== WSFit2
This is a special WSSearch for N=127


===================== How to use WSSearch with parallel computation in LCRC
edit the file <ProofSlurm.sh>
and run as a batch script

===================== Code dependency

RK4.h (core class using RK4 to solve Schrodinger equation)
  |
  |
  V
WS.h  (inheritage from RK4, added A, and redused radius)


1) WSCal.cpp , use WS.h class to calculate WS level
2) WSCompare.cpp , use WS.h class to compare 
3) WSSearch.cpp 
4) WSProof.C , Proof version of WSSearch
5) ProofWSSearch.C, code for running WSProof.C
