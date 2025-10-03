/*
 * MPointCloudInference.h
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "MGlobal.h"


#ifndef __MPointCloudInference__
#define __MPointCloudInference__

using namespace std;

////////////////////////////////////////////////////////////////////////////////

class MPointCloudInference {
 public:
  MPointCloudInference(const string& model_path);
  virtual ~MPointCloudInference() {}
  pair<vector<float>, vector<float>> inference(
    const vector<float>& point_cloud_data, 
    const vector<float>& mask_data,
    int batch_size, 
    int feature_dim, 
    int num_points);
 private:
  void loadModelInfo();

 private:
  unique_ptr<Ort::Env> env;
  unique_ptr<Ort::Session> session;
  Ort::MemoryInfo memory_info;
  vector<string> input_names;
  vector<string> output_names;

#ifdef ___CLING___
 public:
  ClassDef(MPointCloudInference, 0); // no description
#endif

};

#endif



