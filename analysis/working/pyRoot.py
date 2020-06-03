from ROOT import gROOT, gStyle, TCanvas, TMath, TF1, TH1F, TH2F, TFile, TTree, TChain, TObjArray, TCutG
from array import array
gROOT.Reset()

gStyle.SetOptStat("neiou")

tree = TChain("tree")
tree.Add("A_trace_run055-057_059-085_087-099.root")
tree.Add("A_trace_run101-144_147-165_167-214.root")
totEntry = tree.GetEntries()
print ("total Entry = ", totEntry)

c1 = TCanvas( 'c1', 'Example with Formula', 800, 800 )
c1.Divide(2,2)

rdt1 = TH2F("rdt1", "", 300, 0, 4000, 300, 0, 6000)
rdt2 = TH2F("rdt2", "", 300, 0, 4000, 300, 0, 6000)
rdt3 = TH2F("rdt3", "", 300, 0, 4000, 300, 0, 6000)
rdt4 = TH2F("rdt4", "", 300, 0, 4000, 300, 0, 6000)

c1.cd(1); tree.Draw("0.54*rdt[1]:1.1*rdt[0]>>rdt1", "", "colz")
c1.cd(2); tree.Draw("rdt[3]:rdt[2]>>rdt2", "", "colz")
c1.cd(3); tree.Draw("rdt[5]:rdt[4]>>rdt3", "", "colz")
c1.cd(4); tree.Draw("0.50*rdt[7]:1.05*rdt[6]>>rdt4", "", "colz")

#c1.cd(1); tree.Draw("rdt[1]:rdt[0]>>rdt1", "", "colz");
#c1.cd(2); tree.Draw("rdt[3]:rdt[2]>>rdt2", "", "colz");
#c1.cd(3); tree.Draw("rdt[5]:rdt[4]>>rdt3", "", "colz");
#c1.cd(4); tree.Draw("rdt[7]:rdt[6]>>rdt4", "", "colz");
#
#fCut = TFile("rdtCuts_15C_tight.root");
#cutList = fCut.Get("cutList");
#
#c1.cd(1); cutList.At(0).Draw("same");
#c1.cd(2); cutList.At(1).Draw("same");
#c1.cd(3); cutList.At(2).Draw("same");
#c1.cd(4); cutList.At(3).Draw("same");


#c1.cd(2)

'''
i=0
for evt in tree:
  if TMath.IsNaN(evt.e) :
    continue
  h1.Fill(evt.t - evt.tB)
  h2.Fill(evt.z, evt.e) #x,y
  i = i+1
  if i > 10000 : 
    break
'''

'''
# write new tree
fOut = TFile('test.root', 'recreate')
treeOut = TTree ( 'tree', 'tree test')
p1 = array ( "i", [ 0 ])
p2 = array ( "f", 10 * [ 0. ] )
treeOut.Branch( 'p1', p1, 'p1/I')
treeOut.Branch( 'p2', p2, 'p2[10]/F')

for i in range(25):
   p1[0] = min(i,10)
   for j in range(p1[0]):
      p2[j] = i*0.1+j
   treeOut.Fill()
 
fOut.Write()
fOut.Close()
'''
