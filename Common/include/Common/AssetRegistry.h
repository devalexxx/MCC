// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_ASSET_REGISTRY_H
#define MCC_COMMON_ASSET_REGISTRY_H

#include <filesystem>
#include <unordered_map>

#include "Common/Utils/AssetResolver.h"

namespace Mcc
{

    struct Asset
    {
        virtual ~Asset() = default;
    };

    template<typename>
    struct AssetLoader {};

    class AssetRegistry;

    template<typename T>
    concept IsAssetLoadable = requires(AssetRegistry& a, std::string_view b, bool c, AssetLoader<T> loader)
    {
        loader(a, b, c);
    };

    class AssetRegistry : public AssetResolver
    {
      public:
        using AssetResolver::AssetResolver;

        template<typename T, bool IsLoadable = IsAssetLoadable<T>>
        std::shared_ptr<T>     Get(std::string_view path, bool cache = true);
        std::shared_ptr<Asset> Get(std::string_view path, bool warn  = true);

        template<typename T, typename... Args>
        std::shared_ptr<T>     Add(std::string path, Args&&... args);
        std::shared_ptr<Asset> Add(std::string path, std::shared_ptr<Asset> asset);

      private:
        std::unordered_map<std::string, std::shared_ptr<Asset>, StringHash, std::equal_to<>> mCache;
    };

}

#include "Common/AssetRegistry.inl"

#endif
