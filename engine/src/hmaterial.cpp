#define HF_ENGINE_INTERNALS
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
            bufferMemory = (uint8_t*)utils::Allocate(sizeInBytes);
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

    Ref<Material> Material::Create(const MaterialCreationInfo& info)
    {
        return MakeRef<Material>(info);
    }

    void Material::Upload(const void* data) const
    {
        if (sizeInBytes == 0) return;

        memcpy(bufferMemory, data, sizeInBytes);
        const StorageBufferUploadInfo uploadInfo
        {
            .data = bufferMemory,
            .offset = bufferIndex * RENDERING_MAX_MATERIAL_MEMORY_BADGET,
            .size = sizeInBytes
        };
        inter::HF.graphicsResources.materialDataStorage->Upload(uploadInfo);
    }
}
