<!--
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
-->

[![License](https://img.shields.io/badge/License-BSD3-lightgrey.svg)](https://opensource.org/licenses/BSD-3-Clause)

# Triton Inference Server LST Alpaka Backend


## Setup for Backend Compilation

To setup backend compilation, it needs `lst_cuda.so` and `lst_cpu.so` from standalone compilation, and code modified for backend compilation.

1. Clone the backend repository:
```bash
git clone git@github.com:y19y19/LST_alpaka_backend.git
```

2. Clone the standalone repository and compile `lst_cpu.so` and `lst_cuda.so` as instructed in [https://github.com/y19y19/LST_alpaka_standalone_SONIC/tree/CMSSW_16_1_0_pre4](https://github.com/y19y19/LST_alpaka_standalone_SONIC/tree/CMSSW_16_1_0_pre4#build-lst_cudaso-and-lst_cpuso):


3. Clone the standalone repository for backend compilation:
```bash
git clone -b CMSSW_16_1_0_pre4_backend_compilation git@github.com:y19y19/LST_alpaka_standalone_SONIC.git lst_standalone_for_backend_compilation
```

4. Copy code over into the backend:
```bash
cp -r lst_standalone_for_backend_compilation/RecoTracker/LSTCore LST_alpaka_backend/
cp lst_standalone/RecoTracker/LSTCore/standalone/LST/lst_cpu.so LST_alpaka_backend/LSTCore/standalone/LST/
cp lst_standalone/RecoTracker/LSTCore/standalone/LST/lst_cuda.so LST_alpaka_backend/LSTCore/standalone/LST/
```

## Build

Use cmake to build and install in a local directory. Build it with container docker://y19y19/tritonserver_builder_gcc13:v4

1. If you don't have a container, pull it once. 
```bash
singularity pull --disable-cache docker://y19y19/tritonserver_builder_gcc13:v4
```

2. Build it in container
```bash
singularity run --nv -e --no-home -B <path_to>/LST_alpaka_backend/:/workspace/backend/ -B /cvmfs/:/cvmfs/ <path_to>/tritonserver_builder_gcc13_v4.sif
# In the container
cd /workspace/backend/
rm -rf build # if build folder exists
mkdir build
cd build
export CUDA_ARCH_LIST="7.5" # For Tesla T4 GPU
cmake -DTRITON_ENABLE_GPU=ON -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/install -DTRITON_BACKEND_REPO_TAG=r24.11 -DTRITON_CORE_REPO_TAG=r24.11 -DTRITON_COMMON_REPO_TAG=r24.11 ..
make install
```

The following required Triton repositories will be pulled and used in
the build. By default the "main" branch/tag will be used for each repo
but the listed CMake argument can be used to override.

* triton-inference-server/backend: -DTRITON_BACKEND_REPO_TAG=[tag]
* triton-inference-server/core: -DTRITON_CORE_REPO_TAG=[tag]
* triton-inference-server/common: -DTRITON_COMMON_REPO_TAG=[tag]

If you are building on a release branch (or on a development branch
that is based off of a release branch), then you must set these cmake
arguments to point to that release branch as well. 

## Custom Metric Example

When `TRITON_ENABLE_METRICS` is enabled, this backend implements an example
of registering a custom metric to Triton's existing metrics endpoint via the
[Metrics API](https://github.com/triton-inference-server/server/blob/main/docs/user_guide/metrics.md#custom-metrics).
