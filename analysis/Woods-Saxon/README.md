# Compilation

simply 

 >make

# WSCal
WSCal is a simple calculation of the WS levels.
It only required
1) the depth (in negative)
2) radius ( not the reduced radius )
3) diffusiveness 

# WSRange 
WSRange generates range of WS energy. useful for plotting

# WSCompare
WSCompare is a simple comparison of the WS levels
It will not do parameter search 

# WSFit 
WSSearch will search parameters and find the best fit 

# WSSearch

## How to use WSSearch with parallel computation in LCRC
edit the file `ProofSlurm.sh`
and run as a batch script

# Code dependency

- RK4.h (core class using RK4 to solve Schrodinger equation)
- WS.h  (inheritage from RK4, added A, and redused radius)