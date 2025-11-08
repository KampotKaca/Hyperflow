#ifndef HYPERFLOW_APPLICATIONSCENE_H
#define HYPERFLOW_APPLICATIONSCENE_H

#include "hyperflow.h"

namespace app
{
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
        
        hf::Ref<hf::AudioPlayer> background_music;
        hf::Ref<hf::AudioPlayer3D> background_music3D;
    };
}

#endif //HYPERFLOW_APPLICATIONSCENE_H
