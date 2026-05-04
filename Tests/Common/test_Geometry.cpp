// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "glm_stdout.h"
#include "doctest/doctest.h"

#include "Common/World/Geometry.h"

namespace Mcc
{

    template<typename S, typename C>
    static std::ostream& operator<<(std::ostream& os, const Position<S, C>& p)
    {
        return os << format_as(p);
    }

    template<typename C>
    static std::ostream& operator<<(std::ostream& os, const Translation<C>& p)
    {
        return os << format_as(p);
    }

}

TEST_SUITE("Geometry")
{

    TEST_CASE("TranslationV")
    {
        using namespace Mcc;

        SUBCASE("ChunkPosVSub")
        {
            ChunkPosV    lhs(-2, 0,  1);
            ChunkPosV    rhs( 2, 0, -1);
            TranslationV r  (Chunk::Size * 4, 0, Chunk::Size * -2);

            CHECK_EQ(rhs - lhs, r);
        }

        SUBCASE("LocalPosVSub")
        {
            LocalPosV    lhs( 5, 12, 5);
            LocalPosV    rhs(10,  6, 3);
            TranslationV r  (10 - 5, 6 - 12, 3 - 5);

            CHECK_EQ(rhs - lhs, r);
        }

        SUBCASE("WorldPosVSub")
        {
            WorldPosV    lhs({ -2, 0,  1 }, {  5, 12, 5 });
            WorldPosV    rhs({  2, 0, -1 }, { 10,  6, 3 });
            TranslationV r(
                Chunk::Size *  4 + 10 -  5, 
                                    6 - 12, 
                Chunk::Size * -2 +  3 -  5
            );

            CHECK_EQ(rhs - lhs, r);
        }

        SUBCASE("TranslateChunkPos")
        {
            TranslationV t(1, 10, -2);
            ChunkPosV    p(0, 0, 0);
            ChunkPosV    r_add(1, 10, -2);
            ChunkPosV    r_sub(-1, -10, 2);

            CHECK_EQ(p + t, r_add);
            CHECK_EQ(p - t, r_sub);
        }

        SUBCASE("TranslateLocalPos")
        {
            TranslationV t(50, 10, 3);
            LocalPosV    p( 0, 20, 0);


            TranslationV t_add(1, 0, 0);
            TranslationV t_sub(-2, 0, -1);
            LocalPosV    r_add(19, 30,  3);
            LocalPosV    r_sub(12, 10, 28);

            auto [ rr_add, tt_add ] = p + t;
            auto [ rr_sub, tt_sub ] = p - t;

            CHECK_EQ(rr_add, r_add);
            CHECK_EQ(rr_sub, r_sub);
            CHECK_EQ(tt_add, t_add);
            CHECK_EQ(tt_sub, t_sub);
        }

        SUBCASE("TranslateWorldPos")
        {
            TranslationV t(50, 10, 3);
            WorldPosV    p({0, 0, 0}, {0, 20, 0});
            WorldPosV    r_add({1, 0, 0}, {19, 30, 3});
            WorldPosV    r_sub({-2, 0, -1}, {12, 10, 28});

            CHECK_EQ(p + t, r_add);
            CHECK_EQ(p - t, r_sub);
        }
    }

    TEST_CASE("TranslationE")
    {
        using namespace Mcc;

        SUBCASE("LocalPosESub")
        {
            LocalPosE    lhs(.5, .2, .1);
            LocalPosE    rhs(.3, .8, .2);
            TranslationE r  (.3 - .5, .8 - .2, .2 - .1);

            CHECK_EQ(rhs - lhs, r);
        }

        SUBCASE("WorldPosESub")
        {
            WorldPosE    lhs({ -2, 0,  1 }, { .5, .2, .1 });
            WorldPosE    rhs({  2, 0, -1 }, { .3, .8, .2 });
            TranslationE r(
                 4 + .3 - .5, 
                     .8 - .2, 
                -2 + .2 - .1
            );

            CHECK_EQ(rhs - lhs, r);
        }

        SUBCASE("TranslateWorldPos")
        {
            TranslationE t(1.f, 1.f, -1.f);
            WorldPosE    p({0, 0, 0}, {.5f, 0.f, .5f});
            WorldPosE    r_add({ 1, 1, -1}, {.5f, 0.f, .5f});
            WorldPosE    r_sub({-1, -1,  1}, {.5f, 0.f, .5f});

            CHECK_EQ(p + t, r_add);
            CHECK_EQ(p - t, r_sub);
        }

        SUBCASE("TranslateLocalPos")
        {
            // TODO
        }
    }

    TEST_CASE("TranslationF")
    {
        using namespace Mcc;        

        SUBCASE("WorldPosFSub")
        {
            WorldPosF    lhs(-5, 12, -5);
            WorldPosF    rhs(10, -6,  3);
            TranslationF r(
                10 - -5, 
                -6 - 12, 
                 3 - -5
            );

            CHECK_EQ(rhs - lhs, r);
        }

        SUBCASE("TranslateWorldPos")
        {
            TranslationF t(50.f, 199.f, -33.f);
            WorldPosF    p(10.f, 0.f, 12.f);
            WorldPosF    r_add(60.f, 199.f, -21.f);
            WorldPosF    r_sub(-40.F, -199.f, 45.f);

            CHECK_EQ(p + t, r_add);
            CHECK_EQ(p - t, r_sub);
        }
    }

    TEST_CASE("Lerp")
    {
        using namespace Mcc;

        SUBCASE("LerpWorldPosV")
        {
            WorldPosV lhs({  2, 0, -5 }, { 22, 12, 10 });
            WorldPosV rhs({ -1, 0,  3 }, {  5,  0, 12 });

            for (auto v: { .25f, .50f, .75f })
            {
                int dx = glm::floor((-1 * Chunk::Size -  2 * Chunk::Size +  5 - 22) * v);
                int dy = glm::floor(( 0 - 12) * v);
                int dz = glm::floor(( 3 * Chunk::Size - -5 * Chunk::Size + 12 - 10) * v);

                float fx = dx;
                float fz = dz;

                WorldPosV e(
                    {
                        static_cast<int32_t>(glm::floor(( 2 * Chunk::Size + 22 + fx) / Chunk::Size)),
                        0,
                        static_cast<int32_t>(glm::floor((-5 * Chunk::Size + 10 + fz) / Chunk::Size)),
                    },
                    {
                        static_cast<uint32_t>(Chunk::Size + ((22 + dx) % Chunk::Size)) % Chunk::Size,
                        static_cast<uint32_t>(12 + dy),
                        static_cast<uint32_t>(Chunk::Size + ((10 + dz) % Chunk::Size)) % Chunk::Size
                    }
                );

                CHECK_MESSAGE(Lerp(lhs, rhs, v) == e, fmt::format("{} {} {}", dx, dz, v));
            }
        }

        SUBCASE("LerpWorldPosE")
        {
            // TODO
        }

        SUBCASE("LerpWorldPosF")
        {
            WorldPosF lhs( 20, 22,  10);
            WorldPosF rhs(-21, 44, -20);

            WorldPosF fq(20 + (-21 - 20) * .25, 22 + (44 - 22) * .25, 10 + (-20 - 10) * .25);
            WorldPosF ha(20 + (-21 - 20) * .50, 22 + (44 - 22) * .50, 10 + (-20 - 10) * .50);
            WorldPosF lq(20 + (-21 - 20) * .75, 22 + (44 - 22) * .75, 10 + (-20 - 10) * .75);

            CHECK_EQ(Lerp(lhs, rhs, .25), fq);
            CHECK_EQ(Lerp(lhs, rhs, .50), ha);
            CHECK_EQ(Lerp(lhs, rhs, .75), lq);
        }

    }

}