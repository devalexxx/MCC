// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Scene/Scene.h"

#include "Client/Scene/MenuScene/Scene.h"
#include "Client/Scene/MenuScene/System.h"

namespace Mcc
{

    SceneModule<MenuScene>::SceneModule(flecs::world& world) : BaseModule(world)
    {
    }

    void SceneModule<MenuScene>::RegisterComponent(flecs::world& /* world */)
    {}

    void SceneModule<MenuScene>::RegisterSystem(flecs::world& world)
    {
        world.system().kind<Phase::OnDrawGui>().run(DisplayMainMenuSystem).add<MenuScene>();
    }

    void SceneModule<MenuScene>::RegisterHandler(flecs::world& /* world */)
    {}

}
