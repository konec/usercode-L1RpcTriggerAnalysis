#!/bin/csh
#
set cmsenvdir=${CMSSW_BASE}/src
#
if ( ${#argv} < 1 ) then
 echo
 echo "Usage:  "`basename $0`" <CrabDir_1> [<CrabDir_2> ...]"
 echo
 echo "where:"
 echo "        <CrabDir_#> - is a directory with crab.cfg, JSON and"
 echo "                      synchroAnalysis.py files."
 echo
 echo "The script will create, validate and submit CRAB jobs" 
 echo "(one per given directory)."
 echo "NOTE: Initialize CMSSW environment beforehand!"
 echo  
 exit -1
endif
#
if ( ( "${cmsenvdir}" == "" ) || ! ( -d "${cmsenvdir}") ) then
 echo "CMSSW environment not set!"
 exit -1
endif 
#
source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
cd ${cmsenvdir}; cmsenv; cd -
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
#
set firstjob=1
set pdir=`pwd`
#
foreach i (`seq 1 $#argv`) 
 set dir=$argv[$i]
 echo $dir - start. 
 cd $dir
 if $firstjob == 1 then 
 set firstjob=0
 endif
 crab -create
 crab -validateCfg synchroAnalysis.py
 crab -submit
 cd $pdir
 echo $dir - done.
end

#foreach i (`seq 1 $#argv`) 
# set dir=$argv[$i]
# echo $dir - start. 
# cd $dir
# if $firstjob == 1 then 
#  source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
#  cd /afs/cern.ch/user/c/cwiok/scratch0/eff_synchro.V00-02-00/CMSSW_3_7_0/src; cmsenv; cd -
#   source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
#   voms-proxy-init -voms cms -valid 400:00
#  set firstjob=0
# endif
# crab -create
# crab -validateCfg synchroAnalysis.py
# crab -submit
# cd $pdir
# echo $dir - done.
#end

