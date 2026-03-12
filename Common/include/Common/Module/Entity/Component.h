// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_ENTITY_COMPONENT_H
#define MCC_COMMON_MODULE_ENTITY_COMPONENT_H

#include "Common/Export.h"
#include "Common/Module/Core/Component.h"
#include "Common/Utils/FlecsUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <deque>
#include <string>
#include <unordered_map>

namespace glm
{

    static constexpr vec3 forward { 0, 0, -1 };
    static constexpr vec3 back { 0, 0, 1 };
    static constexpr vec3 right { 1, 0, 0 };
    static constexpr vec3 left { -1, 0, 0 };
    static constexpr vec3 up { 0, 1, 0 };
    static constexpr vec3 down { 0, -1, 0 };

}

namespace Mcc
{

    struct TEntity        {};
    struct TNetworkEntity {};
    struct TUserEntity    {};

    struct PEntity     {};
    struct PNetEntity  {};
    struct PUserEntity {};

    struct MCC_LIB_API UserInput
    {
        struct Meta
        {
            unsigned short id;
            float          dt;

            static unsigned short GetNextID();
        } meta;

        struct Movement
        {
            bool forward;
            bool backward;
            bool left;
            bool right;
            bool up;
            bool down;
        } movement;

        struct Axis
        {
            float x;
            float y;
        } axis;
    };


    namespace _ { struct EntityModuleTag {}; };
    using CEntityDataMap  = ComponentWrapper<std::unordered_map<std::string, std::string>, _::EntityModuleTag>;
    using CUserInputQueue = ComponentWrapper<std::deque<UserInput>                       , _::EntityModuleTag>;

    template<typename Archive>
    void serialize(Archive& ar, UserInput& input);

    namespace Helper
    {

        MCC_LIB_API bool IsNull(const UserInput& input);
        MCC_LIB_API void ApplyMovement(const UserInput& input, CTransform& transform, float speed, float dt);
        MCC_LIB_API void ApplyXAxis(const UserInput& input, CTransform& transform);
        MCC_LIB_API void ApplyYAxis(const UserInput& input, CTransform& transform);
        MCC_LIB_API void ApplyBothAxis(const UserInput& input, CTransform& transform);

    }

}

#include "Common/Module/Entity/Component.inl"

#endif
