// SPDX-FileCopyrightText: Copyright 2012-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Tests for arithmetic_magma.hpp for integer types.
*/

#ifndef MATH_TEST_ARITHMETIC_MAGMA_TESTS_INTEGER_HPP_INCLUDED
#define MATH_TEST_ARITHMETIC_MAGMA_TESTS_INTEGER_HPP_INCLUDED

#include "math/arithmetic_magma.hpp"

#include <iostream>
#include <typeinfo>

#include "boost/mpl/assert.hpp"

#include "range/std/vector.hpp"

#include "math/check/report_check_magma_boost_test.hpp"

/* Produce example values. */

/**
Get signed integer examples for test.
Note that overflow yields undefined behaviour on signed integers.
Therefore, these values make sure to stay well clear of overflows, even on
checking associativity (a * a * a).
*/
template <class Type> std::vector<Type> get_signed_integer_examples()
{
    std::vector<Type> examples;

    examples.push_back(0);
    examples.push_back(1);
    examples.push_back(3);
    examples.push_back(5);

    examples.push_back(-1);
    examples.push_back(-3);
    examples.push_back(-5);

    return std::move(examples);
}

/**
Get unsigned integer examples for test.
For unsigned integers, overflow is defined, so this will yield the minimum and
maximum values possible.
*/
template <class Type> std::vector<Type> get_unequal_unsigned_integer_examples()
{
    std::vector<Type> examples;

    Type max = std::numeric_limits<Type>::max();
    examples.push_back(max);
    examples.push_back(0);

    examples.push_back(1);
    examples.push_back(3);
    examples.push_back(5);

    return std::move(examples);
}

template <class Type> std::vector<Type> get_unsigned_integer_examples()
{
    std::vector<Type> examples;

    Type min = std::numeric_limits<Type>::min();
    examples.push_back(min);
    Type max = std::numeric_limits<Type>::max();
    examples.push_back(max);
    examples.push_back(0);
    examples.push_back(Type(-1));

    examples.push_back(1);
    examples.push_back(3);
    examples.push_back(5);
    examples.push_back(Type(-3));
    examples.push_back(Type(-5));

    return std::move(examples);
}

/* Actual tests. */

template <class Type, class Examples> void test_arithmetic_magma_integer(
    Examples const & unequal_examples, Examples const & examples)
{
    Type a = 3;
    Type b = 5;

    namespace callable = math::callable;
    using math::has;

    // Always available; but always true.
    static_assert(has<callable::is_member(Type &)>::value);
    BOOST_CHECK(math::is_member(a));
    static_assert(
        std::is_same_v<decltype(math::is_member(a)), rime::true_type>);

    static_assert(has<callable::equal(Type const &, Type)>::value);
    BOOST_CHECK(math::equal(a, a));
    BOOST_CHECK(math::equal(b, b));
    BOOST_CHECK(!math::equal(a, b));
    BOOST_CHECK(!math::equal(b, a));

    static_assert(
        has<callable::approximately_equal(Type const &, Type)>::value);

    // Produce.
    static_assert(!has<callable::non_member<Type const>()>::value);

    static_assert(has<callable::identity<Type, callable::plus>()>::value);
    static_assert(has<callable::identity<Type>(callable::plus)>::value);
    BOOST_CHECK_EQUAL((math::identity<Type, callable::plus>()), Type(0));
    BOOST_CHECK_EQUAL((math::identity<Type>(math::plus)), Type(0));

    static_assert(has<callable::identity<Type, callable::times>()>::value);
    BOOST_CHECK_EQUAL((math::identity<Type, callable::times>()), Type(1));
    BOOST_CHECK_EQUAL((math::identity<Type>(math::times)), Type(1));

    static_assert(!has<callable::annihilator<Type, callable::plus>()>::value);
    static_assert(has<callable::annihilator<Type, callable::times>()>::value);
    BOOST_CHECK_EQUAL((math::annihilator<Type, callable::times>()), Type(0));

    BOOST_CHECK_EQUAL(math::zero<Type>(), Type(0));
    BOOST_CHECK_EQUAL(math::one<Type>(), Type(1));

    // Operations.
    static_assert(has<callable::compare(Type, Type)>::value);
    static_assert(!math::is::approximate<callable::compare(Type, Type)>::value);
    BOOST_CHECK_EQUAL(math::compare(a, a), false);
    BOOST_CHECK_EQUAL(math::compare(a, b), true);
    BOOST_CHECK_EQUAL(math::compare(b, a), false);

    static_assert(has<callable::pick(bool, Type, Type)>::value);
    static_assert(has<callable::pick(rime::true_type, Type, Type)>::value);
    static_assert(has<callable::pick(std::false_type, Type, Type)>::value);

    BOOST_CHECK_EQUAL(math::pick(true, a, b), a);
    BOOST_CHECK_EQUAL(math::pick(false, a, b), b);
    BOOST_CHECK_EQUAL(math::pick(std::true_type(), b, a), b);
    BOOST_CHECK_EQUAL(math::pick(rime::false_, b, a), a);

    static_assert(!has<callable::choose(Type, Type)>::value);

    static_assert(has<callable::times(Type, Type)>::value);
    static_assert(!math::is::approximate<callable::times(Type, Type)>::value);
    BOOST_CHECK_EQUAL(math::times(a, b), Type(15));

    static_assert(has<callable::plus(Type, Type)>::value);
    static_assert(!math::is::approximate<callable::plus(Type, Type)>::value);
    BOOST_CHECK_EQUAL(math::plus(a, b), Type(8));

    static_assert(!has<callable::divide<>(Type, Type)>::value);
    static_assert(!has<callable::divide<math::left>(Type, Type)>::value);
    static_assert(!has<callable::divide<math::right>(Type, Type)>::value);

    static_assert(
        !has<callable::invert<math::left, math::callable::times>(Type)>::value);
    static_assert(!has<callable::invert<math::right, math::callable::times>(
                      Type)>::value);
    static_assert(!has<callable::invert<math::either, math::callable::times>(
                      Type)>::value);
    static_assert(!has<callable::invert<math::callable::times>(Type)>::value);

    // Properties
    static_assert(math::is::associative<callable::times(Type, Type &)>::value);
    static_assert(math::is::commutative<callable::times(Type &, Type)>::value);
    static_assert(!math::is::idempotent<callable::times(Type, Type)>::value);
    static_assert(!math::is::path_operation<callable::times(
                      Type &, Type const &)>::value);

    static_assert(math::is::associative<callable::plus(Type, Type &)>::value);
    static_assert(math::is::commutative<callable::plus(Type &, Type)>::value);
    static_assert(!math::is::idempotent<callable::plus(Type, Type)>::value);
    static_assert(
        !math::is::path_operation<callable::plus(Type &, Type const &)>::value);

    static_assert(math::is::distributive<
                  math::left, callable::times, callable::plus, Type>::value);

    static_assert(math::is::distributive<callable::times(
                      callable::plus(Type, Type const), Type &)>::value);
    static_assert(math::is::distributive<callable::times(
                      Type, callable::plus(Type &, Type const &))>::value);

    static_assert(math::is::semiring<
                  math::either, callable::times, callable::plus, Type>::value);

    // Check for consistency.
    math::report_check_semiring<Type, math::either>(
        math::times, math::plus, unequal_examples, examples);
}

#endif  // MATH_TEST_ARITHMETIC_MAGMA_TESTS_INTEGER_HPP_INCLUDED
