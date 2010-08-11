#!/bin/csh
#
set cmsenvdir=${CMSSW_BASE}/src
#
# select the right CAF queue: 
#set queue=cmscaf1nh
set queue=cmscaf1nd
#set queue=cmscaf1nw
#
if ($#argv<1) then
 echo
 echo "Usage:  "`basename $0`" <JobDir_1> [ <JobDir_2> ... ]"
 echo
 echo "where:"
 echo "        <JobDir_#> - is a directory created by "
 echo "        'create_synchroAnalysis_batch.sh' script."
 echo
 echo "The script will submit jobs (one job per given directory)"
 echo "to the ${queue} queue."
 echo "NOTE: Initialize CMSSW environment beforehand!"
 echo
 exit -1
endif
#
if ( ( "${cmsenvdir}" == "" ) || ! ( -d "${cmsenvdir}") ) then
 echo "CMSSW environment not set!"
 exit -1
endif 
#
set pdir=`pwd`
foreach i ( `seq 1 $#argv` )
 set dir=$argv[$i]
 echo $dir - start. 
 cd $dir;
 set filecmd=`basename ${dir}`.sh
 if ! ( -f "${filecmd}" ) then
  echo "Batch command file ${filecmd} does not exist!"
  continue
 endif
 #
 # this will suppress creating LSFJOB_xxxxxx directory 
 # (saves disk space, job status will be sent by email)
 bsub -o /tmp/$USER/job_%J -q ${queue} ${filecmd}
 #
 # this will produce LSFJOB_xxxxxx directory with STDOUT 
 #bsub -q ${queue} ${filecmd}
 echo $dir - done.
 cd -
end
cd ${pdir}
