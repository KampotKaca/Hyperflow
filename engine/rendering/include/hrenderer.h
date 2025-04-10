#ifndef HRENDERER_H
#define HRENDERER_H

#include "hshared.h"

namespace hf
{
	struct  Renderer
	{
		Renderer(const Ref<Window>& window, const char* version);
		~Renderer();

		void* handle;
	};
}

#endif //HRENDERER_H
