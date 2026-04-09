// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Entity/Module.h"

#include "Common/Module/Entity/Component.h"
#include "Common/Module/Network/Component.h"
#include "Common/Utils/FlecsUtils.h"

#include <glm/gtx/quaternion.hpp>

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

        world.component<WorldPosE>()
            .opaque(flecs::String)
            .serialize([](const flecs::serializer* s, const WorldPosE* data) {
                auto [ parent, local ] = *data;
                const std::string str = fmt::format("{} {}", parent, local);
                return s->value(flecs::String, str.c_str());
            });

        world.component<WorldPosE>("WorldPosE")
            .opaque(
                world.component()
                    .member<glm::ivec2>("parent")
                    .member<glm::fvec3>("local")
            )
            .serialize([](const flecs::serializer* s, const WorldPosE* data)
            {
                auto [ parent, local ] = *data;
                s->member("parent");
                s->value(glm::ivec2(parent));
                s->member("local");
                s->value(glm::vec3(local));
                return 0;
            })
            .ensure_member([](WorldPosE*, const char* member) -> void*
            {
                static glm::ivec2 fake1;
                static glm::fvec3 fake2;

                const auto str = std::string_view(member);
                if (str == "parent") return &fake1;
                if (str == "local")  return &fake2;

                return nullptr;
            });

        world.component<CEntityTransform>("CEntityTransform")
            .member("position", &CEntityTransform::position)
            .member("rotation", &CEntityTransform::rotation)
            .member("scale"   , &CEntityTransform::scale);

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
            .set<CEntityTransform>({});

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