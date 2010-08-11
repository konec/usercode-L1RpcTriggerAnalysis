#!/bin/bash
. /etc/bashrc
#
#
# Scans multiple <CrabDir_#> directories,
# extracts crab_*.xml files from the tarball out_*.tgz files,
# calls 'crab -report' and extracts processed lumi sections to a JSON file 
#
# check number of arguments
if [ $# -eq 0 ]; then
 echo
 echo "Usage: "`basename $0`" <CrabDir_1> [<CrabDir_2] ...]"
 echo
 echo "where:"
 echo "        <CrabDir_#> - is a directory created by "
 echo "        'crab -create' command and contains a valid output from"
 echo "        the processed CRAB job,"
 echo "        i.e. <CrabDir_#>/res/out_*.tgz files MUST be present."
 echo 
 echo "For each <CrabDir_#> directory the script will:"
 echo "- extract crab_*.xml files from the tarball <CrabDir_#>/res/out_*.tgz,"
 echo "- call 'crab -report' command."
 echo
 exit -1
fi
#
cmsenvdir=${CMSSW_BASE}
if ! [ -d "${cmsenvdir}" ]; then
 echo "CMSSW environment not set!!!"
 exit -1
fi
/afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh
cd ${cmsenvdir} ; eval `scramv1 runtime -sh`; cd -
/afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh
# 
pdir=`pwd`
for j in "$@"; do
 #
 # check if this is an absolute path
 dirjob="${j}"
 echo "Checking ${dirjob}..." 
 firstchar=`echo "${dirjob}" | cut -c1-1`
 if ! [ "${firstchar}" = "/" ]; then
  dirjob="${dir}/${j}"
 fi
 for tarball in `ls -d ${dirjob}/res/out_*.tgz`; do
     if ! [ -f "${tarball}" ]; then
	 echo "Can't find tarball ${tarball}"
	 continue
     fi
     cd `dirname ${tarball}`
     tar xvzf "${tarball}" "crab*.xml"
     cd -
 done
 #
 crab -report -c ${dirjob}
 ls -la ${dirjob}/res/*.json
done
cd ${pdir}
