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
