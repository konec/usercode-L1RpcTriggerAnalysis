#!/bin/bash
. /etc/bashrc
#
#
if [ $# -ne 1 ]; then
 echo
 echo "Usage:  "`basename $0`" <delays>.txt"
 echo
 echo "where:"
 echo "        <delays>.txt - is a text file with BX hit distributions"
 echo "        after running 'cmsRun synchroMerger.py'."
 echo
 echo "The script will create several <delays>_xxx.txt files"
 echo "where xxx stands for the following geographic locations:"
 echo "  Barrel"
 echo "  Barrel_RB1 ... Barrel_RB4"
 echo "  Barrel_W-2 ... Barrel_W+2"
 echo "  Endcap"
 echo "  Endcap_Neg Endcap_Pos"
 echo "  Endcap_RE1 ... Endcap_RE3"
 echo "  Endcap_RE-3 ... Endcap_RE+3."
 echo
 exit -1
fi
#
if ! [ -f $1 ]; then 
 echo "Can't read input file $1!"
 exit -1
fi
pdir=`pwd`
cd `dirname $1`
input=`pwd`/`basename $1`
prefix=`basename $1 .txt`
cd -
# 
cat "${input}" | grep 'LB_RB-2' > "${prefix}_Barrel_W-2.txt" 
cat "${input}" | grep 'LB_RB-1' > "${prefix}_Barrel_W-1.txt" 
cat "${input}" | grep 'LB_RB0'  > "${prefix}_Barrel_W0.txt" 
cat "${input}" | grep 'LB_RB+1' > "${prefix}_Barrel_W+1.txt" 
cat "${input}" | grep 'LB_RB+2' > "${prefix}_Barrel_W+2.txt" 
cat "${prefix}_Barrel_W-2.txt" \
    "${prefix}_Barrel_W-1.txt" \
    "${prefix}_Barrel_W0.txt" \
    "${prefix}_Barrel_W+1.txt" \
    "${prefix}_Barrel_W+2.txt" > "${prefix}_Barrel.txt" 
cat "${input}" | grep 'LB_RB' | grep "/RB1" > "${prefix}_Barrel_RB1.txt" 
cat "${input}" | grep 'LB_RB' | grep "/RB2" > "${prefix}_Barrel_RB2.txt" 
cat "${input}" | grep 'LB_RB' | grep "/RB3" > "${prefix}_Barrel_RB3.txt" 
cat "${input}" | grep 'LB_RB' | grep "/RB4" > "${prefix}_Barrel_RB4.txt" 

cat "${input}" | grep 'LB_RE-1' | grep 'RE-1/' > "${prefix}_Endcap_RE-1.txt" 
cat "${input}" | grep 'LB_RE-1' | grep 'RE-2/' > "${prefix}_Endcap_RE-2.txt" 
cat "${input}" | grep 'LB_RE-3' | grep 'RE-3/' > "${prefix}_Endcap_RE-3.txt" 
cat "${input}" | grep 'LB_RE+1' | grep 'RE+1/' > "${prefix}_Endcap_RE+1.txt" 
cat "${input}" | grep 'LB_RE+1' | grep 'RE+2/' > "${prefix}_Endcap_RE+2.txt" 
cat "${input}" | grep 'LB_RE+3' | grep 'RE+3/' > "${prefix}_Endcap_RE+3.txt" 
cat "${prefix}_Endcap_RE-1.txt" "${prefix}_Endcap_RE+1.txt" > "${prefix}_Endcap_RE1.txt"
cat "${prefix}_Endcap_RE-2.txt" "${prefix}_Endcap_RE+2.txt" > "${prefix}_Endcap_RE2.txt"
cat "${prefix}_Endcap_RE-3.txt" "${prefix}_Endcap_RE+3.txt" > "${prefix}_Endcap_RE3.txt"
cat "${prefix}_Endcap_RE-1.txt" \
    "${prefix}_Endcap_RE-2.txt" \
    "${prefix}_Endcap_RE-3.txt" > "${prefix}_Endcap_Neg.txt"
cat "${prefix}_Endcap_RE+1.txt" \
    "${prefix}_Endcap_RE+2.txt" \
    "${prefix}_Endcap_RE+3.txt" > "${prefix}_Endcap_Pos.txt"
cat "${prefix}_Endcap_Neg.txt" \
    "${prefix}_Endcap_Pos.txt" > "${prefix}_Endcap.txt"
cd ${pdir}