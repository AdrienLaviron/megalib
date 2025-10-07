/*
 * MEREventTypeOnnx.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


// Include the header:
#include "MEREventTypeOnnx.h"


// MEGAlib libs:
#include "MAssert.h"
#include "MFileEventsType.h"
#include "MPointCloudInference.h"

// Onnx libs:
//#include <onnxruntime_cxx_api.h>

// C++ std libs:
#include <iostream>
#include <memory>
#include <vector>
#include <string>


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MEREventTypeOnnx)
#endif


////////////////////////////////////////////////////////////////////////////////

MEREventTypeOnnx::MEREventTypeOnnx() : MEREventType()
{
  m_model = nullptr;
  m_EventTypeFileName = "";
}

MEREventTypeOnnx::~MEREventTypeOnnx()
{
  if(m_model) delete m_model;
}


////////////////////////////////////////////////////////////////////////////////

void MEREventTypeOnnx::SetParameters(MString EventTypeFileName)
{
  m_EventTypeFileName = EventTypeFileName;
  m_model = new MPointCloudInference(m_EventTypeFileName.ToString());
  /*try {
    m_module = torch::jit::load(m_EventTypeFileName);
    m_module.eval();
    mout << "Module " << m_EventTypeFileName << " loaded successfully." << endl;
  } catch (const c10::Error& e) {
    merr << "Error loading model: " << e.msg() << endl;
  }*/
}

bool MEREventTypeOnnx::PostAnalysis()
{
  //return m_FileEventsType->Close();
  return true;
}


////////////////////////////////////////////////////////////////////////////////

bool MEREventTypeOnnx::Analyze(MRawEventIncarnations* List)
{
  MERConstruction::Analyze(List);
/*
  if (! m_FileEventsType->IsOpen() ) {//First event
    m_FileEventsType->Open(m_EventTypeFileName); //Read-mode
  }

  // Read file
  streampos filePos = m_FileEventsType->GetFilePosition(); // Position at start of REI
  MRERawEvent* RE = nullptr;  
  for (int e = 0; e < m_List->GetNRawEvents(); e++) {
    m_FileEventsType->Seek(filePos);// Rewind to position at start of REI
    RE = m_List->GetRawEventAt(e);
    while(m_FileEventsType->GetNextEvent() && m_FileEventsType->GetEventId() != RE->GetEventId()) {}
    mout << "evtid=" << m_FileEventsType->GetEventId() << endl;
    if (m_FileEventsType->GetEventId() == RE->GetEventId()) { // if found matching ID
      RE->SetEventType( m_FileEventsType->GetEventType() );
      RE->SetEventTypeProbability( m_FileEventsType->GetEventTypeProbability() );
    } else {
      merr << "MEREventTypeOnnx: No event type found for event ID " << RE->GetEventId() << endl;
      RE->SetEventType( c_UnknownEvent );
      RE->SetEventTypeProbability( 0. );
    }
  }*/
  return true;
}


////////////////////////////////////////////////////////////////////////////////

MString MEREventTypeOnnx::ToString(bool CoreOnly) const
{
  // Dump an options string gor the tra file:
  ostringstream out;

  if (CoreOnly == false) {
    out<<"# Event type - options:"<<endl;
    out<<"# "<<endl;
  }
  out<<"# Event Type file name:         "<<m_EventTypeFileName<<endl;
  if (CoreOnly == false) {
    out<<"# "<<endl;
  }

  return out.str().c_str();
}

