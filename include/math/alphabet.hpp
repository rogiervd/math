/*
Copyright 2014 Rogier van Dalen.

This file is part of Rogier van Dalen's Mathematical tools library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MATH_ALPHABET_HPP_INCLUDED
#define MATH_ALPHABET_HPP_INCLUDED

#include <memory>
#include <type_traits>
#include <limits>
#include <cstdint>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/pair.hpp>

#include <boost/bimap.hpp>

#include "utility/returns.hpp"

#include "meta/vector.hpp"
#include "meta/filter.hpp"
#include "meta/contains.hpp"
#include "meta/equal.hpp"

#include "rime/core.hpp"
#include "rime/assert.hpp"

namespace math {

/** \class alphabet
\brief Alphabet of symbols.

This deals with two types of symbols: normal symbols, and special symbols.
Normal symbols are all of one type, but have different values.
For example, they can be words in a word list.
Special symbols are compile-time symbols: they have different types, but no
meaningful value.
Objects of these types must always compare equal to each other.
For example, these could be the "empty" symbol, which might be program-internal.

Symbols are assigned a dense (integer) type, too.
The first normal symbol is assigned a value of 0, the second of 1, et cetera.
Special symbols are assigned negative values from -1 down.
This makes debugging easier.

The integer type is chosen automatically as the smallest integer that can
contain the set.
By default, space is reserved in a 32-bit integer with 128 special symbols.

Normal symbols are added to an alphabet at run time.
To make sure that they cannot be accidentally re-used, symbols cannot be removed
from an alphabet.

Special symbols are added at compile time, as it were, with the
add_special_symbol function, which returns the alphabet with the extra special
symbol type.
This alphabet shares the set of normal symbols with its parent.
If a normal symbol is added to its parent, the symbol is at the same tim added
to it.

\tparam NormalSymbol
    The type for normal symbols.
\tparam Tag
    An arbitrary type that can be used to distinguish alphabets at compile time.
    The default value is \c void.
\tparam max_normal_symbol_num
    The number of normal symbols to reserve room for.
    The default value uses all of 32 bits, except for 129 symbols.
\tparam SpecialSymbols
    The special symbol types contained in the alphabet, as a meta range.
    The default value is meta::vector<>.
\tparam special_symbol_headroom
    The maximum number of special symbols.
    The default value is 128.
*/
template <class NormalSymbol, class Tag = void,
    std::size_t max_normal_symbol_num = 0xFFFFFF7F,
    class SpecialSymbols = meta::vector<>,
    std::size_t special_symbol_headroom = 0x80>
class alphabet;

/**
Exception that is thrown on an attempt to add another element to an alphabet
when it is full.
*/
class alphabet_overflow : public std::length_error {
public:
    alphabet_overflow()
    : std::length_error (
        "Attempt to add a symbol to an alphabet that has no room left.") {}
};

/**
Exception that can be thrown when a symbol is not contained in an alphabet.
An object should normally be of derived type symbol_not_found_of.
*/
class symbol_not_found : public std::out_of_range {
public:
    symbol_not_found()
    : std::out_of_range ("Symbol not found in alphabet.") {}
};

/**
Exception that can be thrown when a symbol is not contained in an alphabet.
\tparam Symbol The type of the symbol that was not found.
*/
template <class Symbol> class symbol_not_found_of : public symbol_not_found {
    Symbol symbol_;
public:
    symbol_not_found_of (Symbol const & symbol)
    : symbol_ (symbol) {}

    // GCC 4.6 seems to automatically generate this without "noexcept".
    ~symbol_not_found_of() noexcept {}

    /// \return The symbol that was not found.
    Symbol const & symbol() const { return symbol_; }
};

/**
Represent a symbol from an alphabet with an integer.
There is usually no reason to explicitly use this class; \a alphabet will
normally produce objects of this class.
\tparam Value
    The integer type that the symbol is mapped to.
    This type can also be a compile-time constant (normally rime::constant),
    if the value is known at compile time, which is often the case for special
    symbols.
\tparam Tag
    A user-defined tag to distinguish alphabets at compile time.
    Must be the same as the \c Tag template parameter of the matching
    \a alphabet.
*/
template <class Value, class Tag
/// \cond DONT_DOCUMENT
    , class Enable = void
