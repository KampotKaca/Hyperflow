#include "applicationscene.h"
#include "applicationsceneassets.h"
#include "resources.h"
#include "application.h"

namespace app
{
    ApplicationScene::ApplicationScene() :
    Scene(APPLICATION_SCENE_ASSET_LOCATIONS.data(), APPLICATION_SCENE_ASSET_LOCATIONS.size())
    {
        viking_room_albedo_texture = hf::Cast<hf::Texture>  (hf::GetAsset(ASSET_VIKING_ROOM_ALBEDO_TEXTURE, hf::AssetType::Texture));
        greek_head_texture         = hf::Cast<hf::Texture>  (hf::GetAsset(ASSET_GREEK_HEAD_ALBEDO_TEXTURE, hf::AssetType::Texture));
        viking_room_model          = hf::Cast<hf::Model>    (hf::GetAsset(ASSET_VIKING_ROOM_MODEL, hf::AssetType::Model));

        cartoon_comedy             = hf::Cast<hf::AudioClip>(hf::GetAsset(ASSET_CARTOON_COMEDY_AUDIO_CLIP, hf::AssetType::AudioClip));

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

        {
            hf::AudioPlayerCreationInfo info{};
            info.clip = cartoon_comedy;
            info.settings.loopingEnabled = true;

            background_music = hf::Create(info);
        }

        {
            hf::AudioPlayer3DCreationInfo info{};
            info.clip = cartoon_comedy;

            background_music3D = hf::Create(info);
        }
    }

    void ApplicationScene::Update()
    {

    }

    void ApplicationScene::Render()
    {
        hf::dp::StartShaderLayout(APP_SHADER_LAYOUTS.default_lit); //Viking room setup
        {
            hf::dp::BindGlobalUniformBuffer();

            hf::dp::StartShader(APP_SHADERS.default_lit);
            {
                hf::dp::StartMat(nullptr);
                {
                    hf::dp::MatAddTexPackBinding(viking_room_pack, 1);

                    hf::dp::StartDrawGroup();
                    {
                        auto mesh = hf::GetMesh(viking_room_model, 0);
                        const auto meshVolume = hf::GetMeshBoundingVolume(mesh);
                        DefaultPushConstant pc{};
                        pc.phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 0.8 };

                        hf::dp::DrawGroupSetPushConstant(pc);
                        hf::dp::StartDrawCall(mesh);

                        for (uint32_t x = 0; x < VIKING_ROOM_AXIS_SIZE; x++)
                            for (uint32_t z = 0; z < VIKING_ROOM_AXIS_SIZE; z++)
                            {
                                auto& vikingRoom = APP_OBJECTS.vikingRooms[x * VIKING_ROOM_AXIS_SIZE + z];
                                vikingRoom.cullingVolume.Update(vikingRoom.transform, meshVolume);
                                hf::dp::DrawAddInstance(DefaultInstanceData
                                {
                                    .modelMatrix = vikingRoom.cullingVolume.matrix,
                                    .color = hf::vec4{ hf::utils::ColorFromHash(0xFFFFFF), 1 },
                                }, vikingRoom.cullingVolume);
                            }

                        hf::dp::EndDrawCall();
                    }
                    hf::dp::EndDrawGroup();

                    hf::dp::StartDrawGroup();
                    {
                        auto mesh = hf::primitives::GetMesh(hf::PrimitiveMeshType::IcoSphere);
                        const auto meshVolume = hf::GetMeshBoundingVolume(mesh);

                        hf::dp::DrawGroupAddTexPackBinding(APP_TEXTURE_PACKS.white_pack, 1);
                        DefaultPushConstant pc{};
                        pc.phongData = hf::vec4{ hf::utils::ColorFromHash(0x9B9B9B), 1.0 };

                        hf::dp::DrawGroupSetPushConstant(pc);
                        hf::dp::StartDrawCall(mesh);

                        {
                            auto& sphere = APP_OBJECTS.sphere;
                            sphere.cullingVolume.Update(sphere.transform, meshVolume);

                            hf::dp::DrawAddInstance(DefaultInstanceData
                            {
                                .modelMatrix = sphere.cullingVolume.matrix,
                                .color = hf::vec4{ hf::utils::ColorFromHash(0x9E0505), 1 },
                            }, sphere.cullingVolume);
                        }

                        hf::dp::EndDrawCall();
                    }
                    hf::dp::EndDrawGroup();
                }
                hf::dp::EndMat();
            }
            hf::dp::EndShader();
        }
        hf::dp::EndShaderLayout();

        hf::dp::StartShaderLayout(APP_SHADER_LAYOUTS.default_unlit); //Ground setup
        {
            hf::dp::BindGlobalUniformBuffer();

            hf::dp::StartShader(APP_SHADERS.default_unlit);
            {
                hf::dp::StartMat(nullptr);
                {
                    hf::dp::StartDrawGroup();
                    {
                        const auto meshVolume = hf::GetMeshBoundingVolume(hf::primitives::GetMesh(hf::PrimitiveMeshType::Plane));
                        hf::dp::StartDrawCall(hf::primitives::GetMesh(hf::PrimitiveMeshType::Plane));

                        {
                            auto& ground = APP_OBJECTS.ground;
                            ground.cullingVolume.Update(ground.transform, meshVolume);

                            hf::dp::DrawAddInstance(DefaultInstanceData
                            {
                                .modelMatrix = ground.cullingVolume.matrix,
                                .color = hf::vec4{ hf::utils::ColorFromHash(0x19CB1E), 1 }
                            }, ground.cullingVolume);
                        }

                        hf::dp::EndDrawCall();
                    }
                    hf::dp::EndDrawGroup();
                }
                hf::dp::EndMat();
            }
            hf::dp::EndShader();
        }
        hf::dp::EndShaderLayout();
    }
}