#!/bin/bash
. /etc/bashrc
#
# converts name returned by DBS search
# into CASTOR format
#
array=( `cat $1` )
n=${#array}
#
for f in "${array[@]}"; do
 echo "/castor/cern.ch/cms"$f
done


 

