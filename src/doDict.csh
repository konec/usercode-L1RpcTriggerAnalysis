#! /usr/local/bin/tcsh -f

if ( ! ( $?ROOTSYS ) ) eval `scram runtime -csh`

$ROOTSYS/bin/rootcint -f rootMAnaDict.cc  -c -p \
    -I../../../  \
    -I`scram tool info clhep | grep ^INCLUDE | sed 's/INCLUDE=//'` \
    UserCode/L1RpcTriggerAnalysis/interface/EventObj.h \
    UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h \
    UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h
