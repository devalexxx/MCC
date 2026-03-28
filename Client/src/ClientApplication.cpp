// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/ClientApplication.h"

#include "Client/Module/Camera/Module.h"
#include "Client/Module/Player/Module.h"
#include "Client/Module/ServerSession/Module.h"
#include "Client/Module/TerrainRenderer/Module.h"
#include "Client/Scene/Scene.h"
#include "Client/WorldContext.h"

#include "Common/Network/Event.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    ClientApplication::ClientApplication() : ClientApplication(0, nullptr)
    {}

    ClientApplication::ClientApplication(const int argc, char** argv) :
        Application(argc, argv),
        mSettings(
            { .fov             = FOV_DEFAULT,
              .sensitivity     = SENSITIVITY_DEFAULT,
              .renderDistance  = RENDER_DISTANCE_DEFAULT,
              .preloadDistance = PRELOAD_DISTANCE_DEFAULT }
        ),
        mNetworkManager(mCmdLineStore),
        mWindow("MachinaCubicaCatalyst"),
        mAssetRegistry(
            {
                { "shader" , "Shaders"  },
                { "texture", "Textures" }
            },
            std::filesystem::current_path() / "Assets"
        )
    {
        if (const auto param = mCmdLineStore.GetParameter("asset-path"); param)
        {
            mAssetRegistry.SetBasePath(*param);
        }

        mNetworkManager.Subscribe<MalformedPacketEvent>([]([[maybe_unused]] const auto& packet) {
            MCC_LOG_WARN("Malformed packet caught", packet.packet == nullptr);
        });
    }

    int ClientApplication::Run()
    {
        if (const int error = mNetworkManager.Setup())
        {
            MCC_LOG_ERROR("Failed to setup network host");
            return error;
        }


        MCC_LOG_DEBUG("Setup modules...");
        mWorld.set_ctx(
            new ClientWorldContext {
                {
                    .networkManager = mNetworkManager,
                    .networkMapping = {},
                    .scheduler      = mScheduler,
                    .assetRegistry  = mAssetRegistry,
                    .chunkMap       = {}
                },
                {},
                {},
                mSettings,
                mWindow
        },
            [](void* ptr) { delete static_cast<ClientWorldContext*>(ptr); }
        );
        mWorld.import <CSceneImporter>();

        MCC_LOG_INFO("Application started");
        const auto builtinPipeline = mWorld.lookup("flecs::pipeline::BuiltinPipeline");
        while (!mWorld.should_quit() && !mWindow.ShouldClose())
        {
            mWorld.run_pipeline(builtinPipeline);
            mWorld.progress();
            mNetworkManager.Poll();
        }

        MCC_LOG_INFO("Shutdown...");

        return EXIT_SUCCESS;
    }

}