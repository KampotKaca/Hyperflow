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
        rn->UploadStart_TexturePack(inter::HF.staticResources.skyboxTexturePack);
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
            rn->UploadAdd_TexturePackBinding(bindingInfo);
        }
        rn->UploadEnd_TexturePack();
    }

    void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info)
    {
        rn->Start_ShaderSetup(inter::HF.staticResources.skyboxShaderSetup);
        {
            Camera3DCore::BindCurrentToUniform(rn);

            const ShaderBindingInfo shaderInfo
            {
                .shader = inter::HF.staticResources.skyboxShader,
                .attrib = inter::HF.staticResources.cubeAttrib,
                .bindingPoint = RenderBindingType::Graphics
            };
            rn->Start_Shader(shaderInfo);
            {
                rn->Start_Material(inter::HF.staticResources.emptyMaterial);
                {
                    rn->Start_Draw();
                    {
                        rn->DrawAdd_TexturePackBinding(inter::HF.staticResources.skyboxTexturePack, 1);
                        rn->DrawAdd_DrawCall(inter::HF.staticResources.cube);
                    }
                    rn->End_Draw();
                }
                rn->End_Material();
            }
            rn->End_Shader();
        }
        rn->End_ShaderSetup();
    }

    bool IsDefaultCubemapBound() { return inter::HF.staticResources.boundCubemap == inter::HF.staticResources.defaultSkyboxCubemap; }
}