#!/bin/bash -f

runListFile=runsFromJson.list
base=/castor/cern.ch/cms
#castor=/store/express/Run2010A/ExpressPhysics/FEVT/v4/000
castor=/store/express/Run2011A/ExpressPhysics/FEVT/Express-v4/000

#echo  $base/$castor

for runA in `nsls $base/$castor`
do

path=$base/$castor/$runA
#echo $path

  for  runB in `nsls $path`
    do
	fullpath=$path/$runB
	#echo $fullpath
	
	run=$runA$runB
	#echo run  $run
	
	# runy z json
	for listRun in in `more $runListFile  | awk '{print $2}'`
	do

	
	if [ $listRun = $run ] 
	then
	        echo run from list $listRun $run

		for file in `nsls $fullpath`
		do		
		echo $castor/$runA/$runB/$file
		done
	fi		
	done

    done

done	

