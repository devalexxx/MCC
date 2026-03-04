// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/ServerSession/System.h"

#include "Common/SceneImporter.h"

namespace Mcc
{

    void SetupSessionStateSystem(const flecs::iter& it)
    {
        SrvConnState::Disconnected::Enter(it.world());
        SessionState::Inactive    ::Enter(it.world());
    }

    void HandleConnectionResultSystem(const flecs::iter& it, size_t, SrvConnResult& result)
    {
        const auto world = it.world();
        switch (result.GetState())
        {
            case Hx::TaskState::Cancelled:
                SrvConnState::Failed::Enter(world);
                break;
            case Hx::TaskState::Done:
                if (!result.GetResult()->get())
                {
                    SrvConnState::Failed::Enter(world);
                    break;
                }

                SrvConnState::Connected::Enter(world);
                break;
            default:
                return;
        }
        world.remove<SrvConnResult>();
    }

    void HandleDisconnectionResultSystem(const flecs::iter& it, size_t, SrvDConnResult& result)
    {
        const auto world = it.world();
        switch (result.GetState())
        {
            case Hx::TaskState::Cancelled:
                SrvConnState::Failed::Enter(world);
                break;
            case Hx::TaskState::Done:
                if (!result.GetResult()->get())
                {
                    SrvConnState::Failed::Enter(world);
                    break;
                }

                SrvConnState::Disconnected::Enter(world);
                break;
            default:
                return;
        }
        world.remove<SrvDConnResult>();
    }

}
