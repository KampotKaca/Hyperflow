#include "voxelterrain/voxelterrainhelpersfunctions.h"

namespace app
{
    static void CreateChildrenLoop(VoxelOctave* octave, hf::uvec4 index, uint32_t depth);

    VoxelOctave* SetVoxel(hf::uvec3 position, VoxelType type)
    {
        auto* currentOctave = VOXEL_TERRAIN.octreeRoot;
        auto currentAxisSize = VOXEL_TERRAIN.terrainAxisSize;
        hf::uvec4 pathIndex = VOXEL_TERRAIN.octreeRootIndex;
        uint32_t depth = VOXEL_TERRAIN_MAX_DEPTH;

        while (currentAxisSize > 1)
        {
            const auto step = currentAxisSize / 2u;
            auto posOffset = hf::uvec3(
                (uint32_t)(position.x >= step),
                (uint32_t)(position.y >= step),
                (uint32_t)(position.z >= step));
            const uint32_t index = (posOffset.x << 2u) | (posOffset.y << 1u) | posOffset.z;
            position -= posOffset * step;

            TrySetChildren(currentOctave, pathIndex);

            pathIndex = GetIndexPath(currentOctave->firstChildLocation);
            pathIndex.z += index;
            currentOctave = GetOctave(pathIndex);
            currentAxisSize = step;
            depth--;
        }

        currentOctave->type = type;
        return PruneBranch(currentOctave, depth);
    }

    VoxelOctave* PruneBranch(VoxelOctave* octave, uint32_t octaveDepth)
    {
        if (octave->parentLocation && octaveDepth <= VOXEL_TERRAIN_MAX_CLEAR_DEPTH)
        {
            const auto parent = GetOctave(octave->parentLocation);
            const auto children = GetOctave(parent->firstChildLocation);

            auto initialType = children[0].type;
            if(initialType != children[1].type ||
               initialType != children[2].type ||
               initialType != children[3].type ||
               initialType != children[4].type ||
               initialType != children[5].type ||
               initialType != children[6].type ||
               initialType != children[7].type) return octave;

            //Free the memory
            FreeOctaves(parent->firstChildLocation, 8);
            parent->firstChildLocation = 0;
            parent->type = initialType;
            parent->flags = (VoxelFlags)(parent->flags | VOXEL_FLAG_IS_LEAF);
            return PruneBranch(parent, octaveDepth + 1);
        }
        return octave;
    }

    VoxelType GetVoxel(hf::uvec3 position)
    {
        auto currentOctave = VOXEL_TERRAIN.octreeRoot;
        auto currentAxisSize = VOXEL_TERRAIN.terrainAxisSize;

        while (currentAxisSize > 1)
        {
            if (currentOctave->flags & VOXEL_FLAG_IS_LEAF) return currentOctave->type;

            const auto step = currentAxisSize / 2u;
            hf::uvec3 posOffset = hf::uvec3((uint32_t)(position.x >= step), (uint32_t)(position.y >= step), (uint32_t)(position.z >= step));
            uint32_t index = (posOffset.x << 2u) | (posOffset.y << 1u) | posOffset.z;
            position -= posOffset * step;

            currentOctave = &GetOctave(currentOctave->firstChildLocation)[index];
            currentAxisSize = step;
        }

        return currentOctave->type;
    }

    void MarkOctaves(Chunk_L0& chunk, uint32_t offset, OctavePath parentPath)
    {
        for (uint32_t i = offset; i < offset + 8; i++)
        {
            auto& octave = chunk.cells[i];
            octave.flags = (VoxelFlags)(octave.flags | VOXEL_FLAG_IS_LEAF);
            octave.parentLocation = parentPath;
        }
    }

    void TrySetChildren(VoxelOctave* octave, hf::uvec4 octaveIndex)
    {
        auto& flags = octave->flags;
        if (flags & VOXEL_FLAG_IS_LEAF)
        {
            flags = (VoxelFlags)(flags & ~VOXEL_FLAG_IS_LEAF);
            octave->firstChildLocation = CreateChildren(octaveIndex);
        }
    }

    void TrySetChildren(VoxelOctave* octave, OctavePath octavePath)
    {
        auto& flags = octave->flags;
        if (flags & VOXEL_FLAG_IS_LEAF)
        {
            flags = (VoxelFlags)(flags & ~VOXEL_FLAG_IS_LEAF);
            octave->firstChildLocation = CreateChildren(octavePath);
        }
    }

    OctavePath CreateChildren(hf::uvec4 parentIndex) { return CreateChildren(ToPath(parentIndex)); }

