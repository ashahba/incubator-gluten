/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <glog/logging.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <thread>
#include <utility>

#include "substrait/SubstraitToVeloxPlan.h"

#include "compute/ProtobufUtils.h"
#include "memory/VeloxColumnarBatch.h"
#include "memory/VeloxMemoryManager.h"
#include "utils/Exception.h"
#include "velox/common/memory/Memory.h"

DECLARE_int64(batch_size);
DECLARE_int32(cpu);
DECLARE_int32(threads);
DECLARE_int32(iterations);

namespace gluten {

std::unordered_map<std::string, std::string> defaultConf();

/// Initialize the Velox backend.
void initVeloxBackend(const std::unordered_map<std::string, std::string>& conf = {});

// Get the location of a file generated by Java unittest.
inline std::string getGeneratedFilePath(const std::string& fileName) {
  std::string currentPath = std::filesystem::current_path().c_str();
  auto generatedFilePath = currentPath + "/../../../../backends-velox/generated-native-benchmark/";
  std::filesystem::directory_entry filePath{generatedFilePath + fileName};
  if (filePath.exists()) {
    if (filePath.is_regular_file() && filePath.path().extension().native() == ".json") {
      // If fileName points to a regular file, it should be substrait json plan.
      return filePath.path().c_str();
    } else if (filePath.is_directory()) {
      // If fileName points to a directory, get the generated parquet data.
      auto dirItr = std::filesystem::directory_iterator(std::filesystem::path(filePath));
      for (auto& itr : dirItr) {
        if (itr.is_regular_file() && itr.path().extension().native() == ".parquet") {
          return itr.path().c_str();
        }
      }
    }
  }
  throw gluten::GlutenException("Could not get generated file from given path: " + fileName);
}

/// Read binary data from a json file.
std::string getPlanFromFile(const std::string& type, const std::string& filePath);

/// Get the file paths, starts, lengths from a directory.
/// Use fileFormat to specify the format to read, eg., orc, parquet.
/// Return a split info.
std::shared_ptr<gluten::SplitInfo> getSplitInfos(const std::string& datasetPath, const std::string& fileFormat);

std::shared_ptr<gluten::SplitInfo> getSplitInfosFromFile(const std::string& fileName, const std::string& fileFormat);

bool checkPathExists(const std::string& filepath);

void abortIfFileNotExists(const std::string& filepath);

inline std::shared_ptr<gluten::ColumnarBatch> convertBatch(std::shared_ptr<gluten::ColumnarBatch> cb) {
  return gluten::VeloxColumnarBatch::from(gluten::defaultLeafVeloxMemoryPool().get(), cb);
}

/// Return whether the data ends with suffix.
bool endsWith(const std::string& data, const std::string& suffix);

void setCpu(uint32_t cpuIndex);

} // namespace gluten
