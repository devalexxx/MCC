// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Network/Module.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Network/System.h"
#include "Common/Utils/Logging.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    NetworkModule::NetworkModule(flecs::world& world) : BaseModule(world)
    {
        world.prefab<NetworkObjectPrefab>()
            .add<NetworkObjectTag>()
            .set_auto_override<NetworkProps>({ Null() });
    }

    void NetworkModule::RegisterComponent(flecs::world& world)
    {
        world.component<ServerTag>().add(flecs::Singleton);
        world.component<ClientTag>().add(flecs::Singleton);
        world.component<NetworkObjectTag>();

        world.component<NetworkObjectPrefab>();

        world.component<NetworkProps>();
    }

    void NetworkModule::RegisterSystem(flecs::world& /* world */)
    {}

    void NetworkModule::RegisterHandler(flecs::world& world)
    {
        world.observer<NetworkProps>()
            .event(flecs::OnAdd)
            .with<ServerTag>()
            .each(GenerateNetworkHandleObserver);

        world.observer<const NetworkProps>()
            .event(flecs::OnSet)
            .run(AddToContextObserver);

        world.observer<const NetworkProps>()
            .event(flecs::OnRemove)
            .run(RemoveFromContextObserver);
    }


}