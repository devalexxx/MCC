// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/Module/UserSession/Module.h"

#include "Server/Module/EntityReplication/Component.h"
#include "Server/Module/UserSession/Component.h"
#include "Server/ServerNetworkManager.h"
#include "Server/WorldContext.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Network/Component.h"
#include "Common/SceneImporter.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    UserSession* UserSession::Get(const ENetPeer* peer)
    {
        return static_cast<UserSession*>(peer->data);
    }

    UserSessionModule::UserSessionModule(flecs::world& world) : BaseModule(world)
    {
        const auto* ctx = WorldContext<>::Get(world);
        ctx->networkManager.Subscribe<ConnectEvent>      (OnConnectEventHandler, world);
        ctx->networkManager.Subscribe<DisconnectEvent>   (OnDisconnectEventHandler, world);
        ctx->networkManager.Subscribe<From<OnClientInfo>>(OnClientInfoHandler, world);
    }

    void UserSessionModule::RegisterComponent(flecs::world& world)
    {
        world.component<CUserSession>("CUserSession");
    }

    void UserSessionModule::RegisterPrefab(flecs::world& /* world */) {}

    void UserSessionModule::RegisterSystem(flecs::world& /* world */) {}

    void UserSessionModule::RegisterObserver(flecs::world& /* world */) {}

    void UserSessionModule::OnConnectEventHandler(const ConnectEvent& event, const flecs::world& world)
    {
        const auto* ctx = ServerWorldContext::Get(world);

        auto handle      = GenerateNetworkHandle();
        event.peer->data = new UserSession
        {
            .pInfo={ handle }, .cInfo={}, .peer=event.peer,
            .replicatedChunks={},
        };

        char hostname[100];
        enet_address_get_host_ip(&event.peer->address, hostname, 100);
        MCC_LOG_INFO(
            "Connection opened on port {} with network id {} (from {})", event.peer->address.port, handle, hostname
        );

        ctx->networkManager.Send<OnWaitingInfo>(event.peer, {}, ENET_PACKET_FLAG_RELIABLE, 0);
    }

    void UserSessionModule::OnClientInfoHandler(const From<OnClientInfo>& from, const flecs::world& world)
    {
        const auto* ctx = ServerWorldContext::Get(world);
        if (auto* session = UserSession::Get(from.peer))
        {
            const OnConnectionAccepted packet { .playerInfo=session->pInfo, .serverInfo=ctx->settings };
            session->cInfo = from.packet.info;

            ctx->networkManager.Send(from.peer, packet, ENET_PACKET_FLAG_RELIABLE, 0);

            const auto entity = world.entity()
                .is_a<PUserEntity>()
                .set<CUserSession>(session)
                .set<CNetProps>({ session->pInfo.handle })
                .set<CEntityTransform>({{
                    .position={{ 0, 0, 0 }, { 0, .75f, 0 }},
                    .rotation={},
                    .scale   ={ 1, 1, 1 }
                }})
                .add<TEntityCreated>()
                .child_of<SceneRoot>();

            MCC_LOG_INFO("UserEntity({}, #{}) created", session->pInfo.handle, entity.id());
        }
        else
        {
            ctx->networkManager.Send<OnConnectionRefused>(
                from.peer, { "No user session found" }, ENET_PACKET_FLAG_RELIABLE, 0
            );
        }
    }

    void UserSessionModule::OnDisconnectEventHandler(const DisconnectEvent& event, const flecs::world& world)
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
                MCC_LOG_WARN("Entity({}) isn't associated to a local entity", rHandle);
                return;
            }

            if (!world.is_alive(*lHandle))
            {
                MCC_LOG_WARN("Entity({}) isn't alive", rHandle);
                return;
            }

            world.entity(*lHandle).add<TEntityDestroyed>();
        }
    }

}