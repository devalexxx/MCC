// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/CommandLineStore.h"

namespace Mcc
{

    CommandLineStore::CommandLineStore(const int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            if (std::string_view arg(argv[i]); arg[0] == '-')
            {
                int offset = 1;
                if (arg[1] == '-')
                {
                    offset++;
                }

                arg = arg.substr(offset);

                if (i < argc - 1)
                {
                    if (argv[i + 1][0] != '-')
                    {
                        std::string val(argv[i + 1]);
                        mParameters.emplace(arg, std::move(val));
                        i++;
                    }
                    else
                    {
                        mFlags.emplace(arg);
                    }
                }
                else
                {
                    mFlags.emplace(arg);
                }
            }
        }
    }

    std::optional<std::string> CommandLineStore::GetParameter(const std::string_view name) const
    {
        if (const auto it = mParameters.find(name); it != mParameters.cend())
        {
            return it->second;
        }

        return std::nullopt;
    }

    bool CommandLineStore::HasFlag(const std::string_view name) const
    {
        return mFlags.contains(name);
    }

}