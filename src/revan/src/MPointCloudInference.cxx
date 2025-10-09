/*
 * MPointCloudInference.cxx
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
#include "MPointCloudInference.h"

// MEGAlib libs:
#include "MStreams.h"

// Onnx libs:
//#include <onnxruntime_cxx_api.h>

// C++ std libs:
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#ifdef ___CLING___
  ClassImp(MPointCloudInference);
#endif

using namespace std;


MPointCloudInference::MPointCloudInference(const string& model_path) : memory_info(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {
  
  //mout << "Creating ONNX environment..." << endl;
  env = make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "MPointCloudInference");
  
  //mout << "Setting up session options..." << endl;
  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
  
  mout << "Loading model: " << model_path << endl;
  try {
      session = make_unique<Ort::Session>(*env, model_path.c_str(), session_options);
      //mout << "Model loaded successfully!" << endl;
  } catch (const Ort::Exception& e) {
      cerr << "Failed to load model: " << e.what() << endl;
      throw;
  }
  
  //mout << "Getting model metadata..." << endl;
  loadModelInfo();
}

pair<vector<float>, vector<float>> MPointCloudInference::inference(
  const vector<float>& point_cloud_data, 
  const vector<float>& mask_data,
  int batch_size, 
  int feature_dim, 
  int num_points) {
  
  //mout << "Starting inference..." << endl;
  //mout << "Input sizes - PC: " << point_cloud_data.size() 
  //          << ", Mask: " << mask_data.size() << endl;
  //mout << "Expected sizes - PC: " << (batch_size * feature_dim * num_points)
  //          << ", Mask: " << (batch_size * num_points) << endl;
  
  // Validate input sizes
  if (point_cloud_data.size() != static_cast<size_t>(batch_size * feature_dim * num_points)) {
      throw invalid_argument("Point cloud data size mismatch");
  }
  if (mask_data.size() != static_cast<size_t>(batch_size * num_points)) {
      throw invalid_argument("Mask data size mismatch");
  }
  
  // Create input tensors
  //mout << "Creating input tensors..." << endl;
  vector<int64_t> point_cloud_shape = {batch_size, feature_dim, num_points};
  vector<int64_t> mask_shape = {batch_size, num_points};
  
  //mout << "PC shape: [" << point_cloud_shape[0] << ", " 
  //          << point_cloud_shape[1] << ", " << point_cloud_shape[2] << "]" << endl;
  //mout << "Mask shape: [" << mask_shape[0] << ", " << mask_shape[1] << "]" << endl;
  
  // Use const_cast carefully - ensure data lifetime
  auto point_cloud_tensor = Ort::Value::CreateTensor<float>(
      memory_info, 
      const_cast<float*>(point_cloud_data.data()), 
      point_cloud_data.size(),
      point_cloud_shape.data(), 
      point_cloud_shape.size()
  );
  auto mask_tensor = Ort::Value::CreateTensor<float>(
      memory_info, 
      const_cast<float*>(mask_data.data()), 
      mask_data.size(),
      mask_shape.data(), 
      mask_shape.size()
  );
  
  //mout << "Tensors created successfully" << endl;
  
  // Prepare inputs
  vector<Ort::Value> input_tensors;
  input_tensors.push_back(std::move(point_cloud_tensor));
  input_tensors.push_back(std::move(mask_tensor));
  
  // Convert names to char*
  vector<const char*> input_names_char;
  vector<const char*> output_names_char;
  
  for (const auto& name : input_names) {
      input_names_char.push_back(name.c_str());
  }
  for (const auto& name : output_names) {
      output_names_char.push_back(name.c_str());
  }
  
  //mout << "Running inference..." << endl;
  
  // Run inference with error handling
  vector<Ort::Value> output_tensors;
  try {
      output_tensors = session->Run(
          Ort::RunOptions{nullptr}, 
          input_names_char.data(), 
          input_tensors.data(), 
          input_tensors.size(),
          output_names_char.data(), 
          output_names.size()
      );
      //mout << "Inference completed successfully!" << endl;
  } catch (const Ort::Exception& e) {
      cerr << "Inference failed: " << e.what() << endl;
      throw;
  }
  
  //mout << "Extracting outputs..." << endl;
  
  // Get output data safely
  float* logits_data = output_tensors[0].GetTensorMutableData<float>();
  float* trans_feat_data = output_tensors[1].GetTensorMutableData<float>();
  
  // Get output shapes for verification
  auto logits_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
  auto trans_feat_shape = output_tensors[1].GetTensorTypeAndShapeInfo().GetShape();
  
  //mout << "Output shapes:" << endl;
  //mout << "Logits: ";
  //for (auto dim : logits_shape) mout << dim << " ";
  //mout << endl;
  //mout << "Trans_feat: ";
  //for (auto dim : trans_feat_shape) mout << dim << " ";
  //mout << endl;
  
  // Calculate actual sizes from shapes
  size_t logits_size = 1;
  for (auto dim : logits_shape) logits_size *= dim;
  
  size_t trans_feat_size = 1;
  for (auto dim : trans_feat_shape) trans_feat_size *= dim;
  
  //mout << "Calculated sizes - Logits: " << logits_size 
  //          << ", Trans_feat: " << trans_feat_size << endl;
  
  // Copy results
  vector<float> logits(logits_data, logits_data + logits_size);
  vector<float> trans_feat(trans_feat_data, trans_feat_data + trans_feat_size);
  
  //mout << "Results extracted successfully!" << endl;
  
  return make_pair(logits, trans_feat);
}

void MPointCloudInference::loadModelInfo() {
  Ort::AllocatorWithDefaultOptions allocator;
  
  size_t num_input_nodes = session->GetInputCount();
  //mout << "Number of inputs: " << num_input_nodes << endl;
  
  for (size_t i = 0; i < num_input_nodes; i++) {
      auto input_name = session->GetInputNameAllocated(i, allocator);
      input_names.emplace_back(input_name.get());
      //mout << "Input " << i << ": " << input_names.back() << endl;
  }
  
  size_t num_output_nodes = session->GetOutputCount();
  //mout << "Number of outputs: " << num_output_nodes << endl;
  
  for (size_t i = 0; i < num_output_nodes; i++) {
      auto output_name = session->GetOutputNameAllocated(i, allocator);
      output_names.emplace_back(output_name.get());
      //mout << "Output " << i << ": " << output_names.back() << endl;
  }
}

float MPointCloudInference::sigmoid(float x) {
  return 1. / (1. + exp(-x) );
}



