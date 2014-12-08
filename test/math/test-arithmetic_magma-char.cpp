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

#define BOOST_TEST_MODULE test_arithmetic_magma_char
#include "utility/test/boost_unit_test.hpp"

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-integer.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_arithmetic_magma_char)

BOOST_AUTO_TEST_CASE (test_arithmetic_magma_integers) {
    test_arithmetic_magma_integer <char> (get_signed_integer_examples <char>());
    test_arithmetic_magma_integer <unsigned char> (
        get_unsigned_integer_examples <unsigned char>());
}

BOOST_AUTO_TEST_SUITE_END()
