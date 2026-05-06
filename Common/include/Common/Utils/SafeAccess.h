// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_SAFE_ACCESS_H
#define MCC_COMMON_UTILS_SAFE_ACCESS_H

#include <mutex>

namespace Mcc
{
    // Source: https://ngathanasiou.wordpress.com/2021/08/09/a-c-locking-wrapper/

    template<typename U>
    class CallProxy
    {
      public:
        CallProxy(U* ptr, std::recursive_mutex& mutex);
        CallProxy(const CallProxy& other);
        ~CallProxy();

        CallProxy& operator=(const CallProxy&) = delete;

        U* operator->();
        U& operator*();
        const U* operator->() const;
        const U& operator* () const;

      private:
        U*                    mPtr;
        std::recursive_mutex& mMutex;
        mutable bool          mOwn;
    };

    template<typename T>
    class SafeAccess
    {
      public:
        template<typename... Args>
        explicit SafeAccess(Args&&... args);

        SafeAccess() requires std::is_default_constructible_v<T>
        = default;

        CallProxy<T> operator->();
        CallProxy<T> operator*();

      private:
        T                            mData;
        mutable std::recursive_mutex mMutex;
    };

}

#include "Common/Utils/SafeAccess.inl"

#endif
