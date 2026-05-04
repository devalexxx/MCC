// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Scene/GameScene/System.h"

#include "Client/Module/Player/Component.h"
#include "Client/Module/Renderer/Component.h"
#include "Client/Module/ServerSession/Module.h"
#include "Client/Module/TerrainRenderer/Component.h"
#include "Client/Scene/GameScene/Component.h"
#include "Client/Scene/GameScene/Scene.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/Utils/FlecsUtils.h"

#include "Hexis/Core/LambdaAsFuncPtr.h"
#include "Hexis/Math/FloatingPoint.h"

#include <algorithm>

#include "imgui.h"

namespace Mcc
{

    void OnEnterGameStateInGame(flecs::iter& it)
    {
        auto world = it.world();
        ClientWorldContext::Get(world)->window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        if (const auto module = world.try_get_mut<PlayerModule>(); module)
        {
            module->SetInputHandler(world);
        }

        IgnoreIter(it);
    }

    void OnExitGameStateInGame(flecs::iter& it)
    {
        const auto world = it.world();
        ClientWorldContext::Get(world)->window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (const auto module = world.try_get_mut<PlayerModule>(); module)
        {
            module->ClearInputHandler(world);
        }

        IgnoreIter(it);
    }

    void OnEnterGameStateShutdown(flecs::iter& it)
    {
        const auto world = it.world();
        world.each<CChunkMeshGenTask>([](CChunkMeshGenTask& task) { task.Cancel(); });
        ClientWorldContext::Get(world)->scheduler.StartJoin("game_group");

        IgnoreIter(it);
    }

    void OnEnterSessionStateActive(flecs::iter& it)
    {
        GameState::InGame::Enter(it.world());

        IgnoreIter(it);
    }

    void OnSessionOrConnLost(flecs::iter& it)
    {
        GameState::Shutdown::Enter(it.world());

        IgnoreIter(it);
    }

    void SetupGameStateSystem(flecs::iter& it)
    {
        GameState::Load::Enter(it.world());

        IgnoreIter(it);
    }

    void ConnectToServerSystem(flecs::iter& it)
    {
        ServerSessionModule::Connect(it.world());

        IgnoreIter(it);
    }

    void ClearGameInfoSystem(flecs::iter& it)
    {
        const auto ctx      = ClientWorldContext::Get(it.world());
        ctx->chunkMapping   = {};
        ctx->networkMapping = {};

        IgnoreIter(it);
    }

    void DisconnectFromServerSystem(flecs::iter& it)
    {
        ServerSessionModule::Disconnect(it.world());

        IgnoreIter(it);
    }

    void DisplayEscapeMenuSystem(const flecs::iter& it, size_t)
    {
        const auto world = it.world();
        const auto ctx   = ClientWorldContext::Get(world);
        ImGui::Begin("Escape Menu");
        if (ImGui::Button("Return"))
        {
            GameState::InGame::Enter(world);
        }
        if (ImGui::Button("Back Main Menu"))
        {
            GameState::Enter<GameState::Shutdown>(world);
        }
        if (ImGui::Button("Quit Game"))
        {
            ctx->window.SetShouldClose();
        }
        ImGui::End();
    }

    void DisplayLoadScreenSystem(const flecs::iter& it, size_t)
    {
        ImGui::Begin("Loading...");
        if (ImGui::Button("Cancel"))
        {
            GameState::Enter<GameState::Shutdown>(it.world());
        }
        ImGui::End();
    }

    void JoinPendingMeshTaskSystem(const flecs::iter& it, size_t)
    {
        const auto world = it.world();
        const auto ctx   = ClientWorldContext::Get(world);
        if (ctx->scheduler.IsJoined("game_group"))
        {
            ctx->scheduler.StopJoin("game_group");
            it.world().add<ActiveScene, MenuScene>();
        }
    }

