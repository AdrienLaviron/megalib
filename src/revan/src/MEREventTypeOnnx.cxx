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
}

bool MEREventTypeOnnx::PostAnalysis()
{
  return true;
}


////////////////////////////////////////////////////////////////////////////////

bool MEREventTypeOnnx::Analyze(MRawEventIncarnations* List)
{
  MERConstruction::Analyze(List);
  MRERawEvent* RE = nullptr;
  MRESEList* hits = new MRESEList();
  for (int e = 0; e < m_List->GetNRawEvents(); e++) {
    RE = m_List->GetRawEventAt(e);
    // Instantiate data structures
    int batch_size = 1;
    int feature_dim = 4;
    int num_points = 0;//Different for each event
    for (int i = 0; i < RE->GetNRESEs(); i++) {
      if (RE->GetRESEAt(i)->GetType() == MRESE::c_Hit || 
          RE->GetRESEAt(i)->GetType() == MRESE::c_Cluster) {
        hits->AddRESE(RE->GetRESEAt(i));
        num_points++;
      }
    }
    vector<float> point_cloud_data(batch_size * feature_dim * num_points);
    vector<float> mask_data(batch_size * num_points);
    // Mask is just 1s everywhere
    for (size_t i = 0; i < mask_data.size(); ++i) { mask_data[i] = 1.0f; }
    // Get data into point_could_data - assuming batch size 1
    MRESE* rese = nullptr;
    for (int i = 0; i < num_points; ++i) {
      rese = hits->GetRESEAt(i);
      /*point_cloud_data[ batch_size*feature_dim*i ]   = rese->GetPositionX();
      point_cloud_data[ batch_size*feature_dim*i +1] = rese->GetPositionY();
      point_cloud_data[ batch_size*feature_dim*i +2] = rese->GetPositionZ();
      point_cloud_data[ batch_size*feature_dim*i +3] = rese->GetEnergy();*/
      point_cloud_data[ i ]   = rese->GetPositionX();
      point_cloud_data[ i +num_points] = rese->GetPositionY();
      point_cloud_data[ i +2*num_points] = rese->GetPositionZ();
      point_cloud_data[ i +3*num_points] = rese->GetEnergy();
    }
    cout << "EventID= " << RE->GetEventID();
    for (int i = 0; i < num_points*batch_size*feature_dim; i++) {
      cout << " " << point_cloud_data[i];
      if (i%5 == 4) cout << endl;
    }
    cout << endl;

    pair<vector<float>, vector<float>> 
              results = m_model->inference(point_cloud_data, mask_data,
                                           batch_size, feature_dim, num_points);
    vector<float>& logits = results.first;      // N x 1 (batch_size x 1)

    if ( logits[0] > 0 ) {
      RE->SetEventType( c_PairEvent );
      RE->SetEventTypeProbability( MPointCloudInference::sigmoid(logits[0]) );
      mdebug << "ID " << RE->GetEventID() << " PA " << MPointCloudInference::sigmoid(logits[0]) << endl;
    } else {
      RE->SetEventType( c_ComptonEvent );
      RE->SetEventTypeProbability( 1 - MPointCloudInference::sigmoid(logits[0]) );
      mdebug << "ID " << RE->GetEventID() << " CO " << 1- MPointCloudInference::sigmoid(logits[0]) << endl;
    }
  }

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

