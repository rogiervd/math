// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_sequence_hash
#include <boost/test/unit_test.hpp>

#include "math/sequence.hpp"

#include <string>
#include <vector>

#include <boost/functional/hash.hpp>

#include "range/std/string.hpp"
#include "range/std/vector.hpp"
#include "range/tuple.hpp"

#include "math/check/report_check_magma_boost_test.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_sequence_hash)

template <class Direction> void check_hash_homogeneous()
{
    typedef math::sequence<char, Direction> sequence;
    std::vector<sequence> examples;
    examples.push_back(sequence());
    examples.push_back(sequence(std::string("a")));
    examples.push_back(sequence(std::string("b")));
    examples.push_back(sequence(std::string("a\0")));
    examples.push_back(sequence(std::string("abcd")));
    examples.push_back(math::sequence_annihilator<char, Direction>());

    math::report_check_hash(examples);
}

template <class Direction> void check_hash_heterogeneous()
{
    typedef math::sequence<char, Direction> sequence;
    auto examples = range::make_tuple(
        math::sequence_annihilator<char, Direction>(),
        math::empty_sequence<char, Direction>(),
        math::single_sequence<char, Direction>('a'),
        math::single_sequence<char, Direction>('b'),
        math::optional_sequence<char, Direction>(),
        math::optional_sequence<char, Direction>('a'), sequence(),
        sequence(std::string("a")), sequence(std::string("abc")));

    math::report_check_hash(examples);
    math::report_check_cast_hash<math::sequence<char, Direction>>(examples);
}

BOOST_AUTO_TEST_CASE(test_hash)
{
    check_hash_homogeneous<math::left>();
    check_hash_homogeneous<math::right>();
    check_hash_heterogeneous<math::left>();
    check_hash_heterogeneous<math::right>();
}

BOOST_AUTO_TEST_SUITE_END()
