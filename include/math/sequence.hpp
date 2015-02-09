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

#ifndef MATH_SEQUENCE_HPP_INCLUDED
#define MATH_SEQUENCE_HPP_INCLUDED

#include <vector>
#include <list>
#include <stdexcept>
#include <algorithm> // for std::reverse.

#include <iosfwd>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/optional.hpp>

#include "utility/overload_order.hpp"
#include "utility/disable_if_same.hpp"

#include "range/core.hpp"
#include "range/for_each_macro.hpp"
#include "range/empty_view.hpp"
#include "range/tuple.hpp"
#include "range/std/view_optional.hpp"
#include "range/std/container.hpp"
#include "range/equal.hpp"
#include "range/less_lexicographical.hpp"

#include "rime/if.hpp"
#include "rime/call_if.hpp"
#include "rime/assert.hpp"

#include "magma.hpp"

namespace math {

/**
Semiring that contains a sequence of zero or more symbols, or it can be
the multiplicative annihilator.
All specialised classes can be converted to this one implicitly; the other
way around the conversion is explicit, because they may not be possible.

\ref times concatenates two sequences.
\ref plus returns the longest common prefix (if \a Direction is \ref left) or
suffix (if \a Direction is \ref right).
\ref divide is only defined from \a Direction, and then only if the divisor is
a prefix (or suffix) of the dividend.

\ref compare implements a strict weak ordering by sorting elements
lexicographically from \a Direction.
\ref choose picks the shortest sequence first, and uses lexicographical order
from \a Direction as a tie-breaker.
This makes the sequence a semiring with \ref times and \ref choose in both
directions, whatever the value of \a Direction

\sa math::empty_sequence, math::single_sequence, math::optional_sequence,
math::sequence_annihilator

\todo Should a bi-directional version be implemented, which would not provide
the plus operation, but allow division in either direction?
*/
template <class Symbol, class Direction = left> class sequence;
template <class Symbol, class Direction = left> class empty_sequence;
template <class Symbol, class Direction = left> class single_sequence;
template <class Symbol, class Direction = left> class optional_sequence;
template <class Symbol, class Direction = left> class sequence_annihilator;

template <class Symbol, class Direction> struct sequence_tag;

template <class Symbol, class Direction>
    struct decayed_magma_tag <sequence <Symbol, Direction>>
{ typedef sequence_tag <Symbol, Direction> type; };

template <class Symbol, class Direction>
    struct decayed_magma_tag <empty_sequence <Symbol, Direction>>
{ typedef sequence_tag <Symbol, Direction> type; };

template <class Symbol, class Direction>
    struct decayed_magma_tag <single_sequence <Symbol, Direction>>
{ typedef sequence_tag <Symbol, Direction> type; };

template <class Symbol, class Direction>
    struct decayed_magma_tag <optional_sequence <Symbol, Direction>>
{ typedef sequence_tag <Symbol, Direction> type; };

template <class Symbol, class Direction>
    struct decayed_magma_tag <sequence_annihilator <Symbol, Direction>>
{ typedef sequence_tag <Symbol, Direction> type; };

template <class Symbol, class Direction> class sequence {
private:
    bool is_annihilator_;
    std::vector <Symbol> symbols_;
public:
    /**
    Initialise with no symbols.
    (Multiplicative identity.)
    */
    sequence() : is_annihilator_ (false), symbols_() {}

    /**
    Initialise with the empty sequence.
    */
    sequence (empty_sequence <Symbol, Direction> const &)
    : is_annihilator_ (false), symbols_() {}

    /**
    Initialise with a sequence with one element.
    */
    sequence (single_sequence <Symbol, Direction> const & s)
    : is_annihilator_ (false), symbols_ (1, s.symbol()) {}

    /**
    Initialise with a sequence with zero or one element.
    */
    sequence (optional_sequence <Symbol, Direction> const & s)
    : is_annihilator_ (false)
    {
        if (!s.empty())
            symbols_.push_back (s.symbol().get());
        assert (s.empty() == this->empty());
    }

    /**
    Initialise as the multiplicative annihilator.
    (Additive identity.)
    */
    sequence (sequence_annihilator <Symbol, Direction> const &)
    : is_annihilator_ (true) {}

    /**
    Initialise with a range of symbols.
    */
    template <class Range, class Enable = typename
            boost::enable_if <range::is_range <Range>>::type>
        explicit sequence (Range && range)
    : is_annihilator_ (false)
    {
        RANGE_FOR_EACH (element, range)
            symbols_.push_back (element);
    }

    /**
    Initialise with a range of symbols.
    (Optimised version using the move constructor of std::vector.)
    */
    template <class Range> explicit sequence (std::vector <Symbol> && range)
    : is_annihilator_ (false), symbols_ (std::move (range)) {}

    /**
    Return \c true iff this is an annihilator.
    */
    bool is_annihilator() const { return is_annihilator_; }

    /**
    Return \c true iff this contains a symbol sequence of zero elements.
    \pre This is not an annihilator.
    */
    bool empty() const {
        assert (!is_annihilator());
        return range::empty (symbols_);
    }

    /**
    Return a range containing the symbols.
    \pre This is not an annihilator.
    */
    std::vector <Symbol> const & symbols() const {
        assert (!is_annihilator());
        return symbols_;
    }
};

/**
Sequence that is known at compile time to be of zero length.

\sa math::sequence, math::single_sequence, math::optional_sequence
*/
template <class Symbol, class Direction> class empty_sequence {
public:
    empty_sequence() {}

    /**
    Initialise with a sequence, which must be empty.
    \throw magma_not_convertible If the sequence is non-empty.
    */
    explicit empty_sequence (sequence <Symbol, Direction> const & s) {
        if (s.is_annihilator() || !s.empty())
            throw magma_not_convertible();
    }

    /**
    Initialise with single_sequence, which is never possible and always throws.
    \throw magma_not_convertible
    */
    explicit empty_sequence (single_sequence <Symbol, Direction> const &)
    { throw magma_not_convertible(); }

    /**
    Initialise with optional_sequence, which must be empty.
    \throw magma_not_convertible If the sequence is non-empty.
    */
    explicit empty_sequence (optional_sequence <Symbol, Direction> const & s) {
        if (!s.empty())
            throw magma_not_convertible();
    }

    /**
    Initialise with a range of symbols.
    This range must be empty.
    */
    template <class Range, class Enable = typename
        boost::enable_if <range::is_range <Range>>::type>
    explicit empty_sequence (Range && range)
    { rime::assert_ (range::empty (range)); }

    rime::false_type is_annihilator() const { return rime::false_; }
    rime::true_type empty() const { return rime::true_; }

    range::empty_view symbols() const { return range::empty_view(); }
};

/**
Sequence that is known at compile time to be of length one.

\sa math::sequence, math::empty_sequence, math::optional_sequence
*/
template <class Symbol, class Direction> class single_sequence {
private:
    Symbol symbol_;

    /**
    Extract the first symbol from a symbol_sequence.
    If the range has another length than 1, throw \c magma_not_convertible.
    */
    static Symbol get_only_symbol_from (sequence <Symbol, Direction> const & s)
    {
        if (s.is_annihilator())
            throw magma_not_convertible();
        auto && symbols = s.symbols();
        if (range::empty (symbols) || !range::empty (range::drop (symbols)))
            throw magma_not_convertible();
        else
            return range::first (symbols);
    }

public:
    /**
    Initialise with an explicit symbol.
    \todo Add overloads for Symbol && and Symbol &?
    */
    explicit single_sequence (Symbol const & symbol)
    : symbol_ (symbol) {}

    /**
    Initialise with a range of symbols, which must have one element.
    */
    template <class Range,
        class Enable1 = typename
            boost::enable_if <range::is_range <Range>>::type,
        class Enable2 = typename
            utility::disable_if_same_or_derived <Symbol, Range>::type>
    explicit single_sequence (Range && range)
    : symbol_ (range::first (range))
    { rime::assert_ (range::empty (range::drop (range))); }

    /**
    Convert from sequence: explicit.
    \pre The sequence must contain exactly one element.
    \throw magma_not_convertible If the sequence has another length than 1.
    */
    explicit single_sequence (sequence <Symbol, Direction> const & that)
    : symbol_ (get_only_symbol_from (that)) {}

    rime::false_type empty() const { return rime::false_; }
    rime::false_type is_annihilator() const { return rime::false_; }

    Symbol const & symbol() const { return symbol_; }

    range::tuple <Symbol const &> symbols() const
    { return range::tie (symbol_); }
};

/**
Sequence that is known at compile time to be of length zero or one.

\sa math::sequence
*/
template <class Symbol, class Direction> class optional_sequence {
private:
    boost::optional <Symbol> symbol_;

    /**
    Extract an optional element from a range of length zero or one.
    */
    template <class Range>
        static boost::optional <Symbol> extract_from_range (Range && range)
    {
        if (range::empty (range))
            return boost::optional <Symbol>();
        else {
            auto chopped = range::chop (range);
            assert (range::empty (chopped.rest()));
            return boost::optional <Symbol> (chopped.move_first());
        }
    }
public:
    /**
    Initialise empty.
    */
    optional_sequence() {}

    /**
    Initialise with an explicit symbol.
    */
    explicit optional_sequence (Symbol const & symbol)
    : symbol_ (symbol) {}

    /**
    Convert empty, from empty_sequence.
    */
    optional_sequence (empty_sequence <Symbol, Direction> const & that)
    : symbol_ () {}

    /**
    Convert with one symbol, from single_sequence.
    */
    optional_sequence (
        single_sequence <Symbol, Direction> const & that)
    : symbol_ (that.symbol()) {}

    /**
    Convert from sequence: explicit.
    \pre The sequence must contain either zero or one element.
    \throw magma_not_convertible
        If the sequence is an annihilator or has a greater length.
    */
    explicit optional_sequence (sequence <Symbol, Direction> const & that) {
        if (that.is_annihilator())
            throw magma_not_convertible();
        auto && symbols = that.symbols();
        if (!range::empty (symbols)) {
            if (!range::empty (range::drop (symbols)))
                throw magma_not_convertible();
            else
                symbol_ = range::first (symbols);
        }
        assert (that.empty() == this->empty());
    }

    /**
    Initialise with a range of symbols, which must have one element.
    */
    template <class Range,
        class Enable1 = typename
            boost::enable_if <range::is_range <Range>>::type,
        class Enable2 = typename
            utility::disable_if_same_or_derived <Symbol, Range>::type>
    explicit optional_sequence (Range && range)
    : symbol_ (extract_from_range (std::forward <Range> (range))) {}

    rime::false_type is_annihilator() const { return rime::false_; }
    bool empty() const { return !symbol_; }

    boost::optional <Symbol> const & symbol() const { return symbol_; }

    typename range::result_of <range::callable::view_optional (
        boost::optional <Symbol> const &)>::type
    symbols() const
    { return range::view_optional (symbol_); }
};

/**
Sequence that is known at compile time to be the multiplicative annihilator:
Multiplying this with any sequence yields a sequence_annihilator.

\sa math::sequence
*/
template <class Symbol, class Direction> class sequence_annihilator {
public:
    sequence_annihilator() {}

    /**
    Construct from a sequence, which must be an annihilator.
    \throw magma_not_convertible If the argument is not an annihilator.
    */
    explicit sequence_annihilator (sequence <Symbol, Direction> const & s) {
        if (!s.is_annihilator())
            throw magma_not_convertible();
    }

    rime::true_type is_annihilator() const { return rime::true_; }
};

namespace detail {

