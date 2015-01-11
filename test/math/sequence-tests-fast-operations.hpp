/*
Copyright 2014, 2015 Rogier van Dalen.

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

#ifndef MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED
#define MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED

#include "math/sequence.hpp"

#include <string>

#include <boost/mpl/assert.hpp>

template <class Direction> void test_construction() {
    {
        typedef math::sequence <char, Direction> sequence;
        typedef math::empty_sequence <char, Direction> empty_sequence;
        typedef math::optional_sequence <char, Direction> optional_sequence;
        typedef math::single_sequence <char, Direction> single_sequence;

        std::string empty_string;

        // Construction.
        {
            empty_sequence empty1;
            BOOST_CHECK (range::empty (empty1.symbols()));

            empty_sequence empty2 (empty_string);
            BOOST_CHECK (range::empty (empty2.symbols()));
            BOOST_CHECK (empty2 == empty1);

            sequence seq1;
            BOOST_CHECK (range::empty (seq1.symbols()));
            BOOST_CHECK (seq1 == empty1);

            sequence seq2 (empty_string);
            BOOST_CHECK (range::empty (seq2.symbols()));
            BOOST_CHECK (seq2 == empty1);

            optional_sequence optional1;
            BOOST_CHECK (range::empty (optional1.symbols()));
            BOOST_CHECK (optional1 == empty1);

            optional_sequence optional2 (empty_string);
            BOOST_CHECK (range::empty (optional2.symbols()));
            BOOST_CHECK (optional2 == empty1);
        }
        {
            single_sequence single1 ('z');
            BOOST_CHECK_EQUAL (range::size (single1.symbols()), 1);
            BOOST_CHECK_EQUAL (range::first (single1.symbols()), 'z');

            single_sequence single2 (std::string ("z"));
            BOOST_CHECK_EQUAL (range::size (single2.symbols()), 1);
            BOOST_CHECK_EQUAL (range::first (single2.symbols()), 'z');
            BOOST_CHECK (single2 == single1);

            sequence seq (std::string ("z"));
            BOOST_CHECK_EQUAL (range::size (seq.symbols()), 1);
            BOOST_CHECK_EQUAL (range::first (seq.symbols()), 'z');
            BOOST_CHECK (seq == single1);

            optional_sequence optional (std::string ("z"));
            BOOST_CHECK_EQUAL (range::size (optional.symbols()), 1);
            BOOST_CHECK_EQUAL (range::first (optional.symbols()), 'z');

            BOOST_CHECK (optional == single1);
        }
    }

    // Try to confuse with overloads.
    // If the symbol type is a range, initialising the sequences may become
    // tricky.
    {
        std::vector <std::string> symbols;
        symbols.push_back ("ab");
        // Initialise sequence<> with a sequence. That should be fine.
        math::sequence <std::string, Direction> sequence (symbols);

        // This should use the string as one symbol, not try to initialise with
        // a whole
        math::single_sequence <std::string, Direction> single1 (
            std::string ("ab"));
        math::single_sequence <std::string, Direction> single2 (symbols);
        math::optional_sequence <std::string, Direction> optional1 (
            std::string ("ab"));
        math::optional_sequence <std::string, Direction> optional2 (symbols);

        BOOST_CHECK (single1 == sequence);
        BOOST_CHECK (single2 == sequence);
        BOOST_CHECK (optional1 == sequence);
        BOOST_CHECK (optional2 == sequence);
    }
}

/* Check operations. */

#define CHECK_OPERATOR_TYPE(operation, left_type, right_type, result_type) \
    BOOST_MPL_ASSERT ((std::is_same < \
        typename std::decay <typename math::result_of < \
            operation (left_type, right_type)>::type>::type, result_type>))

#define CHECK_TIMES_TYPE(left_type, right_type, result_type) \
    CHECK_OPERATOR_TYPE (math::callable::times, \
        left_type, right_type, result_type)

#define CHECK_PLUS_TYPE(left_type, right_type, result_type) \
    CHECK_OPERATOR_TYPE (math::callable::plus, \
        left_type, right_type, result_type)

#define CHECK_PICK_TYPE(condition_type, left_type, right_type, result_type) \
    BOOST_MPL_ASSERT ((std::is_same < \
        typename std::decay <typename math::result_of < \
            math::callable::pick (condition_type, left_type, right_type) \
            >::type>::type, result_type>))

#define CHECK_CHOOSE_TYPE(left_type, right_type, result_type) \
    CHECK_OPERATOR_TYPE (math::callable::choose, \
        left_type, right_type, result_type)

#define CHECK_DIVIDE_TYPE(Direction, left_type, right_type, result_type) \
    CHECK_OPERATOR_TYPE (math::callable::divide <Direction>, \
        left_type, right_type, result_type)

template <class Sequence> bool convertible_to_optional (Sequence const & s) {
    if (s.is_annihilator())
        return false;
    else {
        auto length = range::size (s.symbols());
        return length == 0 || length == 1;
    }
}

// Specialisation, because sequence_annihilator does not have .symbols().
template <class Symbol, class Direction> bool convertible_to_optional (
    math::sequence_annihilator <Symbol, Direction> const &)
{ return false; }

template <class Sequence, class OptionalSequence, class Operation,
        class Left, class Right, class ExpectedResult>
    inline void check_binary_operation (Operation const & operation,
        Left const & left, Right const & right,
        ExpectedResult const & expected_result)
{
    // Test operation.
    BOOST_CHECK_EQUAL (operation (left, right), expected_result);
    // Test operation on main sequence type.
    BOOST_CHECK_EQUAL (operation (Sequence (left), right), expected_result);
    BOOST_CHECK_EQUAL (operation (left, Sequence (right)), expected_result);
    BOOST_CHECK_EQUAL (
        operation (Sequence (left), Sequence (right)), expected_result);

    // Test operation on optional sequence type.
    if (convertible_to_optional (left)) {
        BOOST_CHECK_EQUAL (
            operation (OptionalSequence (left), right), expected_result);
        BOOST_CHECK_EQUAL (
            operation (OptionalSequence (left), Sequence (right)),
            expected_result);
        if (convertible_to_optional (right)) {
            BOOST_CHECK_EQUAL (
                operation (OptionalSequence (left), OptionalSequence (right)),
                expected_result);
        }
    }
    if (convertible_to_optional (right)) {
        BOOST_CHECK_EQUAL (
            operation (Sequence (left), OptionalSequence (right)),
            expected_result);
        BOOST_CHECK_EQUAL (
            operation (left, OptionalSequence (right)), expected_result);
    }
}

