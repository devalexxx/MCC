// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Network/System.h"

#include "Common/WorldContext.h"

namespace Mcc
{

    void PollNetworkSystem(flecs::iter& it)
    {
        const auto world = it.world();
        const auto ctx   = WorldContext<>::Get(world);

        world.defer_suspend();
        ctx->networkManager.Poll();
        world.defer_resume();

        IgnoreIter(it);
    }

    void GenerateNetworkHandleObserver(flecs::entity, CNetProps& props)
    {
        props.handle = GenerateNetworkHandle();
    }

    void AddToContextObserver(flecs::iter& it)
    {
        auto* ctx = WorldContext<>::Get(it.world());
        while (it.next())
        {
            for (const auto i: it)
            {
                auto props = it.field_at<const CNetProps>(0, i);
                auto entity = it.entity(i);
                // if (IsValid(props[i].handle))
                // {
                //     ctx->networkMapping.Set(props[i].handle, entity.id());
                // }
                if (IsValid(props.handle))
                {
                    ctx->networkMapping.Set(props.handle, entity.id());
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