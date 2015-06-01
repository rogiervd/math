/*
Copyright 2014 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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
