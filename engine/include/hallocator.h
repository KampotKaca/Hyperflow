#ifndef HALLOCATOR_H
#define HALLOCATOR_H

#include "hshared.h"

namespace hf
{
    void LoadAllocator();
    void UnloadAllocator();

    void LoadAllocatorThread();
    void UnloadAllocatorThread();

    void LogMemoryStats();
    void LogThreadMemoryStats();
}

#endif //HALLOCATOR_H
