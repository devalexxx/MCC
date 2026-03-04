// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename S, typename T>
    void StateValue<S, T>::Register(const flecs::world& world)
    {
        world.component<T>().add(flecs::Target).template child_of<S>();
    }

    template<typename S, typename T>
    bool StateValue<S, T>::IsActive(const auto& obj)
    {
        return S::template IsActive<T>(obj);
    }

    template<typename S, typename T>
    void StateValue<S, T>::Enter(const auto& obj)
    {
        S::template Enter<T>(obj);
    }

    template<typename S, typename T>
    O<S> StateValue<S, T>::OnEnter(const auto& obj)
    {
        return S::template OnEnter<T>(obj);
    }

    template<typename S, typename T>
    O<S> StateValue<S, T>::OnExit(const auto& obj)
    {
        return S::template OnExit<T>(obj);
    }

    template<typename T>
    template<typename... Ts>
    void BaseState<T>::Register(const flecs::world& world)
    {
        world.component<T>().add(flecs::Exclusive);
        (Ts::Register(world), ...);
    }

    template<typename T>
    template<typename SV>
    bool BaseState<T>::IsActive(const auto& obj)
    {
        return obj.template has<T, SV>();
    }

    template<typename T>
    template<typename SV>
    void BaseState<T>::Enter(const auto& obj)
    {
        if (BaseState::IsActive<SV>(obj))
            return;
        obj.template add<T, SV>();
    }

    template<typename T>
    template<typename SV>
    O<T> BaseState<T>::OnEnter(const flecs::world& world)
    {
        return world.observer<T>().event(flecs::OnAdd).template second<SV>();
    }

    template<typename T>
    template<typename SV>
    O<T> BaseState<T>::OnExit(const flecs::world& world)
    {
        return world.observer<T>().event(flecs::OnRemove).template second<SV>();
    }

    template<typename T>
    template<typename SV>
    O<T> State<StateTargetEntity, T>::OnEnter(const flecs::entity& entity)
    {
        return BaseState<T>::template OnEnter<SV>(entity.world()).src(entity);
    }

    template<typename T>
    template<typename SV>
    O<T> State<StateTargetEntity, T>::OnExit(const flecs::entity& entity)
    {
        return BaseState<T>::template OnExit<SV>(entity.world()).src(entity);
    }

    struct SceneRoot;

    template<typename T>
    template<typename SV>
    bool State<StateTargetScene, T>::IsActive(const flecs::world& world)
    {
        return State<StateTargetEntity, T>::template IsActive<SV>(world.entity<SceneRoot>());
    }

    template<typename T>
    template<typename SV>
    void State<StateTargetScene, T>::Enter(const flecs::world& world)
    {
        State<StateTargetEntity, T>::template Enter<SV>(world.entity<SceneRoot>());
    }

    template<typename T>
    template<typename SV>
    O<T> State<StateTargetScene, T>::OnEnter(const flecs::world& world)
    {
        return State<StateTargetEntity, T>::template OnEnter<SV>(world.entity<SceneRoot>());
    }

    template<typename T>
    template<typename SV>
    O<T> State<StateTargetScene, T>::OnExit(const flecs::world& world)
    {
        return State<StateTargetEntity, T>::template OnExit<SV>(world.entity<SceneRoot>());
    }

}
