// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_PLAYER_MODULE_H
#define MCC_SERVER_MODULE_PLAYER_MODULE_H

#include "Server/Module/EntityReplication/Module.h"

#include "Common/Module/Base/Module.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Network/Packet.h"

namespace Mcc
{

    class PlayerModule final : public BaseModule<PlayerModule, EntityReplicationModule>
    {
      public:
        explicit PlayerModule(flecs::world& world);

        void RegisterComponent(flecs::world& world) override;
        void RegisterSystem(flecs::world& world) override;
        void RegisterHandler(flecs::world& world) override;

      private:
        static void OnPlayerInputHandler(const flecs::world& world, const From<OnPlayerInput>& from);
    };

}

#endif
