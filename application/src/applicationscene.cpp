#include "applicationscene.h"
#include "applicationsceneassets.h"
#include "resources.h"
#include "application.h"

namespace app
{
    ApplicationScene::ApplicationScene() :
    Scene(APPLICATION_SCENE_ASSET_LOCATIONS.data(), APPLICATION_SCENE_ASSET_LOCATIONS.size())
    {
        viking_room_albedo_texture = hf::Cast<hf::Texture>(hf::GetAsset(ASSET_VIKING_ROOM_ALBEDO_TEXTURE));
        greek_head_texture         = hf::Cast<hf::Texture>(hf::GetAsset(ASSET_GREEK_HEAD_ALBEDO_TEXTURE));
        viking_room_mesh           = hf::Cast<hf::Mesh>   (hf::GetAsset(ASSET_VIKING_ROOM_MESH));

        //viking_room_pack
        {
            hf::TexturePackBindingInfo<hf::Texture>::TextureInfo viking_texture_info
            {
                .texture = viking_room_albedo_texture,
                .index = 0
            };

            hf::TexturePackBindingInfo<hf::Texture> viking_room_binding
            {
                .sampler = APP_TEXTURE_SAMPLERS.defaultSampler,
                .textures = &viking_texture_info,
                .arraySize = 1,
                .bindingIndex = 0
            };

            const hf::TexturePackCreationInfo texPackInfo
            {
                .pTextureBindings = &viking_room_binding,
                .textureBindingCount = 1,
                .layout = APP_TEXTURE_LAYOUTS.default_lit,
            };

            viking_room_pack = hf::Create(texPackInfo);
        }
    }

    void ApplicationScene::Update()
    {

    }

    void ApplicationScene::Render(const hf::Ref<hf::Renderer>& rn)
    {
        hf::Start_ShaderLayout(rn, APP_SHADER_LAYOUTS.default_lit); //Viking room setup
        {
            hf::primitives::BindGlobalUniformBuffer(rn);

            hf::Start_Shader(rn, APP_SHADERS.default_lit);
            {
                hf::Start_Material(rn, nullptr);
                {
                    hf::MaterialAdd_TexturePackBinding(rn, viking_room_pack, 1);

                    hf::Start_DrawGroup(rn);
                    {
                        const auto meshVolume = hf::GetSubmeshBoundingVolume(viking_room_mesh, 0);
                        DefaultPushConstant pc{};
                        pc.phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 0.8 };

                        hf::DrawGroupSet_PushConstant(rn, pc);
                        hf::Start_DrawCall(rn, viking_room_mesh, 0);

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
    }
}