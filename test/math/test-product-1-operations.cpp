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

template <class Inverses> void test_product_spot_check_without_inverses() {
    typedef math::product <math::over <float, math::sequence <char>>, Inverses>
        product;

    // Construction.
    {
        product p (1.5f, math::sequence <char> (std::string ("!")));
        BOOST_CHECK_EQUAL (range::first (p.components()), 1.5f);
        BOOST_CHECK_EQUAL (
            range::first (range::second (p.components()).symbols()), '!');

        // Use make_product.
        auto p2 = math::make_product <Inverses> (
            2.5f, math::sequence <char> (std::string ("?")));

        static_assert (std::is_same <decltype (p2), product>::value, "");
        BOOST_CHECK_EQUAL (range::first (p2.components()), 2.5f);
        BOOST_CHECK_EQUAL (
            range::first (range::second (p2.components()).symbols()), '?');

        // Use make_product_over.
        range::tuple <float, math::sequence <char>> components (
            4.5f, math::sequence <char> (std::string ("~")));
        auto p3 = math::make_product_over <Inverses> (components);

        static_assert (std::is_same <decltype (p3), product>::value, "");
        BOOST_CHECK_EQUAL (range::first (p3.components()), 4.5f);
        BOOST_CHECK_EQUAL (
            range::first (range::second (p3.components()).symbols()), '~');
    }

    // is_member.
    {
        product not_a_member (math::non_member <float>(),
            math::sequence <char> (std::string ("a")));
        BOOST_CHECK (!math::is_member (not_a_member));

        product member (0,
            math::sequence <char> (std::string ("a")));
        BOOST_CHECK (math::is_member (member));

        // Check that one non-member makes a non-member.
        math::product <math::over <float, float>> not_a_member_2 (
            math::non_member <float>(), 0);
        BOOST_CHECK (!math::is_member (not_a_member_2));

        math::product <math::over <float, float>> not_a_member_3 (
            math::non_member <float>(), math::non_member <float>());
        BOOST_CHECK (!math::is_member (not_a_member_3));
    }

    // equal.
    {
        product a1 (1, math::sequence <char> (std::string ("a")));
        product a1_2 (1, math::sequence <char> (std::string ("a")));
        product a1_3 (1, math::single_sequence <char> ('a'));
        product a1_approximate (1.000001, math::single_sequence <char> ('a'));
        product b1 (1, math::sequence <char> (std::string ("b")));
        product a2 (2, math::sequence <char> (std::string ("a")));
        product nothing2_1 (2, math::sequence <char> (std::string ("")));
        product nothing2_2 (2, math::empty_sequence <char>());
        product annihilator3_1 (3,
            math::sequence <char> (math::sequence_annihilator <char>()));
        product annihilator3_2 (3, math::sequence_annihilator <char>());

        BOOST_CHECK (math::equal (a1, a1));
        BOOST_CHECK (math::approximately_equal (a1, a1));

        BOOST_CHECK (math::equal (a1, a1_2));
        BOOST_CHECK (math::approximately_equal (a1, a1_2));

        BOOST_CHECK (math::equal (a1, a1_3));
        BOOST_CHECK (math::approximately_equal (a1, a1_3));

        BOOST_CHECK (!math::equal (a1, a1_approximate));
        BOOST_CHECK (math::approximately_equal (a1, a1_approximate));

        BOOST_CHECK (!math::equal (a1, b1));
        BOOST_CHECK (!math::approximately_equal (a1, b1));

        BOOST_CHECK (!math::equal (a1, a2));
        BOOST_CHECK (!math::approximately_equal (a1, a2));

        BOOST_CHECK (!math::equal (a1, nothing2_1));
        BOOST_CHECK (!math::approximately_equal (a1, nothing2_1));

        BOOST_CHECK (!math::equal (a1, nothing2_2));
        BOOST_CHECK (!math::approximately_equal (a1, nothing2_2));

        BOOST_CHECK (math::equal (nothing2_1, nothing2_2));
        BOOST_CHECK (math::approximately_equal (nothing2_1, nothing2_2));

        BOOST_CHECK (math::equal (annihilator3_1, annihilator3_2));
        BOOST_CHECK (math::approximately_equal (
            annihilator3_1, annihilator3_2));
        BOOST_CHECK (!math::equal (a1, annihilator3_2));
        BOOST_CHECK (!math::approximately_equal (a1, annihilator3_2));
    }

    // Test identity <plus>.
    {
        auto zero = math::zero <product>();
        BOOST_CHECK_EQUAL (range::first (zero.components()), 0);
        BOOST_CHECK_EQUAL (range::at_c <1> (zero.components()),
            math::sequence_annihilator <char>());
    }
    {
        // Implicitly convert to "product".
        product zero = math::zero <product>();
        BOOST_CHECK_EQUAL (range::first (zero.components()), 0);
        BOOST_CHECK_EQUAL (range::at_c <1> (zero.components()),
            math::sequence_annihilator <char>());
    }

    // Test identity <times>.
    {
        auto one = math::one <product>();
        BOOST_CHECK_EQUAL (range::first (one.components()), 1);
        BOOST_CHECK_EQUAL (range::at_c <1> (one.components()),
            math::empty_sequence <char>());
    }
    {
        product one = math::one <product>();
        BOOST_CHECK_EQUAL (range::first (one.components()), 1);
        BOOST_CHECK_EQUAL (range::at_c <1> (one.components()),
            math::empty_sequence <char>());
    }

    // Test annihilator.
    {
        auto zero = math::annihilator <product, math::callable::times>();
        BOOST_CHECK_EQUAL (range::first (zero.components()), 0);
        BOOST_CHECK_EQUAL (range::at_c <1> (zero.components()),
            math::sequence_annihilator <char>());
    }
    {
        BOOST_MPL_ASSERT_NOT ((math::has <
            math::callable::annihilator <product, math::callable::plus>()>));
        // auto test = math::annihilator <product, math::callable::plus>();
    }

    {
        BOOST_MPL_ASSERT_NOT ((math::has <
            math::callable::invert <math::callable::plus> (product)>));
    }

    product a2 (2, math::sequence <char> (std::string ("a")));
    product b4 (4, math::sequence <char> (std::string ("b")));
    product ab8 = a2 * b4;
    BOOST_CHECK_EQUAL (range::first (ab8.components()), 8);
    BOOST_CHECK_EQUAL (range::at_c <1> (ab8.components()),
        math::sequence <char> (std::string ("ab")));
    BOOST_CHECK_EQUAL (ab8,
        product (8, math::sequence <char> (std::string ("ab"))));

    product a10 = ab8 + a2;
    BOOST_CHECK_EQUAL (range::first (a10.components()), 10);
    BOOST_CHECK_EQUAL (range::at_c <1> (a10.components()),
        math::sequence <char> (std::string ("a")));

    // compare.
    BOOST_MPL_ASSERT ((math::has <math::callable::compare (product, product)>));

    product a4 (4, math::sequence <char> (std::string ("a")));
    product b2 (2, math::sequence <char> (std::string ("b")));
    BOOST_CHECK (math::compare (a2, b4));
    BOOST_CHECK (!math::compare (a2, a2));
    BOOST_CHECK (!math::compare (b4, b4));
    BOOST_CHECK (math::compare (a2, b2));

    // Test reverse.
    {
        auto ab8_reverse = math::reverse <math::callable::times> (ab8);
        // Should be the same as ab8, except the sequence has become a right
        // sequence and is in reverse order.
        BOOST_MPL_ASSERT ((std::is_same <decltype (ab8_reverse),
            math::product <math::over <float,
                math::sequence <char, math::right>>, Inverses>>));

        BOOST_CHECK_EQUAL (range::first (ab8_reverse.components()), 8);
        BOOST_CHECK_EQUAL (range::at_c <1> (ab8_reverse.components()),
            (math::sequence <char, math::right> (std::string ("ba"))));
    }

    // Test properties.
    BOOST_MPL_ASSERT_NOT ((
        math::has <math::callable::minus <math::left> (product, product)>));
}

