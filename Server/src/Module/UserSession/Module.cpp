// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/UserSession/Module.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/Module/UserSession/Component.h"
#include "Server/ServerNetworkManager.h"
#include "Server/WorldContext.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Benchmark.h"
#include "Common/Utils/Logging.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    UserSession* UserSession::Get(const ENetPeer* peer)
    {
        return static_cast<UserSession*>(peer->data);
    }

    UserSessionModule::UserSessionModule(flecs::world& world)
    {
        MCC_ASSERT(world.has<EntityModule>(), "UserSessionModule require EntityModule, you must import it before.");
        MCC_LOG_DEBUG("Import PlayerSessionModule...");
        world.module<UserSessionModule>();

        world.component<UserSessionHolder>();

        const auto* ctx = WorldContext<>::Get(world);

        ctx->networkManager.Subscribe<ConnectEvent>([&](const auto& event) { OnConnectEventHandler(world, event); });
        ctx->networkManager.Subscribe<DisconnectEvent>([&](const auto& event) {
            OnDisconnectEventHandler(world, event);
        });

        ctx->networkManager.Subscribe<From<OnClientInfo>>([&](const auto& from) { OnClientInfoHandler(world, from); });
    }

    void UserSessionModule::OnConnectEventHandler(const flecs::world& world, const ConnectEvent& event)
    {
        const auto* ctx = ServerWorldContext::Get(world);

        auto handle      = GenerateNetworkHandle();
        event.peer->data = new UserSession { { handle }, {}, event.peer, {}, {}, {} };

        char hostname[100];
        enet_address_get_host_ip(&event.peer->address, hostname, 100);
        MCC_LOG_INFO(
            "Connection opened on port {} with network id {} (from {})", event.peer->address.port, handle, hostname
        );

        ctx->networkManager.Send<OnWaitingInfo>(event.peer, {}, ENET_PACKET_FLAG_RELIABLE, 0);
    }

    void UserSessionModule::OnClientInfoHandler(const flecs::world& world, const From<OnClientInfo>& from)
    {
        const auto* ctx = ServerWorldContext::Get(world);
        if (auto* session = UserSession::Get(from.peer))
        {
            const OnConnectionAccepted packet { session->pInfo, ctx->settings };
            session->cInfo = from.packet.info;

            ctx->networkManager.Send(from.peer, packet, ENET_PACKET_FLAG_RELIABLE, 0);

            world.entity()
                .is_a<UserEntityPrefab>()
                .set<UserSessionHolder>({
                    session
            })
                .set<NetworkProps>({ session->pInfo.handle })
                .set<Transform>({ { 0, 100, 0 }, {}, { 1, 1, 1 } })
                .add<EntityCreatedTag>();
        }
        else
        {
            ctx->networkManager.Send<OnConnectionRefused>(
                from.peer, { "No user session found" }, ENET_PACKET_FLAG_RELIABLE, 0
            );
        }
    }

    void UserSessionModule::OnDisconnectEventHandler(const flecs::world& world, const DisconnectEvent& event)
    {
        const auto* ctx = WorldContext<>::Get(world);
        if (const auto* session = UserSession::Get(event.peer))
        {
            const auto rHandle = session->pInfo.handle;
            const auto lHandle = ctx->networkMapping.GetLHandle(rHandle);

            MCC_LOG_INFO("Connection closed on port {} with network id {}", event.peer->address.port, rHandle);
            delete session;

            if (!lHandle.has_value())
            {
                MCC_LOG_WARN("The network id {} isn't associated to a local entity", rHandle);
                return;
            }

            if (!world.is_alive(*lHandle))
            {
                MCC_LOG_WARN("The local entity associated to the network id {} isn't alive", rHandle);
                return;
            }

            world.entity(*lHandle).add<EntityDestroyedTag>();
        }
    }

}