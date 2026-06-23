#ifndef RecoTracker_LSTCore_interface_LSTSonic_h
#define RecoTracker_LSTCore_interface_LSTSonic_h

#include <map>
#include <string>

// Plain host function — no alpaka namespace, no template
// Triton backend calls this directly

namespace lst{

  class LST_SONIC {

  public:

    void lst_run_SONIC(
      bool verbose,
      const std::map<std::string, const void*>& input_data,
      const std::map<std::string, unsigned int>& input_sizes,
      bool no_pls_dupclean,
      bool tc_pls_triplets
    );

    unsigned int& getOutput_nTrackCandidates() {
      return output_nTrackCandidates_;
    }

    std::vector<unsigned int>& getOutput_pixelSeedIndex() {
        return output_pixelSeedIndex_;  // 
    }

    std::vector<int8_t>& getOutput_trackCandidateType() {
        return output_trackCandidateType_;  // 
    }

    std::vector<unsigned int>& getOutput_hitIndices_flat() {
        return output_hitIndices_flat_;
    }

    size_t getOutputBufferSize_nTrackCandidates() {return sizeof(unsigned int);} // YY: added
    size_t getOutputBufferSize_pixelSeedIndex() {return output_pixelSeedIndex_.size() * sizeof(unsigned int);}
    size_t getOutputBufferSize_trackCandidateType() {return output_trackCandidateType_.size() * sizeof(int8_t);}
    size_t getOutputBufferSize_hitIndices_flat() {return output_hitIndices_flat_.size() * sizeof(unsigned int);}

  private:
    // Output collection
    unsigned int output_nTrackCandidates_;
    std::vector<unsigned int> output_pixelSeedIndex_;
    std::vector<int8_t> output_trackCandidateType_; // try int8_t if it does not work // LSTObjType
    std::vector<unsigned int> output_hitIndices_flat_;

  };
} // namespace lst
#endif
