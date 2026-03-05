// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_SYSTEM_H
#define MCC_SERVER_MODULE_TERRAIN_SYSTEM_H

#include <flecs.h>

namespace Mcc
{

    struct CTransform;

    void OnPlayerCreatedObserver(flecs::entity entity, const CTransform& transform);
    void OnPlayerMoveObserver   (flecs::iter& it, size_t row);

}

#endif