    template <class Type> struct is_sequence_tag : boost::mpl::false_ {};
    template <class Symbol, class Direction>
        struct is_sequence_tag <sequence_tag <Symbol, Direction>>
    : boost::mpl::true_ {};

} // namespace detail

MATH_MAGMA_GENERATE_OPERATORS (detail::is_sequence_tag)

namespace operation {

    template <bool value> struct return_bool {
        template <class ... Arguments>
            rime::bool_ <value> operator() (Arguments const & ...) const
        { return rime::bool_ <value>(); }
    };

    namespace sequence_detail {

        /**
        Return the range direction associated with the direction.
        <c>range_direction<left>::type</c> is <c>::direction::front</c>.
        <c>range_direction<right>::type</c> is <c>::direction::back</c>.
        */
        template <class Direction> struct range_direction;

        template <> struct range_direction <left>
        { typedef ::direction::front type; };
        template <> struct range_direction <right>
        { typedef ::direction::back type; };

    } // namespace sequence_detail

    /* Queries. */
    template <class Symbol, class Direction>
        struct equal <sequence_tag <Symbol, Direction>>
    {
    private:
        struct when_first_annihilator {
            template <class Sequence1, class Sequence2> auto operator() (
                Sequence1 const & sequence1, Sequence2 const & sequence2) const
            RETURNS (sequence2.is_annihilator());
        };

