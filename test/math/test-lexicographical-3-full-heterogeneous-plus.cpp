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
Test lexicographical.hpp with heterogeneous types and \ref plus.
*/

#define BOOST_TEST_MODULE test_math_lexicographical_full_heterogeneous_plus
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <string>
#include <vector>

#include "math/check/check_magma.hpp"

#include "./make_lexicographical.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_lexicographical_full_heterogeneous_plus)

BOOST_AUTO_TEST_CASE (test_lexicographical_consistency) {
    // Not all types need to be checked.
    // Every extra type requires a lot of extra memory to compile the tests.
    auto examples = range::make_tuple (
        make_empty_lexicographical (0),
        make_empty_lexicographical (3),
        make_optional_lexicographical (2),
        make_optional_lexicographical (2, 'a'),
        make_lexicographical (0, "abc"),
        make_lexicographical (4, "ab"));

    math::check_semiring <lexicographical, math::left> (
        math::times, math::plus, examples);
}

BOOST_AUTO_TEST_SUITE_END()