BOOST_AUTO_TEST_CASE (test_product_properties) {
    using math::product;
    using math::over;
    using math::sequence;
    using math::cost;

    using math::callable::plus;
    using math::callable::times;

    // approximate.
    {
        BOOST_MPL_ASSERT ((math::is::approximate <times (
            product <over <float, float>>,
            product <over <float, float>>)>));
        BOOST_MPL_ASSERT ((math::is::approximate <times (
            product <over <float, sequence <char>>>,
            product <over <float, sequence <char>>>)>));
        BOOST_MPL_ASSERT_NOT ((math::is::approximate <times (
            product <over <sequence <char>>>,
            product <over <sequence <char>>>)>));
    }
    // associative.
    {
        BOOST_MPL_ASSERT ((math::is::associative <times,
            product <over <float, sequence <char>>>>));
    }
    // commutative.
    {
        BOOST_MPL_ASSERT ((math::is::commutative <times,
            product <over <float, float>>>));
        BOOST_MPL_ASSERT ((math::is::commutative <plus,
            product <over <float, float>>>));

        BOOST_MPL_ASSERT_NOT ((math::is::commutative <times,
            product <over <float, sequence <char>>>>));
        BOOST_MPL_ASSERT ((math::is::commutative <plus,
            product <over <float, sequence <char>>>>));
    }
    // idempotent.
    {
        BOOST_MPL_ASSERT_NOT ((math::is::idempotent <times,
            product <over <float, float>>>));
        BOOST_MPL_ASSERT_NOT ((math::is::idempotent <plus,
            product <over <float, float>>>));

        BOOST_MPL_ASSERT_NOT ((math::is::idempotent <times,
            product <over <cost <float>, sequence <char>>>>));
        BOOST_MPL_ASSERT ((math::is::idempotent <plus,
            product <over <cost <float>, sequence <char>>>>));
    }
    // path_operation.
    {
        BOOST_MPL_ASSERT_NOT ((math::is::path_operation <times,
            product <over <float, float>>>));
        BOOST_MPL_ASSERT_NOT ((math::is::path_operation <plus,
            product <over <float, float>>>));

        BOOST_MPL_ASSERT_NOT ((math::is::path_operation <times,
            product <over <cost <float>, sequence <char>>>>));
        BOOST_MPL_ASSERT_NOT ((math::is::path_operation <plus,
            product <over <cost <float>, sequence <char>>>>));
        // Sanity
        BOOST_MPL_ASSERT ((math::is::path_operation <plus, cost <float>>));
        BOOST_MPL_ASSERT ((math::is::path_operation <plus,
            product <over <cost <float>, cost <float>>>>));
    }
}