template <class Sequence, class OptionalSequence, class Operation>
    struct check_binary_operation_2
{
    template <class Left, class Right, class ExpectedResult>
        inline void operator() (Left const & left, Right const & right,
            ExpectedResult const & expected_result) const
    {
        Operation operation;
        // Test operation.
        BOOST_CHECK_EQUAL (operation (left, right), expected_result);
        // Test operation on main sequence type.
        BOOST_CHECK_EQUAL (operation (Sequence (left), right), expected_result);
        BOOST_CHECK_EQUAL (operation (left, Sequence (right)), expected_result);
        BOOST_CHECK_EQUAL (
            operation (Sequence (left), Sequence (right)), expected_result);

        // Test operation on optional sequence type.
        if (convertible_to_optional (left)) {
            BOOST_CHECK_EQUAL (
                operation (OptionalSequence (left), right), expected_result);
            BOOST_CHECK_EQUAL (
                operation (OptionalSequence (left), Sequence (right)),
                expected_result);
            if (convertible_to_optional (right)) {
                BOOST_CHECK_EQUAL (operation (
                        OptionalSequence (left), OptionalSequence (right)),
                    expected_result);
            }
        }
        if (convertible_to_optional (right)) {
            BOOST_CHECK_EQUAL (
                operation (Sequence (left), OptionalSequence (right)),
                expected_result);
            BOOST_CHECK_EQUAL (
                operation (left, OptionalSequence (right)), expected_result);
        }
    }
};

// This is the same as check_binary_operation, except it checks that an
// exception is thrown.
template <class Sequence, class OptionalSequence, class Exception,
        class Operation, class Left, class Right>
    inline void check_binary_operation_throw (Operation const & operation,
        Left const & left, Right const & right)
{
    // Test operation.
    BOOST_CHECK_THROW (operation (left, right), Exception);
    // Test operation on main sequence type.
    BOOST_CHECK_THROW (operation (Sequence (left), right), Exception);
    BOOST_CHECK_THROW (operation (left, Sequence (right)), Exception);
    BOOST_CHECK_THROW (
        operation (Sequence (left), Sequence (right)), Exception);

    // Test operation on optional sequence type.
    if (convertible_to_optional (left)) {
        BOOST_CHECK_THROW (
            operation (OptionalSequence (left), right), Exception);
        BOOST_CHECK_THROW (
            operation (OptionalSequence (left), Sequence (right)),
            Exception);
        if (convertible_to_optional (right)) {
            BOOST_CHECK_THROW (
                operation (OptionalSequence (left), OptionalSequence (right)),
                Exception);
        }
    }
    if (convertible_to_optional (right)) {
        BOOST_CHECK_THROW (
            operation (Sequence (left), OptionalSequence (right)),
            Exception);
        BOOST_CHECK_THROW (
            operation (left, OptionalSequence (right)), Exception);
    }
}

