// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0


#define BOOST_TEST_MODULE test_math_alphabet_new
#include "gtest/gtest.h"

#include <cstddef>
#include <iosfwd>
#include <limits>
#include <memory>
#include <sstream>
#include <type_traits>
#include <variant>

#include "boost/unordered/unordered_flat_set.hpp"

#include "meta/vector.hpp"
#include "rime/core.hpp"

/**
 * Special symbol used for default-constructed symbols.
 */
struct not_a_symbol {};

inline auto operator==(not_a_symbol, not_a_symbol) { return rime::true_; }
inline auto operator!=(not_a_symbol, not_a_symbol) { return rime::false_; }

std::ostream & operator<<(std::ostream & os, not_a_symbol)
{
    return os << "(not a symbol)";
}

namespace detail {

struct unusable_type
{
    unusable_type() = delete;
};

struct as_special_symbol {};

/**
 * Implement the basic behaviour for dense_symbol, mostly related to normal
 * symbols.
 */
template <class AlphabetDefinition> class dense_symbol_base
{
public:
    using alphabet_definition = AlphabetDefinition;
    using underlying_type = typename AlphabetDefinition::underlying_type;

protected:
    dense_symbol_base(detail::as_special_symbol, underlying_type const & value)
    : underlying_value_(value)
    {
        assert(is_special());
    }

public:
    /**
     * Construct with a normal symbol.
     */
    explicit dense_symbol_base(underlying_type const & value)
    : underlying_value_(value)
    {
        if (!is_normal()) {
            throw std::runtime_error(
                "The value does not represent a normal value");
        }
    }

    constexpr bool is_normal() const
    {
        return underlying_value_ < alphabet_definition::special_symbol_start;
    }

    constexpr bool is_special() const
    {
        return underlying_value_ >= alphabet_definition::special_symbol_start;
    }

    underlying_type normal_value() const
    {
        assert(is_normal());
        return underlying_value_;
    }

    bool operator==(dense_symbol_base const & other) const
    {
        return this->underlying_value_ == other.underlying_value_;
    }

    bool operator!=(dense_symbol_base const & other) const
    {
        return this->underlying_value_ != other.underlying_value_;
    }

    template <class Function>
    friend auto visit(Function && function, dense_symbol_base const & symbol)
    {
        if (symbol.is_normal()) {
            return std::forward<Function>(function)(symbol.normal_value());
        }
        alphabet_definition::visit(
            std::forward<Function>(function), symbol.underlying_value_);
    }

protected:
    underlying_type underlying_value_;
};

template <class Base, class Type, Type index, class SpecialSymbols>
struct dense_symbol_special_impl;

// No more special symbols: derive from Base.
template <class Base, class Type, Type index>
struct dense_symbol_special_impl<Base, Type, index, meta::vector<>> : Base
{
    using Base::Base;
};

// Otherwise: implement overloads for the constructor and comparison.
template <
    class Base, class Type, Type index, class FirstSymbol,
    class... OtherSymbols>
struct dense_symbol_special_impl<
    Base, Type, index, meta::vector<FirstSymbol, OtherSymbols...>>
: dense_symbol_special_impl<
      Base, Type, Type(index - 1), meta::vector<OtherSymbols...>>
{
    using base_type = dense_symbol_special_impl<
        Base, Type, Type(index - 1), meta::vector<OtherSymbols...>>;

    // Inherit constructors.
    using base_type::base_type;

    // Define a new constructor for FirstSymbol.
    explicit dense_symbol_special_impl(FirstSymbol)
    : base_type(as_special_symbol {}, index)
    {}

    // Define new comparison operators with FirstSymbol.
    friend constexpr bool operator==(
        dense_symbol_special_impl const & left, FirstSymbol)
    {
        return left.underlying_value_ == index;
    }

    friend constexpr bool operator!=(
        dense_symbol_special_impl const & left, FirstSymbol)
    {
        return left.underlying_value_ != index;
    }

    friend bool operator==(FirstSymbol, dense_symbol_special_impl const & right)
    {
        return right.underlying_value_ == index;
    }

    friend constexpr bool operator!=(
        FirstSymbol, dense_symbol_special_impl const & right)
    {
        return right.underlying_value_ != index;
    }
};

/**
 * Implement behaviour for dense_symbol for its interaction with the special
 * symbols, with telescoping inheritance.
 * Ultimately, derive from \c Base.
 */
template <class Base, class AlphabetDefinition> class implement_dense_special
: public dense_symbol_special_impl<
      Base, typename AlphabetDefinition::underlying_type,
      typename AlphabetDefinition::underlying_type(-1),
      typename AlphabetDefinition::special_symbols>
{
    using base_type =
        typename implement_dense_special::dense_symbol_special_impl;

public:
    using base_type::base_type;
};

}  // namespace detail


