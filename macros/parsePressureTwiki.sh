#!/bin/bash
. /etc/bashrc
#
if [ $# == 0 ] ; then echo "Missing input file!" ; exit -1 ; fi
if [ $# -gt 1 ] ;  then echo "More than one input file!" ; exit -1 ; fi
if ! [ -f $1 ] ; then echo "File does not exist!" ; exit -1 ; fi
#
file1=/tmp/${USER}/tempfile_${USER}_${RANDOM}
file2=/tmp/${USER}/tempfile_${USER}_${RANDOM}
\rm -f $file1 $file2
cut $1 -d'|' -f 2 | cut -d' ' -f 2 > file1
cut $1 -d'|' -f 7 | cut -d' ' -f 2 > file2
paste file1 file2
\rm -f $file1 $file2
#
