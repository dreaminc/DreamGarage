// Copyright 2019 The Shaderc Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "common_shaders_for_test.h"
#include "shaderc/spvc.hpp"

using shaderc_spvc::CompilationResult;
using shaderc_spvc::CompileOptions;
using shaderc_spvc::Compiler;

namespace {

TEST(Compile, Glsl) {
  Compiler compiler;
  CompileOptions options;
  options.SetSourceEnvironment(shaderc_target_env_webgpu,
                               shaderc_env_version_webgpu);
  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_1);

  CompilationResult result = compiler.CompileSpvToGlsl(
      kWebGPUShaderBinary, sizeof(kWebGPUShaderBinary) / sizeof(uint32_t),
      options);
  EXPECT_EQ(shaderc_compilation_status_success, result.GetCompilationStatus());
  EXPECT_NE(0, result.GetStringOutput().size());
  EXPECT_EQ(0, result.GetBinaryOutput().size());
}

TEST(Compile, Hlsl) {
  Compiler compiler;
  CompileOptions options;
  options.SetSourceEnvironment(shaderc_target_env_webgpu,
                               shaderc_env_version_webgpu);
  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_1);

  CompilationResult result = compiler.CompileSpvToHlsl(
      kWebGPUShaderBinary, sizeof(kWebGPUShaderBinary) / sizeof(uint32_t),
      options);
  EXPECT_EQ(shaderc_compilation_status_success, result.GetCompilationStatus());
  EXPECT_NE(0, result.GetStringOutput().size());
  EXPECT_EQ(0, result.GetBinaryOutput().size());
}

TEST(Compile, Msl) {
  Compiler compiler;
  CompileOptions options;
  options.SetSourceEnvironment(shaderc_target_env_webgpu,
                               shaderc_env_version_webgpu);
  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_1);

  CompilationResult result = compiler.CompileSpvToMsl(
      kWebGPUShaderBinary, sizeof(kWebGPUShaderBinary) / sizeof(uint32_t),
      options);
  EXPECT_EQ(shaderc_compilation_status_success, result.GetCompilationStatus());
  EXPECT_NE(0, result.GetStringOutput().size());
  EXPECT_EQ(0, result.GetBinaryOutput().size());
}

TEST(Compile, Vulkan) {
  Compiler compiler;
  CompileOptions options;
  options.SetSourceEnvironment(shaderc_target_env_webgpu,
                               shaderc_env_version_webgpu);
  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_1);

  CompilationResult result = compiler.CompileSpvToVulkan(
      kWebGPUShaderBinary, sizeof(kWebGPUShaderBinary) / sizeof(uint32_t),
      options);
  EXPECT_EQ(shaderc_compilation_status_success, result.GetCompilationStatus());
  EXPECT_EQ(0, result.GetStringOutput().size());
  EXPECT_NE(0, result.GetBinaryOutput().size());
}

}  // anonymous namespace
