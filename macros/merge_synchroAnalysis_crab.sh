#!/bin/bash
. /etc/bashrc
#
cmsenvdir=${CMSSW_BASE}/src
#
if [ $# -eq 0 ]; then
 echo
 echo "Usage:  "`basename $0`" <CrabDir_1> [ <CrabDir_2> ... ]"
 echo
 echo "where:"
 echo "        <CrabDir_#> - is a directory created by CRAB,"
 echo "        eg. crab_0_100713_123708."
 echo
 echo "The script will parse all res/out_files_*.tgz files and extract"
 echo "rootuples relevant for efficiency analysis. Then it will call"
 echo "'cmsRun efficiencyAnalysis.py' and produce final 'efficienyTree.root'"
 echo "and 'efficiencyHistos.root' files."
 echo "NOTE: Initialize CMSSW environment beforehand!"
 echo
 exit -1
fi
#
if [ "${cmsenvdir}" == "" ] || ! [ -d "${cmsenvdir}" ] || ! [ -f "${cmsenvdir}/UserCode/L1RpcTriggerAnalysis/test/synchroMerger.py" ]; then
 echo "CMSSW environment not set or synchroMerger.py missing!"
 exit -2
fi 
#
pdir=`pwd`
cd `dirname $0`; scriptdir=`pwd` ; cd -
dirtmp=/tmp/$USER/BATCHTEMP___${RANDOM}___`date +%s`
mkdir -p "${dirtmp}"
if ! [ -d "${dirtmp}" ]; then
 echo "Can't create TEMP directory ${dirtmp}"
 exit -1
fi
cd ${dirtmp}
\rm *.out *.root
#
# loop over job results and extract hit BX distributions from .log files
cd ${cmsenvdir} ; eval `scramv1 runtime -sh` ; cd -
declare -i n1=0
declare -i n2=0
declare -i n3=0
for dir in "$@"; do
 #
 # check if absolute path is given
 firstchar=`echo ${dir}|cut -c1-1`
 if [ "$firstchar" != "/" ]; then
  dir=${pdir}/${dir}
 fi 
 #
 for f in `ls -d ${dir}/res/out_files_*.tgz`; do
  echo "----------------"
  tar tvzf $f 
  echo "----------------"
  \rm C*.stdout rawMonitor*.root synchroAnalysis*.root
  tar xzf $f "C*.stdout" "rawMonitor*.root" "synchroAnalysis*.root"
  cat C*.stdout | grep -i lb | grep -i mean >> out.txt
  \rm C*.stdout  
  for e in `ls rawMonitor*.root`; do
   mv ${e} ___`basename ${e} .root`___${n1}.root
   n1=$(( $n1+1 ))
  done    
  for e in `ls synchroAnalysis*.root`; do
   mv ${e} ___`basename ${e} .root`___${n2}.root
   n2=$(( $n2+1 ))
  done    
 done
done
echo "----------------"
ls -lart
echo "----------------"
hadd rawMonitor.root      ___rawMonitor_*.root
\rm ___rawMonitor_*.root
hadd synchroAnalysis.root ___synchroAnalysis_*.root
\rm ___synchroAnalysis_*.root
#
# run synchroMerger (needs out.txt)
cmsRun ${cmsenvdir}/UserCode/L1RpcTriggerAnalysis/test/synchroMerger.py | grep -i lb | grep mean > delays.txt
echo "----------------"
ls -lart
echo "----------------"
#
# move the results to submit directory
cp delays.txt *.root  ${pdir}/.
\rm -rf ${dirtmp}
cd ${pdir}
