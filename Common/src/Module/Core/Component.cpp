// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Module/Core/Component.h"

namespace Mcc
{

    CTransform CTransform::Identity()
    {
        return {
            .position={ 0.f, 0.f, 0.f },
            .rotation={ 0.f, 0.f, 0.f, 0.f },
            .scale   ={ 1.f, 1.f, 1.f }
        };
    }

}
