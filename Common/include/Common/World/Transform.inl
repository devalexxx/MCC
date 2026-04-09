// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename C>
    template<typename C2>
    constexpr Transform<C>::operator Transform<C2>() const
    {
        if constexpr (std::is_same_v<C, C2>) return *this;

        return Transform<C2> { position, rotation, scale };
    }

    template<typename Archive, typename C>
    constexpr void serialize(Archive& ar, Transform<C>& transform)
    {
        ar(transform.position, transform.rotation, transform.scale);
    }


}
