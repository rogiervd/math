/*
Copyright 2012-2014 Rogier van Dalen.

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
Tests for arithmetic_magma on reals.
*/

#ifndef MATH_TEST_ARITHMETIC_MAGMA_TESTS_REAL_HPP_INCLUDED
#define MATH_TEST_ARITHMETIC_MAGMA_TESTS_REAL_HPP_INCLUDED

#include "math/arithmetic_magma.hpp"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/tools/test.hpp>
#include <boost/math/special_functions/next.hpp>

#include <boost/mpl/assert.hpp>

#include "range/std/container.hpp"

#include "math/check/check_magma.hpp"

/* Produce example values. */

template <class Type> std::vector <Type> get_unsigned_real_examples() {
    std::vector <Type> examples;
    examples.push_back (Type (0));
    examples.push_back (Type (0.2));
    examples.push_back (Type (0.5));
    examples.push_back (Type (1.));
    examples.push_back (Type (2.3));
    examples.push_back (Type (5.));
    return std::move (examples);
}

template <class Type> std::vector <Type> get_signed_real_examples() {
    std::vector <Type> examples = get_unsigned_real_examples <Type>();
    // Different from the positive examples so that signed_log_float does not
    // end up in problems.
    examples.push_back (Type (-5.11));
    examples.push_back (Type (-2.27));
    examples.push_back (Type (-1.25));
    examples.push_back (Type (-0.68));
    examples.push_back (Type (-0.23));
    return std::move (examples);
}

template <class Type> std::vector <Type> get_signed_float_examples() {
    std::vector <Type> examples = get_signed_real_examples <Type>();
    // Find a value whose inversion is approximate, i.e. (1/value) * value != 1.
    Type candidate = Type (3);
    Type inverse;
    do {
        candidate = boost::math::float_next (candidate);
        inverse = Type (1) / candidate;
    } while (candidate * inverse == Type (1));

    BOOST_CHECK (!(
        math::invert <math::callable::times> (candidate) * candidate
        == Type (1)));

    examples.push_back (candidate);

    return std::move (examples);
}

/* Actual tests. */

