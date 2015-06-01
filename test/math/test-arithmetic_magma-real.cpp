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
This performs tests on floating-point numbers.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma
#include "utility/test/boost_unit_test.hpp"

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-real.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_arithmetic_magma)

BOOST_AUTO_TEST_CASE (test_arithmetic_magma_floating_point) {
    test_arithmetic_magma_real <float> (
        get_signed_float_examples <float>());
    test_arithmetic_magma_real_signed <float>();

    test_arithmetic_magma_real <double> (
        get_signed_float_examples <double>());
    test_arithmetic_magma_real_signed <double>();
    // This used to be tested on long double as well, but this takes seemingly
    // infinite time under Valgrind and does not yield anything that the above
    // tests do not.
}

BOOST_AUTO_TEST_SUITE_END()
