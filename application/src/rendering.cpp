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
        hf::Set_Camera(rn, APP_DEBUG.camera.camera3D);
        hf::Add_Light(rn, APP_OBJECTS.mainLight);

        DebugPrepass(rn);
        hf::Start_RenderTexture(rn, APP_RENDER_TEXTURES.mainDrawRenderTexture);
        {
            hf::Start_ShaderLayout(rn, APP_SHADER_LAYOUTS.default_lit); //Viking room setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                hf::Start_Shader(rn, APP_SHADERS.default_lit);
                {
                    hf::Start_Material(rn, nullptr);
                    {
                        hf::MaterialAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack, 1);

                        hf::Start_DrawGroup(rn);
                        {
                            const auto meshVolume = hf::GetSubmeshBoundingVolume(APP_MESHES.viking_room, 0);
                            DefaultPushConstant pc{};
                            pc.phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 0.8 };

                            hf::DrawGroupSet_PushConstant(rn, pc);
                            hf::Start_DrawCall(rn, APP_MESHES.viking_room, 0);

                            for (uint32_t x = 0; x < VIKING_ROOM_AXIS_SIZE; x++)
                                for (uint32_t z = 0; z < VIKING_ROOM_AXIS_SIZE; z++)
                                {
                                    auto& vikingRoom = APP_OBJECTS.vikingRooms[x * VIKING_ROOM_AXIS_SIZE + z];
                                    vikingRoom.cullingVolume.Update(vikingRoom.transform, meshVolume);
                                    hf::DrawAdd_Instance(rn, DefaultInstanceData
                                    {
                                        .modelMatrix = vikingRoom.cullingVolume.matrix,
                                        .color = hf::vec4{ hf::utils::ColorFromHash(0xFFFFFF), 1 },
                                    }, vikingRoom.cullingVolume);
                                }

                            hf::End_DrawCall(rn);
                        }
                        hf::End_DrawGroup(rn);

                        hf::Start_DrawGroup(rn);
                        {
                            const auto meshVolume = hf::GetSubmeshBoundingVolume(hf::primitives::GetMesh(hf::PrimitiveMeshType::IcoSphere), 0);

                            hf::DrawGroupAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.white_pack, 1);
                            DefaultPushConstant pc{};
                            pc.phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 1.0 };

                            hf::DrawGroupSet_PushConstant(rn, pc);
                            hf::Start_DrawCall(rn, hf::primitives::GetMesh(hf::PrimitiveMeshType::IcoSphere), 0);

                            {
                                auto& sphere = APP_OBJECTS.sphere;
                                sphere.cullingVolume.Update(sphere.transform, meshVolume);

                                hf::DrawAdd_Instance(rn, DefaultInstanceData
                                {
                                    .modelMatrix = sphere.cullingVolume.matrix,
                                    .color = hf::vec4{ hf::utils::ColorFromHash(0x9E0505), 1 },
                                }, sphere.cullingVolume);
                            }

                            hf::End_DrawCall(rn);
                        }
                        hf::End_DrawGroup(rn);
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
                    hf::Start_Material(rn, nullptr);
                    {
                        hf::Start_DrawGroup(rn);
                        {
                            const auto meshVolume = hf::GetSubmeshBoundingVolume(hf::primitives::GetMesh(hf::PrimitiveMeshType::Plane), 0);
                            hf::Start_DrawCall(rn, hf::primitives::GetMesh(hf::PrimitiveMeshType::Plane), 0);

                            {
                                auto& ground = APP_OBJECTS.ground;
                                ground.cullingVolume.Update(ground.transform, meshVolume);

                                hf::DrawAdd_Instance(rn, DefaultInstanceData
                                {
                                    .modelMatrix = ground.cullingVolume.matrix,
                                    .color = hf::vec4{ hf::utils::ColorFromHash(0x19CB1E), 1 }
                                }, ground.cullingVolume);
                            }

                            hf::End_DrawCall(rn);
                        }
                        hf::End_DrawGroup(rn);
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