        struct when_neither_annihilator {
            template <class Sequence1, class Sequence2> auto operator() (
                Sequence1 const & sequence1, Sequence2 const & sequence2) const
            RETURNS (range::equal (sequence1.symbols(), sequence2.symbols()));
        };

        struct when_first_not_annihilator {
            template <class Sequence1, class Sequence2> auto operator() (
                Sequence1 const & sequence1, Sequence2 const & sequence2) const
            RETURNS (rime::call_if (sequence2.is_annihilator(),
                return_bool <false>(), when_neither_annihilator(),
                sequence1, sequence2));
        };

    public:
        template <class Sequence1, class Sequence2> auto operator () (
            Sequence1 const & sequence1, Sequence2 const & sequence2) const
        RETURNS (rime::call_if (sequence1.is_annihilator(),
            when_first_annihilator(), when_first_not_annihilator(),
            sequence1, sequence2));
    };

    template <class Symbol, class Direction>
        struct compare <sequence_tag <Symbol, Direction>>
    {
    private:
        // It is recommended that the "best" element comes first in the strict
        // weak order, so the annihilator is last in the sorting order.
        struct when_first_annihilator {
            template <class Sequence1, class Sequence2> auto operator() (
                Sequence1 const & sequence1, Sequence2 const & sequence2) const
            RETURNS (rime::false_);
        };

        struct when_neither_annihilator {
            template <class Sequence1, class Sequence2> auto operator() (
                Sequence1 const & sequence1, Sequence2 const & sequence2) const
            RETURNS (range::less_lexicographical (
                typename sequence_detail::range_direction <Direction>::type(),
                sequence1.symbols(), sequence2.symbols()));
        };

        struct when_first_not_annihilator {
            template <class Sequence1, class Sequence2> auto operator() (
                Sequence1 const & sequence1, Sequence2 const & sequence2) const
            RETURNS (rime::call_if (sequence2.is_annihilator(),
                return_bool <true>(), when_neither_annihilator(),
                sequence1, sequence2));
        };

    public:
        template <class Sequence1, class Sequence2> auto operator () (
            Sequence1 const & sequence1, Sequence2 const & sequence2) const
        RETURNS (rime::call_if (sequence1.is_annihilator(),
            when_first_annihilator(), when_first_not_annihilator(),
            sequence1, sequence2));
    };

    /* Produce. */

    template <class Symbol, class Direction, class Sequence>
        struct identity <sequence_tag <Symbol, Direction>, callable::times,
            Sequence>
    {
        empty_sequence <Symbol, Direction> operator() () const
        { return empty_sequence <Symbol, Direction>(); }
    };

    template <class Symbol, class Direction, class Sequence>
        struct identity <sequence_tag <Symbol, Direction>, callable::plus,
            Sequence>
    {
        sequence_annihilator <Symbol, Direction> operator() () const
        { return sequence_annihilator <Symbol, Direction>(); }
    };

    // identity for choose: forward to identity for plus.
    template <class Symbol, class Direction, class Sequence>
        struct identity <sequence_tag <Symbol, Direction>, callable::choose,
            Sequence>
    : identity <sequence_tag <Symbol, Direction>, callable::plus, Sequence> {};

    template <class Symbol, class Direction, class Sequence>
        struct annihilator <sequence_tag <Symbol, Direction>, callable::times,
            Sequence>
    {
        sequence_annihilator <Symbol, Direction> operator() () const
        { return sequence_annihilator <Symbol, Direction>(); }
    };

    /* Operations. */