template <
    class Underlying, class SpecialSymbols,
    Underlying MaxNumSpecialSymbols = 128>
struct dense_alphabet_definition
{
    static constexpr bool is_dense = true;

    using underlying_type = Underlying;

    using special_symbols = SpecialSymbols;

    static constexpr underlying_type max_num_special_symbols =
        MaxNumSpecialSymbols;

    static_assert(meta::size<SpecialSymbols>::value <= max_num_special_symbols);

    static constexpr underlying_type special_symbol_start =
        static_cast<underlying_type>(0 - max_num_special_symbols);
};


/* sparse */

namespace detail {

template <class AlphabetDefinition> class sparse_symbol_base
{
    using variant_type = typename AlphabetDefinition::variant_type;

public:
public:
    using alphabet_definition = AlphabetDefinition;
    using value_type = typename AlphabetDefinition::value_type;

    // /**
    //  * Initialise with \c not_a_symbol.
    //  */
    // sparse_symbol_base() : underlying_value_(not_a_symbol {}) {}

protected:
    template <class SpecialSymbol>
    sparse_symbol_base(detail::as_special_symbol, SpecialSymbol symbol)
    : underlying_value_(symbol)
    {
        assert(is_special());
    }

public:
    /**
     * Construct with a normal .
     */
    template <class Value> explicit sparse_symbol_base(Value const & value)
    : underlying_value_(value)
    {}

    bool is_normal() const { return underlying_value_.index() == 0; }

    bool is_special() const { return underlying_value_.index() != 0; }

    value_type normal_value() const
    {
        assert(is_normal());
        return std::get<value_type>(underlying_value_);
    }

    bool operator==(sparse_symbol_base const & other) const
    {
        return this->underlying_value_ == other.underlying_value_;
    }

    bool operator!=(sparse_symbol_base const & other) const
    {
        return this->underlying_value_ != other.underlying_value_;
    }

    template <class Function>
    friend auto visit(Function && function, sparse_symbol_base const & symbol)
    {
        std::visit(std::forward<Function>(function), symbol.underlying_value_);
    }

protected:
    variant_type underlying_value_;
};


template <class Base, class SpecialSymbols> struct sparse_symbol_special_impl;

// No more special symbols: derive from Base.
template <class Base> struct sparse_symbol_special_impl<Base, meta::vector<>>
: Base
{
    using Base::Base;
};

template <class Base, class FirstSymbol, class... OtherSymbols>
struct sparse_symbol_special_impl<
    Base, meta::vector<FirstSymbol, OtherSymbols...>>
: sparse_symbol_special_impl<Base, meta::vector<OtherSymbols...>>
{
    using base_type =
        sparse_symbol_special_impl<Base, meta::vector<OtherSymbols...>>;

    // Inherit constructors.
    using base_type::base_type;

    // Define a new constructor for FirstSymbol.
    explicit sparse_symbol_special_impl(FirstSymbol symbol)
    : base_type(as_special_symbol {}, symbol)
    {}

    // Define new comparison operators with FirstSymbol.
    friend constexpr bool operator==(
        sparse_symbol_special_impl const & left, FirstSymbol)
    {
        return std::holds_alternative<FirstSymbol>(left.underlying_value_);
    }

    friend constexpr bool operator!=(
        sparse_symbol_special_impl const & left, FirstSymbol)
    {
        return !std::holds_alternative<FirstSymbol>(left.underlying_value_);
    }

    friend bool operator==(
        FirstSymbol, sparse_symbol_special_impl const & right)
    {
        return std::holds_alternative<FirstSymbol>(right.underlying_value_);
    }

    friend constexpr bool operator!=(
        FirstSymbol, sparse_symbol_special_impl const & right)
    {
        return !std::holds_alternative<FirstSymbol>(right.underlying_value_);
    }
};

/**
 * Implement behaviour for sparse_symbol for its interaction with the special
 * symbols, with telescoping inheritance.
 * Ultimately, derive from \c Base.
 */
template <class Base, class AlphabetDefinition> class implement_sparse_special
: public sparse_symbol_special_impl<
      Base, typename AlphabetDefinition::special_symbols>
{
    using base_type =
        typename implement_sparse_special::sparse_symbol_special_impl;

public:
    using base_type::base_type;
};

template <class Value, class SpecialSymbols> struct define_variant_type;

template <class Value, class... SpecialSymbols>
struct define_variant_type<Value, meta::vector<SpecialSymbols...>>
{
    using variant_type = std::variant<Value, SpecialSymbols...>;
};

}  // namespace detail

template <class Value, class SpecialSymbols> struct sparse_alphabet_definition
: detail::define_variant_type<Value, SpecialSymbols>
{
    static constexpr bool is_dense = false;

    using value_type = Value;

    using special_symbols = SpecialSymbols;
};

