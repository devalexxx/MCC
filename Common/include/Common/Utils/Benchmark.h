// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_BENCHMARK_H
#define MCC_COMMON_UTILS_BENCHMARK_H

#include "Common/Utils/Logging.h"

#include <chrono>
#include <mutex>

#if defined(MCC_DEBUG) || defined(MCC_BENCH)
static_assert(MCC_LOG_LEVEL >= MCC_DEBUG_LEVEL);
#    define MCC_BENCH_TIME(NAME, CALLABLE)                                                                 \
        [&]() {                                                                                            \
            struct __Tag##__LINE__                                                                         \
            {                                                                                              \
                static constexpr const char* name()                                                        \
                {                                                                                          \
                    return #NAME;                                                                          \
                }                                                                                          \
            };                                                                                             \
            static Mcc::_::TimeBenchmark<__Tag##__LINE__> __benchmark##__LINE__;                           \
            return [=](auto&&... args) -> decltype(auto) {                                                 \
                const auto __start = _::BenchClock::now();                                                 \
                if constexpr (std::is_void_v<decltype(CALLABLE(std::declval<decltype(args)>()...))>)       \
                {                                                                                          \
                    CALLABLE(std::forward<decltype(args)>(args)...);                                       \
                    const auto __end      = _::BenchClock::now();                                          \
                    float      __duration = std::chrono::duration<float>(__end - __start).count();         \
                    __benchmark##__LINE__.AddCall(__duration);                                             \
                }                                                                                          \
                else                                                                                       \
                {                                                                                          \
                    auto       __result   = CALLABLE(std::forward<decltype(args)>(args)...);               \
                    const auto __end      = _::BenchClock::now();                                          \
                    float      __duration = std::chrono::duration<float>(__end - __start).count();         \
                    __benchmark##__LINE__.AddCall(__duration);                                             \
                    return __result;                                                                       \
                }                                                                                          \
            };                                                                                             \
        }()
#else
#    define MCC_BENCH_TIME(NAME, CALLABLE) CALLABLE
#endif

namespace Mcc::_
{

    using BenchClock = std::chrono::steady_clock;

    template<typename Tag>
    struct TimeBenchmark
    {
        ~TimeBenchmark();

        void AddCall(float elapsed);

        std::mutex mutex {};
        float      timeSpent {};
        size_t     callCount {};
    };

}

#include "Common/Utils/Benchmar.inl"

#endif