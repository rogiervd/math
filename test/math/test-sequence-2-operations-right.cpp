// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test sequence.hpp: spot checks.

Compiling this takes less memory and time than test-sequence-slow-{left,right}.
This file also comes earlier in a lexicographical sort of file names, so it will
be run first.
*/

#define BOOST_TEST_MODULE test_sequence_fast_right_operations
#include <boost/test/unit_test.hpp>

#include "sequence-tests-fast-operations.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_sequence_fast_right_operations)

BOOST_AUTO_TEST_CASE(test_sequence)
{
    test_construction<math::right>();
    test_comparison<math::right>("ba", "cba");
    test_times<math::right>();
    test_plus<math::right>("ba", "cba");
    test_pick<math::right>();
    test_choose<math::right>("ba", "cba");
    test_divide<math::right>("ba", "cba", "cb");
}

BOOST_AUTO_TEST_SUITE_END()
