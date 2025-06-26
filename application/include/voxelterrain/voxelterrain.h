#ifndef VOXELTERRAIN_H
#define VOXELTERRAIN_H

#include "hyperflow.h"
#include "../../appconfig.h"

namespace app
{
    enum VoxelType : uint16_t
    {
        VOXEL_NONE = 0, VOXEL_GRASS = 1, VOXEL_DIRT = 2, VOXEL_STONE = 3,
    };

    enum VoxelFlags : uint16_t
    {
        VOXEL_FLAG_NONE = 0, VOXEL_FLAG_IS_LEAF = 1 << 0, VOXEL_FLAG_IS_ROOT = 1 << 1
    };

    //12 bit chunk location 6x3 bit path to the octave
    typedef uint32_t OctavePath;

    struct VoxelOctave
    {
        OctavePath firstChildLocation{};
        OctavePath parentLocation{};
        VoxelType type{};
        VoxelFlags flags{};
    };

    typedef hf::OctreeChunk<VoxelOctave> Chunk_L0;
    typedef hf::OctreeChunk<Chunk_L0> Chunk_L1;
    typedef hf::OctreeChunk<Chunk_L1> Chunk_L2;

    struct VoxelTerrain
    {
        Chunk_L2 cl;
        VoxelOctave* octreeRoot{};
        hf::uvec4 octreeRootIndex{};
        std::vector<Chunk_L2> octreeChunks{};
        uint32_t terrainAxisSize{};
        uint32_t usedOctaves{};

        hf::ThreadPool<>* jobPool{};
    };

    extern VoxelTerrain VOXEL_TERRAIN;

    void VoxelTerrainGenerate();
    void VoxelTerrainUpdate();

    void VoxelTerrainPreDraw(const hf::Ref<hf::Renderer>& rn);
    void VoxelTerrainDraw(const hf::Ref<hf::Renderer>& rn);

    void VoxelTerrainDispose();
}

#endif //VOXELTERRAIN_H