/**
Test basic properties of comparisons.
The parameters are potentially reversed, for the right sequence, so that plus
can be tested.
*/
template <class Direction>
    void test_comparison (std::string const & ab_, std::string const & abc_)
{
    typedef math::sequence_annihilator <char, Direction> sequence_annihilator;
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;

    /* Annihilators. */
    sequence_annihilator annihilator;
    {
        typedef decltype (annihilator.is_annihilator()) is_annihilator;
        static_assert (is_annihilator::value, "");
    }
    sequence annihilator2 (annihilator);
    BOOST_CHECK (annihilator2.is_annihilator());

    BOOST_CHECK_EQUAL (annihilator, annihilator2);

    /* Empty sequences. */
    empty_sequence empty;
    {
        typedef decltype (empty.is_annihilator()) is_annihilator;
        static_assert (!is_annihilator::value, "");
        typedef decltype (empty.empty()) is_empty;
        static_assert (is_empty::value, "");
    }
    optional_sequence empty2;
    BOOST_CHECK (!empty2.is_annihilator());
    BOOST_CHECK (empty2.empty());
    {
        optional_sequence empty2a (empty);
        BOOST_CHECK (!empty2a.is_annihilator());
        BOOST_CHECK (empty2a.empty());
    }
    sequence empty3;
    BOOST_CHECK (!empty3.is_annihilator());
    BOOST_CHECK (empty3.empty());
    {
        sequence empty3a (empty);
        BOOST_CHECK (!empty3a.is_annihilator());
        BOOST_CHECK (empty3a.empty());
    }

    BOOST_CHECK_EQUAL (empty, empty2);
    BOOST_CHECK_EQUAL (empty, empty3);
    BOOST_CHECK_EQUAL (empty2, empty3);

    /* Single-symbol sequences. */
    single_sequence a ('a');
    single_sequence b ('b');
    {
        typedef decltype (a.is_annihilator()) is_annihilator;
        static_assert (!is_annihilator::value, "");
        typedef decltype (a.empty()) is_empty;
        static_assert (!is_empty::value, "");
    }
    BOOST_CHECK_EQUAL (range::size (a.symbols()), 1u);
    BOOST_CHECK_EQUAL (range::first (a.symbols()), 'a');
    BOOST_CHECK_EQUAL (range::size (b.symbols()), 1u);
    BOOST_CHECK_EQUAL (range::first (b.symbols()), 'b');
    BOOST_CHECK (a != b);

    optional_sequence a2 ('a');
    optional_sequence b2 ('b');
    {
        typedef decltype (a2.is_annihilator()) is_annihilator;
        static_assert (!is_annihilator::value, "");
    }
    BOOST_CHECK (!a2.empty());
    BOOST_CHECK_EQUAL (range::size (a2.symbols()), 1u);
    BOOST_CHECK_EQUAL (range::first (a2.symbols()), 'a');
    {
        optional_sequence a2a (a);
        BOOST_CHECK (!a2a.empty());
        BOOST_CHECK_EQUAL (a2a, a2);
        BOOST_CHECK_EQUAL (a2a, a);
    }
    BOOST_CHECK_EQUAL (a2, a);
    BOOST_CHECK_EQUAL (b2, b);

    sequence a3 (std::string ("a"));
    sequence b3 (std::string ("b"));
    BOOST_CHECK (!a3.is_annihilator());
    BOOST_CHECK (!a3.empty());
    BOOST_CHECK_EQUAL (range::size (a3.symbols()), 1u);
    BOOST_CHECK_EQUAL (range::first (a3.symbols()), 'a');
    {
        sequence a3a (a);
        BOOST_CHECK (!a3a.empty());
        BOOST_CHECK_EQUAL (a3a, a3);
        BOOST_CHECK_EQUAL (a3a, a2);
        BOOST_CHECK_EQUAL (a3a, a);
    }
    BOOST_CHECK_EQUAL (a3, a);
    BOOST_CHECK_EQUAL (a3, a2);
    BOOST_CHECK_EQUAL (b3, b);
    BOOST_CHECK_EQUAL (b3, b2);

    sequence ab (std::string ("ab"));
    BOOST_CHECK_EQUAL (range::size (ab.symbols()), 2u);
    BOOST_CHECK_EQUAL (range::first (ab.symbols()), 'a');
    BOOST_CHECK_EQUAL (range::at (1, ab.symbols()), 'b');
    sequence ac (std::string ("ac"));
    sequence bc (std::string ("bc"));
    sequence ca (std::string ("ca"));
    sequence r_ab (ab_);
    sequence r_abc (abc_);

    // Compare all combinations of two objects.
    BOOST_CHECK (empty == empty);
    BOOST_CHECK (empty == empty2);
    BOOST_CHECK (empty == empty3);
    BOOST_CHECK (empty != a);
    BOOST_CHECK (empty != a2);
    BOOST_CHECK (empty != a3);
    BOOST_CHECK (empty != r_ab);
    BOOST_CHECK (empty != r_abc);
    BOOST_CHECK (empty != b);
    BOOST_CHECK (empty != annihilator);
    BOOST_CHECK (empty != annihilator2);

    BOOST_CHECK (empty2 == empty);
    BOOST_CHECK (empty2 == empty2);
    BOOST_CHECK (empty2 == empty3);
    BOOST_CHECK (empty2 != a);
    BOOST_CHECK (empty2 != a2);
    BOOST_CHECK (empty2 != a3);
    BOOST_CHECK (empty2 != r_ab);
    BOOST_CHECK (empty2 != r_abc);
    BOOST_CHECK (empty2 != b);
    BOOST_CHECK (empty2 != annihilator);
    BOOST_CHECK (empty2 != annihilator2);

    BOOST_CHECK (empty3 == empty);
    BOOST_CHECK (empty3 == empty2);
    BOOST_CHECK (empty3 == empty3);
    BOOST_CHECK (empty3 != a);
    BOOST_CHECK (empty3 != a2);
    BOOST_CHECK (empty3 != a3);
    BOOST_CHECK (empty3 != r_ab);
    BOOST_CHECK (empty3 != r_abc);
    BOOST_CHECK (empty3 != b);
    BOOST_CHECK (empty3 != annihilator);
    BOOST_CHECK (empty3 != annihilator2);

    BOOST_CHECK (a != empty);
    BOOST_CHECK (a != empty2);
    BOOST_CHECK (a != empty3);
    BOOST_CHECK (a == a);
    BOOST_CHECK (a == a2);
    BOOST_CHECK (a == a3);
    BOOST_CHECK (a != r_ab);
    BOOST_CHECK (a != r_abc);
    BOOST_CHECK (a != b);
    BOOST_CHECK (a != annihilator);
    BOOST_CHECK (a != annihilator2);

    BOOST_CHECK (a2 != empty);
    BOOST_CHECK (a2 != empty2);
    BOOST_CHECK (a2 != empty3);
    BOOST_CHECK (a2 == a);
    BOOST_CHECK (a2 == a2);
    BOOST_CHECK (a2 == a3);
    BOOST_CHECK (a2 != r_ab);
    BOOST_CHECK (a2 != r_abc);
    BOOST_CHECK (a2 != b);
    BOOST_CHECK (a2 != annihilator);
    BOOST_CHECK (a2 != annihilator2);

    BOOST_CHECK (a3 != empty);
    BOOST_CHECK (a3 != empty2);
    BOOST_CHECK (a3 != empty3);
    BOOST_CHECK (a3 == a);
    BOOST_CHECK (a3 == a2);
    BOOST_CHECK (a3 == a3);
    BOOST_CHECK (a3 != r_ab);
    BOOST_CHECK (a3 != r_abc);
    BOOST_CHECK (a3 != b);
    BOOST_CHECK (a3 != annihilator);
    BOOST_CHECK (a3 != annihilator2);

    BOOST_CHECK (r_ab != empty);
    BOOST_CHECK (r_ab != empty2);
    BOOST_CHECK (r_ab != empty3);
    BOOST_CHECK (r_ab != a);
    BOOST_CHECK (r_ab != a2);
    BOOST_CHECK (r_ab != a3);
    BOOST_CHECK (r_ab == r_ab);
    BOOST_CHECK (r_ab != r_abc);
    BOOST_CHECK (r_ab != b);
    BOOST_CHECK (r_ab != annihilator);
    BOOST_CHECK (r_ab != annihilator2);

    BOOST_CHECK (r_abc != empty);
    BOOST_CHECK (r_abc != empty2);
    BOOST_CHECK (r_abc != empty3);
    BOOST_CHECK (r_abc != a);
    BOOST_CHECK (r_abc != a2);
    BOOST_CHECK (r_abc != a3);
    BOOST_CHECK (r_abc != r_ab);
    BOOST_CHECK (r_abc == r_abc);
    BOOST_CHECK (r_abc != b);
    BOOST_CHECK (r_abc != annihilator);
    BOOST_CHECK (r_abc != annihilator2);

    BOOST_CHECK (b != empty);
    BOOST_CHECK (b != empty2);
    BOOST_CHECK (b != empty3);
    BOOST_CHECK (b != a);
    BOOST_CHECK (b != a2);
    BOOST_CHECK (b != a3);
    BOOST_CHECK (b != r_ab);
    BOOST_CHECK (b != r_abc);
    BOOST_CHECK (b == b);
    BOOST_CHECK (b != annihilator);
    BOOST_CHECK (b != annihilator2);

    BOOST_CHECK (annihilator != empty);
    BOOST_CHECK (annihilator != empty2);
    BOOST_CHECK (annihilator != empty3);
    BOOST_CHECK (annihilator != a);
    BOOST_CHECK (annihilator != a2);
    BOOST_CHECK (annihilator != a3);
    BOOST_CHECK (annihilator != r_ab);
    BOOST_CHECK (annihilator != r_abc);
    BOOST_CHECK (annihilator != b);
    BOOST_CHECK (annihilator == annihilator);
    BOOST_CHECK (annihilator == annihilator2);

    BOOST_CHECK (annihilator2 != empty);
    BOOST_CHECK (annihilator2 != empty2);
    BOOST_CHECK (annihilator2 != empty3);
    BOOST_CHECK (annihilator2 != a);
    BOOST_CHECK (annihilator2 != a2);
    BOOST_CHECK (annihilator2 != a3);
    BOOST_CHECK (annihilator2 != r_ab);
    BOOST_CHECK (annihilator2 != r_abc);
    BOOST_CHECK (annihilator2 != b);
    BOOST_CHECK (annihilator2 == annihilator);
    BOOST_CHECK (annihilator2 == annihilator2);

    /* Compare for inequality. */
    BOOST_CHECK (!(empty < empty));
    BOOST_CHECK (!(empty < empty2));
    BOOST_CHECK (!(empty < empty3));
    BOOST_CHECK (empty < a);
    BOOST_CHECK (empty < a2);
    BOOST_CHECK (empty < a3);
    BOOST_CHECK (empty < r_ab);
    BOOST_CHECK (empty < r_abc);
    BOOST_CHECK (empty < b);
    BOOST_CHECK (empty < annihilator);
    BOOST_CHECK (empty < annihilator2);

    BOOST_CHECK (!(empty2 < empty));
    BOOST_CHECK (!(empty2 < empty2));
    BOOST_CHECK (!(empty2 < empty3));
    BOOST_CHECK (empty2 < a);
    BOOST_CHECK (empty2 < a2);
    BOOST_CHECK (empty2 < a3);
    BOOST_CHECK (empty2 < r_ab);
    BOOST_CHECK (empty2 < r_abc);
    BOOST_CHECK (empty2 < b);
    BOOST_CHECK (empty2 < annihilator);
    BOOST_CHECK (empty2 < annihilator2);

    BOOST_CHECK (!(empty3 < empty));
    BOOST_CHECK (!(empty3 < empty2));
    BOOST_CHECK (!(empty3 < empty3));
    BOOST_CHECK (empty3 < a);
    BOOST_CHECK (empty3 < a2);
    BOOST_CHECK (empty3 < a3);
    BOOST_CHECK (empty3 < r_ab);
    BOOST_CHECK (empty3 < r_abc);
    BOOST_CHECK (empty3 < b);
    BOOST_CHECK (empty3 < annihilator);
    BOOST_CHECK (empty3 < annihilator2);

    BOOST_CHECK (!(a < empty));
    BOOST_CHECK (!(a < empty2));
    BOOST_CHECK (!(a < empty3));
    BOOST_CHECK (!(a < a));
    BOOST_CHECK (!(a < a2));
    BOOST_CHECK (!(a < a3));
    BOOST_CHECK (a < r_ab);
    BOOST_CHECK (a < r_abc);
    BOOST_CHECK (a < b);
    BOOST_CHECK (a < annihilator);
    BOOST_CHECK (a < annihilator2);

    BOOST_CHECK (!(a2 < empty));
    BOOST_CHECK (!(a2 < empty2));
    BOOST_CHECK (!(a2 < empty3));
    BOOST_CHECK (!(a2 < a));
    BOOST_CHECK (!(a2 < a2));
    BOOST_CHECK (!(a2 < a3));
    BOOST_CHECK (a2 < r_ab);
    BOOST_CHECK (a2 < r_abc);
    BOOST_CHECK (a2 < b);
    BOOST_CHECK (a2 < annihilator);
    BOOST_CHECK (a2 < annihilator2);

    BOOST_CHECK (!(a3 < empty));
    BOOST_CHECK (!(a3 < empty2));
    BOOST_CHECK (!(a3 < empty3));
    BOOST_CHECK (!(a3 < a));
    BOOST_CHECK (!(a3 < a2));
    BOOST_CHECK (!(a3 < a3));
    BOOST_CHECK (a3 < r_ab);
    BOOST_CHECK (a3 < r_abc);
    BOOST_CHECK (a3 < b);
    BOOST_CHECK (a3 < annihilator);
    BOOST_CHECK (a3 < annihilator2);

    BOOST_CHECK (!(r_ab < empty));
    BOOST_CHECK (!(r_ab < empty2));
    BOOST_CHECK (!(r_ab < empty3));
    BOOST_CHECK (!(r_ab < a));
    BOOST_CHECK (!(r_ab < a2));
    BOOST_CHECK (!(r_ab < a3));
    BOOST_CHECK (!(r_ab < r_ab));
    BOOST_CHECK (r_ab < r_abc);
    BOOST_CHECK (r_ab < b);
    BOOST_CHECK (r_ab < annihilator);
    BOOST_CHECK (r_ab < annihilator2);

    BOOST_CHECK (!(r_abc < empty));
    BOOST_CHECK (!(r_abc < empty2));
    BOOST_CHECK (!(r_abc < empty3));
    BOOST_CHECK (!(r_abc < a));
    BOOST_CHECK (!(r_abc < a2));
    BOOST_CHECK (!(r_abc < a3));
    BOOST_CHECK (!(r_abc < r_ab));
    BOOST_CHECK (!(r_abc < r_abc));
    BOOST_CHECK (r_abc < b);
    BOOST_CHECK (r_abc < annihilator);
    BOOST_CHECK (r_abc < annihilator2);

    BOOST_CHECK (!(b < empty));
    BOOST_CHECK (!(b < empty2));
    BOOST_CHECK (!(b < empty3));
    BOOST_CHECK (!(b < a));
    BOOST_CHECK (!(b < a2));
    BOOST_CHECK (!(b < a3));
    BOOST_CHECK (!(b < r_ab));
    BOOST_CHECK (!(b < r_abc));
    BOOST_CHECK (!(b < b));
    BOOST_CHECK (b < annihilator);
    BOOST_CHECK (b < annihilator2);

    BOOST_CHECK (!(annihilator < empty));
    BOOST_CHECK (!(annihilator < empty2));
    BOOST_CHECK (!(annihilator < empty3));
    BOOST_CHECK (!(annihilator < a));
    BOOST_CHECK (!(annihilator < a2));
    BOOST_CHECK (!(annihilator < a3));
    BOOST_CHECK (!(annihilator < r_ab));
    BOOST_CHECK (!(annihilator < r_abc));
    BOOST_CHECK (!(annihilator < b));
    BOOST_CHECK (!(annihilator < annihilator));
    BOOST_CHECK (!(annihilator < annihilator2));

    BOOST_CHECK (!(annihilator2 < empty));
    BOOST_CHECK (!(annihilator2 < empty2));
    BOOST_CHECK (!(annihilator2 < empty3));
    BOOST_CHECK (!(annihilator2 < a));
    BOOST_CHECK (!(annihilator2 < a2));
    BOOST_CHECK (!(annihilator2 < a3));
    BOOST_CHECK (!(annihilator2 < r_ab));
    BOOST_CHECK (!(annihilator2 < r_abc));
    BOOST_CHECK (!(annihilator2 < b));
    BOOST_CHECK (!(annihilator2 < annihilator));
    BOOST_CHECK (!(annihilator2 < annihilator2));
}

