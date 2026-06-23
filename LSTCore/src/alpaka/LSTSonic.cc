#include "RecoTracker/LSTCore/interface/alpaka/LST.h"

#include "RecoTracker/LSTCore/interface/LSTSonic.h"

void LST::run_SONIC(bool verbose,
                   //LSTESData<alpaka_common::DevHost> const* hostESData,
                   //LSTInputHostCollection const* lstInputHC,  
                   std::map<std::string, const void*> inputs_name_buffer,
                   std::map<std::string, uint32_t> inputs_name_buffer_byte_size,
                   bool no_pls_dupclean,
                   bool tc_pls_triplets) {

    const float* see_px_ptr = static_cast<const float*>(inputs_name_buffer["see_px"]);
    const float* see_py_ptr = static_cast<const float*>(inputs_name_buffer["see_py"]);
    const float* see_pz_ptr = static_cast<const float*>(inputs_name_buffer["see_pz"]);
    const float* see_dxy_ptr = static_cast<const float*>(inputs_name_buffer["see_dxy"]);
    const float* see_dz_ptr = static_cast<const float*>(inputs_name_buffer["see_dz"]);
    const float* see_ptErr_ptr = static_cast<const float*>(inputs_name_buffer["see_ptErr"]);
    const float* see_etaErr_ptr = static_cast<const float*>(inputs_name_buffer["see_etaErr"]);
    const float* see_stateTrajGlbX_ptr = static_cast<const float*>(inputs_name_buffer["see_stateTrajGlbX"]);
    const float* see_stateTrajGlbY_ptr = static_cast<const float*>(inputs_name_buffer["see_stateTrajGlbY"]);
    const float* see_stateTrajGlbZ_ptr = static_cast<const float*>(inputs_name_buffer["see_stateTrajGlbZ"]);
    const float* see_stateTrajGlbPx_ptr = static_cast<const float*>(inputs_name_buffer["see_stateTrajGlbPx"]);
    const float* see_stateTrajGlbPy_ptr = static_cast<const float*>(inputs_name_buffer["see_stateTrajGlbPy"]);
    const float* see_stateTrajGlbPz_ptr = static_cast<const float*>(inputs_name_buffer["see_stateTrajGlbPz"]);
    const int* see_q_ptr = static_cast<const int*>(inputs_name_buffer["see_q"]);
    const int* see_hit_size_ptr = static_cast<const int*>(inputs_name_buffer["see_hit_size"]);
    const int* see_hitIdx_ptr = static_cast<const int*>(inputs_name_buffer["see_hitIdx"]);
    const int* see_hitType_ptr = static_cast<const int*>(inputs_name_buffer["see_hitType"]);
    const unsigned int* see_algo_ptr = static_cast<const unsigned int*>(inputs_name_buffer["see_algo"]);
    const unsigned int* ph2_detId_ptr = static_cast<const unsigned int*>(inputs_name_buffer["ph2_detId"]);
    const uint16_t* ph2_clustSize_ptr = static_cast<const uint16_t*>(inputs_name_buffer["ph2_clustSize"]);
    const float* ph2_x_ptr = static_cast<const float*>(inputs_name_buffer["ph2_x"]);
    const float* ph2_y_ptr = static_cast<const float*>(inputs_name_buffer["ph2_y"]);
    const float* ph2_z_ptr = static_cast<const float*>(inputs_name_buffer["ph2_z"]);

    const float* ptCut_ptr = static_cast<const float*>(inputs_name_buffer["ptCut"]);
    const uint16_t* clustSizeCut_ptr = static_cast<const uint16_t*>(inputs_name_buffer["clustSizeCut"]);

    int N_seed = inputs_name_buffer_byte_size["see_px"]/sizeof(float);
    std::vector<float> see_px(see_px_ptr, see_px_ptr + N_seed);
    std::vector<float> see_py(see_py_ptr, see_py_ptr + N_seed);
    std::vector<float> see_pz(see_pz_ptr, see_pz_ptr + N_seed);
    std::vector<float> see_dxy(see_dxy_ptr, see_dxy_ptr + N_seed);
    std::vector<float> see_dz(see_dz_ptr, see_dz_ptr + N_seed);
    std::vector<float> see_ptErr(see_ptErr_ptr, see_ptErr_ptr + N_seed);
    std::vector<float> see_etaErr(see_etaErr_ptr, see_etaErr_ptr + N_seed);
    std::vector<float> see_stateTrajGlbX(see_stateTrajGlbX_ptr, see_stateTrajGlbX_ptr + N_seed);
    std::vector<float> see_stateTrajGlbY(see_stateTrajGlbY_ptr, see_stateTrajGlbY_ptr + N_seed);
    std::vector<float> see_stateTrajGlbZ(see_stateTrajGlbZ_ptr, see_stateTrajGlbZ_ptr + N_seed);
    std::vector<float> see_stateTrajGlbPx(see_stateTrajGlbPx_ptr, see_stateTrajGlbPx_ptr + N_seed);
    std::vector<float> see_stateTrajGlbPy(see_stateTrajGlbPy_ptr, see_stateTrajGlbPy_ptr + N_seed);
    std::vector<float> see_stateTrajGlbPz(see_stateTrajGlbPz_ptr, see_stateTrajGlbPz_ptr + N_seed);

    std::vector<int> see_q(see_q_ptr, see_q_ptr + N_seed);
    std::vector<int> see_hit_size(see_hit_size_ptr, see_hit_size_ptr + N_seed);
    int N_hit = 0;
    for (int number: see_hit_size) N_hit += number;
    std::vector<int> see_hitIdx_flat(see_hitIdx_ptr, see_hitIdx_ptr + N_hit);
    std::vector<int> see_hitType_flat(see_hitType_ptr, see_hitType_ptr + N_hit);
    std::vector<std::vector<int>> see_hitIdx;
    std::vector<std::vector<int>> see_hitType;
    size_t index = 0;
    for (int size : see_hit_size) {
        see_hitIdx.emplace_back(see_hitIdx_flat.begin() + index, see_hitIdx_flat.begin() + index + size);
        see_hitType.emplace_back(see_hitType_flat.begin() + index, see_hitType_flat.begin() + index + size);
        index += size;
    }
    std::vector<unsigned int> see_algo(see_algo_ptr, see_algo_ptr + N_seed);
    int N_ph2 = inputs_name_buffer_byte_size["ph2_x"]/sizeof(float);
    std::vector<unsigned int> ph2_detId(ph2_detId_ptr, ph2_detId_ptr + N_ph2);
    std::vector<uint16_t> ph2_clustSize(ph2_clustSize_ptr, ph2_clustSize_ptr + N_ph2);
    std::vector<float> ph2_x(ph2_x_ptr, ph2_x_ptr + N_ph2);
    std::vector<float> ph2_y(ph2_y_ptr, ph2_y_ptr + N_ph2);
    std::vector<float> ph2_z(ph2_z_ptr, ph2_z_ptr + N_ph2);
    float ptCut = *ptCut_ptr;
    uint16_t clustSizeCut = *clustSizeCut_ptr;

    // YY: this is found in standalone/bin/lst.cc run_lst()
    ALPAKA_ACCELERATOR_NAMESPACE::Device devAcc = alpaka::getDevByIdx(ALPAKA_ACCELERATOR_NAMESPACE::Platform{}, 0u);
    //std::vector<ALPAKA_ACCELERATOR_NAMESPACE::Queue> queues;
    //queues.push_back(ALPAKA_ACCELERATOR_NAMESPACE::Queue(devAcc));
    ALPAKA_ACCELERATOR_NAMESPACE::Queue queue_(devAcc);

    LSTInputHostCollection lstInputHC =
      prepareInput(see_px,
                   see_py,
                   see_pz,
                   see_dxy,
                   see_dz,
                   see_ptErr,
                   see_etaErr,
                   see_stateTrajGlbX,
                   see_stateTrajGlbY,
                   see_stateTrajGlbZ,
                   see_stateTrajGlbPx,
                   see_stateTrajGlbPy,
                   see_stateTrajGlbPz,
                   see_q,
                   see_hitIdx,
                   see_hitType,
                   see_algo,
                   ph2_detId,
                   ph2_clustSize,
                   ph2_x,
                   ph2_y,
                   ph2_z,
                   ptCut,
                   queue_);

    // Get ESData on Host and then copy to Device
    std::string ptCutString = (ptCut >= 0.8) ? "0.8" : "0.6";
    std::unique_ptr<LSTESData<alpaka_common::DevHost>> hostESData = loadAndFillESHost(ptCutString); // YY: why this is not auto type? 


    auto deviceESData =
      cms::alpakatools::CopyToDevice<LSTESData<alpaka_common::DevHost>>::copyAsync(queue_, *hostESData.get());
    // Copy input from Host to Device
    //LSTInputDeviceCollection lstInputDC(lstInputHC.sizes(), queue_); // YY: old syntax? 
    LSTInputDeviceCollection lstInputDC(queue_, lstInputHC->metadata().size());
    alpaka::memcpy(queue_, lstInputDC.buffer(), lstInputHC.buffer());
    alpaka::wait(queue_);
    // Run LST 
    run(queue_,
        verbose /*verbose*/,
        ptCut,
        clustSizeCut,
        &deviceESData,
        &lstInputDC,
        no_pls_dupclean /*no_pls_dupclean*/,
        tc_pls_triplets /*tc_pls_triplets*/
    );

   std::unique_ptr<TrackCandidatesBaseDeviceCollection> lstOutputDC_ptr = getTrackCandidates();

   // YY: now move it to Host if it is on device, otherwise not...  trackCandidatesBaseDC_
   // Idk why the previous one works but this does not work, let's try compile and let Claude code see what's wrong
   //TrackCandidatesBaseHostCollection lstOutputHC(queue_, lstOutputDC_ptr->metadata().size());
   TrackCandidatesBaseHostCollection lstOutputHC(queue_, lstOutputDC_ptr->size());
   alpaka::memcpy(queue_, lstOutputHC.buffer(), lstOutputDC_ptr->buffer());
   alpaka::wait(queue_);

   // Get data out of HC
   auto lstOutput_view = lstOutputHC.const_view();
   output_nTrackCandidates_ = lstOutput_view.nTrackCandidates();
   for (unsigned int i = 0; i < output_nTrackCandidates_; i++) {
      output_pixelSeedIndex_.push_back(lstOutput_view.pixelSeedIndex()[i]);
      output_trackCandidateType_.push_back(lstOutput_view.trackCandidateType()[i]);
      for (unsigned int layerSlot = 0; layerSlot < lst::Params_TC::kLayers; ++layerSlot) {
         // 13 layers
         for (unsigned int hitSlot = 0; hitSlot < lst::Params_TC::kHitsPerLayer; ++hitSlot) {
           // 2 hits
           output_hitIndices_flat_.push_back(lstOutput_view.hitIndices()[i][layerSlot][hitSlot]);
         }
      }
   }


