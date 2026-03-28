// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Application.h"

#include "Common/Utils/Logging.h"

#include <charconv>
#include <csignal>
#include <thread>
#include <tuple>

namespace Mcc
{

#ifdef MCC_POSIX
    static void* SigHandler(void* args)
    {
        auto [set, world] = *static_cast<std::tuple<sigset_t, flecs::world>*>(args);
        int sig;
        sigwait(&set, &sig);
        if (sig == SIGINT || sig == SIGABRT)
        {
            world.quit();
        }
        return nullptr;
    }
#endif

    Application::Application(const int argc, char** argv) :
        mCmdLineStore(argc, argv)
#ifdef MCC_POSIX
        ,
        mArgs({}, mWorld)
#endif
    {
#if defined(MCC_DEBUG) or defined(MCC_RELEASE_DEBUG)
        mWorld.import <flecs::stats>();
        if (const auto param = mCmdLineStore.GetParameter("fport"); param)
        {
            flecs::Rest config;
            std::from_chars(param->data(), param->data() + param->size(), config.port);
            mWorld.set<flecs::Rest>(config);
        }
        else
        {
            mWorld.set<flecs::Rest>({});
        }
#endif

#ifdef MCC_POSIX
        sigset_t  set;
        pthread_t thread;

        sigemptyset(&set);
        sigaddset(&set, SIGINT);
        sigaddset(&set, SIGABRT);
        sigprocmask(SIG_BLOCK, &set, nullptr);

        std::get<sigset_t>(mArgs) = set;
        pthread_create(&thread, nullptr, SigHandler, &mArgs);
#endif
    }

}