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

    void AppPreRender()
    {
        DebugPreRender();

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
            if (hf::dp::SkyboxIsDefaultCubemapBound()) hf::dp::SkyboxBindCubemap(APP_CUBEMAPS.cosmos);
            else hf::dp::SkyboxBindDefaultCubemap();
        }

        VoxelTerrainPreDraw();
    }

    void AppStartRender()
    {
        hf::dp::SetCamera(APP_DEBUG.camera.camera3D);
        hf::dp::AddLight(APP_OBJECTS.mainLight);
        hf::dp::AddLight(APP_OBJECTS.central_light);

        DebugPrepass();
        hf::dp::StartRenderTex(APP_RENDER_TEXTURES.mainDrawRenderTexture);
    }

    void AppEndRender()
    {
        VoxelTerrainDraw();
        hf::dp::Draw(APP_OBJECTS.skybox);
        if (APP_DEBUG.drawGridLines) hf::dp::Draw(APP_DEBUG.gridLinesInfo);
        DebugRender();

        hf::dp::EndRenderTex();
    }
}