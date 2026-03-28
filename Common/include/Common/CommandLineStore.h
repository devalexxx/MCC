// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_COMMON_COMMAND_LINE_STORE_H
#define MCC_COMMON_COMMAND_LINE_STORE_H

#include "Common/Export.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Utils/StringUtils.h"

namespace Mcc
{

    class MCC_LIB_API CommandLineStore
    {
      public:
        CommandLineStore(int argc, char** argv);

        std::optional<std::string> GetParameter(std::string_view name) const;
        bool                       HasFlag     (std::string_view name) const;

      private:
        std::unordered_map<std::string, std::string, StringHash, std::equal_to<>> mParameters;
        std::unordered_set<std::string, StringHash, std::equal_to<>>              mFlags;
    };

}

#endif
