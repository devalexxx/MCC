// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/BlockUtils.h"

#include "Common/Module/Terrain/Component.h"

namespace Mcc
{

    std::string_view GetBlockName(const flecs::entity e)
    {
        if (const auto meta = e.try_get<CBlockMeta>(); meta)
        {
            return meta->id;
        }

        return e.name().c_str();
    }

    bool IsSolid(const flecs::entity e)
    {
        if (const auto type = e.try_get<CBlockType>(); type)
        {
            return *type == CBlockType::Solid;
        }

        return false;
    }

}
