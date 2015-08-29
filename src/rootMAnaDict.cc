// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME rootMAnaDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventData.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObjVect.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_EventObj(void *p = 0);
   static void *newArray_EventObj(Long_t size, void *p);
   static void delete_EventObj(void *p);
   static void deleteArray_EventObj(void *p);
   static void destruct_EventObj(void *p);
   static void streamer_EventObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EventObj*)
   {
      ::EventObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::EventObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("EventObj", ::EventObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h", 8,
                  typeid(::EventObj), DefineBehavior(ptr, ptr),
                  &::EventObj::Dictionary, isa_proxy, 16,
                  sizeof(::EventObj) );
      instance.SetNew(&new_EventObj);
      instance.SetNewArray(&newArray_EventObj);
      instance.SetDelete(&delete_EventObj);
      instance.SetDeleteArray(&deleteArray_EventObj);
      instance.SetDestructor(&destruct_EventObj);
      instance.SetStreamerFunc(&streamer_EventObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EventObj*)
   {
      return GenerateInitInstanceLocal((::EventObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::EventObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_EventData(void *p = 0);
   static void *newArray_EventData(Long_t size, void *p);
   static void delete_EventData(void *p);
   static void deleteArray_EventData(void *p);
   static void destruct_EventData(void *p);
   static void streamer_EventData(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EventData*)
   {
      ::EventData *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::EventData >(0);
      static ::ROOT::TGenericClassInfo 
         instance("EventData", ::EventData::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/EventData.h", 11,
                  typeid(::EventData), DefineBehavior(ptr, ptr),
                  &::EventData::Dictionary, isa_proxy, 16,
                  sizeof(::EventData) );
      instance.SetNew(&new_EventData);
      instance.SetNewArray(&newArray_EventData);
      instance.SetDelete(&delete_EventData);
      instance.SetDeleteArray(&deleteArray_EventData);
      instance.SetDestructor(&destruct_EventData);
      instance.SetStreamerFunc(&streamer_EventData);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EventData*)
   {
      return GenerateInitInstanceLocal((::EventData*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::EventData*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TrackObj(void *p = 0);
   static void *newArray_TrackObj(Long_t size, void *p);
   static void delete_TrackObj(void *p);
   static void deleteArray_TrackObj(void *p);
   static void destruct_TrackObj(void *p);
   static void streamer_TrackObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TrackObj*)
   {
      ::TrackObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TrackObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TrackObj", ::TrackObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h", 6,
                  typeid(::TrackObj), DefineBehavior(ptr, ptr),
                  &::TrackObj::Dictionary, isa_proxy, 16,
                  sizeof(::TrackObj) );
      instance.SetNew(&new_TrackObj);
      instance.SetNewArray(&newArray_TrackObj);
      instance.SetDelete(&delete_TrackObj);
      instance.SetDeleteArray(&deleteArray_TrackObj);
      instance.SetDestructor(&destruct_TrackObj);
      instance.SetStreamerFunc(&streamer_TrackObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TrackObj*)
   {
      return GenerateInitInstanceLocal((::TrackObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TrackObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MuonObj(void *p = 0);
   static void *newArray_MuonObj(Long_t size, void *p);
   static void delete_MuonObj(void *p);
   static void deleteArray_MuonObj(void *p);
   static void destruct_MuonObj(void *p);
   static void streamer_MuonObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MuonObj*)
   {
      ::MuonObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MuonObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("MuonObj", ::MuonObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h", 6,
                  typeid(::MuonObj), DefineBehavior(ptr, ptr),
                  &::MuonObj::Dictionary, isa_proxy, 16,
                  sizeof(::MuonObj) );
      instance.SetNew(&new_MuonObj);
      instance.SetNewArray(&newArray_MuonObj);
      instance.SetDelete(&delete_MuonObj);
      instance.SetDeleteArray(&deleteArray_MuonObj);
      instance.SetDestructor(&destruct_MuonObj);
      instance.SetStreamerFunc(&streamer_MuonObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MuonObj*)
   {
      return GenerateInitInstanceLocal((::MuonObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MuonObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_L1Obj(void *p = 0);
   static void *newArray_L1Obj(Long_t size, void *p);
   static void delete_L1Obj(void *p);
   static void deleteArray_L1Obj(void *p);
   static void destruct_L1Obj(void *p);
   static void streamer_L1Obj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::L1Obj*)
   {
      ::L1Obj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::L1Obj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("L1Obj", ::L1Obj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h", 6,
                  typeid(::L1Obj), DefineBehavior(ptr, ptr),
                  &::L1Obj::Dictionary, isa_proxy, 16,
                  sizeof(::L1Obj) );
      instance.SetNew(&new_L1Obj);
      instance.SetNewArray(&newArray_L1Obj);
      instance.SetDelete(&delete_L1Obj);
      instance.SetDeleteArray(&deleteArray_L1Obj);
      instance.SetDestructor(&destruct_L1Obj);
      instance.SetStreamerFunc(&streamer_L1Obj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::L1Obj*)
   {
      return GenerateInitInstanceLocal((::L1Obj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::L1Obj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_L1ObjColl(void *p = 0);
   static void *newArray_L1ObjColl(Long_t size, void *p);
   static void delete_L1ObjColl(void *p);
   static void deleteArray_L1ObjColl(void *p);
   static void destruct_L1ObjColl(void *p);
   static void streamer_L1ObjColl(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::L1ObjColl*)
   {
      ::L1ObjColl *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::L1ObjColl >(0);
      static ::ROOT::TGenericClassInfo 
         instance("L1ObjColl", ::L1ObjColl::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h", 9,
                  typeid(::L1ObjColl), DefineBehavior(ptr, ptr),
                  &::L1ObjColl::Dictionary, isa_proxy, 16,
                  sizeof(::L1ObjColl) );
      instance.SetNew(&new_L1ObjColl);
      instance.SetNewArray(&newArray_L1ObjColl);
      instance.SetDelete(&delete_L1ObjColl);
      instance.SetDeleteArray(&deleteArray_L1ObjColl);
      instance.SetDestructor(&destruct_L1ObjColl);
      instance.SetStreamerFunc(&streamer_L1ObjColl);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::L1ObjColl*)
   {
      return GenerateInitInstanceLocal((::L1ObjColl*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::L1ObjColl*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_SynchroCountsObj(void *p = 0);
   static void *newArray_SynchroCountsObj(Long_t size, void *p);
   static void delete_SynchroCountsObj(void *p);
   static void deleteArray_SynchroCountsObj(void *p);
   static void destruct_SynchroCountsObj(void *p);
   static void streamer_SynchroCountsObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SynchroCountsObj*)
   {
      ::SynchroCountsObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SynchroCountsObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("SynchroCountsObj", ::SynchroCountsObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h", 6,
                  typeid(::SynchroCountsObj), DefineBehavior(ptr, ptr),
                  &::SynchroCountsObj::Dictionary, isa_proxy, 16,
                  sizeof(::SynchroCountsObj) );
      instance.SetNew(&new_SynchroCountsObj);
      instance.SetNewArray(&newArray_SynchroCountsObj);
      instance.SetDelete(&delete_SynchroCountsObj);
      instance.SetDeleteArray(&deleteArray_SynchroCountsObj);
      instance.SetDestructor(&destruct_SynchroCountsObj);
      instance.SetStreamerFunc(&streamer_SynchroCountsObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SynchroCountsObj*)
   {
      return GenerateInitInstanceLocal((::SynchroCountsObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::SynchroCountsObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_SynchroCountsObjVect(void *p = 0);
   static void *newArray_SynchroCountsObjVect(Long_t size, void *p);
   static void delete_SynchroCountsObjVect(void *p);
   static void deleteArray_SynchroCountsObjVect(void *p);
   static void destruct_SynchroCountsObjVect(void *p);
   static void streamer_SynchroCountsObjVect(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SynchroCountsObjVect*)
   {
      ::SynchroCountsObjVect *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SynchroCountsObjVect >(0);
      static ::ROOT::TGenericClassInfo 
         instance("SynchroCountsObjVect", ::SynchroCountsObjVect::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h", 9,
                  typeid(::SynchroCountsObjVect), DefineBehavior(ptr, ptr),
                  &::SynchroCountsObjVect::Dictionary, isa_proxy, 16,
                  sizeof(::SynchroCountsObjVect) );
      instance.SetNew(&new_SynchroCountsObjVect);
      instance.SetNewArray(&newArray_SynchroCountsObjVect);
      instance.SetDelete(&delete_SynchroCountsObjVect);
      instance.SetDeleteArray(&deleteArray_SynchroCountsObjVect);
      instance.SetDestructor(&destruct_SynchroCountsObjVect);
      instance.SetStreamerFunc(&streamer_SynchroCountsObjVect);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SynchroCountsObjVect*)
   {
      return GenerateInitInstanceLocal((::SynchroCountsObjVect*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::SynchroCountsObjVect*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_DetCluDigiObj(void *p = 0);
   static void *newArray_DetCluDigiObj(Long_t size, void *p);
   static void delete_DetCluDigiObj(void *p);
   static void deleteArray_DetCluDigiObj(void *p);
   static void destruct_DetCluDigiObj(void *p);
   static void streamer_DetCluDigiObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DetCluDigiObj*)
   {
      ::DetCluDigiObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::DetCluDigiObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("DetCluDigiObj", ::DetCluDigiObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h", 6,
                  typeid(::DetCluDigiObj), DefineBehavior(ptr, ptr),
                  &::DetCluDigiObj::Dictionary, isa_proxy, 16,
                  sizeof(::DetCluDigiObj) );
      instance.SetNew(&new_DetCluDigiObj);
      instance.SetNewArray(&newArray_DetCluDigiObj);
      instance.SetDelete(&delete_DetCluDigiObj);
      instance.SetDeleteArray(&deleteArray_DetCluDigiObj);
      instance.SetDestructor(&destruct_DetCluDigiObj);
      instance.SetStreamerFunc(&streamer_DetCluDigiObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DetCluDigiObj*)
   {
      return GenerateInitInstanceLocal((::DetCluDigiObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::DetCluDigiObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_DetCluDigiObjVect(void *p = 0);
   static void *newArray_DetCluDigiObjVect(Long_t size, void *p);
   static void delete_DetCluDigiObjVect(void *p);
   static void deleteArray_DetCluDigiObjVect(void *p);
   static void destruct_DetCluDigiObjVect(void *p);
   static void streamer_DetCluDigiObjVect(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DetCluDigiObjVect*)
   {
      ::DetCluDigiObjVect *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::DetCluDigiObjVect >(0);
      static ::ROOT::TGenericClassInfo 
         instance("DetCluDigiObjVect", ::DetCluDigiObjVect::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObjVect.h", 9,
                  typeid(::DetCluDigiObjVect), DefineBehavior(ptr, ptr),
                  &::DetCluDigiObjVect::Dictionary, isa_proxy, 16,
                  sizeof(::DetCluDigiObjVect) );
      instance.SetNew(&new_DetCluDigiObjVect);
      instance.SetNewArray(&newArray_DetCluDigiObjVect);
      instance.SetDelete(&delete_DetCluDigiObjVect);
      instance.SetDeleteArray(&deleteArray_DetCluDigiObjVect);
      instance.SetDestructor(&destruct_DetCluDigiObjVect);
      instance.SetStreamerFunc(&streamer_DetCluDigiObjVect);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DetCluDigiObjVect*)
   {
      return GenerateInitInstanceLocal((::DetCluDigiObjVect*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::DetCluDigiObjVect*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TriggerMenuResultObj(void *p = 0);
   static void *newArray_TriggerMenuResultObj(Long_t size, void *p);
   static void delete_TriggerMenuResultObj(void *p);
   static void deleteArray_TriggerMenuResultObj(void *p);
   static void destruct_TriggerMenuResultObj(void *p);
   static void streamer_TriggerMenuResultObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TriggerMenuResultObj*)
   {
      ::TriggerMenuResultObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TriggerMenuResultObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TriggerMenuResultObj", ::TriggerMenuResultObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h", 8,
                  typeid(::TriggerMenuResultObj), DefineBehavior(ptr, ptr),
                  &::TriggerMenuResultObj::Dictionary, isa_proxy, 16,
                  sizeof(::TriggerMenuResultObj) );
      instance.SetNew(&new_TriggerMenuResultObj);
      instance.SetNewArray(&newArray_TriggerMenuResultObj);
      instance.SetDelete(&delete_TriggerMenuResultObj);
      instance.SetDeleteArray(&deleteArray_TriggerMenuResultObj);
      instance.SetDestructor(&destruct_TriggerMenuResultObj);
      instance.SetStreamerFunc(&streamer_TriggerMenuResultObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TriggerMenuResultObj*)
   {
      return GenerateInitInstanceLocal((::TriggerMenuResultObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TriggerMenuResultObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_HitSpecObj(void *p = 0);
   static void *newArray_HitSpecObj(Long_t size, void *p);
   static void delete_HitSpecObj(void *p);
   static void deleteArray_HitSpecObj(void *p);
   static void destruct_HitSpecObj(void *p);
   static void streamer_HitSpecObj(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::HitSpecObj*)
   {
      ::HitSpecObj *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::HitSpecObj >(0);
      static ::ROOT::TGenericClassInfo 
         instance("HitSpecObj", ::HitSpecObj::Class_Version(), "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h", 8,
                  typeid(::HitSpecObj), DefineBehavior(ptr, ptr),
                  &::HitSpecObj::Dictionary, isa_proxy, 16,
                  sizeof(::HitSpecObj) );
      instance.SetNew(&new_HitSpecObj);
      instance.SetNewArray(&newArray_HitSpecObj);
      instance.SetDelete(&delete_HitSpecObj);
      instance.SetDeleteArray(&deleteArray_HitSpecObj);
      instance.SetDestructor(&destruct_HitSpecObj);
      instance.SetStreamerFunc(&streamer_HitSpecObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::HitSpecObj*)
   {
      return GenerateInitInstanceLocal((::HitSpecObj*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::HitSpecObj*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr EventObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *EventObj::Class_Name()
{
   return "EventObj";
}

//______________________________________________________________________________
const char *EventObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int EventObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *EventObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *EventObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr EventData::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *EventData::Class_Name()
{
   return "EventData";
}

//______________________________________________________________________________
const char *EventData::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventData*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int EventData::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventData*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *EventData::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventData*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *EventData::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventData*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TrackObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TrackObj::Class_Name()
{
   return "TrackObj";
}

//______________________________________________________________________________
const char *TrackObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TrackObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TrackObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TrackObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TrackObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TrackObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TrackObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TrackObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MuonObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MuonObj::Class_Name()
{
   return "MuonObj";
}

//______________________________________________________________________________
const char *MuonObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MuonObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MuonObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MuonObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MuonObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MuonObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MuonObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MuonObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr L1Obj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *L1Obj::Class_Name()
{
   return "L1Obj";
}

//______________________________________________________________________________
const char *L1Obj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::L1Obj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int L1Obj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::L1Obj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *L1Obj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::L1Obj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *L1Obj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::L1Obj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr L1ObjColl::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *L1ObjColl::Class_Name()
{
   return "L1ObjColl";
}

//______________________________________________________________________________
const char *L1ObjColl::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::L1ObjColl*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int L1ObjColl::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::L1ObjColl*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *L1ObjColl::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::L1ObjColl*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *L1ObjColl::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::L1ObjColl*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr SynchroCountsObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *SynchroCountsObj::Class_Name()
{
   return "SynchroCountsObj";
}

//______________________________________________________________________________
const char *SynchroCountsObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int SynchroCountsObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SynchroCountsObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SynchroCountsObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr SynchroCountsObjVect::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *SynchroCountsObjVect::Class_Name()
{
   return "SynchroCountsObjVect";
}

//______________________________________________________________________________
const char *SynchroCountsObjVect::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObjVect*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int SynchroCountsObjVect::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObjVect*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SynchroCountsObjVect::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObjVect*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SynchroCountsObjVect::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SynchroCountsObjVect*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr DetCluDigiObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *DetCluDigiObj::Class_Name()
{
   return "DetCluDigiObj";
}

//______________________________________________________________________________
const char *DetCluDigiObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int DetCluDigiObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *DetCluDigiObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *DetCluDigiObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr DetCluDigiObjVect::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *DetCluDigiObjVect::Class_Name()
{
   return "DetCluDigiObjVect";
}

//______________________________________________________________________________
const char *DetCluDigiObjVect::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObjVect*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int DetCluDigiObjVect::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObjVect*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *DetCluDigiObjVect::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObjVect*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *DetCluDigiObjVect::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DetCluDigiObjVect*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TriggerMenuResultObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TriggerMenuResultObj::Class_Name()
{
   return "TriggerMenuResultObj";
}

//______________________________________________________________________________
const char *TriggerMenuResultObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TriggerMenuResultObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TriggerMenuResultObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TriggerMenuResultObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TriggerMenuResultObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TriggerMenuResultObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TriggerMenuResultObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TriggerMenuResultObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr HitSpecObj::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *HitSpecObj::Class_Name()
{
   return "HitSpecObj";
}

//______________________________________________________________________________
const char *HitSpecObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HitSpecObj*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int HitSpecObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::HitSpecObj*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *HitSpecObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HitSpecObj*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *HitSpecObj::Class()
{
   if (!fgIsA) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::HitSpecObj*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void EventObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> bx;
      R__b >> id;
      R__b >> lumi;
      R__b >> run;
      R__b >> time;
      R__b >> orbit;
      R__b.CheckByteCount(R__s, R__c, EventObj::IsA());
   } else {
      R__c = R__b.WriteVersion(EventObj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << bx;
      R__b << id;
      R__b << lumi;
      R__b << run;
      R__b << time;
      R__b << orbit;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_EventObj(void *p) {
      return  p ? new(p) ::EventObj : new ::EventObj;
   }
   static void *newArray_EventObj(Long_t nElements, void *p) {
      return p ? new(p) ::EventObj[nElements] : new ::EventObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_EventObj(void *p) {
      delete ((::EventObj*)p);
   }
   static void deleteArray_EventObj(void *p) {
      delete [] ((::EventObj*)p);
   }
   static void destruct_EventObj(void *p) {
      typedef ::EventObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_EventObj(TBuffer &buf, void *obj) {
      ((::EventObj*)obj)->::EventObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::EventObj

//______________________________________________________________________________
void EventData::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventData.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> weight;
      R__b >> pt;
      R__b >> eta;
      R__b >> phi;
      R__b >> phiHit;
      R__b >> etaHit;
      R__b >> charge;
      {
         vector<L1Obj> &R__stl =  l1ObjectsOtf;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            L1Obj R__t;
            R__t.Streamer(R__b);
            R__stl.push_back(R__t);
         }
      }
      {
         vector<L1Obj> &R__stl =  l1ObjectsGmt;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            L1Obj R__t;
            R__t.Streamer(R__b);
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, EventData::IsA());
   } else {
      R__c = R__b.WriteVersion(EventData::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << weight;
      R__b << pt;
      R__b << eta;
      R__b << phi;
      R__b << phiHit;
      R__b << etaHit;
      R__b << charge;
      {
         vector<L1Obj> &R__stl =  l1ObjectsOtf;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<L1Obj>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            ((L1Obj&)(*R__k)).Streamer(R__b);
            }
         }
      }
      {
         vector<L1Obj> &R__stl =  l1ObjectsGmt;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<L1Obj>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            ((L1Obj&)(*R__k)).Streamer(R__b);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_EventData(void *p) {
      return  p ? new(p) ::EventData : new ::EventData;
   }
   static void *newArray_EventData(Long_t nElements, void *p) {
      return p ? new(p) ::EventData[nElements] : new ::EventData[nElements];
   }
   // Wrapper around operator delete
   static void delete_EventData(void *p) {
      delete ((::EventData*)p);
   }
   static void deleteArray_EventData(void *p) {
      delete [] ((::EventData*)p);
   }
   static void destruct_EventData(void *p) {
      typedef ::EventData current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_EventData(TBuffer &buf, void *obj) {
      ((::EventData*)obj)->::EventData::Streamer(buf);
   }
} // end of namespace ROOT for class ::EventData

//______________________________________________________________________________
void TrackObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class TrackObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> thePt;
      R__b >> theEta;
      R__b >> thePhi;
      R__b >> theCharge;
      R__b.CheckByteCount(R__s, R__c, TrackObj::IsA());
   } else {
      R__c = R__b.WriteVersion(TrackObj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << thePt;
      R__b << theEta;
      R__b << thePhi;
      R__b << theCharge;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TrackObj(void *p) {
      return  p ? new(p) ::TrackObj : new ::TrackObj;
   }
   static void *newArray_TrackObj(Long_t nElements, void *p) {
      return p ? new(p) ::TrackObj[nElements] : new ::TrackObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_TrackObj(void *p) {
      delete ((::TrackObj*)p);
   }
   static void deleteArray_TrackObj(void *p) {
      delete [] ((::TrackObj*)p);
   }
   static void destruct_TrackObj(void *p) {
      typedef ::TrackObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TrackObj(TBuffer &buf, void *obj) {
      ((::TrackObj*)obj)->::TrackObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::TrackObj

//______________________________________________________________________________
void MuonObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class MuonObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TrackObj::Streamer(R__b);
      R__b >> nRPCHits;
      R__b >> nDTHits;
      R__b >> nCSCHits;
      R__b >> nTrackerHits;
      R__b >> nMatchedStations;
      R__b >> isUnique;
      R__b >> nAllMuons;
      R__b >> theMuonBits;
      R__b.CheckByteCount(R__s, R__c, MuonObj::IsA());
   } else {
      R__c = R__b.WriteVersion(MuonObj::IsA(), kTRUE);
      TrackObj::Streamer(R__b);
      R__b << nRPCHits;
      R__b << nDTHits;
      R__b << nCSCHits;
      R__b << nTrackerHits;
      R__b << nMatchedStations;
      R__b << isUnique;
      R__b << nAllMuons;
      R__b << theMuonBits;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MuonObj(void *p) {
      return  p ? new(p) ::MuonObj : new ::MuonObj;
   }
   static void *newArray_MuonObj(Long_t nElements, void *p) {
      return p ? new(p) ::MuonObj[nElements] : new ::MuonObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_MuonObj(void *p) {
      delete ((::MuonObj*)p);
   }
   static void deleteArray_MuonObj(void *p) {
      delete [] ((::MuonObj*)p);
   }
   static void destruct_MuonObj(void *p) {
      typedef ::MuonObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MuonObj(TBuffer &buf, void *obj) {
      ((::MuonObj*)obj)->::MuonObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::MuonObj

//______________________________________________________________________________
void L1Obj::Streamer(TBuffer &R__b)
{
   // Stream an object of class L1Obj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> pt;
      R__b >> eta;
      R__b >> phi;
      R__b >> disc;
      R__b >> bx;
      R__b >> q;
      R__b >> charge;
      void *ptr_type = (void*)&type;
      R__b >> *reinterpret_cast<Int_t*>(ptr_type);
      R__b.CheckByteCount(R__s, R__c, L1Obj::IsA());
   } else {
      R__c = R__b.WriteVersion(L1Obj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << pt;
      R__b << eta;
      R__b << phi;
      R__b << disc;
      R__b << bx;
      R__b << q;
      R__b << charge;
      R__b << (Int_t)type;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_L1Obj(void *p) {
      return  p ? new(p) ::L1Obj : new ::L1Obj;
   }
   static void *newArray_L1Obj(Long_t nElements, void *p) {
      return p ? new(p) ::L1Obj[nElements] : new ::L1Obj[nElements];
   }
   // Wrapper around operator delete
   static void delete_L1Obj(void *p) {
      delete ((::L1Obj*)p);
   }
   static void deleteArray_L1Obj(void *p) {
      delete [] ((::L1Obj*)p);
   }
   static void destruct_L1Obj(void *p) {
      typedef ::L1Obj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_L1Obj(TBuffer &buf, void *obj) {
      ((::L1Obj*)obj)->::L1Obj::Streamer(buf);
   }
} // end of namespace ROOT for class ::L1Obj

//______________________________________________________________________________
void L1ObjColl::Streamer(TBuffer &R__b)
{
   // Stream an object of class L1ObjColl.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      {
         vector<L1Obj> &R__stl =  theL1Obj;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            L1Obj R__t;
            R__t.Streamer(R__b);
            R__stl.push_back(R__t);
         }
      }
      {
         vector<bool> &R__stl =  theL1Matching;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            bool R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      {
         vector<double> &R__stl =  theDeltaR;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            double R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, L1ObjColl::IsA());
   } else {
      R__c = R__b.WriteVersion(L1ObjColl::IsA(), kTRUE);
      TObject::Streamer(R__b);
      {
         vector<L1Obj> &R__stl =  theL1Obj;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<L1Obj>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            ((L1Obj&)(*R__k)).Streamer(R__b);
            }
         }
      }
      {
         vector<bool> &R__stl =  theL1Matching;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<bool>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      {
         vector<double> &R__stl =  theDeltaR;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<double>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_L1ObjColl(void *p) {
      return  p ? new(p) ::L1ObjColl : new ::L1ObjColl;
   }
   static void *newArray_L1ObjColl(Long_t nElements, void *p) {
      return p ? new(p) ::L1ObjColl[nElements] : new ::L1ObjColl[nElements];
   }
   // Wrapper around operator delete
   static void delete_L1ObjColl(void *p) {
      delete ((::L1ObjColl*)p);
   }
   static void deleteArray_L1ObjColl(void *p) {
      delete [] ((::L1ObjColl*)p);
   }
   static void destruct_L1ObjColl(void *p) {
      typedef ::L1ObjColl current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_L1ObjColl(TBuffer &buf, void *obj) {
      ((::L1ObjColl*)obj)->::L1ObjColl::Streamer(buf);
   }
} // end of namespace ROOT for class ::L1ObjColl

//______________________________________________________________________________
void SynchroCountsObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class SynchroCountsObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> dccId;
      R__b >> dccInputChannelNum;
      R__b >> tbLinkInputNum;
      R__b >> lbNumInLink;
      R__b >> bx;
      R__b.CheckByteCount(R__s, R__c, SynchroCountsObj::IsA());
   } else {
      R__c = R__b.WriteVersion(SynchroCountsObj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << dccId;
      R__b << dccInputChannelNum;
      R__b << tbLinkInputNum;
      R__b << lbNumInLink;
      R__b << bx;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SynchroCountsObj(void *p) {
      return  p ? new(p) ::SynchroCountsObj : new ::SynchroCountsObj;
   }
   static void *newArray_SynchroCountsObj(Long_t nElements, void *p) {
      return p ? new(p) ::SynchroCountsObj[nElements] : new ::SynchroCountsObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_SynchroCountsObj(void *p) {
      delete ((::SynchroCountsObj*)p);
   }
   static void deleteArray_SynchroCountsObj(void *p) {
      delete [] ((::SynchroCountsObj*)p);
   }
   static void destruct_SynchroCountsObj(void *p) {
      typedef ::SynchroCountsObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_SynchroCountsObj(TBuffer &buf, void *obj) {
      ((::SynchroCountsObj*)obj)->::SynchroCountsObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::SynchroCountsObj

//______________________________________________________________________________
void SynchroCountsObjVect::Streamer(TBuffer &R__b)
{
   // Stream an object of class SynchroCountsObjVect.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      {
         vector<SynchroCountsObj> &R__stl =  data;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            SynchroCountsObj R__t;
            R__t.Streamer(R__b);
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, SynchroCountsObjVect::IsA());
   } else {
      R__c = R__b.WriteVersion(SynchroCountsObjVect::IsA(), kTRUE);
      TObject::Streamer(R__b);
      {
         vector<SynchroCountsObj> &R__stl =  data;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<SynchroCountsObj>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            ((SynchroCountsObj&)(*R__k)).Streamer(R__b);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SynchroCountsObjVect(void *p) {
      return  p ? new(p) ::SynchroCountsObjVect : new ::SynchroCountsObjVect;
   }
   static void *newArray_SynchroCountsObjVect(Long_t nElements, void *p) {
      return p ? new(p) ::SynchroCountsObjVect[nElements] : new ::SynchroCountsObjVect[nElements];
   }
   // Wrapper around operator delete
   static void delete_SynchroCountsObjVect(void *p) {
      delete ((::SynchroCountsObjVect*)p);
   }
   static void deleteArray_SynchroCountsObjVect(void *p) {
      delete [] ((::SynchroCountsObjVect*)p);
   }
   static void destruct_SynchroCountsObjVect(void *p) {
      typedef ::SynchroCountsObjVect current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_SynchroCountsObjVect(TBuffer &buf, void *obj) {
      ((::SynchroCountsObjVect*)obj)->::SynchroCountsObjVect::Streamer(buf);
   }
} // end of namespace ROOT for class ::SynchroCountsObjVect

//______________________________________________________________________________
void DetCluDigiObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class DetCluDigiObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> det;
      R__b >> clusterSize;
      R__b >> nDigis;
      R__b.CheckByteCount(R__s, R__c, DetCluDigiObj::IsA());
   } else {
      R__c = R__b.WriteVersion(DetCluDigiObj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << det;
      R__b << clusterSize;
      R__b << nDigis;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_DetCluDigiObj(void *p) {
      return  p ? new(p) ::DetCluDigiObj : new ::DetCluDigiObj;
   }
   static void *newArray_DetCluDigiObj(Long_t nElements, void *p) {
      return p ? new(p) ::DetCluDigiObj[nElements] : new ::DetCluDigiObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_DetCluDigiObj(void *p) {
      delete ((::DetCluDigiObj*)p);
   }
   static void deleteArray_DetCluDigiObj(void *p) {
      delete [] ((::DetCluDigiObj*)p);
   }
   static void destruct_DetCluDigiObj(void *p) {
      typedef ::DetCluDigiObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_DetCluDigiObj(TBuffer &buf, void *obj) {
      ((::DetCluDigiObj*)obj)->::DetCluDigiObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::DetCluDigiObj

//______________________________________________________________________________
void DetCluDigiObjVect::Streamer(TBuffer &R__b)
{
   // Stream an object of class DetCluDigiObjVect.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      {
         vector<DetCluDigiObj> &R__stl =  data;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            DetCluDigiObj R__t;
            R__t.Streamer(R__b);
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, DetCluDigiObjVect::IsA());
   } else {
      R__c = R__b.WriteVersion(DetCluDigiObjVect::IsA(), kTRUE);
      TObject::Streamer(R__b);
      {
         vector<DetCluDigiObj> &R__stl =  data;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<DetCluDigiObj>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            ((DetCluDigiObj&)(*R__k)).Streamer(R__b);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_DetCluDigiObjVect(void *p) {
      return  p ? new(p) ::DetCluDigiObjVect : new ::DetCluDigiObjVect;
   }
   static void *newArray_DetCluDigiObjVect(Long_t nElements, void *p) {
      return p ? new(p) ::DetCluDigiObjVect[nElements] : new ::DetCluDigiObjVect[nElements];
   }
   // Wrapper around operator delete
   static void delete_DetCluDigiObjVect(void *p) {
      delete ((::DetCluDigiObjVect*)p);
   }
   static void deleteArray_DetCluDigiObjVect(void *p) {
      delete [] ((::DetCluDigiObjVect*)p);
   }
   static void destruct_DetCluDigiObjVect(void *p) {
      typedef ::DetCluDigiObjVect current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_DetCluDigiObjVect(TBuffer &buf, void *obj) {
      ((::DetCluDigiObjVect*)obj)->::DetCluDigiObjVect::Streamer(buf);
   }
} // end of namespace ROOT for class ::DetCluDigiObjVect

//______________________________________________________________________________
void TriggerMenuResultObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class TriggerMenuResultObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      {
         vector<string> &R__stl =  names;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            string R__t;
            {TString R__str;
             R__str.Streamer(R__b);
             R__t = R__str.Data();}
            R__stl.push_back(R__t);
         }
      }
      {
         vector<unsigned int> &R__stl =  firedAlgos;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            unsigned int R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, TriggerMenuResultObj::IsA());
   } else {
      R__c = R__b.WriteVersion(TriggerMenuResultObj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      {
         vector<string> &R__stl =  names;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<string>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            {TString R__str((*R__k).c_str());
             R__str.Streamer(R__b);};
            }
         }
      }
      {
         vector<unsigned int> &R__stl =  firedAlgos;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<unsigned int>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TriggerMenuResultObj(void *p) {
      return  p ? new(p) ::TriggerMenuResultObj : new ::TriggerMenuResultObj;
   }
   static void *newArray_TriggerMenuResultObj(Long_t nElements, void *p) {
      return p ? new(p) ::TriggerMenuResultObj[nElements] : new ::TriggerMenuResultObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_TriggerMenuResultObj(void *p) {
      delete ((::TriggerMenuResultObj*)p);
   }
   static void deleteArray_TriggerMenuResultObj(void *p) {
      delete [] ((::TriggerMenuResultObj*)p);
   }
   static void destruct_TriggerMenuResultObj(void *p) {
      typedef ::TriggerMenuResultObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TriggerMenuResultObj(TBuffer &buf, void *obj) {
      ((::TriggerMenuResultObj*)obj)->::TriggerMenuResultObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::TriggerMenuResultObj

//______________________________________________________________________________
void HitSpecObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class HitSpecObj.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> theRawId;
      R__b >> pos_x;
      R__b >> pos_y;
      R__b >> pos_z;
      R__b >> mom_x;
      R__b >> mom_y;
      R__b >> mom_z;
      R__b.CheckByteCount(R__s, R__c, HitSpecObj::IsA());
   } else {
      R__c = R__b.WriteVersion(HitSpecObj::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << theRawId;
      R__b << pos_x;
      R__b << pos_y;
      R__b << pos_z;
      R__b << mom_x;
      R__b << mom_y;
      R__b << mom_z;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_HitSpecObj(void *p) {
      return  p ? new(p) ::HitSpecObj : new ::HitSpecObj;
   }
   static void *newArray_HitSpecObj(Long_t nElements, void *p) {
      return p ? new(p) ::HitSpecObj[nElements] : new ::HitSpecObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_HitSpecObj(void *p) {
      delete ((::HitSpecObj*)p);
   }
   static void deleteArray_HitSpecObj(void *p) {
      delete [] ((::HitSpecObj*)p);
   }
   static void destruct_HitSpecObj(void *p) {
      typedef ::HitSpecObj current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_HitSpecObj(TBuffer &buf, void *obj) {
      ((::HitSpecObj*)obj)->::HitSpecObj::Streamer(buf);
   }
} // end of namespace ROOT for class ::HitSpecObj

namespace ROOT {
   static TClass *vectorlEunsignedsPintgR_Dictionary();
   static void vectorlEunsignedsPintgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPintgR(void *p = 0);
   static void *newArray_vectorlEunsignedsPintgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPintgR(void *p);
   static void deleteArray_vectorlEunsignedsPintgR(void *p);
   static void destruct_vectorlEunsignedsPintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned int>*)
   {
      vector<unsigned int> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned int>", -2, "vector", 214,
                  typeid(vector<unsigned int>), DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned int>) );
      instance.SetNew(&new_vectorlEunsignedsPintgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPintgR);
      instance.SetDelete(&delete_vectorlEunsignedsPintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPintgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned int> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<unsigned int>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned int>*)0x0)->GetClass();
      vectorlEunsignedsPintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPintgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<unsigned int> : new vector<unsigned int>;
   }
   static void *newArray_vectorlEunsignedsPintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<unsigned int>[nElements] : new vector<unsigned int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPintgR(void *p) {
      delete ((vector<unsigned int>*)p);
   }
   static void deleteArray_vectorlEunsignedsPintgR(void *p) {
      delete [] ((vector<unsigned int>*)p);
   }
   static void destruct_vectorlEunsignedsPintgR(void *p) {
      typedef vector<unsigned int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned int>

namespace ROOT {
   static TClass *vectorlEstringgR_Dictionary();
   static void vectorlEstringgR_TClassManip(TClass*);
   static void *new_vectorlEstringgR(void *p = 0);
   static void *newArray_vectorlEstringgR(Long_t size, void *p);
   static void delete_vectorlEstringgR(void *p);
   static void deleteArray_vectorlEstringgR(void *p);
   static void destruct_vectorlEstringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<string>*)
   {
      vector<string> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<string>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<string>", -2, "vector", 214,
                  typeid(vector<string>), DefineBehavior(ptr, ptr),
                  &vectorlEstringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<string>) );
      instance.SetNew(&new_vectorlEstringgR);
      instance.SetNewArray(&newArray_vectorlEstringgR);
      instance.SetDelete(&delete_vectorlEstringgR);
      instance.SetDeleteArray(&deleteArray_vectorlEstringgR);
      instance.SetDestructor(&destruct_vectorlEstringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<string> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<string>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEstringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<string>*)0x0)->GetClass();
      vectorlEstringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEstringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEstringgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<string> : new vector<string>;
   }
   static void *newArray_vectorlEstringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<string>[nElements] : new vector<string>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEstringgR(void *p) {
      delete ((vector<string>*)p);
   }
   static void deleteArray_vectorlEstringgR(void *p) {
      delete [] ((vector<string>*)p);
   }
   static void destruct_vectorlEstringgR(void *p) {
      typedef vector<string> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<string>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 214,
                  typeid(vector<double>), DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *vectorlEboolgR_Dictionary();
   static void vectorlEboolgR_TClassManip(TClass*);
   static void *new_vectorlEboolgR(void *p = 0);
   static void *newArray_vectorlEboolgR(Long_t size, void *p);
   static void delete_vectorlEboolgR(void *p);
   static void deleteArray_vectorlEboolgR(void *p);
   static void destruct_vectorlEboolgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<bool>*)
   {
      vector<bool> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<bool>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<bool>", -2, "vector", 526,
                  typeid(vector<bool>), DefineBehavior(ptr, ptr),
                  &vectorlEboolgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<bool>) );
      instance.SetNew(&new_vectorlEboolgR);
      instance.SetNewArray(&newArray_vectorlEboolgR);
      instance.SetDelete(&delete_vectorlEboolgR);
      instance.SetDeleteArray(&deleteArray_vectorlEboolgR);
      instance.SetDestructor(&destruct_vectorlEboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<bool> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<bool>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEboolgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<bool>*)0x0)->GetClass();
      vectorlEboolgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEboolgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEboolgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<bool> : new vector<bool>;
   }
   static void *newArray_vectorlEboolgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<bool>[nElements] : new vector<bool>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEboolgR(void *p) {
      delete ((vector<bool>*)p);
   }
   static void deleteArray_vectorlEboolgR(void *p) {
      delete [] ((vector<bool>*)p);
   }
   static void destruct_vectorlEboolgR(void *p) {
      typedef vector<bool> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<bool>

namespace ROOT {
   static TClass *vectorlESynchroCountsObjgR_Dictionary();
   static void vectorlESynchroCountsObjgR_TClassManip(TClass*);
   static void *new_vectorlESynchroCountsObjgR(void *p = 0);
   static void *newArray_vectorlESynchroCountsObjgR(Long_t size, void *p);
   static void delete_vectorlESynchroCountsObjgR(void *p);
   static void deleteArray_vectorlESynchroCountsObjgR(void *p);
   static void destruct_vectorlESynchroCountsObjgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<SynchroCountsObj>*)
   {
      vector<SynchroCountsObj> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<SynchroCountsObj>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<SynchroCountsObj>", -2, "vector", 214,
                  typeid(vector<SynchroCountsObj>), DefineBehavior(ptr, ptr),
                  &vectorlESynchroCountsObjgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<SynchroCountsObj>) );
      instance.SetNew(&new_vectorlESynchroCountsObjgR);
      instance.SetNewArray(&newArray_vectorlESynchroCountsObjgR);
      instance.SetDelete(&delete_vectorlESynchroCountsObjgR);
      instance.SetDeleteArray(&deleteArray_vectorlESynchroCountsObjgR);
      instance.SetDestructor(&destruct_vectorlESynchroCountsObjgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<SynchroCountsObj> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<SynchroCountsObj>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlESynchroCountsObjgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<SynchroCountsObj>*)0x0)->GetClass();
      vectorlESynchroCountsObjgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlESynchroCountsObjgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlESynchroCountsObjgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<SynchroCountsObj> : new vector<SynchroCountsObj>;
   }
   static void *newArray_vectorlESynchroCountsObjgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<SynchroCountsObj>[nElements] : new vector<SynchroCountsObj>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlESynchroCountsObjgR(void *p) {
      delete ((vector<SynchroCountsObj>*)p);
   }
   static void deleteArray_vectorlESynchroCountsObjgR(void *p) {
      delete [] ((vector<SynchroCountsObj>*)p);
   }
   static void destruct_vectorlESynchroCountsObjgR(void *p) {
      typedef vector<SynchroCountsObj> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<SynchroCountsObj>

namespace ROOT {
   static TClass *vectorlEL1ObjgR_Dictionary();
   static void vectorlEL1ObjgR_TClassManip(TClass*);
   static void *new_vectorlEL1ObjgR(void *p = 0);
   static void *newArray_vectorlEL1ObjgR(Long_t size, void *p);
   static void delete_vectorlEL1ObjgR(void *p);
   static void deleteArray_vectorlEL1ObjgR(void *p);
   static void destruct_vectorlEL1ObjgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<L1Obj>*)
   {
      vector<L1Obj> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<L1Obj>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<L1Obj>", -2, "vector", 214,
                  typeid(vector<L1Obj>), DefineBehavior(ptr, ptr),
                  &vectorlEL1ObjgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<L1Obj>) );
      instance.SetNew(&new_vectorlEL1ObjgR);
      instance.SetNewArray(&newArray_vectorlEL1ObjgR);
      instance.SetDelete(&delete_vectorlEL1ObjgR);
      instance.SetDeleteArray(&deleteArray_vectorlEL1ObjgR);
      instance.SetDestructor(&destruct_vectorlEL1ObjgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<L1Obj> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<L1Obj>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEL1ObjgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<L1Obj>*)0x0)->GetClass();
      vectorlEL1ObjgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEL1ObjgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEL1ObjgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<L1Obj> : new vector<L1Obj>;
   }
   static void *newArray_vectorlEL1ObjgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<L1Obj>[nElements] : new vector<L1Obj>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEL1ObjgR(void *p) {
      delete ((vector<L1Obj>*)p);
   }
   static void deleteArray_vectorlEL1ObjgR(void *p) {
      delete [] ((vector<L1Obj>*)p);
   }
   static void destruct_vectorlEL1ObjgR(void *p) {
      typedef vector<L1Obj> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<L1Obj>

namespace ROOT {
   static TClass *vectorlEDetCluDigiObjgR_Dictionary();
   static void vectorlEDetCluDigiObjgR_TClassManip(TClass*);
   static void *new_vectorlEDetCluDigiObjgR(void *p = 0);
   static void *newArray_vectorlEDetCluDigiObjgR(Long_t size, void *p);
   static void delete_vectorlEDetCluDigiObjgR(void *p);
   static void deleteArray_vectorlEDetCluDigiObjgR(void *p);
   static void destruct_vectorlEDetCluDigiObjgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<DetCluDigiObj>*)
   {
      vector<DetCluDigiObj> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<DetCluDigiObj>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<DetCluDigiObj>", -2, "vector", 214,
                  typeid(vector<DetCluDigiObj>), DefineBehavior(ptr, ptr),
                  &vectorlEDetCluDigiObjgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<DetCluDigiObj>) );
      instance.SetNew(&new_vectorlEDetCluDigiObjgR);
      instance.SetNewArray(&newArray_vectorlEDetCluDigiObjgR);
      instance.SetDelete(&delete_vectorlEDetCluDigiObjgR);
      instance.SetDeleteArray(&deleteArray_vectorlEDetCluDigiObjgR);
      instance.SetDestructor(&destruct_vectorlEDetCluDigiObjgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<DetCluDigiObj> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<DetCluDigiObj>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEDetCluDigiObjgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<DetCluDigiObj>*)0x0)->GetClass();
      vectorlEDetCluDigiObjgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEDetCluDigiObjgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEDetCluDigiObjgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<DetCluDigiObj> : new vector<DetCluDigiObj>;
   }
   static void *newArray_vectorlEDetCluDigiObjgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<DetCluDigiObj>[nElements] : new vector<DetCluDigiObj>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEDetCluDigiObjgR(void *p) {
      delete ((vector<DetCluDigiObj>*)p);
   }
   static void deleteArray_vectorlEDetCluDigiObjgR(void *p) {
      delete [] ((vector<DetCluDigiObj>*)p);
   }
   static void destruct_vectorlEDetCluDigiObjgR(void *p) {
      typedef vector<DetCluDigiObj> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<DetCluDigiObj>

namespace {
  void TriggerDictionaryInitialization_rootMAnaDict_Impl() {
    static const char* headers[] = {
"UserCode/L1RpcTriggerAnalysis/interface/EventObj.h",
"UserCode/L1RpcTriggerAnalysis/interface/EventData.h",
"UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h",
"UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h",
"UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h",
"UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h",
"UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h",
"UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h",
"UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h",
"UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObjVect.h",
"UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h",
"UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h",
0
    };
    static const char* includePaths[] = {
"../../../",
"/cvmfs/cms.cern.ch/slc6_amd64_gcc491/cms/cmssw/CMSSW_7_5_0_pre1/src/",
"/cvmfs/cms.cern.ch/slc6_amd64_gcc491/external/clhep/2.1.4.1-cms/include",
"/cvmfs/cms.cern.ch/slc6_amd64_gcc491/lcg/root/6.02.00-lnjiaj2/include",
"/afs/cern.ch/work/k/konec/CMSSW_7_5_0_pre1.OMTF/src/UserCode/L1RpcTriggerAnalysis/src/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/EventObj.h")))  EventObj;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/EventData.h")))  EventData;
class __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h")))  TrackObj;
class __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h")))  MuonObj;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/EventData.h")))  L1Obj;
class __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h")))  L1ObjColl;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h")))  SynchroCountsObj;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h")))  SynchroCountsObjVect;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h")))  DetCluDigiObj;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObjVect.h")))  DetCluDigiObjVect;
struct __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h")))  TriggerMenuResultObj;
class __attribute__((annotate("$clingAutoload$UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h")))  HitSpecObj;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventData.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObjVect.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"DetCluDigiObj", payloadCode, "@",
"DetCluDigiObjVect", payloadCode, "@",
"EventData", payloadCode, "@",
"EventObj", payloadCode, "@",
"HitSpecObj", payloadCode, "@",
"L1Obj", payloadCode, "@",
"L1ObjColl", payloadCode, "@",
"MuonObj", payloadCode, "@",
"SynchroCountsObj", payloadCode, "@",
"SynchroCountsObjVect", payloadCode, "@",
"TrackObj", payloadCode, "@",
"TriggerMenuResultObj", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("rootMAnaDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_rootMAnaDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_rootMAnaDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_rootMAnaDict() {
  TriggerDictionaryInitialization_rootMAnaDict_Impl();
}
