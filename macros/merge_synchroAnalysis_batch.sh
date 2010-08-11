#!/bin/bash
. /etc/bashrc
#
cmsenvdir=${CMSSW_BASE}/src
#
if [ $# -eq 0 ]; then
 echo
 echo "Usage:  "`basename $0`" <JobDir_1> [ <JobDir_2> ... ]"
 echo
 echo "where:"
 echo "        <JobDir_#> - is a directory created by "
 echo "        'create_snchroAnalysis_batch.sh' script."
 echo "The script will parse all *_out.tgz files and create output"
 echo "'out.txt'. Then it will call  'cmsRun synchroMerger.py'"
 echo "and produce final .root and .txt files."
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
dirtmp=/tmp/$USER/BATCHTEMP___${RANDOM}___`date +%s`
mkdir -p "${dirtmp}"
if ! [ -d "${dirtmp}" ]; then
 echo "Can't create TEMP directory ${dirtmp}"
 exit -1
fi
cd ${dirtmp}
\rm *.txt *.root
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
 echo "----------------"
 tar tvzf ${dir}/`basename ${dir}`_out.tgz
 echo "----------------"
 tar xzf ${dir}/`basename ${dir}`_out.tgz "*.log" "rawMonitor*.root" "synchroAnalysis*.root"
 cat *.log | grep -i lb | grep -i mean >> out.txt
 \rm *.log
 hadd ___rawMonitor_${n}.root rawMonitor*.root
 \rm rawMonitor*.root
 hadd ___synchroAnalysis_${n}.root synchroAnalysis*.root
 \rm synchroAnalysis*.root
 n=$(( $n+1 ))
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
