// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_ENTITY_REPLICATION_MODULE_H
#define MCC_SERVER_MODULE_ENTITY_REPLICATION_MODULE_H

#include "Common/Module/Base/Module.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Module/Network/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct EntityReplicationModule final : BaseModule<EntityReplicationModule, EntityModule>
    {
        explicit EntityReplicationModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;
    };

}

#endif
