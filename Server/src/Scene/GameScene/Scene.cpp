// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Scene/Scene.h"

#include "Server/Scene/GameScene/Scene.h"

#include "Common/Module/Network/Component.h"

namespace Mcc
{

    SceneModule<GameScene>::SceneModule(flecs::world& world) : BaseModule(world)
    {
        world.add<ServerTag>();
    }

    void SceneModule<GameScene>::RegisterComponent(flecs::world& /* world */)
    {

    }

    void SceneModule<GameScene>::RegisterSystem(flecs::world& /* world */)
    {
    }

    void SceneModule<GameScene>::RegisterHandler(flecs::world& /* world */)
    {

    }

}
