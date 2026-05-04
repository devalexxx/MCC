// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_SERVER_APPLICATION_H
#define MCC_SERVER_SERVER_APPLICATION_H

#include "Server/ServerNetworkManager.h"
#include "Server/ServerSettings.h"

#include "Common/Application.h"
#include "Common/AssetRegistry.h"

#include "Hexis/Core/TaskScheduler.h"

#define TICK_RATE_DEFAULT 20
#define TICK_RATE_MAX 60
#define TICK_RATE_MIN 20

#define RENDER_DISTANCE_DEFAULT 16
#define RENDER_DISTANCE_MAX 64
#define RENDER_DISTANCE_MIN 8

#define USER_SPEED_DEFAULT 20

namespace Mcc
{

    class ServerApplication final : public Application
    {
      public:
        ServerApplication(int argc, char** argv);
        ServerApplication();

        int Run() override;

      private:
        ServerSettings       mSettings;
        ServerNetworkManager mNetworkManager;
        Hx::TaskScheduler    mScheduler;
        AssetRegistry        mAssetRegistry;
    };

}

#endif
