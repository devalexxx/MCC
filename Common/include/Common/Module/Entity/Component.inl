// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename Archive>
    void serialize(Archive& ar, UserInput& input)
    {
        ar(input.movement.forward, input.movement.backward, input.movement.right, input.movement.left,
           input.movement.up, input.movement.down, input.axis.x, input.axis.y, input.meta.id, input.meta.dt);
    }

}
