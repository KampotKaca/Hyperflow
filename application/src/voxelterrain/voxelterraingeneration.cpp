#include "voxelterrain/voxelterraingeneration.h"

namespace app
{
    VoxelTerrainNoiseSetup NOISE_SETUP{};

    void VoxelTerrainGenerateNoise()
    {
        NOISE_SETUP = VoxelTerrainGetNoiseSetup();
    }
}

