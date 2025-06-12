#include "hskybox.h"
#include "hexternal.h"

namespace hf
{
    void Skybox::Draw(const Ref<Renderer>& rn) const
    {
        draw::StartShaderSetupPacket(rn, inter::HF.staticResources.skyboxShaderSetup);
        {
            Camera3DCore::BindCurrentToUniform(rn);

            const ShaderBindingInfo shaderInfo
            {
                .shader = inter::HF.staticResources.skyboxShader,
                .attrib = inter::HF.staticResources.cubeAttrib,
                .bindingPoint = RenderBindingType::Graphics
            };
            draw::StartShaderPacket(rn, shaderInfo);
            {
                draw::StartMaterialPacket(rn, inter::HF.staticResources.emptyMaterial);
                {
                    draw::StartDrawPacket(rn);
                    {
                        draw::PacketAdd_DrawCall(rn, inter::HF.staticResources.cube);
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