#ifndef LST_BACKEND_ILSTSONIC_H
#define LST_BACKEND_ILSTSONIC_H

// Pure C++ dispatch interface for LST SONIC execution.
// No alpaka or CMSSW dependencies — allows LST.cc (g++-compiled Triton backend)
// to dispatch to either a CPU or GPU runner via virtual calls.

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace lst {

class ILSTSonic {
 public:
  virtual ~ILSTSonic() = default;

  virtual void run_SONIC(bool verbose,
                         std::map<std::string, const void*> inputs_name_buffer,
                         std::map<std::string, uint32_t> inputs_name_buffer_byte_size,
                         bool no_pls_dupclean,
                         bool tc_pls_triplets) = 0;

  virtual const unsigned int& getOutput_nTrackCandidates() const = 0;
  virtual const std::vector<unsigned int>& getOutput_pixelSeedIndex() const = 0;
  virtual const std::vector<int8_t>& getOutput_trackCandidateType() const = 0;
  virtual const std::vector<unsigned int>& getOutput_hitIndices_flat() const = 0;

  virtual size_t getOutputBufferSize_nTrackCandidates() const = 0;
  virtual size_t getOutputBufferSize_pixelSeedIndex() const = 0;
  virtual size_t getOutputBufferSize_trackCandidateType() const = 0;
  virtual size_t getOutputBufferSize_hitIndices_flat() const = 0;
};

// Factory functions — defined in LSTRunnerCPU.cc and LSTRunnerGPU.cc.
ILSTSonic* createCPURunner();
ILSTSonic* createGPURunner();

}  // namespace lst

#endif  // LST_BACKEND_ILSTSONIC_H
