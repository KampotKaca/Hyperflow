#include "application.h"
#include "debug.h"
#include "hrenderer.h"
#include "resources.h"
#include "voxelterrain/voxelterrain.h"

namespace app
{
    void AppPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPreRender(rn);
        APP_UNIFORMS.globalUniformInfo.time = hf::GetTimeUniformInfo();
        UniformUploadAll(rn);

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

        hf::Start_RenderPass(rn, APP_RENDER_PASSES.mainPresentPass);
        {
            hf::Start_ShaderSetup(rn, APP_SHADER_SETUPS.viking_room); //Viking room setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                const hf::ShaderBindingInfo vikingRoomShaderInfo
                {
                    .shader = APP_SHADERS.viking_room,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };
                hf::Start_Shader(rn, vikingRoomShaderInfo);
                {
                    hf::Start_Material(rn, hf::primitives::GetEmptyMaterial());
                    {
                        hf::MaterialAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack, 1);
                        hf::Start_Draw(rn);
                        {
                            const DefaultPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoomTransform.ToMat4(),
                                .color = { hf::utils::ColorFromHash(0xFFFFFF), 1 }
                            };
                            hf::DrawSet_PushConstant(rn, &pc, sizeof(DefaultPushConstant));
                            hf::DrawAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::End_Draw(rn);

                        hf::Start_Draw(rn);
                        {
                            const DefaultPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoom2Transform.ToMat4(),
                                .color = { hf::utils::ColorFromHash(0xFFFFFF), 1 }
                            };
                            hf::DrawSet_PushConstant(rn, &pc, sizeof(DefaultPushConstant));
                            hf::DrawAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::End_Draw(rn);
                    }
                    hf::End_Material(rn);
                }
                hf::End_Shader(rn);
            }
            hf::End_ShaderSetup(rn);

            hf::Start_ShaderSetup(rn, APP_SHADER_SETUPS.color); //Ground setup
            {
                hf::ShaderBindingInfo shaderInfo
                {
                    .shader = APP_SHADERS.color,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };

                hf::Start_Shader(rn, shaderInfo);
                {
                    hf::Start_Material(rn, hf::primitives::GetEmptyMaterial());
                    {
                        hf::Start_Draw(rn);
                        {
                            const DefaultPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.groundTransform.ToMat4(),
                                .color = { hf::utils::ColorFromHash(0x19CB1E), 1 }
                            };
                            hf::DrawSet_PushConstant(rn, &pc, sizeof(DefaultPushConstant));
                            hf::DrawAdd_DrawCall(rn, APP_MESHES.plane);
                        }
                        hf::End_Draw(rn);
                    }
                    hf::End_Material(rn);
                }
                hf::End_Shader(rn);
            }
            hf::End_ShaderSetup(rn);

            VoxelTerrainDraw(rn);
            hf::skybox::Draw(rn, APP_OBJECTS.skybox);
            DebugRender(rn);
        }
        hf::End_RenderPass(rn);
    }
}