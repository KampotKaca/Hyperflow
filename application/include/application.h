#ifndef APPLICATION_H
#define APPLICATION_H

#include <hyperflow.h>

namespace app
{
    constexpr uint32_t VIKING_ROOM_AXIS_SIZE = 100;

    struct VikingRoom
    {
        hf::Transform transform{};
        hf::VolumeTransform cullingVolume{};
    };

    struct Sphere
    {
        hf::Transform transform{};
        hf::VolumeTransform cullingVolume{};
    };

    struct Ground
    {
        hf::Transform transform{};
        hf::VolumeTransform cullingVolume{};
    };

	struct AppObjects
	{
		hf::DirectionalLight mainLight{};
		hf::SkyboxInfo skybox{};

	    VikingRoom vikingRooms[VIKING_ROOM_AXIS_SIZE * VIKING_ROOM_AXIS_SIZE]{};
	    Sphere sphere{};
	    Ground ground{};
	};
	extern AppObjects APP_OBJECTS;

	void AppRendererLoad();
	void AppLoadResources();
	void AppStart();
	void AppUpdate();
	void AppQuit();

	void AppRendererInit();
	void AppRendererShutdown();

    void AppPreRender(const hf::Ref<hf::Renderer>& rn);
	void AppRender(const hf::Ref<hf::Renderer>& rn);
}

#endif //APPLICATION_H
