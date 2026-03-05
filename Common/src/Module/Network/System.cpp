// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Network/System.h"

#include "Common/WorldContext.h"

namespace Mcc
{

    void GenerateNetworkHandleObserver(flecs::entity, CNetProps& props)
    {
        props.handle = GenerateNetworkHandle();
    }

    void AddToContextObserver(flecs::iter& it)
    {
        auto* ctx = WorldContext<>::Get(it.world());
        while (it.next())
        {
            auto props = it.field<const CNetProps>(0);

            for (const auto i: it)
            {
                auto entity = it.entity(i);
                if (IsValid(props[i].handle))
                {
                    ctx->networkMapping.Set(props[i].handle, entity.id());
                }
            }
        }
    }

    void RemoveFromContextObserver(flecs::iter& it)
    {
        auto* ctx = WorldContext<>::Get(it.world());
        while (it.next())
        {
            auto props = it.field<const CNetProps>(0);

            for (const auto i: it)
            {
                if (IsValid(props[i].handle))
                {
                    ctx->networkMapping.RRemove(props[i].handle);
                }
            }
        }
    }

}