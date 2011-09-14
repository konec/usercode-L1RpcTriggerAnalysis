#!/bin/bash
. /etc/bashrc
rm -f histo_list run_list
cat pressure_twiki_*-*.dat | cut -c1-6 >> run_list
for f in `cat run_list` 
   do echo "MonitorHistos/Histos_SC_run_${f}_"'*'".root" >> histo_list
done