    /**
    To make this a semiring with "choose" and "times", the order for "choose"
    prefers shorter sequences and uses a lexicographical comparison as a
    tie-breaker.
    */
    template <class Symbol, class Direction>
        struct order <sequence_tag <Symbol, Direction>, callable::choose>
    {
        typedef typename sequence_detail::range_direction <Direction>::type
            range_direction;

        typedef empty_sequence <Symbol, Direction> empty;
        typedef single_sequence <Symbol, Direction> single;
        typedef optional_sequence <Symbol, Direction> optional;
        typedef sequence <Symbol, Direction> sequence_type;
        typedef sequence_annihilator <Symbol, Direction> annihilator;

        // Left is empty_sequence.
        rime::false_type operator() (
            empty const & left, empty const & right) const
        { return rime::false_; }

        rime::true_type operator() (
            empty const & left, single const & right) const
        { return rime::true_; }

        bool operator() (empty const & left, optional const & right) const
        { return !right.empty(); }

        rime::true_type operator() (
            empty const & left, annihilator const & right) const
        { return rime::true_; }

        bool operator() (empty const & left, sequence_type const & right) const
        { return right.is_annihilator() || !right.empty(); }

        // Left is single_sequence.
        rime::false_type operator() (
            single const & left, empty const & right) const
        { return rime::false_; }

        bool operator() (single const & left, single const & right) const
        { return left.symbol() < right.symbol(); }

        bool operator() (single const & left, optional const & right) const {
            if (right.empty())
                return false;
            else
                return left.symbol() < right.symbol();
        }

        rime::true_type operator() (
            single const & left, annihilator const & right) const
        { return rime::true_; }

        bool operator() (single const & left, sequence_type const & right) const
        {
            if (right.is_annihilator())
                return true;
            if (right.empty())
                return false;
            if (range::size (right.symbols()) == 1)
                return left.symbol()
                    < range::first (range_direction(), right.symbols());
            return true;
        }

        // Left is optional_sequence.

        rime::false_type operator() (
            optional const & left, empty const & right) const
        { return rime::false_; }

        bool operator() (optional const & left, single const & right) const {
            if (left.empty())
                return true;
            else
                return left.symbol() < right.symbol();
        }

        bool operator() (optional const & left, optional const & right) const {
            if (left.empty())
                return !right.empty();
            if (right.empty())
                return false;
            return left.symbol() < right.symbol();
        }

        rime::true_type operator() (
            optional const & left, annihilator const & right) const
        { return rime::true_; }

        bool operator() (optional const & left, sequence_type const & right)
            const
        {
            if (right.is_annihilator())
                return true;
            if (left.empty())
                return !right.empty();
            if (right.empty())
                return false;
            if (range::size (right.symbols()) == 1)
                return left.symbol()
                    < range::first (range_direction(), right.symbols());
            return true;
        }

        // Left is an annihilator.
        template <class Sequence>
        rime::false_type operator() (
            annihilator const & left, Sequence const & right) const
        { return rime::false_; }

        // Left is a sequence.
        bool operator() (sequence_type const & left, empty const & right) const
        { return !left.is_annihilator() && left.empty(); }

        bool operator() (sequence_type const & left, single const & right) const
        {
            if (left.is_annihilator())
                return false;
            if (left.empty())
                return true;
            if (range::size (left.symbols()) == 1)
                return range::first (range_direction(), left.symbols())
                    < right.symbol();
            return false;
        }

        bool operator() (sequence_type const & left, optional const & right)
            const
        {
            if (left.is_annihilator())
                return false;
            if (left.empty())
                return !right.empty();
            if (right.empty())
                return false;
            if (range::size (left.symbols()) == 1)
                return range::first (range_direction(), left.symbols())
                    < right.symbol();
            return false;
        }

        rime::false_type operator() (
            sequence_type const & left, annihilator const & right) const
        { return rime::false_; }

        bool operator() (
            sequence_type const & left, sequence_type const & right) const
        {
            if (left.is_annihilator())
                return false;
            if (right.is_annihilator())
                return true;
            auto && left_symbols = left.symbols();
            auto && right_symbols = right.symbols();
            if (range::size (left_symbols) < range::size (right_symbols))
                return true;
            if (range::size (right_symbols) < range::size (left_symbols))
                return false;
            return range::less_lexicographical (left_symbols, right_symbols);
        }
    };

