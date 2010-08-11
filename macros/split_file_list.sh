#!/bin/bash
. /etc/bashrc
#
# splits input file list into N pieces
#
if [ $# -ne 2 ]; then
 echo
 echo "Usage:  "`basename $0`" <File_list> <Max_files_per_job>"
 echo
 exit -1
fi
if ! [ -f "$1"  ]; then 
 echo "File $1 does not exist!"
 exit -2
fi
declare -i ntot=`cat $1|wc -l`
declare -i nperjob=$2
if [ $ntot -lt 1 ]; then 
 echo "Total number of files must be >= 1."
 exit -3
fi
if [ $nperjob -lt 1 ]; then 
 echo "Number of files per job must be >= 1."
 exit -4
fi
#
declare -i nsuffix=4
prefix=`basename $1`_
split -d -a $nsuffix -l $nperjob "$1" "${prefix}"
#
echo Done.
