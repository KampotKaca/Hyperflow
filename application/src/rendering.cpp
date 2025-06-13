#include "application.h"
#include "debug.h"
#include "resources.h"

namespace app
{
    void AppPreRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPreRender(rn);
    }

    void AppRender(const hf::Ref<hf::Renderer>& rn)
    {
        DebugPrepass(rn);
        UniformUploadTime(rn);

        hf::draw::StartRenderPassPacket(rn, APP_RENDER_PASSES.mainPresentPass);
        {
            hf::draw::StartShaderSetupPacket(rn, APP_SHADER_SETUPS.viking_room); //Viking room setup
            {
                hf::Camera3DCore::BindCurrentToUniform(rn);
                UniformBindTime(rn);

                const hf::ShaderBindingInfo vikingRoomShaderInfo
                {
                    .shader = APP_SHADERS.viking_room,
                    .attrib = APP_BUFFER_ATTRIBUTES.pos_col_tex,
                    .bindingPoint = hf::RenderBindingType::Graphics
                };
                hf::draw::StartShaderPacket(rn, vikingRoomShaderInfo);
                {
                    hf::draw::StartMaterialPacket(rn, hf::primitives::GetEmptyMaterial());
                    {
                        hf::draw::MaterialAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack, 2);

                        hf::draw::StartDrawPacket(rn);
                        {
                            const auto trs = APP_OBJECTS.vikingRoomTransform.ToMat4();
                            // hf::draw::PacketAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack);
                            hf::draw::PacketSet_PushConstant(rn, &trs, sizeof(trs));
                            hf::draw::PacketAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::draw::EndDrawPacket(rn);

                        hf::draw::StartDrawPacket(rn);
                        {
                            const auto trs = APP_OBJECTS.vikingRoom2Transform.ToMat4();
                            // hf::draw::PacketAdd_TexturePackBinding(rn, APP_TEXTURE_PACKS.viking_room_pack2);
                            hf::draw::PacketSet_PushConstant(rn, &trs, sizeof(trs));
                            hf::draw::PacketAdd_DrawCall(rn, APP_MESHES.viking_room);
                        }
                        hf::draw::EndDrawPacket(rn);
                    }
                    hf::draw::EndMaterialPacket(rn);
                }
                hf::draw::EndShaderPacket(rn);
            }
            hf::draw::EndShaderSetupPacket(rn);

            // VoxelTerrainDraw(rn);
            DebugRender(rn);
            // APP_OBJECTS.skybox.Draw(rn);
        }
        hf::draw::EndRenderPassPacket(rn);
    }
}