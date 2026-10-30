// SPDX-FileCopyrightText: Copyright 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test lexicographical.hpp with heterogeneous types and \ref choose.
*/

#define BOOST_TEST_MODULE test_math_lexicographical_full_heterogeneous_choose
#include "boost/test/unit_test.hpp"

#include "math/lexicographical.hpp"

#include <string>
#include <vector>

#include "range/std/string.hpp"

#include "math/check/report_check_magma_boost_test.hpp"

#include "./make_lexicographical.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_lexicographical_full_heterogeneous_choose)

BOOST_AUTO_TEST_CASE(test_lexicographical_consistency)
{
    // Not all types need to be checked.
    // Every extra type requires a lot of extra memory to compile the tests.
    auto examples = range::make_tuple(
        make_empty_lexicographical(0), make_empty_lexicographical(3),
        make_optional_lexicographical(2), make_optional_lexicographical(2, 'a'),
        make_lexicographical(0, "abc"), make_lexicographical(4, "ab"));

    math::report_check_semiring<lexicographical, math::left>(
        math::times, math::choose, examples, examples);
}

BOOST_AUTO_TEST_SUITE_END()
