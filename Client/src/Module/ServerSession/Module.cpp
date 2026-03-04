// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/ServerSession/Module.h"

#include "Client/Module/ServerSession/Component.h"
#include "Client/Module/ServerSession/System.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/SceneImporter.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    ServerSessionModule::ServerSessionModule(flecs::world& world) : BaseModule(world)
    {}


    void ServerSessionModule::RegisterComponent(flecs::world& world)
    {
        SessionState::Register(world);
        SrvConnState::Register(world);

        world.component<SrvConnResult> ().add(flecs::Singleton);
        world.component<SrvDConnResult>().add(flecs::Singleton);
    }

    void ServerSessionModule::RegisterSystem(flecs::world& world)
    {
        world.system()
            .kind<Phase::OnLoad>()
            .run(SetupSessionStateSystem)
            .add<GameScene>();

        world.system<SrvConnResult>()
            .kind<Phase::OnSetup>()
            .each(HandleConnectionResultSystem);

        world.system<SrvDConnResult>()
            .kind<Phase::OnSetup>()
            .each(HandleDisconnectionResultSystem);
    }

    void ServerSessionModule::RegisterHandler(flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        ctx->networkManager.Subscribe<OnWaitingInfo>(OnWaitingInfoHandler, world);
        ctx->networkManager.Subscribe<OnConnectionAccepted>(OnConnectionAcceptedHandler, world);
        ctx->networkManager.Subscribe<OnConnectionRefused>(OnConnectionRefusedHandler, world);
        ctx->networkManager.Subscribe<DisconnectEvent>(OnDisconnectEventHandler, world);
    }

    void ServerSessionModule::Connect(const flecs::world& world)
    {
        const auto ctx = ClientWorldContext::Get(world);
        auto result = ctx->scheduler.Insert([=]() { return ctx->networkManager.Connect() == 0; }).AsUnique().Enqueue();
        world.emplace<SrvConnResult>(std::move(result));
        SrvConnState::Pending::Enter(world);
    }

    void ServerSessionModule::Disconnect(const flecs::world& world)
    {
        const auto ctx = ClientWorldContext::Get(world);
        auto result = ctx->scheduler.Insert([=]() { return ctx->networkManager.Disconnect() == 0; }).AsUnique().Enqueue();
        world.emplace<SrvDConnResult>(std::move(result));
    }

    void ServerSessionModule::OnWaitingInfoHandler(const OnWaitingInfo&, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        ctx->networkManager.Send<OnClientInfo>(
            { static_cast<ClientInfo>(ctx->settings) }, ENET_PACKET_FLAG_RELIABLE, 0
        );

        SessionState::Pending::Enter(world);
    }

    void ServerSessionModule::OnConnectionAcceptedHandler(const OnConnectionAccepted& packet, const flecs::world& world)
    {
        auto* ctx       = ClientWorldContext::Get(world);
        ctx->playerInfo = packet.playerInfo;
        ctx->serverInfo = packet.serverInfo;

        SessionState::Active::Enter(world);
    }

    void ServerSessionModule::OnConnectionRefusedHandler(const OnConnectionRefused& packet, const flecs::world& world)
    {
        MCC_LOG_ERROR("Connection refused with: {}", packet.reason);

        SessionState::Failed::Enter(world);
    }

    void ServerSessionModule::OnDisconnectEventHandler(const DisconnectEvent&, const flecs::world& world)
    {
        auto* ctx       = ClientWorldContext::Get(world);
        ctx->playerInfo = {};
        ctx->serverInfo = {};

        if (!SessionState::Failed::IsActive(world))
            SessionState::Inactive::Enter(world);

        if (!SrvConnState::Failed::IsActive(world))
            SrvConnState::Disconnected::Enter(world);
    }

}