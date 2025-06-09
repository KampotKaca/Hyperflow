#ifndef UNIFORMS_H
#define UNIFORMS_H

#include <hyperflow.h>

namespace app
{
    struct AppUniforms
    {
        hf::UniformBuffer timeUniform = 0;

        std::array<hf::UniformBuffer, 1> uniforms{};
        hf::UniformAllocator allocator = 0;
    };

    struct TimeUniform
    {
        alignas(8) double deltaTime;
        alignas(8) double timeSinceStartup;
    };

    extern AppUniforms APP_UNIFORMS;

    void UniformDefineAll();
    void UniformStartAll();

    void UniformUploadTime(const hf::Ref<hf::Renderer>& rn);
    void UniformBindTime(const hf::Ref<hf::Renderer>& rn);
}

#endif //UNIFORMS_H
