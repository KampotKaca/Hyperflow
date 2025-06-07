#include "hmaterial.h"
#include "hgenericexception.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    static Octree<Material*, RENDERING_MAX_MATERIAL_OCTREE_COUNT> MATERIAL_OCTREE{};

    Material::Material(const MaterialCreationInfo& info) : shader(info.shader), sizeInBytes(info.sizeInBytes)
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

    namespace material
    {
        Ref<Material> Create(const MaterialCreationInfo& info)
        {
            return MakeRef<Material>(info);
        }

        void Upload(const Ref<Material>& material, const void* data)
        {
            if (material->sizeInBytes == 0) return;

            memcpy(material->bufferMemory, data, material->sizeInBytes);
            const StorageBufferUploadInfo uploadInfo
            {
                .buffer = inter::HF.graphicsResources.materialDataStorage,
                .data = material->bufferMemory,
                .offset = material->bufferIndex * RENDERING_MAX_MATERIAL_MEMORY_BADGET,
                .size = material->sizeInBytes
            };
            storagebuffer::Upload(uploadInfo);
        }

        uint16_t GetBufferIndex(const Ref<Material>& material) { return material->bufferIndex; }
    }
}
