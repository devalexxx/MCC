// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_NETWORK_SYSTEM_H
#define MCC_COMMON_MODULE_NETWORK_SYSTEM_H

#include "Common/Module/Network/Component.h"

#include <flecs.h>

namespace Mcc
{

    MCC_LIB_API void GenerateNetworkHandleObserver(flecs::entity, CNetProps& props);
    MCC_LIB_API void AddToContextObserver         (flecs::iter& it);
    MCC_LIB_API void RemoveFromContextObserver    (flecs::iter& it);

}

#endif
