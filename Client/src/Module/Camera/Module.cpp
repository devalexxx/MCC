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
        world.system<CTransform, const CCameraFollowSettings>("CameraFollowSystem")
            .kind<Phase::PostUpdate>()
            .with<TCameraFollow>()
            .with<RCameraFollow>(flecs::Wildcard)
            .each(CameraFollowSystem);
    }

    void CameraModule::RegisterObserver(flecs::world& /* world */) {}


}