/**
Test basic properties of times.
*/
template <class Direction> void test_times() {
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;
    typedef math::sequence_annihilator <char, Direction> sequence_annihilator;

    /* Check types. */
    CHECK_TIMES_TYPE (sequence_annihilator, sequence_annihilator,
        sequence_annihilator);
    CHECK_TIMES_TYPE (sequence_annihilator, empty_sequence,
        sequence_annihilator);
    CHECK_TIMES_TYPE (sequence_annihilator, single_sequence,
        sequence_annihilator);
    CHECK_TIMES_TYPE (sequence_annihilator, optional_sequence,
        sequence_annihilator);
    CHECK_TIMES_TYPE (sequence_annihilator, sequence, sequence_annihilator);

    CHECK_TIMES_TYPE (empty_sequence, sequence_annihilator,
        sequence_annihilator);
    CHECK_TIMES_TYPE (empty_sequence, empty_sequence, empty_sequence);
    CHECK_TIMES_TYPE (empty_sequence, single_sequence, single_sequence);
    CHECK_TIMES_TYPE (empty_sequence, optional_sequence, optional_sequence);
    CHECK_TIMES_TYPE (empty_sequence, sequence, sequence);

    CHECK_TIMES_TYPE (single_sequence, sequence_annihilator,
        sequence_annihilator);
    CHECK_TIMES_TYPE (single_sequence, empty_sequence, single_sequence);
    CHECK_TIMES_TYPE (single_sequence, single_sequence, sequence);
    CHECK_TIMES_TYPE (single_sequence, optional_sequence, sequence);
    CHECK_TIMES_TYPE (single_sequence, sequence, sequence);

    CHECK_TIMES_TYPE (optional_sequence, sequence_annihilator,
        sequence_annihilator);
    CHECK_TIMES_TYPE (optional_sequence, empty_sequence, optional_sequence);
    CHECK_TIMES_TYPE (optional_sequence, single_sequence, sequence);
    CHECK_TIMES_TYPE (optional_sequence, optional_sequence, sequence);
    CHECK_TIMES_TYPE (optional_sequence, sequence, sequence);

    CHECK_TIMES_TYPE (sequence, sequence_annihilator, sequence_annihilator);
    CHECK_TIMES_TYPE (sequence, empty_sequence, sequence);
    CHECK_TIMES_TYPE (sequence, single_sequence, sequence);
    CHECK_TIMES_TYPE (sequence, optional_sequence, sequence);
    CHECK_TIMES_TYPE (sequence, sequence, sequence);

    /* Check results. */

    empty_sequence empty;
    single_sequence a ('a');
    single_sequence b ('b');
    sequence ab (std::string ("ab"));
    sequence abc (std::string ("abc"));
    sequence_annihilator annihilator;

    check_binary_operation_2 <
        sequence, optional_sequence, math::callable::times> check;

    check (annihilator, annihilator, annihilator);
    check (annihilator, empty, annihilator);
    check (annihilator, a, annihilator);
    check (annihilator, ab, annihilator);
    check (annihilator, abc, annihilator);

    check (empty, annihilator, annihilator);
    check (empty, empty, empty);
    check (empty, a, a);
    check (empty, ab, ab);
    check (empty, abc, abc);

    check (a, annihilator, annihilator);
    check (a, empty, a);
    check (a, b, ab);
    check (a, sequence (std::string ("bc")), abc);

    check (ab, annihilator, annihilator);
    check (ab, empty, ab);
    check (ab, a, sequence (std::string ("aba")));
    check (ab, sequence (std::string ("cd")), sequence (std::string ("abcd")));
    check (ab, abc, sequence (std::string ("ababc")));

    check (abc, annihilator, annihilator);
    check (abc, empty, abc);
    check (abc, a, sequence (std::string ("abca")));
    check (abc, sequence (std::string ("de")),
        sequence (std::string ("abcde")));
    check (abc, abc, sequence (std::string ("abcabc")));
}

