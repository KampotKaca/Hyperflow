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
        dp::UploadStartTexPack(rn, inter::HF.staticResources.skyboxResources.texturePack);
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

            dp::UploadAddTexPackBindings(rn, bindingInfo);
        }
        dp::UploadEndTexPack(rn);
    }

    void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info)
    {
        dp::StartShaderLayout(rn, inter::HF.staticResources.shaderLayouts.skybox);
        {
            primitives::BindGlobalUniformBuffer(rn);

            dp::StartShader(rn, inter::HF.staticResources.shaders.skybox);
            {
                dp::StartMat(rn, nullptr);
                {
                    dp::StartDrawGroup(rn);
                    {
                        dp::DrawGroupAddTexPackBinding(rn, inter::HF.staticResources.skyboxResources.texturePack, 1);
                        dp::DrawGroupAddDrawCall(rn, primitives::GetMesh(PrimitiveMeshType::Cube));
                    }
                    dp::EndDrawGroup(rn);
                }
                dp::EndMat(rn);
            }
            dp::EndShader(rn);
        }
        dp::EndShaderLayout(rn);
    }

    bool IsDefaultCubemapBound() { return inter::HF.staticResources.skyboxResources.boundCubemap == inter::HF.staticResources.skyboxResources.defaultCubemap; }
}