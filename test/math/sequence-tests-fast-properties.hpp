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

#ifndef MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED
#define MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED

#include "math/sequence.hpp"

#include <string>

#include <boost/mpl/assert.hpp>

template <class Direction, class Opposite> void test_sequence_spot() {
    typedef math::sequence <char, Direction> sequence;
    typedef math::empty_sequence <char, Direction> empty_sequence;
    typedef math::optional_sequence <char, Direction> optional_sequence;
    typedef math::single_sequence <char, Direction> single_sequence;
    typedef math::sequence_annihilator <char, Direction>
        sequence_annihilator;

    typedef math::sequence <char, Opposite> opposite_sequence;

    // Conversion.
    {
        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <sequence, opposite_sequence>));

        empty_sequence e;
        single_sequence s ('a');
        // This must be possible without throwing.
        optional_sequence o_e (e);
        optional_sequence o_s ('b');

        sequence_annihilator a;

        // It is impossible to disable conversions from, say, empty_sequence
        // to single_sequence, since the compiler will convert to sequence, and
        // then call the explicit constructor anyway.
        // But these transparently wrong constructors should throw!

        // From empty_sequence.
        BOOST_MPL_ASSERT ((
            std::is_convertible <empty_sequence, empty_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <empty_sequence, empty_sequence>));

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <empty_sequence, single_sequence>));
        BOOST_CHECK_THROW (
            single_sequence s (e), math::magma_not_convertible);

        BOOST_MPL_ASSERT ((
            std::is_convertible <empty_sequence, optional_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <optional_sequence, empty_sequence>));

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <empty_sequence, sequence_annihilator>));
        BOOST_CHECK_THROW (
            sequence_annihilator a2 (e), math::magma_not_convertible);

        BOOST_MPL_ASSERT ((
            std::is_convertible <empty_sequence, sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <sequence, empty_sequence>));

        // From single_sequence.
        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <single_sequence, empty_sequence>));
        BOOST_CHECK_THROW (empty_sequence e2 (s), math::magma_not_convertible);

        BOOST_MPL_ASSERT ((
            std::is_convertible <single_sequence, single_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <single_sequence, single_sequence>));

        BOOST_MPL_ASSERT ((
            std::is_convertible <single_sequence, optional_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <optional_sequence, single_sequence>));

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <single_sequence, sequence_annihilator>));
        BOOST_CHECK_THROW (
            sequence_annihilator e2 (s), math::magma_not_convertible);

        BOOST_MPL_ASSERT ((std::is_convertible <single_sequence, sequence>));
        BOOST_MPL_ASSERT ((std::is_constructible <sequence, single_sequence>));

        // From optional_sequence.
        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <optional_sequence, empty_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <optional_sequence, empty_sequence>));
        {
            empty_sequence e2 (o_e);
            BOOST_CHECK_THROW (
                empty_sequence e3 (o_s), math::magma_not_convertible);
        }

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <optional_sequence, single_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <single_sequence, optional_sequence>));
        {
            single_sequence s2 (o_s);
            BOOST_CHECK_THROW (
                single_sequence s3 (o_e), math::magma_not_convertible);
        }

        BOOST_MPL_ASSERT ((
            std::is_convertible <optional_sequence, optional_sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <optional_sequence, optional_sequence>));

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <optional_sequence, sequence_annihilator>));
        BOOST_CHECK_THROW (
            sequence_annihilator e2 (s), math::magma_not_convertible);

        BOOST_MPL_ASSERT ((std::is_convertible <optional_sequence, sequence>));
        BOOST_MPL_ASSERT ((std::is_constructible <
            sequence, optional_sequence>));
        {
            sequence s_o_e (o_e);
            sequence s_o_s (o_s);
        }

        // From sequence_annihilator.
        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <sequence_annihilator, empty_sequence>));
        BOOST_CHECK_THROW (empty_sequence s (a), math::magma_not_convertible);

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <sequence_annihilator, single_sequence>));
        BOOST_CHECK_THROW (
            single_sequence s (a), math::magma_not_convertible);

        BOOST_MPL_ASSERT ((
            std::is_convertible <sequence_annihilator, sequence_annihilator>));
        BOOST_MPL_ASSERT ((std::is_constructible <
            sequence_annihilator, sequence_annihilator>));

        BOOST_MPL_ASSERT ((
            std::is_convertible <sequence_annihilator, sequence>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <sequence, sequence_annihilator>));

        sequence s_e (e);
        sequence s_s (s);
        sequence s_a (a);
        sequence s_m (std::string ("abc"));

        // Converting from a sequence into anything else: only possible if the
        // sequence contains the right number of elements.
        BOOST_MPL_ASSERT_NOT ((std::is_convertible <sequence, empty_sequence>));
        BOOST_MPL_ASSERT ((std::is_constructible <empty_sequence, sequence>));
        {
            empty_sequence e1 (s_e);
            BOOST_CHECK_THROW (
                empty_sequence e2 (s_s), math::magma_not_convertible);
            BOOST_CHECK_THROW (
                empty_sequence e3 (s_a), math::magma_not_convertible);
            BOOST_CHECK_THROW (
                empty_sequence e4 (s_m), math::magma_not_convertible);
        }

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <sequence, single_sequence>));
        BOOST_MPL_ASSERT ((std::is_constructible <single_sequence, sequence>));
        {
            BOOST_CHECK_THROW (
                single_sequence e1 (s_e), math::magma_not_convertible);
            single_sequence e2 (s_s);
            BOOST_CHECK_EQUAL (e2.symbol(), 'a');
            BOOST_CHECK_THROW (
                single_sequence e3 (s_a), math::magma_not_convertible);
            BOOST_CHECK_THROW (
                single_sequence e4 (s_m), math::magma_not_convertible);
        }

        BOOST_MPL_ASSERT_NOT ((
            std::is_convertible <sequence, sequence_annihilator>));
        BOOST_MPL_ASSERT ((
            std::is_constructible <sequence_annihilator, sequence>));
        {
            BOOST_CHECK_THROW (
                sequence_annihilator e1 (s_e), math::magma_not_convertible);
            BOOST_CHECK_THROW (
                sequence_annihilator e2 (s_s), math::magma_not_convertible);
            sequence_annihilator e3 (s_a);
            BOOST_CHECK_THROW (
                sequence_annihilator e4 (s_m), math::magma_not_convertible);
        }

        BOOST_MPL_ASSERT ((std::is_convertible <sequence, sequence>));
        BOOST_MPL_ASSERT ((std::is_constructible <sequence, sequence>));
        {
            sequence s1 (s_e);
            sequence s2 (s_s);
            sequence s3 (s_a);
            sequence s4 (s_m);
        }
    }

    BOOST_MPL_ASSERT_NOT ((
        math::has <math::callable::non_member <sequence>()>));
    BOOST_MPL_ASSERT ((math::has <math::callable::is_member (sequence)>));

    BOOST_MPL_ASSERT ((math::has <math::callable::equal (sequence, sequence)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::approximately_equal (sequence, sequence)>));

    // Properties of times.
    BOOST_MPL_ASSERT ((math::has <math::callable::times (sequence, sequence)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        math::callable::times (sequence, sequence)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <sequence, math::callable::times>()>));
    BOOST_CHECK (range::empty (math::one <sequence>().symbols()));
    // Annihilator.
    BOOST_MPL_ASSERT ((math::has <
        math::callable::annihilator <sequence, math::callable::times>()>));
    BOOST_CHECK ((math::annihilator <sequence, math::callable::times>()
        .is_annihilator()));

    BOOST_MPL_ASSERT ((
        math::is::associative <math::callable::times, sequence>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::commutative <math::callable::times, sequence>));

    BOOST_MPL_ASSERT ((math::has <
        math::callable::reverse<> (math::callable::times, sequence)>));

    // Properties of plus.
    BOOST_MPL_ASSERT ((math::has <math::callable::plus (sequence, sequence)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        math::callable::plus (sequence, sequence)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <sequence, math::callable::plus>()>));
    BOOST_MPL_ASSERT ((math::has <math::callable::zero <sequence>()>));
    BOOST_CHECK (math::zero <sequence>().is_annihilator());
    BOOST_MPL_ASSERT_NOT ((math::has <
        math::callable::annihilator <sequence, math::callable::plus>()>));

    BOOST_MPL_ASSERT ((
        math::is::associative <math::callable::plus, sequence>));
    BOOST_MPL_ASSERT ((
        math::is::commutative <math::callable::plus, sequence>));

    BOOST_MPL_ASSERT ((math::is::idempotent <math::callable::plus, sequence>));
    BOOST_MPL_ASSERT_NOT ((
        math::is::path_operation <math::callable::plus, sequence>));

    // Properties of choose.
    BOOST_MPL_ASSERT ((math::has <math::callable::choose (sequence, sequence)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        math::callable::choose (sequence, sequence)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::identity <sequence, math::callable::choose>()>));
    BOOST_CHECK (math::identity <sequence> (math::choose).is_annihilator());
    BOOST_MPL_ASSERT_NOT ((math::has <
        math::callable::annihilator <sequence, math::callable::choose>()>));

    BOOST_MPL_ASSERT ((
        math::is::associative <math::callable::choose, sequence>));
    BOOST_MPL_ASSERT ((
        math::is::commutative <math::callable::choose, sequence>));

    BOOST_MPL_ASSERT ((math::is::idempotent <math::callable::choose, sequence>));
    BOOST_MPL_ASSERT ((
        math::is::path_operation <math::callable::choose, sequence>));

    // Properties of divide.
    BOOST_MPL_ASSERT_NOT ((math::has <
        math::callable::divide<> (sequence, sequence)>));
    BOOST_MPL_ASSERT ((math::has <
        math::callable::divide <Direction> (sequence, sequence)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <
        math::callable::divide <Direction> (sequence, sequence)>));
    BOOST_MPL_ASSERT_NOT ((math::has <
        math::callable::divide <Opposite> (sequence, sequence)>));
    BOOST_MPL_ASSERT ((math::is::throw_if_undefined <
        math::callable::divide <Direction> (sequence, sequence)>));

    // Distributivity.
    BOOST_MPL_ASSERT_NOT ((math::is::distributive <math::either,
        math::callable::times, math::callable::plus, sequence>));
    BOOST_MPL_ASSERT ((math::is::distributive <Direction,
        math::callable::times, math::callable::plus, sequence>));
    BOOST_MPL_ASSERT_NOT ((math::is::distributive <Opposite,
        math::callable::times, math::callable::plus, sequence>));
}

#endif // MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED
