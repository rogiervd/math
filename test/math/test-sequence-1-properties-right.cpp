// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test sequence.hpp: spot checks.

Compiling this takes less memory and time than test-sequence-slow-{left,right}.
This file also comes earlier in a lexicographical sort of file names, so it will
be run first.
*/

#define BOOST_TEST_MODULE test_sequence_fast_right_properties
#include "boost/test/unit_test.hpp"

#include "sequence-tests-fast-properties.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_sequence_fast_right_properties)

BOOST_AUTO_TEST_CASE(test_sequence)
{
    test_sequence_spot<math::right, math::left>();
}

BOOST_AUTO_TEST_SUITE_END()
