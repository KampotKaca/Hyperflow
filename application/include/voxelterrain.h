#ifndef VOXELTERRAIN_H
#define VOXELTERRAIN_H

#include "hyperflow.h"
#include "../appconfig.h"

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
        VoxelOctave* octreeRoot{};
        std::vector<Chunk_L2> octreeChunks{};
        uint32_t terrainAxisSize{};
    };

    extern VoxelTerrain VOXEL_TERRAIN;

    void VoxelTerrainGenerate();
    void VoxelTerrainUpdate();
    void VoxelTerrainDraw(const hf::Ref<hf::Renderer>& rn);

    //returns the octave at a certain location
    hf::uvec4 GetIndexPath(OctavePath path);
    OctavePath ToPath(hf::uvec4 index);
    VoxelType GetVoxel(hf::uvec3 position);
    VoxelOctave* GetOctave(OctavePath path);
    VoxelOctave* SetVoxel(hf::uvec3 position, VoxelType type);
    VoxelOctave* PruneBranch(VoxelOctave* octave);

    OctavePath ReserveOctaves(OctavePath parentPath);
}

#endif //VOXELTERRAIN_H
