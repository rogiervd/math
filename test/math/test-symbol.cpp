// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#include "math/symbol.hpp"

#include "gtest/gtest.h"

#include <cstdint>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

#include "meta/vector.hpp"
#include "rime/core.hpp"

using math::not_a_symbol;

/* Test */

struct empty_symbol {};

inline auto operator==(empty_symbol, empty_symbol) { return rime::true_; }
inline auto operator!=(empty_symbol, empty_symbol) { return rime::false_; }

std::ostream & operator<<(std::ostream & os, empty_symbol)
{
    return os << "(empty symbol)";
}

struct end_of_sentence {};

inline auto operator==(end_of_sentence, end_of_sentence) { return rime::true_; }
inline auto operator!=(end_of_sentence, end_of_sentence)
{
    return rime::false_;
}

std::ostream & operator<<(std::ostream & os, end_of_sentence)
{
    return os << "(end of sentence)";
}


struct example_alphabet_definition
: math::dense_alphabet_definition<
      std::uint32_t,
      meta::vector<not_a_symbol, empty_symbol, end_of_sentence>> {};

struct example_alphabet_definition_2
: math::sparse_alphabet_definition<
      std::uint32_t,
      meta::vector<not_a_symbol, empty_symbol, end_of_sentence>> {};


#define EXPECT_EQ_BOTH(left, right) \
    EXPECT_EQ(left, right);         \
    EXPECT_EQ(right, left)

#define EXPECT_NE_BOTH(left, right) \
    EXPECT_NE(left, right);         \
    EXPECT_NE(right, left)


template <class Symbol> class SymbolTest : public ::testing::Test {};

using symbol_types = ::testing::Types<
    math::symbol<example_alphabet_definition>,
    math::symbol<example_alphabet_definition_2>>;

TYPED_TEST_SUITE(SymbolTest, symbol_types);


// TODO generalise this test
TYPED_TEST(SymbolTest, Basics)
{
    using symbol = TypeParam;

    static_assert(!std::is_convertible_v<std::uint32_t, symbol>);
    static_assert(!std::is_convertible_v<not_a_symbol, symbol>);

    // Default-construct.
    {
        symbol s;
        EXPECT_TRUE(s.is_special());
        EXPECT_TRUE(!s.is_normal());
        EXPECT_NE_BOTH(s, symbol {0u});
        EXPECT_NE_BOTH(s, symbol {10u});
        EXPECT_EQ_BOTH(s, not_a_symbol {});
        EXPECT_NE_BOTH(s, empty_symbol {});
        EXPECT_NE_BOTH(s, end_of_sentence {});
        EXPECT_NE_BOTH(s, symbol(empty_symbol {}));
    }

    // Construct as normal symbol.
    {
        symbol s(0u);
        EXPECT_TRUE(!s.is_special());
        EXPECT_TRUE(s.is_normal());
        EXPECT_EQ_BOTH(s.normal_value(), 0);
        EXPECT_EQ_BOTH(s, symbol {0u});
        EXPECT_NE_BOTH(s, symbol {10u});
        EXPECT_NE_BOTH(s, not_a_symbol {});
        EXPECT_NE_BOTH(s, empty_symbol {});
        EXPECT_NE_BOTH(s, end_of_sentence {});
        EXPECT_NE_BOTH(s, symbol(empty_symbol {}));
    }
    {
        symbol s(10u);
        EXPECT_TRUE(!s.is_special());
        EXPECT_TRUE(s.is_normal());
        EXPECT_EQ_BOTH(s.normal_value(), 10);
        EXPECT_NE_BOTH(s, symbol {0u});
        EXPECT_EQ_BOTH(s, symbol {10u});
        EXPECT_NE_BOTH(s, not_a_symbol {});
        EXPECT_NE_BOTH(s, empty_symbol {});
        EXPECT_NE_BOTH(s, end_of_sentence {});
        EXPECT_NE_BOTH(s, symbol(empty_symbol {}));
    }

    // Construct with special symbols.
    {
        symbol s {not_a_symbol {}};
        EXPECT_TRUE(s.is_special());
        EXPECT_TRUE(!s.is_normal());
        EXPECT_NE_BOTH(s, symbol {0u});
        EXPECT_NE_BOTH(s, symbol {10u});
        EXPECT_EQ_BOTH(s, not_a_symbol {});
        EXPECT_NE_BOTH(s, empty_symbol {});
        EXPECT_NE_BOTH(s, end_of_sentence {});
        EXPECT_NE_BOTH(s, symbol(empty_symbol {}));
    }
    {
        symbol s {empty_symbol {}};
        EXPECT_TRUE(s.is_special());
        EXPECT_TRUE(!s.is_normal());
        EXPECT_NE_BOTH(s, symbol {0u});
        EXPECT_NE_BOTH(s, symbol {10u});
        EXPECT_EQ_BOTH(s, empty_symbol {});
        EXPECT_NE_BOTH(s, not_a_symbol {});
        EXPECT_NE_BOTH(s, end_of_sentence {});
        EXPECT_EQ_BOTH(s, symbol(empty_symbol {}));
    }
    {
        symbol s {end_of_sentence {}};
        EXPECT_TRUE(s.is_special());
        EXPECT_TRUE(!s.is_normal());
        EXPECT_NE_BOTH(s, symbol {0u});
        EXPECT_NE_BOTH(s, symbol {10u});
        EXPECT_EQ_BOTH(s, end_of_sentence {});
        EXPECT_NE_BOTH(s, not_a_symbol {});
        EXPECT_NE_BOTH(s, empty_symbol {});
        EXPECT_NE_BOTH(s, symbol(empty_symbol {}));
    }
}

