// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test sequence.hpp: spot checks.

Compiling this takes less memory and time than test-sequence-slow-{left,right}.
This file also comes earlier in a lexicographical sort of file names, so it will
be run first.
*/

#define BOOST_TEST_MODULE test_sequence_fast_left_operations
#include <boost/test/unit_test.hpp>

#include "sequence-tests-fast-operations.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_sequence_fast_left_operations)

BOOST_AUTO_TEST_CASE(test_sequence)
{
    test_construction<math::left>();
    test_comparison<math::left>("ab", "abc");
    test_times<math::left>();
    test_plus<math::left>("ab", "abc");
    test_pick<math::left>();
    test_choose<math::left>("ab", "abc");
    test_divide<math::left>("ab", "abc", "bc");
}

BOOST_AUTO_TEST_SUITE_END()
