// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_STRING_UTILS_H
#define MCC_COMMON_UTILS_STRING_UTILS_H

#include <string>
#include <string_view>

namespace Mcc
{

    struct StringHash
    {
        using is_transparent = void;

        size_t operator()(const char *txt) const;
        size_t operator()(std::string_view txt) const;
        size_t operator()(const std::string &txt) const;
    };

}

#endif
