// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_TERRAIN_RENDERER_SYSTEM_H
#define MCC_CLIENT_MODULE_TERRAIN_RENDERER_SYSTEM_H

#include "Client/Module/TerrainRenderer/Component.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Terrain/Component.h"

#include <flecs.h>

namespace Mcc
{

    void OnPlayerMoveObserver  (const flecs::iter& it, size_t, const CTransform& transform);
    void OnChunkCreatedObserver(flecs::entity entity, const CChunkPos& p);
    void OnChunkChangedObserver(flecs::entity entity, const CChunkPtr&);

    void SetupChunkRenderingMeshSystem(flecs::entity entity, CChunkMeshGenTask& task);
    void SetupChunkProgramSystem      (flecs::iter& it);
    void CleanChunkProgramSystem      (flecs::iter& it);
    void RenderChunkMeshSystem        (flecs::iter& it);

    void BuildChunkMeshSystem(flecs::entity entity, const CChunkPtr& ptr, const CChunkPos& pos);

}

#endif
