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
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
This performs tests on log_float.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_log_float
#include "utility/test/boost_unit_test.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/log-float.hpp"

#include "arithmetic_magma-tests-real.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_arithmetic_magma_log_float)

BOOST_AUTO_TEST_CASE (test_arithmetic_magma_log_float) {
    test_arithmetic_magma_real <math::log_float <float>> (
        get_unsigned_real_examples <math::log_float <float>>());
    test_arithmetic_magma_real <math::log_float <double>> (
        get_unsigned_real_examples <math::log_float <double>>());

    // Test signed_log_float with only spot checks, by passing it an empty
    // vector.
    // The problem is that 1+2-2, say, is not close enough to 1.
    test_arithmetic_magma_real <math::signed_log_float <float>> (
         std::vector <math::signed_log_float <float>>());
    test_arithmetic_magma_real_signed <math::signed_log_float <double>>();
}

BOOST_AUTO_TEST_SUITE_END()
