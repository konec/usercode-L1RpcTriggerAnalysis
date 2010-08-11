#!/bin/bash
. /etc/bashrc
#
if [ $# -ne 4 ] && [ $# -ne 6 ]; then
 echo
 echo "Usage:  "`basename $0`" <Rootfile_1> <Rootfile_2> \\"
 echo "        <File_with_hist_names> <Rootfile_directory_name>"
 echo 
 echo "or:     "`basename $0`" <Rootfile_1> <Rootfile_2> \\"
 echo "        <File_with_hist_names_1> <File_with_hist_names_2> \\"
 echo "        <Rootfile_directory_name_1> <Rootfile_directory_name_2>"
 echo 
 echo "This script calls 'calc_chi2_for_2_files.C' ROOT macro."
 echo "For two input .root files and for a given list of histogram names"
 echo "it computes:"
 echo " - chi2/nbins"
 echo " - shift of mean + its error"
 echo " - shift of rms + its error"
 echo " - pull of mean"
 echo " - pull of rms."
 echo "It supports: TH1F, TH2F, TH1D, TH2D, TProfile, TProfile2D."
 echo "All TH* histograms are first normalized and then compared."
 echo "All TProfile* histograms are left as-is."
 echo "If .root file contains TCanvas which name matches the list of"
 echo "histograms then the first histogram from TCanvas->GetListOfPrimitives"
 echo "is taken."
 echo
 exit -1
fi
# convert to absolute paths
cd `dirname $0` ; scriptdir=`pwd` ; cd -
cd `dirname $1` ; input1=`pwd`/`basename $1` ; cd -
cd `dirname $2` ; input2=`pwd`/`basename $2` ; cd -
#
if [ $# -eq 4 ]; then
 string="${scriptdir}/calc_chi2_for_2_files.C++(\"${input1}\", \"${input2}\", \"$3\", \"$4\")"
root.exe -l -q "${string}"
fi
if [ $# -eq 6 ]; then
 string="${scriptdir}/calc_chi2_for_2_files.C++(\"${input1}\", \"${input2}\", \"$3\", \"$4\", \"$5\", \"$6\")"
root.exe -l -q "${string}"
fi

