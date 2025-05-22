#ifndef APPLICATION_H
#define APPLICATION_H

#include <hyperflow.h>

namespace app
{
	void AppRendererLoad();
	void AppLoadResources();
	void AppStart();
	void AppUpdate();
	void AppQuit();

	void AppPreRender(const hf::Ref<hf::Renderer>& rn);
	void AppRender(const hf::Ref<hf::Renderer>& rn);
}

#endif //APPLICATION_H
