// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Utils/AssetResolver.h"

#include "Common/Utils/Assert.h"
#include "Common/Utils/Logging.h"

#include <algorithm>
#include <regex>

namespace Mcc
{

    AssetResolver::AssetResolver(ProtocolMapping mapping, std::filesystem::path basePath) :
        mProtocolToFolder(std::move(mapping)),
        mBasePath(std::move(basePath))
    {}

    void AssetResolver::SetBasePath(std::filesystem::path basePath)
    {
        mBasePath = std::move(basePath);
    }

    const std::filesystem::path& AssetResolver::GetBasePath() const
    {
        return mBasePath;
    }

    std::optional<std::filesystem::path> AssetResolver::Resolve(const std::string_view path) const
    {
        std::filesystem::path result = path;

        // Process protocol path
        if (const auto delim = path.find_first_of("://"); delim != std::string_view::npos)
        {
            const auto protocol = path.substr(0, delim);
            if (const auto it = mProtocolToFolder.find(protocol); it != mProtocolToFolder.end())
            {
                result = mBasePath / it->second / path.substr(delim + 3);
            }
            else
            {
                MCC_LOG_ERROR("[AssetResolver] Unknown protocol: {}", protocol);
                return std::nullopt;
            }
        }

        // Check if it is a valid path
        if (!std::filesystem::exists(result))
        {
            MCC_LOG_ERROR("[AssetResolver] Asset not found at {}", result.c_str());
            return std::nullopt;
        }

        return result;
    }

    std::optional<std::string_view> AssetResolver::GetProtocol(const std::string_view path)
    {
        if (const auto delim = path.find_first_of("://"); delim != std::string_view::npos)
        {
            return path.substr(0, delim);
        }

        return std::nullopt;
    }

}