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
        APP_UNIFORMS.globalUniformInfo.time = hf::GetTimeUniformInfo();

        APP_UNIFORMS.globalUniformInfo.light.lightCounts = { 1, 0, 0 };
        APP_UNIFORMS.globalUniformInfo.light.directionalLights[0] = APP_OBJECTS.mainLight.GetUniformInfo();

        UniformUploadAll(rn);

        if (hf::IsKeyDown(hf::Key::P) || hf::IsKeyDown(hf::Key::O))
        {
            hf::UploadStart_TexturePack(rn, APP_TEXTURE_PACKS.viking_room_pack);
            {
                if (hf::IsKeyDown(hf::Key::P)) hf::UploadAdd_TexturePackBinding(rn, 0, APP_TEXTURES.viking_room);
                if (hf::IsKeyDown(hf::Key::O)) hf::UploadAdd_TexturePackBinding(rn, 0, APP_TEXTURES.greek_head);
            }
            hf::UploadEnd_TexturePack(rn);
        }

        if (hf::IsKeyDown(hf::Key::G))
        {
            if (hf::skybox::IsDefaultCubemapBound()) hf::skybox::BindCubemap(rn, APP_CUBEMAPS.cosmos);
            else hf::skybox::BindDefaultCubemap(rn);
        }

        VoxelTerrainPreDraw(rn);
    }

    void AppRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPrepass(rn);
        hf::Start_RenderTexture(rn, APP_RENDER_TEXTURES.mainDrawRenderTexture);
        {
            hf::Start_ShaderLayout(rn, APP_SHADER_LAYOUTS.default_lit); //Viking room setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                hf::Start_Shader(rn, APP_SHADERS.default_lit);
                {
                    hf::Start_Material(rn, hf::primitives::GetEmptyMaterial());
                    {
                        hf::MaterialAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack, 1);

                        hf::Start_Draw(rn);
                        {
                            const DefaultPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoomTransform.ToMat4(),
                                .color = hf::vec4{ hf::utils::ColorFromHash(0xFFFFFF), 1 },
                                .phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 0.8 }
                            };
                            hf::DrawSet_PushConstant(rn, pc);
                            hf::DrawAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::End_Draw(rn);

                        hf::Start_Draw(rn);
                        {
                            hf::DrawAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.white_pack, 1);
                            const DefaultPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.sphereTransform.ToMat4(),
                                .color = hf::vec4{ hf::utils::ColorFromHash(0x9E0505), 1 },
                                .phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 1.0 }
                            };
                            hf::DrawSet_PushConstant(rn, pc);
                            hf::DrawAdd_DrawCall(rn, hf::primitives::GetIcoSphere());
                        }
                        hf::End_Draw(rn);
                    }
                    hf::End_Material(rn);
                }
                hf::End_Shader(rn);
            }
            hf::End_ShaderLayout(rn);

            hf::Start_ShaderLayout(rn, APP_SHADER_LAYOUTS.default_unlit); //Ground setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                hf::Start_Shader(rn, APP_SHADERS.default_unlit);
                {
                    hf::Start_Material(rn, hf::primitives::GetEmptyMaterial());
                    {
                        hf::Start_Draw(rn);
                        {
                            const UnlitColorPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.groundTransform.ToMat4(),
                                .color = hf::vec4{ hf::utils::ColorFromHash(0x19CB1E), 1 }
                            };
                            hf::DrawSet_PushConstant(rn, pc);
                            hf::DrawAdd_DrawCall(rn, hf::primitives::GetPlane());
                        }
                        hf::End_Draw(rn);
                    }
                    hf::End_Material(rn);
                }
                hf::End_Shader(rn);
            }
            hf::End_ShaderLayout(rn);

            VoxelTerrainDraw(rn);
            hf::skybox::Draw(rn, APP_OBJECTS.skybox);
            if (APP_DEBUG.drawGridLines) hf::gridlines::Draw(rn, APP_DEBUG.gridLinesInfo);
            DebugRender(rn);
        }
        hf::End_RenderTexture(rn);
    }
}