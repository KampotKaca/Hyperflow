#ifndef APPLICATION_H
#define APPLICATION_H

#include <hyperflow.h>
#include "applicationscene.h"

namespace app
{
	struct AppObjects
	{
		hf::DirectionalLight mainLight{};
	    hf::PointLight central_light{};
		hf::SkyboxInfo skybox{};
	    hf::Ref<hf::AudioListener> mainListener;
	    
	    hf::Ref<ApplicationScene> mainScene{};
	};
	extern AppObjects APP_OBJECTS;

	void AppRendererLoad();
	void AppLoadResources();
	void AppStart();
	void AppPreUpdate();
	void AppPostUpdate();
	void AppQuit();

	void AppRendererInit();
	void AppRendererShutdown();

    void AppPreRender();
	void AppStartRender();
	void AppEndRender();
}

#endif //APPLICATION_H
