#!/bin/bash
. /etc/bashrc
#
cmsenvdir=${CMSSW_BASE}/src
#
#
# Scans multiple <CrabDir_#> directories,
# combines JSON files for actually processed luminosity blocks,
# prints recorded luminosity in 1/microbarn units.
#
# check number of arguments
if [ $# -eq 0 ]; then
 echo
 echo "Usage: "`basename $0`" <CrabDir_1> [<CrabDir_2] ...]"
 echo
 echo "where:"
 echo "        <CrabDir_#> - is a directory created by "
 echo "        'crab -create' command and contains a valid output from "
 echo "        'crab -report' command"
 echo "        i.e. <CrabDir_#>/res/lumiSummary.json file MUST be present."
 echo 
 echo "The script will do logical OR of all <CrabDir_#>/res/lumiSummary.json"
 echo "files and create 'lumi_report.json' file."
 echo "Then it will call lumiCalc.py script and create recorded luminosity"
 echo "report in 'lumi_report.log' file. The recorded luminosity is given in"
 echo "inverse microbarns."
 echo
 exit -1
fi
#
if [ "${cmsenvdir}" == "" ] || ! [ -d "${cmsenvdir}" ] ; then
 echo "CMSSW environment not set!!!"
 exit -2
fi
#
combine=${cmsenvdir}/FWCore/PythonUtilities/scripts/combine_JSON.py
if ! [ -f "${combine}" ]; then
 echo "Can't find ${combine} script"
 exit -3
else
 combine=`basename ${combine}`
fi
#
lumicalc=${cmsenvdir}/RecoLuminosity/LumiDB/scripts/lumiCalc.py
if ! [ -f "${lumicalc}" ]; then
 echo "Can't find ${lumicalc} script"
 exit -4
else
 lumicalc=`basename ${lumicalc}`
fi
#
pdir=`pwd`
dirtmp=/tmp/$USER/BATCHTEMP___${RANDOM}___`date +%s`
outjson=${dirtmp}/lumi_report.json
outlog=${dirtmp}/lumi_report.log
mkdir -p "${dirtmp}"
if ! [ -d "${dirtmp}" ]; then
 echo "Can't create TEMP directory ${dirtmp}"
 exit -1
fi
#
cd ${cmsenvdir} ; pwd ; eval `scramv1 runtime -sh`; cd - 
cd ${dirtmp}
#
\rm -f ${outlog} ${outjson} ${outjson}.new ${outjson}.old
for j in "$@"; do
 #
 # check if this is an absolute path
 dirjob="${j}/res"
 echo "Checking ${dirjob}..." 
 firstchar=`echo "${dirjob}" | cut -c1-1`
 if ! [ "${firstchar}" = "/" ]; then
  dirjob="${pdir}/${j}/res"
 fi
 json="${dirjob}/lumiSummary.json"
 if ! [ -f "${json}" ]; then
  echo "Can't find JSON file ${json}"
  continue
 fi
 if ! [ -f ${outjson}.old ]; then
     ${combine} -a ${json} -b ${json} -o ${outjson}.new -r or
 else
     ${combine} -a ${json} -b ${outjson}.old -o ${outjson}.new -r or
 fi
 if [ -f ${outjson}.new ]; then
     mv ${outjson}.new ${outjson}.old
 fi 
done
mv ${outjson}.old  ${outjson}
cd ${dirtmp}
#
${lumicalc} -c frontier://LumiProd/CMS_LUMI_PROD -i ${outjson} recorded >& ${outlog}
cp ${outjson} ${outlog} ${pdir}/.
cat ${outlog}
\rm -rf ${dirtmp}
#
cd ${pdir}
echo Done.