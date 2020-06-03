from ROOT import gROOT,TFile, TTree
from array import array
import numpy as np

gROOT.Reset()

print("======================= save to a tree")

fOut = TFile('test.root', 'recreate')
treeOut = TTree ( 'tree', 'tree test')

e_p = np.array([1], dtype=np.float32) 
z_p = np.array([1], dtype=np.float32)

treeOut.Branch( 'e', e_p, 'e/F')
treeOut.Branch( 'z', z_p, 'z/F')

for i in range(10):
  e_p[0] = i
  z_p[0] = i*i
  print("%d , %f %f " % (i, e_p, z_p))
  treeOut.Fill()
 
treeOut.Write() 

treeOut.Scan()

fOut.Write()
#fOut.Close()

