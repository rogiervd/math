// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Contain templated helpers for testing math::sequence.
By instantiating these tests once for math::left and once for math::right,
compiling takes a lot less memory and time.
*/

#ifndef MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED
#define MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED

#include "math/sequence.hpp"

#include <string>

#include "boost/mpl/assert.hpp"

#include "range/std/string.hpp"

template <class Direction, class Opposite> void test_sequence_spot()
{
    typedef math::sequence<char, Direction> sequence;
    typedef math::empty_sequence<char, Direction> empty_sequence;
    typedef math::optional_sequence<char, Direction> optional_sequence;
    typedef math::single_sequence<char, Direction> single_sequence;
    typedef math::sequence_annihilator<char, Direction> sequence_annihilator;

    typedef math::sequence<char, Opposite> opposite_sequence;

    // Conversion.
    {
        static_assert(!std::is_convertible_v<sequence, opposite_sequence>);

        empty_sequence e;
        single_sequence s('a');
        // This must be possible without throwing.
        optional_sequence o_e(e);
        optional_sequence o_s('b');

        sequence_annihilator a;

        // It is impossible to disable conversions from, say, empty_sequence
        // to single_sequence, since the compiler will convert to sequence, and
        // then call the explicit constructor anyway.
        // But these transparently wrong constructors should throw!

        // From empty_sequence.
        static_assert(std::is_convertible_v<empty_sequence, empty_sequence>);
        static_assert(
            std::is_constructible<empty_sequence, empty_sequence>::value);

        static_assert(!std::is_convertible_v<empty_sequence, single_sequence>);
        BOOST_CHECK_THROW(single_sequence s(e), math::magma_not_convertible);

        static_assert(std::is_convertible_v<empty_sequence, optional_sequence>);
        static_assert(
            std::is_constructible<optional_sequence, empty_sequence>::value);

        static_assert(
            !std::is_convertible_v<empty_sequence, sequence_annihilator>);
        BOOST_CHECK_THROW(
            sequence_annihilator a2(e), math::magma_not_convertible);

        static_assert(std::is_convertible_v<empty_sequence, sequence>);
        static_assert(std::is_constructible<sequence, empty_sequence>::value);

        // From single_sequence.
        static_assert(!std::is_convertible_v<single_sequence, empty_sequence>);
        BOOST_CHECK_THROW(empty_sequence e2(s), math::magma_not_convertible);

        static_assert(std::is_convertible_v<single_sequence, single_sequence>);
        static_assert(
            std::is_constructible<single_sequence, single_sequence>::value);

        static_assert(
            std::is_convertible_v<single_sequence, optional_sequence>);
        static_assert(
            std::is_constructible<optional_sequence, single_sequence>::value);

        static_assert(
            !std::is_convertible_v<single_sequence, sequence_annihilator>);
        BOOST_CHECK_THROW(
            sequence_annihilator e2(s), math::magma_not_convertible);

        static_assert(std::is_convertible_v<single_sequence, sequence>);
        static_assert(std::is_constructible<sequence, single_sequence>::value);

        // From optional_sequence.
        static_assert(
            !std::is_convertible_v<optional_sequence, empty_sequence>);
        static_assert(
            std::is_constructible<optional_sequence, empty_sequence>::value);
        {
            empty_sequence e2(o_e);
            BOOST_CHECK_THROW(
                empty_sequence e3(o_s), math::magma_not_convertible);
        }

        static_assert(
            !std::is_convertible_v<optional_sequence, single_sequence>);
        static_assert(
            std::is_constructible<single_sequence, optional_sequence>::value);
        {
            single_sequence s2(o_s);
            BOOST_CHECK_THROW(
                single_sequence s3(o_e), math::magma_not_convertible);
        }

        static_assert(
            std::is_convertible_v<optional_sequence, optional_sequence>);
        static_assert(
            std::is_constructible<optional_sequence, optional_sequence>::value);

        static_assert(
            !std::is_convertible_v<optional_sequence, sequence_annihilator>);
        BOOST_CHECK_THROW(
            sequence_annihilator e2(s), math::magma_not_convertible);

        static_assert(std::is_convertible_v<optional_sequence, sequence>);
        static_assert(
            std::is_constructible<sequence, optional_sequence>::value);
        {
            sequence s_o_e(o_e);
            sequence s_o_s(o_s);
        }

        // From sequence_annihilator.
        static_assert(
            !std::is_convertible_v<sequence_annihilator, empty_sequence>);
        BOOST_CHECK_THROW(empty_sequence s(a), math::magma_not_convertible);

        static_assert(
            !std::is_convertible_v<sequence_annihilator, single_sequence>);
        BOOST_CHECK_THROW(single_sequence s(a), math::magma_not_convertible);

        static_assert(
            std::is_convertible_v<sequence_annihilator, sequence_annihilator>);
        static_assert(std::is_constructible<
                      sequence_annihilator, sequence_annihilator>::value);

        static_assert(std::is_convertible_v<sequence_annihilator, sequence>);
        static_assert(
            std::is_constructible<sequence, sequence_annihilator>::value);

        sequence s_e(e);
        sequence s_s(s);
        sequence s_a(a);
        sequence s_m(std::string("abc"));

        // Converting from a sequence into anything else: only possible if the
        // sequence contains the right number of elements.
        static_assert(!std::is_convertible_v<sequence, empty_sequence>);
        static_assert(std::is_constructible<empty_sequence, sequence>::value);
        {
            empty_sequence e1(s_e);
            BOOST_CHECK_THROW(
                empty_sequence e2(s_s), math::magma_not_convertible);
            BOOST_CHECK_THROW(
                empty_sequence e3(s_a), math::magma_not_convertible);
            BOOST_CHECK_THROW(
                empty_sequence e4(s_m), math::magma_not_convertible);
        }

        static_assert(!std::is_convertible_v<sequence, single_sequence>);
        static_assert(std::is_constructible<single_sequence, sequence>::value);
        {
            BOOST_CHECK_THROW(
                single_sequence e1(s_e), math::magma_not_convertible);
            single_sequence e2(s_s);
            BOOST_CHECK_EQUAL(e2.symbol(), 'a');
            BOOST_CHECK_THROW(
                single_sequence e3(s_a), math::magma_not_convertible);
            BOOST_CHECK_THROW(
                single_sequence e4(s_m), math::magma_not_convertible);
        }

        static_assert(!std::is_convertible_v<sequence, sequence_annihilator>);
        static_assert(
            std::is_constructible<sequence_annihilator, sequence>::value);
        {
            BOOST_CHECK_THROW(
                sequence_annihilator e1(s_e), math::magma_not_convertible);
            BOOST_CHECK_THROW(
                sequence_annihilator e2(s_s), math::magma_not_convertible);
            sequence_annihilator e3(s_a);
            BOOST_CHECK_THROW(
                sequence_annihilator e4(s_m), math::magma_not_convertible);
        }

        static_assert(std::is_convertible_v<sequence, sequence>);
        static_assert(std::is_constructible<sequence, sequence>::value);
        {
            sequence s1(s_e);
            sequence s2(s_s);
            sequence s3(s_a);
            sequence s4(s_m);
        }
    }

    static_assert(!math::has<math::callable::non_member<sequence>()>::value);
    static_assert(math::has<math::callable::is_member(sequence)>::value);

    static_assert(math::has<math::callable::equal(sequence, sequence)>::value);
    static_assert(math::has<math::callable::approximately_equal(
                      sequence, sequence)>::value);

    // Properties of times.
    static_assert(math::has<math::callable::times(sequence, sequence)>::value);
    static_assert(!math::is::approximate<math::callable::times(
                      sequence, sequence)>::value);
    static_assert(
        math::has<math::callable::identity<sequence, math::callable::times>()>::
            value);
    BOOST_CHECK(range::empty(math::one<sequence>().symbols()));
    // Annihilator.
    static_assert(math::has<math::callable::annihilator<
                      sequence, math::callable::times>()>::value);
    BOOST_CHECK((
        math::annihilator<sequence, math::callable::times>().is_annihilator()));

    static_assert(
        math::is::associative<math::callable::times, sequence>::value);
    static_assert(
        !math::is::commutative<math::callable::times, sequence>::value);

    static_assert(math::has<math::callable::reverse<>(
                      math::callable::times, sequence)>::value);

    // Properties of plus.
    static_assert(math::has<math::callable::plus(sequence, sequence)>::value);
    static_assert(!math::is::approximate<math::callable::plus(
                      sequence, sequence)>::value);
    static_assert(
        math::has<
            math::callable::identity<sequence, math::callable::plus>()>::value);
    static_assert(math::has<math::callable::zero<sequence>()>::value);
    BOOST_CHECK(math::zero<sequence>().is_annihilator());
    static_assert(!math::has<math::callable::annihilator<
                      sequence, math::callable::plus>()>::value);

    static_assert(math::is::associative<math::callable::plus, sequence>::value);
    static_assert(math::is::commutative<math::callable::plus, sequence>::value);

    static_assert(math::is::idempotent<math::callable::plus, sequence>::value);
    static_assert(
        !math::is::path_operation<math::callable::plus, sequence>::value);

    // Properties of choose.
    static_assert(math::has<math::callable::choose(sequence, sequence)>::value);
    static_assert(!math::is::approximate<math::callable::choose(
                      sequence, sequence)>::value);
    static_assert(math::has<math::callable::identity<
                      sequence, math::callable::choose>()>::value);
    BOOST_CHECK(math::identity<sequence>(math::choose).is_annihilator());
    static_assert(!math::has<math::callable::annihilator<
                      sequence, math::callable::choose>()>::value);

    static_assert(
        math::is::associative<math::callable::choose, sequence>::value);
    static_assert(
        math::is::commutative<math::callable::choose, sequence>::value);

    static_assert(
        math::is::idempotent<math::callable::choose, sequence>::value);
    static_assert(
        math::is::path_operation<math::callable::choose, sequence>::value);

    // Properties of divide.
    static_assert(
        !math::has<math::callable::divide<>(sequence, sequence)>::value);
    static_assert(math::has<math::callable::divide<Direction>(
                      sequence, sequence)>::value);
    static_assert(!math::is::approximate<math::callable::divide<Direction>(
                      sequence, sequence)>::value);
    static_assert(!math::has<math::callable::divide<Opposite>(
                      sequence, sequence)>::value);
    static_assert(
        math::is::throw_if_undefined<math::callable::divide<Direction>(
            sequence, sequence)>::value);

    // Distributivity with plus: only in one direction.
    static_assert(!math::is::distributive<
                  math::either, math::callable::times, math::callable::plus,
                  sequence>::value);
    static_assert(math::is::distributive<
                  Direction, math::callable::times, math::callable::plus,
                  sequence>::value);
    static_assert(!math::is::distributive<
                  Opposite, math::callable::times, math::callable::plus,
                  sequence>::value);

    // Distributivity with choose: in either direction.
    static_assert(math::is::distributive<
                  math::either, math::callable::times, math::callable::choose,
                  sequence>::value);
    static_assert(math::is::distributive<
                  Direction, math::callable::times, math::callable::choose,
                  sequence>::value);
    static_assert(math::is::distributive<
                  Opposite, math::callable::times, math::callable::choose,
                  sequence>::value);

    // Semiring with plus: only in one direction.
    static_assert(!math::is::semiring<
                  math::either, math::callable::times, math::callable::plus,
                  sequence>::value);
    static_assert(math::is::semiring<
                  Direction, math::callable::times, math::callable::plus,
                  sequence>::value);
    static_assert(!math::is::semiring<
                  Opposite, math::callable::times, math::callable::plus,
                  sequence>::value);

    // Semiring with choose: in either direction.
    static_assert(math::is::semiring<
                  math::either, math::callable::times, math::callable::choose,
                  sequence>::value);
    static_assert(math::is::semiring<
                  Direction, math::callable::times, math::callable::choose,
                  sequence>::value);
    static_assert(math::is::semiring<
                  Opposite, math::callable::times, math::callable::choose,
                  sequence>::value);
}

#endif  // MATH_TEST_MATH_TEST_SEQUENCE_TESTS_FAST_HPP_INCLUDED