    // Even though "order" is defined, this overrides the default implementation
    // to get the types right.
    template <class Symbol, class Direction>
        struct choose <sequence_tag <Symbol, Direction>>
    : associative, commutative, path_operation
    {
        typedef typename sequence_detail::range_direction <Direction>::type
            range_direction;

        typedef empty_sequence <Symbol, Direction> empty;
        typedef single_sequence <Symbol, Direction> single;
        typedef optional_sequence <Symbol, Direction> optional;
        typedef sequence <Symbol, Direction> sequence_type;
        typedef sequence_annihilator <Symbol, Direction> annihilator;

        struct implementation {
            // One argument is known to be an annihilator.
            template <class Sequence> Sequence operator() (
                annihilator const &, Sequence const & right,
                utility::overload_order <1> *) const
            { return right; }

            template <class Sequence> Sequence operator() (
                Sequence const & left, annihilator const &,
                utility::overload_order <2> *) const
            { return left; }

            // One argument is known-empty.
            template <class Right> empty operator() (
                empty const &, Right const &,
                utility::overload_order <3> *) const
            { return empty(); }

            template <class Left> empty operator() (
                Left const &, empty const &,
                utility::overload_order <4> *) const
            { return empty(); }

            // One argument is known-single.
            single operator() (single const & left, single const & right,
                utility::overload_order <5> *) const
            {
                if (left.symbol() < right.symbol())
                    return left;
                else
                    return right;
            }

            // Case where one argument is at most one symbol long.
            template <class Short, class Long> optional when_short (
                Short const & s, Long const & l) const
            {
                if (l.is_annihilator())
                    return s;
                if (s.empty() || l.empty())
                    return optional();
                if (!range::empty (range::drop (range_direction(),
                        l.symbols())))
                    // l is more than one symbol long.
                    return s;
                if (s.symbol() < range::first (range_direction(), l.symbols()))
                    return s;
                else
                    return optional (l);
            }

            // One argument is an optional_sequence.
            template <class Sequence> optional operator() (
                optional const & left, Sequence const & right,
                utility::overload_order <6> *) const
            { return when_short (left, right); }

            template <class Sequence> optional operator() (
                Sequence const & left, optional const & right,
                utility::overload_order <7> *) const
            { return when_short (right, left); }

            // One argument is an single_sequence.
            template <class Sequence> optional operator() (
                single const & left, Sequence const & right,
                utility::overload_order <6> *) const
            { return when_short (left, right); }

            template <class Sequence> optional operator() (
                Sequence const & left, single const & right,
                utility::overload_order <7> *) const
            { return when_short (right, left); }

            // Both are sequences.
            sequence_type operator() (
                sequence_type const & left, sequence_type const & right,
                utility::overload_order <8> *) const
            {
                if (left.is_annihilator())
                    return right;
                if (right.is_annihilator())
                    return left;
                auto left_size = range::size (left.symbols());
                auto right_size = range::size (right.symbols());
                if (left_size < right_size)
                    return left;
                if (right_size < left_size)
                    return right;
                if (range::less_lexicographical (
                        left.symbols(), right.symbols()))
                    return left;
                else
                    return right;
            }
        };

        template <class Left, class Right> auto operator() (
            Left const & left, Right const & right) const
        RETURNS (implementation() (left, right, utility::pick_overload()));
    };

    // Since "choose" selects the shortest sequence first, and compares
    // lexicographically as a tie-breaker, sequences in one direction are
    // semirings in both directions.
    template <class Symbol, class Direction> struct is_semiring <
        sequence_tag <Symbol, Direction>, either,
        callable::times, callable::choose>
    : rime::true_type {};

    // Concatenate
    template <class Symbol, class Direction>
        struct times <sequence_tag <Symbol, Direction>>
    : associative
    {
    private:
        typedef sequence_annihilator <Symbol, Direction> annihilator_type;
        typedef empty_sequence <Symbol, Direction> empty_type;
        typedef single_sequence <Symbol, Direction> single_type;
        typedef optional_sequence <Symbol, Direction> optional_type;
        typedef sequence <Symbol, Direction> sequence_type;

        struct implementation {
            // Annihilator: annihilates.
            auto operator() (
                annihilator_type const & left, annihilator_type const &,
                utility::overload_order <1> *) const
            RETURNS (left);

            template <class Sequence> auto operator() (
                annihilator_type const & annihilator, Sequence const &,
                utility::overload_order <1> *) const
            RETURNS (annihilator);

            template <class Sequence> auto operator() (
                Sequence const &, annihilator_type const & annihilator,
                utility::overload_order <1> *) const
            RETURNS (annihilator);

            // Empty symbol: return the other one.
            auto operator() (
                empty_type const & left, empty_type const &,
                utility::overload_order <2> *) const
            RETURNS (left);

            template <class Sequence> auto operator() (
                empty_type const &, Sequence const & right,
                utility::overload_order <2> *) const
            RETURNS (right);

            template <class Sequence> auto operator() (
                Sequence const & left, empty_type const &,
                utility::overload_order <2> *) const
            RETURNS (left);

            // Non-empty: both operands could be three types.

            void append_symbols (
                std::vector <Symbol> & symbols, single_type const & s) const
            { symbols.push_back (s.symbol()); }

            void append_symbols (
                std::vector <Symbol> & symbols, optional_type const & s) const
            {
                if (!s.empty())
                    symbols.push_back (s.symbol().get());
            }

            void append_symbols (
                std::vector <Symbol> & symbols, sequence_type const & s) const
            {
                auto && s_symbols = s.symbols();
                symbols.insert (symbols.end(),
                    s_symbols.begin(), s_symbols.end());
            }

            template <class Sequence1, class Sequence2>
                sequence <Symbol, Direction> operator() (
                    Sequence1 const & sequence1, Sequence2 const & sequence2,
                    utility::overload_order <3> *) const
            {
                if (sequence1.is_annihilator())
                    return sequence1;
                if (sequence2.is_annihilator())
                    return sequence2;

                std::vector <Symbol> concatenation;
                append_symbols (concatenation, sequence1);
                append_symbols (concatenation, sequence2);
                return sequence <Symbol, Direction> (std::move (concatenation));
            }
        };

    public:
        template <class Left, class Right> auto operator() (
            Left const & left, Right const & right) const
        RETURNS (implementation() (left, right, utility::pick_overload()));

        // This returns the wrong type but is useful for debugging:
        /*
        template <class Left, class Right> sequence <Symbol, Direction>
            operator() (Left const & left, Right const & right) const
        { return implementation() (left, right, utility::pick_overload()); }
        */
    };

