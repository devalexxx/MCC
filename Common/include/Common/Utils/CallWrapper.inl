// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename T>
    constexpr RWCallWrapper<T>::RWCallWrapper(T* ptr) : mPtr(ptr)
    {
        mPtr->LockReadWrite();
    }

    template<typename T>
    constexpr RWCallWrapper<T>::~RWCallWrapper()
    {
        if (mPtr) mPtr->UnlockReadWrite();
    }

    template<typename T>
    constexpr RWCallWrapper<T>::RWCallWrapper(RWCallWrapper&& other) noexcept
    {
        mPtr       = other.mPtr;
        other.mPtr = nullptr;
    }

    template<typename T>
    constexpr RWCallWrapper<T>& RWCallWrapper<T>::operator=(RWCallWrapper&& other) noexcept
    {
        mPtr       = other.mPtr;
        other.mPtr = nullptr;
        return this;
    }

    template<typename T>
    constexpr T* RWCallWrapper<T>::operator->()
    {
        return mPtr;
    }

    template<typename T>
    constexpr T& RWCallWrapper<T>::operator*()
    {
        return *mPtr;
    }

    template<typename T>
    constexpr const T* RWCallWrapper<T>::operator->() const
    {
        return mPtr;
    }

    template<typename T>
    constexpr const T& RWCallWrapper<T>::operator*() const
    {
        return *mPtr;
    }

    template<typename T>
    constexpr RWCallWrapper<std::shared_ptr<T>>::RWCallWrapper(std::shared_ptr<T> ptr) :
        RWCallWrapper<T>(ptr.get()),
        mShared(std::move(ptr))
    {}

    template<typename T>
    constexpr ROCallWrapper<T>::ROCallWrapper(T* ptr) : mPtr(ptr)
    {
        mPtr->LockReadOnly();
    }

    template<typename T>
    constexpr ROCallWrapper<T>::~ROCallWrapper()
    {
        if (mPtr) mPtr->UnlockReadOnly();
    }

    template<typename T>
    constexpr ROCallWrapper<T>::ROCallWrapper(const ROCallWrapper& other)
    {
        mPtr = other.mPtr;
        mPtr->LockReadOnly();
    }

    template<typename T>
    constexpr ROCallWrapper<T>& ROCallWrapper<T>::operator=(const ROCallWrapper& other)
    {
        mPtr = other.mPtr;
        mPtr->LockReadOnly();
        return this;
    }

    template<typename T>
    constexpr ROCallWrapper<T>::ROCallWrapper(ROCallWrapper&& other) noexcept
    {
        mPtr       = other.mPtr;
        other.mPtr = nullptr;
    }

    template<typename T>
    constexpr ROCallWrapper<T>& ROCallWrapper<T>::operator=(ROCallWrapper&& other) noexcept
    {
        mPtr       = other.mPtr;
        other.mPtr = nullptr;
        return this;
    }

    template<typename T>
    constexpr T* ROCallWrapper<T>::operator->()
    {
        return mPtr;
    }

    template<typename T>
    constexpr T& ROCallWrapper<T>::operator*()
    {
        return *mPtr;
    }

    template<typename T>
    constexpr const T* ROCallWrapper<T>::operator->() const
    {
        return mPtr;
    }

    template<typename T>
    constexpr const T& ROCallWrapper<T>::operator*() const
    {
        return *mPtr;
    }

    template<typename T>
    constexpr ROCallWrapper<std::shared_ptr<T>>::ROCallWrapper(std::shared_ptr<T> ptr) :
        ROCallWrapper<T>(ptr.get()),
        mShared(std::move(ptr))
    {}

}
