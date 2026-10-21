// SPDX-FileCopyrightText: Copyright 2012, 2013, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test log_float.hpp.
Text-search for "****" to find these sections:
- domain error
- overflow error
- underflow error
- basic functionality on corner cases
- binary operations
- functions that mimic the ones from <cmath>
- conversion and interaction

Particular care is taken (hopefully!) to check that everything works with
different policies.
The only error handling policies that are tested are throwing and ignoring.
The implementation uses Boost.Math functions to raise errors so this should
generalise to other forms of behaviour.
Testing the ignoring policies is important if specialisations for error policies
bypass explicit error handling in favour of leaving this to the hardware.
*/

#define BOOST_TEST_MODULE log_float_hash
#include <boost/test/unit_test.hpp>

#include <boost/test/tools/floating_point_comparison.hpp>

#include "math/log-float.hpp"

#include "range/std/vector.hpp"

#include "math/check/report_check_magma_boost_test.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_log_float_hash)

BOOST_AUTO_TEST_CASE(test_hash)
{
    typedef math::log_float<double> log_float;
    typedef math::signed_log_float<double> signed_log_float;

    // Check hash on log_float.
    std::vector<log_float> examples;
    examples.push_back(0);
    examples.push_back(
        log_float(-std::numeric_limits<double>::max(), math::as_exponent()));
    examples.push_back(1.e-30);
    examples.push_back(1.e-10);
    examples.push_back(.1);
    examples.push_back(log_float(-0., math::as_exponent()));
    examples.push_back(log_float(0., math::as_exponent()));
    examples.push_back(.5);
    examples.push_back(1);
    examples.push_back(2);
    examples.push_back(10);
    examples.push_back(1.e+30);
    examples.push_back(
        log_float(std::numeric_limits<double>::max(), math::as_exponent()));
    examples.push_back(log_float(
        std::numeric_limits<double>::infinity(), math::as_exponent()));

    math::report_check_hash(examples);

    // Check hash on signed_log_float.
    // For positive values, the hash value should be the same.
    math::report_check_cast_hash<signed_log_float>(examples);

    std::vector<signed_log_float> signed_examples;
    for (log_float example : examples) {
        signed_examples.push_back(example);
        signed_examples.push_back(-example);
    }
    math::report_check_hash(signed_examples);
}

BOOST_AUTO_TEST_SUITE_END()