template <class Type, class Examples>
    void test_arithmetic_magma_real (Examples const & examples)
{
    Type a (3);
    Type b (5);
    Type c (2.5);

    BOOST_MPL_ASSERT ((
        math::has <math::callable::non_member <Type const &>()>));
    BOOST_MPL_ASSERT ((math::has <math::callable::is_member (Type const)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::equal (Type &, Type const)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::approximately_equal (Type, Type const &)>));

    // compare.
    BOOST_MPL_ASSERT ((math::has <math::callable::compare (Type, Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        math::callable::compare (Type, Type)>));
    BOOST_CHECK_EQUAL (math::compare (a, a), false);
    BOOST_CHECK_EQUAL (math::compare (a, b), true);
    BOOST_CHECK_EQUAL (math::compare (a, c), false);
    BOOST_CHECK_EQUAL (math::compare (b, a), false);
    BOOST_CHECK_EQUAL (math::compare (b, b), false);
    BOOST_CHECK_EQUAL (math::compare (b, c), false);
    BOOST_CHECK_EQUAL (math::compare (c, a), true);
    BOOST_CHECK_EQUAL (math::compare (c, b), true);
    BOOST_CHECK_EQUAL (math::compare (c, c), false);

    // times.
    BOOST_MPL_ASSERT ((math::has <
        math::callable::times (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::times (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <Type &, math::callable::times>()>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <Type &> (math::callable::times)>));
    BOOST_MPL_ASSERT ((math::has <math::callable::one <Type const &>()>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::annihilator <Type &, math::callable::times> ()>));

    BOOST_CHECK_EQUAL (math::one <Type>(), Type (1));
    BOOST_CHECK_EQUAL ((math::identity <Type, math::callable::times>()),
        Type (1));
    BOOST_CHECK_EQUAL (math::identity <Type> (math::times), Type (1));
    BOOST_CHECK_EQUAL ((math::annihilator <Type, math::callable::times>()),
        Type (0));
    BOOST_CHECK_EQUAL (math::annihilator <Type> (math::times), Type (0));

    BOOST_MPL_ASSERT ((math::is::commutative <math::callable::times, Type &>));
    BOOST_MPL_ASSERT ((
        math::is::associative <math::callable::times, Type const &>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::idempotent <math::callable::times, Type>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::path_operation <math::callable::times, Type>));

    // plus.
    BOOST_MPL_ASSERT ((math::has <
        math::callable::plus (Type const &, Type &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::plus (Type const &, Type &)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <Type &, math::callable::plus>()>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <Type &> (math::callable::plus)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::zero <Type const>()>));
    BOOST_MPL_ASSERT_NOT ((math::has <
        math::callable::annihilator <Type &, math::callable::plus> ()>));

    BOOST_CHECK_EQUAL (math::zero <Type>(), Type (0));
    BOOST_CHECK_EQUAL (
        (math::identity <Type, math::callable::plus>()), Type (0));
    BOOST_CHECK_EQUAL (math::identity <Type> (math::plus), Type (0));

    BOOST_MPL_ASSERT ((math::is::associative <math::callable::plus, Type &>));
    BOOST_MPL_ASSERT ((math::is::commutative <math::callable::plus, Type &>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::path_operation <math::callable::plus, Type &>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::idempotent <math::callable::plus, Type &>));

    // divide.
    BOOST_MPL_ASSERT ((math::has <
        math::callable::divide<> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::divide <math::left> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::divide <math::right> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::divide<> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::divide <math::left> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::divide <math::right> (Type &, Type const &)>));

    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::left, math::callable::times> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::right, math::callable::times> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::either, math::callable::times> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::callable::times> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert<> (math::callable::times, Type)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::invert <math::callable::times> (Type)>));

    // reverse.
    BOOST_MPL_ASSERT ((
        math::has <math::callable::reverse <math::callable::plus> (Type)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::reverse<> (math::callable::plus, Type)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::reverse <math::callable::times> (Type)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::reverse<> (math::callable::times, Type)>));

    // Check for sensible results.
    {
        Type result = math::times (a, b);
        BOOST_CHECK_EQUAL (result, Type (15));
    }
    {
        Type result = math::plus (a, b);
        BOOST_CHECK_EQUAL (result, Type (8));
    }
    // divide.
    {
        // The result of this is 0.5, but it is not exact for log_float.
        Type result = math::divide (c, b);
        BOOST_CHECK_EQUAL (result, Type (2.5) / Type (5));
        result = math::divide <math::left> (c, b);
        BOOST_CHECK_EQUAL (result, Type (2.5) / Type (5));
        result = math::divide <math::right> (c, b);
        BOOST_CHECK_EQUAL (result, Type (2.5) / Type (5));
    }
    // invert.
    {
        Type result = math::invert <math::left, math::callable::times> (a);
        BOOST_CHECK_EQUAL (result, Type (1)/a);
        result = math::invert <math::right, math::callable::times> (b);
        BOOST_CHECK_EQUAL (result, Type (1)/b);
        result = math::invert <math::either, math::callable::times> (c);
        BOOST_CHECK_EQUAL (result, Type (1)/c);
        result = math::invert <math::callable::times> (c);
        BOOST_CHECK_EQUAL (result, Type (1)/c);

        result = math::invert <math::left> (math::times, a);
        BOOST_CHECK_EQUAL (result, Type (1)/a);
        result = math::invert <math::right> (math::times, b);
        BOOST_CHECK_EQUAL (result, Type (1)/b);
        result = math::invert <math::either> (math::times, c);
        BOOST_CHECK_EQUAL (result, Type (1)/c);
        result = math::invert (math::times, c);
        BOOST_CHECK_EQUAL (result, Type (1)/c);
    }
    // reverse.
    {
        BOOST_CHECK_EQUAL (math::reverse <math::callable::plus> (a), a);
        BOOST_CHECK_EQUAL (math::reverse (math::plus, b), b);
        BOOST_CHECK_EQUAL (math::reverse <math::callable::times> (a), a);
        BOOST_CHECK_EQUAL (math::reverse (math::times, b), b);
    }

    // inverse_operation on times.
    {
        // The result of this is 0.5, but it is not exact for log_float.
        Type result = math::inverse_operation (math::times) (c, b);
        BOOST_CHECK_EQUAL (result, Type (2.5) / Type (5));
        result = math::inverse_operation <math::left, math::callable::times>()
            (c, b);
        BOOST_CHECK_EQUAL (result, Type (2.5) / Type (5));
        result = math::inverse_operation <math::right> (math::times) (c, b);
        BOOST_CHECK_EQUAL (result, Type (2.5) / Type (5));
    }

    // Check for consistency.
    math::check_equal_on (examples);

    math::check_semiring <Type, math::either> (
        math::times, math::plus, examples);
}

template <class Type> void test_arithmetic_magma_real_signed() {
    Type a = 3;
    Type b = 5;

    // minus.
    BOOST_MPL_ASSERT ((math::has <
        math::callable::minus<> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::minus <math::left> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::minus <math::right> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::minus<> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::minus <math::left> (Type &, Type const &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::minus <math::right> (Type &, Type const &)>));

    // invert.
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::left, math::callable::plus> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::right, math::callable::plus> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::either, math::callable::plus> (Type)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::invert <math::callable::plus> (Type)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        math::callable::invert <math::callable::plus> (Type)>));

    // minus.
    {
        Type result = math::minus (a, b);
        BOOST_CHECK_EQUAL (result, - (Type (5) - Type (3)));
        result = math::minus <math::left> (a, b);
        BOOST_CHECK_EQUAL (result, - (Type (5) - Type (3)));
        result = math::minus <math::right> (a, b);
        BOOST_CHECK_EQUAL (result, - (Type (5) - Type (3)));
    }

    // inverse_operation on plus.
    {
        Type result = math::inverse_operation (math::plus) (a, b);
        BOOST_CHECK_EQUAL (result, - (Type (5) - Type (3)));
        result = math::inverse_operation <math::left> (math::plus) (a, b);
        BOOST_CHECK_EQUAL (result, - (Type (5) - Type (3)));
        result = math::inverse_operation <math::right, math::callable::plus>()
            (a, b);
        BOOST_CHECK_EQUAL (result, - (Type (5) - Type (3)));
    }
}

#endif // MATH_TEST_ARITHMETIC_MAGMA_TESTS_REAL_HPP_INCLUDED
