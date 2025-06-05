#include "resources/uniforms.h"

namespace app
{
    AppUniforms APP_UNIFORMS{};

    struct UniformStorage_T
    {
        hf::UniformBufferBindingInfo uniformBindingInfos[8]{};
        hf::UniformBuffer uniformBuffers[8]{};
        hf::UniformBufferUpload uploads[8]{};
    };

    struct CameraTimeUniformUpload_T
    {
        CameraUniform camera{};
        TimeUniform time{};
    };

    static UniformStorage_T TEMP_ST{};
    static CameraTimeUniformUpload_T TEMP_CAMERA_TIME_UPLOAD{};

    void UniformDefineAll()
    {
        //cameraTimeUniform
        {
            hf::UniformBufferBindingInfo cameraBindingInfo
            {
                .usageFlags = hf::ShaderUsageStage::Default,
                .arraySize = 1,
                .elementSizeInBytes = sizeof(CameraUniform)
            };

            hf::UniformBufferBindingInfo timeBindingInfo
            {
                .usageFlags = hf::ShaderUsageStage::Default,
                .arraySize = 1,
                .elementSizeInBytes = sizeof(TimeUniform)
            };

            TEMP_ST.uniformBindingInfos[0] = cameraBindingInfo;
            TEMP_ST.uniformBindingInfos[1] = timeBindingInfo;

            hf::UniformBufferDefinitionInfo cameraBufferDefinitionInfo
            {
                .bindingId = 0,
                .pBindings = TEMP_ST.uniformBindingInfos,
                .bindingCount = 2
            };

            auto cameraTimeUniform = hf::uniformbuffer::Define(cameraBufferDefinitionInfo);

            TEMP_ST.uniformBuffers[0] = cameraTimeUniform;
            APP_UNIFORMS.cameraTimeBuffer = cameraTimeUniform;
        }

        //allocator
        {
            hf::UniformAllocatorDefinitionInfo uniformAllocatorDefinitionInfo
            {
                .pBuffers = TEMP_ST.uniformBuffers,
                .bufferCount = 1,
            };

            auto allocator = hf::uniformallocator::Define(uniformAllocatorDefinitionInfo);
            APP_UNIFORMS.allocator = allocator;
        }
    }

    void UniformStartAll()
    {

    }

    void UniformUploadCameraTime(const hf::Ref<hf::Renderer>& rn, const hf::Camera3DCore& cameraCore,
        hf::vec3 lookDirection, hf::vec3 position,
        const hf::mat4& view, const hf::mat4& model)
    {
        auto& utime = TEMP_CAMERA_TIME_UPLOAD.time;
        utime.deltaTime = hf::time::GetDeltaTime();
        utime.timeSinceStartup = hf::time::GetTimePassed();

        auto& uCam = TEMP_CAMERA_TIME_UPLOAD.camera;
        uCam.model = model;
        uCam.position = position;
        uCam.lookDirection = lookDirection;
        uCam.view = view;
        uCam.invView = glm::inverse(view);
        uCam.proj = cameraCore.ToProjectionMat4(rn);
        uCam.invProj = glm::inverse(uCam.proj);
        uCam.viewProj = uCam.proj * uCam.view;

        hf::UniformBufferUpload cameraTimeUpload
        {
            .buffer = APP_UNIFORMS.cameraTimeBuffer,
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(CameraTimeUniformUpload_T),
            .data = &TEMP_CAMERA_TIME_UPLOAD
        };

        TEMP_ST.uploads[0] = cameraTimeUpload;

        hf::UniformBufferUploadInfo uniformBufferUploads
        {
            .pUploads = TEMP_ST.uploads,
            .uploadCount = 1
        };

        hf::uniformbuffer::Upload(rn, uniformBufferUploads);
    }

    void UniformBindCameraTime(const hf::Ref<hf::Renderer>& rn)
    {
        hf::UniformBufferBindInfo info
        {
            .bindingType = hf::RenderBindingType::Graphics,
            .setBindingIndex = 0,
            .pUniforms = TEMP_ST.uniformBuffers,
            .uniformCount = 1
        };

        hf::ShaderSetupAdd_UniformBinding(rn, info);
    }
}