// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_TERRAIN_RENDERER_SYSTEM_H
#define MCC_CLIENT_MODULE_TERRAIN_RENDERER_SYSTEM_H

#include "Client/Module/TerrainRenderer/Component.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Terrain/Component.h"
#include "Common/World/Position.h"

#include <flecs.h>

namespace Mcc
{

    void SetupChunkProgramSystem(flecs::iter& it);
    void LoadTextureSystem      (flecs::iter& it);
    void HandleDirtyChunkSystem (flecs::entity entity);

    void BuildChunkMeshSystem         (flecs::entity entity, const CChunkPtr& ptr, const CChunkPos& pos);
    void SetupChunkRenderingMeshSystem(flecs::entity entity, CChunkMeshGenTask& task);


    void OnPlayerMoveObserver  (const flecs::iter& it, size_t, const CEntityTransform& transform);
    void OnChunkCreatedObserver(flecs::entity entity, const CChunkPos& p);

}

#endif
