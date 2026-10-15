#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "nvdsinfer_custom_impl.h"

extern "C" bool NvDsInferParseCustomYoloV8(
    std::vector<NvDsInferLayerInfo> const& outputLayersInfo,
    NvDsInferNetworkInfo const& networkInfo,
    NvDsInferParseDetectionParams const& detectionParams,
    std::vector<NvDsInferObjectDetectionInfo>& objectList)
{
    if (outputLayersInfo.empty()) return false;
    const NvDsInferLayerInfo& layer = outputLayersInfo[0];
    const float* data = static_cast<const float*>(layer.buffer);

    const unsigned int numClasses = detectionParams.numClassesConfigured;
    const unsigned int channels = layer.inferDims.d[0];
    const unsigned int numAnchors = layer.inferDims.d[1];
    if (channels < 4 + numClasses) return false;

    for (unsigned int i = 0; i < numAnchors; ++i) {
        float maxProb = 0.0f;
        int maxClassId = -1;
        for (unsigned int c = 0; c < numClasses; ++c) {
            float prob = data[(4 + c) * numAnchors + i];
            if (prob > maxProb) {
                maxProb = prob;
                maxClassId = static_cast<int>(c);
            }
        }
        if (maxClassId < 0 || maxProb < detectionParams.perClassPreclusterThreshold[maxClassId]) continue;

        float cx = data[0 * numAnchors + i];
        float cy = data[1 * numAnchors + i];
        float w  = data[2 * numAnchors + i];
        float h  = data[3 * numAnchors + i];

        float left   = std::max(0.0f, cx - w * 0.5f);
        float top    = std::max(0.0f, cy - h * 0.5f);
        float right  = std::min(static_cast<float>(networkInfo.width), cx + w * 0.5f);
        float bottom = std::min(static_cast<float>(networkInfo.height), cy + h * 0.5f);
        if (right <= left || bottom <= top) continue;

        NvDsInferObjectDetectionInfo obj;
        obj.classId = maxClassId;
        obj.left = left;
        obj.top = top;
        obj.width = right - left;
        obj.height = bottom - top;
        obj.detectionConfidence = maxProb;
        objectList.push_back(obj);
    }
    return true;
}

CHECK_CUSTOM_PARSE_FUNC_PROTOTYPE(NvDsInferParseCustomYoloV8);
