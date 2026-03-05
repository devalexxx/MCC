// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_ENTITY_RENDERER_SYSTEM_H
#define MCC_CLIENT_MODULE_ENTITY_RENDERER_SYSTEM_H

#include "flecs.h"

namespace Mcc
{

    void SetupEntityMeshSystem (flecs::iter& it);
    void RenderUserEntitySystem(flecs::iter& it);

}

#endif
