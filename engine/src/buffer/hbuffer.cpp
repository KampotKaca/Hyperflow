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
        ir::rdr::DestroyBuffer_i(this);
    }

    bool IsLoaded(const Ref<RuntimeBufferBase>& rbb) { return rbb->handle; }
    void Destroy(const Ref<RuntimeBufferBase>& rbb)
    {
        if (ir::rdr::DestroyBuffer_i(rbb.get()))
            ir::HF.graphicsResources.buffers.erase((uint64_t)rbb.get());
    }

    void SubmitAllBuffers() { ir::HF.renderingApi.api.SubmitBufferCopyOperations(); }
    void Destroy(const Ref<RuntimeBufferBase>* pBuffers, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto buffer = pBuffers[i];
            if (ir::rdr::DestroyBuffer_i(buffer.get()))
                ir::HF.graphicsResources.buffers.erase((uint64_t)buffer.get());
        }
    }

    namespace ir::rdr
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