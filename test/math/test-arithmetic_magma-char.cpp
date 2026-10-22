// SPDX-FileCopyrightText: Copyright 2012, 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_char
#include <boost/test/unit_test.hpp>

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-integer.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_arithmetic_magma_char)

BOOST_AUTO_TEST_CASE(test_arithmetic_magma_integers)
{
    test_arithmetic_magma_integer<char>(
        get_signed_integer_examples<char>(),
        get_signed_integer_examples<char>());
    test_arithmetic_magma_integer<unsigned char>(
        get_unequal_unsigned_integer_examples<unsigned char>(),
        get_unsigned_integer_examples<unsigned char>());
}

BOOST_AUTO_TEST_SUITE_END()
