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

namespace Mcc
{

    EntityReplicationModule::EntityReplicationModule(flecs::world& world) : BaseModule(world) {}

    void EntityReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<TEntityDirty>();
        world.component<TEntityCreated>();
        world.component<TEntityDestroyed>();
    }

    void EntityReplicationModule::RegisterPrefab(flecs::world& /* world */) {}

    void EntityReplicationModule::RegisterSystem(flecs::world& world)
    {
        world.system<const CEntityTransform, const CEntityDataMap, const CNetProps>("BroadcastEntitiesCreated")
            .kind<Phase::PostUpdate>()
            .with<TEntityCreated>()
            .run(BroadcastEntitiesCreated);

        world.system<const CEntityTransform, const CEntityDataMap, const CNetProps>("BroadcastEntitiesUpdated")
            .kind<Phase::PostUpdate>()
            .with<TEntityDirty>()
            .run(BroadcastEntitiesUpdated);

        world.system<const CNetProps>("BroadcastEntitiesDestroyed")
            .kind<Phase::PostUpdate>()
            .with<TEntityDestroyed>()
            .run(BroadcastEntitiesDestroyed);
    }

    void EntityReplicationModule::RegisterObserver(flecs::world& /* world */) {}

}