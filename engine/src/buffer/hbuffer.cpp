#include "hbuffer.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    RuntimeBufferBase::~RuntimeBufferBase()
    {
        if (buffer &&
            (transferType == DataTransferType::CopyData ||
            transferType == DataTransferType::TransferOwnership))
            utils::Deallocate(buffer);
        inter::rendering::DestroyBuffer_i(this);
    }

    bool IsLoaded(const Ref<RuntimeBufferBase>& rbb) { return rbb->handle; }
    void Destroy(const Ref<RuntimeBufferBase>& rbb)
    {
        if (inter::rendering::DestroyBuffer_i(rbb.get()))
            inter::HF.graphicsResources.buffers.erase((uint64_t)rbb.get());
    }

    void SubmitAllBuffers() { inter::HF.renderingApi.api.SubmitBufferCopyOperations(); }
    void Destroy(const Ref<RuntimeBufferBase>* pBuffers, uint32_t count)
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
        bool DestroyBuffer_i(RuntimeBufferBase* buffer)
        {
            if (buffer->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                ResourcesMarkedForDeletion::TypedBuffer tb{};
                tb.buffer = buffer->handle;
                tb.type = buffer->GetType();

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