// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_ENTITY_REPLICATION_MODULE_H
#define MCC_CLIENT_MODULE_ENTITY_REPLICATION_MODULE_H

#include "Common/Module/Base/Module.h"
#include "Common/Module/Entity/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct OnEntitiesCreated;
    struct OnEntitiesDestroyed;
    struct OnEntitiesUpdated;

    struct EntityReplicationModule final : BaseModule<EntityReplicationModule, EntityModule>
    {
        explicit EntityReplicationModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;

      private:
        static void OnEntitiesCreatedHandler  (const OnEntitiesCreated&   event, const flecs::world& world);
        static void OnEntitiesUpdatedHandler  (const OnEntitiesUpdated&   event, const flecs::world& world);
        static void OnEntitiesDestroyedHandler(const OnEntitiesDestroyed& event, const flecs::world& world);
    };

}

#endif
