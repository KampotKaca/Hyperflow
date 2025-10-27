#include "hrendertexture.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    RenderTexture::RenderTexture(const RenderTextureCreationInfo& info) : createInfo(info)
    {
        ir::rdr::CreateRenderTexture_i(this);
    }

    RenderTexture::~RenderTexture()
    {
        ir::rdr::DestroyRenderTexture_i(this);
    }

    Ref<RenderTexture> Create(const RenderTextureCreationInfo& info)
    {
        Ref<RenderTexture> texture = MakeRef<RenderTexture>(info);
        ir::HF.graphicsResources.renderTextures[(uint64_t)texture.get()] = texture;
        return texture;
    }

    void Destroy(const Ref<RenderTexture>& tex)
    {
        if (ir::rdr::DestroyRenderTexture_i(tex.get()))
            ir::HF.graphicsResources.renderTextures.erase((uint64_t)tex.get());
    }

    void Destroy(const Ref<RenderTexture>* pTextures, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto tex = pTextures[i];
            if (ir::rdr::DestroyRenderTexture_i(tex.get()))
                ir::HF.graphicsResources.renderTextures.erase((uint64_t)tex.get());
        }
    }

    bool IsLoaded(const Ref<RenderTexture>& tex) { return tex->handle; }

    namespace ir::rdr
    {
        bool CreateRenderTexture_i(RenderTexture* tex)
        {
            if (tex->handle) return false;
            tex->handle = HF.renderingApi.api.CreateRenderTexture(tex->createInfo);
            return true;
        }

        bool DestroyRenderTexture_i(RenderTexture* tex)
        {
            if (tex->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.renderTextures.push_back(tex->handle);
                tex->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}