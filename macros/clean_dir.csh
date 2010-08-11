#!/bin/csh
#
# Clean up C* and c* files to save space.
#
if ( $#argv == 0 ) then
 echo
 echo "Usage:  "`basename $0`" <CrabDir_1> [<Crab_Dir_2> ....]"
 echo
 echo "where:  <CrabDir_#> - is a directory created by 'crab -create' command."
 echo
 echo "This script will delete redundant uncompressed STDOUT, STDERR and XML"
 echo "files from all <CrabDir_#> directories. These files are duplicated in"
 echo "CRAB output files <CrabDir_#>/res/out_*.tgz."
 echo
 exit -1  
endif
foreach d ( $argv )
  echo "Removing C*.std* and crab* from "$d
  \rm $d/res/C*.std* $d/res/crab*.xml
end
