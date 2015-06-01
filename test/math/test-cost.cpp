/*
Copyright 2012, 2013 Rogier van Dalen.

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
Test cost.hpp.
*/

#define BOOST_TEST_MODULE test_cost
#include "utility/test/boost_unit_test.hpp"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/tools/test.hpp>

#include "math/cost.hpp"

#include <boost/mpl/assert.hpp>

#include "range/std/container.hpp"

#include "math/check/check_magma.hpp"
#include "math/check/check_hash.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_cost)

typedef math::cost <double> cost;

// Test "choose", or that operation by any other name.
template <class Choose> void test_choose (Choose const & choose) {
    BOOST_MPL_ASSERT ((math::has <Choose (cost, cost)>));
    BOOST_MPL_ASSERT_NOT ((math::is::approximate <Choose (cost, cost)>));
    BOOST_MPL_ASSERT ((math::has <math::callable::identity <cost, Choose>()>));
    BOOST_MPL_ASSERT ((math::has <math::callable::zero <cost>()>));
    BOOST_MPL_ASSERT ((math::is::commutative <Choose, cost>));
    BOOST_MPL_ASSERT ((math::is::associative <Choose, cost>));

    BOOST_MPL_ASSERT ((math::is::path_operation <Choose, cost>));
    BOOST_MPL_ASSERT ((math::is::idempotent <Choose, cost>));

    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::invert <Choose> (cost)>));
    BOOST_MPL_ASSERT_NOT ((math::has <math::callable::invert<>(Choose, cost)>));

    // Distributivity.
    BOOST_MPL_ASSERT ((math::is::distributive <math::either,
        math::callable::times, Choose, cost>));
    BOOST_MPL_ASSERT ((math::is::distributive <math::left,
        math::callable::times, Choose, cost>));
    BOOST_MPL_ASSERT ((math::is::distributive <math::right,
        math::callable::times, Choose, cost>));

    BOOST_CHECK_EQUAL (choose (cost (0), cost (0)).value(), 0.);
    BOOST_CHECK_EQUAL (choose (cost (-1), cost (0)).value(), -1.);
    BOOST_CHECK_EQUAL (choose (cost (0), cost (-1)).value(), -1.);
    BOOST_CHECK_EQUAL (choose (cost (5), cost (-1)).value(), -1.);
    BOOST_CHECK_EQUAL (choose (cost (-1), cost (5)).value(), -1.);
    BOOST_CHECK_EQUAL (choose (cost (2), cost (5)).value(), 2.);
}

BOOST_AUTO_TEST_CASE (test_cost) {
    BOOST_MPL_ASSERT ((math::has <math::callable::non_member <cost>()>));
    BOOST_MPL_ASSERT ((math::has <math::callable::is_member (cost)>));
    BOOST_MPL_ASSERT ((math::has <math::callable::equal (cost, cost)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::approximately_equal (cost, cost)>));

    // "choose" and "plus" are the same.
    test_choose (math::choose);
    test_choose (math::plus);

    // times.
    BOOST_MPL_ASSERT ((math::has <math::callable::times (cost, cost &)>));
    BOOST_MPL_ASSERT ((
        math::is::approximate <math::callable::times (cost, cost &)>));
    BOOST_MPL_ASSERT ((math::is::approximate <
        math::callable::times (math::callable::times (cost, cost &), cost &)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::identity <cost, math::callable::times>()>));
    BOOST_MPL_ASSERT ((math::has <math::callable::one <cost>()>));

    BOOST_MPL_ASSERT ((math::is::commutative <math::callable::times, cost>));
    BOOST_MPL_ASSERT ((math::is::associative <math::callable::times, cost>));

    BOOST_MPL_ASSERT_NOT ((
        math::is::path_operation <math::callable::times, cost>));
    BOOST_MPL_ASSERT_NOT ((math::is::idempotent <math::callable::times, cost>));

    BOOST_MPL_ASSERT ((math::has <math::callable::divide<> (cost, cost)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::invert <math::callable::times> (cost)>));
    BOOST_MPL_ASSERT ((
        math::has <math::callable::reverse <math::callable::times> (cost)>));

    BOOST_CHECK_EQUAL (math::zero <cost>().value(),
        std::numeric_limits <double>::infinity());
    BOOST_CHECK_EQUAL (math::one <cost>().value(), 0.);
    // Initialise as the additive identity, that is, with value "infinity".
    BOOST_CHECK_EQUAL (cost().value(),
        std::numeric_limits <double>::infinity());

    cost a (3.);
    cost b (5.);
    cost c (-2);

    // Multiplication: add costs.
    BOOST_CHECK_EQUAL ((a*b).value(), 8.);
    BOOST_CHECK_EQUAL (math::times (a, b).value(), 8.);
    BOOST_CHECK_EQUAL (math::times (c, b), a);
    // Division: subtract costs.
    BOOST_CHECK_EQUAL ((a/a).value(), 0.);
    BOOST_CHECK_EQUAL (math::divide (a, a).value(), 0.);
    BOOST_CHECK_EQUAL (math::divide (b, c).value(), 7.);
    // Invert element w.r.t. times: negate cost.
    BOOST_CHECK_EQUAL (math::invert <math::callable::times> (a).value(), -3.);
    BOOST_CHECK_EQUAL (math::invert (math::times, b).value(), -5.);
    BOOST_CHECK_EQUAL (math::invert <math::left> (math::times, c).value(), 2.);
    BOOST_CHECK_EQUAL (math::invert <math::right> (math::times, c).value(), 2.);

    // Check for consistency.
    std::vector <cost> examples;
    examples.push_back (cost (-5.));
    examples.push_back (cost (-2.3));
    examples.push_back (cost (-1.));
    examples.push_back (cost (-0.5));
    examples.push_back (cost (-0.2));
    examples.push_back (cost (0.));
    examples.push_back (cost (0.2));
    examples.push_back (cost (0.5));
    examples.push_back (cost (1.));
    examples.push_back (cost (2.3));
    examples.push_back (cost (5.));

    math::check_equal_on (examples);
    math::check_hash (examples);

    math::check_magma <cost> (math::times, math::plus, examples);

    math::check_semiring <cost, math::either> (
        math::times, math::plus, examples);
    math::check_semiring <cost, math::either> (
        math::times, math::choose, examples);
}

BOOST_AUTO_TEST_SUITE_END()
