#include "haxislines.h"
#include "hexternal.h"

namespace hf
{
    void AxisLines::Draw(const Ref<Renderer>& rn) const
    {
        draw::StartShaderSetupPacket(rn, inter::HF.staticResources.axisLinesShaderSetup);
        {
            Camera3DCore::BindCurrentToUniform(rn);

            const ShaderBindingInfo shaderInfo
            {
                .shader = inter::HF.staticResources.axisLinesShader,
                .attrib = inter::HF.staticResources.quadAttrib,
                .bindingPoint = RenderBindingType::Graphics
            };
            draw::StartShaderPacket(rn, shaderInfo);
            {
                draw::StartMaterialPacket(rn, inter::HF.staticResources.emptyMaterial);
                {
                    draw::StartDrawPacket(rn);
                    {
                        draw::PacketSet_PushConstant(rn, this, sizeof(AxisLines));

                        const DrawCallInfo info
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
                draw::EndMaterialPacket(rn);
            }
            draw::EndShaderPacket(rn);
        }
        draw::EndShaderSetupPacket(rn);
    }
}