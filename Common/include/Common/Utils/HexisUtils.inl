// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <cereal/types/array.hpp>

namespace Mcc
{

    template<typename Archive, typename E, typename T>
    void serialize(Archive& ar, Hx::EnumArray<E, T>& enumArray)
    {
        ar(enumArray.GetContainer());
    }

}
