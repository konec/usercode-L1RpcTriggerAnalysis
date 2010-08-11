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
 echo "        'create_synchroAnalysis_batch.sh' script."
 echo
 echo "The script will parse all *_out.tgz files and create "
 echo "'efficiency*.root' files. Then it will call "
 echo "'cmsRun efficiencyAnalysis.py' and produce final efficienyTree.root."
 echo "NOTE: Initialize CMSSW environment beforehand!"
 echo
 exit -1
fi
#
if [ "${cmsenvdir}" == "" ] || ! [ -d "${cmsenvdir}" ] ; then
 echo "CMSSW environment not set!!!"
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
 echo "----------------"
 tar tvzf ${dir}/`basename ${dir}`_out.tgz
 echo "----------------"
 \rm efficiencyTree*.root
 tar xzf ${dir}/`basename ${dir}`_out.tgz "efficiencyTree*.root"
 for e in `ls efficiencyTree*.root`; do
  mv $e ___`basename $e .root`___${n}.root
  n=$(( $n+1 ))
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
cat ${template} | sed s%_LIST_OF_FILES_%"'${list}'"% > ./efficiencyAnalysis.py
if ! [ -f "./efficiencyAnalysis.py" ] || ! [ -f "${list}" ];   then
 echo "Can't find efficiencyAnalysis.py or ${list}."
 exit -1
fi
#
#cd ${cmsenvdir} ; eval `scramv1 runtime -sh` ; cd -
cmsRun ./efficiencyAnalysis.py > efficiencyAnalysis.out 
echo "----------------"

echo "----------------"
# move the results to submit directory
cp ${list} efficiencyAnalysis.py efficiencyAnalysis.out efficiencyTree*.root efficiencyHist*.root ${pdir}/.
\rm -rf ${dirtmp}
cd ${pdir}
