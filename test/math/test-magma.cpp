// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test magma.hpp.
There is very little to test in magma.hpp without an example magma.
The following therefore end up testing the main parts of magma.hpp:
- test-arithmetic_magma.cpp
- test-best.cpp
- test-magma.cpp
*/

#define BOOST_TEST_MODULE test_magma
#include <type_traits>

#include "boost/test/unit_test.hpp"

#include "math/magma.hpp"

BOOST_AUTO_TEST_SUITE(test_magma)

BOOST_AUTO_TEST_CASE(test_magma)
{
    // inverse_operation.
    {
        // Check return type: should be "divide" functor.
        // Two variants: with/without direction;
        // with the operation as compile-time/run-time argument.
        static_assert(
            std::is_same_v<
                std::decay<math::result_of<math::callable::inverse_operation<>(
                    math::callable::times)>::type>::type,
                math::callable::divide<>>);
        static_assert(
            std::is_same_v<
                std::decay<math::result_of<math::callable::inverse_operation<
                    math::left>(math::callable::times)>::type>::type,
                math::callable::divide<math::left>>);
        static_assert(
            std::is_same_v<
                std::decay<math::result_of<math::callable::inverse_operation<
                    math::callable::plus>()>::type>::type,
                math::callable::minus<>>);
        static_assert(
            std::is_same_v<
                std::decay<math::result_of<math::callable::inverse_operation<
                    math::right, math::callable::plus>()>::type>::type,
                math::callable::minus<math::right>>);
    }
}

BOOST_AUTO_TEST_SUITE_END()
