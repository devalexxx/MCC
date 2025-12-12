// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_EVENT_MANAGER_H
#define MCC_COMMON_UTILS_EVENT_MANAGER_H

#include "Hexis/Core/Types.h"

#include <functional>
#include <vector>

namespace Mcc
{

    template<typename Tag>
    struct BaseEvent;

    using EventHandlerID = std::uint64_t;

    template<typename Tag>
    class EventManager
    {
      public:
        template<typename T>
        using EventHandler        = std::function<void(const T&)>;
        using GenericEventHandler = std::function<void(const BaseEvent<Tag>&)>;

        template<typename T>
        void Dispatch(T event);

        template<typename T, typename F, typename... Args>
        EventHandlerID Subscribe(F&& handler, Args&&... args);

        void Withdraw(EventHandlerID id);

      protected:
        std::vector<std::vector<GenericEventHandler>> mEventHandlers;

      private:
        template<typename T>
        struct HandlerFunctor
        {
            EventHandler<T> handler;

            void operator()(const BaseEvent<Tag>& event);
        };

        static std::pair<std::uint32_t, std::uint32_t> ExtractHandlerInfo(EventHandlerID id);
        static EventHandlerID                          BuildHandlerInfo(uint32_t typeId, uint32_t handlerId);
    };

}

#include <Common/Utils/EventManager.inl>

#endif