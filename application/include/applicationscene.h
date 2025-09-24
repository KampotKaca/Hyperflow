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
        void Render(const hf::Ref<hf::Renderer>& rn) override;

        hf::Ref<hf::Texture> viking_room_albedo_texture{};
        hf::Ref<hf::Texture> greek_head_texture{};
        hf::Ref<hf::Mesh> viking_room_mesh{};
        hf::Ref<hf::Mesh> fbx_torus{};

        hf::Ref<hf::TexturePack> viking_room_pack{};
    };
}

#endif //HYPERFLOW_APPLICATIONSCENE_H
