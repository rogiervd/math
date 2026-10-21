// SPDX-FileCopyrightText: Copyright 2012-2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
This performs tests on log_float.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_log_float
#include <boost/test/unit_test.hpp>

#include "math/arithmetic_magma.hpp"
#include "math/log-float.hpp"

#include "arithmetic_magma-tests-real.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_arithmetic_magma_log_float)

BOOST_AUTO_TEST_CASE(test_arithmetic_magma_log_float)
{
    test_arithmetic_magma_real<math::log_float<float>>(
        get_unsigned_real_examples<math::log_float<float>>());
    test_arithmetic_magma_real<math::log_float<double>>(
        get_unsigned_real_examples<math::log_float<double>>());

    // Test signed_log_float without checking minus.
    // The problem is that 1+2-2, say, is not close enough to 1.
    test_arithmetic_magma_real<math::signed_log_float<float>>(
        get_signed_real_examples<math::signed_log_float<float>>(), false);
    test_arithmetic_magma_real<math::signed_log_float<double>>(
        get_signed_real_examples<math::signed_log_float<double>>(), false);

    test_arithmetic_magma_real_signed<math::signed_log_float<float>>();
    test_arithmetic_magma_real_signed<math::signed_log_float<double>>();
}

BOOST_AUTO_TEST_SUITE_END()
