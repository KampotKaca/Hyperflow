#include "hscene.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    Scene::Scene(const AssetLocation* assets, uint32_t assetCount)
    {
        this->assets = std::vector<AssetLocation>(assetCount);
        memcpy(this->assets.data(), assets, sizeof(AssetLocation) * assetCount);

        inter::rendering::LoadScene_i(this);
    }

    Scene::~Scene()
    {
        inter::rendering::UnloadScene_i(this);
    }

    void LoadSceneBase(const Ref<Scene>& scene)
    {
        inter::HF.scenes[(uint64_t)scene.get()] = scene;
    }

    void UnloadScene(const Ref<Scene>& scene)
    {
        if(inter::HF.scenes.erase((uint64_t)scene.get())) inter::rendering::UnloadScene_i(scene.get());
    }

    void UnloadAllScenes()
    {
        for (auto& scene : inter::HF.scenes | std::views::values) UnloadScene(scene);
        inter::HF.scenes.clear();
    }

    namespace inter::rendering
    {
        void LoadScene_i(Scene* scene)
        {
            if (!scene->isLoaded)
            {
                for (const auto& asset : scene->assets) CreateAsset(asset.path, asset.type);
                SubmitAllBuffers();
                SubmitAllTextures();
                scene->isLoaded = true;
            }
        }

        void UnloadScene_i(Scene* scene)
        {
            if (scene->isLoaded)
            {
                for (const auto& asset : scene->assets) DestroyAsset(asset.path);
                scene->isLoaded = false;
            }
        }
    }
}