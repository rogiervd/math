/*
Copyright 2014 Rogier van Dalen.

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
Test product.hpp.
*/

#define BOOST_TEST_MODULE test_math_product
#include "utility/test/boost_unit_test.hpp"

#include "math/product.hpp"

#include <string>
#include <vector>

#include <boost/mpl/assert.hpp>

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"
#include "math/check/check_magma.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_product)

BOOST_AUTO_TEST_CASE (test_product_annihilator) {
    typedef math::product <
        math::over <math::sequence <char>, math::cost <double>>>
        without_inverse;
    typedef math::product <
        math::over <math::sequence <char>, math::cost <double>>,
        math::with_inverse <math::callable::times>> with_divide;
    typedef math::product <
        math::over <math::sequence <char>, math::cost <double>>,
        math::with_inverse <math::callable::plus>> with_minus;

    auto sequence_one = math::one <math::sequence <char>>();
    auto sequence_annihilator =
        math::annihilator <math::sequence <char>> (math::times);
    auto cost_one = math::one <math::cost <double>> ();
    auto cost_annihilator =
        math::annihilator <math::cost <double>> (math::times);

    // divide.
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::divide <math::left> (
        without_inverse, without_inverse)>));
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::invert <
        math::left, math::callable::times> (without_inverse)>));

    BOOST_MPL_ASSERT ((math::has <math::callable::divide <math::left> (
        with_divide, with_divide)>));
    // sequence does not have invert.
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::invert <
        math::left, math::callable::times> (with_divide)>));

    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::divide <math::left> (
        with_minus, with_minus)>));
    BOOST_MPL_ASSERT_NOT ((math::has <
        math::callable::invert <math::left, math::callable::times> (
            with_minus)>));

    // minus.
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::minus <math::left> (
        without_inverse, without_inverse)>));
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::minus <math::left> (
        with_divide, with_divide)>));
    // minus is not actually implemented, since sequence does not have it.
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::minus <math::left> (
        with_minus, with_minus)>));

    // All components annihilators.
    BOOST_CHECK (math::is_annihilator (math::times,
        without_inverse (sequence_annihilator, cost_annihilator)));
    BOOST_CHECK (math::is_annihilator (math::times,
        with_divide (sequence_annihilator, cost_annihilator)));
    BOOST_CHECK (math::is_annihilator (math::times,
        with_minus (sequence_annihilator, cost_annihilator)));

    // One component annihilator.
    BOOST_CHECK (!math::is_annihilator (math::times,
        without_inverse (sequence_one, cost_annihilator)));
    BOOST_CHECK (math::is_annihilator (math::times,
        with_divide (sequence_one, cost_annihilator)));
    BOOST_CHECK (!math::is_annihilator (math::times,
        with_minus (sequence_one, cost_annihilator)));

    BOOST_CHECK (!math::is_annihilator (math::times,
        without_inverse (sequence_annihilator, cost_one)));
    BOOST_CHECK (math::is_annihilator (math::times,
        with_divide (sequence_annihilator, cost_one)));
    BOOST_CHECK (!math::is_annihilator (math::times,
        with_minus (sequence_annihilator, cost_one)));

    // No components annihilators.
    BOOST_CHECK (!math::is_annihilator (math::times,
        without_inverse (sequence_one, cost_one)));
    BOOST_CHECK (!math::is_annihilator (math::times,
        with_divide (sequence_one, cost_one)));
    BOOST_CHECK (!math::is_annihilator (math::times,
        with_minus (sequence_one, cost_one)));

    // Annihilators compare equal.
    BOOST_CHECK_EQUAL (
        without_inverse (sequence_annihilator, cost_annihilator),
        without_inverse (sequence_annihilator, cost_annihilator));
    BOOST_CHECK (!math::compare (
        without_inverse (sequence_annihilator, cost_annihilator),
        without_inverse (sequence_annihilator, cost_annihilator)));

    BOOST_CHECK_EQUAL (
        with_minus (sequence_annihilator, cost_annihilator),
        with_minus (sequence_annihilator, cost_annihilator));
    BOOST_CHECK (!math::compare (
        with_minus (sequence_annihilator, cost_annihilator),
        with_minus (sequence_annihilator, cost_annihilator)));

    BOOST_CHECK_EQUAL (
        with_divide (sequence_annihilator, cost_annihilator),
        with_divide (sequence_annihilator, cost_one));
    BOOST_CHECK (!math::compare (
        with_divide (sequence_annihilator, cost_annihilator),
        with_divide (sequence_annihilator, cost_one)));

    BOOST_CHECK_EQUAL (
        with_divide (sequence_annihilator, cost_annihilator),
        with_divide (sequence_one, cost_annihilator));
    BOOST_CHECK (!math::compare (
        with_divide (sequence_annihilator, cost_annihilator),
        with_divide (sequence_one, cost_annihilator)));

    BOOST_CHECK_EQUAL (
        with_divide (sequence_annihilator, cost_one),
        with_divide (sequence_one, cost_annihilator));
    BOOST_CHECK (!math::compare (
        with_divide (sequence_annihilator, cost_one),
        with_divide (sequence_one, cost_annihilator)));

    // Annihilators and non-annihilators compare unequal.
    BOOST_CHECK (! math::equal (
        without_inverse (sequence_annihilator, cost_annihilator),
        without_inverse (sequence_annihilator, cost_one)));
    BOOST_CHECK (! math::equal (
        with_minus (sequence_annihilator, cost_annihilator),
        with_minus (sequence_annihilator, cost_one)));

    BOOST_CHECK (! math::equal (
        without_inverse (sequence_annihilator, cost_annihilator),
        without_inverse (sequence_one, cost_annihilator)));
    BOOST_CHECK (! math::equal (
        with_minus (sequence_annihilator, cost_annihilator),
        with_minus (sequence_one, cost_annihilator)));

    BOOST_CHECK (! math::equal (
        without_inverse (sequence_annihilator, cost_one),
        without_inverse (sequence_one, cost_annihilator)));
    BOOST_CHECK (! math::equal (
        with_minus (sequence_annihilator, cost_one),
        with_minus (sequence_one, cost_annihilator)));


    BOOST_CHECK (! math::equal (
        with_divide (sequence_annihilator, cost_one),
        with_divide (sequence_one, cost_one)));

    // compare.
    BOOST_CHECK (math::compare (
        with_divide (sequence_one, cost_one),
        with_divide (sequence_annihilator, cost_one)));
    BOOST_CHECK (!math::compare (
        with_divide (sequence_annihilator, cost_one),
        with_divide (sequence_one, cost_one)));

    BOOST_CHECK (!math::compare (
        without_inverse (sequence_annihilator, cost_annihilator),
        without_inverse (sequence_one, cost_annihilator)));
    BOOST_CHECK (math::compare (
        without_inverse (sequence_one, cost_annihilator),
        without_inverse (sequence_annihilator, cost_annihilator)));
}

