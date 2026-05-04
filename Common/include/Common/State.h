// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_STATE_H
#define MCC_COMMON_STATE_H

#define EXPAND(x) x
#define MAP_1_0(f)
#define MAP_1_1(f, x, y) f(x, y)
#define MAP_1_2(f, x, y, ...) f(x, y) EXPAND(MAP_1_1(f, x, __VA_ARGS__))
#define MAP_1_3(f, x, y, ...) f(x, y) EXPAND(MAP_1_2(f, x, __VA_ARGS__))
#define MAP_1_4(f, x, y, ...) f(x, y) EXPAND(MAP_1_3(f, x, __VA_ARGS__))

#define GET_MAP_1(_1_1, _1_2, _1_3, _1_4, NAME, ...) NAME
#define MAP_1(f, x, ...) EXPAND(GET_MAP_1(__VA_ARGS__, MAP_1_4, MAP_1_3, MAP_1_2, MAP_1_1)(f, x, __VA_ARGS__))

#define MAKE_STATE_VALUE(S, T)  \
    struct T : StateValue<S, T> \
    {};

#define REGISTER_STATE(Scope, S, ...)                 \
    struct S : public State<Scope, S>                   \
    {                                                   \
        MAP_1(MAKE_STATE_VALUE, S, __VA_ARGS__)                  \
        static void Register(const flecs::world& world) \
        {                                               \
            State::Register<__VA_ARGS__>(world);                 \
        }                                               \
    };

#include <flecs.h>

namespace Mcc
{

    template<typename>
    class BaseState;

    template<typename, typename>
    struct State;

    struct StateTargetGlobal
    {};
    struct StateTargetEntity
    {};
    struct StateTargetScene
    {};

    template<typename C>
    using O = flecs::observer_builder<C>;

    template<typename S, typename>
    struct StateValue
    {
      private:
        static void Register(const flecs::world& world);
        friend class BaseState<S>;

      public:
        static bool IsActive(const auto& obj);
        static void Enter(const auto& obj);

        static O<S> OnEnter(const auto& obj);
        static O<S> OnExit(const auto& obj);
    };

    template<typename T>
    class BaseState
    {
      protected:
        template<typename... Ts>
        static void Register(const flecs::world& world);

      public:
        template<typename SV>
        static bool IsActive(const auto& obj);
        template<typename SV>
        static void Enter(const auto& obj);

        template<typename SV>
        static O<T> OnEnter(const flecs::world& world);
        template<typename SV>
        static O<T> OnExit(const flecs::world& world);
    };

    template<typename, typename>
    struct State
    {};

    template<typename T>
    struct State<StateTargetGlobal, T> : BaseState<T>
    {};

    template<typename T>
    struct State<StateTargetEntity, T> : BaseState<T>
    {
        using BaseState<T>::OnEnter;
        using BaseState<T>::OnExit;

        template<typename SV>
        static O<T> OnEnter(const flecs::entity& entity);
        template<typename SV>
        static O<T> OnExit(const flecs::entity& entity);
    };

    template<typename T>
    struct State<StateTargetScene, T> : BaseState<T>
    {
        template<typename SV>
        static bool IsActive(const flecs::world& world);
        template<typename SV>
        static void Enter(const flecs::world& world);

        template<typename SV>
        static O<T> OnEnter(const flecs::world& world);
        template<typename SV>
        static O<T> OnExit(const flecs::world& world);
    };

}

#include "Common/State.inl"

#endif
