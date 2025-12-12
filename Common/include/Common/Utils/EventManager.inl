// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/Assert.h"
#include "Common/Utils/Event.h"

#include <fmt/base.h>

#include "Logging.h"

namespace Mcc
{

    template<typename Tag>
    template<typename T>
    void EventManager<Tag>::Dispatch(T event)
    {
        Event<Tag, T> wrapper { {}, std::move(event) };
        auto          type = decltype(wrapper)::GetType();

        if (type >= mEventHandlers.size())
        {
            mEventHandlers.push_back({});
            return;
        }

        for (auto& handler: mEventHandlers[type])
        {
            if (handler)
            {
                handler(wrapper);
            }
        }
    }

    template<typename Tag>
    template<typename T, typename F, typename... Args>
    EventHandlerID EventManager<Tag>::Subscribe(F&& handler, Args&&... args)
    {
        auto typeId = Event<Tag, T>::GetType();
        auto bound  = std::bind(std::forward<F>(handler), std::placeholders::_1, std::forward<Args>(args)...);

        if (mEventHandlers.size() <= typeId)
        {
            mEventHandlers.push_back({});
        }


        Hx::u64 handlerId = 0;
        for (Hx::u64 i = 0; i < mEventHandlers[typeId].size(); ++i)
        {
            if (!mEventHandlers[typeId][i])
            {
                mEventHandlers[typeId][i] = HandlerFunctor<T> { std::move(bound) };
                handlerId                 = i + 1;
            }
        }
        if (handlerId == 0)
        {
            mEventHandlers[typeId].push_back(HandlerFunctor<T> { std::move(bound) });
            handlerId = mEventHandlers[typeId].size();
        }

        return BuildHandlerInfo(typeId, handlerId);
    }

    template<typename Tag>
    void EventManager<Tag>::Withdraw(const EventHandlerID id)
    {
        MCC_ASSERT(id != 0, "Trying to withdraw an invalid event handler");

        auto [typeId, handlerId] = ExtractHandlerInfo(id);
        MCC_ASSERT(mEventHandlers.size() > typeId, "Trying to withdraw an invalid event handler");
        MCC_ASSERT(mEventHandlers[typeId].size() > handlerId, "Trying to withdraw an invalid event handler");

        mEventHandlers[typeId][handlerId] = {};
    }

    template<typename Tag>
    template<typename T>
    void EventManager<Tag>::HandlerFunctor<T>::operator()(const BaseEvent<Tag>& event)
    {
        handler(static_cast<const Event<Tag, T>&>(event).event);
    }

    template<typename Tag>
    std::pair<std::uint32_t, std::uint32_t> EventManager<Tag>::ExtractHandlerInfo(const EventHandlerID id)
    {
        auto typeId    = (id >> 32) & 0x00000000ffffffff;
        auto handlerId = (id & 0x00000000ffffffff) - 1;

        return { typeId, handlerId };
    }

    template<typename Tag>
    EventHandlerID EventManager<Tag>::BuildHandlerInfo(const uint32_t typeId, const uint32_t handlerId)
    {
        MCC_ASSERT(!(typeId & 0xffffffff00000000), "Type id is too big.");
        MCC_ASSERT(!(handlerId & 0xffffffff00000000), "Handler id is too big.");

        const uint64_t wrap = typeId;
        return (wrap << 32) + handlerId;
    }

}