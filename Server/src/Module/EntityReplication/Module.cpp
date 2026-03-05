// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/EntityReplication/Module.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/Module/EntityReplication/System.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Module/Network/Component.h"
#include "Common/Phase.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    EntityReplicationModule::EntityReplicationModule(flecs::world& world) : BaseModule(world)
    {}

    void EntityReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<EntityDirtyTag>();
        world.component<EntityCreatedTag>();
        world.component<EntityDestroyedTag>();
    }

    void EntityReplicationModule::RegisterSystem(flecs::world& world)
    {
        world.system<const Transform, const Extra, const NetworkProps>("BroadcastEntitiesCreated")
            .kind<Phase::PostUpdate>()
            .with<EntityCreatedTag>()
            .run(BroadcastEntitiesCreated);

        world.system<const Transform, const Extra, const NetworkProps>("BroadcastEntitiesUpdated")
            .kind<Phase::PostUpdate>()
            .with<EntityDirtyTag>()
            .run(BroadcastEntitiesUpdated);

        world.system<const NetworkProps>("BroadcastEntitiesDestroyed")
            .kind<Phase::PostUpdate>()
            .with<EntityDestroyedTag>()
            .run(BroadcastEntitiesDestroyed);
    }

    void EntityReplicationModule::RegisterHandler(flecs::world& /* world */)
    {
    }

}