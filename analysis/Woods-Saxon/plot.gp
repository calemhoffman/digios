reset

set terminal qt size 1000,600

#set title "Woods-Saxon Barrier Energy - WS Energy" font ", 14"
set title "Woods-Saxon Energy" font ", 14"
set ylabel "Energy [MeV]" font ",14"

inFile="range.txt"
n1=24
r0=1.25

set yrange [-5:0]

set grid

set key autotitle columnheader
set key outside
set key font ",14"

N=`awk 'NR==8 {print NF}' range.txt`
N=30

####################################### plot

set xrange [191:221]
set xtics 191, 2, 221
set xlabel "mass A" font ",14"
plot for [i=n1:N] inFile using 1:i with lines

#set xrange [2:8]
#set xlabel "R0=1.25 A^{1/3} [fm]" font ",14"
#plot for [i=n1:N] inFile using (r0*(($1)**(1./3.))):i with lines

#########################################

x1=0.3
y1=0.27
h1=0.03
fs=14
fontsize(n)=sprintf("Courier,%d", n)

set label 'V_0= -43 MeV'    at screen x1,y1      right font fontsize(fs)
set label 'r_0=1.30  fm'    at screen x1,y1-1*h1 right font fontsize(fs)
set label 'a_0=0.71  fm'    at screen x1,y1-2*h1 right font fontsize(fs)
set label 'V_{SO}=  27 MeV' at screen x1,y1-3*h1 right font fontsize(fs)
set label 'r_{SO}=1.20  fm' at screen x1,y1-4*h1 right font fontsize(fs)
set label 'a_{SO}=0.68  fm' at screen x1,y1-5*h1 right font fontsize(fs)

replot

