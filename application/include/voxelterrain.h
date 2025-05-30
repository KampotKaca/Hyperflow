#ifndef VOXELTERRAIN_H
#define VOXELTERRAIN_H

#include "hyperflow.h"
#include "../appconfig.h"

namespace app
{
    struct VoxelOctave
    {
        uint32_t firstChildLocation{};
        uint32_t parentLocation{};
        uint16_t type{}; //lower 12 bits are type, higher 4 bits are flags
    };

    template<typename T>
    struct Chunk
    {
        T cells[64]{};
        uint64_t occupancyMask = 0;
    };

    typedef Chunk<VoxelOctave> Chunk_L0;
    typedef Chunk<Chunk_L0> Chunk_L1;
    typedef Chunk<Chunk_L1> Chunk_L2;

    struct VoxelTerrain
    {
        std::vector<Chunk_L2> octreeChunks{};
    };

    extern VoxelTerrain VOXEL_TERRAIN;

    void VoxelTerrainGenerate();
    void VoxelTerrainUpdate();
    void VoxelTerrainDraw(const hf::Ref<hf::Renderer>& rn);
}

#endif //VOXELTERRAIN_H