    OctavePath CreateChildren(OctavePath parentPath)
    {
        VOXEL_TERRAIN.usedOctaves += 8;
        hf::uvec4 location{};
        for (location.w = 0; location.w < VOXEL_TERRAIN.octreeChunks.size(); location.w++)
        {
            auto& chunk = VOXEL_TERRAIN.octreeChunks[location.w];
            if (chunk.occupancyMask == ~0llu) continue;

            location.x = hf::utils::GetFirstBitZero64(chunk.occupancyMask);
            auto& chunkL1 = chunk.cells[location.x];
            location.y = hf::utils::GetFirstBitZero64(chunkL1.occupancyMask);
            auto& chunkL0 = chunkL1.cells[location.y];
            location.z = hf::utils::GetFirstBitZero64(chunkL0.occupancyMask);
            chunkL0.occupancyMask |= ((1llu << 8llu) - 1llu) << location.z;

            if (chunkL0.occupancyMask == ~0llu) chunkL1.occupancyMask |= 1llu << location.y;
            if (chunkL1.occupancyMask == ~0llu) chunk.occupancyMask |= 1llu << location.x;

            MarkOctaves(chunkL0, location.z, parentPath);
            return ToPath(location);
        }

        VOXEL_TERRAIN.octreeChunks.emplace_back();
        auto& chunk = VOXEL_TERRAIN.octreeChunks.back().cells[0].cells[0];
        chunk.occupancyMask |= (1llu << 8llu) - 1llu;
        location.x = 0;
        location.y = 0;
        location.z = 0;
        MarkOctaves(chunk, 0, parentPath);
        return ToPath(location);
    }

    void FreeOctaves(OctavePath path, uint32_t count)
    {
        hf::uvec4 location = GetIndexPath(path);
        auto& chunkL2 = VOXEL_TERRAIN.octreeChunks[location.w];
        auto& chunkL1 = chunkL2.cells[location.x];
        auto& chunkL0 = chunkL1.cells[location.y];
        auto* octave = &chunkL0.cells[location.z];

        memset(octave, 0, sizeof(VoxelOctave) * count);
        chunkL0.occupancyMask &= ~(((1llu << count) - 1llu) << location.z);
        if (chunkL0.occupancyMask != ~0llu) chunkL1.occupancyMask &= ~(1llu << location.y);
        if (chunkL1.occupancyMask != ~0llu) chunkL2.occupancyMask &= ~(1llu << location.x);
        VOXEL_TERRAIN.usedOctaves -= 8;
    }

    void CreateVoxelOctreeRoot()
    {
        VOXEL_TERRAIN.octreeChunks = std::vector<Chunk_L2>(1);
        VOXEL_TERRAIN.terrainAxisSize = (uint32_t)std::pow(2, VOXEL_TERRAIN_MAX_DEPTH);
        VOXEL_TERRAIN.octreeRootIndex = hf::uvec4(0, 0, 1, 0);

        auto& chunk = VOXEL_TERRAIN.octreeChunks[VOXEL_TERRAIN.octreeRootIndex.w]
                                          .cells[VOXEL_TERRAIN.octreeRootIndex.x]
                                          .cells[VOXEL_TERRAIN.octreeRootIndex.y];
        chunk.occupancyMask |= (1llu << 8llu) - 1llu;
        chunk.cells[VOXEL_TERRAIN.octreeRootIndex.z] = VoxelOctave
        {
            .firstChildLocation = 0,
            .parentLocation = 0,
            .type = VOXEL_NONE,
            .flags = (VoxelFlags)(VOXEL_FLAG_IS_ROOT | VOXEL_FLAG_IS_LEAF),
        };
        VOXEL_TERRAIN.octreeRoot = &chunk.cells[VOXEL_TERRAIN.octreeRootIndex.z];
        VOXEL_TERRAIN.usedOctaves = 8;
    }

    void CreateUnPrunableBranches()
    {
        CreateChildrenLoop(VOXEL_TERRAIN.octreeRoot, VOXEL_TERRAIN.octreeRootIndex, VOXEL_TERRAIN_MAX_DEPTH - VOXEL_TERRAIN_MAX_CLEAR_DEPTH);
    }

    void CreateChildrenLoop(VoxelOctave* octave, hf::uvec4 index, uint32_t depth)
    {
        if (depth == 0) return;
        TrySetChildren(octave, index);
        depth--;
        index = GetIndexPath(octave->firstChildLocation);
        auto* children = GetOctave(index);
        for (uint32_t i = 0; i < 8; i++) CreateChildrenLoop(&children[i], index, depth);
    }
}