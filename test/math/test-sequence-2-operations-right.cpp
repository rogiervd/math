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
Test sequence.hpp: spot checks.

Compiling this takes less memory and time than test-sequence-slow-{left,right}.
This file also comes earlier in a lexicographical sort of file names, so it will
be run first.
*/

#define BOOST_TEST_MODULE test_sequence_fast_right_operations
#include "utility/test/boost_unit_test.hpp"

#include "sequence-tests-fast-operations.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_sequence_fast_right_operations)

BOOST_AUTO_TEST_CASE (test_sequence) {
    test_construction <math::right>();
    test_comparison <math::right> ("ba", "cba");
    test_times <math::right> ();
    test_plus <math::right> ("ba", "cba");
    test_choose <math::right> ("ba", "cba");
    test_divide <math::right> ("ba", "cba", "cb");
}

BOOST_AUTO_TEST_SUITE_END()