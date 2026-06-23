# Copyright 2020-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.31.8)

project(tritonLSTbackend LANGUAGES C CXX CUDA)

# Use C++17 standard as Triton's minimum required.
set(TRITON_MIN_CXX_STANDARD 17 CACHE STRING "The minimum C++ standard which features are requested to build this target.")

# Use C++20 standard as required by CMSSW modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES "75;80;86;89;90")
endif()

#
# Options
#
# Must include options required for this project as well as any
# projects included in this one by FetchContent.
#
# GPU support is disabled by default because LST backend doesn't
# support GPUs.
#
option(TRITON_ENABLE_GPU "Enable GPU support in backend" ON)
option(TRITON_ENABLE_STATS "Include statistics collections in backend" ON)
option(TRITON_ENABLE_METRICS "Include metrics support in backend" ON)

set(TRITON_REPO_ORGANIZATION "https://github.com/triton-inference-server" CACHE STRING "Git repository to pull from")
set(TRITON_COMMON_REPO_TAG "main" CACHE STRING "Tag for triton-inference-server/common repo")
set(TRITON_CORE_REPO_TAG "main" CACHE STRING "Tag for triton-inference-server/core repo")
set(TRITON_BACKEND_REPO_TAG "main" CACHE STRING "Tag for triton-inference-server/backend repo")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#
# Dependencies
#
# FetchContent's composability isn't very good. We must include the
# transitive closure of all repos so that we can override the tag.
#
include(FetchContent)

FetchContent_Declare(
  repo-common
  GIT_REPOSITORY ${TRITON_REPO_ORGANIZATION}/common.git
  GIT_TAG ${TRITON_COMMON_REPO_TAG}
  GIT_SHALLOW ON
)
FetchContent_Declare(
  repo-core
  GIT_REPOSITORY ${TRITON_REPO_ORGANIZATION}/core.git
  GIT_TAG ${TRITON_CORE_REPO_TAG}
  GIT_SHALLOW ON
)
FetchContent_Declare(
  repo-backend
  GIT_REPOSITORY ${TRITON_REPO_ORGANIZATION}/backend.git
  GIT_TAG ${TRITON_BACKEND_REPO_TAG}
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(repo-common repo-core repo-backend)

#
# Shared library implementing the Triton Backend API
#
configure_file(src/libtriton_LST.ldscript libtriton_LST.ldscript COPYONLY)

add_library(
  triton-LST-backend SHARED
  src/LST.cc
  src/LSTRunnerCPU.cc
  src/LSTRunnerGPU.cc
)

# LSTRunnerGPU.cc includes alpaka CUDA headers (device builtins, <<<>>> syntax)
# and must be compiled with nvcc.  LST.cc and LSTRunnerCPU.cc use plain g++.
set_source_files_properties(src/LSTRunnerGPU.cc PROPERTIES LANGUAGE CUDA)

# Per-file alpaka accelerator backend selection.
# LSTRunnerGPU.cc: CUDA async backend (alpaka_cuda_async::lst::LST)
set_source_files_properties(src/LSTRunnerGPU.cc PROPERTIES
    COMPILE_DEFINITIONS "ALPAKA_ACC_GPU_CUDA_ENABLED"
)
# LSTRunnerCPU.cc: serial sync backend (alpaka_serial_sync::lst::LST)
set_source_files_properties(src/LSTRunnerCPU.cc PROPERTIES
    COMPILE_DEFINITIONS "ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED"
)

add_library(
  TritonLSTBackend::triton-LST-backend ALIAS triton-LST-backend
)

target_include_directories(
  triton-LST-backend
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    /cvmfs/cms.cern.ch/el8_amd64_gcc13/external/alpaka/2.1.1-3caaac8d71f39d400ab2511b2403675a/include
    /cvmfs/cms.cern.ch/el8_amd64_gcc13/external/boost/1.80.0-6429d45cb860600db5fc5d037104a1f8/include
    /cvmfs/cms.cern.ch/el8_amd64_gcc13/lcg/root/6.36.11-c9684ba7622be6ba003c6d1250a79c91/include
    /cvmfs/cms.cern.ch/el8_amd64_gcc13/cms/cmssw/CMSSW_16_1_0_pre4/src # YY: I am not sure all of them are necessary.. 
    /cvmfs/cms.cern.ch/el8_amd64_gcc13/external/clhep/2.4.7.2-a1886bcfe0fba01667305d6d8ed03de5/include
    /cvmfs/cms.cern.ch/el8_amd64_gcc13/external/fmt/10.2.1-31d67b0504b4ba2262f03d3c5cad83c1/include
    /usr/local/cuda/include
)

target_compile_features(triton-LST-backend PRIVATE cxx_std_${TRITON_MIN_CXX_STANDARD})
target_compile_options(
  triton-LST-backend PRIVATE
  # CXX-only flags: guard with COMPILE_LANGUAGE:CXX so they don't reach nvcc
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>>:
    -Wall -Wextra -Wno-unused-parameter -Wno-type-limits -Werror -Wno-error=attributes -Wno-attributes -Wno-error=unused-local-typedefs -Wno-unused-local-typedefs>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/Wall /D_WIN32_WINNT=0x0A00 /EHsc>
  # CUDA-only flags for nvcc
  $<$<COMPILE_LANGUAGE:CUDA>:
    --expt-relaxed-constexpr
    --extended-lambda
    -Xcudafe --diag_suppress=esa_on_defaulted_function_ignored
  >
)

if(${TRITON_ENABLE_METRICS})
  target_compile_definitions(
    triton-LST-backend
    PRIVATE 
      TRITON_ENABLE_METRICS=1
  )
endif() # TRITON_ENABLE_METRICS

target_compile_definitions(
  triton-LST-backend
  PRIVATE
    LST_STANDALONE
)

target_link_libraries(
  triton-LST-backend
  PRIVATE
    triton-core-serverapi   # from repo-core
    triton-core-backendapi  # from repo-core
    triton-core-serverstub  # from repo-core
    triton-backend-utils    # from repo-backend
)

if(WIN32)
  set_target_properties(
    triton-LST-backend PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME triton_LST
  )
else()
  set_target_properties(
    triton-LST-backend PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME triton_LST
    LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libtriton_LST.ldscript
    LINK_FLAGS "-Wl,--version-script libtriton_LST.ldscript"
  )
endif()

#
# Install
#
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/TritonLSTBackend)

install(
  TARGETS
    triton-LST-backend
  EXPORT
    triton-LST-backend-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/backends/LST
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/backends/LST
)

install(
  EXPORT
    triton-LST-backend-targets
  FILE
    TritonLSTBackendTargets.cmake
  NAMESPACE
    TritonLSTBackend::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/TritonLSTBackendConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/TritonLSTBackendConfig.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/TritonLSTBackendConfig.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

#
# Export from build tree
#
export(
  EXPORT triton-LST-backend-targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/TritonLSTBackendTargets.cmake
  NAMESPACE TritonLSTBackend::
)

export(PACKAGE TritonLSTBackend)
