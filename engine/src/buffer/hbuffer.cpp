#define HF_ENGINE_INTERNALS
#include "hbuffer.h"
#include "hyperflow.h"

namespace hf
{
    Buffer::~Buffer()
    {
        if (transferType == DataTransferType::CopyData ||
            transferType == DataTransferType::TransferOwnership)
            utils::Deallocate(buffer);
        inter::rendering::DestroyBuffer_i(this);
    }

    bool Buffer::IsRunning() const { return handle; }
    void Buffer::Destroy()
    {
        if (inter::rendering::DestroyBuffer_i(this))
            inter::HF.graphicsResources.buffers.erase((uint64_t)this);
    }

    void Buffer::SubmitAll() { inter::HF.renderingApi.api.SubmitBufferCopyOperations(); }
    void Buffer::Destroy(const Ref<Buffer>* pBuffers, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto buffer = pBuffers[i];
            if (inter::rendering::DestroyBuffer_i(buffer.get()))
                inter::HF.graphicsResources.buffers.erase((uint64_t)buffer.get());
        }
    }

    namespace inter::rendering
    {
        bool DestroyBuffer_i(Buffer* buffer)
        {
            if (buffer->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                const ResourcesMarkedForDeletion::TypedBuffer tb
                {
                    .buffer = buffer->handle,
                    .type = buffer->GetType(),
                };
                HF.deletedResources.buffers.push_back(tb);
                buffer->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllBuffers_i(bool internalOnly)
        {
            for (const auto& buffer : std::ranges::views::values(HF.graphicsResources.buffers))
                DestroyBuffer_i(buffer.get());
            if (!internalOnly) HF.graphicsResources.buffers.clear();
        }
    }
}