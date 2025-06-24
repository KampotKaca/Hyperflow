#include "application.h"
#include "debug.h"
#include "resources.h"

namespace app
{
    void AppPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPreRender(rn);
        APP_UNIFORMS.globalUniformInfo.time = hf::time::GetUniformInfo();
        UniformUploadAll(rn);

        if (hf::input::IsDown(hf::Key::G))
        {
            if (hf::skybox::IsDefaultCubemapBound()) hf::skybox::BindCubemap(rn, APP_CUBEMAPS.cosmos);
            else hf::skybox::BindDefaultCubemap(rn);
        }
    }

    void AppRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPrepass(rn);

        rn->Start_RenderPass(APP_RENDER_PASSES.mainPresentPass);
        {
            rn->Start_ShaderSetup(APP_SHADER_SETUPS.viking_room); //Viking room setup
            {
                hf::primitives::BindGlobalUniformBuffer(rn);

                const hf::ShaderBindingInfo vikingRoomShaderInfo
                {
                    .shader = APP_SHADERS.viking_room,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };
                rn->Start_Shader(vikingRoomShaderInfo);
                {
                    rn->Start_Material(APP_MATERIALS.viking_room);
                    {
                        rn->MaterialAdd_TexturePackBinding(APP_TEXTURE_PACKS.viking_room_pack, 2);
                        rn->Start_Draw();
                        {
                            const VikingRoomPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoomTransform.ToMat4(),
                                .materialIndex = APP_MATERIALS.viking_room->GetBufferIndex()
                            };
                            rn->DrawSet_PushConstant(&pc, sizeof(VikingRoomPushConstant));
                            rn->DrawAdd_DrawCall(APP_MESHES.viking_room);
                        }
                        rn->End_Draw();

                        rn->Start_Draw();
                        {
                            const VikingRoomPushConstant pc
                            {
                                .modelMatrix = APP_OBJECTS.vikingRoomTransform.ToMat4(),
                                .materialIndex = APP_MATERIALS.viking_room->GetBufferIndex()
                            };
                            rn->DrawSet_PushConstant(&pc, sizeof(VikingRoomPushConstant));
                            rn->DrawAdd_DrawCall(APP_MESHES.viking_room);
                        }
                        rn->End_Draw();
                    }
                    rn->End_Material();
                }
                rn->End_Shader();
            }
            rn->End_ShaderSetup();

            // VoxelTerrainDraw(rn);
            hf::skybox::Draw(rn, APP_OBJECTS.skybox);
            DebugRender(rn);
        }
        rn->End_RenderPass();
    }
}