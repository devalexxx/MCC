// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_CORE_MODULE_H
#define MCC_COMMON_MODULE_CORE_MODULE_H

#include "Common/Export.h"
#include "Common/Module/Base/Module.h"

namespace Mcc
{

    struct MCC_LIB_API CoreModule final : BaseModule<CoreModule>
    {
        explicit CoreModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterPrefab   (flecs::world& world) override;
        void RegisterSystem   (flecs::world& world) override;
        void RegisterObserver (flecs::world& world) override;
    };

}

#endif
