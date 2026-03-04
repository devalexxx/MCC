// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Entity/Module.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Network/Component.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    EntityModule::EntityModule(flecs::world& world) : BaseModule(world)
    {
        world.prefab<EntityPrefab>().add<EntityTag>().set_auto_override<Transform>({
            { 0.f, 0.f, 0.f },
            {},
            { 1.f, 1.f, 1.f }
        });

        world.prefab<NetworkEntityPrefab>()
            .is_a<NetworkObjectPrefab>()
            .is_a<EntityPrefab>()
            .add<NetworkEntityTag>()
            .set_auto_override<Extra>({});

        world.prefab<UserEntityPrefab>()
            .is_a<NetworkEntityPrefab>()
            .add<UserEntityTag>()
            .set_auto_override<UserInputQueue>({});
    }

    void EntityModule::RegisterComponent(flecs::world& world)
    {
        world.component<EntityTag>();
        world.component<NetworkEntityTag>();
        world.component<UserEntityTag>();

        world.component<EntityPrefab>();
        world.component<NetworkEntityPrefab>();
        world.component<UserEntityPrefab>();

        world.component<Transform>();
        world.component<Extra>();
        world.component<UserInputQueue>();
    }

    void EntityModule::RegisterSystem(flecs::world& /* world */)
    {}

    void EntityModule::RegisterHandler(flecs::world& /* world */)
    {}

}