/**
Test basic properties of plus.
This is supposed to compute the longest common prefix/suffix, from Direction.
The parameters are potentially reversed, so that directionality can be tested.
*/
template <class Direction>
    void test_plus (std::string const & ab_, std::string const & abc_)
{
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;
    typedef math::sequence_annihilator <char, Direction> sequence_annihilator;

    /* Check types. */
    CHECK_PLUS_TYPE (sequence_annihilator, sequence_annihilator,
        sequence_annihilator);
    CHECK_PLUS_TYPE (sequence_annihilator, empty_sequence, empty_sequence);
    CHECK_PLUS_TYPE (sequence_annihilator, single_sequence, single_sequence);
    CHECK_PLUS_TYPE (sequence_annihilator, optional_sequence,
        optional_sequence);
    CHECK_PLUS_TYPE (sequence_annihilator, sequence, sequence);

    CHECK_PLUS_TYPE (empty_sequence, sequence_annihilator, empty_sequence);
    CHECK_PLUS_TYPE (empty_sequence, empty_sequence, empty_sequence);
    CHECK_PLUS_TYPE (empty_sequence, single_sequence, empty_sequence);
    CHECK_PLUS_TYPE (empty_sequence, optional_sequence, empty_sequence);
    CHECK_PLUS_TYPE (empty_sequence, sequence, empty_sequence);

    CHECK_PLUS_TYPE (single_sequence, sequence_annihilator, single_sequence);
    CHECK_PLUS_TYPE (single_sequence, empty_sequence, empty_sequence);
    CHECK_PLUS_TYPE (single_sequence, single_sequence, optional_sequence);
    CHECK_PLUS_TYPE (single_sequence, optional_sequence, optional_sequence);
    CHECK_PLUS_TYPE (single_sequence, sequence, optional_sequence);

    CHECK_PLUS_TYPE (optional_sequence, sequence_annihilator,
        optional_sequence);
    CHECK_PLUS_TYPE (optional_sequence, empty_sequence, empty_sequence);
    CHECK_PLUS_TYPE (optional_sequence, single_sequence, optional_sequence);
    CHECK_PLUS_TYPE (optional_sequence, optional_sequence, optional_sequence);
    CHECK_PLUS_TYPE (optional_sequence, sequence, optional_sequence);

    CHECK_PLUS_TYPE (sequence, sequence_annihilator, sequence);
    CHECK_PLUS_TYPE (sequence, empty_sequence, empty_sequence);
    CHECK_PLUS_TYPE (sequence, single_sequence, optional_sequence);
    CHECK_PLUS_TYPE (sequence, optional_sequence, optional_sequence);
    CHECK_PLUS_TYPE (sequence, sequence, sequence);

    /* Check results. */

    sequence_annihilator annihilator;
    empty_sequence empty;
    single_sequence a ('a');
    single_sequence b ('b');
    sequence r_ab (ab_);
    sequence r_abc (abc_);

    check_binary_operation_2 <sequence, optional_sequence, math::callable::plus>
        check;

    check (annihilator, annihilator, annihilator);
    check (annihilator, empty, empty);
    check (annihilator, a, a);
    check (annihilator, r_ab, r_ab);
    check (annihilator, r_abc, r_abc);

    check (empty, annihilator, empty);
    check (empty, empty, empty);
    check (empty, a, empty);
    check (empty, r_ab, empty);
    check (empty, r_abc, empty);

    check (a, annihilator, a);
    check (a, empty, empty);
    check (a, a, a);
    check (a, b, empty);
    check (a, r_ab, a);
    check (a, r_abc, a);

    check (r_ab, annihilator, r_ab);
    check (r_ab, empty, empty);
    check (r_ab, a, a);
    check (r_ab, b, empty);
    check (r_ab, r_ab, r_ab);
    check (r_ab, r_abc, r_ab);


    check (r_abc, annihilator, r_abc);
    check (r_abc, empty, empty);
    check (r_abc, a, a);
    check (r_abc, b, empty);
    check (r_abc, r_ab, r_ab);
    check (r_abc, r_abc, r_abc);
}

