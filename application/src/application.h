#ifndef APPLICATION_H
#define APPLICATION_H

#include "hyperflow.h"

namespace app
{
	class Application
	{
	public:

	private:
		static void Start();
		static void Update();
		static void Quit();

		static void OnMainWindowRender(const hf::Ref<hf::Renderer>& rn);
	};
}

#endif //APPLICATION_H
