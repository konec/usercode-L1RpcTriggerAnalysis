#!/bin/bash
. /etc/bashrc
#
# splits input file list into N pieces
#
if [ $# -ne 2 ]; then
 echo
 echo "Usage:  "`basename $0`" <Input_file> <N_pieces>"
 echo
 exit -1
fi
if ! [ -f "$1"  ]; then 
 echo "File $1 does not exist!"
 exit -2
fi
declare -i ntot=`cat $1|wc -l`
if [ $ntot -lt $2 ]; then 
 echo "Number of pieces ($2) is less than number of files ($ntot)!"
 exit -3
fi
#
# 
#
declare -i nperjob=$(( ${ntot} / ${2} ))
declare -i reminder=$(( ${ntot} - ${nperjob}*${2} ))
#echo $2, $ntot, $nperjob, $reminder
if [ ${reminder} -gt 0 ] && [ ${2} -gt 1 ]; then
 nperjob=$(( ${nperjob}+1 )) 
 reminder=$(( ${ntot} - ${nperjob}*${2} ))
fi
#echo $2, $ntot, $nperjob, $reminder
#
declare -i nsuffix=1
if [ $2 -gt 10 ]; then nsuffix=2; fi
if [ $2 -gt 100 ]; then nsuffix=3; fi
if [ $2 -gt 1000 ]; then nsuffix=4; fi
#
prefix=`basename $1`_
split -d -a $nsuffix -l $nperjob "$1" "${prefix}"
