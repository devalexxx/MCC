// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_BLOCK_UTILS_H
#define MCC_COMMON_UTILS_BLOCK_UTILS_H

#include "Common/Export.h"

#include "flecs.h"

#include <string_view>

namespace Mcc
{

    MCC_LIB_API std::string_view GetBlockName(flecs::entity e);

    MCC_LIB_API bool IsSolid(flecs::entity e);

}

#endif
