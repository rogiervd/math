/*
Copyright 2012, 2013 Rogier van Dalen.

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
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_int
#include "utility/test/boost_unit_test.hpp"

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-integer.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_arithmetic_magma_int)

BOOST_AUTO_TEST_CASE (test_arithmetic_magma_integers) {
    test_arithmetic_magma_integer <int> (get_signed_integer_examples <int>());
    test_arithmetic_magma_integer <unsigned> (
        get_unsigned_integer_examples <unsigned>());
}

BOOST_AUTO_TEST_CASE (test_arithmetic_magma_integers_signed) {
    namespace callable = math::callable;
    using math::has;

    // Signed types do have minus.
    BOOST_MPL_ASSERT ((has <
        callable::invert <math::left, math::callable::plus> (int)>));
    BOOST_MPL_ASSERT ((has <
        callable::invert <math::right, math::callable::plus> (long)>));
    BOOST_MPL_ASSERT ((has <
        callable::invert <math::either, math::callable::plus> (short)>));
    BOOST_MPL_ASSERT ((has <
        callable::invert <math::callable::plus> (int)>));

    BOOST_MPL_ASSERT ((has <callable::minus<> (signed char, signed char)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        callable::minus<> (long const &, long &)>));
    int a = 2, b = 4;
    BOOST_CHECK_EQUAL (math::minus<> (a, b), int (-2));

    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        callable::invert <math::callable::plus> (int)>));

    // Unsigned types do not have minus.
    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::left, math::callable::plus> (unsigned int)>));
    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::right, math::callable::plus> (unsigned long)>));
    BOOST_MPL_ASSERT_NOT ((has <callable::invert <
        math::either, math::callable::plus> (unsigned short)>));
    BOOST_MPL_ASSERT_NOT ((has <
        callable::invert <math::callable::plus> (unsigned int)>));

    BOOST_MPL_ASSERT_NOT ((has <
        callable::minus<> (unsigned char, unsigned char)>));
}

BOOST_AUTO_TEST_SUITE_END()
