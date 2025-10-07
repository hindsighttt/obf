#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace INVENTORY {
    void OnFrameStageNotify(int nStage);
    extern int customModelIndex;
    extern int MaxcustomModelIndex;
    extern int current_weapon_index;

    // New structure to hold flattened model data
    struct ModelInfo {
        std::string category;
        std::string pack;
        std::string modelName;
        std::string fullPath;
        std::string relativePath;
    };

    extern std::vector<ModelInfo> flatModelList;
}