#ifndef APPLICATION_H
#define APPLICATION_H

#include <hyperflow.h>

namespace app
{
	struct AppObjects
	{
		hf::DirectionalLight mainLight{};
		hf::SkyboxInfo skybox{};
		hf::Transform vikingRoomTransform{};
		hf::Transform sphereTransform{};
		hf::Transform groundTransform{};
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
