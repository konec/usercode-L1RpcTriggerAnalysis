#!/usr/bin/python

lines = open('goodruns.json', 'r').readlines() 

run_map = eval(lines[0]) 

runf = open('runsFromJson.list','w')


for run in run_map.keys():
    N = 1 
    for interval in run_map[run]:
        N += interval[1] - interval[0] + 1 
    runf.write("run "+run+"\n")

runf.close()

