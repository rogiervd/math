// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test sequence.hpp with math::right.

Because the template instantiations for math::left and math::right are mostly
disjoint, instantiating the tests in two different files helps decrease the
amount of memory used for compiling the test.
*/

#define BOOST_TEST_MODULE test_sequence_full_heterogeneous_right
#include "boost/test/unit_test.hpp"

#include "sequence-tests-slow.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_sequence_full_heterogeneous_right)

BOOST_AUTO_TEST_CASE(test_sequence_right)
{
    test_sequence_heterogeneous<math::right>(check_choose<math::right>());
}

BOOST_AUTO_TEST_SUITE_END()
