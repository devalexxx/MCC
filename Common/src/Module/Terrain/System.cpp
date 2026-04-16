// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Terrain/System.h"

#include "Common/Module/Terrain/Component.h"

namespace Mcc
{

    void RemoveDirtyTagSystem(const flecs::entity entity)
    {
        entity.remove<TDirty>();
    }

}
