// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/EntityReplication/Module.h"

#include "Client/Module/EntityReplication/Component.h"
#include "Client/Module/EntityReplication/System.h"
#include "Client/WorldContext.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Module/Network/Component.h"
#include "Common/Network/Packet.h"
#include "Common/Phase.h"
#include "Common/SceneImporter.h"

namespace Mcc
{

    EntityReplicationModule::EntityReplicationModule(flecs::world& world) : BaseModule(world)
    {
        const auto* ctx = ClientWorldContext::Get(world);
        ctx->networkManager.Subscribe<OnEntitiesCreated>  (OnEntitiesCreatedHandler, world);
        ctx->networkManager.Subscribe<OnEntitiesUpdated>  (OnEntitiesUpdatedHandler, world);
        ctx->networkManager.Subscribe<OnEntitiesDestroyed>(OnEntitiesDestroyedHandler, world);
    }

    void EntityReplicationModule::RegisterComponent(flecs::world& world)
    {
        world.component<TInterpolationExcluded>();

        world.component<TimePoint>()
            .opaque(flecs::String)
            .serialize([](const flecs::serializer* s, const TimePoint* data) {
                const std::string str = std::to_string(data->time_since_epoch().count());
                return s->value(flecs::String, str.c_str());
            });

        world.component<Snapshot>()
            .member<CEntityTransform>("transform")
            .member<TimePoint>       ("time");

        AutoRegister<CSnapshotQueue>::Register(world, "CSnapshotQueue");
    }

    void EntityReplicationModule::RegisterPrefab(flecs::world& /* world*/ ) {}

    void EntityReplicationModule::RegisterSystem(flecs::world& world)
    {
        world.system<CEntityTransform, CSnapshotQueue>("EntityInterpolation")
            .kind<Phase::OnUpdate>()
            .without<TInterpolationExcluded>()
            .each(EntityInterpolationSystem);
    }

    void EntityReplicationModule::RegisterObserver(flecs::world& /* world */) {}

    void EntityReplicationModule::OnEntitiesCreatedHandler(const OnEntitiesCreated& event, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        for (auto& state: event.states)
        {
            if (const auto lid = ctx->networkMapping.GetLHandle(state.handle); lid.has_value())
            {
                MCC_LOG_WARN(
                    "[EntityCreatedHandler] The network id {} is already associated to a local entity(#{})",
                    state.handle, *lid
                );
                continue;
            }

            world.entity()
                .is_a<PNetEntity>()
                .set<CNetProps>({ state.handle })
                .set(state.transform)
                .set<CSnapshotQueue>({})
                .child_of<SceneRoot>();
        }
    }


    void EntityReplicationModule::OnEntitiesUpdatedHandler(const OnEntitiesUpdated& event, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        for (const auto& state: event.states)
        {
            if (auto id = ctx->networkMapping.GetLHandle(state.handle); id.has_value())
            {
                if (!world.is_alive(*id))
                {
                    MCC_LOG_WARN("[EntityUpdateHandler] Entity(#{}, {}) isn't alive", *id, state.handle);
                    continue;
                }

                const auto queue = world.entity(*id).try_get_mut<CSnapshotQueue>();
                if (!queue)
                {
                    MCC_LOG_WARN("[EntityUpdateHandler] Entity(#{}, {}) doesn't have a snapshot queue", *id,state.handle);
                    return;
                }

                queue->push_front({ .transform = state.transform, .time = TimeClock::now() });
            }
        }
    }

    void
    EntityReplicationModule::OnEntitiesDestroyedHandler(const OnEntitiesDestroyed& event, const flecs::world& world)
    {
        const auto* ctx = ClientWorldContext::Get(world);

        for (const auto handle: event.handles)
        {
            if (auto id = ctx->networkMapping.GetLHandle(handle); id.has_value())
            {
                if (!world.is_alive(*id))
                {
                    MCC_LOG_WARN("[EntityDestroyedHandler] Entity(#{}, {}) isn't alive", *id, handle);
                    continue;
                }

                world.entity(*id).destruct();
            }
        }
    }

}
