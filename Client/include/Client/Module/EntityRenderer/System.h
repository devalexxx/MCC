// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_ENTITY_RENDERER_SYSTEM_H
#define MCC_CLIENT_MODULE_ENTITY_RENDERER_SYSTEM_H

#include "Common/Module/Entity/Component.h"

#include "flecs.h"

namespace Mcc
{
    struct TEntity;

    void SetupEntityMeshSystem      (flecs::iter& it);
    void UpdateRenderTransformSystem(flecs::entity entity, const CEntityTransform& transform);

    void SetEntityMeshObserver(flecs::iter& it);

}

#endif
