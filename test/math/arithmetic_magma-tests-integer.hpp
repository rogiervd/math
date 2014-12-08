/*
Copyright 2012-2014 Rogier van Dalen.

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

/** \file
Tests for arithmetic_magma.hpp for integer types.
*/

#ifndef MATH_TEST_ARITHMETIC_MAGMA_TESTS_INTEGER_HPP_INCLUDED
#define MATH_TEST_ARITHMETIC_MAGMA_TESTS_INTEGER_HPP_INCLUDED

#include "math/arithmetic_magma.hpp"

#include <boost/mpl/assert.hpp>

#include "range/std/container.hpp"

#include "math/check/check_magma.hpp"

/* Produce example values. */

/**
Get signed integer examples for test.
Note that overflow yields undefined behaviour on signed integers.
Therefore, these values make sure to stay well clear of overflows, even on
checking associativity (a * a * a).
*/
template <class Type> std::vector <Type> get_signed_integer_examples() {
    std::vector <Type> examples;

    examples.push_back (0);
    examples.push_back (1);
    examples.push_back (3);
    examples.push_back (5);

    examples.push_back (-1);
    examples.push_back (-3);
    examples.push_back (-5);

    return std::move (examples);
}

/**
Get unsigned integer examples for test.
For unsigned integers, overflow is defined, so this will yield the minimum and
maximum values possible.
*/
template <class Type> std::vector <Type> get_unsigned_integer_examples() {
    std::vector <Type> examples;

    Type min = std::numeric_limits <Type>::min();
    examples.push_back (min);
    Type max = std::numeric_limits <Type>::max();
    examples.push_back (max);
    examples.push_back (0);
    examples.push_back (-1);

    examples.push_back (1);
    examples.push_back (3);
    examples.push_back (5);
    examples.push_back (-3);
    examples.push_back (-5);

    return std::move (examples);
}

/* Actual tests. */

template <class Type, class Examples>
    void test_arithmetic_magma_integer (Examples const & examples)
{
    Type a = 3;
    Type b = 5;

    namespace callable = math::callable;
    using math::has;

    // Always available; but always true.
    BOOST_MPL_ASSERT ((has <callable::is_member (Type &)>));
    BOOST_CHECK (math::is_member (a));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (math::is_member (a)), rime::true_type>));

    BOOST_MPL_ASSERT ((has <callable::equal (Type const &, Type)>));
    BOOST_CHECK (math::equal (a, a));
    BOOST_CHECK (math::equal (b, b));
    BOOST_CHECK (!math::equal (a, b));
    BOOST_CHECK (!math::equal (b, a));

    BOOST_MPL_ASSERT ((
        has <callable::approximately_equal (Type const &, Type)>));

    // Produce.
    BOOST_MPL_ASSERT_NOT ((has <callable::non_member <Type const> ()>));

    BOOST_MPL_ASSERT ((has <callable::identity <Type, callable::plus>()>));
    BOOST_MPL_ASSERT ((has <callable::identity <Type> (callable::plus)>));
    BOOST_CHECK_EQUAL ((math::identity <Type, callable::plus>()), 0);
    BOOST_CHECK_EQUAL ((math::identity <Type> (math::plus)), 0);

    BOOST_MPL_ASSERT ((has <callable::identity <Type, callable::times>()>));
    BOOST_CHECK_EQUAL ((math::identity <Type, callable::times>()), 1);
    BOOST_CHECK_EQUAL ((math::identity <Type> (math::times)), 1);

    BOOST_MPL_ASSERT_NOT ((
        has <callable::annihilator <Type, callable::plus>()>));
    BOOST_MPL_ASSERT ((has <callable::annihilator <Type, callable::times>()>));
    BOOST_CHECK_EQUAL ((math::annihilator <Type, callable::times>()), 0);

    BOOST_CHECK_EQUAL (math::zero <Type>(), 0);
    BOOST_CHECK_EQUAL (math::one <Type>(), 1);

    // Operations.
    BOOST_MPL_ASSERT ((has <callable::compare (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        callable::compare (Type, Type)>));
    BOOST_CHECK_EQUAL (math::compare (a, a), false);
    BOOST_CHECK_EQUAL (math::compare (a, b), true);
    BOOST_CHECK_EQUAL (math::compare (b, a), false);

    BOOST_MPL_ASSERT_NOT ((has <callable::choose (Type, Type)>));

    BOOST_MPL_ASSERT ((has <callable::times (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        callable::times (Type, Type)>));
    BOOST_CHECK_EQUAL (math::times (a, b), 15);

    BOOST_MPL_ASSERT ((has <callable::plus (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::approximate <callable::plus (Type, Type)>));
    BOOST_CHECK_EQUAL (math::plus (a, b), 8);

    BOOST_MPL_ASSERT_NOT ((has <callable::divide<> (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((has <callable::divide <math::left> (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((has <callable::divide <math::right> (Type, Type)>));

    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::left, math::callable::times> (Type)>));
    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::right, math::callable::times> (Type)>));
    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::either, math::callable::times> (Type)>));
    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::callable::times> (Type)>));

    // Properties
    BOOST_MPL_ASSERT ((math::is::associative <callable::times (Type, Type &)>));
    BOOST_MPL_ASSERT ((math::is::commutative <callable::times (Type& , Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::idempotent <
        callable::times (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::path_operation <
        callable::times (Type &, Type const &)>));

    BOOST_MPL_ASSERT ((math::is::associative <callable::plus (Type, Type &)>));
    BOOST_MPL_ASSERT ((math::is::commutative <callable::plus (Type& , Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::idempotent <
        callable::plus (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::path_operation <
        callable::plus (Type &, Type const &)>));

    BOOST_MPL_ASSERT ((math::is::distributive <
        math::left, callable::times, callable::plus, Type>));

    BOOST_MPL_ASSERT ((math::is::distributive <
        callable::times (callable::plus (Type, Type const), Type &)>));
    BOOST_MPL_ASSERT ((math::is::distributive <
        callable::times (Type, callable::plus (Type &, Type const &))>));

    BOOST_MPL_ASSERT ((math::is::semiring <math::either,
        callable::times, callable::plus, Type>));

    // Check for consistency.
    math::check_semiring <Type, math::either> (
        math::times, math::plus, examples);
}

#endif // MATH_TEST_ARITHMETIC_MAGMA_TESTS_INTEGER_HPP_INCLUDED
