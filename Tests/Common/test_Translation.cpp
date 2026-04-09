// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "glm_stdout.h"
#include "doctest/doctest.h"

#include "Common/World/Translation.h"

namespace Mcc
{

    template<typename C>
    static std::ostream& operator<<(std::ostream& os, const Translation<C>& p)
    {
        return os << format_as(p);
    }

}

TEST_SUITE("Translation")
{

    TEST_CASE("TranslationV")
    {
        using namespace Mcc;

        SUBCASE("Equal")
        {
            TranslationV a(35, 110, -221);
            TranslationV b = a;
            
            CHECK_EQ(a, b);
        }

        SUBCASE("ToTranslationE")
        {
            TranslationV t(35, 110, -221);
            TranslationE e(35.f / Chunk::Size, 110.f / Chunk::Height, -221.f / Chunk::Size);
            TranslationE eT = t;

            CHECK_EQ(e, eT);
        }

        SUBCASE("ToTranslationF")
        {
            TranslationV t(35, 110, -221);
            TranslationF e(35.f, 110.f, -221.f);
            TranslationF eT = t;

            CHECK_EQ(e, eT);
        }

        SUBCASE("TranslationVAddSub")
        {
            TranslationV lhs(10, -30,  10);
            TranslationV rhs(15,  15, -10);

            TranslationV e_add(25, -15,  0);
            TranslationV e_sub(-5, -45, 20);

            TranslationV r_add = lhs + rhs;
            TranslationV r_sub = lhs - rhs;

            CHECK_EQ(e_add, r_add);
            CHECK_EQ(e_sub, r_sub);
        }

        SUBCASE("TranslationVMul")
        {
            TranslationV lhs (10, -30,  10);
            TranslationV e_mul(5, -15, 5);

            TranslationV r_mul = lhs * 0.5;

            CHECK_EQ(e_mul, r_mul);
        }
    }

    TEST_CASE("TranslationE")
    {
        using namespace Mcc;

        SUBCASE("Equal")
        {
            TranslationE a(35.f / Chunk::Size, 110.f / Chunk::Height, -221.f / Chunk::Size);
            TranslationE b = a;
            
            CHECK_EQ(a, b);
        }

        SUBCASE("ToTranslationV")
        {
            TranslationE t(35.f / Chunk::Size, 110.f / Chunk::Height, -221.f / Chunk::Size);
            TranslationV e(35, 110, -221);
            TranslationV eT = t;

            CHECK_EQ(e, eT);
        }

        SUBCASE("ToTranslationF")
        {
            TranslationE t(1.5f, 2.5f, -1.f);
            TranslationF e(1.5f * Chunk::Size, 2.5f * Chunk::Height, -1.f * Chunk::Size);
            TranslationF eT = t;

            CHECK_EQ(e, eT);
        }

        SUBCASE("TranslationEAddSub")
        {
            TranslationE lhs(10, -30,  10);
            TranslationE rhs(15,  15, -10);

            TranslationE e_add(25, -15,  0);
            TranslationE e_sub(-5, -45, 20);

            TranslationE r_add = lhs + rhs;
            TranslationE r_sub = lhs - rhs;

            CHECK_EQ(e_add, r_add);
            CHECK_EQ(e_sub, r_sub);
        }

        SUBCASE("TranslationEMul")
        {
            TranslationE lhs (10, -30,  10);
            TranslationE e_mul(5, -15, 5);

            TranslationE r_mul = lhs * 0.5;

            CHECK_EQ(e_mul, r_mul);
        }
    }

    TEST_CASE("TranslationF")
    {
        using namespace Mcc;

        SUBCASE("Equal")
        {
            TranslationF a(35.f, 110.f, -221.f);
            TranslationF b = a;
            
            CHECK_EQ(a, b);
        }

        SUBCASE("ToTranslationV")
        {
            TranslationF t(35.f, 110.f, -221.f);
            TranslationV e(35, 110, -221);
            TranslationV eT = t;

            CHECK_EQ(e, eT);
        }

        SUBCASE("ToTranslationE")
        {
            TranslationF t(1.5f * Chunk::Size, 2.5f * Chunk::Height, -1.f * Chunk::Size);
            TranslationE e(1.5f, 2.5f, -1.f);
            TranslationE eT = t;

            CHECK_EQ(e, eT);
        }

        SUBCASE("TranslationFAddSub")
        {
            TranslationF lhs(10, -30,  10);
            TranslationF rhs(15,  15, -10);

            TranslationF e_add(25, -15,  0);
            TranslationF e_sub(-5, -45, 20);

            TranslationF r_add = lhs + rhs;
            TranslationF r_sub = lhs - rhs;

            CHECK_EQ(e_add, r_add);
            CHECK_EQ(e_sub, r_sub);
        }

        SUBCASE("TranslationFMul")
        {
            TranslationF lhs (10, -30,  10);
            TranslationF e_mul(5, -15, 5);

            TranslationF r_mul = lhs * 0.5;

            CHECK_EQ(e_mul, r_mul);
        }
    }

}