/// \endcond
> class dense_symbol {
    Value id_;
public:
    dense_symbol() : id_() {}

private:
    template <class NormalSymbol, class Tag2,
        std::size_t max_normal_symbol_num,
        class SpecialSymbols, std::size_t special_symbol_headroom>
    friend class alphabet;

    explicit dense_symbol (Value const & id)
    : id_ (id) {}

public:
    dense_symbol (dense_symbol const &) = default;
    dense_symbol & operator = (dense_symbol const &) = default;

    /**
    Copy constructor that takes a compile-time symbol if this is a run-time
    symbol.
    */
    template <class Value2,
        /// \cond DONT_DOCUMENT
        class Enable2 = typename std::enable_if <
            rime::is_constant <Value2>::value
            && !rime::is_constant <Value>::value>::type
        /// \endcond
        > dense_symbol (dense_symbol <Value2, Tag> const & that)
    : id_ (that.id()) {}

    /**
    Return the dense id of the symbol.
    This always returns an object, not an reference: the object may be
    constructed on the fly.
    */
    Value id() const { return id_; }

    explicit operator Value() const { return id_; }
};

/**
Tag type used to pass the type of a symbol as a parameter without constructing
an object of that type.
*/
template <class Type> struct symbol_type_tag {};

template <class Value1, class Value2, class Tag>
    inline auto operator == (dense_symbol <Value1, Tag> const & symbol1,
        dense_symbol <Value2, Tag> const & symbol2)
RETURNS (symbol1.id() == symbol2.id());

template <class Value1, class Value2, class Tag>
    inline auto operator != (dense_symbol <Value1, Tag> const & symbol1,
        dense_symbol <Value2, Tag> const & symbol2)
RETURNS (symbol1.id() != symbol2.id());

namespace detail {

    typedef meta::vector <std::int_least8_t, std::int_least16_t,
            std::int_least32_t, std::int_least64_t, std::intmax_t>
        possible_dense_types;

    /**
    Select a signed int type with at least "num + 1" different values as an
    value type.
    Why signed?
    Because special symbols are assigned negative numbers starting from -1
    down, and in debugging it is clearer for these to look negative.
    Note that overflow yields undefined behaviour in signed integers but not in
    unsigned integers.
    Internally, therefore, the dense symbols use unsigned integers.
    */
    template <std::size_t num> struct int_type_for {
        /// Check whether "num" is unchanged when placed in the unsigned
        /// type.
        template <class Unsigned, std::size_t check = Unsigned (num)>
            struct equal : boost::mpl::bool_ <check == num> {};

        template <class Int> struct fits
        : equal <typename std::make_unsigned <Int>::type> {};

        typedef typename meta::filter <fits <boost::mpl::_1>,
            possible_dense_types>::type possible_types;

        static_assert (!meta::empty <possible_types>::value,
            "No types found that can contain 'num'.");

        typedef typename meta::first <possible_types>::type type;
    };

    /**
    Mapping of the symbol type to the dense symbol type.
    */
    template <class Symbol, class DenseValue>
        struct normal_symbol_mapping
    {
        DenseValue symbol_num;
        DenseValue max_symbol_num;

        typedef boost::bimap <Symbol, DenseValue> mapping_type;
        mapping_type mapping;
    public:
        normal_symbol_mapping (DenseValue max_symbol_num)
        : symbol_num (0), max_symbol_num (max_symbol_num) {}

        DenseValue get_dense (Symbol const & symbol) const {
            auto symbol_mapping = mapping.left.find (symbol);
            if (symbol_mapping != mapping.left.end()) {
                // The symbol is in the map.
                return symbol_mapping->second;
            } else
                throw symbol_not_found_of <Symbol> (symbol);
        }

        Symbol const & get_symbol (DenseValue const & dense_symbol)
            const
        {
            auto symbol_mapping = mapping.right.find (dense_symbol);
            if (symbol_mapping != mapping.right.end()) {
                // The symbol is in the map.
                return symbol_mapping->second;
            } else
                throw symbol_not_found_of <DenseValue> (dense_symbol);
        }

