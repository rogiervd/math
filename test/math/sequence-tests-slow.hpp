/*
Copyright 2014, 2015 Rogier van Dalen.

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

#include "math/check/report_check_magma_boost_test.hpp"

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

    math::report_check_semiring <sequence, Direction> (
        math::times, math::plus, examples, examples);
    math::report_check_magma <sequence> (math::times, math::choose,
        examples, examples);
}

template <class Direction> void test_empty_sequence() {
    typedef math::empty_sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::empty_sequence <char, Direction>());

    // It is impossible to check is_annihilator at all, so switch that test off.
    {
        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        plus_checks.do_not_check (
            math::operation_properties::is_annihilator);

        math::report_check_semiring <sequence, Direction> (
            math::times, math::plus, examples, examples,
            type_checks, times_checks, plus_checks,
            times_plus_checks, plus_times_checks);
    }
    {
        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        plus_checks.do_not_check (
            math::operation_properties::is_annihilator);

        math::report_check_magma <sequence> (
            math::times, math::choose, examples, examples,
            type_checks, times_checks, plus_checks,
            times_plus_checks, plus_times_checks);
    }
}

template <class Direction> void test_single_sequence() {
    typedef math::single_sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::single_sequence <char, Direction> ('a'));
    examples.push_back (math::single_sequence <char, Direction> ('b'));
    math::report_check_semiring <sequence, Direction> (
        math::times, math::plus, examples, examples);
    math::report_check_magma <sequence> (
        math::times, math::choose, examples, examples);
}

template <class Direction> void test_optional_sequence() {
    typedef math::optional_sequence <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::optional_sequence <char, Direction>());
    examples.push_back (math::optional_sequence <char, Direction> ('a'));
    examples.push_back (math::optional_sequence <char, Direction> ('b'));
    math::report_check_semiring <sequence, Direction> (
        math::times, math::plus, examples, examples);
    math::report_check_magma <sequence> (
        math::times, math::choose, examples, examples);
}

template <class Direction> void test_sequence_annihilator() {
    typedef math::sequence_annihilator <char, Direction> sequence;
    std::vector <sequence> examples;
    examples.push_back (math::sequence_annihilator <char, Direction>());
    // The annihilator is not an additive annihilator so switch that test off.
    {
        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        plus_checks.do_not_check (
            math::operation_properties::is_annihilator);

        math::report_check_semiring <sequence, Direction> (
            math::times, math::plus, examples, examples,
            type_checks, times_checks, plus_checks,
            times_plus_checks, plus_times_checks);
    }
    {
        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        plus_checks.do_not_check (
            math::operation_properties::is_annihilator);

        math::report_check_magma <sequence> (
            math::times, math::choose, examples, examples,
            type_checks, times_checks, plus_checks,
            times_plus_checks, plus_times_checks);
    }
}

/* Heterogeneous tests. */

// Because of all the combinations of types that need to be checked, these
// tests can run out of memory quickly.
//

// Sequences form a semiring with plus.
template <class Direction> struct check_plus {
    template <class UnequalExamples, class Examples>
        void operator() (UnequalExamples const & unequal_examples,
            Examples const & examples) const
    {
        math::report_check_semiring <
                math::sequence <char, Direction>, Direction> (
            math::times, math::plus, unequal_examples, examples);
    }
};

// Sequences form a not-quitesemiring with choose.
template <class Direction> struct check_choose {
    template <class UnequalExamples, class Examples>
        void operator() (UnequalExamples const & unequal_examples,
            Examples const & examples) const
    {
        math::report_check_magma <math::sequence <char, Direction>> (
            math::times, math::choose, unequal_examples, examples);
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

    auto unequal_examples = range::make_tuple (
        empty_sequence(),
        single_sequence ('a'), single_sequence ('b'),
        optional_sequence ('c'),
        sequence (std::string ("d")), sequence (std::string ("ab")),
        sequence (std::string ("abc")), sequence (std::string ("cde")));

    auto examples = range::make_tuple (
        empty_sequence(),
        single_sequence ('a'), single_sequence ('b'),
        optional_sequence(), optional_sequence ('a'),
        sequence (std::string ("")), sequence (std::string ("a")),
        sequence (std::string ("b")), sequence (std::string ("ab")),
        sequence (std::string ("cab")), sequence (std::string ("cde")));

    check (unequal_examples, examples);
}

#endif // MATH_TEST_MATH_TEST_SEQUENCE_TESTS_SLOW_HPP_INCLUDED
