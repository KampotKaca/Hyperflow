#ifndef VOXELTERRAINGENERATION_H
#define VOXELTERRAINGENERATION_H

#include "voxelterrain.h"
#include <FastNoiseLite.h>

namespace app
{
    struct VoxelTerrainNoiseSetup
    {
        fnl_state caveNoise{};
        fnl_state flatNoise{};
        fnl_state smallHillNoise{};
        fnl_state hillNoise{};
        fnl_state mountainNoise{};
        fnl_state highMountainNoise{};
    };

    void VoxelTerrainGenerateNoise();

    VoxelTerrainNoiseSetup VoxelTerrainGetNoiseSetup();
    fnl_state VoxelTerrainGetCaveNoise();
    fnl_state VoxelTerrainGetFlatNoise();
    fnl_state VoxelTerrainGetSmallHillNoise();
    fnl_state VoxelTerrainGetHillNoise();
    fnl_state VoxelTerrainGetMountainNoise();
    fnl_state VoxelTerrainGetHighMountainNoise();
}

#endif //VOXELTERRAINGENERATION_H
