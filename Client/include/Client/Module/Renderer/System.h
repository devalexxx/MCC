// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_RENDERER_SYSTEM_H
#define MCC_CLIENT_MODULE_RENDERER_SYSTEM_H

#include <flecs.h>

namespace Mcc
{

    void SetupRendererSystem  (flecs::iter& it);
    void PollWindowEventSystem(flecs::iter& it);
    void ClearFrameSystem     (flecs::iter& it);
    void RenderFrameSystem    (flecs::iter& it);

}

#endif