/**
Test basic return type of "pick".
Apart from selecting the return type, the implementation is trivial, so the
implementation is only spot-checked.
*/
template <class Direction> void test_pick() {
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty;
    typedef math::single_sequence <char, Direction> single;
    typedef math::optional_sequence <char, Direction> optional;
    typedef math::sequence_annihilator <char, Direction> annihilator;

    typedef std::true_type true_t;
    typedef rime::false_type false_t;

    // Run-time condition.
    CHECK_PICK_TYPE (bool, empty, empty, empty);
    CHECK_PICK_TYPE (bool, empty, single, optional);
    CHECK_PICK_TYPE (bool, empty, optional, optional);
    CHECK_PICK_TYPE (bool, empty, sequence, sequence);
    CHECK_PICK_TYPE (bool, empty, annihilator, sequence);

    CHECK_PICK_TYPE (bool, single, empty, optional);
    CHECK_PICK_TYPE (bool, single, single, single);
    CHECK_PICK_TYPE (bool, single, optional, optional);
    CHECK_PICK_TYPE (bool, single, sequence, sequence);
    CHECK_PICK_TYPE (bool, single, annihilator, sequence);

    CHECK_PICK_TYPE (bool, optional, empty, optional);
    CHECK_PICK_TYPE (bool, optional, single, optional);
    CHECK_PICK_TYPE (bool, optional, optional, optional);
    CHECK_PICK_TYPE (bool, optional, sequence, sequence);
    CHECK_PICK_TYPE (bool, optional, annihilator, sequence);

    CHECK_PICK_TYPE (bool, sequence, empty, sequence);
    CHECK_PICK_TYPE (bool, sequence, single, sequence);
    CHECK_PICK_TYPE (bool, sequence, optional, sequence);
    CHECK_PICK_TYPE (bool, sequence, sequence, sequence);
    CHECK_PICK_TYPE (bool, sequence, annihilator, sequence);

    CHECK_PICK_TYPE (bool, annihilator, empty, sequence);
    CHECK_PICK_TYPE (bool, annihilator, single, sequence);
    CHECK_PICK_TYPE (bool, annihilator, optional, sequence);
    CHECK_PICK_TYPE (bool, annihilator, sequence, sequence);
    CHECK_PICK_TYPE (bool, annihilator, annihilator, annihilator);

    // Compile-time types: pick left or right. Spot tests.
    CHECK_PICK_TYPE (true_t, empty, empty, empty);
    CHECK_PICK_TYPE (false_t, optional, empty, empty);
    CHECK_PICK_TYPE (true_t, empty, single, empty);
    CHECK_PICK_TYPE (false_t, optional, single, single);
    CHECK_PICK_TYPE (true_t, empty, sequence, empty);
    CHECK_PICK_TYPE (false_t, single, optional, optional);
    CHECK_PICK_TYPE (true_t, empty, sequence, empty);
    CHECK_PICK_TYPE (false_t, empty, sequence, sequence);
    CHECK_PICK_TYPE (true_t, empty, single, empty);
    CHECK_PICK_TYPE (false_t, sequence, annihilator, annihilator);

    // Run-time behaviour: spot checks.
    auto pick = math::pick;
    sequence abc (std::string ("abc"));
    single a ('a');

    BOOST_CHECK_EQUAL (pick (true, empty(), abc), empty());
    BOOST_CHECK_EQUAL (pick (false, empty(), abc), abc);

    BOOST_CHECK_EQUAL (pick (true_t(), empty(), abc), empty());
    BOOST_CHECK_EQUAL (pick (false_t(), empty(), abc), abc);

    BOOST_CHECK_EQUAL (pick (true, empty(), a), empty());
    BOOST_CHECK_EQUAL (pick (false, empty(), a), a);

    BOOST_CHECK_EQUAL (pick (true_t(), empty(), a), empty());
    BOOST_CHECK_EQUAL (pick (false_t(), empty(), a), a);
}

