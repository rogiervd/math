// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test product.hpp.
*/

#define BOOST_TEST_MODULE test_math_product_full_homogeneous
#include "boost/test/unit_test.hpp"

#include "product-tests.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_product_full_homogeneous)

BOOST_AUTO_TEST_CASE(test_product_all)
{
    test_product_homogeneous<math::with_inverse<math::callable::times>>();
}

BOOST_AUTO_TEST_SUITE_END()
