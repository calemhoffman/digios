#!/usr/local/bin/python3

################################################
import sys

if len(sys.argv) == 1 :
  print( "usage :" )
  print( "%s AZ [maxEx]" % sys.argv[0] )
  print( "e.g. %s 17O --> 17O ground state data" % sys.argv[0])
  print( "e.g. %s 20O 5 --> 20O ground state data and excited state < 5 MeV" % sys.argv[0])
  exit()

################################################
import pandas as pd

# the API webpage
# https://www-nds.iaea.org/relnsd/vcharthtml/api_v0_guide.html#examples

# the service URL
livechart = "https://nds.iaea.org/relnsd/v0/data?"

import urllib.request

def lc_read_csv(url):
  req = urllib.request.Request(url)
  req.add_header('User-Agent', 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:77.0) Gecko/20100101 Firefox/77.0')
  return pd.read_csv(urllib.request.urlopen(req))

haha = lc_read_csv(livechart + 'fields=ground_states&nuclides=all')

mp = 938.27208816; #MeV/c^2
mn = 939.56542052;

def FindZ(AZ):
  query = livechart + "fields=ground_states&nuclides=" + AZ
  temp = lc_read_csv(query);
  try :
    return temp['z']
  except :
    return 'na'
def FindSym(Z):
  try:
    return (haha['symbol'][haha['z']==Z]).iloc[0]
  except:
    return 'na'
def Mass(A, Z):
  try :
    BEA = float(haha['binding'][haha['z']==Z][haha['n']==(A-Z)])/1000
    return (A-Z)*mn + Z*mp - A * BEA
  except :
    return -404
def MassSym(AZ):
  query = livechart + "fields=ground_states&nuclides=" + AZ
  temp = lc_read_csv(query);
  Z = temp['z']
  N = temp['n']
  try :
    return Z*mp + N*mn - (Z+N)*temp['binding']/1000
  except:
    return -404
def Sp(A,Z,a,z):
  mA = Mass(A,Z)
  mB = Mass(A-a, Z-z)
  if z == 0 :
    mb = a * mn
  elif a == z :
    mb = a * mp
  else :
    mb = Mass(a,z)
  if (mB == -404 or mb == -404 or mA == -404) :
    return -404
  else:
    return mB + mb - mA
def Ex(AZ, maxMeV):
  query = livechart + "fields=levels&nuclides=" + AZ
  tempEx = lc_read_csv(query);
  try :
    return tempEx[['energy', 'jp']][tempEx['energy']<= maxMeV * 1000]
  except:
    return -404

def Info(AZ):
  query = livechart + "fields=ground_states&nuclides=" + AZ
  temp = lc_read_csv(query);
  
  print("============================================== " + AZ )
  try :
    Z = temp['z'][0]
    N = temp['n'][0]
    mass = Z*mp + N*mn - (Z+N)*temp['binding']/1000
    halfLife = temp['half_life_sec'][0]
    print("  A : %3d, Z : %3d, N : %3d, Mass : %.4f MeV" % (Z+N, Z, N, mass))
    print("Jpi : %3s,    half-live : %s sec" % (temp['jp'][0], halfLife))
    print("Sn  : %8.3f MeV, Sp  : %8.3f MeV" % (Sp(Z+N,Z, 1, 0), Sp(Z+N,Z, 1, 1)))
    print("S2n : %8.3f MeV, S2p : %8.3f MeV, Sd : %8.3f MeV" % (Sp(Z+N,Z, 2, 0), Sp(Z+N,Z, 2, 2), Sp(Z+N, Z, 2, 1)))
    print("S3n : %8.3f MeV, S3p : %8.3f MeV, St : %8.3f MeV, S(3He) : %8.3f MeV" % (Sp(Z+N,Z, 3, 0), Sp(Z+N,Z, 3, 3), Sp(Z+N, Z, 3, 1), Sp(Z+N, Z, 3, 2)))
    print("S4n : %8.3f MeV, S4p : %8.3f MeV, Sa : %8.3f MeV" % (Sp(Z+N,Z, 4, 0), Sp(Z+N,Z, 4, 4), Sp(Z+N, Z, 4, 2)))
    print("   magnetic dipole : " + temp['magnetic_dipole'][0] + " mu.N")
    print("electric quadruple : " + temp['electric_quadrupole'][0] + " barn")
    if halfLife > 0 :
      print('------------ decay mode:')
      for i in range(1, 4) :
        print("%5s  %s %%" % (temp["decay_%d" % i][0], temp["decay_%d_%%" % i][0]))
      print('--------------------------')
  except :
    dummy = 1
  print("====================================================\n")
  
  #outfile = open("%s.txt" % AZ, "w")
  #outfile.write("============================================== " + AZ + "\n")
  #try :
  #  Z = temp['z'][0]
  #  N = temp['n'][0]
  #  mass = Z*mp + N*mn - (Z+N)*temp['binding']/1000
  #  halfLife = temp['half_life_sec'][0]
  #  outfile.write("  A : %3d, Z : %3d, N : %3d, Mass : %.4f MeV \n" % (Z+N, Z, N, mass))
  #  outfile.write("Jpi : %3s,    half-live : %s sec \n" % (temp['jp'][0], halfLife))
  #  outfile.write("Sn  : %8.3f MeV, Sp  : %8.3f MeV \n" % (Sp(Z+N,Z, 1, 0), Sp(Z+N,Z, 1, 1)))
  #  outfile.write("S2n : %8.3f MeV, S2p : %8.3f MeV, Sd : %8.3f MeV \n" % (Sp(Z+N,Z, 2, 0), Sp(Z+N,Z, 2, 2), Sp(Z+N, Z, 2, 1)))
  #  outfile.write("S3n : %8.3f MeV, S3p : %8.3f MeV, St : %8.3f MeV, S(3He) : %8.3f MeV \n" % (Sp(Z+N,Z, 3, 0), Sp(Z+N,Z, 3, 3), Sp(Z+N, Z, 3, 1), Sp(Z+N, Z, 3, 2)))
  #  outfile.write("S4n : %8.3f MeV, S4p : %8.3f MeV, Sa : %8.3f MeV \n" % (Sp(Z+N,Z, 4, 0), Sp(Z+N,Z, 4, 4), Sp(Z+N, Z, 4, 2)))
  #  outfile.write("   magnetic dipole : " + temp['magnetic_dipole'][0] + " mu.N\n")
  #  outfile.write("electric quadruple : " + temp['electric_quadrupole'][0] + " barn \n")
  #  if halfLife > 0 :
  #    outfile.write('------------ decay mode: \n')
  #    for i in range(1, 4) :
  #      outfile.write("%5s  %s %% \n" % (temp["decay_%d" % i][0], temp["decay_%d_%%" % i][0]))
  #    outfile.write('-------------------------\n')
  #except :
  #  dummy = 1
  #outfile.write("====================================================\n")
  #outfile.close();
  #
  #infile = open("%s.txt" % AZ, "r")
  #Lines = infile.readlines()
  #
  #for line in Lines:
  #  print(line.strip("\n"))
  #  
  #infile.close();
  

################################################
AZ = sys.argv[1];
maxEx = 0; 
if len(sys.argv) > 2 :
  maxEx = float(sys.argv[2]);

Info(AZ)

if maxEx > 0 :
  print( Ex(AZ, float(maxEx)) )

