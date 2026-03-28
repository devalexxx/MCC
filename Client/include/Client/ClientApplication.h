// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_CLIENT_APPLICATION_H
#define MCC_CLIENT_CLIENT_APPLICATION_H

#include "Client/ClientNetworkManager.h"
#include "Client/ClientSettings.h"
#include "Client/Graphics/Window/Window.h"

#include "Common/Application.h"
#include "Common/AssetRegistry.h"

#include <Hexis/Core/TaskScheduler.h>

#define SENSITIVITY_DEFAULT 0.001
#define FOV_DEFAULT 90
#define RENDER_DISTANCE_DEFAULT 24
#define PRELOAD_DISTANCE_DEFAULT 24

namespace Mcc
{

    class ClientApplication final : public Application
    {
      public:
        ClientApplication();
        ClientApplication(int argc, char** argv);

        int Run() override;

      private:
        ClientSettings            mSettings;
        ClientNetworkManager      mNetworkManager;
        Window                    mWindow;
        Hx::TaskScheduler         mScheduler;
        AssetRegistry             mAssetRegistry;
    };

}

#endif
