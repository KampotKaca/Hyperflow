#include "haxislines.h"
#include "hexternal.h"

namespace hf
{
    void AxisLines::GetNecessaryUniforms(UniformBuffer* location)
    {
        location[0] = inter::HF.staticResources.axisLineUniform;
    }

    void AxisLines::Draw(const Ref<Renderer>& rn) const
    {
        static inter::AxisLineUniform uploadUniform{};
        uploadUniform.planeNormal = vec4(planeNormal, lineThickness);
        uploadUniform.color = color;

        UniformBufferUpload upload
        {
            .buffer = inter::HF.staticResources.axisLineUniform,
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(inter::AxisLineUniform),
            .data = &uploadUniform
        };

        UniformBufferUploadInfo uploadInfo
        {
            .pUploads = &upload,
            .uploadCount = 1
        };

        uniformbuffer::Upload(rn, uploadInfo);

        StartDrawPacket(rn);
        {
            UniformBufferBindInfo bindInfo
            {
                .bindingType = RenderBindingType::Graphics,
                .setBindingIndex = 1,
                .pUniforms = &inter::HF.staticResources.axisLineUniform,
                .uniformCount = 1
            };

            PacketAdd_UniformBinding(rn, bindInfo);

            DrawCallInfo info
            {
                .pVertBuffers = &inter::HF.staticResources.quadBuffer,
                .bufferCount = 1,
                .indexBuffer = nullptr,
                .instanceCount = 1
            };

            PacketAdd_DrawCall(rn, info);
        }
        EndDrawPacket(rn);
    }
}