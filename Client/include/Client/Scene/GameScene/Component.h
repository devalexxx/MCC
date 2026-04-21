// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_SCENE_GAME_SCENE_COMPONENT_H
#define MCC_CLIENT_SCENE_GAME_SCENE_COMPONENT_H

#include "Common/Utils/Logging.h"

#include <Hexis/Core/LambdaAsFuncPtr.h>
#include <Hexis/Math/FloatingPoint.h>

#include <algorithm>
#include <vector>

#include <imgui.h>

namespace Mcc
{

    using HistoricalDataIndexer = float(*)(void*, int);

    template<typename T>
    constexpr HistoricalDataIndexer CreateIndexer();

    template<typename T>
    struct HistoricalData
    {
        constexpr HistoricalData(size_t size, const float step) :
            history(std::vector(size, T{})),
            size(size),
            index(0),
            accumulator(0),
            elapsed(0),
            step(step)
        {}

        std::vector<T> history;
        T              accumulator;
        T              max;
        T              mean;

        size_t size;
        size_t index;

        float elapsed;
        float step;

        constexpr void Plot(const char* title, const char* unit, const char* align, const char* extra)
        {
            ImGui::PlotLines(
                fmt::format("{}\nmean{}: {:.2f} {}\nmax {}: {:.2f} {}{}", title, align, mean, unit, align, max, unit, extra).c_str(),
                CreateIndexer<float>(),
                this,
                size,
                0, nullptr, 0.0f, max, ImVec2(240, 60)
            );
        }

        constexpr void Add(T value, const float dt)
        {
            accumulator += value;
            elapsed     += dt;

            if (elapsed >= step)
            {
                const auto last = history[index];
                const auto curr = accumulator / elapsed;

                max  = std::max(max, curr);
                mean = (mean * (size - 1) + curr) / size;

                if (Hx::Equal(last, max) && !Hx::Equal(curr, max))
                {
                    max = *std::ranges::max_element(history);
                }

                history[index] = accumulator / elapsed;
                accumulator    = 0;
                elapsed        = 0;
                index          = (index + 1) % size;
            }
        }
    };

    template<typename T>
    constexpr HistoricalDataIndexer CreateIndexer()
    {
        static HistoricalDataIndexer indexer = [](void* data, int i) -> float
        {
            auto casted = static_cast<HistoricalData<T>*>(data);
            auto index  = (casted->index + i) % casted->size;
            return static_cast<float>(casted->history[index]);
        };

        return Hx::LambdaAsFuncPtr<HistoricalDataIndexer>(indexer);
    }

    template<typename T1, typename T2>
    constexpr void Add(HistoricalData<T1>& hd, T2&& value)
    {
        hd.history[hd.historyIndex] = static_cast<T1>(value);
        hd.historyIndex = (hd.historyIndex + 1) % hd.historySize;
    }

    struct DebugContext
    {
        std::vector<float> fpsHistory;
        size_t             fpsHistorySize;
        size_t             fpsHistoryIndex;

        HistoricalData<float> incomingBandwidthHistory;
        HistoricalData<float> outgoingBandwidthHistory;
        double                incomingDataTotal;
        double                outgoingDataTotal;
    };

}

#endif
