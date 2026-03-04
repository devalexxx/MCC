// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/FlecsUtils.h"

namespace Mcc
{

    void IgnoreIter(flecs::iter& it)
    {
        while (it.next()) {}
    }

}
