// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_IMGUI_SYSTEM_H
#define MCC_CLIENT_MODULE_IMGUI_SYSTEM_H

#include <flecs.h>

namespace Mcc
{

    void ImGuiBeginFrameSystem (flecs::iter& it);
    void ImGuiRenderFrameSystem(flecs::iter& it);
    void ImGuiDrawFrameSystem  (flecs::iter& it);

}

#endif
