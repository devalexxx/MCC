// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/AssetRegistry.h"

namespace Mcc
{

    std::shared_ptr<Asset> AssetRegistry::Get(std::string_view path, const bool warn)
    {
        const auto it = mCache.find(path);
        if (it == mCache.end())
        {
            if (warn)
            {
                MCC_LOG_WARN(
                    "[AssetRegistry] Failed to retrieve asset at {}\n\tGeneric asset should be cached to be retrieved",
                    path
                );
            }
            return nullptr;
        }


        return it->second;
    }

    std::shared_ptr<Asset> AssetRegistry::Add(std::string path, std::shared_ptr<Asset> asset)
    {
        auto [it, inserted] = mCache.emplace(std::move(path), std::move(asset));
        if (!inserted)
        {
            MCC_LOG_WARN("[AssetRegistry] Asset {} intended to be added, but already exist", it->first);
        }

        return it->second;
    }

}
