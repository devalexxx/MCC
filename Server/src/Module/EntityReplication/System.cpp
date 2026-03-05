// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/EntityReplication/System.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/ServerNetworkManager.h"
#include "Server/WorldContext.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Network/Component.h"
#include "Common/Network/Packet.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    void BroadcastEntitiesCreated(flecs::iter& it)
    {
        const auto* ctx = ServerWorldContext::Get(it.world());

        while (it.next())
        {
            auto t = it.field<const CTransform>(0);
            auto e = it.field<const CEntityDataMap>(1);
            auto n = it.field<const CNetProps>(2);

            OnEntitiesCreated packet;
            for (const auto i: it)
            {
                const auto entity = it.entity(i);
                const auto handle = n[i].handle;

                if (!IsValid(handle))
                {
                    MCC_LOG_WARN("[BroadcastEntitiesCreated] Entity(#{})'s network id is invalid", entity.id());
                    continue;
                }

                packet.states.push_back({ .handle=handle, .transform=t[i], .extra=e[i] });
                entity.remove<TEntityCreated>();
                MCC_LOG_INFO("[BroadcastEntitiesCreated] Entity({}) has been created and replicated", handle);
            }
            ctx->networkManager.Broadcast(std::move(packet), ENET_PACKET_FLAG_RELIABLE, 0);
        }
    }

    void BroadcastEntitiesUpdated(flecs::iter& it)
    {
        const auto* ctx = ServerWorldContext::Get(it.world());

        while (it.next())
        {
            auto t = it.field<const CTransform>(0);
            auto e = it.field<const CEntityDataMap>(1);
            auto n = it.field<const CNetProps>(2);

            OnEntitiesUpdated packet;
            for (const auto i: it)
            {
                const auto entity = it.entity(i);
                const auto handle = n[i].handle;

                if (!IsValid(handle))
                {
                    MCC_LOG_WARN("[BroadcastEntitiesUpdated] Entity(#{})'s network id is invalid", entity.id());
                    entity.destruct();
                    continue;
                }

                packet.states.push_back({ .handle=handle, .transform=t[i], .extra=e[i] });
                entity.remove<TEntityDirty>();
            }
            ctx->networkManager.Broadcast(std::move(packet), ENET_PACKET_FLAG_RELIABLE, 0);
        }
    }

    void BroadcastEntitiesDestroyed(flecs::iter& it)
    {
        const auto* ctx = ServerWorldContext::Get(it.world());

        while (it.next())
        {
            auto n = it.field<const CNetProps>(0);

            OnEntitiesDestroyed packet;
            for (const auto i: it)
            {
                const auto entity = it.entity(i);
                const auto handle = n[i].handle;

                if (!IsValid(handle))
                {
                    MCC_LOG_WARN("[BroadcastEntitiesDestroyed] Entity(#{})'s network id is invalid", entity.id());
                    entity.destruct();
                    continue;
                }

                packet.handles.push_back(handle);
                entity.destruct();
                MCC_LOG_INFO("[BroadcastEntitiesDestroyed] Entity({}) has been destroyed and replicated", handle);
            }
            ctx->networkManager.Broadcast(std::move(packet), ENET_PACKET_FLAG_RELIABLE, 0);
        }
    }

}