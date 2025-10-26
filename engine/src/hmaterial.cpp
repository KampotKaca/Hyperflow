#include "hmaterial.h"
#include "hgenericexception.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    static Octree<Material*, RENDERING_MAX_MATERIAL_OCTREE_COUNT> MATERIAL_OCTREE{};

    Material::Material(const MaterialCreationInfo& info) : sizeInBytes(info.sizeInBytes)
    {
        if (sizeInBytes > RENDERING_MAX_MATERIAL_MEMORY_BADGET)
            throw GENERIC_EXCEPT("[Hyperflow]", "Size is over the maximum material badget!");

        if (sizeInBytes > 0)
        {
            octreePosition = MATERIAL_OCTREE.OccupyNode();
            *MATERIAL_OCTREE.At(octreePosition) = this;

            bufferIndex = octreePosition.x * 64 * 64 + octreePosition.y * 64 + octreePosition.z;
            bufferMemory = (uint8_t*)utils::Alloc(sizeInBytes);
        }
    }

    Material::~Material()
    {
        if (sizeInBytes)
        {
            utils::Deallocate(bufferMemory);
            *MATERIAL_OCTREE.At(octreePosition) = nullptr;
            MATERIAL_OCTREE.FreeNode(octreePosition);
        }
    }

    Ref<Material> Create(const MaterialCreationInfo& info)
    {
        auto mat = MakeRef<Material>(info);
        inter::HF.graphicsResources.materials[(uint64_t)mat.get()] = mat;
        return mat;
    }

    uint16_t GetBufferIndex(const Ref<Material>& mat) { return mat->bufferIndex; }

    void Upload(const Ref<Material>& mat, const void* data)
    {
        if (mat->sizeInBytes > 0)
        {
            memcpy(mat->bufferMemory, data, mat->sizeInBytes);
            mat->isMarkedAsModified = true;
        }
        else throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload in empty material!");
    }
}