        DenseValue add (Symbol const & symbol) {
            auto symbol_mapping = mapping.left.find (symbol);
            if (symbol_mapping != mapping.left.end()) {
                // The symbol is already in the map.
                return symbol_mapping->second;
            } else {
                if (symbol_num == max_symbol_num)
                    throw alphabet_overflow();
                DenseValue new_value (symbol_num);
                ++ symbol_num;
                mapping.insert (typename mapping_type::value_type (
                    symbol, new_value));
                return new_value;
            }
        }
    };

    /**
    Provide implementations for methods of math::alphabet specialised for
    all special symbol types.
    This class inherits from itself (with different template parameters)
    recursively.
    It handles the first of the symbols itself, and derives from
    handle_special_symbols with the rest of the symbols to deal with them.
    \tparam DenseValue The dense (integer) type for symbols.
    \tparam Tag The tag, to distinguish alphabets at compile time.
    \tparam Symbols A list of symbol types to provide methods for.
        This must be a meta-range.
    */
    template <class DenseValue, class Tag, class Symbols>
        class handle_special_symbols;

    template <class DenseValue, class Tag, class Symbols>
        class handle_special_symbols
    /// \cond DONT_DOCUMENT
    : protected handle_special_symbols <DenseValue, Tag,
        typename meta::drop <meta::back, Symbols>::type>
    /// \endcond
    {
        typedef handle_special_symbols <DenseValue, Tag,
            typename meta::drop <meta::back, Symbols>::type> base_type;
        typedef typename meta::first <meta::back, Symbols>::type symbol_type;

        static_assert (rime::equal_constant <rime::true_type,
            decltype (symbol_type() == symbol_type())>::value,
            "Symbol should be a compile-time value and always compare equal.");

        typedef typename std::make_unsigned <DenseValue>::type
            unsigned_dense_type;

        // Use negative values for special symbols.
        // Perform the negation on unsigned_dense_type, because otherwise it
        // may not be well-defined.
        static DenseValue constexpr dense_value =
            DenseValue (- unsigned_dense_type (meta::size <Symbols>::value));

        typedef dense_symbol <rime::constant <DenseValue, dense_value>, Tag>
            dense_symbol_type;

        typedef dense_symbol <DenseValue, Tag>
            general_dense_symbol_type;

    protected:
        /**
        Compile-time list of symbol mappings as mpl::pair's.
        */
        typedef typename meta::push <meta::back,
                boost::mpl::pair <symbol_type, dense_symbol_type>,
                typename base_type::special_symbol_mapping>::type
            special_symbol_mapping;

        struct default_constructible {};

    public:
        using base_type::get_dense;
        using base_type::is_special_symbol;
        using base_type::is_symbol_type;
        using base_type::get_symbol;
        using base_type::visit_type;

        /// \cond DONT_DOCUMENT
        /**
        \return The compile-time dense symbol type corresponding to the symbol.
        \internal
        This picks up only \a symbol_type.
        The base and derived classes pick up other types.
        */
        dense_symbol_type get_dense (symbol_type const &) const
        { return dense_symbol_type(); }

        rime::true_type is_special_symbol (dense_symbol_type const &)
        { return rime::true_; }

        /**
        \return true iff the dense symbol type maps onto a symbol of type
        \a SuspectedSymbol.
        If the dense symbol type is a compile-time symbol type, the result
        is a compile-time constant.
        */
        // Case 1: SuspectedSymbol is the one this class is supposed to handle;
        // the symbol is a run-time dense symbol.
        // The default_constructible parameters distinguishes it from base
        // and derived classes.
        template <class SuspectedSymbol,
            /// \cond DONT_DOCUMENT
            class Enable = typename boost::enable_if <
                std::is_same <SuspectedSymbol, symbol_type>>::type
            /// \endcond
        > bool is_symbol_type (general_dense_symbol_type const & dense_symbol,
            default_constructible = default_constructible()) const
        { return dense_symbol.id() == dense_value; }

        // Case 2: SuspectedSymbol is the one this class is supposed to handle;
        // the matching compile-time dense symbol.
        template <class SuspectedSymbol,
            /// \cond DONT_DOCUMENT
            class Enable = typename boost::enable_if <
                std::is_same <SuspectedSymbol, symbol_type>>::type
            /// \endcond
        > rime::true_type is_symbol_type (dense_symbol_type const &) const
        { return rime::true_; }

        // Case 2: SuspectedSymbol is the one this class is supposed to handle;
        // the non-matching compile-time dense symbol.
        template <class SuspectedSymbol,
            /// \cond DONT_DOCUMENT
            class Enable = typename boost::disable_if <
                std::is_same <SuspectedSymbol, symbol_type>>::type
            /// \endcond
        > rime::false_type is_symbol_type (dense_symbol_type const &) const
        { return rime::false_; }

        /**
        Get the symbol, assuming it is of type SuspectedSymbol.
        The dense symbol can have value type DenseValue, or it can be the
        general symbol.
        In other cases, a static assertion will fail.
        */
        template <class SuspectedSymbol, class DenseSymbol2,
            /// \cond DONT_DOCUMENT
            class Enable = typename boost::enable_if <
                std::is_same <SuspectedSymbol, symbol_type>>::type
            /// \endcond
        > SuspectedSymbol get_symbol (
            dense_symbol <DenseSymbol2, Tag> const & symbol,
            default_constructible = default_constructible()) const
        {
            rime::assert_ (is_symbol_type <SuspectedSymbol> (symbol));
            return symbol_type();
        }

    protected:
        // Recursive visitor.
        template <class Function>
            void visit_general_type (Function && function,
                general_dense_symbol_type const & dense) const
        {
            if (dense.id() == dense_value)
                function (symbol_type_tag <symbol_type>(), dense);
            else
                base_type::visit_general_type (function, dense);
        }

    public:
        template <class Function>
            void visit_type (Function && function,
                dense_symbol_type const & dense) const
        { function (symbol_type_tag <symbol_type>(), dense); }

        /// \endcond
    };

