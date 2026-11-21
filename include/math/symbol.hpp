// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MATH_SYMBOL_HPP_INCLUDED
#define MATH_SYMBOL_HPP_INCLUDED

#include <cassert>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

#include "meta/range.hpp"
#include "meta/vector.hpp"

#include "rime/core.hpp"

namespace math {

/**
 * Special symbol used for default-constructed symbols.
 */
struct not_a_symbol {};

inline auto operator==(not_a_symbol, not_a_symbol) { return rime::true_; }
inline auto operator!=(not_a_symbol, not_a_symbol) { return rime::false_; }

inline std::ostream & operator<<(std::ostream & os, not_a_symbol)
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
        dense_symbol_base(
            detail::as_special_symbol, underlying_type const & value)
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
            return underlying_value_
                < alphabet_definition::special_symbol_start;
        }

        constexpr bool is_special() const
        {
            return underlying_value_
                >= alphabet_definition::special_symbol_start;
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

        template <class Function> friend auto visit(
            Function && function, dense_symbol_base const & symbol)
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

        friend bool operator==(
            FirstSymbol, dense_symbol_special_impl const & right)
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
    template <class Base, class AlphabetDefinition>
    class implement_dense_special
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

        template <class Function> friend auto visit(
            Function && function, sparse_symbol_base const & symbol)
        {
            std::visit(
                std::forward<Function>(function), symbol.underlying_value_);
        }

    protected:
        variant_type underlying_value_;
    };


    template <class Base, class SpecialSymbols>
    struct sparse_symbol_special_impl;

    // No more special symbols: derive from Base.
    template <class Base>
    struct sparse_symbol_special_impl<Base, meta::vector<>> : Base
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
            return !std::holds_alternative<FirstSymbol>(
                right.underlying_value_);
        }
    };

    /**
     * Implement behaviour for sparse_symbol for its interaction with the
     * special symbols, with telescoping inheritance. Ultimately, derive from \c
     * Base.
     */
    template <class Base, class AlphabetDefinition>
    class implement_sparse_special
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

}  // namespace math

#endif  // MATH_SYMBOL_HPP_INCLUDED
