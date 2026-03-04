// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/ServerSession/Module.h"

#include "Client/Module/ServerSession/Component.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/SceneImporter.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    ServerSessionModule::ServerSessionModule(flecs::world& world) : BaseModule(world)
    {
    }

    void ServerSessionModule::RegisterComponent(flecs::world& world)
    {
        ServerConnectionState::Register(world);
    }

    void ServerSessionModule::RegisterSystem(flecs::world& /* world */)
    {}

    void ServerSessionModule::RegisterHandler(flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        ctx->networkManager.Subscribe<OnWaitingInfo>([&](const auto& packet) { OnWaitingInfoHandler(world, packet); });

        ctx->networkManager.Subscribe<OnConnectionAccepted>([&](const auto& packet) {
            OnConnectionAcceptedHandler(world, packet);
        });

        ctx->networkManager.Subscribe<OnConnectionRefused>([&](const auto& packet) {
            OnConnectionRefusedHandler(world, packet);
        });

        ctx->networkManager.Subscribe<DisconnectEvent>([&](const auto& event) {
            OnDisconnectEventHandler(world, event);
        });

        world.system()
            .kind<Phase::OnLoad>()
            .run([](flecs::iter& it) {
                while (it.next()) {};
                ServerConnectionState::Pending::Enter(it.world());
            })
            .add<GameScene>();
    }

    void ServerSessionModule::OnWaitingInfoHandler(const flecs::world& world, const OnWaitingInfo&)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        ctx->networkManager.Send<OnClientInfo>(
            { static_cast<ClientInfo>(ctx->settings) }, ENET_PACKET_FLAG_RELIABLE, 0
        );
    }

    void ServerSessionModule::OnConnectionAcceptedHandler(const flecs::world& world, const OnConnectionAccepted& packet)
    {
        auto* ctx       = ClientWorldContext::Get(world);
        ctx->playerInfo = packet.playerInfo;
        ctx->serverInfo = packet.serverInfo;

        ServerConnectionState::Connected::Enter(world);
    }

    void ServerSessionModule::OnConnectionRefusedHandler(const flecs::world& world, const OnConnectionRefused& packet)
    {
        MCC_LOG_ERROR("Connection refused with: {}", packet.reason);

        ServerConnectionState::Error::Enter(world);
    }

    void ServerSessionModule::OnDisconnectEventHandler(const flecs::world& world, const DisconnectEvent&)
    {
        world.quit();
    }

}