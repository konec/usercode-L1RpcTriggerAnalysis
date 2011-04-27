#!/bin/bash
. /etc/bashrc
#

castor=/castor/cern.ch/user/r/rybinska/CVS_TEST
#castor=/castor/cern.ch/cms/store/caf/user/ccmuon/RPC/RPCTrigger/Synchro/Runs_142928_144114/
cmsswdir=/afs/cern.ch/cms/L1/rpc/CVS_TEST/CMSSW_4_1_4_patch3/src/

scriptdir=`dirname $0`
cd ${scriptdir}; scriptdir=`pwd`; cd -
if ! [ -d "${scriptdir}" ]; then
 echo "Script directory ${scriptdir} does not exist!"
 exit -1
fi
#
#
# check number of arguments
if ! [ $# -eq 4 ]; then
 echo
 echo "Usage: "`basename $0`" <synchroAnalysis.py> <list_of_files> <json_file> <n_jobs>               "
 echo
 echo "where:                                                                "
 echo
 echo "  <synchroAnalysis.py> - is a template CMSSW analysis PYTHON file     "
 echo "  for BATCH job submission (not for CRAB).                            "
 echo
 echo "  <list_of_files> - is a text file with CASTOR files to run on.       "
 echo "  Simply copy-paste output from DBS query here.                       "
 echo
 echo "  <json_file> - is a text JSON file with selected lumi sections.      "
 echo "  Simply copy-paste output from DQM RR query here.                    "
 echo
 echo "  <n_jobs> - is the number of jobs to be created for batch submission."
 echo
 echo "  The PYTHON file must use the following mechanisms for providing     "
 echo "  input files and lumi masks:                                         "
 echo "  ... mylist = FileUtils.loadListFromFile(_LIST_OF_FILES_) ...        "
 echo "  ... Lumis = LumiList.LumiList(filename=_JSON_FILE_) ...             "
 echo "  For each job the script will create a separate directory            " 
 echo "  called job_YYMMDD_hhmm_<i> with the following files:                "
 echo "       <json_file>                                                    "
 echo "       <list_of_files>_<i>                                            "
 echo "       job_YYMMDD_hhmm_<i>.sh                                         "
 echo "       <synchroAnalysis_<i>.py>                                       "
 echo "  In the latter file strings \"_LIST_OF_FILES_\" and \"_JSON_FILE\"   "
 echo "  will be automatically replaced by '<list_of_files>_<i>' and         "
 echo "  '<json_file>'.                                                      "
 echo
 exit -1
fi
#
# 
if ! [ -f "$1" ]; then
 echo "PYTHON file $1 does not exist!"
 exit -1
fi 
if ! [ -f "$2" ]; then
 echo "File list $2 does not exist!"
 exit -1
fi 
if [ $4 -lt 1 ]; then
 echo "Number of jobs <1"
 exit -1
fi 
#
# check if $1 contains an absolute path
fileana=$1
firstchar=`echo "${fileana}" | cut -c1-1`
if ! [ "${firstchar}" = "/" ]; then
 fileana=`pwd`"/${fileana}"
fi
#
# check if $2 contains an absolute path
filelist=$2
firstchar=`echo "${filelist}" | cut -c1-1`
if ! [ "${firstchar}" = "/" ]; then
 filelist=`pwd`"/${filelist}"
fi
#
# check if $3 contains an absolute path
filejson=$3
firstchar=`echo "${filejson}" | cut -c1-1`
if ! [ "${firstchar}" = "/" ]; then
 filejson=`pwd`"/${filejson}"
fi
#
#
dir=`pwd`
dirtmp=/tmp/$USER/BATCHTEMP___
mkdir -p "${dirtmp}"
if ! [ -d "${dirtmp}" ]; then
 echo "Can't create TEMP directory ${dirtmp}"
 exit -1
fi
cd ${dirtmp}
#
# create smaller file lists
${scriptdir}/split_file_list.sh "${filelist}" $4
format="%1d"
jobdate=`date "+%y%m%d_%H%M"`
if [ $4 -gt 10 ]; then format="%02d"; fi
if [ $4 -gt 100 ]; then format="%03d"; fi
if [ $4 -gt 1000 ]; then format="%04d"; fi
for job in `seq 0 $(( $4-1 ))`; do
 echo Preparing job $job
 jobstr=`printf ${format} ${job}`
 jobdir="job_${jobdate}_${jobstr}"
 ana=`basename "${fileana}" | sed s%.py%_"${jobstr}".py%`
 list=`basename ${filelist}`"_${jobstr}"
 json=`basename ${filejson}`"_${jobstr}"
 mkdir ${jobdir}
 cp ${filejson} ${jobdir}/${json}
 mv ${list}     ${jobdir}/.
 cat "${fileana}" | sed s%_LIST_OF_FILES_%"'${list}'"% | sed s%_JSON_FILE_%"'${json}'"% > ${jobdir}/${ana}

 cat > ${jobdir}/${jobdir}.sh <<EOF
#!/bin/csh

pwd

cd $cmsswdir
cmsenv
cd -

ls -la ${dir}/${jobdir}/${ana}
ls -la ${dir}/${jobdir}/${list}
ls -la ${dir}/${jobdir}/${json}

if ( ( -f ${dir}/${jobdir}/${ana}  ) && ( -f ${dir}/${jobdir}/${list} ) && ( -f ${dir}/${jobdir}/${json} )) then
 cp ${dir}/${jobdir}/${ana} .
 cp ${dir}/${jobdir}/${list} .
 cp ${dir}/${jobdir}/${json} .


date;pwd
echo "START cmsRun" 
cmsRun ${ana} |& tee ${jobdir}.log
echo "END cmsRun"
date;pwd

 tar czf ${jobdir}_out.tgz rawMonitor.root synchroAnalysis.root efficiencyHelper.root efficiencyTree.root ${jobdir}.log
rfcp ${jobdir}_out.tgz ${castor}
echo rfcp ${jobdir}_out.tgz ${castor}
cp ${jobdir}_out.tgz ${dir}/${jobdir}/.
echo cp ${jobdir}_out.tgz ${dir}/${jobdir}/.



 echo Done.

else

 echo "Can't find files:"
 echo ${dir}/${jobdir}/${ana}
 echo ${dir}/${jobdir}/${list}
 echo ${dir}/${jobdir}/${json}

 echo Error!

endif

EOF
chmod +x ${jobdir}/${jobdir}.sh
cp -r ${jobdir} ${dir}/.
done
ls -la */.
cd -
\rm -rf ${dirtmp}
