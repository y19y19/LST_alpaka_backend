export CMSSW_RELEASE_BASE=/cvmfs/cms.cern.ch/el8_amd64_gcc13/ 
export GCCDIR=$CMSSW_RELEASE_BASE/external/gcc/13.4.0-6908cfdf803923e783448096ca4f0923/bin/gcc
export PATH=$GCCDIR/bin:$PATH
export LD_LIBRARY_PATH=$GCCDIR/lib64:$GCCDIR/lib:$LD_LIBRARY_PATH
