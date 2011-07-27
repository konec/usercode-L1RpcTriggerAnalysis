#!/bin/bash
. /etc/bashrc
#
# This shell script executes drawSynchro.C ROOT macro.
# Parses delays.txt file created by synchroMerger and plots: 
# - distribution of BX average for LBs 
# - distribution of BX rms for LBs 
# - cumulative distribution of LBs vs window size around L1A
#   that contains a given fraction of LB hits.
#
echo Start.
pdir=`pwd`

if [ $# -ne 3 ]; then
 echo
 echo "Usage:  "`basename $0`" <RunRange_string> <DataSet_string> <OutFiles_prefix>"
 echo
 echo "where:  <RunRange_string>, <DataSet_string> - are comments to be shown"
 echo "        in the plots."
 echo
 echo "        <OutFile_prefix> - prefix for output file names."
 echo 
 echo "The script will process 'delays_*.txt' files located in the current"
 echo "directory and produce synchronization plots in three formats:"
 echo ".root, .png and .eps. They will be stored in 'root', 'png' and 'eps'"
 echo "subdirectories respectively. For example:"
 echo "   "`basename $0`" \"140000-150000\" \"Cosmics\" \"plot\""
 echo "will produce .png files like this:"
 echo "   png/plot_Barrel+Endcap_*.png"
 echo "   png/plot_Barrel_*.png"
 echo "   png/plot_Endcap_*.png, etc, etc"
 echo "and .root files like this:"
 echo "   root/plot_Barrel+Endcap.root"
 echo "   root/plot_Barrel.root"
 echo "   root/plot_Endcap.root, etc, etc."
 echo
 exit -1
fi
#
# convert to absolute paths
cd `dirname $0` ; scriptdir=`pwd` ; cd -
input1=`pwd`
prefix=`basename $3`
string="${scriptdir}/drawSynchro.C++(\"${input1}\", \"$1\", \"$2\", \"$prefix\")"
#
if ! [ -f "${scriptdir}/drawSynchro.C" ]; then
 echo "Can't find ${scriptdir}/drawSynchro.C!"
 exit -2
fi
if ! [ -f "${scriptdir}/Style.C" ]; then
 echo "Can't find ${scriptdir}/Style.C!"
 exit -2
fi
#
root.exe -l -q "${string}"
mkdir -p root eps png
ls -lart
\mv `\ls -1 ${prefix}*.root` root/.
\mv `\ls -1 ${prefix}*.eps` eps/.
\mv `\ls -1 ${prefix}*.png` png/.

cd ${pdir}
echo Done.