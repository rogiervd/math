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
Contain templated helpers for testing math::sequence.
By instantiating these tests once for math::left and once for math::right,
compiling takes a lot less memory and time.
*/

#ifndef MATH_TEST_MATH_TEST_SEQUENCE_TESTS_SLOW_HPP_INCLUDED
#define MATH_TEST_MATH_TEST_SEQUENCE_TESTS_SLOW_HPP_INCLUDED

// Workaround for incorrect warnings on GCC 4.6 and 4.8.
// The warnings occur in check_magma.hpp and only for optimized builds.
#if defined __GNUC__
#   if (__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#       pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#   endif
#   if (__GNUC__ == 4 && __GNUC_MINOR__ == 6)
#       pragma GCC diagnostic ignored "-Wuninitialized"
#   endif
#endif

#include "math/sequence.hpp"

#include <string>

#include "range/tuple.hpp"

#include "math/check/check_magma.hpp"

// Check for consistency.
template <class Direction> void test_sequence_homogeneous() {
    typedef math::sequence <char, Direction> sequence;

    // With runtime types.
    std::vector <sequence> examples;
    examples.push_back (sequence (std::string ("")));
    examples.push_back (sequence (std::string ("a")));
    examples.push_back (sequence (std::string ("b")));
    examples.push_back (sequence (std::string ("c")));
    examples.push_back (sequence (std::string ("ab")));
    examples.push_back (sequence (std::string ("ba")));
    examples.push_back (sequence (std::string ("abc")));
    examples.push_back (sequence (std::string ("cba")));

    math::check_semiring <sequence, Direction> (
        math::times, math::plus, examples);
    math::check_magma <sequence> (math::times, math::choose, examples);
}

template <class Direction> void test_empty_sequence() {
    typedef math::empty_sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::empty_sequence <char, Direction>());
    math::check_semiring <sequence, Direction> (
        math::times, math::plus, examples);
    math::check_magma <sequence> (math::times, math::choose, examples);
}

template <class Direction> void test_single_sequence() {
    typedef math::single_sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::single_sequence <char, Direction> ('a'));
    examples.push_back (math::single_sequence <char, Direction> ('b'));
    math::check_semiring <sequence, Direction> (
        math::times, math::plus, examples);
    math::check_magma <sequence> (math::times, math::choose, examples);
}

template <class Direction> void test_optional_sequence() {
    typedef math::optional_sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::optional_sequence <char, Direction>());
    examples.push_back (math::optional_sequence <char, Direction> ('a'));
    examples.push_back (math::optional_sequence <char, Direction> ('b'));
    math::check_semiring <sequence, Direction> (
        math::times, math::plus, examples);
    math::check_magma <sequence> (math::times, math::choose, examples);
}

template <class Direction> void test_sequence_annihilator() {
    typedef math::sequence_annihilator <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::sequence_annihilator <char, Direction>());
    math::check_semiring <sequence, Direction> (
        math::times, math::plus, examples);
    math::check_magma <sequence> (math::times, math::choose, examples);
}

/* Heterogeneous tests. */

// Because of all the combinations of types that need to be checked, these
// tests can run out of memory quickly.
//

// Sequences form a semiring with plus.
template <class Direction> struct check_plus {
    template <class Examples> void operator() (Examples const & examples) const
    {
        math::check_semiring <math::sequence <char, Direction>, Direction> (
            math::times, math::plus, examples);
    }
};

// Sequences form a not-quitesemiring with choose.
template <class Direction> struct check_choose {
    template <class Examples> void operator() (Examples const & examples) const
    {
        math::check_magma <math::sequence <char, Direction>> (
            math::times, math::choose, examples);
    }
};

// To make this feasible to compile within reasonable memory, all types are
// always represented in the same order, but the values are different.
template <class Direction, class Check>
    void test_sequence_heterogeneous (Check const & check)
{
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;
    // Do not include annihilator or the memory requirements for compiling go
    // over 2GB.
    // typedef math::sequence_annihilator <char, Direction> annihilator;

    auto examples = range::make_tuple (
        empty_sequence(),
        single_sequence ('a'), single_sequence ('b'),
        optional_sequence(), optional_sequence ('a'),
        sequence (std::string ("")), sequence (std::string ("a")),
        sequence (std::string ("b")), sequence (std::string ("ab")),
        sequence (std::string ("cab")), sequence (std::string ("cde")));

    check (examples);
}

#endif // MATH_TEST_MATH_TEST_SEQUENCE_TESTS_SLOW_HPP_INCLUDED
