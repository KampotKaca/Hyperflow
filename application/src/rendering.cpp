#include "application.h"
#include "debug.h"
#include "hrenderer.h"
#include "resources.h"
#include "voxelterrain/voxelterrain.h"

namespace app
{
    void AppRendererInit()
    {
        DebugRendererInit();
    }

    void AppRendererShutdown()
    {
        DebugRendererShutdown();
    }

    void AppPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPreRender(rn);

        // if (hf::IsKeyDown(hf::Key::P) || hf::IsKeyDown(hf::Key::O))
        // {
        //     hf::UploadStart_TexturePack(rn, APP_TEXTURE_PACKS.viking_room_pack);
        //     {
        //         if (hf::IsKeyDown(hf::Key::P)) hf::UploadAdd_TexturePackBinding(rn, 0, APP_TEXTURES.viking_room);
        //         if (hf::IsKeyDown(hf::Key::O)) hf::UploadAdd_TexturePackBinding(rn, 0, APP_TEXTURES.greek_head);
        //     }
        //     hf::UploadEnd_TexturePack(rn);
        // }

        if (hf::IsKeyDown(hf::Key::G))
        {
            if (hf::dp::SkyboxIsDefaultCubemapBound()) hf::dp::SkyboxBindCubemap(rn, APP_CUBEMAPS.cosmos);
            else hf::dp::SkyboxBindDefaultCubemap(rn);
        }

        VoxelTerrainPreDraw(rn);
    }

    void AppStartRender(const hf::Ref<hf::Renderer>& rn)
    {
        hf::dp::SetCamera(rn, APP_DEBUG.camera.camera3D);
        hf::dp::AddLight(rn, APP_OBJECTS.mainLight);

        DebugPrepass(rn);
        hf::dp::StartRenderTex(rn, APP_RENDER_TEXTURES.mainDrawRenderTexture);
    }

    void AppEndRender(const hf::Ref<hf::Renderer>& rn)
    {
        VoxelTerrainDraw(rn);
        hf::dp::Draw(rn, APP_OBJECTS.skybox);
        if (APP_DEBUG.drawGridLines) hf::dp::Draw(rn, APP_DEBUG.gridLinesInfo);
        DebugRender(rn);

        hf::dp::EndRenderTex(rn);
    }
}