struct describe
{
    std::string operator()(std::uint32_t value) const
    {
        return std::to_string(value);
    }
    std::string operator()(not_a_symbol) const { return "not_a_symbol"; }
    std::string operator()(empty_symbol) const { return "empty_symbol"; }
    std::string operator()(end_of_sentence) const { return "end_of_sentence"; }
};

TYPED_TEST(SymbolTest, Visit)
{
    using symbol = TypeParam;

    EXPECT_EQ(visit(describe {}, symbol {}), "not_a_symbol");
    EXPECT_EQ(visit(describe {}, symbol {0u}), "0");
    EXPECT_EQ(visit(describe {}, symbol {10u}), "10");
    EXPECT_EQ(visit(describe {}, symbol {not_a_symbol {}}), "not_a_symbol");
    EXPECT_EQ(visit(describe {}, symbol {empty_symbol {}}), "empty_symbol");
    EXPECT_EQ(
        visit(describe {}, symbol {end_of_sentence {}}), "end_of_sentence");
}

TYPED_TEST(SymbolTest, Ordering)
{
    using symbol = TypeParam;

    // Include duplicates, to check equivalence, and the largest normal value
    // that dense_alphabet_definition<std::uint32_t, ...> allows.
    std::vector<symbol> const symbols {
        symbol {},
        symbol {0u},
        symbol {1u},
        symbol {10u},
        symbol {10u},
        symbol {0xffffff7fu},  // The maximum non-special symbol.
        symbol {not_a_symbol {}},
        symbol {empty_symbol {}},
        symbol {end_of_sentence {}},
        symbol {end_of_sentence {}},
    };

    auto incomparable = [](symbol const & a, symbol const & b) {
        return !(a < b) && !(b < a);
    };

    for (symbol const & a : symbols) {
        // Irreflexivity.
        EXPECT_FALSE(a < a);

        for (symbol const & b : symbols) {
            // Asymmetry.
            if (a < b) {
                EXPECT_FALSE(b < a);
            }

            // Equivalence is equality.
            EXPECT_EQ(incomparable(a, b), a == b);

            // The other operators are consistent with <.
            EXPECT_EQ(a > b, b < a);
            EXPECT_EQ(a <= b, !(b < a));
            EXPECT_EQ(a >= b, !(a < b));

            for (symbol const & c : symbols) {
                // Transitivity.
                if (a < b && b < c) {
                    EXPECT_TRUE(a < c);
                }
                // Transitivity of incomparability.
                if (incomparable(a, b) && incomparable(b, c)) {
                    EXPECT_TRUE(incomparable(a, c));
                }
            }
        }
    }
}
