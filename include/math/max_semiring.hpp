/*
Copyright 2013-2015 Rogier van Dalen.

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

#ifndef MATH_MAX_SEMIRING_HPP_INCLUDED
#define MATH_MAX_SEMIRING_HPP_INCLUDED

#include <limits>
#include <iosfwd>
#include <type_traits>

#include <boost/mpl/bool.hpp>

#include <boost/functional/hash_fwd.hpp>

#include "magma.hpp"

#include "detail/is_close.hpp"

namespace math {

/**
Semiring whose \ref plus (and \ref choose) operation picks the maximum of the
two values.
\ref times performs multiplication on the underlying value.

The underlying value must always be non-negative, so that the additive identity
has value 0.
The multiplicative identity has value 1.

Division is only implemented for non-integer types.

This type supports Boost.Hash, if \c boost/functional/hash.hpp is included.

\tparam Type
    Underlying type that represents the value.
*/
template <class Type> class max_semiring;

template <class Type> struct max_semiring_tag;

template <class Type> struct decayed_magma_tag <max_semiring <Type>>
{ typedef max_semiring_tag <Type> type; };

template <class Type> class max_semiring {
private:
    Type value_;
public:
    /**
    Initialise with 0.
    */
    max_semiring() : value_()
    { assert (value_ >= Type (0)); }

    /**
    Initialise with \a value as the value.
    */
    explicit max_semiring (Type const & value) : value_ (value) {}

    /**
    Return the underlying value.
    */
    Type const & value() const { return value_; }
};

namespace detail {

    template <class Type> struct is_max_semiring_tag : boost::mpl::false_ {};
    template <class Type> struct is_max_semiring_tag <max_semiring_tag <Type>>
    : boost::mpl::true_ {};

} // namespace detail

MATH_MAGMA_GENERATE_OPERATORS (detail::is_max_semiring_tag)

namespace operation {

    /* Queries. */

    template <class Type>
        struct is_member <max_semiring_tag <Type>, typename std::enable_if <
            std::numeric_limits <Type>::has_quiet_NaN>::type>
    {
        // Detect NaN.
        bool operator() (max_semiring <Type> const & v) const { return v == v; }
    };

    template <class Type> struct equal <max_semiring_tag <Type>> {
        bool operator() (
            max_semiring <Type> const & left, max_semiring <Type> const & right)
            const
        { return left.value() == right.value(); }
    };

    template <class Type> struct approximately_equal <max_semiring_tag <Type>,
        typename std::enable_if <!std::numeric_limits <Type>::is_exact>::type>
    {
        bool operator() (
            max_semiring <Type> const & left, max_semiring <Type> const & right)
            const
        {
            static Type const tolerance = 1e-5;
            return math::detail::is_close (left.value(), right.value(),
                tolerance);
        }
    };

    // compare always orders smaller values first.
    template <class Type> struct compare <max_semiring_tag <Type>> {
        bool operator() (
            max_semiring <Type> const & left, max_semiring <Type> const & right)
            const
        { return left.value() < right.value(); }
    };

    // "choose" and "plus" both select the maximal element.
    template <class Type>
    struct order <max_semiring_tag <Type>, callable::choose>
    : reverse_order <compare <max_semiring_tag <Type>>> {};

    template <class Type>
    struct order <max_semiring_tag <Type>, callable::plus>
    : reverse_order <compare <max_semiring_tag <Type>>> {};

    /* Produce. */

    // Return the not-a-number value, not-a-number, if it is available.
    template <class Type> struct non_member <max_semiring_tag <Type>, typename
        std::enable_if <std::numeric_limits <Type>::has_quiet_NaN>::type>
    {
        max_semiring <Type> operator() () const {
            return max_semiring <Type> (
                std::numeric_limits <Type>::quiet_NaN());
        }
    };

    template <class Type>
        struct identity <max_semiring_tag <Type>, callable::times>
    {
        max_semiring <Type> operator() () const
        { return max_semiring <Type> (1); }
    };

    template <class Type>
        struct identity <max_semiring_tag <Type>, callable::choose>
    {
        max_semiring <Type> operator() () const
        { return max_semiring <Type> (0); }
    };

    // Additive identity: same as "choose".
    template <class Type>
        struct identity <max_semiring_tag <Type>, callable::plus>
    : identity <max_semiring_tag <Type>, callable::choose> {};

    // Multiplicative annihilator: forward to additive identity.
    template <class Type>
        struct annihilator <max_semiring_tag <Type>, callable::times>
    : identity <max_semiring_tag <Type>, callable::plus> {};

    /* Operations. */

    // "choose" and "plus" are implemented automatically, since "order" is
    // implemented for them.

    template <class Type> struct times <max_semiring_tag <Type>>
    : associative, commutative, approximate_if <
        boost::mpl::bool_ <!std::numeric_limits <Type>::is_exact>>
    {
        max_semiring <Type> operator() (max_semiring <Type> const & left,
            max_semiring <Type> const & right) const
        { return max_semiring <Type> (left.value() * right.value()); }
    };

    // Semiring in both directions.
    template <class Type> struct is_semiring <
        max_semiring_tag <Type>, either, callable::times, callable::choose>
    : rime::true_type {};

    template <class Type> struct is_semiring <
        max_semiring_tag <Type>, either, callable::times, callable::plus>
    : rime::true_type {};

    // Division is only implemented for non-integer types.
    template <class Type> struct divide <max_semiring_tag <Type>, either,
        typename std::enable_if <!std::numeric_limits <Type>::is_integer>::type>
    : approximate_if <boost::mpl::bool_ <!std::numeric_limits <Type>::is_exact>>
    {
        max_semiring <Type> operator() (max_semiring <Type> const & left,
            max_semiring <Type> const & right) const
        { return max_semiring <Type> (left.value() / right.value()); }
    };

    // Inversion is only implemented for non-integer types.
    template <class Type>
        struct invert <max_semiring_tag <Type>, either, callable::times,
        typename std::enable_if <!std::numeric_limits <Type>::is_integer>::type>
    {
        max_semiring <Type> operator() (max_semiring <Type> const & c) const
        { return max_semiring <Type> (1 / c.value()); }
    };

    template <class Type> struct print <max_semiring_tag <Type>> {
        template <class Stream>
            void operator() (Stream & stream, max_semiring <Type> const & c)
            const
        { stream << c.value(); }
    };

} // namespace operation

template <class Type>
    inline std::size_t hash_value (max_semiring <Type> const & m)
{ return boost::hash <Type>() (m.value()); }

} // namespace math

#endif // MATH_MAX_SEMIRING_HPP_INCLUDED
