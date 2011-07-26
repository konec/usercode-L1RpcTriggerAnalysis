#!/usr/bin/env python

import os, commands
from array import array
import ROOT


###Read data from txt file
file = open('pressure_twiki_160406-171484.dat')
counter = 0
runs = []
pressure = []
for line in file:
    print line.split()
    runs.append(float(line.split()[0]))
    if(len(line.split())>1):               
               pressure.append(float(line.split()[1]))
    else:
               pressure.append(0.0)               
    counter+=1

x = array("d",runs)
y = array("d",pressure)

###Define efficiency map histogram
myFile = ROOT.TFile("PressureGraph.root","RECREATE")
myGraph = ROOT.TGraph(len(runs),x,y)

myGraph.Write()
myFile.Write()
myFile.Close()
