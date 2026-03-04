// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_SCENE_MENU_SCENE_SCENE_H
#define MCC_CLIENT_SCENE_MENU_SCENE_SCENE_H

#include "Common/SceneImporter.h"

#include <flecs.h>

namespace Mcc
{

    struct MenuScene;

    template<>
    struct SceneModule<MenuScene> final :
        BaseModule<
            SceneModule<MenuScene>,
            ImGuiModule
        >
    {
        SceneModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;
    };

}

#endif
