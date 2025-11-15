#ifndef HYPERFLOW_APPLICATIONSCENE_H
#define HYPERFLOW_APPLICATIONSCENE_H

#include "hyperflow.h"

namespace app
{
    constexpr uint32_t VIKING_ROOM_AXIS_SIZE = 10;

    struct VikingRoom
    {
        hf::Transform transform{};
        hf::VolumeTransform cullingVolume{};
    };

    struct Sphere
    {
        hf::Transform transform{};
        hf::VolumeTransform cullingVolume{};
        float smoothness = 1.0f;
    };

    struct Ground
    {
        hf::Transform transform{};
        hf::VolumeTransform cullingVolume{};
        float smoothness = 0.0f;
        hf::vec3 color = hf::vec3(0.1f, 0.1f, 0.1f);
    };

    struct ApplicationScene final : public hf::Scene
    {
        ApplicationScene();
        ~ApplicationScene() override = default;

        void Update() override;
        void Render() override;

        hf::Ref<hf::Texture> viking_room_albedo_texture{};
        hf::Ref<hf::Texture> greek_head_texture{};
        hf::Ref<hf::Model> viking_room_model{};

        hf::Ref<hf::AudioClip> cartoon_comedy{};

        hf::Ref<hf::TexturePack> viking_room_pack{};
        
        hf::Ref<hf::AudioPlayer> background_music{};
        hf::Ref<hf::AudioPlayer3D> background_music3D{};

        VikingRoom vikingRooms[VIKING_ROOM_AXIS_SIZE * VIKING_ROOM_AXIS_SIZE]{};
        Sphere sphere{};
        Ground ground{};
    };
}

#endif //HYPERFLOW_APPLICATIONSCENE_H
