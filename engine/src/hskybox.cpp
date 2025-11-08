#include "hyperflow.h"
#include "hinternal.h"

namespace hf::dp
{
    void SkyboxBindDefaultCubemap()
    {
        SkyboxBindCubemap(ir::HF.staticResources.skyboxResources.defaultCubemap);
    }

    void SkyboxBindCubemap(const Ref<Cubemap>& cubemap)
    {
        ir::HF.staticResources.skyboxResources.boundCubemap = cubemap;
        UploadStartTexPack(ir::HF.staticResources.skyboxResources.texturePack);
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

            UploadAddTexPackBindings(bindingInfo);
        }
        UploadEndTexPack();
    }

    void Draw(const SkyboxInfo& info)
    {
        StartShaderLayout(ir::HF.staticResources.shaderLayouts.skybox);
        {
            BindGlobalUniformBuffer();

            StartShader(ir::HF.staticResources.shaders.skybox);
            {
                StartMat(nullptr);
                {
                    StartDrawGroup();
                    {
                        DrawGroupAddTexPackBinding(ir::HF.staticResources.skyboxResources.texturePack, 1);
                        DrawGroupAddDrawCall(primitives::GetMesh(PrimitiveMeshType::Cube));
                    }
                    EndDrawGroup();
                }
                EndMat();
            }
            EndShader();
        }
        EndShaderLayout();
    }

    bool SkyboxIsDefaultCubemapBound() { return ir::HF.staticResources.skyboxResources.boundCubemap == ir::HF.staticResources.skyboxResources.defaultCubemap; }
}