// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Entity/Module.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Network/Component.h"
#include "Common/Utils/FlecsUtils.h"

namespace Mcc
{

    EntityModule::EntityModule(flecs::world& world) : BaseModule(world) {}

    void EntityModule::RegisterComponent(flecs::world& world)
    {
        world.component<TEntity>();
        world.component<TNetworkEntity>();
        world.component<TUserEntity>();

        world.component<PEntity>();
        world.component<PNetEntity>();
        world.component<PUserEntity>();

        world.component<CTransform>()
            .member<glm::vec3>("position")
            .member<glm::quat>("rotation")
            .member<glm::vec3>("scale");

        AutoRegister<CEntityDataMap>::Register(world, "CEntityDataMap");

        world.component<UserInput::Meta>()
            .member<uint16_t>("id")
            .member<float>   ("dt");

        world.component<UserInput::Movement>()
            .member<bool>("forward")
            .member<bool>("backward")
            .member<bool>("left")
            .member<bool>("right")
            .member<bool>("up")
            .member<bool>("down");

        world.component<UserInput::Axis>()
            .member<float>("x")
            .member<float>("y");

        world.component<UserInput>()
            .member<UserInput::Meta>    ("meta")
            .member<UserInput::Movement>("movement")
            .member<UserInput::Axis>    ("axis");

        AutoRegister<CUserInputQueue>::Register(world, "CUserInputQueue");
    }

    void EntityModule::RegisterPrefab(flecs::world& world)
    {
        world.prefab<PEntity>()
            .add<TEntity>()
            .set<CTransform>(CTransform::Identity());

        world.prefab<PNetEntity>()
            .is_a<PNetObject>()
            .is_a<PEntity>()
            .add<TNetworkEntity>()
            .set<CEntityDataMap>({});

        world.prefab<PUserEntity>()
            .is_a<PNetEntity>()
            .add<TUserEntity>()
            .set<CUserInputQueue>({});
    }

    void EntityModule::RegisterSystem(flecs::world& /* world */) {}

    void EntityModule::RegisterObserver(flecs::world& /* world */) {}

}