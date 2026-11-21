// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#include "math/symbol.hpp"

#include "gtest/gtest.h"

#include "boost/hash2/fnv1a.hpp"
#include "boost/hash2/hash_append.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "meta/vector.hpp"
#include "rime/core.hpp"

using math::not_a_symbol;

/* Special symbols. */

// Special symbols must compare equal to themselves, and for the tests they
// must be printable.
#define DEFINE_SPECIAL_SYMBOL(name, text)                       \
    struct name {};                                             \
    inline auto operator==(name, name) { return rime::true_; }  \
    inline auto operator!=(name, name) { return rime::false_; } \
    inline std::ostream & operator<<(std::ostream & os, name)   \
    {                                                           \
        return os << text;                                      \
    }

DEFINE_SPECIAL_SYMBOL(empty_symbol, "(empty symbol)")
DEFINE_SPECIAL_SYMBOL(start_of_sentence, "(start of sentence)")
DEFINE_SPECIAL_SYMBOL(end_of_sentence, "(end of sentence)")
DEFINE_SPECIAL_SYMBOL(unknown_word, "(unknown word)")
// Not in any of the alphabets below.
DEFINE_SPECIAL_SYMBOL(not_in_alphabet, "(not in alphabet)")

#undef DEFINE_SPECIAL_SYMBOL

/* Test cases. */

/*
Each test case defines an alphabet, and distinct normal values to test with.
The test cases vary the number and order of special symbols (which affects
both the dense encoding and the index into the variant for sparse symbols), the
maximum number of special symbols for dense alphabets, and the normal type.
For dense alphabets, the normal values include the largest one allowed.
*/

struct dense_32_case
{
    static constexpr char const * name = "dense_32";
    using alphabet = math::dense_alphabet_definition<
        std::uint32_t,
        meta::vector<not_a_symbol, empty_symbol, end_of_sentence>>;
    static std::vector<std::uint32_t> normal_values()
    {
        return {0, 1, 10, 0xffffff7f};
    }
};

struct dense_32_one_special_case
{
    static constexpr char const * name = "dense_32_one_special";
    using alphabet = math::dense_alphabet_definition<
        std::uint32_t, meta::vector<not_a_symbol>>;
    static std::vector<std::uint32_t> normal_values()
    {
        return {0, 5, 0xffffff7f};
    }
};

struct dense_64_case
{
    static constexpr char const * name = "dense_64";
    // not_a_symbol is not first, and only 4 special symbols are allowed.
    using alphabet = math::dense_alphabet_definition<
        std::uint64_t, meta::vector<start_of_sentence, not_a_symbol>, 4>;
    static std::vector<std::uint64_t> normal_values()
    {
        return {0, 1, std::uint64_t(1) << 40, 0xfffffffffffffffb};
    }
};

struct sparse_32_case
{
    static constexpr char const * name = "sparse_32";
    using alphabet = math::sparse_alphabet_definition<
        std::uint32_t,
        meta::vector<not_a_symbol, empty_symbol, end_of_sentence>>;
    static std::vector<std::uint32_t> normal_values()
    {
        return {0, 1, 10, 0xffffffff};
    }
};

struct sparse_string_case
{
    static constexpr char const * name = "sparse_string";
    using alphabet = math::sparse_alphabet_definition<
        std::string, meta::vector<end_of_sentence, not_a_symbol, unknown_word>>;
    static std::vector<std::string> normal_values()
    {
        // The long string is allocated on the heap.
        return {"", "a", "abc", std::string(100, 'x')};
    }
};

/* Helpers. */

/**
 * Call \a function with each special symbol in \a SpecialSymbols and its
 * position.
 */
template <class SpecialSymbols> struct for_each_special_impl;

template <class... SpecialSymbols>
struct for_each_special_impl<meta::vector<SpecialSymbols...>>
{
    template <class Function> static void apply(Function && function)
    {
        std::size_t position = 0;
        (function(SpecialSymbols {}, position++), ...);
    }
};

template <class SpecialSymbols, class Function>
void for_each_special(Function && function)
{
    for_each_special_impl<SpecialSymbols>::apply(
        std::forward<Function>(function));
}

/**
 * Convert anything printable to text.
 */
struct to_text_function
{
    template <class Value> std::string operator()(Value const & value) const
    {
        std::ostringstream stream;
        stream << value;
        return stream.str();
    }
};

inline constexpr to_text_function to_text {};

template <class Left, class Right, class Enable = void>
struct is_equality_comparable : std::false_type {};

template <class Left, class Right> struct is_equality_comparable<
    Left, Right,
    std::void_t<
        decltype(std::declval<Left const &>() == std::declval<Right const &>())>>
: std::true_type {};

template <class Value> std::uint64_t get_hash(Value const & value)
{
    boost::hash2::fnv1a_64 hash;
    boost::hash2::hash_append(hash, {}, value);
    return hash.result();
}

/* Test fixture. */

