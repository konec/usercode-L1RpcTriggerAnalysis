#!/bin/bash
. /etc/bashrc
#
# This shell script executes drawEff.C ROOT macro.
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
 echo "The script will process 'efficiencyHistos.root' file located in the"
 echo "current directory and produce efficiency plots in three formats:"
 echo ".root, .gif and .eps. They will be stored in 'root', 'gif' and 'eps'"
 echo "subdirectories respectively. For example:"
 echo "   "`basename $0`" \"140000-150000\" \"Cosmics\" \"ploteff\""
 echo "will produce .gif files like this:"
 echo "   gif/ploteff_*.gif"
 echo "and .root file:"
 echo "   root/ploteff.root, etc, etc."
 echo
 exit -1
fi
#
# convert to absolute paths
cd `dirname $0` ; scriptdir=`pwd` ; cd -
input1=`pwd`
prefix=`basename $3`
string="${scriptdir}/drawEff.C++(\"${input1}\", \"$1\", \"$2\", \"$prefix\")"
#
if ! [ -f "${scriptdir}/drawEff.C" ]; then
 echo "Can't find ${scriptdir}/drawEff.C!"
 exit -2
fi
if ! [ -f "${scriptdir}/Style.C" ]; then
 echo "Can't find ${scriptdir}/Style.C!"
 exit -2
fi
#
root.exe -l -q "${string}"
mkdir -p root eps gif
ls -lart
\mv `\ls -1 ${prefix}.root` root/.
\mv `\ls -1 ${prefix}_*.eps` eps/.
\mv `\ls -1 ${prefix}_*.gif` gif/.

cd ${pdir}
echo Done.