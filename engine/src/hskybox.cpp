#include "hyperflow.h"
#include "hinternal.h"

namespace hf::skybox
{
    void BindDefaultCubemap(const Ref<Renderer>& rn)
    {
        BindCubemap(rn, inter::HF.staticResources.defaultSkyboxCubemap);
    }

    void BindCubemap(const Ref<Renderer>& rn, const Ref<Cubemap>& cubemap)
    {
        inter::HF.staticResources.boundCubemap = cubemap;
        UploadStart_TexturePack(rn, inter::HF.staticResources.skyboxTexturePack);
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
            UploadAdd_TexturePackBinding(rn, bindingInfo);
        }
        UploadEnd_TexturePack(rn);
    }

    void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info)
    {
        Start_ShaderSetup(rn, inter::HF.staticResources.skyboxShaderSetup);
        {
            primitives::BindGlobalUniformBuffer(rn);

            const ShaderBindingInfo shaderInfo
            {
                .shader = inter::HF.staticResources.skyboxShader,
                .attrib = inter::HF.staticResources.defaultAttrib,
                .bindingPoint = RenderBindingType::Graphics
            };
            Start_Shader(rn, shaderInfo);
            {
                Start_Material(rn, inter::HF.staticResources.emptyMaterial);
                {
                    Start_Draw(rn);
                    {
                        DrawAdd_TexturePackBinding(rn, inter::HF.staticResources.skyboxTexturePack, 1);
                        DrawAdd_DrawCall(rn, inter::HF.staticResources.cube);
                    }
                    End_Draw(rn);
                }
                End_Material(rn);
            }
            End_Shader(rn);
        }
        End_ShaderSetup(rn);
    }

    bool IsDefaultCubemapBound() { return inter::HF.staticResources.boundCubemap == inter::HF.staticResources.defaultSkyboxCubemap; }
}