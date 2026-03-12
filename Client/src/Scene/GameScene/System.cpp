// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Scene/GameScene/System.h"

#include "Client/Module/ServerSession/Component.h"
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
        ctx->chunkMap       = {};
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

        const auto ctx  = it.ctx<DebugContext>();
        elapsed        += it.delta_time();
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

        if (const auto peer = ClientWorldContext::Get(it.world())->networkManager.GetPeer())
        {
            if (ImGui::CollapsingHeader("Network"))
            {
                ImGui::Text("ping %d ms", peer->roundTripTime);
                ImGui::Text("loss %d", peer->packetLoss);
                ImGui::Text("packet sent: %d", peer->packetsSent);
            }
        }

        if (ImGui::CollapsingHeader("Chunk"))
        {
            // const auto totalChunk = it.world().count<ChunkTag>();
            const auto queued     = it.world().count<CChunkMeshGenTask>();
            ImGui::Text("queued : %d", queued);
            ImGui::Text("display: %d", it.world().count<TShouldRenderChunk>());
        }
        ImGui::End();

        IgnoreIter(it);
    }

}
