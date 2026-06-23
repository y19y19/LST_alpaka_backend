#!/bin/bash

###########################################################################################################
# Setup environments
###########################################################################################################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/code/rooutil/thisrooutil.sh

ARCH=$(uname -m)
export SCRAM_ARCH=el8_amd64_gcc13 # for dedicated machine and version
if [ -z ${CMSSW_SEARCH_PATH+x} ]; then
  if [ -z ${FORCED_CMSSW_VERSION+x} ]; then
    export CMSSW_VERSION=CMSSW_16_1_0_pre4
  else
    export CMSSW_VERSION=$FORCED_CMSSW_VERSION
  fi

  source /cvmfs/cms.cern.ch/cmsset_default.sh
  CMSSW_PATH=$(scram list -c CMSSW | grep -w $CMSSW_VERSION | awk '{print $3}')
  echo $CMSSW_PATH
  cd $CMSSW_PATH
  eval `scramv1 runtime -sh`
else
  cd $CMSSW_BASE/src
fi

# Export paths to libraries we need
export ALPAKA_ROOT=$(scram tool info alpaka | grep ALPAKA_BASE | cut -d'=' -f2)
export BOOST_ROOT=$(scram tool info boost | grep BOOST_BASE | cut -d'=' -f2)
export CUDA_HOME=$(scram tool info cuda | grep CUDA_BASE | cut -d'=' -f2)
export FMT_ROOT=$(scram tool info fmt | grep FMT_BASE | cut -d'=' -f2)
export ROCM_ROOT=$(scram tool info rocm | grep ROCM_BASE | cut -d'=' -f2)
export ROOT_ROOT=$(scram tool info root_interface | grep ROOT_INTERFACE_BASE | cut -d'=' -f2)
export JSON_ROOT=$(scram tool info json | grep JSON_BASE | cut -d'=' -f2)
echo "ALPAKA_ROOT=${ALPAKA_ROOT}" 
echo "BOOST_ROOT=${BOOST_ROOT}"
echo "CUDA_HOME=${CUDA_HOME}"
echo "FMT_ROOT=${FMT_ROOT}"
echo "ROCM_ROOT=${ROCM_ROOT}" 
echo "ROOT_ROOT=${ROOT_ROOT}"
echo "JSON_ROOT=${JSON_ROOT}"

#ALPAKA_ROOT=/cvmfs/cms.cern.ch/el8_amd64_gcc13/external/alpaka/2.1.1-3caaac8d71f39d400ab2511b2403675a
#BOOST_ROOT=/cvmfs/cms.cern.ch/el8_amd64_gcc13/external/boost/1.80.0-6429d45cb860600db5fc5d037104a1f8
#CUDA_HOME=/cvmfs/cms.cern.ch/el8_amd64_gcc13/external/cuda/12.9.1-cff83d5f72da96ebfea8cafd87a05296
#FMT_ROOT=/cvmfs/cms.cern.ch/el8_amd64_gcc13/external/fmt/10.2.1-31d67b0504b4ba2262f03d3c5cad83c1
#ROCM_ROOT=/cvmfs/cms.cern.ch/el8_amd64_gcc13/external/rocm/7.1.0-3368aff4c211469d06c08ddf03fbf8f5
#ROOT_ROOT=/cvmfs/cms.cern.ch/el8_amd64_gcc13/lcg/root/6.36.11-c9684ba7622be6ba003c6d1250a79c91
#JSON_ROOT=/cvmfs/cms.cern.ch/el8_amd64_gcc13/external/json/3.11.3-3bf93f232c63d55abe3d04e52aa0a168

cd - > /dev/null
echo "Setup following ROOT. Make sure the appropriate setup file has been run. Otherwise the looper won't compile."
which root

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH=$DIR/LST:$DIR:$LD_LIBRARY_PATH
export PATH=$DIR/bin:$PATH
export PATH=$DIR/efficiency/bin:$PATH
export PATH=$DIR/efficiency/python:$PATH
export TRACKLOOPERDIR=$DIR
#export TRACKINGNTUPLEDIR=/data2/segmentlinking/CMSSW_12_2_0_pre2/
export TRACKINGNTUPLEDIR=/depot/cms/users/yao317/datasets/
export LSTOUTPUTDIR=.

hostname=$(hostname)
if [[ $hostname == *cornell* ]]; then
  export LSTPERFORMANCEWEBDIR="/cdat/tem/${USER}/LSTPerformanceWeb"
else
  export LSTPERFORMANCEWEBDIR="/home/users/phchang/public_html/LSTPerformanceWeb"
fi

###########################################################################################################
# Validation scripts
###########################################################################################################

# List of benchmark efficiencies are set as an environment variable
export LATEST_CPU_BENCHMARK_EFF_MUONGUN="/data2/segmentlinking/muonGun_cpu_efficiencies.root"
export LATEST_CPU_BENCHMARK_EFF_PU200="/data2/segmentlinking/pu200_cpu_efficiencies.root"
#eof