    /*
    The base class provides member functions so that derived classes can say
    "using ...", but makes them as unreachable as possible.
    unusable_type can never be constructed or converted to.
    */
    struct unusable_type { unusable_type() = delete; };

    template <class DenseValue, class Tag>
        class handle_special_symbols <DenseValue, Tag, meta::vector<>>
    {
    protected:
        typedef meta::vector<> special_symbol_mapping;
    public:
        void get_dense (unusable_type) const;
        void is_special_symbol (unusable_type) const;
        void is_symbol_type (unusable_type) const;
        void get_symbol (unusable_type) const;

    protected:
        template <class Function, class Symbol> void visit_general_type (
            Function && function, Symbol const &) const
        { assert (false); }

    public:
        void visit_type (unusable_type, unusable_type) const;
    };

    /**
    A metafunction that evaluates to \c true iff the first symbols of
    \a AugmentedSymbols are equal to Symbols.
    Both template parameters must be meta::vector's.
    */
    template <class AugmentedSymbols, class Symbols>
        struct augments_special_symbols
    : boost::mpl::false_ {};

    template <class First, class ... RestAugmented, class ... RestSymbols>
        struct augments_special_symbols <
            meta::vector <First, RestAugmented ...>,
            meta::vector <First, RestSymbols ...>
    > : augments_special_symbols <
        meta::vector <RestAugmented ...>, meta::vector <RestSymbols ...>> {};

    template <class ... Augmented> struct augments_special_symbols <
        meta::vector <Augmented ...>, meta::vector <>>
    : boost::mpl::true_ {};

} // namespace detail

template <class NormalSymbol, class Tag, std::size_t max_normal_symbol_num,
    class SpecialSymbols, std::size_t special_symbol_headroom>
class alphabet
/// \cond DONT_DOCUMENT
: public detail::handle_special_symbols <
    typename detail::int_type_for <
        max_normal_symbol_num + special_symbol_headroom>::type,
    Tag, typename meta::as_vector <SpecialSymbols>::type>
