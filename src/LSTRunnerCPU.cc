// Compiled with g++ + ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED.
// Wraps alpaka_serial_sync::lst::LST (from liblst_cpu.so) in the ILSTSonic
// interface so that LST.cc can call it without alpaka headers.

#include "ILSTSonic.h"
#include "../LSTCore/interface/alpaka/LST.h"

namespace lst {

class LSTCPUAdapter : public ILSTSonic {
 public:
  LSTCPUAdapter() = default;

  void run_SONIC(bool verbose,
                 std::map<std::string, const void*> inputs_name_buffer,
                 std::map<std::string, uint32_t> inputs_name_buffer_byte_size,
                 bool no_pls_dupclean,
                 bool tc_pls_triplets) override {
    impl_.run_SONIC(verbose, inputs_name_buffer, inputs_name_buffer_byte_size,
                    no_pls_dupclean, tc_pls_triplets);
  }

  const unsigned int& getOutput_nTrackCandidates() const override {
    return impl_.getOutput_nTrackCandidates();
  }
  const std::vector<unsigned int>& getOutput_pixelSeedIndex() const override {
    return impl_.getOutput_pixelSeedIndex();
  }
  const std::vector<int8_t>& getOutput_trackCandidateType() const override {
    return impl_.getOutput_trackCandidateType();
  }
  const std::vector<unsigned int>& getOutput_hitIndices_flat() const override {
    return impl_.getOutput_hitIndices_flat();
  }

  size_t getOutputBufferSize_nTrackCandidates() const override {
    return impl_.getOutputBufferSize_nTrackCandidates();
  }
  size_t getOutputBufferSize_pixelSeedIndex() const override {
    return impl_.getOutputBufferSize_pixelSeedIndex();
  }
  size_t getOutputBufferSize_trackCandidateType() const override {
    return impl_.getOutputBufferSize_trackCandidateType();
  }
  size_t getOutputBufferSize_hitIndices_flat() const override {
    return impl_.getOutputBufferSize_hitIndices_flat();
  }

 private:
  alpaka_serial_sync::lst::LST impl_;
};

ILSTSonic* createCPURunner() { return new LSTCPUAdapter(); }

}  // namespace lst
