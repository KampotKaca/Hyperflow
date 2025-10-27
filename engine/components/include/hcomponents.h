#ifndef HCOMPONENTS_H
#define HCOMPONENTS_H

#include "hgenericexception.h"
#include "hcamera3dcore.h"
#include "hcamera3dfreelook.h"
#include "hcamera3danchored.h"
#include "hfreemovecamera3d.h"
#include "htransform.h"
#include "hoctree.h"
#include "hdirectionallight.h"
#include "hspotlight.h"
#include "hpointlight.h"
#include "BS_thread_pool.hpp"

namespace hf
{
    template <BS::opt_t OptFlags = BS::tp::none>
    using ThreadPool = BS::thread_pool<OptFlags>;
}

#endif //HCOMPONENTS_H