// \endcond
{
public:
    typedef typename meta::as_vector <SpecialSymbols>::type special_symbols;
private:
    static std::size_t constexpr special_symbol_num =
        meta::size <special_symbols>::value;

    static_assert (
        special_symbol_num <= special_symbol_headroom,
        "Not enough room reserved for special symbols."
        " Please increase special_symbol_headroom.");

    typedef typename detail::int_type_for <
        max_normal_symbol_num + special_symbol_headroom>::type dense_type;

    // Unsigned types are guaranteed to overflow sensibly.
    // (For signed types overflow yields undefined behaviour.)
    typedef typename std::make_unsigned <dense_type>::type
        unsigned_dense_type;

    /**
    The value below the special symbols.
    This value is more useful than the lowest special symbol, which may be 0.
    */
    static const unsigned_dense_type below_special_symbol =
        - unsigned_dense_type (special_symbol_num + 1);

    typedef detail::handle_special_symbols <dense_type, Tag,
        special_symbols> handle_special_symbols;

    typedef detail::normal_symbol_mapping <NormalSymbol, unsigned_dense_type>
        symbol_mapping_type;
    std::shared_ptr <symbol_mapping_type> normal_symbol_mapping;

public:
    /**
    The normal symbol type.
    Equal to \c NormalSymbol.
    */
    typedef NormalSymbol normal_symbol_type;

    /**
    The dense symbol type.
    This is an object type, with inside the smallest integer type that can fit
    all symbols in the set.
    */
    typedef dense_symbol <dense_type, Tag> dense_symbol_type;

    /**
    Construct an alphabet that is empty apart from any special symbols.
    */
    alphabet()
    : normal_symbol_mapping (
        std::make_shared <symbol_mapping_type> (max_normal_symbol_num)) {}

    template <class SpecialSymbols2>
    alphabet (alphabet <NormalSymbol, Tag, max_normal_symbol_num,
        SpecialSymbols2, special_symbol_headroom> const & other)
    : normal_symbol_mapping (other.normal_symbol_mapping) {
        static_assert (detail::augments_special_symbols <
                special_symbols, typename
                meta::as_vector <SpecialSymbols2>::type>::value,
            "alphabet can only be initialised from another alphabet whose "
            "special symbols it augments.");
    }

    /**
    Add a new normal symbol to the alphabet.
    \return The dense representation of the symbol.
    \throw alphabet_overflow if there is no room in the alphabet.
    */
    dense_symbol_type add_symbol (NormalSymbol const & symbol) {
        dense_type s = normal_symbol_mapping->add (symbol);
        return dense_symbol_type (s);
    }

    // Pull in functions from base classes to deal with special symbols.
    using handle_special_symbols::get_dense;
    using handle_special_symbols::is_special_symbol;
    using handle_special_symbols::is_symbol_type;
    using handle_special_symbols::get_symbol;
    using handle_special_symbols::visit_type;

    /**
    \return An dense representation of the symbol.
    \param symbol The representation.
        This can be of the normal symbol type, or of any of the special symbol
        types.
    */
    dense_symbol_type get_dense (NormalSymbol const & symbol) const {
        dense_type s = normal_symbol_mapping->get_dense (symbol);
        return dense_symbol_type (s);
    }

    /**
    \return \c true iff the dense symbol denotes a special symbol.
    The dense symbol can be a general dense symbol, or have a specific value
    type.
    */
    bool is_special_symbol (dense_symbol_type const & dense) const {
        unsigned_dense_type id = dense.id();
        assert (id < max_normal_symbol_num || id > below_special_symbol);
        return id > below_special_symbol;
    }

    /**
    Check whether an dense symbol represents a specific type.
    If \a SuspectedSymbol is a compile-time symbol and \p dense is a
    compile-time representation, the result is a compile-time constant.
    \tparam SuspectedSymbol
        The type that it is suspected the symbol might represent.
    \param dense
        The symbol to be checked.
        This can be a run-time or a compile-time symbol.
    \internal
    The specialisations for compile-time symbols are in
    detail::handle_special_symbols, which this class derives from.
    */
    template <class SuspectedSymbol,
        /// \cond DONT_DOCUMENT
        class Enable = typename boost::enable_if <
            std::is_same <SuspectedSymbol, NormalSymbol>>::type
        /// \endcond
    > bool is_symbol_type (dense_symbol_type const & dense) const
    { return !is_special_symbol (dense); }

    /**
    Retrieve the symbol represented by the dense symbol passed in.
    \tparam SuspectedSymbol
        The return type, which should be the actual type of the symbol.

    \pre \p SuspectedSymbol is the actual type of the symbol.
        If this is not the case, then an assertion may be triggered.
    \pre The dense symbol exists, at run time, in the alphabet.
        If this is not the case, symbol_not_found_of will be thrown.
    */
    template <class SuspectedSymbol,
        /// \cond DONT_DOCUMENT
        class Enable = typename boost::enable_if <
            std::is_same <SuspectedSymbol, NormalSymbol>>::type
        /// \endcond
    > SuspectedSymbol const & get_symbol (dense_symbol_type const & dense)
    const
    {
        assert (!is_special_symbol (dense));
        return normal_symbol_mapping->get_symbol (dense.id());
    }

    /**
    Dispatch a function with two parameters: the symbol type corresponding to
    symbol wrapped as an \a symbol_type object, and the original
    symbol.
    */
    template <class Function> void visit_type (
            Function && function, dense_symbol_type const & dense) const
    {
        if (is_special_symbol (dense))
            handle_special_symbols::visit_general_type (function, dense);
        else
            function (symbol_type_tag <normal_symbol_type>(), dense);
    }

private:
    /// \cond DONT_DOCUMENT
    template <class Function> struct visitor {
        Function && function;
        alphabet const & a;

        visitor (Function && function, alphabet const & a)
        : function (std::forward <Function> (function)), a (a) {}

        template <class Type, class ActualSymbol>
            void operator() (symbol_type_tag <Type>,
                dense_symbol <ActualSymbol, Tag> const & dense) const
        { function (a.get_symbol <Type> (dense)); }
    };
    /// \endcond

public:
    template <class Function, class ActualSymbol>
        void visit (Function && function,
            dense_symbol <ActualSymbol, Tag> const & dense) const
    {
        // Wrap the function in a visitor object and call visit_type.
        visit_type (visitor <Function> (function, *this), dense);
    }

private:
    template <class NormalSymbol2, class Tag2,
            std::size_t max_normal_symbol_num2,
            class SpecialSymbols2, std::size_t special_symbol_headroom2>
        friend class alphabet;
};

