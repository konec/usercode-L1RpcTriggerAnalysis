#! /usr/local/bin/tcsh -f

if ( ! ( $?ROOTSYS ) ) eval `scram runtime -csh`

$ROOTSYS/bin/rootcint -f rootMAnaDict.cc  -c -p \
    -I../../../  \
    -I`scram tool info clhep | grep ^INCLUDE | sed 's/INCLUDE=//'` \
    UserCode/L1RpcTriggerAnalysis/interface/EventObj.h \
    UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h \
    UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h \
    UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h \
    UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h \
    UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h \
    UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h \
    UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h
