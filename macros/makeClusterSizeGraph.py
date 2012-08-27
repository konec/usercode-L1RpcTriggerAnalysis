#!/usr/bin/env python

import os, commands, sys
from array import array
import ROOT

if __name__ == "__main__":
    if len(sys.argv[1:]) != 1:
        print "\nUsage "+sys.argv[0]+" <file>"
        print "where <file> is a text file with at least 6 columns:"
        print "RUN_NUMBER PRESSURE EFF_B EFF_E CSIZE_B CSIZE_E PERCENT_MASKED_B PERCENT_MASKED_E\n"
        exit()

    ###Read data from txt file
    file = open(sys.argv[1])
    counter = 0
    runsP = []
    runsCB = []
    runsCE = []
    pressure = []
    csizeBarrel = []
    csizeEndcap = []
    for line in file:
        print line.split()
        if(line[0]=="#"):
            print "Line skipped"
            continue
        if(len(line.split())>5):
            r  = float(line.split()[0])
            p  = float(line.split()[1])
            cb = float(line.split()[4])
            ce = float(line.split()[5])
            if(r>0 and p>0):
                runsP.append(r);
                pressure.append(p);
            if(r>0 and cb>0):
                runsCB.append(r);
                csizeBarrel.append(cb);
            if(r>0 and ce>0):
                runsCE.append(r);
                csizeEndcap.append(ce);

    x1 = array("d",runsP)
    x2 = array("d",runsCB)
    x3 = array("d",runsCE)
    y1 = array("d",pressure)
    y2 = array("d",csizeBarrel)
    y3 = array("d",csizeEndcap)

    #myFile1  = ROOT.TFile("PressureGraph.root","RECREATE")
    #myGraph1 = ROOT.TGraph(len(runsP),x1,y1)
    #myGraph1.SetName("Graph");
    #myGraph1.SetTitle("Pressure vs Run number;Run;Pressure [mbar]");
    #myGraph1.Write()
    #myFile1.Write()
    #myFile1.Close()

    myFile2  = ROOT.TFile("ClusterSizeGraph_Barrel.root","RECREATE")
    myGraph2 = ROOT.TGraph(len(runsCB),x2,y2)
    myGraph2.SetName("Graph");
    myGraph2.SetTitle("Barrel cluster size vs Run number;Run;Cluster size [strips]");
    myGraph2.Write()
    myFile2.Write()
    myFile2.Close()

    myFile3  = ROOT.TFile("ClusterSizeGraph_Endcap.root","RECREATE")
    myGraph3 = ROOT.TGraph(len(runsCE),x3,y3)
    myGraph3.SetName("Graph");
    myGraph3.SetTitle("Endcap cluster size vs Run number;Run;Cluster size [strips]");
    myGraph3.Write()
    myFile3.Write()
    myFile3.Close()
