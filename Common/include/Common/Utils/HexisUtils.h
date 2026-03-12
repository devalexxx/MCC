// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_HEXIS_UTILS_H
#define MCC_COMMON_UTILS_HEXIS_UTILS_H

#include <Hexis/Core/EnumArray.h>

namespace Mcc
{

    template<typename Archive, typename E, typename T>
    void serialize(Archive& ar, Hx::EnumArray<E, T>& enumArray);

}

#include "Common/Utils/HexisUtils.inl"

#endif
