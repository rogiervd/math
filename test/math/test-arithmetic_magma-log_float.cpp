/*
Copyright 2012-2014 Rogier van Dalen.

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