BOOST_AUTO_TEST_CASE (test_product_spot_three) {
    typedef math::product <
        math::over <math::sequence <char>, int, math::cost <float>>> product;

    product a_4_4 (
        math::sequence <char> (std::string ("a")), 4, math::cost <float> (4.));
    product bc_2_2 (
        math::sequence <char> (std::string ("bc")), 2, math::cost <float> (2.));

    BOOST_CHECK ((math::not_equal (a_4_4, bc_2_2)));

    product abc_8_6 = a_4_4 * bc_2_2;
    BOOST_CHECK_EQUAL (range::at_c <0> (abc_8_6.components()),
        math::sequence <char> (std::string ("abc")));
    BOOST_CHECK_EQUAL (range::at_c <1> (abc_8_6.components()), 8);
    BOOST_CHECK_EQUAL (
        range::at_c <2> (abc_8_6.components()), math::cost <float> (6.));

    product a_12_4 = abc_8_6 + a_4_4;
    BOOST_CHECK_EQUAL (range::at_c <0> (a_12_4.components()),
        math::sequence <char> (std::string ("a")));
    BOOST_CHECK_EQUAL (range::at_c <1> (a_12_4.components()), 12);
    BOOST_CHECK_EQUAL (
        range::at_c <2> (a_12_4.components()), math::cost <float> (4.));
}

BOOST_AUTO_TEST_CASE (test_product_spot_floats) {
    typedef math::product <math::over <float, float>,
        math::with_inverse <math::callable::times>> product;

    product a (4, 4);
    product inverse = math::invert <math::callable::times> (a);
    BOOST_CHECK_EQUAL (inverse, product (.25, .25));
}

BOOST_AUTO_TEST_CASE (test_product_automaton_determinisation) {
    /*
    This is a simple use case: determinisation of a finite-state automaton.
    This does things like turning (a*b) + (a*c) into a*(b+c).
    Imagine an automaton with two paths:
    (a, 1), (b, 3)
    (a, 2), (c, 4)
    This can be turned into, e.g.
    (a, 1), (b, 3)
    (a, 1), (c, 5)
    and now the first transition is the same and can be shared.
    */

    using math::divide;
    using math::left;
    typedef math::with_inverse <math::callable::times> inverses;

    typedef math::single_sequence <char> symbol;
    typedef math::product <math::over <symbol, math::cost <double>>, inverses>
        product;
    product a_1 (symbol ('a'), 1.);
    product a_2 (symbol ('a'), 2.);
    product b_3 (symbol ('b'), 3.);
    product c_4 (symbol ('c'), 4.);

    auto merged_untyped = a_1 + a_2;
    // Explicitly convert to "product".
    product merged (merged_untyped);
    auto residue1 = divide <left> (a_1, merged);
    auto residue2 = divide <left> (a_2, merged);

    auto follow1 = residue1 * b_3;
    auto follow2 = residue2 * c_4;

    BOOST_CHECK_EQUAL (merged, a_1);
    BOOST_CHECK_EQUAL (follow1, b_3);
    BOOST_CHECK_EQUAL (follow2, product (symbol ('c'), 5.));

    BOOST_CHECK_EQUAL (merged * follow1, a_1 * b_3);
    BOOST_CHECK_EQUAL (merged * follow2, a_2 * c_4);
}

BOOST_AUTO_TEST_SUITE_END()
