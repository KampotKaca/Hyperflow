#include "hyperflow.h"
#include "hinternal.h"

namespace hf::dp
{
    void SkyboxBindDefaultCubemap(const Ref<Renderer>& rn)
    {
        SkyboxBindCubemap(rn, ir::HF.staticResources.skyboxResources.defaultCubemap);
    }

    void SkyboxBindCubemap(const Ref<Renderer>& rn, const Ref<Cubemap>& cubemap)
    {
        ir::HF.staticResources.skyboxResources.boundCubemap = cubemap;
        UploadStartTexPack(rn, ir::HF.staticResources.skyboxResources.texturePack);
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

            UploadAddTexPackBindings(rn, bindingInfo);
        }
        UploadEndTexPack(rn);
    }

    void Draw(const Ref<Renderer>& rn, const SkyboxInfo& info)
    {
        StartShaderLayout(rn, ir::HF.staticResources.shaderLayouts.skybox);
        {
            BindGlobalUniformBuffer(rn);

            StartShader(rn, ir::HF.staticResources.shaders.skybox);
            {
                StartMat(rn, nullptr);
                {
                    StartDrawGroup(rn);
                    {
                        DrawGroupAddTexPackBinding(rn, ir::HF.staticResources.skyboxResources.texturePack, 1);
                        DrawGroupAddDrawCall(rn, primitives::GetMesh(PrimitiveMeshType::Cube));
                    }
                    EndDrawGroup(rn);
                }
                EndMat(rn);
            }
            EndShader(rn);
        }
        EndShaderLayout(rn);
    }

    bool SkyboxIsDefaultCubemapBound() { return ir::HF.staticResources.skyboxResources.boundCubemap == ir::HF.staticResources.skyboxResources.defaultCubemap; }
}