#include "hyperflow.h"
#include "hinternal.h"

namespace hf::skybox
{
    void BindDefaultCubemap()
    {
        BindCubemap(inter::HF.staticResources.defaultSkyboxCubemap);
    }

    void BindCubemap(const Ref<Cubemap>& cubemap)
    {
        TexturePackBindingUploadInfo<Cubemap> bindingInfo
        {
            .bindingIndex = 0,
            .texInfo = (TexturePackTextureUploadInfo<Cubemap>)
            {
                .pTextures = &cubemap,
                .count = 1,
                .offset = 0
            }
        };
        inter::HF.staticResources.skyboxTexturePack->SetBinding(bindingInfo);
        inter::HF.staticResources.boundCubemap = cubemap;
    }

    void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info)
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
                        draw::PacketAdd_TexturePackBinding(rn, inter::HF.staticResources.skyboxTexturePack, 1);
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

    bool IsDefaultCubemapBound() { return inter::HF.staticResources.boundCubemap == inter::HF.staticResources.defaultSkyboxCubemap; }
}