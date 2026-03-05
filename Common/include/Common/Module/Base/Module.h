// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_BASE_MODULE_H
#define MCC_COMMON_MODULE_BASE_MODULE_H

#include <flecs.h>

namespace Mcc
{

    template<typename, typename...>
    struct BaseModule
    {
        virtual ~BaseModule() = default;
        BaseModule(flecs::world& world);

        virtual void RegisterComponent(flecs::world& world) = 0;
        virtual void RegisterPrefab   (flecs::world& world) = 0;
        virtual void RegisterSystem   (flecs::world& world) = 0;
        virtual void RegisterObserver (flecs::world& world) = 0;
    };

}

#include "Common/Module/Base/Module.inl"

#endif
