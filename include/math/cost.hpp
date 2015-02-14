/*
Copyright 2013, 2014 Rogier van Dalen.

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

#ifndef MATH_COST_HPP_INCLUDED
#define MATH_COST_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <iosfwd>
#include <functional>

#include <boost/utility/enable_if.hpp>
#include <boost/functional/hash_fwd.hpp>

#include "magma.hpp"

#include "detail/is_close.hpp"

namespace math {

/**
Semiring that is helpful to minimise a cost.
It is also known as the "tropical" semiring.

\ref times adds costs; \ref plus and \ref choose pick the lowest-cost argument.

This type supports Boost.Hash, if \c boost/functional/hash.hpp is included.

\tparam Type
    Arithmetic type that represents the cost.
    The type needs to be able to represent infinity, so that the additive
    identity can be implemented.
    <c>std::numeric_limits<Type>::has_infinity</c> must be \c true.
*/
template <class Type> class cost;

template <class Type> struct cost_tag;

template <class Type> struct decayed_magma_tag <cost <Type>>
{ typedef cost_tag <Type> type; };

template <class Type> class cost {
private:
    Type value_;

    static_assert (std::numeric_limits <Type>::has_infinity,
        "The additive identity is the infinity, which type 'Type' must have.");
public:
    /**
    Initialise with infinite cost.
    This results in the additive identity, also known as "zero".
    \post <c>this->value() == std::numeric_limits<Type>::infinity()</c>
    */
    cost()
    : value_(std::numeric_limits <Type>::infinity()) {}

    /**
    Initialise with a cost of \a value.
    */
    explicit cost (Type const & value) : value_ (value) {}

    /**
    Return the underlying value.
    */
    Type const & value() const { return value_; }
};

template <class Type> inline std::size_t hash_value (cost <Type> const & c)
{ return boost::hash <Type>() (c.value()); }

namespace detail {

    template <class Type> struct is_cost_tag : boost::mpl::false_ {};
    template <class Type> struct is_cost_tag <cost_tag <Type>>
    : boost::mpl::true_ {};

} // namespace detail

MATH_MAGMA_GENERATE_OPERATORS (detail::is_cost_tag)

namespace operation {

    /* Queries. */

    template <class Type>
        struct is_member <cost_tag <Type>, typename boost::enable_if_c <
            std::numeric_limits <Type>::has_quiet_NaN>::type>
    {
        // Detect NaN.
        bool operator() (cost <Type> const & v) const { return v == v; }
    };

    template <class Type> struct equal <cost_tag <Type>> {
        bool operator() (cost <Type> const & left, cost <Type> const & right)
            const
        { return left.value() == right.value(); }
    };

    template <class Type> struct approximately_equal <cost_tag <Type>> {
        bool operator() (cost <Type> const & left, cost <Type> const & right)
            const
        {
            static Type const tolerance = 1e-5;
            return math::detail::is_close (left.value(), right.value(),
                tolerance);
        }
    };

    template <class Type> struct compare <cost_tag <Type>> {
        bool operator() (cost <Type> const & left, cost <Type> const & right)
            const
        { return left.value() < right.value(); }
    };

    // "choose" and "plus" both select the lowest-cost element.
    template <class Type> struct order <cost_tag <Type>, callable::choose>
    : compare <cost_tag <Type>> {};

    template <class Type> struct order <cost_tag <Type>, callable::plus>
    : compare <cost_tag <Type>> {};

    /* Produce. */

    // Return the not-a-number value, not-a-number, if it is available.
    template <class Type> struct non_member <cost_tag <Type>, typename
        boost::enable_if_c <std::numeric_limits <Type>::has_quiet_NaN>::type>
    {
        cost <Type> operator() () const
        { return cost <Type> (std::numeric_limits <Type>::quiet_NaN()); }
    };

    template <class Type> struct identity <cost_tag <Type>, callable::times>
    { cost <Type> operator() () const { return cost <Type> (0); } };

    template <class Type> struct identity <cost_tag <Type>, callable::choose> {
        cost <Type> operator() () const
        { return cost <Type> (std::numeric_limits <Type>::infinity()); }
    };

    template <class Type> struct identity <cost_tag <Type>, callable::plus>
    : identity <cost_tag <Type>, callable::choose> {};

    // Multiplicative annihilator: forward to additive identity.
    template <class Type> struct annihilator <cost_tag <Type>, callable::times>
    : identity <cost_tag <Type>, callable::plus> {};

    /* Operations. */

    // "choose" and "plus" are implemented implicitly, since "order" is
    // implemented for them.

    template <class Type> struct times <cost_tag <Type>>
    : approximate, associative, commutative
    {
        cost <Type> operator() (
            cost <Type> const & left, cost <Type> const & right) const
        { return cost <Type> (left.value() + right.value()); }
    };

    // Semiring in both directions.
    template <class Type> struct is_semiring <
        cost_tag <Type>, either, callable::times, callable::choose>
    : rime::true_type {};

    template <class Type> struct is_semiring <
        cost_tag <Type>, either, callable::times, callable::plus>
    : rime::true_type {};

    template <class Type> struct divide <cost_tag <Type>, either>
    : approximate
    {
        cost <Type> operator() (
            cost <Type> const & left, cost <Type> const & right) const
        { return cost <Type> (left.value() - right.value()); }
    };

    template <class Type>
        struct invert <cost_tag <Type>, either, callable::times>
    {
        cost <Type> operator() (cost <Type> const & c) const
        { return cost <Type> (-c.value()); }
    };

    template <class Type> struct print <cost_tag <Type>> {
        template <class Stream>
            void operator() (Stream & stream, cost <Type> const & c) const
        { stream << c.value(); }
    };

} // namespace operation

} // namespace math

#endif // MATH_COST_HPP_INCLUDED
