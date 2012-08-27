#! /bin/sh
. /etc/bashrc
# 
if [ $# -ne 1 ]; then
  echo "Usage:  $0 <file_list>"
  echo "where:"
  echo "<file_list> - a text file with list of rootuples from CMS WBM"
  echo "              containing RPC gas pressure and humidity data."
  echo
  exit -1
fi
#
root.exe -b -l << EOF
.L makePressureHumidityGraph.C++
makePressureHumidityGraph t(0,"$1","PressureGraph","HumidityGraph");
for(int i=0;i<10;i++) t.Show(i);
t.Loop();   
.q
EOF
#
ls -l PressureGraph.* HumidityGraph.*
echo "Done."