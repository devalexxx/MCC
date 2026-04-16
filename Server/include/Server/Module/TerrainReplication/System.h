// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_TERRAIN_SYSTEM_H
#define MCC_SERVER_MODULE_TERRAIN_SYSTEM_H

#include "Server/Module/TerrainGeneration/Component.h"

#include "Common/Module/Entity/Component.h"

#include <flecs.h>

#include "Component.h"

namespace Mcc
{

    void DispatchPendingBlockReplicationSystem(flecs::entity entity, CPendingReplication& pending);
    void DispatchPendingChunkReplicationSystem(flecs::entity entity, CPendingReplication& pending);

    void OnPlayerCreatedObserver (flecs::entity entity, const CEntityTransform& transform);
    void OnPlayerMoveObserver    (flecs::iter& it, size_t row);


}

#endif