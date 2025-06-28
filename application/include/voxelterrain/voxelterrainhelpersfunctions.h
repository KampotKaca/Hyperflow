#ifndef VOXELTERRAINHELPERSFUNCTIONS_H
#define VOXELTERRAINHELPERSFUNCTIONS_H

#include "hyperflow.h"
#include "voxelterrain.h"

namespace app
{
    //returns the octave at a certain location
    VoxelType GetVoxel(hf::uvec3 position);
    VoxelOctave* SetVoxel(hf::uvec3 position, VoxelType type);
    VoxelOctave* PruneBranch(VoxelOctave* octave, uint32_t octaveDepth);

    void TrySetChildren(VoxelOctave* octave, hf::uvec4 octaveIndex);
    void TrySetChildren(VoxelOctave* octave, OctavePath octavePath);
    OctavePath CreateChildren(hf::uvec4 parentIndex);
    OctavePath CreateChildren(OctavePath parentPath);
    void FreeOctaves(OctavePath path, uint32_t count);

    void CreateVoxelOctreeRoot();
    void CreateUnPrunableBranches();

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

#endif //VOXELTERRAINHELPERSFUNCTIONS_H
