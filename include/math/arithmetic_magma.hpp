/*
Copyright 2014 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/** \file
Define magma properties for number types.
*/

#ifndef MATH_ARITHMETIC_MAGMA_HPP_INCLUDED
#define MATH_ARITHMETIC_MAGMA_HPP_INCLUDED

#include <cmath>
#include <limits>
#include <type_traits>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/utility/enable_if.hpp>

#include "magma.hpp"

#include "detail/is_close.hpp"

namespace math {

template <class Type> struct arithmetic_magma_tag;


template <class Type> struct decayed_magma_tag <Type, typename
    boost::enable_if_c <(
        std::numeric_limits <Type>::is_specialized
        && !std::is_same <Type, bool>::value)
    >::type>
{ typedef arithmetic_magma_tag <Type> type; };

namespace operation {

    /* Queries. */

    template <class Type>
        struct is_member <arithmetic_magma_tag <Type>, typename
            boost::enable_if_c <std::numeric_limits <Type>::has_quiet_NaN>::type
        >
    // Detect NaN.
    { bool operator() (Type const & v) const { return v == v; } };

    template <class Type> struct equal <arithmetic_magma_tag <Type>>
    : operator_equal <bool> {};

    template <class Type>
        struct approximately_equal <arithmetic_magma_tag <Type>, typename
            boost::enable_if_c <!std::numeric_limits <Type>::is_exact>::type>
    {
        bool operator() (Type const & left, Type const & right) const {
            static float const tolerance = 1e-5;
            return math::detail::is_close (left, right, tolerance);
        }
    };

    /* Produce. */

    // Return the not-a-number value.
    template <class Type> struct non_member <arithmetic_magma_tag <Type>,
        typename std::enable_if <std::numeric_limits <Type>::has_quiet_NaN
        >::type>
    {
        Type operator() () const
        { return std::numeric_limits <Type>::quiet_NaN(); }
    };

    template <class Type>
        struct identity <arithmetic_magma_tag <Type>, callable::times>
    { Type operator() () const { return Type (1); } };

    template <class Type>
        struct identity <arithmetic_magma_tag <Type>, callable::plus>
    { Type operator() () const { return Type (0); } };

    template <class Type>
        struct annihilator <arithmetic_magma_tag <Type>, callable::times>
    { Type operator() () const { return Type (0); } };

    /* Operations. */

    template <class Type> struct compare <arithmetic_magma_tag <Type>>
    : operator_less<> {};

    // "choose" is not defined: it is not known whether it should be the
    // maximum or the minimum.

    template <class Type> struct times <arithmetic_magma_tag <Type>>
    : operator_times <Type>, associative, commutative,
        approximate_if <boost::mpl::bool_ <
            !std::numeric_limits <Type>::is_exact>> {};

    template <class Type> struct plus <arithmetic_magma_tag <Type>>
    : operator_plus <Type>, associative, commutative,
        approximate_if <boost::mpl::bool_ <
            !std::numeric_limits <Type>::is_exact>> {};

    template <class Type>
        struct is_semiring <arithmetic_magma_tag <Type>, either,
            callable::times, callable::plus>
    : rime::true_type {};

    template <class Type> struct divide <arithmetic_magma_tag <Type>, either,
        typename boost::enable_if_c <!std::numeric_limits <Type>::is_integer
        >::type>
    : operator_divide <Type>, approximate {};

    template <class Type> struct minus <arithmetic_magma_tag <Type>, either,
        typename boost::enable_if_c <std::numeric_limits <Type>::is_signed
            >::type>
    : operator_minus <Type>, approximate_if <boost::mpl::bool_ <
        !std::numeric_limits <Type>::is_exact>> {};

    template <class Type>
        struct invert <arithmetic_magma_tag <Type>, either, callable::plus,
            typename boost::enable_if_c <std::numeric_limits <Type>::is_signed
                >::type>
    { Type operator() (Type const & v) const { return Type (-v); } };

    template <class Type>
        struct invert <arithmetic_magma_tag <Type>, either, callable::times,
            typename boost::disable_if_c <std::numeric_limits <Type>::is_integer
                >::type>
    : approximate_if <boost::mpl::bool_ <!std::numeric_limits <Type>::is_exact>>
    { Type operator() (Type const & v) const { return Type (Type (1) / v); } };

    template <class Type> struct print <arithmetic_magma_tag <Type>>
    : operator_shift_left_stream {};

} // namespace operation

} // namespace math

#endif // MATH_ARITHMETIC_MAGMA_HPP_INCLUDED
