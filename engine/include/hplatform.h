#ifndef HPLATFORM_H
#define HPLATFORM_H

#include "hshared.h"

namespace hf
{
	void Platform_HandleEvents(std::vector<hRef<Window>>& windows, EngineUpdateType updateType);
	void Platform_Initialize();
}

#endif //HPLATFORM_H
