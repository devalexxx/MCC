// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "glm_stdout.h"
#include "doctest/doctest.h"

#include "Common/World/Position.h"

namespace Mcc
{

    template<typename S, typename C>
    static std::ostream& operator<<(std::ostream& os, const Position<S, C>& p)
    {
        return os << format_as(p);
    }

}

TEST_SUITE("Position")
{

    // Test both WorldSpace and LocalSpace
    TEST_CASE("WorldPosEToWorldPosV")
    {
        using namespace Mcc;

        std::vector<std::tuple<glm::vec3, glm::uvec3>> data {
            { {        0, 30.f / 31.f, 0}, { 0, 30, 0} },
            { {        0,           0, 0}, { 0,  0, 0} },
            { {30.f /31.f,          0, 0}, {30,  0, 0} },
            { { 1.f /31.f,          0, 0}, { 1,  0, 0} },
            { {30.5f/31.f,          0, 0}, {30,  0, 0} },
        };

        for (auto& [ a, except ] : data)
        {
            WorldPosE p({0, 0, 0}, a);
            WorldPosV g = p;

            auto [ cc, ci ] = g;
            glm::uvec3 vci = ci;

            CHECK_EQ(vci, except);
        }
    }

    // Test both WorldSpace and LocalSpace
    TEST_CASE("WorldPosVToWorldPosE")
    {
        using namespace Mcc;

        std::vector<std::tuple<glm::uvec3, glm::fvec3>> data {
            { { 0, 30, 0}, {        0, 30.f / 31.f, 0} },
            { { 0,  0, 0}, {        0,           0, 0} },
            { {30,  0, 0}, {30.f/31.f,           0, 0} },
            { { 1,  0, 0}, { 1.f/31.f,           0, 0} },
        };

        for (auto& [ a, except ] : data)
        {
            WorldPosV p({0, 0, 0}, a);
            WorldPosE g = p;

            auto [ cc, ci ] = g;
            glm::fvec3 vci = ci;

            CHECK_EQ(vci, except);
        }
    }

    TEST_CASE("FloatCoordToVoxelCoord")
    {
        using namespace Mcc;

        std::vector<std::tuple<glm::vec3, glm::ivec3, glm::uvec3>> data {
            { {  0  , 126  ,   0}, { 0, 126   / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { {  0  , 126.5,   0}, { 0, 126.5 / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { { 30  , 126  ,   0}, { 0, 126   / Chunk::Size,  0}, {30, 126 % Chunk::Size,  0} },
            { { 30  , 126  ,  30}, { 0, 126   / Chunk::Size,  0}, {30, 126 % Chunk::Size, 30} },
            { {  0  , 126  ,  30}, { 0, 126   / Chunk::Size,  0}, { 0, 126 % Chunk::Size, 30} },
            { {  0  ,   0  ,   0}, { 0,                   0,  0}, { 0,                 0,  0} },
            { { 30.5,   0  ,   0}, { 0,                   0,  0}, {30,                 0,  0} },
            { { 31  ,   0  ,   0}, { 1,                   0,  0}, { 0,                 0,  0} },
            { { 31.5,   0  ,   0}, { 1,                   0,  0}, { 0,                 0,  0} },
            { { 32  ,   0  ,   0}, { 1,                   0,  0}, { 1,                 0,  0} },
            { {-32  , 110  , -10}, {-2, 110   / Chunk::Size, -1}, {30, 110 % Chunk::Size, 21} },
            { {-1   ,   0  ,   0}, {-1,                   0,  0}, {30,                 0,  0} },
            { {-31  ,   0  ,   0}, {-1,                   0,  0}, { 0,                 0,  0} },
            { {-31.5,   0  ,   0}, {-2,                   0,  0}, {30,                 0,  0} },
            { {-32  ,   0  ,   0}, {-2,                   0,  0}, {30,                 0,  0} },
        };

        for (auto& [ w, c, i ] : data)
        {
            WorldPosF p(w);
            WorldPosV g = p;

            auto [ cc, ci ] = g;
            glm::ivec3 vcc = cc;
            glm::uvec3 vci = ci;

            CHECK_EQ(vcc, c);
            CHECK_EQ(vci, i);
        }
    }

    TEST_CASE("VoxelCoordToFloatCoord")
    {
        using namespace Mcc;

        std::vector<std::tuple<glm::vec3, glm::ivec3, glm::uvec3>> data {
            { {  0  , 126,   0}, { 0, 126 / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { {  0  , 126,   0}, { 0, 126 / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { { 30  , 126,   0}, { 0, 126 / Chunk::Size,  0}, {30, 126 % Chunk::Size,  0} },
            { {  0  , 126,  30}, { 0, 126 / Chunk::Size,  0}, { 0, 126 % Chunk::Size, 30} },
            { { 30  , 126,  30}, { 0, 126 / Chunk::Size,  0}, {30, 126 % Chunk::Size, 30} },
            { {  0  ,   0,   0}, { 0,                 0,  0}, { 0,                 0,  0} },
            { { 30  ,   0,   0}, { 0,                 0,  0}, {30,                 0,  0} },
            { { 31  ,   0,   0}, { 1,                 0,  0}, { 0,                 0,  0} },
            { { 31  ,   0,   0}, { 1,                 0,  0}, { 0,                 0,  0} },
            { { 32  ,   0,   0}, { 1,                 0,  0}, { 1,                 0,  0} },
            { {-32  , 110, -10}, {-2, 110 / Chunk::Size, -1}, {30, 110 % Chunk::Size, 21} },
            { {-1   ,   0,   0}, {-1,                 0,  0}, {30,                 0,  0} },
            { {-31  ,   0,   0}, {-1,                 0,  0}, { 0,                 0,  0} },
            { {-32  ,   0,   0}, {-2,                 0,  0}, {30,                 0,  0} },
        };

        for (auto& [ w, c, i ] : data)
        {
            WorldPosV p(c, i);
            WorldPosF g = p;

            glm::fvec3 vw = g;

            CHECK_EQ(vw, w);
        }
    }

    TEST_CASE("FloatCoordToEnttyCoord")
    {
        using namespace Mcc;

        std::vector<std::tuple<glm::vec3, glm::ivec3, glm::uvec3>> data {
            { {  0  , 126  ,   0  }, { 0, 126   / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { {  0  , 126.5,   0  }, { 0, 126.5 / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { { 30  , 126  ,   0  }, { 0, 126   / Chunk::Size,  0}, {30, 126 % Chunk::Size,  0} },
            { {  0  , 126  ,  30  }, { 0, 126   / Chunk::Size,  0}, { 0, 126 % Chunk::Size, 30} },
            { { 30  , 126  ,  30  }, { 0, 126   / Chunk::Size,  0}, {30, 126 % Chunk::Size, 30} },
            { {  0  ,   0  ,   0  }, { 0,                   0,  0}, { 0,                 0,  0} },
            { { 30.5,   0  ,   0  }, { 0,                   0,  0}, {30,                 0,  0} },
            { { 31  ,   0  ,   0  }, { 1,                   0,  0}, { 0,                 0,  0} },
            { { 31.5,   0  ,   0  }, { 1,                   0,  0}, { 0,                 0,  0} },
            { { 32  ,   0  ,   0  }, { 1,                   0,  0}, { 1,                 0,  0} },
            { {-32  , 110  , - 1.5}, {-2, 110   / Chunk::Size, -1}, {30, 110 % Chunk::Size, 29} },
            { {-32  , 110  , -10  }, {-2, 110   / Chunk::Size, -1}, {30, 110 % Chunk::Size, 21} },
            { {-1   ,   0  ,   0  }, {-1,                   0,  0}, {30,                 0,  0} },
            { {-31  ,   0  ,   0  }, {-1,                   0,  0}, { 0,                 0,  0} },
            { {-31.5,   0  ,   0  }, {-2,                   0,  0}, {30,                 0,  0} },
            { {-32  ,   0  ,   0  }, {-2,                   0,  0}, {30,                 0,  0} },
        };

        for (auto& [ w, c, i ] : data)
        {
            WorldPosF p(w);
            WorldPosE t = p;
            WorldPosV g = t;

            auto [ cc, ci ] = g;
            glm::ivec3 vcc = cc;
            glm::uvec3 vci = ci;

            CHECK_EQ(vcc, c);
            CHECK_EQ(vci, i);
        }
    }

    TEST_CASE("EnttyCoordToFloatCoord")
    {
        using namespace Mcc;

        std::vector<std::tuple<glm::vec3, glm::ivec3, glm::uvec3>> data {
            { {  0, 126,   0}, { 0, 126 / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { {  0, 126,   0}, { 0, 126 / Chunk::Size,  0}, { 0, 126 % Chunk::Size,  0} },
            { { 30, 126,   0}, { 0, 126 / Chunk::Size,  0}, {30, 126 % Chunk::Size,  0} },
            { {  0, 126,  30}, { 0, 126 / Chunk::Size,  0}, { 0, 126 % Chunk::Size, 30} },
            { { 30, 126,  30}, { 0, 126 / Chunk::Size,  0}, {30, 126 % Chunk::Size, 30} },
            { {  0,   0,   0}, { 0,                 0,  0}, { 0,                 0,  0} },
            { { 30,   0,   0}, { 0,                 0,  0}, {30,                 0,  0} },
            { { 31,   0,   0}, { 1,                 0,  0}, { 0,                 0,  0} },
            { { 31,   0,   0}, { 1,                 0,  0}, { 0,                 0,  0} },
            { { 32,   0,   0}, { 1,                 0,  0}, { 1,                 0,  0} },
            { {-32, 110, -10}, {-2, 110 / Chunk::Size, -1}, {30, 110 % Chunk::Size, 21} },
            { {-1 ,   0,   0}, {-1,                 0,  0}, {30,                 0,  0} },
            { {-31,   0,   0}, {-1,                 0,  0}, { 0,                 0,  0} },
            { {-32,   0,   0}, {-2,                 0,  0}, {30,                 0,  0} },
        };

        for (auto& [ w, c, i ] : data)
        {
            WorldPosV p(c, i);
            WorldPosE t = p;
            WorldPosF g = t;
            
            glm::fvec3 vw = g;

            CHECK_EQ(vw, w);
        }
    }

    TEST_CASE("ChunkCoordToFloatCoord")
    {
        using namespace Mcc;
        
        std::vector<std::tuple<glm::vec3, glm::ivec3>> data {
            {{  0,   0,   0}, { 0, 0,  0}},
            {{ 31,   0,   0}, { 1, 0,  0}},
            {{-62,   0, -31}, {-2, 0, -1}},
            {{-31 ,  0,   0}, {-1, 0,  0}},
        };

        for (auto& [ w, c ] : data)
        {
            ChunkPosV p(c);
            WorldPosF t = p;
            WorldPosF g = t;
            
            glm::fvec3 vw = g;

            CHECK_EQ(vw, w);
        }
    }

    TEST_CASE("FloatToChunkCoord")
    {
        using namespace Mcc;
        
        std::vector<std::tuple<glm::vec3, glm::ivec3>> data {
            {{  0, 126,   0}, { 0, 126 / Chunk::Size,  0}},
            {{  0, 126,   0}, { 0, 126 / Chunk::Size,  0}},
            {{ 30, 126,   0}, { 0, 126 / Chunk::Size,  0}},
            {{  0, 126,  30}, { 0, 126 / Chunk::Size,  0}},
            {{ 30, 126,  30}, { 0, 126 / Chunk::Size,  0}},
            {{  0,   0,   0}, { 0,                 0,  0}},
            {{ 30,   0,   0}, { 0,                 0,  0}},
            {{ 31,   0,   0}, { 1,                 0,  0}},
            {{ 31,   0,   0}, { 1,                 0,  0}},
            {{ 32,   0,   0}, { 1,                 0,  0}},
            {{-32, 110, -10}, {-2, 110 / Chunk::Size, -1}},
            {{-1 ,   0,   0}, {-1,                 0,  0}},
            {{-31,   0,   0}, {-1,                 0,  0}},
            {{-32,   0,   0}, {-2,                 0,  0}},
        };

        for (auto& [ w, c ] : data)
        {
            WorldPosF p(w);
            ChunkPosV t = p;
            ChunkPosV g = t;
            
            glm::ivec3 vc = g;

            CHECK_EQ(vc, c);
        }
    }

    TEST_CASE("ChunkPosV")
    {
        using namespace Mcc;

        ChunkPosV a(1, 0, -3);

        SUBCASE("Equal")
        {
            auto b = a;
            CHECK_EQ(a, b);
        }
    }

    TEST_CASE("LocalPosV")
    {
        using namespace Mcc;

        LocalPosV a(2, 30, 21);
        LocalPosV b(2, 30, 21);

        SUBCASE("Equal")
        {
            CHECK_EQ(a, b);
        }
    }

    TEST_CASE("LocalPosE")
    {
        using namespace Mcc;

        LocalPosE a(0.5, .44, 0.23);
        LocalPosE b(0.5, .44, 0.23);

        SUBCASE("Equal")
        {
            CHECK_EQ(a, b);
        }
    }

    TEST_CASE("Position<WorldSpace, Grid3DCoord>")
    {
        using namespace Mcc;

        WorldPosV a({10, 0, -10}, {10, 30, 20});

        SUBCASE("Equal")
        {
            auto b = a;
            CHECK_EQ(a, b);
        }
    }

    TEST_CASE("Position<WorldSpace, EntityCoord>")
    {
        using namespace Mcc;

        WorldPosE a({10, 0, -10}, {0.5, 0.44, 0.22});

        SUBCASE("Equal")
        {
            auto b = a;
            CHECK_EQ(a, b);
        }
    }

}