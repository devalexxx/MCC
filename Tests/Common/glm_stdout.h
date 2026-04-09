// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include <fmt/format.h>

#include "Common/Utils/GLMUtils.h"

static std::ostream &operator<<(std::ostream &os, const glm::vec3 & v) 
{
    return os << fmt::format("{}", v);
}

static std::ostream &operator<<(std::ostream &os, const glm::uvec3 & v) 
{
    return os << fmt::format("{}", v);
}

static std::ostream &operator<<(std::ostream &os, const glm::ivec2 & v) 
{
    return os << fmt::format("{}", v);
}