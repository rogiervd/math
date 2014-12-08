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
Tests for math::product.
*/

#ifndef MATH_TEST_MATH_PRODUCT_TESTS_HPP_INCLUDED
#define MATH_TEST_MATH_PRODUCT_TESTS_HPP_INCLUDED

#include "math/product.hpp"

#include <string>
#include <vector>

#include <boost/mpl/assert.hpp>

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"
#include "math/check/check_magma.hpp"

template <class Inverses> void test_product_homogeneous() {
    // Test the empty product.
    // This is (trivially!) almost a semiring.
    // However, it cannot have an annihilator.
    {
        typedef math::product <math::over<>, Inverses> product;
        std::vector <product> examples;
        examples.push_back (product());

        static_assert (!math::has <
                math::callable::annihilator <product> (math::callable::times)
            >::value, "An empty product cannot have an annihilator.");

        math::check_magma <product> (math::times, math::plus, examples);
    }

    // Test homogeneous of (float, sequence).
    {
        typedef math::product <
            math::over <float, math::sequence <char>>, Inverses> product;

        static_assert (math::has <
                math::callable::annihilator <product> (math::callable::times)
            >::value, "An empty product cannot have an annihilator.");

        static_assert (utility::is_assignable <
            typename product::components_type &,
            typename product::components_type &&>::value, "");
        static_assert (utility::is_assignable <
            product &, product &&>::value, "");

        std::vector <product> examples;
        examples.push_back (product (
            0, math::sequence <char> (std::string (""))));
        // This is the same for with_inverse <times>.
        examples.push_back (product (
            0, math::sequence <char> (std::string ("q"))));
        examples.push_back (product (
            4, math::sequence <char> (std::string (""))));
        examples.push_back (product (
            2, math::sequence <char> (std::string ("a"))));
        examples.push_back (product (
            5, math::sequence <char> (std::string ("ab"))));
        examples.push_back (product (
            7, math::sequence <char> (std::string ("cba"))));
        examples.push_back (product (
            3, math::sequence <char> (std::string ("aba"))));
        examples.push_back (product (
            0, math::sequence_annihilator <char> ()));
        examples.push_back (product (
            3, math::sequence_annihilator <char> ()));

        math::check_semiring <product, math::left> (
            math::times, math::plus, examples);
    }
}

template <class Inverses, class Sequence>
    inline math::product <math::over <float, Sequence>, Inverses>
        make_product (float f, Sequence s)
{ return math::product <math::over <float, Sequence>, Inverses> (f, s); }

// Test heterogeneous types.
template <class Inverses> void test_product_heterogeneous() {
    typedef math::product <math::over <float, math::sequence <char>>, Inverses>
        product;
    {
        auto examples = range::make_tuple (
            make_product <Inverses> (2.f, math::empty_sequence <char>()),
            make_product <Inverses> (4.f, math::empty_sequence <char>()),
            make_product <Inverses> (4.f,
                math::sequence <char> (std::string())),
            make_product <Inverses> (5.f, math::single_sequence <char> ('A')),
            make_product <Inverses> (5.f, math::single_sequence <char> ('z')),
            make_product <Inverses> (5.f,
                math::sequence <char> (std::string("z"))),
            make_product <Inverses> (1.f,
                math::sequence <char> (std::string("Az"))),
            make_product <Inverses> (1.f,
                math::sequence <char> (std::string("zAz")))
            );

        math::check_semiring <product, math::left> (
            math::times, math::plus, examples);
    }
}

#endif // MATH_TEST_MATH_PRODUCT_TESTS_HPP_INCLUDED
