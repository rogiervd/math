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

#define BOOST_TEST_MODULE test_math_product_full_homogeneous
#include "utility/test/boost_unit_test.hpp"

#include "product-tests.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_product_full_homogeneous)

BOOST_AUTO_TEST_CASE (test_product_all) {
    test_product_homogeneous <math::with_inverse <math::callable::plus>>();
}

BOOST_AUTO_TEST_SUITE_END()
