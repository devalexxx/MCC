// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_MODULE_PLAYER_MODULE_H
#define MCC_CLIENT_MODULE_PLAYER_MODULE_H

#include "Client/Graphics/Window/Event.h"
#include "Client/Module/Camera/Module.h"
#include "Client/Module/EntityReplication/Module.h"

#include "Common/Module/Base/Module.h"
#include "Common/Utils/EventManager.h"

#include <flecs.h>

namespace Mcc
{

    class PlayerModule final : public BaseModule<PlayerModule, EntityReplicationModule, CameraModule>
    {
      public:
        explicit PlayerModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;

        void SetInputHandler(flecs::world& world);
        void ClearInputHandler(flecs::world& world);

      private:
        static void OnEntitiesCreatedHandler(const OnEntitiesCreated& event, const flecs::world& world);
        static void OnEntitiesUpdatedHandler(const OnEntitiesUpdated& event, const flecs::world& world);

        static void OnKeyEventHandler(const KeyEvent& event, const flecs::world& world);
        static void OnCursorPosEventHandler(const CursorPosEvent& event, const flecs::world& world);

        EventHandlerID mKeyEventHandlerID;
        EventHandlerID mCursorPosEventHandlerID;
    };

}

#endif
