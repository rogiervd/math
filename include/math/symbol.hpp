// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MATH_SYMBOL_HPP_INCLUDED
#define MATH_SYMBOL_HPP_INCLUDED

#include <cassert>
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

#include "boost/hash2/hash_append_fwd.hpp"

#include "meta/contains.hpp"
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

    struct as_special_symbol {};

    /**
     * Position of a special symbol in the alphabet's list of special symbols.
     */
    template <std::size_t Position> using special_position =
        std::integral_constant<std::size_t, Position>;

    /*
    Storage for symbols.
    There are two implementations: dense_symbol_storage and
    sparse_symbol_storage.
    Each provides the following, and the rest of the implementation of symbol
    is shared:
    - a public explicit constructor from a normal value;
    - a protected constructor
      (as_special_symbol, special_position<Position>, SpecialSymbol const &);
    - is_normal() and normal_value();
    - a protected is_special_at(special_position<Position>);
    - a protected less_than(other), which implements a strict weak ordering;
    - a protected hash_append(provider, hash, flavor), for Boost.Hash2, which
      gives equal symbols equal hashes;
    - a protected member underlying_value_, which compares equal exactly when
      the symbols are equal.
    */

    /**
     * Store a symbol as a single integer.
     * Normal symbols are stored as themselves; special symbols are stored as
     * values counting down from the maximum value of the underlying type.
     */
    template <class AlphabetDefinition> class dense_symbol_storage
    {
    public:
        using alphabet_definition = AlphabetDefinition;
        /** \brief
         * The type of normal symbols.
         */
        using normal_type = typename AlphabetDefinition::underlying_type;

    private:
        using underlying_type = normal_type;
        /**
         * The underlying value that represents the special symbol at
         * \a Position in the alphabet's list of special symbols.
         * The first special symbol (Position 0) is represented by the maximum
         * value of the underlying type, the second by the maximum minus one,
         * and so on.
         * These values are all at or above
         * \c alphabet_definition::special_symbol_start, so they never coincide
         * with normal values; the constructor checks this with a
         * static_assert.
         */
        template <std::size_t Position>
        static constexpr underlying_type special_value =
            static_cast<underlying_type>(underlying_type(-1) - Position);

    protected:
        template <std::size_t Position, class SpecialSymbol>
        constexpr dense_symbol_storage(
            as_special_symbol, special_position<Position>,
            SpecialSymbol const &)
        : underlying_value_(special_value<Position>)
        {
            static_assert(
                special_value<Position>
                >= alphabet_definition::special_symbol_start);
        }

    public:
        /**
         * Construct with a normal symbol.
         */
        explicit dense_symbol_storage(underlying_type const & value)
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

        normal_type normal_value() const
        {
            assert(is_normal());
            return underlying_value_;
        }

    protected:
        template <std::size_t Position>
        constexpr bool is_special_at(special_position<Position>) const
        {
            return underlying_value_ == special_value<Position>;
        }

        constexpr bool less_than(dense_symbol_storage const & other) const
        {
            return underlying_value_ < other.underlying_value_;
        }

        template <class Provider, class Hash, class Flavor>
        constexpr void hash_append(
            Provider const & provider, Hash & hash, Flavor const & flavor) const
        {
            provider.hash_append(hash, flavor, underlying_value_);
        }

        underlying_type underlying_value_;
    };

    /**
     * Store a symbol as a std::variant of the normal value type and the
     * special symbol types.
     */
    template <class AlphabetDefinition> class sparse_symbol_storage
    {
    public:
        using alphabet_definition = AlphabetDefinition;
        /**
         * The type used for normal values.
         */
        using normal_type = typename AlphabetDefinition::normal_type;

    private:
        using variant_type = typename AlphabetDefinition::variant_type;

    protected:
        template <std::size_t Position, class SpecialSymbol>
        constexpr sparse_symbol_storage(
            as_special_symbol, special_position<Position>,
            SpecialSymbol const & symbol)
        : underlying_value_(std::in_place_index<Position + 1>, symbol)
        {}

    public:
        /**
         * Construct with a normal symbol.
         */
        explicit sparse_symbol_storage(normal_type const & value)
        : underlying_value_(std::in_place_index<0>, value)
        {}

        constexpr bool is_normal() const
        {
            return underlying_value_.index() == 0;
        }

        normal_type normal_value() const
        {
            assert(is_normal());
            return std::get<0>(underlying_value_);
        }

    protected:
        template <std::size_t Position>
        constexpr bool is_special_at(special_position<Position>) const
        {
            return underlying_value_.index() == Position + 1;
        }

        /**
         * Order by the index into the variant first, and then by the normal
         * value.
         * Special symbols are not compared with each other: they need not
         * define operator<, and two of the same type are always equivalent.
         */
        constexpr bool less_than(sparse_symbol_storage const & other) const
        {
            auto const index = underlying_value_.index();
            auto const other_index = other.underlying_value_.index();
            if (index != other_index) {
                return index < other_index;
            }
            if (index == 0) {
                return std::get<0>(underlying_value_)
                    < std::get<0>(other.underlying_value_);
            }
            return false;
        }

        /**
         * Hash the index into the variant, and then the normal value, if any.
         * Special symbols are empty, so the index identifies them completely
         * and they need not be hashable themselves.
         */
        template <class Provider, class Hash, class Flavor>
        constexpr void hash_append(
            Provider const & provider, Hash & hash, Flavor const & flavor) const
        {
            provider.hash_append_size(hash, flavor, underlying_value_.index());
            if (is_normal()) {
                provider.hash_append(
                    hash, flavor, std::get<0>(underlying_value_));
            }
        }

        variant_type underlying_value_;
    };

    /**
     * Implement the interaction of a symbol with the special symbols, with
     * telescoping inheritance.
     * Each level adds a constructor and comparison operators for one special
     * symbol.
     * Ultimately, derive from \c Storage.
     */
    template <class Storage, std::size_t Position, class SpecialSymbols>
    struct symbol_special_impl;

    // No more special symbols: derive from Storage.
    template <class Storage, std::size_t Position>
    struct symbol_special_impl<Storage, Position, meta::vector<>> : Storage
    {
        using Storage::Storage;
    };

    // Otherwise: implement overloads for the constructor and comparison.
    template <
        class Storage, std::size_t Position, class FirstSymbol,
        class... OtherSymbols>
    struct symbol_special_impl<
        Storage, Position, meta::vector<FirstSymbol, OtherSymbols...>>
    : symbol_special_impl<Storage, Position + 1, meta::vector<OtherSymbols...>>
    {
    private:
        using base_type = symbol_special_impl<
            Storage, Position + 1, meta::vector<OtherSymbols...>>;
        using position = special_position<Position>;

        // The value of a special symbol is not stored (for dense symbols), and
        // visit passes FirstSymbol {}, so special symbols must be empty.
        static_assert(
            std::is_empty_v<FirstSymbol>,
            "Special symbols must be empty classes");

    public:
        // Inherit constructors.
        using base_type::base_type;

        // Define a new constructor for FirstSymbol.
        explicit constexpr symbol_special_impl(FirstSymbol const & symbol)
        : base_type(as_special_symbol {}, position {}, symbol)
        {}

        // Define new comparison operators with FirstSymbol.
        friend constexpr bool operator==(
            symbol_special_impl const & left, FirstSymbol const &)
        {
            return left.is_special_at(position {});
        }

        friend constexpr bool operator!=(
            symbol_special_impl const & left, FirstSymbol const &)
        {
            return !left.is_special_at(position {});
        }

        friend constexpr bool operator==(
            FirstSymbol const &, symbol_special_impl const & right)
        {
            return right.is_special_at(position {});
        }

        friend constexpr bool operator!=(
            FirstSymbol const &, symbol_special_impl const & right)
        {
            return !right.is_special_at(position {});
        }

    protected:
        /**
         * Call \a function with the special symbol this holds.
         * This must hold FirstSymbol or one of OtherSymbols.
         */
        template <class Function> auto visit_special(Function && function) const
        {
            if constexpr (sizeof...(OtherSymbols) == 0) {
                assert(this->is_special_at(position {}));
                return std::forward<Function>(function)(FirstSymbol {});
            } else {
                if (this->is_special_at(position {})) {
                    return std::forward<Function>(function)(FirstSymbol {});
                }
                return base_type::visit_special(
                    std::forward<Function>(function));
            }
        }
    };

    template <class Value, class SpecialSymbols> struct define_variant_type;

    template <class Value, class... SpecialSymbols>
    struct define_variant_type<Value, meta::vector<SpecialSymbols...>>
    {
        using variant_type = std::variant<Value, SpecialSymbols...>;
    };

    template <class AlphabetDefinition>
    using symbol_storage = std::conditional_t<
        AlphabetDefinition::is_dense, dense_symbol_storage<AlphabetDefinition>,
        sparse_symbol_storage<AlphabetDefinition>>;

}  // namespace detail

