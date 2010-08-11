#!/bin/bash
. /etc/bashrc

pdir=`pwd`

if [ $# -eq 0 ]; then
 echo
 echo "Usage:  "`basename $0`" <JobDir_1> [ <JobDir_2> ... ]"
 echo
 echo "where:"
 echo "        <JobDir_#> - is a directory created by "
 echo "        'create_synchroAnalysis_batch.sh' script."
 echo "The script will check if LSFJOB* directory exists and "
 echo "if so, will move its contents to .tgz file to save disk space."
 echo
 exit -1
fi
#
echo "Checking: $# directories"
#declare -i ncomp=0
for dir in "$@"; do
 #echo $dir
 if ! [ -d "${dir}" ]; then
  continue
 fi
 cd ${dir}
 arr=(`ls -1 | grep LSFJOB_ | grep -v .tgz`)
 for lsfdir in "${arr[@]}"; do
    echo "Compressing ${dir}/${lsfdir}"
    tar czf /tmp/$USER/${lsfdir}.tgz --remove-files ${lsfdir}
    mv /tmp/$USER/${lsfdir}.tgz .
    #rm -r ${lsfdir}
    #ncomp = $(( ${ncomp}+1 ))
 done
 cd ${pdir}
done
cd ${pdir}
#echo "Compressed: ${ncomp} LSFJOB directories"