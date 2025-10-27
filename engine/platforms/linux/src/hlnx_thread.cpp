#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h>
#include <dlfcn.h>
#include "hinternal.h"

using PthreadCreateType = int (*)(pthread_t*, const pthread_attr_t*, void* (*)(void*), void*);

int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)
{
    static auto real_pthread_create = (PthreadCreateType)dlsym(RTLD_NEXT, "pthread_create");

    struct ThreadWrapper
    {
        void* (*original_start_routine)(void*);
        void* original_arg;
    };

    auto thread_wrapper = [](void* wrapper_arg) -> void*
    {
        hf::ir::alloc::LoadAllocatorThread_i();
        auto wrapper = (ThreadWrapper*)wrapper_arg;
        void* result = wrapper->original_start_routine(wrapper->original_arg);

        delete wrapper;
        hf::ir::alloc::UnloadAllocatorThread_i();
        return result;
    };

    auto wrapper = new ThreadWrapper{start_routine, arg};
    return real_pthread_create(thread, attr, thread_wrapper, wrapper);
}
