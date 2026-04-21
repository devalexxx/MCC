// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_PHASE_H
#define MCC_COMMON_PHASE_H

namespace Mcc
{

    struct Phase
    {
        struct OnLoad {};

        struct OnSetup     {};
        struct OnPoll      {};
        struct PreUpdate   {};
        struct OnUpdate    {};
        struct PostUpdate  {};
        struct OnDrawGui   {};
        struct PostDrawGui {};
        struct OnClear     {};
        struct OnDraw      {};
        struct PreRender   {};
        struct OnRender    {};

        struct OnQuit {};
    };

}

#endif
