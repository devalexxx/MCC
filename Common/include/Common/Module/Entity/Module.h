// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_ENTITY_MODULE_H
#define MCC_COMMON_MODULE_ENTITY_MODULE_H

#include "Common/Export.h"
#include "Common/Module/Base/Module.h"
#include "Common/Module/Network/Module.h"

#include <flecs.h>

namespace Mcc
{

    struct MCC_LIB_API EntityModule final : BaseModule<EntityModule, NetworkModule>
    {
        explicit EntityModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;
    };

}

#endif
