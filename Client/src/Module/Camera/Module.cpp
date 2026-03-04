// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Camera/Module.h"

#include "Client/Module/Camera/Component.h"
#include "Client/Module/Camera/System.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Entity/Module.h"
#include "Common/Phase.h"
#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    CameraModule::CameraModule(flecs::world& world) : BaseModule(world)
    {
        world.prefab<CameraPrefab>()
            .is_a<EntityPrefab>()
            .add<CameraTag>()
            .set_auto_override<CameraSettings>({});

        world.prefab<CameraFollowPrefab>()
            .is_a<CameraPrefab>()
            .add<CameraFollowTag>()
            .set_auto_override<CameraFollowSettings>({});
    }

    void CameraModule::RegisterComponent(flecs::world& world)
    {
        world.component<CameraTag>();
        world.component<CameraFollowTag>();
        world.component<ActiveCameraTag>();

        world.component<CameraPrefab>();
        world.component<CameraFollowPrefab>();

        world.component<CameraFollowRelation>();

        world.component<CameraSettings>();
        world.component<CameraFollowSettings>();
    }

    void CameraModule::RegisterSystem(flecs::world& world)
    {
        world.system<Transform, const CameraFollowSettings>()
            .kind<Phase::OnUpdate>()
            .with<CameraFollowTag>()
            .with<CameraFollowRelation>(flecs::Wildcard)
            .each(CameraFollowSystem);
    }

    void CameraModule::RegisterHandler(flecs::world& /* world */)
    {}


}