template <
    class Underlying, class SpecialSymbols,
    Underlying MaxNumSpecialSymbols = 128>
struct dense_alphabet_definition
{
    static constexpr bool is_dense = true;

    /**
     * The type used to store the value of this object.
     */
    using underlying_type = Underlying;

    /**
     * The type used for normal values.
     */
    using normal_type = Underlying;

    using special_symbols = SpecialSymbols;

    static constexpr underlying_type max_num_special_symbols =
        MaxNumSpecialSymbols;

    static_assert(meta::size<SpecialSymbols>::value <= max_num_special_symbols);

    static constexpr underlying_type special_symbol_start =
        static_cast<underlying_type>(0 - max_num_special_symbols);
};

template <class Value, class SpecialSymbols> struct sparse_alphabet_definition
: detail::define_variant_type<Value, SpecialSymbols>
{
    static constexpr bool is_dense = false;

    using normal_type = Value;

    using special_symbols = SpecialSymbols;
};

/**
 * Symbol from an alphabet: either a normal symbol, which has a run-time value,
 * or one of a compile-time list of special symbols.
 * Special symbols must be empty, default-constructible types.
 * The special symbol \c not_a_symbol must be one of the symbols.
 * A default-constructed symbol is equivalent to a symbol constructed from
 * \c not_a_symbol.
 */