    void DisplayDebugMenuSystem(flecs::iter& it)
    {
        static float  elapsed = 0, max = 0;
        static size_t frames      = 0;
        static char   overlay[50] = "value";

        const auto world = it.world();
        const auto ctx   = it.ctx<DebugContext>();
        const auto dt    = it.delta_time();
        elapsed         += dt;
        frames++;
        if (elapsed >= .2f)
        {
            const auto old   = ctx->fpsHistory[ctx->fpsHistoryIndex];
            const auto value = static_cast<float>(frames) / elapsed;
            if (value > max)
                max = value;

            ctx->fpsHistory[ctx->fpsHistoryIndex] = value;
            if (Hx::Equal(max, old))
                max = *std::ranges::max_element(ctx->fpsHistory);

            snprintf(overlay, 50, "value %.2f", value);
            ctx->fpsHistoryIndex = (ctx->fpsHistoryIndex + 1) % ctx->fpsHistorySize;
            elapsed              = 0;
            frames               = 0;
        }

        const auto plotter = Hx::LambdaAsFuncPtr<float (*)(void*, int)>([&ctx](void*, const int i) {
            const auto index = (ctx->fpsHistoryIndex + i) % ctx->fpsHistorySize;
            return ctx->fpsHistory[index];
        });


        ImGui::Begin("Debug");
        ImGui::PlotLines(
            "FPS", plotter, &ctx->fpsHistory, ctx->fpsHistorySize, 0, overlay, 0.0f, max * 1.4f, ImVec2(200, 50)
        );

        if (ImGui::CollapsingHeader("RenderSetting"))
        {
            auto& [wireframe] = world.get_mut<CRendererSettings>();
            if (ImGui::Checkbox("Wireframe", &wireframe))
            {
                // wireframe = !wireframe;
            }
        }


        const auto host = ClientWorldContext::Get(it.world())->networkManager.GetHost();
        const auto peer = ClientWorldContext::Get(it.world())->networkManager.GetPeer();
        if (host && peer)
        {
            ctx->incomingBandwidthHistory.Add(static_cast<float>(host->totalReceivedData) * 0.0001f, dt);
            ctx->outgoingBandwidthHistory.Add(static_cast<float>(host->totalSentData    ) * 0.0001f, dt);
            ctx->incomingDataTotal += static_cast<double>(host->totalReceivedData) * 0.0001;
            ctx->incomingDataTotal += static_cast<double>(host->totalSentData    ) * 0.0001;
            host->totalReceivedData = 0;
            host->totalSentData     = 0;

            if (ImGui::CollapsingHeader("Network"))
            {
                ctx->incomingBandwidthHistory.Plot(
                    "Received", "kb/s", " ",fmt::format("\ntotal: {:.2f} kb", ctx->incomingDataTotal).c_str()
                );

                ctx->outgoingBandwidthHistory.Plot(
                    "Received", "kb/s", " ",fmt::format("\ntotal: {:.2f} kb", ctx->outgoingDataTotal).c_str()
                );

                ImGui::Spacing();
                ImGui::Text("packet received: %d", host->totalReceivedPackets);
                ImGui::Text("packet sent    : %d", peer->packetsSent);
                ImGui::Spacing();
                ImGui::Text("ping %d ms", peer->roundTripTime);
                ImGui::Text("loss %d", peer->packetLoss);
            }
        }

        if (ImGui::CollapsingHeader("Chunk"))
        {
            // const auto totalChunk = it.world().count<ChunkTag>();
            const auto queued     = world.count<CChunkMeshGenTask>();
            ImGui::Text("queued : %d", queued);
            ImGui::Text("display: %d", world.count<TShouldRenderChunk>());
        }

        if (ImGui::CollapsingHeader("Player"))
        {
            if (const auto player = PlayerModule::GetPlayer(it.world()); player.is_valid())
            {
                const auto& transform         = player.get_mut<CEntityTransform>();
                auto  position                = glm::vec3(WorldPosF(transform.position));
                auto  [chunkPos, posInChunk]  = transform.position;
                ImGui::InputFloat3("Position", &position[0]);
                ImGui::InputInt3  ("ChunkPosition", &static_cast<glm::ivec3>(chunkPos)[0]);
                ImGui::InputFloat3("PositionInChunk", &static_cast<glm::vec3>(posInChunk)[0]);
            }

            if (const auto facing = world.get<PlayerModule>().GetFacingInfo())
            {
                ImGui::Text("distance: %f", facing->distance);
                ImGui::Text("block id: %s", facing->blockName);
            }
        }

        if (ImGui::CollapsingHeader("Camera"))
        {
            if (const auto player = PlayerModule::GetPlayer(world); player.is_valid())
            {
                auto camera  = CameraModule::GetActiveCamera(world);
                auto cameraTransform = camera.get<CEntityTransform>();
                auto cameraDirection = glm::normalize(cameraTransform.rotation * glm::forward);
                ImGui::InputFloat3("CameraDirection", &cameraDirection[0]);
            }
        }

        ImGui::End();

        IgnoreIter(it);
    }

}
