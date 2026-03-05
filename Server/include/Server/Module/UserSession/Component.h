// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_SERVER_MODULE_USER_SESSION_COMPONENT_H
#define MCC_SERVER_MODULE_USER_SESSION_COMPONENT_H

#include "Common/Utils/FlecsUtils.h"

namespace Mcc
{

    namespace _ { struct UserSessionModuleTag {}; };
    using CUserSession = ComponentPtrWrapper<UserSession, _::UserSessionModuleTag>;

}

#endif
