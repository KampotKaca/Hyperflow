#include "haxislines.h"
#include "hexternal.h"

namespace hf
{
    void AxisLines::Draw(const Ref<Renderer>& rn) const
    {
        draw::StartDrawPacket(rn);
        {
            UploadInfo uploadInfo
            {
                .planeNormal = vec4(planeNormal, lineThickness),
                .color = color
            };

            draw::PacketSet_PushConstant(rn, &uploadInfo, sizeof(UploadInfo));

            DrawCallInfo info
            {
                .pVertBuffers = &inter::HF.staticResources.quadBuffer,
                .bufferCount = 1,
                .indexBuffer = nullptr,
                .instanceCount = 1
            };

            draw::PacketAdd_DrawCall(rn, info);
        }
        draw::EndDrawPacket(rn);
    }

    void AxisLines::StartDrawProcess(const Ref<Renderer>& rn)
    {
        draw::StartShaderSetupPacket(rn, inter::HF.staticResources.axisLinesShaderSetup);

        const ShaderBindingInfo shaderInfo
        {
            .shader = inter::HF.staticResources.axisLinesShader,
            .attrib = primitives::GetQuadBufferAttrib(),
            .bindingPoint = RenderBindingType::Graphics
        };
        draw::StartShaderPacket(rn, shaderInfo);
        draw::StartMaterialPacket(rn, inter::HF.staticResources.emptyMaterial);
    }

    void AxisLines::EndDrawProcess(const Ref<Renderer>& rn)
    {
        draw::EndMaterialPacket(rn);
        draw::EndShaderPacket(rn);
        draw::EndShaderSetupPacket(rn);
    }
}