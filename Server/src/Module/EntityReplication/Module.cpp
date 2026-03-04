// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/EntityReplication/Module.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/Module/EntityReplication/System.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Module/Network/Component.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    EntityReplicationModule::EntityReplicationModule(const flecs::world& world)
    {
        MCC_ASSERT(
            world.has<EntityModule>(), "EntityReplicationModule require EntityModule, you must import it before."
        );
        MCC_LOG_DEBUG("Import EntityReplicationModule...");
        world.module<EntityReplicationModule>();

        world.component<EntityDirtyTag>();
        world.component<EntityCreatedTag>();
        world.component<EntityDestroyedTag>();

        world.system<const Transform, const Extra, const NetworkProps>("BroadcastEntitiesCreated")
            .with<EntityCreatedTag>()
            .run(BroadcastEntitiesCreated);

        world.system<const Transform, const Extra, const NetworkProps>("BroadcastEntitiesUpdated")
            .with<EntityDirtyTag>()
            .run(BroadcastEntitiesUpdated);

        world.system<const NetworkProps>("BroadcastEntitiesDestroyed")
            .with<EntityDestroyedTag>()
            .run(BroadcastEntitiesDestroyed);
    }

}