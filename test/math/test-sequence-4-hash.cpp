/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_sequence_hash
#include "utility/test/boost_unit_test.hpp"

#include "math/sequence.hpp"

#include <vector>
#include <string>

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std/container.hpp"

#include "math/check/report_check_magma_boost_test.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_sequence_hash)

template <class Direction> void check_hash_homogeneous() {
    typedef math::sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (sequence());
    examples.push_back (sequence (std::string ("a")));
    examples.push_back (sequence (std::string ("b")));
    examples.push_back (sequence (std::string ("a\0")));
    examples.push_back (sequence (std::string ("abcd")));
    examples.push_back (math::sequence_annihilator <char, Direction>());

    math::report_check_hash (examples);
}

template <class Direction> void check_hash_heterogeneous() {
    typedef math::sequence <char, Direction> sequence;
    auto examples = range::make_tuple (
        math::sequence_annihilator <char, Direction>(),
        math::empty_sequence <char, Direction>(),
        math::single_sequence <char, Direction> ('a'),
        math::single_sequence <char, Direction> ('b'),
        math::optional_sequence <char, Direction>(),
        math::optional_sequence <char, Direction> ('a'),
        sequence (),
        sequence (std::string ("a")),
        sequence (std::string ("abc")));

    math::report_check_hash (examples);
    math::report_check_cast_hash <math::sequence <char, Direction>> (examples);
}

BOOST_AUTO_TEST_CASE (test_hash) {
    check_hash_homogeneous <math::left>();
    check_hash_homogeneous <math::right>();
    check_hash_heterogeneous <math::left>();
    check_hash_heterogeneous <math::right>();
}

BOOST_AUTO_TEST_SUITE_END()
