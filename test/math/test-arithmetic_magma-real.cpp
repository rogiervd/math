// SPDX-FileCopyrightText: Copyright 2012-2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
This performs tests on floating-point numbers.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma
#include <boost/test/unit_test.hpp>

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-real.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_arithmetic_magma)

BOOST_AUTO_TEST_CASE(test_arithmetic_magma_floating_point)
{
    test_arithmetic_magma_real<float>(get_signed_float_examples<float>());
    test_arithmetic_magma_real_signed<float>();

    test_arithmetic_magma_real<double>(get_signed_float_examples<double>());
    test_arithmetic_magma_real_signed<double>();
    // This used to be tested on long double as well, but this takes seemingly
    // infinite time under Valgrind and does not yield anything that the above
    // tests do not.
}

BOOST_AUTO_TEST_SUITE_END()