    /**
    Addition for left sequences: find the longest common prefix.
    */
    template <class Symbol, class Direction>
        struct plus <sequence_tag <Symbol, Direction>>
    : associative, commutative, idempotent
    {
    private:
        typedef sequence_annihilator <Symbol, Direction> annihilator_type;
        typedef empty_sequence <Symbol, Direction> empty_type;
        typedef single_sequence <Symbol, Direction> single_type;
        typedef optional_sequence <Symbol, Direction> optional_type;
        typedef sequence <Symbol, Direction> sequence_type;

        struct implementation {
            // Annihalator is the additive identity.
            template <class Sequence> auto operator() (
                annihilator_type const &, Sequence const & right,
                utility::overload_order <1> *) const
            RETURNS (right);

            template <class Sequence> auto operator() (
                Sequence const & left, annihilator_type const &,
                utility::overload_order <2> *) const
            RETURNS (left);

            // Empty sequence: return the empty sequence.
            template <class Sequence> auto operator() (
                empty_type const & empty,
                Sequence const &,
                utility::overload_order <3> *) const
            RETURNS (empty);

            template <class Sequence> auto operator() (
                Sequence const &, empty_type const & empty,
                utility::overload_order <4> *) const
            RETURNS (empty);

            // Two sequences: different implementation for left and right.
            /*
            \todo This should really be one implementation for left and right
            sequences.
            This is easy (take the implementation for the right sequence,and
            s/range::back/direction/), except for dealing with "push_back".
            This requires range::subrange.
            */
            sequence <Symbol, left> operator() (
                sequence <Symbol, left> const & sequence1,
                sequence <Symbol, left> const & sequence2,
                utility::overload_order <5> * pick) const
            {
                // Annihalator is the additive identity.
                if (sequence1.is_annihilator())
                    return sequence2;
                if (sequence2.is_annihilator())
                    return sequence1;

                // This should be replaced by something with range::subrange.
                std::vector <Symbol> longest_common_prefix;
                auto symbols1 = range::view (sequence1.symbols());
                auto symbols2 = range::view (sequence2.symbols());
                while (!range::empty (symbols1) && !range::empty (symbols2)
                    && range::first (symbols1) == range::first (symbols2))
                {
                    longest_common_prefix.push_back (range::first (symbols1));
                    symbols1 = range::drop (symbols1);
                    symbols2 = range::drop (symbols2);
                }
                return sequence <Symbol, left> (longest_common_prefix);
            }

            sequence <Symbol, right> operator() (
                sequence <Symbol, right> const & sequence1,
                sequence <Symbol, right> const & sequence2,
                utility::overload_order <5> * pick) const
            {
                // Annihalator is the additive identity.
                if (sequence1.is_annihilator())
                    return sequence2;
                if (sequence2.is_annihilator())
                    return sequence1;

                std::list <Symbol> longest_common_prefix;
                auto symbols1 = range::view (range::back, sequence1.symbols());
                auto symbols2 = range::view (range::back, sequence2.symbols());
                while (!range::empty (range::back, symbols1)
                    && !range::empty (range::back, symbols2)
                    && range::first (range::back, symbols1)
                        == range::first (range::back, symbols2))
                {
                    longest_common_prefix.push_front (
                        range::first (range::back, symbols1));
                    symbols1 = range::drop (range::back, symbols1);
                    symbols2 = range::drop (range::back, symbols2);
                }
                return sequence <Symbol, right> (longest_common_prefix);
            }

            // At least one of the arguments has zero or one, or one elements.
            // The common prefix therefore is zero or one elements long.

            static Symbol const & first_symbol (single_type const & s)
            { return s.symbol(); }
            static Symbol const & first_symbol (optional_type const & s)
            { return s.symbol().get(); }

            static Symbol const & first_symbol (
                sequence <Symbol, left> const & s)
            { return range::first (range::front, s.symbols()); }
            static Symbol const & first_symbol (
                sequence <Symbol, right> const & s)
            { return range::first (range::back, s.symbols()); }

            template <class Sequence1, class Sequence2>
                optional_type operator() (
                    Sequence1 const & sequence1, Sequence2 const & sequence2,
                    utility::overload_order <5> * pick) const
            {
                typedef optional_type result_type;
                // Annihilator is the additive identity.
                if (sequence1.is_annihilator())
                    return result_type (sequence2);
                if (sequence2.is_annihilator())
                    return result_type (sequence1);

                if (sequence1.empty() || sequence2.empty())
                    return result_type();

                Symbol const & symbol = first_symbol (sequence1);
                if (symbol == first_symbol (sequence2))
                    return result_type (symbol);
                else
                    return result_type();
            }
        };

    public:
        template <class Left, class Right> auto operator() (
            Left const & left, Right const & right) const
        RETURNS (implementation() (left, right, utility::pick_overload()));

        // This returns the wrong type but is useful for debugging:
        /*
        template <class Left, class Right> sequence <Symbol, Direction>
            operator() (Left const & left, Right const & right) const
        { return implementation() (left, right, utility::pick_overload()); }
        */
    };

    // The direction matches the direction of the sequence:
    // left and right sequences are left and right semirings over times and
    // plus.
    template <class Symbol, class Direction> struct is_semiring <
        sequence_tag <Symbol, Direction>, Direction,
        callable::times, callable::plus>
    : rime::true_type {};

