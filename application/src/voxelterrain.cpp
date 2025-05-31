#include "voxelterrain.h"

namespace app
{
    VoxelTerrain VOXEL_TERRAIN;

    void VoxelTerrainGenerate()
    {
        VOXEL_TERRAIN.octreeChunks = std::vector<Chunk_L2>(1);
        VOXEL_TERRAIN.terrainAxisSize = (uint32_t)std::pow(2, VOXEL_TERRAIN_MAX_DEPTH);

        auto& chunk = VOXEL_TERRAIN.octreeChunks[0].cells[0].cells[0];
        chunk.occupancyMask |= (1llu << 8llu) - 1llu;
        chunk.cells[1] = VoxelOctave
        {
            .firstChildLocation = 0,
            .parentLocation = 0,
            .type = VOXEL_NONE,
            .flags = (VoxelFlags)(VOXEL_FLAG_IS_ROOT | VOXEL_FLAG_IS_LEAF),
        };
        VOXEL_TERRAIN.octreeRoot = &chunk.cells[1];

        for (uint32_t x = 0; x < 255; x++)
        {
            for (uint32_t z = 0; z < 255; z++)
            {
                for (uint32_t y = 0; y < 255; y++)
                {
                    SetVoxel(hf::uvec3(x, y, z), VOXEL_GRASS);
                }
            }
        }
    }

    void VoxelTerrainUpdate()
    {

    }

    void VoxelTerrainDraw(const hf::Ref<hf::Renderer>& rn)
    {

    }

    hf::uvec4 GetIndexPath(OctavePath path)
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

    VoxelOctave* GetOctave(OctavePath path)
    {
        hf::uvec4 location = GetIndexPath(path);
        return &VOXEL_TERRAIN.octreeChunks[location.w].cells[location.x].cells[location.y].cells[location.z];
    }

    VoxelOctave* SetVoxel(hf::uvec3 position, VoxelType type)
    {
        auto currentOctave = VOXEL_TERRAIN.octreeRoot;
        hf::uvec4 currentOctavePath = hf::uvec4(0, 0, 1, 0);
        auto currentAxisSize = VOXEL_TERRAIN.terrainAxisSize;
        hf::uvec3 pos = position;

        while (currentAxisSize > 1)
        {
            const auto step = currentAxisSize / 2;
            uint32_t index = ((pos.x >= step) << 2) | ((pos.y >= step) << 1) || (pos.z >= step);
            pos += hf::uvec3(index & 1, index & 2, index & 4) * step;

            auto& flags = currentOctave->flags;
            if (flags & VOXEL_FLAG_IS_LEAF)
            {
                flags = (VoxelFlags)(flags & ~VOXEL_FLAG_IS_LEAF);
                currentOctave->firstChildLocation = ReserveOctaves(ToPath(currentOctavePath));
            }

            currentOctave = &GetOctave(currentOctave->firstChildLocation)[index];
            currentAxisSize = step;
        }

        currentOctave->type = type;
        return PruneBranch(currentOctave);
    }

    OctavePath ToPath(hf::uvec4 index)
    {
        OctavePath path = index.w;
        path <<= 6;
        path |= index.x;
        path <<= 6;
        path |= index.y;
        path <<= 6;
        path |= index.z;
        return path;
    }

    VoxelOctave* PruneBranch(VoxelOctave* octave)
    {
        if (octave->parentLocation)
        {
            auto parent = GetOctave(octave->parentLocation);
            auto children = GetOctave(parent->firstChildLocation);
            auto type = children[0].type;
            for (uint32_t i = 1; i < 8; i++) if (type != children[i].type) return octave;

            //Free the memory
            FreeOctaves(parent->firstChildLocation, 8);
            parent->firstChildLocation = 0;
            parent->flags = (VoxelFlags)(parent->flags | VOXEL_FLAG_IS_LEAF);
            return PruneBranch(parent);
        }
        return octave;
    }

    VoxelType GetVoxel(hf::uvec3 position)
    {
        auto currentOctave = VOXEL_TERRAIN.octreeRoot;
        auto currentAxisSize = VOXEL_TERRAIN.terrainAxisSize;
        hf::uvec3 pos = position;

        while (currentAxisSize > 1)
        {
            if (currentOctave->flags & VOXEL_FLAG_IS_LEAF) return currentOctave->type;

            const auto step = currentAxisSize / 2;
            uint32_t index = ((pos.x >= step) << 2) | ((pos.y >= step) << 1) || (pos.z >= step);
            pos += hf::uvec3(index & 1, index & 2, index & 4) * step;

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

    OctavePath ReserveOctaves(OctavePath parentPath)
    {
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
        if (chunkL0.occupancyMask == 0) chunkL1.occupancyMask &= ~(1llu << location.y);
        if (chunkL1.occupancyMask == 0) chunkL2.occupancyMask &= ~(1llu << location.x);
    }
}