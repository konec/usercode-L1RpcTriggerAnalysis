#!/bin/bash
. /etc/bashrc
#
cmsenvdir=${CMSSW_BASE}/src
#
scriptdir=`dirname $0`
cd ${scriptdir}; scriptdir=`pwd`; cd -
if ! [ -d "${scriptdir}" ]; then
 echo "Script directory ${scriptdir} does not exist!"
 exit -1
fi
#
# check number of arguments
if ! [ $# -eq 4 ]; then
 echo
 echo "Usage: "`basename $0`" <synchroAnalysis.py> <list_of_files> <json_file> <n_files_per_job>"
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
 echo "  <n_files_per_job> - is the maximal number of files per job. Number  "
 echo "  of jobs will be set automatically.                                  "
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
 echo "  NOTE: Initialize CMSSW environment beforehand!"
 echo
 exit -1
fi
#
if [ "${cmsenvdir}" == "" ] || ! [ -d "${cmsenvdir}" ] ; then
 echo "CMSSW environment not set!"
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
 echo "Number of files per job <1"
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
format="%04d"
jobdate=`date "+%y%m%d_%H%M"`
prefix=`basename ${filelist}`
njobs=`ls -1 ${prefix}_* | wc -l`
for job in `seq 0 $(( ${njobs}-1 ))`; do
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
#BSUB -q 1nh

echo 'Start'

ls -la ${dir}/${jobdir}/${ana}
ls -la ${dir}/${jobdir}/${list}
ls -la ${dir}/${jobdir}/${json}

if ( ( -f ${dir}/${jobdir}/${ana}  ) && ( -f ${dir}/${jobdir}/${list} ) && ( -f ${dir}/${jobdir}/${json} )) then
 cp ${dir}/${jobdir}/${ana} .
 cp ${dir}/${jobdir}/${list} .
 cp ${dir}/${jobdir}/${json} .

 cd ${CMSSW_BASE}/src/UserCode/L1RpcTriggerAnalysis/test; cmsenv ; cd -
 cmsRun ${ana} > ${jobdir}.log

 tar czf ${jobdir}_out.tgz rawMonitor.root synchroAnalysis.root efficiencyHelper.root efficiencyTree.root ${jobdir}.log
 rfcp ${jobdir}_out.tgz ${dir}/${jobdir}/.

 echo 'Done.'

else

 echo "Can't find files:"
 echo ${dir}/${jobdir}/${ana}
 echo ${dir}/${jobdir}/${list}
 echo ${dir}/${jobdir}/${json}

 echo 'Error!'

endif

EOF
chmod +x ${jobdir}/${jobdir}.sh
cp -r ${jobdir} ${dir}/.
done
ls -la */.
cd -
\rm -rf ${dirtmp}