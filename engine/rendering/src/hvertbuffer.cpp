#include "hvertbuffer.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "export/hex_renderer.h"

namespace hf
{
    VertBuffer::VertBuffer(const VertBufferCreationInfo& info)
    {
        if (info.bufferAttrib == 0) throw GENERIC_EXCEPT("[Hyperflow]", "buffer attribute must be set");
        handle = inter::HF.renderingApi.api.CreateVertBuffer(info);
    }

    VertBuffer::~VertBuffer()
    {
        inter::rendering::DestroyVertBuffer_i(this);
    }

    namespace vertbuffer
    {
        BufferAttrib CreateAttrib(const BufferAttribCreateInfo& info)
        {
            uint32_t fullStride = 0;
            for (uint32_t i = 0; i < info.formatCount; i++)
            {
                auto& stride = info.pFormats[i];
                fullStride += stride.size;
            }

            return inter::HF.renderingApi.api.CreateBufferAttrib(info, fullStride);
        }

        Ref<VertBuffer> Create(const VertBufferCreationInfo& info)
        {
            Ref<VertBuffer> buffer = MakeRef<VertBuffer>(info);
            inter::HF.graphicsResources.vertBuffers[buffer.get()] = buffer;
            return buffer;
        }

        void Destroy(const Ref<VertBuffer>& buffer)
        {
            inter::HF.renderingApi.api.WaitForRendering();
            if (inter::rendering::DestroyVertBuffer_i(buffer.get()))
                inter::HF.graphicsResources.vertBuffers.erase(buffer.get());
        }

        void Destroy(const Ref<VertBuffer>* pBuffers, uint32_t count)
        {
            inter::HF.renderingApi.api.WaitForRendering();
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pBuffers[i];
                if (inter::rendering::DestroyVertBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.vertBuffers.erase(buffer.get());
            }
        }

        void DestroyAll()
        {
            inter::HF.renderingApi.api.WaitForRendering();
            auto& vertBuffers = inter::HF.graphicsResources.vertBuffers;
            for (const auto& buffer : std::ranges::views::values(vertBuffers))
                inter::rendering::DestroyVertBuffer_i(buffer.get());
            vertBuffers.clear();
        }

        bool IsRunning(const Ref<VertBuffer>& buffer) { return buffer->handle; }
    }

    namespace inter::rendering
    {
        bool DestroyVertBuffer_i(VertBuffer* buffer)
        {
            if (buffer->handle)
            {
                HF.renderingApi.api.DestroyVertBuffer(buffer->handle);
                buffer->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}