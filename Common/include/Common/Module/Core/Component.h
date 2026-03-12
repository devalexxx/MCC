// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_MODULE_CORE_COMPONENT_H
#define MCC_COMMON_MODULE_CORE_COMPONENT_H

#include "Common/Export.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glm
{
    template<class Archive>
    void serialize(Archive& ar, vec3& vec);

    template<class Archive>
    void serialize(Archive& ar, uvec3& vec);

    template<class Archive>
    void serialize(Archive& ar, ivec3& vec);

    template<class Archive>
    void serialize(Archive& ar, quat& quat);
}

namespace Mcc
{

    struct MCC_LIB_API CTransform
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        static CTransform Identity();
    };

    template<class Archive>
    void serialize(Archive& ar, CTransform& transform);

}

#include "Common/Module/Core/Component.inl"

#endif