    /**
    The prefix of the dividend must be equal to the divisor.
    \return The dividend without the prefix.
    */
    template <class Symbol, class Direction>
        struct divide <sequence_tag <Symbol, Direction>, Direction>
    : throw_if_undefined
    {
        typedef sequence <Symbol, Direction> sequence_type;
        typedef empty_sequence <Symbol, Direction> empty_sequence_type;
        typedef single_sequence <Symbol, Direction> single_sequence_type;
        typedef optional_sequence <Symbol, Direction> optional_sequence_type;
        typedef sequence_annihilator <Symbol, Direction>
            sequence_annihilator_type;

        struct implementation {
            /**
            Implementation for the run-time type.
            This works for all sequences, but it returns the general type, which
            is not efficient.
            */
            sequence_type operator() (sequence_type const & dividend,
                sequence_type const & divisor, utility::overload_order <10> *)
            const
            {
                if (dividend.is_annihilator()) {
                    if (divisor.is_annihilator())
                        throw operation_undefined();
                    else
                        return dividend;
                } else if (divisor.is_annihilator())
                    throw divide_by_zero();

                // Start at the front of the sequence for a left division, and
                // at the back for a right division.
                auto start = typename
                    sequence_detail::range_direction <Direction>::type();
                auto divisor_symbols = range::view (start, divisor.symbols());
                auto dividend_symbols = range::view (start, dividend.symbols());
                while (!range::empty (start, divisor_symbols)) {
                    if (range::empty (start, dividend_symbols))
                        throw operation_undefined();
                    if (range::first (start, dividend_symbols)
                            != range::first (start, divisor_symbols))
                        throw operation_undefined();
                    divisor_symbols = range::drop (start, divisor_symbols);
                    dividend_symbols = range::drop (start, dividend_symbols);
                }
                return sequence_type (dividend_symbols);
            }

            /* Specialisations. */

            /* 1. annihilators. */

            sequence_type operator() (
                sequence_annihilator_type const & dividend,
                sequence_annihilator_type const & divisor,
                utility::overload_order <1> *) const
            { throw operation_undefined(); }

            // Divide by annihilator.
            template <class Dividend> sequence_type operator() (
                Dividend const & dividend,
                sequence_annihilator_type const & divisor,
                utility::overload_order <1> *) const
            { throw divide_by_zero(); }

            // Divide annihilator by something else.
            template <class Divisor> sequence_annihilator_type operator() (
                sequence_annihilator_type const & dividend,
                Divisor const & divisor, utility::overload_order <1> *) const
            {
                if (divisor.is_annihilator())
                    throw operation_undefined();
                else
                    return dividend;
            }

            /* 2. Empty divisor. */
            template <class Dividend> auto operator() (
                Dividend const & dividend, empty_sequence_type const & divisor,
                utility::overload_order <2> *) const
            RETURNS (dividend);

            /* 3. Empty dividend. */
            template <class Divisor> empty_sequence_type operator() (
                empty_sequence_type const & dividend, Divisor const & divisor,
                utility::overload_order <3> *) const
            {
                if (divisor.is_annihilator())
                    throw divide_by_zero();
                else if (!divisor.empty())
                    throw operation_undefined();
                else
                    return dividend;
            }

            /* 4. Combinations of single, optional, and general sequence. */

            Symbol const & first_symbol (sequence <Symbol, left> const & s)
                const
            { return range::first (s.symbols()); }

            Symbol const & first_symbol (sequence <Symbol, right> const & s)
                const
            { return range::first (range::back, s.symbols()); }

            empty_sequence_type operator() (
                single_sequence_type const & dividend,
                single_sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (dividend.symbol() == divisor.symbol())
                    return empty_sequence_type();
                else
                    throw operation_undefined();
            }

            optional_sequence_type operator() (
                single_sequence_type const & dividend,
                optional_sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                // Correct case.
                if (divisor.empty())
                    return dividend;
                if (dividend.symbol() == divisor.symbol().get())
                    return empty_sequence_type();
                throw operation_undefined();
            }

            optional_sequence_type operator() (
                single_sequence_type const & dividend,
                sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (divisor.is_annihilator())
                    throw divide_by_zero();
                // Correct case.
                if (divisor.empty())
                    return dividend;
                if (range::size (divisor.symbols()) == 1
                        && dividend.symbol() == first_symbol (divisor))
                    return empty_sequence_type();
                // Otherwise, the division is not defined.
                throw operation_undefined();
            }

            // Optional-symbol dividend.
            empty_sequence_type operator() (
                optional_sequence_type const & dividend,
                single_sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (!dividend.empty()
                        && dividend.symbol().get() == divisor.symbol())
                    return empty_sequence_type();
                throw operation_undefined();
            }

            optional_sequence_type operator() (
                optional_sequence_type const & dividend,
                optional_sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (divisor.empty())
                    return dividend;
                if (!dividend.empty() && !divisor.empty()
                        && dividend.symbol() == divisor.symbol())
                    return empty_sequence_type();
                throw operation_undefined();
            }

            optional_sequence_type operator() (
                optional_sequence_type const & dividend,
                sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (divisor.is_annihilator())
                    throw divide_by_zero();
                // Correct case.
                if (divisor.empty())
                    return dividend;
                if (!dividend.empty()
                        && range::size (divisor.symbols()) == 1
                        && dividend.symbol() == first_symbol (divisor))
                    return empty_sequence_type();
                // Otherwise, the division is not defined.
                throw operation_undefined();
            }

            // Negotiate between std::vector and the Range library.
            std::vector <Symbol> drop_copy (
                std::vector <Symbol> const & symbols) const
            {
                auto result_symbols_source = range::drop (typename
                    sequence_detail::range_direction <Direction>::type(),
                    symbols);
                return std::vector <Symbol> (
                    result_symbols_source.begin(), result_symbols_source.end());
            }

            // Divisor is a single_sequence or an optional_sequence.
            sequence_type operator() (
                sequence_type const & dividend,
                single_sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (dividend.is_annihilator())
                    return dividend;

                if (dividend.empty())
                    throw operation_undefined();
                if (first_symbol (dividend) != divisor.symbol())
                    throw operation_undefined();

                return sequence_type (drop_copy (dividend.symbols()));
            }

            sequence_type operator() (
                sequence_type const & dividend,
                optional_sequence_type const & divisor,
                utility::overload_order <4> *) const
            {
                if (divisor.empty() || dividend.is_annihilator())
                    return dividend;

                if (dividend.empty())
                    throw operation_undefined();
                if (first_symbol (dividend) != divisor.symbol().get())
                    throw operation_undefined();

                return sequence_type (drop_copy (dividend.symbols()));
            }
        };

        template <class Sequence1, class Sequence2> auto operator() (
            Sequence1 const & sequence1, Sequence2 const & sequence2) const
        RETURNS (
            implementation() (sequence1, sequence2, utility::pick_overload()));
    };

