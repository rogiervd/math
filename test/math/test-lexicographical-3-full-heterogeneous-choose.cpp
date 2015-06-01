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
Test lexicographical.hpp with heterogeneous types and \ref choose.
*/

#define BOOST_TEST_MODULE test_math_lexicographical_full_heterogeneous_choose
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <string>
#include <vector>

#include "math/check/check_magma.hpp"

#include "./make_lexicographical.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_lexicographical_full_heterogeneous_choose)

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
        math::times, math::choose, examples);
}

BOOST_AUTO_TEST_SUITE_END()
