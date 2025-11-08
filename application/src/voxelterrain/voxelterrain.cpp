#include "voxelterrain/voxelterrain.h"
#include "voxelterrain/voxelterrainhelpersfunctions.h"

namespace app
{
    VoxelTerrain VOXEL_TERRAIN;

    static void GenerateOctaveMap2D_Thread(VoxelOctave* octave, hf::uvec3 position, hf::uvec3 size);

    void VoxelTerrainGenerate()
    {
        VOXEL_TERRAIN.jobPool = new hf::ThreadPool<>();

        CreateVoxelOctreeRoot();
        CreateUnPrunableBranches();

        for (uint32_t x = 0; x < 32; x++)
        {
            for (uint32_t z = 0; z < 32; z++)
            {
                for (uint32_t y = 0; y < 32; y++)
                {
                    SetVoxel(hf::uvec3(x, y, z), VOXEL_GRASS);
                }
            }

            for (uint32_t z = 128; z < 128 + 32; z++)
            {
                for (uint32_t y = 0; y < 32; y++)
                {
                    SetVoxel(hf::uvec3(x, y, z), VOXEL_GRASS);
                }
            }
        }
    }

    void VoxelTerrainUpdate()
    {

    }

    void VoxelTerrainPreDraw()
    {

    }

    void VoxelTerrainDraw()
    {

    }

    void VoxelTerrainDispose()
    {
        delete VOXEL_TERRAIN.jobPool;
        VOXEL_TERRAIN.jobPool = nullptr;
    }
}