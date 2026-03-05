// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_IMGUI_MODULE_H
#define MCC_CLIENT_MODULE_IMGUI_MODULE_H

#include "Client/Module/Renderer/Module.h"

#include "Common/Module/Base/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct ImGuiModule final : BaseModule<ImGuiModule, RendererModule>
    {
        ImGuiModule(flecs::world& world);
        ~ImGuiModule() override;

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;
    };

}

#endif
