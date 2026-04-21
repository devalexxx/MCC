// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Network/Module.h"

#include "Common/Module/Network/Component.h"
#include "Common/Module/Network/System.h"
#include "Common/Phase.h"
#include "Common/WorldContext.h"

namespace Mcc
{

    NetworkModule::NetworkModule(flecs::world& world) : BaseModule(world) {}

    void NetworkModule::RegisterComponent(flecs::world& world)
    {
        world.component<TServer>().add(flecs::Singleton);
        world.component<TClient>().add(flecs::Singleton);
        world.component<TNetObject>();

        world.component<PNetObject>();

        world.component<CNetProps>()
            .member<uint32_t>("handle");
    }

    void NetworkModule::RegisterPrefab(flecs::world& world)
    {
        world.prefab<PNetObject>()
            .add<TNetObject>()
            .set<CNetProps>({ Null() });
    }

    void NetworkModule::RegisterSystem(flecs::world& world)
    {
        world.system("PollNetwork")
            .kind<Phase::OnPoll>()
            .immediate()
            .run(PollNetworkSystem);
    }

    void NetworkModule::RegisterObserver(flecs::world& world)
    {
        world.observer<CNetProps>()
            .event(flecs::OnAdd)
            .with<TServer>()
            .each(GenerateNetworkHandleObserver);

        world.observer<const CNetProps>()
            .event(flecs::OnSet)
            .run(AddToContextObserver);

        world.observer<const CNetProps>()
            .event(flecs::OnRemove)
            .run(RemoveFromContextObserver);
    }


}