template <class AlphabetDefinition> class symbol
: public std::conditional<
      AlphabetDefinition::is_dense,
      detail::implement_dense_special<
          detail::dense_symbol_base<AlphabetDefinition>, AlphabetDefinition>,
      detail::implement_sparse_special<
          detail::sparse_symbol_base<AlphabetDefinition>,
          AlphabetDefinition>>::type
{
    using base_type = typename std::conditional<
        AlphabetDefinition::is_dense,
        detail::implement_dense_special<
            detail::dense_symbol_base<AlphabetDefinition>, AlphabetDefinition>,
        detail::implement_sparse_special<
            detail::sparse_symbol_base<AlphabetDefinition>,
            AlphabetDefinition>>::type;

public:
    using base_type::base_type;

    /**
     * Initialise with \c not_a_symbol.
     */
    symbol() : base_type(not_a_symbol {}) {}
};


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
: dense_alphabet_definition<
      std::uint32_t,
      meta::vector<not_a_symbol, empty_symbol, end_of_sentence>> {};

struct example_alphabet_definition_2
: sparse_alphabet_definition<
      std::uint32_t,
      meta::vector<not_a_symbol, empty_symbol, end_of_sentence>> {};

// TEST(Symbol, DenseAlphabetDefinitionOne)
// {
//     auto definition =
//         dense_alphabet_definition<unsigned, meta::vector<not_a_symbol>> {};

//     constexpr unsigned not_a_symbol_value = static_cast<unsigned>(-1);

//     EXPECT_EQ(definition.type_to_index(not_a_symbol()), not_a_symbol_value);
//     EXPECT_EQ(
//         definition.index_to_type(
//             rime::constant<unsigned, not_a_symbol_value>()),
//         not_a_symbol {});

//     // visit.
//     definition.visit([](not_a_symbol) {}, not_a_symbol_value);

//     EXPECT_THROW(definition.visit([](not_a_symbol) {}, -2),
//     std::runtime_error); EXPECT_THROW(definition.visit([](not_a_symbol) {},
//     0), std::runtime_error);
// }

// TEST(Symbol, DenseAlphabetDefinitionThree)
// {
//     auto definition = dense_alphabet_definition<
//         unsigned, meta::vector<not_a_symbol, empty_symbol, end_of_sentence>>
//         {};

//     constexpr unsigned not_a_symbol_value = static_cast<unsigned>(-1);
//     constexpr unsigned empty_symbol_value = static_cast<unsigned>(-2);
//     constexpr unsigned end_of_sentence_value = static_cast<unsigned>(-3);

//     static_assert(
//         decltype(definition.type_to_index(not_a_symbol()))::value
//         == not_a_symbol_value);
//     static_assert(std::is_same_v<
//                   decltype(definition.index_to_type(
//                       rime::constant<unsigned, not_a_symbol_value>())),
//                   not_a_symbol>);

//     static_assert(
//         decltype(definition.type_to_index(empty_symbol()))::value
//         == empty_symbol_value);
//     static_assert(std::is_same_v<
//                   decltype(definition.index_to_type(
//                       rime::constant<unsigned, empty_symbol_value>())),
//                   empty_symbol>);

//     static_assert(
//         decltype(definition.type_to_index(end_of_sentence()))::value
//         == end_of_sentence_value);
//     static_assert(std::is_same_v<
//                   decltype(definition.index_to_type(
//                       rime::constant<unsigned, end_of_sentence_value>())),
//                   end_of_sentence>);

//     // visit.
//     {
//         std::stringstream stream;
//         definition.visit(
//             [&stream](auto value) { stream << value; }, not_a_symbol_value);
//         EXPECT_EQ(stream.str(), "(not a symbol)");
//     }

//     {
//         std::stringstream stream;
//         definition.visit(
//             [&stream](auto value) { stream << value; }, empty_symbol_value);
//         EXPECT_EQ(stream.str(), "(empty symbol)");
//     }

//     {
//         std::stringstream stream;
//         definition.visit(
//             [&stream](auto value) { stream << value; },
//             end_of_sentence_value);
//         EXPECT_EQ(stream.str(), "(end of sentence)");
//     }
// }

#define EXPECT_EQ_BOTH(left, right) \
    EXPECT_EQ(left, right);         \
    EXPECT_EQ(right, left)

#define EXPECT_NE_BOTH(left, right) \
    EXPECT_NE(left, right);         \
    EXPECT_NE(right, left)


template <class Symbol> class SymbolTest : public ::testing::Test {};

using symbol_types = ::testing::Types<
    symbol<example_alphabet_definition>, symbol<example_alphabet_definition_2>>;

TYPED_TEST_SUITE(SymbolTest, symbol_types);

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
