/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_sequence_hash
#include "utility/test/boost_unit_test.hpp"

#include "math/sequence.hpp"

#include <vector>
#include <string>

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std/container.hpp"

#include "math/check/check_hash.hpp"

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

    math::check_hash (examples);
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

    math::check_hash (examples);
    math::check_cast_hash <math::sequence <char, Direction>> (examples);
}

BOOST_AUTO_TEST_CASE (test_hash) {
    check_hash_homogeneous <math::left>();
    check_hash_homogeneous <math::right>();
    check_hash_heterogeneous <math::left>();
    check_hash_heterogeneous <math::right>();
}

BOOST_AUTO_TEST_SUITE_END()
