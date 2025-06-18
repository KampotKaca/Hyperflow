#include "application.h"
#include "debug.h"
#include "resources.h"

namespace app
{
    void AppPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPreRender(rn);
        UniformUploadTime(rn);

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
                hf::Camera3DCore::BindCurrentToUniform(rn);
                UniformBindTime(rn);

                const hf::ShaderBindingInfo vikingRoomShaderInfo
                {
                    .shader = APP_SHADERS.viking_room,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };
                rn->Start_Shader(vikingRoomShaderInfo);
                {
                    rn->Start_Material(hf::primitives::GetEmptyMaterial());
                    {
                        rn->MaterialAdd_TexturePackBinding(APP_TEXTURE_PACKS.viking_room_pack, 2);
                        rn->Start_Draw();
                        {
                            const auto trs = APP_OBJECTS.vikingRoomTransform.ToMat4();
                            rn->DrawSet_PushConstant(&trs, sizeof(trs));
                            rn->DrawAdd_DrawCall(APP_MESHES.viking_room);
                        }
                        rn->End_Draw();

                        rn->Start_Draw();
                        {
                            const auto trs = APP_OBJECTS.vikingRoom2Transform.ToMat4();
                            rn->DrawSet_PushConstant(&trs, sizeof(trs));
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