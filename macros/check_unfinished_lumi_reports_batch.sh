#!/bin/bash
. /etc/bashrc
#
# threshold in bytes for GOOD *_out.json files.
declare -i threshold=14
#
if [ $# -lt 1 ]; then
 echo
 echo "Usage:  "`basename $0`" <JobDir_1> [<JobDir_2> ...]"
 echo
 echo "where:"
 echo "        <JobDir_#> - is a directory created by "
 echo "        'create_synchroAnalysis_batch.sh' script."
 echo 
 echo "For each <JobDir_#> the script will check if a valid summary JSON"
 echo "file called <JobDir_#>/<JobDir_#>_out.json already exists or not"
 echo "(size > ${threshold} bytes)."
 echo "At the end number of: total, finished and unfinished jobs will be "
 echo "printed and corresponding job locations will be stored in:"
 echo "___all, ___finished and ___unfinished text files, respectively."
 echo
 exit -1
fi
#
# check if absolute path is given
\rm ___all ___finished ___unfinished
for dir in "$@"; do
 firstchar=`echo ${dir}|cut -c1-1`
 if [ "$firstchar" != "/" ]; then
  dir=$PWD/${dir}
 fi 
 #
 # list all jobs
 for dir in `ls -1d ${dir}`; do echo ${dir}; done >> ___all
 #
 # list of (un)finished jobs
 for dir in `ls -1d ${dir}`; do 
  out=${dir}/`basename ${dir}`_out.json
  if [ -f "${out}" ] && \
     [ `cat "${out}" | wc -c` -ge ${threshold} ]; then
   echo ${dir} >> ___finished 
  else
   echo ${dir} >> ___unfinished 
  fi
 done 
done
echo
echo "   Finished: "`cat ___finished|wc -l`  
echo " Unfinished: "`cat ___unfinished|wc -l`
echo "---------------------"  
echo "      Total: "`cat ___all|wc -l`
echo
echo "Exact job locations are in: ___all, ___finished, ___unfinished."
echo   