template <class AlphabetDefinition> class symbol
: public detail::symbol_special_impl<
      detail::symbol_storage<AlphabetDefinition>, 0,
      typename AlphabetDefinition::special_symbols>
{
    using base_type = detail::symbol_special_impl<
        detail::symbol_storage<AlphabetDefinition>, 0,
        typename AlphabetDefinition::special_symbols>;

public:
    using base_type::base_type;

    static_assert(
        meta::contains<
            not_a_symbol, typename AlphabetDefinition::special_symbols>::value,
        "not_a_symbol must be in the alphabet");

    /**
     * Initialise with \c not_a_symbol.
     */
    symbol() : base_type(not_a_symbol {}) {}

    constexpr bool is_special() const { return !this->is_normal(); }

    friend bool operator==(symbol const & left, symbol const & right)
    {
        return left.underlying_value_ == right.underlying_value_;
    }

    friend bool operator!=(symbol const & left, symbol const & right)
    {
        return left.underlying_value_ != right.underlying_value_;
    }

    /*
    Ordering.
    This is a strict weak ordering (if the normal type of a sparse alphabet has
    one), but it is otherwise arbitrary: it can differ between dense and sparse
    symbols, and the position of special symbols relative to normal symbols is
    unspecified.
    */

    friend bool operator<(symbol const & left, symbol const & right)
    {
        return left.less_than(right);
    }

    friend bool operator<=(symbol const & left, symbol const & right)
    {
        return !right.less_than(left);
    }

    friend bool operator>(symbol const & left, symbol const & right)
    {
        return right.less_than(left);
    }

    friend bool operator>=(symbol const & left, symbol const & right)
    {
        return !left.less_than(right);
    }

    /**
     * Support hashing with Boost.Hash2.
     */
    template <class Provider, class Hash, class Flavor>
    friend constexpr void tag_invoke(
        boost::hash2::hash_append_tag const &, Provider const & provider,
        Hash & hash, Flavor const & flavor, symbol const * s)
    {
        s->hash_append(provider, hash, flavor);
    }

    /**
     * Call \a function with the normal value or the special symbol that
     * \a symbol holds.
     */
    template <class Function>
    friend auto visit(Function && function, symbol const & s)
    {
        if constexpr (
            meta::size<typename AlphabetDefinition::special_symbols>::value
            == 0) {
            return std::forward<Function>(function)(s.normal_value());
        } else {
            if (s.is_normal()) {
                return std::forward<Function>(function)(s.normal_value());
            }
            return s.visit_special(std::forward<Function>(function));
        }
    }
};

}  // namespace math

#endif  // MATH_SYMBOL_HPP_INCLUDED
