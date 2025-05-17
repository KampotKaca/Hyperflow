#ifndef APPLICATION_H
#define APPLICATION_H

#include "hyperflow.h"

namespace app
{
	class Application
	{
	public:

	private:
		static hf::RenderPass OnPassCreationCallback(const hf::Ref<hf::Renderer>& rn);
		static void OnRendererLoad();
		static void LoadResources();
		static void Start();
		static void Update();
		static void Quit();

		static void OnPreRender(const hf::Ref<hf::Renderer>& rn);
		static void OnRender(const hf::Ref<hf::Renderer>& rn);
	};
}

#endif //APPLICATION_H