/**
Return a new alphabet with a new special symbol added to it.
If the alphabet already contains the symbol, then return the alphabet itself.
*/
template <class NewSpecialSymbol,
    class NormalSymbol, class Tag, std::size_t max_normal_symbol_num,
    class SpecialSymbols, std::size_t special_symbol_headroom,
    /// \cond DONT_DOCUMENT
    class Enable = typename std::enable_if <
        !meta::contains <NewSpecialSymbol, SpecialSymbols>::value>::type
    /// \endcond
> inline auto
add_special_symbol (alphabet <NormalSymbol, Tag, max_normal_symbol_num,
    SpecialSymbols, special_symbol_headroom> const & a)
RETURNS (alphabet <NormalSymbol, Tag, max_normal_symbol_num, typename
        meta::push <meta::back, NewSpecialSymbol, SpecialSymbols>::type,
        special_symbol_headroom> (a));

// If the alphabet already contains the symbol, return the alphabet itself.
/// \cond DONT_DOCUMENT
template <class NewSpecialSymbol,
    class NormalSymbol, class Tag, std::size_t max_normal_symbol_num,
    class SpecialSymbols, std::size_t special_symbol_headroom
    , class Enable = typename std::enable_if <
        meta::contains <NewSpecialSymbol, SpecialSymbols>::value>::type
    >
inline auto
add_special_symbol (alphabet <NormalSymbol, Tag, max_normal_symbol_num,
    SpecialSymbols, special_symbol_headroom> const & a)
RETURNS (a);
/// \endcond

} // namespace math

#endif // MATH_ALPHABET_HPP_INCLUDED
