// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/Logging.h"

#include "Hexis/Core/TypeName.h"

namespace Mcc
{

    template<typename T, bool IsLoadable>
    std::shared_ptr<T> AssetRegistry::Get(std::string_view path, bool cache)
    {
        if (const auto asset = Get(path, false))
        {
            if (auto casted = std::dynamic_pointer_cast<T>(asset); casted)
            {
                return casted;
            }

            MCC_LOG_ERROR("[AssetRegistry] Asset {} is not of type {}", path, Hx::TypeName<T>());
            return nullptr;
        }

        if (const auto protocol = GetProtocol(path); protocol)
        {
            if (*protocol == "rt")
            {
                MCC_LOG_ERROR("[AssetRegistry] Asset {} does not exist", path);
                return nullptr;
            }
        }

        if constexpr (IsLoadable)
        {
            return AssetLoader<T>{}(*this, path, cache);
        }

        return nullptr;
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> AssetRegistry::Add(std::string path, Args&&... args)
    {
        return std::dynamic_pointer_cast<T>(Add(std::move(path), std::make_shared<T>(std::forward<Args>(args)...)));
    }

}