BOOST_AUTO_TEST_CASE (test_product_spot_check) {
    test_product_spot_check_without_inverses <math::with_inverse<>>();
    {
        // Spot check for divide.
        test_product_spot_check_without_inverses <
            math::with_inverse <math::callable::times>>();

        typedef math::product <math::over <float, math::sequence <char>>,
            math::with_inverse <math::callable::times>> product;

        product a2 (2, math::sequence <char> (std::string ("a")));
        product b4 (4, math::sequence <char> (std::string ("b")));
        product ab8 = a2 * b4;

        product b4_again = math::divide <math::left> (ab8, a2);
        BOOST_CHECK_EQUAL (range::first (b4_again.components()), 4);
        BOOST_CHECK_EQUAL (range::at_c <1> (b4_again.components()),
            math::sequence <char> (std::string ("b")));
        BOOST_CHECK_EQUAL (b4_again, b4);
    }
    {
        // Spot check for plus.
        test_product_spot_check_without_inverses <
            math::with_inverse <math::callable::plus>>();

        // Spot check for minus.
        typedef math::product <math::over <float, int>,
            math::with_inverse <math::callable::plus>> product;

        product p1 (2, 3);
        product p2 (4, 8);

        product p3 = p1 * p2;
        BOOST_CHECK_EQUAL (p3, product (8, 24));

        product p4 = p1 + p2;
        BOOST_CHECK_EQUAL (p4, product (6., 11));

        product p1_again = p4 - p2;
        BOOST_CHECK_EQUAL (p1_again, p1);
        product p2_again = p4 - p1;
        BOOST_CHECK_EQUAL (p2_again, p2);
    }
}

BOOST_AUTO_TEST_SUITE_END()
