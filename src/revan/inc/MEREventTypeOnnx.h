/*
 * MEREventTypeOnnx.h
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MEREventTypeOnnx__
#define __MEREventTypeOnnx__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MEREventType.h"
#include "MFileEventsType.h"
#include "MRawEventIncarnations.h"
#include "MPointCloudInference.h"

// Onnx libs:
//#include <onnxruntime_cxx_api.h>

// Forward declarations:

////////////////////////////////////////////////////////////////////////////////


class MEREventTypeOnnx : public MEREventType
{
  // public interface:
 public:
  MEREventTypeOnnx();
  virtual ~MEREventTypeOnnx();

  //! Global parameters used by all electron tracking algorithms
  virtual void SetParameters(MString EventTypeFileName);
  bool Analyze(MRawEventIncarnations* List);

  bool PostAnalysis();
  MString ToString(bool CoreOnly = false) const;


  // protected members:
 protected:
  MString m_EventTypeFileName;
  //MFileEventsType* m_FileEventsType;

  MPointCloudInference* m_model;
  //torch::jit::script::Module m_module;


#ifdef ___CLING___
 public:
  ClassDef(MEREventTypeOnnx, 0) // no description
#endif

};

#endif




