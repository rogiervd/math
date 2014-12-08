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
Test product.hpp.
*/

#define BOOST_TEST_MODULE test_math_product_full_homogeneous
#include "utility/test/boost_unit_test.hpp"

#include "product-tests.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_product_full_homogeneous)

BOOST_AUTO_TEST_CASE (test_product_all) {
    test_product_homogeneous <math::with_inverse<>>();
    // These are tested in two separate files:
    // test_product_homogeneous <math::with_inverse <math::callable::plus>>();
    // test_product_homogeneous <math::with_inverse <math::callable::times>>();
}

BOOST_AUTO_TEST_SUITE_END()
