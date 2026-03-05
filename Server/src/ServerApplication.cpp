// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Server/ServerApplication.h"

#include "Server/Scene/Scene.h"
#include "Server/World/ChunkGenerator.h"
#include "Server/WorldContext.h"

#include "Common/Utils/Logging.h"

#include <fmt/format.h>

#include <charconv>

namespace Mcc
{

    ServerApplication::ServerApplication() : ServerApplication(0, nullptr)
    {}

    ServerApplication::ServerApplication(const int argc, char** argv) :
        Application(argc, argv),
        mSettings(
            { .tickRate       = TICK_RATE_DEFAULT,
              .renderDistance = RENDER_DISTANCE_DEFAULT,
              .userSpeed      = USER_SPEED_DEFAULT }
        ),
        mNetworkManager(mCmdLineStore)
    {
        if (CommandLineStore::OptParameter param; (param = mCmdLineStore.GetParameter("tick-rate").or_else([&] {
                                                      return mCmdLineStore.GetParameter("tr");
                                                  })).has_value())
        {
            unsigned long tickRate;
            std::from_chars(param->data(), param->data() + param->size(), tickRate);
            if (tickRate < TICK_RATE_MIN || tickRate > TICK_RATE_MAX)
            {
                MCC_LOG_WARN(
                    "TickRate must be between {} and {}, but was set to {}", TICK_RATE_MIN, TICK_RATE_MAX,
                    TICK_RATE_DEFAULT
                );
                tickRate = TICK_RATE_DEFAULT;
            }
            mSettings.tickRate = tickRate;
        }

        if (CommandLineStore::OptParameter param; (param = mCmdLineStore.GetParameter("render-distance").or_else([&] {
                                                      return mCmdLineStore.GetParameter("rd");
                                                  })).has_value())
        {
            unsigned long renderDistance;
            std::from_chars(param->cbegin(), param->cend(), renderDistance);
            if (renderDistance < RENDER_DISTANCE_MIN || renderDistance > RENDER_DISTANCE_MAX)
            {
                MCC_LOG_WARN(
                    "RenderDistance must be between {} and {}, but was set to {}", RENDER_DISTANCE_MIN,
                    RENDER_DISTANCE_MAX, RENDER_DISTANCE_DEFAULT
                );
                renderDistance = TICK_RATE_DEFAULT;
            }
            mSettings.renderDistance = renderDistance;
        }
    }

    int ServerApplication::Run()
    {
        if (const int state = mNetworkManager.Setup())
        {
            MCC_LOG_ERROR("Failed to create the network host");
            return state;
        }

        MCC_LOG_DEBUG("Setup world...");
        mWorld.set_ctx(
            new ServerWorldContext {
                { .networkManager = mNetworkManager, .networkMapping = {}, .scheduler = mScheduler, .chunkMap = {} },
                mSettings,
        },
            [](void* ptr) { delete static_cast<ServerWorldContext*>(ptr); }
        );
        mWorld.import<SSceneImporter>();

        MCC_LOG_INFO("Application started and listening on port {}", mNetworkManager.mAddr.port);
        mWorld.set_target_fps(mSettings.tickRate);
        const auto builtinPipeline = mWorld.lookup("flecs::pipeline::BuiltinPipeline");
        while (!mWorld.should_quit())
        {
            mWorld.run_pipeline(builtinPipeline);
            mWorld.progress();
            mNetworkManager.Poll();
        }

        MCC_LOG_INFO("Shutdown...");

        return EXIT_SUCCESS;
    }

}