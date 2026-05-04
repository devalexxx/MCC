// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Entity/Component.h"

#include "Common/World/Translation.h"
#include "Common/World/Geometry.h"

namespace Mcc
{

    unsigned short UserInput::Meta::GetNextID()
    {
        static unsigned short next = 0;
        return next++;
    }

    namespace Helper
    {

        bool IsNull(const UserInput& input)
        {
            return !(
                input.movement.forward || input.movement.backward || input.movement.left || input.movement.right ||
                input.movement.up || input.movement.down || input.axis.x != 0 || input.axis.y != 0
            );
        }

        void ApplyMovement(const UserInput& input, CEntityTransform& transform, const float speed, const float dt)
        {
            auto translation = [&](const glm::vec3& v) {
                return static_cast<TranslationE>(TranslationF(transform.rotation * v * speed * dt));
            };

            if (input.movement.forward && !input.movement.backward)
                transform.position = transform.position + translation(glm::forward);

            if (input.movement.backward && !input.movement.forward)
                transform.position = transform.position - translation(glm::forward);

            if (input.movement.right && !input.movement.left)
                transform.position = transform.position + translation(glm::right);

            if (input.movement.left && !input.movement.right)
                transform.position = transform.position - translation(glm::right);

            if (input.movement.up && !input.movement.down)
                transform.position = transform.position + translation(glm::up);

            if (input.movement.down && !input.movement.up)
                transform.position = transform.position - translation(glm::up);
        }

        void ApplyXAxis(const UserInput& input, CEntityTransform& transform)
        {
            transform.rotation = glm::normalize(glm::angleAxis(input.axis.x, glm::up) * transform.rotation);
        }

        void ApplyYAxis(const UserInput& input, CEntityTransform& transform)
        {
            const glm::vec3 right  = transform.rotation * glm::right;
            const glm::quat qPitch = glm::angleAxis(input.axis.y, right);

            const glm::quat proposed   = qPitch * transform.rotation;
            const glm::vec3 newForward = proposed * glm::forward;

            const float angle = glm::degrees(glm::asin(glm::normalize(newForward).y));
            if (angle < 80.0f && angle > -80.0f)
            {
                transform.rotation = glm::normalize(qPitch * transform.rotation);
            }
        }

        void ApplyBothAxis(const UserInput& input, CEntityTransform& transform)
        {
            ApplyXAxis(input, transform);
            ApplyYAxis(input, transform);
        }

    }

}