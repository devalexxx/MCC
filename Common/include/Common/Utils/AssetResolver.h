// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_ASSET_RESOLVER_H
#define MCC_ASSET_RESOLVER_H

#include "Common/Utils/StringUtils.h"

#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace Mcc
{

    class AssetResolver
    {
      public:
        using ProtocolMapping = std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>;

        AssetResolver(ProtocolMapping mapping, std::filesystem::path basePath);

        void SetBasePath(std::filesystem::path basePath);

        const std::filesystem::path&         GetBasePath() const;
        std::optional<std::filesystem::path> Resolve    (std::string_view path) const;

        static std::optional<std::string_view> GetProtocol(std::string_view path);

      private:
        ProtocolMapping       mProtocolToFolder;
        std::filesystem::path mBasePath;
    };

}

#endif