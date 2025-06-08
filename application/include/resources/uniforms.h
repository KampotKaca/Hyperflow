#ifndef UNIFORMS_H
#define UNIFORMS_H

#include <hyperflow.h>

namespace app
{
    struct AppUniforms
    {
        hf::UniformBuffer cameraUniform = 0;
        hf::UniformBuffer timeUniform = 0;

        std::array<hf::UniformBuffer, 2> uniforms{};
        hf::UniformAllocator allocator = 0;
    };

    struct CameraUniform
    {
        alignas(16) hf::vec3 lookDirection;
        alignas(16) hf::vec3 position;
        alignas(16) hf::mat4 view{};
        alignas(16) hf::mat4 invView{};
        alignas(16) hf::mat4 proj{};
        alignas(16) hf::mat4 invProj{};
        alignas(16) hf::mat4 viewProj{};
    };

    struct TimeUniform
    {
        alignas(8) double deltaTime;
        alignas(8) double timeSinceStartup;
    };

    extern AppUniforms APP_UNIFORMS;

    void UniformDefineAll();
    void UniformStartAll();
    void UniformUploadCamera(const hf::Ref<hf::Renderer>& rn, const hf::Camera3DCore& cameraCore,
        hf::vec3 lookDirection, hf::vec3 position, const hf::mat4& view);
    void UniformUploadTime(const hf::Ref<hf::Renderer>& rn);

    void UniformBindCamera(const hf::Ref<hf::Renderer>& rn);
    void UniformBindTime(const hf::Ref<hf::Renderer>& rn);
}

#endif //UNIFORMS_H
