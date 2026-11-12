// SPDX-FileCopyrightText: Copyright 2012, 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_int
#include "boost/test/unit_test.hpp"

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-integer.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_arithmetic_magma_int)

BOOST_AUTO_TEST_CASE(test_arithmetic_magma_integers)
{
    test_arithmetic_magma_integer<int>(
        get_signed_integer_examples<int>(), get_signed_integer_examples<int>());
    test_arithmetic_magma_integer<unsigned>(
        get_unequal_unsigned_integer_examples<unsigned>(),
        get_unsigned_integer_examples<unsigned>());
}

BOOST_AUTO_TEST_CASE(test_arithmetic_magma_integers_signed)
{
    namespace callable = math::callable;
    using math::has;

    // Signed types do have minus.
    static_assert(
        has<callable::invert<math::left, math::callable::plus>(int)>::value);
    static_assert(
        has<callable::invert<math::right, math::callable::plus>(long)>::value);
    static_assert(has<callable::invert<math::either, math::callable::plus>(
                      short)>::value);
    static_assert(has<callable::invert<math::callable::plus>(int)>::value);

    static_assert(has<callable::minus<>(signed char, signed char)>::value);
    static_assert(
        !math::is::approximate<callable::minus<>(long const &, long &)>::value);
    int a = 2, b = 4;
    BOOST_CHECK_EQUAL(math::minus<>(a, b), int(-2));

    static_assert(!math::is::approximate<callable::invert<math::callable::plus>(
                      int)>::value);

    // Unsigned types do not have minus.
    static_assert(!has<callable::invert<math::left, math::callable::plus>(
                      unsigned int)>::value);
    static_assert(!has<callable::invert<math::right, math::callable::plus>(
                      unsigned long)>::value);
    static_assert(!has<callable::invert<math::either, math::callable::plus>(
                      unsigned short)>::value);
    static_assert(
        !has<callable::invert<math::callable::plus>(unsigned int)>::value);

    static_assert(!has<callable::minus<>(unsigned char, unsigned char)>::value);
}

BOOST_AUTO_TEST_SUITE_END()
