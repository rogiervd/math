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
Test lexicographical.hpp with homogeneous types.
*/

#define BOOST_TEST_MODULE test_math_lexicographical_full_homogeneous
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <string>
#include <vector>

#include "math/check/check_magma.hpp"

#include "./make_lexicographical.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_lexicographical_full_homogeneous)

BOOST_AUTO_TEST_CASE (test_lexicographical_consistency) {
    std::vector <lexicographical> examples;
    examples.push_back (make_lexicographical (0, ""));
    examples.push_back (make_lexicographical (0, "q"));
    examples.push_back (make_lexicographical (1, "qz"));
    examples.push_back (make_lexicographical (1, ""));
    examples.push_back (make_lexicographical (-5, ""));
    examples.push_back (make_lexicographical (-5, "a"));
    examples.push_back (make_lexicographical (4, "ab"));
    examples.push_back (make_lexicographical (11, "ab"));
    examples.push_back (make_lexicographical (11, "abc"));
    examples.push_back (make_lexicographical (20, "bc"));
    examples.push_back (make_annihilator_lexicographical (0));
    examples.push_back (make_annihilator_lexicographical (11));

    math::check_semiring <lexicographical, math::left> (
        math::times, math::plus, examples);
    math::check_semiring <lexicographical, math::left> (
        math::times, math::choose, examples);
}

BOOST_AUTO_TEST_SUITE_END()
