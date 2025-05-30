#include "voxelterrain.h"

namespace app
{
    VoxelTerrain VOXEL_TERRAIN;

#define OCTAVE_IS_LEAF(v) (v.type & (1 << 12) > 0)
#define OCTAVE_SET_IS_LEAF(v) v.type |= (1 << 12)
#define OCTAVE_REMOVE_IS_LEAF(v) v.type &= ~(1 << 12)
#define OCTAVE_GET_TYPE(v) (v.type & ((1 << 12) - 1))
#define OCTAVE_SET_TYPE(v, t) v.type &= ~((1 << 12) - 1);\
                             v.type |= t

    static inline bool IsLeaf(const VoxelOctave& v) { return v.firstChildLocation == 0; }

    void VoxelTerrainGenerate()
    {
        VOXEL_TERRAIN.octreeChunks = std::vector<Chunk_L2>(1);

    }

    void VoxelTerrainUpdate()
    {

    }

    void VoxelTerrainDraw(const hf::Ref<hf::Renderer>& rn)
    {

    }
}