template <class Case> class SymbolTest : public ::testing::Test
{
protected:
    using alphabet = typename Case::alphabet;
    using symbol = math::symbol<alphabet>;
    using normal_type = typename symbol::normal_type;
    using special_symbols = typename alphabet::special_symbols;

    /**
     * A symbol to test with.
     * Two samples compare equal if and only if their ids are equal.
     * \c text is what visiting the symbol with \c to_text should return.
     */
    struct sample
    {
        symbol value;
        std::size_t id;
        std::string text;
    };

    static std::size_t num_normal_values()
    {
        return Case::normal_values().size();
    }

    /**
     * Return samples: each normal value, each special symbol, and a
     * default-constructed symbol.
     * Every sample is generated twice, constructed independently, so that each
     * has a distinct but equal counterpart.
     */
    static std::vector<sample> samples()
    {
        std::vector<sample> result;
        for (int repeat = 0; repeat != 2; ++repeat) {
            auto const values = Case::normal_values();
            for (std::size_t index = 0; index != values.size(); ++index) {
                result.push_back(
                    {symbol {values[index]}, index, to_text(values[index])});
            }
            for_each_special<special_symbols>([&](auto special,
                                                  std::size_t position) {
                std::size_t const id = values.size() + position;
                result.push_back({symbol {special}, id, to_text(special)});
                if constexpr (std::is_same_v<decltype(special), not_a_symbol>) {
                    result.push_back({symbol {}, id, to_text(special)});
                }
            });
        }
        return result;
    }
};

struct case_name
{
    template <class Case> static std::string GetName(int) { return Case::name; }
};

using test_cases = ::testing::Types<
    dense_32_case, dense_32_one_special_case, dense_64_case, sparse_32_case,
    sparse_string_case>;

TYPED_TEST_SUITE(SymbolTest, test_cases, case_name);

/* Tests. */

TYPED_TEST(SymbolTest, Construction)
{
    using alphabet = typename TestFixture::alphabet;
    using symbol = typename TestFixture::symbol;
    using normal_type = typename TestFixture::normal_type;
    using special_symbols = typename TestFixture::special_symbols;

    // Construction must be explicit.
    static_assert(!std::is_convertible_v<normal_type, symbol>);

    // Comparing with a special symbol that is not in the alphabet must not
    // compile.
    static_assert(!is_equality_comparable<symbol, not_in_alphabet>::value);
    static_assert(!is_equality_comparable<not_in_alphabet, symbol>::value);

    // Normal symbols.
    for (normal_type const & value : TypeParam::normal_values()) {
        SCOPED_TRACE(to_text(value));
        symbol s {value};
        EXPECT_TRUE(s.is_normal());
        EXPECT_FALSE(s.is_special());
        EXPECT_EQ(s.normal_value(), value);
    }

    // Special symbols.
    for_each_special<special_symbols>([](auto special, std::size_t) {
        using special_symbol = decltype(special);
        static_assert(!std::is_convertible_v<special_symbol, symbol>);
        static_assert(is_equality_comparable<symbol, special_symbol>::value);
        static_assert(is_equality_comparable<special_symbol, symbol>::value);

        SCOPED_TRACE(to_text(special));
        symbol s {special};
        EXPECT_FALSE(s.is_normal());
        EXPECT_TRUE(s.is_special());
    });

    // Default construction.
    {
        symbol s;
        EXPECT_FALSE(s.is_normal());
        EXPECT_TRUE(s.is_special());
        EXPECT_TRUE(s == not_a_symbol {});
    }

    // Values that are reserved for special symbols.
    if constexpr (alphabet::is_dense) {
        EXPECT_THROW(
            symbol {alphabet::special_symbol_start}, std::runtime_error);
        EXPECT_THROW(
            symbol {std::numeric_limits<normal_type>::max()},
            std::runtime_error);
    }
}

TYPED_TEST(SymbolTest, Equality)
{
    using special_symbols = typename TestFixture::special_symbols;

    auto const samples = this->samples();
    auto const num_normal_values = this->num_normal_values();

    for (auto const & a : samples) {
        for (auto const & b : samples) {
            SCOPED_TRACE(a.text + " and " + b.text);
            EXPECT_EQ(a.value == b.value, a.id == b.id);
            EXPECT_EQ(a.value != b.value, a.id != b.id);
        }

        // Comparison with each special symbol, in both directions.
        for_each_special<special_symbols>(
            [&](auto special, std::size_t position) {
                SCOPED_TRACE(a.text + " and " + to_text(special));
                bool const expected = a.id == num_normal_values + position;
                EXPECT_EQ(a.value == special, expected);
                EXPECT_EQ(special == a.value, expected);
                EXPECT_EQ(a.value != special, !expected);
                EXPECT_EQ(special != a.value, !expected);
            });
    }
}

TYPED_TEST(SymbolTest, Visit)
{
    for (auto const & sample : this->samples()) {
        EXPECT_EQ(visit(to_text, sample.value), sample.text);
    }
}

TYPED_TEST(SymbolTest, Ordering)
{
    using symbol = typename TestFixture::symbol;

    std::vector<symbol> symbols;
    for (auto const & sample : this->samples()) {
        symbols.push_back(sample.value);
    }

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

TYPED_TEST(SymbolTest, Hash)
{
    // The special symbols do not support hashing themselves, so this
    // compiling shows that they are not hashed.
    auto const samples = this->samples();
    for (auto const & a : samples) {
        for (auto const & b : samples) {
            SCOPED_TRACE(a.text + " and " + b.text);
            // Equal symbols must have equal hashes; for these few distinct
            // symbols, the hashes should all differ.
            EXPECT_EQ(get_hash(a.value) == get_hash(b.value), a.id == b.id);
        }
    }
}