    template <class Symbol, class Direction>
        struct reverse <sequence_tag <Symbol, Direction>, callable::times>
    {
        typedef typename opposite_direction <Direction>::type other_direction;

        sequence <Symbol, other_direction> operator() (
            sequence <Symbol, Direction> const & s) const
        {
            if (s.is_annihilator())
                return sequence_annihilator <Symbol, other_direction>();
            else {
                std::vector <Symbol> symbols = s.symbols();
                std::reverse (symbols.begin(), symbols.end());
                return sequence <Symbol, other_direction> (std::move (symbols));
            }
        }

        empty_sequence <Symbol, other_direction> operator() (
            empty_sequence <Symbol, Direction> const & s) const
        { return empty_sequence <Symbol, other_direction>(); }

        single_sequence <Symbol, other_direction> operator() (
            single_sequence <Symbol, Direction> const & s) const
        { return single_sequence <Symbol, other_direction> (s.symbol()); }

        sequence_annihilator <Symbol, other_direction> operator() (
            sequence_annihilator <Symbol, Direction> const & s) const
        { return sequence_annihilator <Symbol, other_direction>(); }
    };

    template <class Symbol, class Direction>
        struct print <sequence_tag <Symbol, Direction>>
    {
        template <class Stream>
            void operator() (Stream & stream,
                sequence <Symbol, Direction> const & s) const
        {
            if (s.is_annihilator())
                stream << "<annihilator>";
            else {
                stream << "[ ";
                RANGE_FOR_EACH (symbol, s.symbols())
                    stream << symbol << " ";
                stream << "]";
            }
        }

        template <class Stream>
            void operator() (Stream & stream,
                empty_sequence <Symbol, Direction> const &) const
        { stream << "[]"; }

        template <class Stream>
            void operator() (Stream & stream,
                single_sequence <Symbol, Direction> const & s) const
        { stream << "[ " << s.symbol() << "]"; }

        template <class Stream>
            void operator() (Stream & stream,
                sequence_annihilator <Symbol, Direction> const &) const
        { stream << "<annihilator>"; }
    };

    /**
    If the sequence types are the same, return that type.
    If the sequence types are not the same, return sequence <...> or
    optional_sequence <...>.
    */
    // Same type.
    template <class Symbol, class Direction, class Sequence>
        struct unify_type <sequence_tag <Symbol, Direction>, Sequence, Sequence>
    { typedef Sequence type; };

    template <class Symbol, class Direction, class Sequence1, class Sequence2>
        struct unify_type <sequence_tag <Symbol, Direction>,
            Sequence1, Sequence2>
    { typedef sequence <Symbol, Direction> type; };

    // Mixing empty/single sequences.
    template <class Symbol, class Direction>
        struct unify_type <sequence_tag <Symbol, Direction>,
            empty_sequence <Symbol, Direction>,
            single_sequence <Symbol, Direction>>
    { typedef optional_sequence <Symbol, Direction> type; };

    template <class Symbol, class Direction>
        struct unify_type <sequence_tag <Symbol, Direction>,
            empty_sequence <Symbol, Direction>,
            optional_sequence <Symbol, Direction>>
    { typedef optional_sequence <Symbol, Direction> type; };

    template <class Symbol, class Direction>
        struct unify_type <sequence_tag <Symbol, Direction>,
            single_sequence <Symbol, Direction>,
            empty_sequence <Symbol, Direction>>
    { typedef optional_sequence <Symbol, Direction> type; };

    template <class Symbol, class Direction>
        struct unify_type <sequence_tag <Symbol, Direction>,
            single_sequence <Symbol, Direction>,
            optional_sequence <Symbol, Direction>>
    { typedef optional_sequence <Symbol, Direction> type; };

    template <class Symbol, class Direction>
        struct unify_type <sequence_tag <Symbol, Direction>,
            optional_sequence <Symbol, Direction>,
            empty_sequence <Symbol, Direction>>
    { typedef optional_sequence <Symbol, Direction> type; };

    template <class Symbol, class Direction>
        struct unify_type <sequence_tag <Symbol, Direction>,
            optional_sequence <Symbol, Direction>,
            single_sequence <Symbol, Direction>>
    { typedef optional_sequence <Symbol, Direction> type; };

} // namespace operation

} // namespace math

#endif // MATH_SEQUENCE_HPP_INCLUDED
