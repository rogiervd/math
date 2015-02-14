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
Test max_semiring.hpp.
*/

#define BOOST_TEST_MODULE test_max_semiring
#include "utility/test/boost_unit_test.hpp"

#include "math/max_semiring.hpp"

#include <boost/mpl/assert.hpp>

#include "range/std/container.hpp"

#include "math/check/check_magma.hpp"

#include "./check_hash.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_max_semiring)

template <class Type> void check_max_semiring_for() {
    typedef math::max_semiring <Type> semiring;

    {
        semiring five (5);
        semiring three (3);
        semiring zero (0);
        semiring zero2;

        BOOST_CHECK (zero == zero2);

        BOOST_CHECK_EQUAL ((five * three).value(), 15);
        BOOST_CHECK_EQUAL ((five + three), five);
        BOOST_CHECK_EQUAL (math::choose (five, three), five);

        BOOST_CHECK (three < five);
        BOOST_CHECK (zero < five);
        BOOST_CHECK (zero < three);
    }

    static_assert (math::has <math::callable::times (
        semiring, semiring)>::value, "");
    static_assert (math::has <math::callable::plus (
        semiring, semiring)>::value, "");
    static_assert (math::has <math::callable::choose (
        semiring, semiring)>::value, "");

    static_assert (math::is::associative <math::callable::times (
        semiring, semiring)>::value, "");
    static_assert (math::is::commutative <math::callable::times (
        semiring, semiring)>::value, "");
    static_assert (!math::is::idempotent <math::callable::times (
        semiring, semiring)>::value, "");

    static_assert (math::is::path_operation <math::callable::plus (
        semiring, semiring)>::value, "");
    static_assert (math::is::path_operation <math::callable::choose (
        semiring, semiring)>::value, "");

    std::vector <semiring> examples;
    examples.push_back (semiring (0));
    examples.push_back (semiring (3));
    examples.push_back (semiring (5));
    examples.push_back (semiring (17));

    BOOST_CHECK (semiring (3) == semiring (3));

    math::check_equal_on (examples);
    math::check_hash (examples);

    math::check_semiring <semiring, math::either> (
        math::times, math::plus, examples);
    math::check_semiring <semiring, math::either> (
        math::times, math::choose, examples);
}

BOOST_AUTO_TEST_CASE (test_max_semiring_complete) {
    check_max_semiring_for <int>();
    check_max_semiring_for <float>();
    check_max_semiring_for <double>();
}

// Test whether floating-point numbers and integers are treated correctly when
// they should behave differently.
BOOST_AUTO_TEST_CASE (test_max_semiring_float) {
    // non_member, divide, and invert available for floating-point numbers.
    static_assert (math::has <
        math::callable::non_member <math::max_semiring <double>>()>::value, "");
    static_assert (math::has <
        math::callable::invert <math::callable::times> (
            math::max_semiring <double>)>::value, "");
    static_assert (math::has <math::callable::divide<> (
            math::max_semiring <double>, math::max_semiring <double>)>::value,
        "");

    // times and divide are approximate for floating-point numbers; plus is not.
    static_assert (!math::is::approximate <math::callable::plus (
        math::max_semiring <double>, math::max_semiring <double>)>::value, "");
    static_assert (math::is::approximate <math::callable::times (
        math::max_semiring <double>, math::max_semiring <double>)>::value, "");
    static_assert (math::is::approximate <math::callable::divide<> (
        math::max_semiring <double>, math::max_semiring <double>)>::value, "");

    // But not for integers.
    static_assert (!math::has <
        math::callable::non_member <math::max_semiring <int>>()>::value, "");
    static_assert (!math::has <math::callable::invert <math::callable::times> (
            math::max_semiring <int>)>::value, "");
    static_assert (!math::has <math::callable::divide<> (
            math::max_semiring <int>, math::max_semiring <int>)>::value, "");

    static_assert (!math::is::approximate <math::callable::plus (
        math::max_semiring <int>, math::max_semiring <int>)>::value, "");
    static_assert (!math::is::approximate <math::callable::times (
        math::max_semiring <int>, math::max_semiring <int>)>::value, "");
    static_assert (!math::is::approximate <math::callable::divide<> (
        math::max_semiring <int>, math::max_semiring <int>)>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
