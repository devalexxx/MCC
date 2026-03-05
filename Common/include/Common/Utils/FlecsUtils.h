// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_FLECS_UTILS_H
#define MCC_COMMON_UTILS_FLECS_UTILS_H

#include "flecs.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Mcc
{

    template<typename T, typename...>
    struct ComponentWrapper : T
    {
        using Type = T;

        using T::T;
        ComponentWrapper(const T& data) : T(data) {}
        ComponentWrapper(T&& data)      : T(data) {}
    };

    template<typename T, typename...>
    struct ComponentPtrWrapper
    {
        T* ptr;

        ComponentPtrWrapper()             : ptr(nullptr) {}
        ComponentPtrWrapper(T* ptr)       : ptr(ptr)     {}
        ComponentPtrWrapper(const T* ptr) : ptr(ptr)     {}

        explicit operator bool() const { return ptr != nullptr; }

        T* operator->() { return  ptr; }
        T& operator* () { return *ptr; }
        T* Get       () { return  ptr; }
        const T* operator->() const { return  ptr; }
        const T& operator* () const { return *ptr; }
        const T* Get       () const { return  ptr; }
    };

    void IgnoreIter(flecs::iter& it);

    template<typename T, typename C = std::vector<T>>
    flecs::opaque<C, T> VectorReflection(flecs::world& world);

    template<typename T, typename C = std::deque<T>>
    flecs::opaque<C, T> DequeReflection(flecs::world& world);

    template<typename T, typename C = std::unordered_set<T>>
    flecs::opaque<C, T> USetReflection(flecs::world& world);

    template<typename K, typename V, typename T = std::pair<const K, V>, typename C = std::unordered_map<K, V>>
    flecs::opaque<C, T> UMapReflection(flecs::world& world);


    template<typename>
    struct AutoRegister
    {
        static bool Register(const flecs::world& /* world */)
        {
            return false;
        }
    };

    template<typename T, typename... Ts>
    struct AutoRegister<ComponentWrapper<T, Ts...>>
    {
        static bool Register(const flecs::world& world, const char* name)
        {
            auto component = world.component<ComponentWrapper<T, Ts...>>(name);

            if (!AutoRegister<T>::Register(world))
                return false;

            component.template is_a<T>();
            return true;
        }
    };

    template<typename T>
    requires (!std::is_pointer_v<T>)
    struct AutoRegister<T>
    {
        static bool Register(const flecs::world& world)
        {
            return world.exists(world.entity<T>());
        }
    };

    template<typename T>
    requires (!std::is_pointer_v<T>)
    struct AutoRegister<std::vector<T>>
    {
        static bool Register(const flecs::world& world)
        {
            world.component<std::vector<T>>().opaque(VectorReflection<T>);
            return true;
        }
    };

    template<typename T>
    requires (!std::is_pointer_v<T>)
    struct AutoRegister<std::deque<T>>
    {
        static bool Register(const flecs::world& world)
        {
            world.component<std::deque<T>>().opaque(DequeReflection<T>);
            return true;
        }
    };

    template<typename T>
    requires (!std::is_pointer_v<T>)
    struct AutoRegister<std::unordered_set<T>>
    {
        static bool Register(const flecs::world& world)
        {
            world.component<std::unordered_set<T>>().opaque(USetReflection<T>);
            return true;
        }
    };

    template<typename K, typename V>
    requires (!std::is_pointer_v<K> && !std::is_pointer_v<V>)
    struct AutoRegister<std::unordered_map<K, V>>
    {
        static bool Register(const flecs::world& world)
        {
            world.component<std::unordered_map<K, V>>().opaque(UMapReflection<K, V>);
            return true;
        }
    };

}

#include "Common/Utils/FlecsUtils.inl"

#endif
