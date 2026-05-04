// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Camera/Module.h"

#include "Client/Module/Camera/Component.h"
#include "Client/Module/Camera/System.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Phase.h"

namespace Mcc
{

    CameraModule::CameraModule(flecs::world& world) : BaseModule(world)
    {}

    void CameraModule::RegisterComponent(flecs::world& world)
    {
        world.component<TCamera>();
        world.component<TCameraFollow>();
        world.component<TActiveCamera>();

        world.component<PCamera>();
        world.component<PCameraFollow>();

        world.component<RCameraFollow>();

        world.component<CCameraSettings>()
            .member<float>("fov")
            .member<float>("zNear")
            .member<float>("zFar");

        world.component<CCameraFollowSettings>()
            .member<glm::vec3>("offset");
    }

    void CameraModule::RegisterPrefab(flecs::world& world)
    {
        world.prefab<PCamera>()
            .is_a<PEntity>()
            .add<TCamera>()
            .set<CCameraSettings>({});

        world.prefab<PCameraFollow>()
            .is_a<PCamera>()
            .add<TCameraFollow>()
            .set<CCameraFollowSettings>({});
    }

    void CameraModule::RegisterSystem(flecs::world& world)
    {
        world.system<CEntityTransform, const CCameraFollowSettings>("CameraFollowSystem")
            .kind<Phase::PostUpdate>()
            .with<TCameraFollow>()
            .with<RCameraFollow>(flecs::Wildcard)
            .each(CameraFollowSystem);
    }


    void CameraModule::RegisterObserver(flecs::world& /* world */) {}

    flecs::entity CameraModule::GetActiveCamera(const flecs::world& world)
    {
        static auto query = world.query_builder().with<TActiveCamera>().build();

        if (query.count() == 0)
        {
            MCC_LOG_ERROR("[CameraModule] No active camera");
            return flecs::entity::null();
        }

        if (query.count() > 1)
        {
            MCC_LOG_WARN("[CameraModule] More than one camera active");
        }

        return query.first();
    }


}