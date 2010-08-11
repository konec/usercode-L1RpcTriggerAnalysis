#!/bin/bash
. /etc/bashrc
#
#
# Scans multiple <JobDir_#> directories,
# extracts .log file from the tarball *_out.tgz file,
# parses .log file and extracts processed lumi sections to a JSON file 
#
# check number of arguments
if [ $# -eq 0 ]; then
 echo
 echo "Usage: "`basename $0`" <JobDir_1> [<JobDir_2] ...]                    "
 echo
 echo "where:"
 echo "        <JobDir_#> - is a directory created by "
 echo "        'create_synchroAnalysis_batch.sh' script and "
 echo "        contains a valid output from the processed batch job,"
 echo "        i.e. <JobDir_#>/<JobDir_#>_out.tgz file MUST be present."
 echo 
 echo "For each <JobDir_#> directory the script will:"
 echo "- extract .log file from the tarball <JobDir_#>_out.tgz,"
 echo "- parse .log file and extract processed lumi sections,"
 echo "- create a JSON file called <JobDir_#>_out.json" 
 echo
 exit -1
fi
#
# 
dir=`pwd`
dirtmp=/tmp/$USER/BATCHTEMP___${RANDOM}___`date +%s`
mkdir -p "${dirtmp}"
if ! [ -d "${dirtmp}" ]; then
 echo "Can't create TEMP directory ${dirtmp}"
 exit -1
fi
cd ${dirtmp}
#
for j in "$@"; do
 #
 # check if this is an absolute path
 dirjob="${j}"
 echo "Checking ${dirjob}..." 
 firstchar=`echo "${dirjob}" | cut -c1-1`
 if ! [ "${firstchar}" = "/" ]; then
  dirjob="${dir}/${j}"
 fi
 log=`basename ${dirjob}`".log"
 json=`basename ${dirjob}`"_out.json"
 tarball="${dirjob}/"`basename ${dirjob}`"_out.tgz"
 if ! [ -f "${tarball}" ]; then
  echo "Can't find tarball ${tarball}"
  continue
 fi
 if [ -f "${log}" ]; then
  echo "WARNING: Summary JSON file ${json} already exists!"
  \rm "${log}"
 fi
 tar xzf "${tarball}" "${log}"
 if ! [ -f "${log}" ]; then
  echo "Can't extract logfile ${log} from tarball ${tarball}"
  continue
 fi
 if [ -f "${json}" ]; then 
  echo "WARNING: Summary JSON file ${json} already exists!"
  \rm "${json}"
 fi
 arr=(`cat ${log} | grep "Lumi" | grep "LumiSection" | grep "Begin processing the" | grep "record" | cut -f7,11 -d' ' | sed s%', '%'|'%`)
 runold=-1
 lsold=-1
 ls=-1
 run=-1
 for line in "${arr[@]}"; do
  run=`echo $line|cut -f1 -d'|'`
  ls=`echo $line|cut -f2 -d'|'`
  if [ "${runold}" != "${run}" ]; then 
   if [ "${runold}" != "-1" ]; then 
    echo "${lsold}]],\"${run}\":[[${ls}," >> ${json}
   else
    echo "{\"${run}\":[[${ls}," > ${json}
   fi
  else 
   if [ "${lsold}" != "${ls}" ]; then
    echo "${lsold}],[${ls}," >> ${json}
   fi
  fi
  lsold=$ls
  runold=$run
 done
 if [ "${lsold}" != "-1" ]; then
  echo "${lsold}]]}" >> ${json}
 fi
 # remove blank spaces
 cat ${json} | sed s%' '%% | sed s%'\n'%% > ${dirjob}/${json}
 \rm ${log}
 \rm ${json}
done
#
ls -lart
cd -
\rm -rf ${dirtmp}
