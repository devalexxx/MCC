// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_UTILS_CALL_WRAPPER_H
#define MCC_COMMON_UTILS_CALL_WRAPPER_H

namespace Mcc
{

    template<typename T>
    class RWCallWrapper
    {
      public:
        constexpr RWCallWrapper(T* ptr);
        constexpr ~RWCallWrapper();

        constexpr RWCallWrapper(const RWCallWrapper& other) = delete;
        constexpr RWCallWrapper& operator=(const RWCallWrapper& other) = delete;
        constexpr RWCallWrapper(RWCallWrapper&& other) noexcept;
        constexpr RWCallWrapper& operator=(RWCallWrapper&& other) noexcept;

        constexpr T* operator->();
        constexpr T& operator* ();
        constexpr const T* operator->() const;
        constexpr const T& operator* () const;

      private:
        T* mPtr;
    };

    template<typename T>
    class RWCallWrapper<std::shared_ptr<T>> : public RWCallWrapper<T>
    {
      public:
        constexpr RWCallWrapper(std::shared_ptr<T> ptr);

      private:
        std::shared_ptr<T> mShared;
    };

    template<typename T>
    class ROCallWrapper
    {
      public:
        constexpr ROCallWrapper(T* ptr);
        constexpr ~ROCallWrapper();

        constexpr ROCallWrapper(const ROCallWrapper& other);
        constexpr ROCallWrapper& operator=(const ROCallWrapper& other);
        constexpr ROCallWrapper(ROCallWrapper&& other) noexcept;
        constexpr ROCallWrapper& operator=(ROCallWrapper&& other) noexcept;

        constexpr T* operator->();
        constexpr T& operator* ();
        constexpr const T* operator->() const;
        constexpr const T& operator* () const;

      private:
        T* mPtr;
    };

    template<typename T>
    class ROCallWrapper<std::shared_ptr<T>> : public ROCallWrapper<T>
    {
    public:
        constexpr ROCallWrapper(std::shared_ptr<T> ptr);

    private:
        std::shared_ptr<T> mShared;
    };

}

#include "Common/Utils/CallWrapper.inl"

#endif
