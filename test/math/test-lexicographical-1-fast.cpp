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
Test lexicographical.hpp.
*/

#define BOOST_TEST_MODULE test_math_lexicographical_fast
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <string>
#include <vector>

#include <boost/mpl/assert.hpp>

#include "math/sequence.hpp"
#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"

#include "./make_lexicographical.hpp"

using range::first;
using range::second;

BOOST_AUTO_TEST_SUITE (test_suite_lexicographical)

BOOST_AUTO_TEST_CASE (test_lexicographical_static) {
    // Basic requirements for the component types.
    BOOST_MPL_ASSERT ((math::is::monoid <math::callable::choose, cost>));
    BOOST_MPL_ASSERT ((math::is::monoid <math::callable::choose,
        math::sequence <char>>));

    BOOST_MPL_ASSERT ((math::is::semiring <
        math::left, math::callable::times, math::callable::plus,
        lexicographical>));

    BOOST_MPL_ASSERT ((math::is::distributive <
        math::left, math::callable::times, math::callable::plus,
        lexicographical>));
    BOOST_MPL_ASSERT_NOT ((math::is::distributive <
        math::right, math::callable::times, math::callable::plus,
        lexicographical>));
    BOOST_MPL_ASSERT_NOT ((math::is::distributive <
        math::either, math::callable::times, math::callable::plus,
        lexicographical>));

    BOOST_MPL_ASSERT ((math::is::distributive <
        math::left, math::callable::times, math::callable::choose,
        lexicographical>));
    BOOST_MPL_ASSERT ((math::is::distributive <
        math::left, math::callable::times, math::callable::choose, cost>));
    BOOST_MPL_ASSERT ((math::is::distributive <
        math::left, math::callable::times, math::callable::choose,
        math::sequence <char>>));

    BOOST_MPL_ASSERT ((math::is::distributive <
        math::left, math::callable::times, math::callable::choose,
        lexicographical>));

    {
        BOOST_MPL_ASSERT ((math::is::associative <math::callable::times,
            math::lexicographical <math::over <math::cost <float>>>>));
        BOOST_MPL_ASSERT ((math::is::associative <math::callable::times,
            math::lexicographical <math::over <math::sequence <char>>>>));
    }
    {
        BOOST_MPL_ASSERT ((math::is::commutative <math::callable::times,
            math::lexicographical <math::over <math::cost <float>>>>));
        BOOST_MPL_ASSERT_NOT ((math::is::commutative <math::callable::times,
            math::lexicographical <math::over <math::sequence <char>>>>));
    }
    typedef math::lexicographical <math::over <math::cost <float>>>
        cost;
    typedef math::lexicographical <math::over <math::sequence <char>>>
        sequence;
    {
        BOOST_MPL_ASSERT ((math::is::approximate <
            math::callable::times (cost, cost)>));
        BOOST_MPL_ASSERT_NOT ((math::is::approximate <
            math::callable::times (sequence, sequence)>));
        BOOST_MPL_ASSERT_NOT ((math::is::approximate <
            math::callable::plus (cost, cost)>));
        BOOST_MPL_ASSERT_NOT ((math::is::approximate <
            math::callable::plus (sequence, sequence)>));
    }
    {
        BOOST_MPL_ASSERT_NOT ((math::is::idempotent <
            math::callable::times (cost, cost)>));
        BOOST_MPL_ASSERT_NOT ((math::is::idempotent <
            math::callable::times (sequence, sequence)>));
        BOOST_MPL_ASSERT ((math::is::idempotent <
            math::callable::plus (cost, cost)>));
        BOOST_MPL_ASSERT ((math::is::idempotent <
            math::callable::plus (sequence, sequence)>));
    }
    {
        BOOST_MPL_ASSERT_NOT ((math::is::path_operation <
            math::callable::times (cost, cost)>));
        BOOST_MPL_ASSERT_NOT ((math::is::path_operation <
            math::callable::times (sequence, sequence)>));
        BOOST_MPL_ASSERT ((math::is::path_operation <
            math::callable::plus (cost, cost)>));
        BOOST_MPL_ASSERT ((math::is::path_operation <
            math::callable::plus (sequence, sequence)>));
    }
}

BOOST_AUTO_TEST_CASE (test_lexicographical_spot) {
    std::string ab = "ab";
    std::string abc = "abc";
    std::string abd = "abd";
    std::string c = "c";
    std::string d = "d";

    lexicographical ab4 = make_lexicographical (4, ab);
    BOOST_CHECK_EQUAL (first (ab4.components()).value(), 4.f);
    BOOST_CHECK_EQUAL (first (second (ab4.components()).symbols()), 'a');
    BOOST_CHECK_EQUAL (second (second (ab4.components()).symbols()), 'b');

    lexicographical c7 = make_lexicographical (7, c);
    lexicographical abc11 = make_lexicographical (11, abc);

    {
        auto ab4_2 = math::make_lexicographical (
            math::cost <float> (4), math::sequence <char> (std::string ("ab")));
        static_assert (
            std::is_same <decltype (ab4_2), lexicographical>::value, "");

        BOOST_CHECK (ab4_2 == ab4);
    }
    {
        auto components = range::make_tuple (
            math::cost <float> (4), math::sequence <char> (std::string ("ab")));
        auto ab4_2 = math::make_lexicographical_over (components);
        static_assert (
            std::is_same <decltype (ab4_2), lexicographical>::value, "");

        BOOST_CHECK (ab4_2 == ab4);
    }

    BOOST_CHECK_EQUAL (ab4 * c7, abc11);
    BOOST_CHECK_EQUAL (ab4 * math::one <lexicographical>(), ab4);
    // divide is not implemented.
    // BOOST_CHECK_EQUAL (math::divide <math::left> (abc11, ab4), c7);

    BOOST_CHECK_EQUAL (ab4 + abc11, ab4);
    BOOST_CHECK_EQUAL (ab4 + math::zero <lexicographical>(), ab4);

    BOOST_CHECK (math::equal (
        make_empty_lexicographical (0), make_lexicographical (0, "")));
    BOOST_CHECK (math::equal (
        make_empty_lexicographical (7), make_lexicographical (7, "")));
    BOOST_CHECK (!math::equal (
        make_empty_lexicographical (7), make_lexicographical (0, "")));
    BOOST_CHECK (!math::equal (
        make_empty_lexicographical (7), make_lexicographical (6, "")));

    BOOST_CHECK (math::equal (
        make_single_lexicographical (0, 'a'),
        make_lexicographical (0, "a")));
    BOOST_CHECK (math::equal (
        make_single_lexicographical (7, 'b'),
        make_lexicographical (7, "b")));
    BOOST_CHECK (!math::equal (
        make_single_lexicographical (7, 'a'),
        make_lexicographical (6, "a")));
    BOOST_CHECK (!math::equal (
        make_single_lexicographical (7, 'a'),
        make_lexicographical (7, "b")));
}

BOOST_AUTO_TEST_SUITE_END()
