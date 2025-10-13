#include "hyperflow.h"
#include "hinternal.h"

namespace hf::skybox
{
    void BindDefaultCubemap(const Ref<Renderer>& rn)
    {
        BindCubemap(rn, inter::HF.staticResources.skyboxResources.defaultCubemap);
    }

    void BindCubemap(const Ref<Renderer>& rn, const Ref<Cubemap>& cubemap)
    {
        inter::HF.staticResources.skyboxResources.boundCubemap = cubemap;
        UploadStart_TexturePack(rn, inter::HF.staticResources.skyboxResources.texturePack);
        {
            TexturePackTextureUploadInfo<Cubemap>::TextureInfo tInfo{};
            tInfo.texture = cubemap;
            tInfo.index = 0;

            TexturePackTextureUploadInfo<Cubemap> uInfo{};
            uInfo.pTextures = &tInfo;
            uInfo.count = 1;
            uInfo.offset = 0;

            TexturePackBindingUploadInfo<Cubemap> bindingInfo{};
            bindingInfo.bindingIndex = 0;
            bindingInfo.texInfo = uInfo;

            UploadAdd_TexturePackBindings(rn, bindingInfo);
        }
        UploadEnd_TexturePack(rn);
    }

    void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info)
    {
        Start_ShaderLayout(rn, inter::HF.staticResources.shaderLayouts.skybox);
        {
            primitives::BindGlobalUniformBuffer(rn);

            Start_Shader(rn, inter::HF.staticResources.shaders.skybox);
            {
                Start_Material(rn, nullptr);
                {
                    Start_DrawGroup(rn);
                    {
                        DrawGroupAdd_TexturePackBinding(rn, inter::HF.staticResources.skyboxResources.texturePack, 1);
                        DrawGroupAdd_DrawCall(rn, primitives::GetMesh(PrimitiveMeshType::Cube));
                    }
                    End_DrawGroup(rn);
                }
                End_Material(rn);
            }
            End_Shader(rn);
        }
        End_ShaderLayout(rn);
    }

    bool IsDefaultCubemapBound() { return inter::HF.staticResources.skyboxResources.boundCubemap == inter::HF.staticResources.skyboxResources.defaultCubemap; }
}