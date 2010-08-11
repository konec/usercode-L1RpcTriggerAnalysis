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
if [ "${cmsenvdir}" == "" ] || ! [ -d "${cmsenvdir}" ] ; then
 echo "CMSSW environment not set!"
 exit -1
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
declare -i n=0
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
  \rm efficiencyTree*.root
  tar xzf $f "efficiencyTree*.root"
  for e in `ls efficiencyTree*.root`; do
   mv $e ___`basename $e .root`___${n}.root
   n=$(( $n+1 ))
  done    
 done
done
hadd efficiencyTree.root ___*.root
echo "----------------"
ls -lart
echo "----------------"
list=efficiencyTree.filelist
ls -1 `pwd`/___efficiencyTree*.root > ${list}
echo "----------------"
cat ${list}
echo "----------------"
#
template=${cmsenvdir}/UserCode/L1RpcTriggerAnalysis/test/efficiencyAnalysis_template.py
if ! [ -f "${template}" ]; then
 echo "Can't find template PYTHON file ${template}"
 exit -1
fi
cat ${template} | sed s%_LIST_OF_FILES_%${list}% > ./efficiencyAnalysis.py
if ! [ -f "./efficiencyAnalysis.py" ] || ! [ -f "${list}" ];   then
 echo "Can't find efficiencyAnalysis.py or ${list}."
 exit -1
fi
#
cmsRun ./efficiencyAnalysis.py > efficiencyAnalysis.out 
echo "----------------"

echo "----------------"
# move the results to submit directory
cp ${list} efficiencyAnalysis.py efficiencyAnalysis.out efficiencyTree*.root efficiencyHist*.root ${pdir}/.
\rm -rf ${dirtmp}
cd ${pdir}
