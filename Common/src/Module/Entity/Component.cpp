// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Entity/Component.h"

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

        void ApplyMovement(const UserInput& input, CTransform& transform, float speed, float dt)
        {
            if (input.movement.forward && !input.movement.backward)
                transform.position += transform.rotation * glm::forward * speed * dt;

            if (input.movement.backward && !input.movement.forward)
                transform.position -= transform.rotation * glm::forward * speed * dt;

            if (input.movement.right && !input.movement.left)
                transform.position += transform.rotation * glm::right * speed * dt;

            if (input.movement.left && !input.movement.right)
                transform.position -= transform.rotation * glm::right * speed * dt;

            if (input.movement.up && !input.movement.down)
                transform.position += transform.rotation * glm::up * speed * dt;

            if (input.movement.down && !input.movement.up)
                transform.position -= transform.rotation * glm::up * speed * dt;
        }

        void ApplyXAxis(const UserInput& input, CTransform& transform)
        {
            transform.rotation = glm::normalize(glm::angleAxis(input.axis.x, glm::up) * transform.rotation);
        }

        void ApplyYAxis(const UserInput& input, CTransform& transform)
        {
            glm::vec3 right  = transform.rotation * glm::right;
            glm::quat qPitch = glm::angleAxis(input.axis.y, right);

            glm::quat proposed   = qPitch * transform.rotation;
            glm::vec3 newForward = proposed * glm::forward;

            float angle = glm::degrees(glm::asin(glm::normalize(newForward).y));

            if (angle < 80.0f && angle > -80.0f)
            {
                transform.rotation = glm::normalize(qPitch * transform.rotation);
            }
        }

        void ApplyBothAxis(const UserInput& input, CTransform& transform)
        {
            ApplyXAxis(input, transform);
            ApplyYAxis(input, transform);
        }

    }

}