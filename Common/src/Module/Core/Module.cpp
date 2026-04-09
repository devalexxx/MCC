// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Core/Module.h"

#include "Common/World/Position.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Mcc
{

    CoreModule::CoreModule(flecs::world& world) : BaseModule(world) {}

    void CoreModule::RegisterComponent(flecs::world& world)
    {
        world.component<std::string>()
            .opaque(flecs::String)
            .serialize([](const flecs::serializer *s, const std::string *data) {
                const char *str = data->c_str();
                return s->value(flecs::String, &str);
            })
            .assign_string([](std::string* data, const char *value) {
                *data = value;
            });

        world.component<glm::vec3>()
            .member<float>("x")
            .member<float>("y")
            .member<float>("z");

        world.component<glm::ivec3>()
            .member<int32_t>("x")
            .member<int32_t>("y")
            .member<int32_t>("z");


        world.component<glm::ivec2>()
            .member<int32_t>("x")
            .member<int32_t>("y");

        world.component<glm::uvec3>()
            .member<uint32_t>("x")
            .member<uint32_t>("y")
            .member<uint32_t>("z");

        world.component<glm::quat>()
            .member<float>("x")
            .member<float>("y")
            .member<float>("z")
            .member<float>("w");

        world.component<ChunkPosV>();
    }

    void CoreModule::RegisterPrefab(flecs::world& /* world */) {}

    void CoreModule::RegisterSystem(flecs::world& /* world */) {}

    void CoreModule::RegisterObserver(flecs::world& /* world */) {}

}
