#include "hscene.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    Scene::Scene(const AssetLocation* assets, uint32_t assetCount)
    {
        this->assets = List<AssetLocation>(assetCount);
        memcpy(this->assets.data(), assets, sizeof(AssetLocation) * assetCount);

        ir::general::LoadScene_i(this);
    }

    Scene::~Scene()
    {
        ir::general::UnloadScene_i(this);
    }

    void LoadSceneBase(const Ref<Scene>& scene)
    {
        ir::HF.scenes[(uint64_t)scene.get()] = scene;
    }

    void UnloadScene(const Ref<Scene>& scene)
    {
        if(ir::HF.scenes.erase((uint64_t)scene.get())) ir::general::UnloadScene_i(scene.get());
    }

    void UnloadAllScenes()
    {
        for (auto& scene : ir::HF.scenes | std::views::values) UnloadScene(scene);
        ir::HF.scenes.clear();
    }

    namespace ir::general
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
                for (const auto& asset : scene->assets) DestroyAsset(asset.path, asset.type);
                scene->isLoaded = false;
            }
        }
    }
}