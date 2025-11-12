#ifndef HOCTREE_H
#define HOCTREE_H

#include "hshared.h"

namespace hf
{
    template<typename T>
    struct OctreeChunk
    {
        T cells[64]{};
        uint64_t occupancyMask = 0;

        [[nodiscard]] bool IsOccupied() const { return occupancyMask == ~0llu; }
        void OccupyPosition(const uint8_t index) { occupancyMask |= 1llu << index; }

        [[nodiscard]] uint8_t GetFreeNode() const
        {
            if (IsOccupied()) return 255u;
            return utils::GetFirstBitZero64(occupancyMask);
        }

        uint8_t OccupyNode()
        {
            if (IsOccupied()) return 255u;
            uint8_t location = utils::GetFirstBitZero64(occupancyMask);
            OccupyPosition(location);
            return location;
        }

        void ReleaseNode(const uint8_t index)
        {
            occupancyMask &= ~(1llu << index);
        }
    };

    template<typename T, int Size>
    struct Octree
    {
        OctreeChunk<OctreeChunk<T>> chunks[Size]{};

        T* At(uvec3 pos) { return &chunks[pos.x].cells[pos.y].cells[pos.z]; }

        uvec3 OccupyNode()
        {
            uvec3 result{};
            for (result.x = 0; result.x < Size; result.x++)
            {
                auto& chunk = chunks[result.x];
                if (chunk.IsOccupied()) continue;

                result.y = chunk.GetFreeNode();
                auto& chunkT0 = chunk.cells[result.y];
                result.z = chunkT0.OccupyNode();
                if (chunkT0.IsOccupied()) chunk.OccupyPosition(result.z);
                return result;
            }

            log_fatal_s("[Hyperflow] Chunk is filled!!!");
            abort();
        }

        void FreeNode(uvec3 index)
        {
            auto chunkT0 = chunks[index.x];
            auto chunkT1 = chunkT0.cells[index.y];
            chunkT0.ReleaseNode(index.y);
            chunkT1.ReleaseNode(index.z);
        }
    };
}

#endif //HOCTREE_H
