// SPDX-FileCopyrightText: Copyright 2012, 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_long
#include "boost/test/unit_test.hpp"

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-integer.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_arithmetic_magma_long)

BOOST_AUTO_TEST_CASE(test_arithmetic_magma_integers)
{
    test_arithmetic_magma_integer<long>(
        get_signed_integer_examples<long>(),
        get_signed_integer_examples<long>());
    test_arithmetic_magma_integer<unsigned long>(
        get_unequal_unsigned_integer_examples<unsigned long>(),
        get_unsigned_integer_examples<unsigned long>());
}

BOOST_AUTO_TEST_SUITE_END()
