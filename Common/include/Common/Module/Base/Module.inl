// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/Logging.h"

#include "Hexis/Core/TypeName.h"

namespace Mcc
{

    template<typename T, typename... Ds>
    BaseModule<T, Ds...>::BaseModule(flecs::world& world)
    {
        (world.import<Ds>(), ...);
        MCC_LOG_DEBUG("Import {}...", Hx::TypeName<T>());
        world.module<T>();

        static_cast<T*>(this)->RegisterComponent(world);
        static_cast<T*>(this)->RegisterPrefab   (world);
        static_cast<T*>(this)->RegisterSystem   (world);
        static_cast<T*>(this)->RegisterObserver (world);
    }

}
