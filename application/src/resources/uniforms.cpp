#include "resources/uniforms.h"
#include "../appconfig.h"

namespace app
{
    AppUniforms APP_UNIFORMS{};

    void UniformDefineAll()
    {
        //Camera uniform
        {
            hf::UniformBufferBindingInfo bindingInfo
            {
                .usageFlags = hf::ShaderUsageStage::Default,
                .arraySize = 1,
                .elementSizeInBytes = sizeof(CameraUniform)
            };

            const hf::UniformBufferDefinitionInfo uniform
            {
                .bindingId = 0,
                .pBindings = &bindingInfo,
                .bindingCount = 1
            };

            APP_UNIFORMS.cameraUniform = hf::uniformbuffer::Define(uniform);
        }

        //Time uniform
        {
            hf::UniformBufferBindingInfo bindingInfo
            {
                .usageFlags = hf::ShaderUsageStage::Default,
                .arraySize = 1,
                .elementSizeInBytes = sizeof(TimeUniform)
            };

            hf::UniformBufferDefinitionInfo uniform
            {
                .bindingId = 0,
                .pBindings = &bindingInfo,
                .bindingCount = 1
            };

            APP_UNIFORMS.timeUniform = hf::uniformbuffer::Define(uniform);
        }

        //allocator
        {
            APP_UNIFORMS.uniforms[0] = APP_UNIFORMS.cameraUniform;
            APP_UNIFORMS.uniforms[1] = APP_UNIFORMS.timeUniform;

            const hf::UniformAllocatorDefinitionInfo info
            {
                .pBuffers = APP_UNIFORMS.uniforms.data(),
                .bufferCount = APP_UNIFORMS.uniforms.size(),
            };

            APP_UNIFORMS.allocator = hf::uniformallocator::Define(info);
        }
    }

    void UniformStartAll()
    {

    }

    void UniformUploadCamera(const hf::Ref<hf::Renderer>& rn, const hf::Camera3DCore& cameraCore,
        hf::vec3 lookDirection, hf::vec3 position, const hf::mat4& view)
    {
        static CameraUniform UNIFORM{};
        UNIFORM.position = position;
        UNIFORM.lookDirection = lookDirection;
        UNIFORM.view = view;
        UNIFORM.invView = glm::inverse(view);
        UNIFORM.proj = cameraCore.ToProjectionMat4(rn);
        UNIFORM.invProj = glm::inverse(UNIFORM.proj);
        UNIFORM.viewProj = UNIFORM.proj * UNIFORM.view;

        const hf::UniformBufferUpload uniformUpload
        {
            .buffer = APP_UNIFORMS.cameraUniform,
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(CameraUniform),
            .data = &UNIFORM
        };

        hf::draw::UploadUniformPacket(rn, uniformUpload);
    }

    void UniformUploadTime(const hf::Ref<hf::Renderer>& rn)
    {
        static TimeUniform UNIFORM{};
        UNIFORM.deltaTime = hf::time::GetDeltaTime();
        UNIFORM.timeSinceStartup = hf::time::GetTimePassed();

        const hf::UniformBufferUpload uniformUpload
        {
            .buffer = APP_UNIFORMS.cameraUniform,
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(TimeUniform),
            .data = &UNIFORM
        };

        hf::draw::UploadUniformPacket(rn, uniformUpload);
    }

    void UniformBindCamera(const hf::Ref<hf::Renderer>& rn)
    {
        constexpr hf::UniformBufferBindInfo info
        {
            .bindingType = hf::RenderBindingType::Graphics,
            .setBindingIndex = UNIFORM_CAMERA_SET_INDEX,
            .pUniforms = &APP_UNIFORMS.cameraUniform,
            .uniformCount = 1,
        };

        hf::draw::ShaderSetupAdd_UniformBinding(rn, info);
    }

    void UniformBindTime(const hf::Ref<hf::Renderer>& rn)
    {
        constexpr hf::UniformBufferBindInfo info
        {
            .bindingType = hf::RenderBindingType::Graphics,
            .setBindingIndex = UNIFORM_TIME_SET_INDEX,
            .pUniforms = &APP_UNIFORMS.timeUniform,
            .uniformCount = 1,
        };

        hf::draw::ShaderSetupAdd_UniformBinding(rn, info);
    }
}