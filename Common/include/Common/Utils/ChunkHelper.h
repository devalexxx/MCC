// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_CHUNK_HELPER_H
#define MCC_COMMON_UTILS_CHUNK_HELPER_H

#include <Common/Module/Entity/Component.h>

#include <glm/glm.hpp>

#include <functional>

namespace Mcc::Helper
{

    MCC_LIB_API bool IsInCircle(const glm::ivec2& c, const glm::ivec2& p, long radius);
    MCC_LIB_API bool IsInSphere(const glm::ivec3& c, const glm::ivec3& p, long radius);

    MCC_LIB_API void ForInCircle(long x, long y, int radius, std::function<void(long x, long y)>&& fn);
    MCC_LIB_API void ForInSphere(long x, long y, long z, int radius, std::function<void(long x, long y, long z)>&& fn);

}

#endif
