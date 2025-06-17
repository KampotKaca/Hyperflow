#ifndef HBUFFER_H
#define HBUFFER_H

#include "hshared.h"

namespace hf
{
    enum class BufferType { Vertex, Index, Storage };
    struct Buffer
    {
        virtual ~Buffer();

        [[nodiscard]] bool IsRunning() const;
        void Destroy();

        static void SubmitAll();
        static void Destroy(const Ref<Buffer>* pBuffers, uint32_t count);
        [[nodiscard]] virtual BufferType GetType() const = 0;

#ifndef HF_ENGINE_INTERNALS
    private:
#endif
        DataTransferType transferType = DataTransferType::DoNotOwn;
        void* handle{};
        void* buffer{};
    };
}

#endif //HBUFFER_H
