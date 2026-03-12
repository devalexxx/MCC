// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename U>
    CallProxy<U>::CallProxy(U* ptr, std::recursive_mutex& mutex) : mPtr(ptr), mMutex(mutex), mOwn(true)
    {}

    template<typename U>
    CallProxy<U>::CallProxy(const CallProxy& other) : mPtr(other.mPtr), mMutex(other.mMutex), mOwn(true)
    {
        other.mOwn = false;
    }

    template<typename U>
    CallProxy<U>::~CallProxy()
    {
        if (mOwn)
        {
            mMutex.unlock();
        }
    }

    template<typename U>
    U* CallProxy<U>::operator->()
    {
        return mPtr;
    }

    template<typename T>
    template<typename... Args>
    SafeAccess<T>::SafeAccess(Args&&... args) : mData(std::forward<Args>(args)...)
    {}

    template<typename T>
    CallProxy<T> SafeAccess<T>::operator->()
    {
        mMutex.lock();
        return CallProxy<T>(&mData, mMutex);
    }

    template<typename T>
    CallProxy<T> SafeAccess<T>::operator*()
    {
        mMutex.lock();
        return CallProxy<T>(&mData, mMutex);
    }

}
