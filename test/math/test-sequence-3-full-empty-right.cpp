/*
Copyright 2014 Rogier van Dalen.

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
Test sequence.hpp with math::right.

Because the template instantiations for math::left and math::right are mostly
disjoint, instantiating the tests in two different files helps decrease the
amount of memory used for compiling the test.
*/

#define BOOST_TEST_MODULE test_sequence_full_types_right
#include "utility/test/boost_unit_test.hpp"

#include "sequence-tests-slow.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_sequence_full_types_right)

BOOST_AUTO_TEST_CASE (test_sequence_right) {
    test_empty_sequence <math::right>();
}

BOOST_AUTO_TEST_SUITE_END()