/**
Test basic properties of choose.
This is supposed to compute the longest common prefix/suffix, from Direction.
The parameters are potentially reversed, so that directionality can be tested.
*/
template <class Direction>
    void test_choose (std::string const & ab_, std::string const & abc_)
{
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;
    typedef math::sequence_annihilator <char, Direction> sequence_annihilator;

    /* Check types. */
    CHECK_CHOOSE_TYPE (empty_sequence, empty_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (empty_sequence, single_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (empty_sequence, optional_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (empty_sequence, sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (empty_sequence, sequence_annihilator, empty_sequence);

    CHECK_CHOOSE_TYPE (single_sequence, empty_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (single_sequence, single_sequence, single_sequence);
    CHECK_CHOOSE_TYPE (single_sequence, optional_sequence, optional_sequence);
    CHECK_CHOOSE_TYPE (single_sequence, sequence, sequence);
    CHECK_CHOOSE_TYPE (single_sequence, sequence_annihilator, single_sequence);

    CHECK_CHOOSE_TYPE (optional_sequence, empty_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (optional_sequence, single_sequence, optional_sequence);
    CHECK_CHOOSE_TYPE (optional_sequence, optional_sequence, optional_sequence);
    CHECK_CHOOSE_TYPE (optional_sequence, sequence, sequence);
    CHECK_CHOOSE_TYPE (optional_sequence, sequence_annihilator,
        optional_sequence);

    CHECK_CHOOSE_TYPE (sequence, empty_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (sequence, single_sequence, sequence);
    CHECK_CHOOSE_TYPE (sequence, optional_sequence, sequence);
    CHECK_CHOOSE_TYPE (sequence, sequence, sequence);
    CHECK_CHOOSE_TYPE (sequence, sequence_annihilator, sequence);

    CHECK_CHOOSE_TYPE (sequence_annihilator, sequence_annihilator,
        sequence_annihilator);
    CHECK_CHOOSE_TYPE (sequence_annihilator, empty_sequence, empty_sequence);
    CHECK_CHOOSE_TYPE (sequence_annihilator, single_sequence, single_sequence);
    CHECK_CHOOSE_TYPE (sequence_annihilator, optional_sequence,
        optional_sequence);
    CHECK_CHOOSE_TYPE (sequence_annihilator, sequence, sequence);

    /* Check results. */

    sequence_annihilator annihilator;
    empty_sequence empty;
    single_sequence a ('a');
    single_sequence b ('b');
    sequence r_ab (ab_);
    sequence r_abc (abc_);

    check_binary_operation_2 <sequence, optional_sequence,
        math::callable::choose> check;

    check (empty, empty, empty);
    check (empty, a, empty);
    check (empty, r_ab, empty);
    check (empty, r_abc, empty);
    check (empty, annihilator, empty);

    check (a, empty, empty);
    check (a, a, a);
    check (a, b, a);
    check (a, r_ab, a);
    check (a, r_abc, a);
    check (r_ab, r_abc, r_ab);
    check (a, annihilator, a);

    check (r_ab, empty, empty);
    check (r_ab, a, a);
    check (r_ab, b, r_ab);
    check (r_ab, r_ab, r_ab);
    check (r_ab, r_abc, r_ab);
    check (r_ab, annihilator, r_ab);

    check (r_abc, empty, empty);
    check (r_abc, a, a);
    check (r_abc, b, r_abc);
    check (r_abc, r_ab, r_ab);
    check (r_abc, r_abc, r_abc);
    check (r_abc, annihilator, r_abc);

    check (annihilator, empty, empty);
    check (annihilator, a, a);
    check (annihilator, r_ab, r_ab);
    check (annihilator, r_abc, r_abc);
    check (annihilator, annihilator, annihilator);
}

/**
Test basic properties of divide.
The parameters are potentially reversed, so that directionality can be tested.
*/
template <class Direction>
    void test_divide (std::string const & ab_, std::string const & abc_,
        std::string const & bc_)
{
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;
    typedef math::sequence_annihilator <char, Direction> sequence_annihilator;

    /* Check types. */
    // Undefined: CHECK_DIVIDE_TYPE (Direction, sequence_annihilator,
    //      sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, sequence_annihilator, empty_sequence,
        sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, sequence_annihilator, single_sequence,
        sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, sequence_annihilator, optional_sequence,
        sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, sequence_annihilator, sequence,
        sequence_annihilator);

    // Always throws: CHECK_DIVIDE_TYPE (Direction, empty_sequence,
    //      sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, empty_sequence, empty_sequence,
        empty_sequence);
    // Always throws: CHECK_DIVIDE_TYPE (Direction, empty_sequence,
    //      single_sequence);
    CHECK_DIVIDE_TYPE (Direction, empty_sequence, optional_sequence,
        empty_sequence);
    CHECK_DIVIDE_TYPE (Direction, empty_sequence, sequence, empty_sequence);

    // Always throws: CHECK_DIVIDE_TYPE (Direction, single_sequence,
    //      sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, single_sequence, empty_sequence,
        single_sequence);
    CHECK_DIVIDE_TYPE (Direction, single_sequence, single_sequence,
        empty_sequence);
    CHECK_DIVIDE_TYPE (Direction, single_sequence, optional_sequence,
        optional_sequence);
    CHECK_DIVIDE_TYPE (Direction, single_sequence, sequence, optional_sequence);

    // Always throws: CHECK_DIVIDE_TYPE (Direction, optional_sequence,
    //      sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, optional_sequence, empty_sequence,
        optional_sequence);
    CHECK_DIVIDE_TYPE (Direction, optional_sequence, single_sequence,
        empty_sequence);
    CHECK_DIVIDE_TYPE (Direction, optional_sequence, optional_sequence,
        optional_sequence);
    CHECK_DIVIDE_TYPE (Direction, optional_sequence, sequence,
        optional_sequence);

    // Always throws: CHECK_DIVIDE_TYPE (Direction, sequence,
    //      sequence_annihilator);
    CHECK_DIVIDE_TYPE (Direction, sequence, empty_sequence, sequence);
    CHECK_DIVIDE_TYPE (Direction, sequence, single_sequence, sequence);
    CHECK_DIVIDE_TYPE (Direction, sequence, optional_sequence, sequence);
    CHECK_DIVIDE_TYPE (Direction, sequence, sequence, sequence);

    /* Check results. */

    empty_sequence empty;
    single_sequence a ('a');
    single_sequence b ('b');
    sequence c (std::string ("c"));
    sequence r_ab (ab_);
    sequence r_abc (abc_);
    sequence r_bc (bc_);
    sequence_annihilator annihilator;

    check_binary_operation_2 <
        sequence, optional_sequence, math::callable::divide <Direction>> check;

    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), annihilator, annihilator);
    check (annihilator, empty, annihilator);
    check (annihilator, a, annihilator);
    check (annihilator, b, annihilator);
    check (annihilator, r_ab, annihilator);
    check (annihilator, r_abc, annihilator);

    check_binary_operation_throw <
        sequence, optional_sequence, math::inverse_of_annihilator> (
            math::callable::divide <Direction>(), empty, annihilator);
    check (empty, empty, empty);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), empty, a);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), empty, b);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), empty, r_ab);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), empty, r_abc);

    check_binary_operation_throw <
        sequence, optional_sequence, math::inverse_of_annihilator> (
            math::callable::divide <Direction>(), a, annihilator);
    check (a, empty, a);
    check (a, a, empty);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), a, b);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
        math::callable::divide <Direction>(), a, r_ab);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
        math::callable::divide <Direction>(), a, r_abc);

    check_binary_operation_throw <
        sequence, optional_sequence, math::inverse_of_annihilator> (
            math::callable::divide <Direction>(), b, annihilator);
    check (b, empty, b);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), b, a);
    check (b, b, empty);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), b, r_ab);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), b, r_abc);

    check_binary_operation_throw <
        sequence, optional_sequence, math::inverse_of_annihilator> (
            math::callable::divide <Direction>(), r_ab, annihilator);
    check (r_ab, empty, r_ab);
    check (r_ab, a, b);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), r_ab, b);
    check (r_ab, r_ab, empty);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), r_ab, r_abc);

    check_binary_operation_throw <
        sequence, optional_sequence, math::inverse_of_annihilator> (
            math::callable::divide <Direction>(), r_abc, annihilator);
    check (r_abc, empty, r_abc);
    check (r_abc, a, r_bc);
    check_binary_operation_throw <
        sequence, optional_sequence, math::operation_undefined> (
            math::callable::divide <Direction>(), r_abc, b);
    check (r_abc, r_ab, c);
    check (r_abc, r_abc, empty);
}

#endif // MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED
