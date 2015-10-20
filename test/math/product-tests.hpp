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

#include "math/check/report_check_magma_boost_test.hpp"

template <class Inverses> void test_product_homogeneous() {
    // Test the empty product.
    // This is (trivially!) almost a semiring.
    // Its only element is also an annihilator.
    {
        typedef math::product <math::over<>, Inverses> product;
        std::vector <product> examples;
        examples.push_back (product());

        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        // Inversion can't be checked since there is no non-annihilator.
        times_checks.do_not_check (
            math::operation_properties::invert_either);
        times_checks.do_not_check (
            math::operation_properties::invert_left);
        times_checks.do_not_check (
            math::operation_properties::invert_right);
        times_checks.do_not_check (
            math::operation_properties::inverse_operator);

        plus_checks.do_not_check (
            math::operation_properties::invert_either);
        plus_checks.do_not_check (
            math::operation_properties::invert_left);
        plus_checks.do_not_check (
            math::operation_properties::invert_right);
        plus_checks.do_not_check (
            math::operation_properties::inverse_operator);

        math::report_check_magma <product> (
            math::times, math::plus, examples, examples,
            type_checks, times_checks, plus_checks,
            times_plus_checks, plus_times_checks);
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

        std::vector <product> unequal_examples;
        unequal_examples.push_back (product (
            0, math::sequence <char> (std::string (""))));
        unequal_examples.push_back (product (
            4, math::sequence <char> (std::string (""))));
        unequal_examples.push_back (product (
            2, math::sequence <char> (std::string ("a"))));
        unequal_examples.push_back (product (
            5, math::sequence <char> (std::string ("ab"))));
        unequal_examples.push_back (product (
            7, math::sequence <char> (std::string ("cba"))));
        unequal_examples.push_back (product (
            3, math::sequence <char> (std::string ("aba"))));

        std::vector <product> examples = unequal_examples;
        // This is the same as (0, "") for with_inverse <times>.
        examples.push_back (product (
            0, math::sequence <char> (std::string ("q"))));
        // Any product with an annihilator compares equal.
        examples.push_back (product (
            0, math::sequence_annihilator <char> ()));
        examples.push_back (product (
            3, math::sequence_annihilator <char> ()));

        math::report_check_semiring <product, math::left> (
            math::times, math::plus, unequal_examples, examples);
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
        auto unequal_examples = range::make_tuple (
            make_product <Inverses> (2.f, math::empty_sequence <char>()),
            make_product <Inverses> (4.f, math::empty_sequence <char>()),
            make_product <Inverses> (5.f, math::single_sequence <char> ('A')),
            make_product <Inverses> (5.f, math::single_sequence <char> ('z')),
            make_product <Inverses> (1.f,
                math::sequence <char> (std::string("Az"))),
            make_product <Inverses> (1.f,
                math::sequence <char> (std::string("zAz")))
            );
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

        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        // There seems to be a strange and hard-to-debug error with CLang 3.4
        // and 3.5 with -O2 and up.
        // Skip the test that causes this.
        // \todo Check that this is actually a compiler bug.
#if (NDEBUG && BOOST_CLANG && __clang_major__ == 3 && \
    (__clang_minor__ == 4 || __clang_minor__ == 5))
        times_checks.do_not_check (math::operation_properties::operator_);
        plus_checks.do_not_check (math::operation_properties::operator_);
#endif

        math::report_check_semiring <product, math::left> (
            math::times, math::plus, unequal_examples, examples,
            type_checks, times_checks, plus_checks,
            times_plus_checks, plus_times_checks);
    }
}

#endif // MATH_TEST_MATH_PRODUCT_TESTS_HPP_INCLUDED
