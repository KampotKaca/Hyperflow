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

        hf::UniformAllocatorDefinitionInfo uniformAllocatorDefinitionInfo
        {
            .pBuffers = TEMP_ST.uniformBuffers,
            .bufferCount = 1,
        };

        auto allocator = hf::uniformallocator::Define(uniformAllocatorDefinitionInfo);

        APP_UNIFORMS =
        {
            .allocator = allocator,
            .cameraTimeBuffer = cameraTimeUniform
        };
    }

    void UniformStartAll()
    {

    }

    void UniformUploadCameraTime(const hf::Ref<hf::Renderer>& rn, const hf::Camera3D& camera)
    {
        TEMP_CAMERA_TIME_UPLOAD.time.deltaTime = hf::time::GetDeltaTime();
        TEMP_CAMERA_TIME_UPLOAD.time.timeSinceStartup = hf::time::GetTimePassed();

        TEMP_CAMERA_TIME_UPLOAD.camera.model = glm::rotate(hf::mat4(1.0f), (float)hf::time::GetTimePassed() * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        TEMP_CAMERA_TIME_UPLOAD.camera.view = camera.ToViewMat4();
        TEMP_CAMERA_TIME_UPLOAD.camera.proj = camera.ToProjectionMat4(rn);
        TEMP_CAMERA_TIME_UPLOAD.camera.viewProj = TEMP_CAMERA_TIME_UPLOAD.camera.proj * TEMP_CAMERA_TIME_UPLOAD.camera.view;

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
            .bindingType = hf::RenderBindingType::Graphics,
            .setBindingIndex = 0,
            .pUploads = TEMP_ST.uploads,
            .uploadCount = 1
        };

        hf::uniformbuffer::Upload(rn, uniformBufferUploads);
    }
}