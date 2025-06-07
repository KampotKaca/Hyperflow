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

        BS::thread_pool<>* jobPool{};
    };

    extern VoxelTerrain VOXEL_TERRAIN;

    void VoxelTerrainGenerate();
    void VoxelTerrainUpdate();
    void VoxelTerrainDraw(const hf::Ref<hf::Renderer>& rn);
    void VoxelTerrainDispose();

    //returns the octave at a certain location
    VoxelType GetVoxel(hf::uvec3 position);
    VoxelOctave* SetVoxel(hf::uvec3 position, VoxelType type);
    VoxelOctave* PruneBranch(VoxelOctave* octave, uint32_t octaveDepth);

    void TrySetChildren(VoxelOctave* octave, hf::uvec4 octaveIndex);
    void TrySetChildren(VoxelOctave* octave, OctavePath octavePath);
    OctavePath CreateChildren(hf::uvec4 parentIndex);
    OctavePath CreateChildren(OctavePath parentPath);
    void FreeOctaves(OctavePath path, uint32_t count);

    inline OctavePath ToPath(const hf::uvec4 index) { return index.w << 18u | index.x << 12u | index.y << 6u | index.z; }

    inline hf::uvec4 GetIndexPath(OctavePath path)
    {
        hf::uvec4 location{};
        location.z = path & ((1u << 6) - 1u);
        path >>= 6;
        location.y = path & ((1u << 6) - 1u);
        path >>= 6;
        location.x = path & ((1u << 6) - 1u);
        path >>= 6;
        location.w = path & ((1u << 12) - 1u);

        return location;
    }

    inline VoxelOctave* GetOctave(const hf::uvec4 location)
    {
        return &VOXEL_TERRAIN.octreeChunks[location.w].cells[location.x].cells[location.y].cells[location.z];
    }

    inline VoxelOctave* GetOctave(OctavePath path) { return GetOctave(GetIndexPath(path)); }
}

#endif //VOXELTERRAIN_H
