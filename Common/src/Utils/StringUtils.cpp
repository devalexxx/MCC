// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/StringUtils.h"

namespace Mcc
{

    size_t StringHash::operator()(const char* txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }

    size_t StringHash::operator()(const std::string_view txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }

    size_t StringHash::operator()(const std::string& txt) const
    {
        return std::hash<std::string>{}(txt);
    }

}
