// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_generalise_type
#include "boost/test/unit_test.hpp"

#include "math/generalise_type.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_generalise_type)

BOOST_AUTO_TEST_CASE(test_generalise_type)
{
    // No operations.
    static_assert(
        std::is_same_v<math::generalise_type<meta::vector<>, int>::type, int>);
    static_assert(
        std::is_same_v<
            math::generalise_type<meta::vector<>, math::cost<float>>::type,
            math::cost<float>>);
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<>, math::single_sequence<char>>::type,
                  math::single_sequence<char>>);

    // plus.
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::plus>, int>::type,
                  int>);
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::plus>,
                      math::cost<float> const &>::type,
                  math::cost<float>>);
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::plus>,
                      math::single_sequence<char> const>::type,
                  math::optional_sequence<char>>);

    // times.
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::times>, int>::type,
                  int>);
    static_assert(
        std::is_same_v<
            math::generalise_type<
                meta::vector<math::callable::times>, math::cost<float>>::type,
            math::cost<float>>);
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::times>,
                      math::single_sequence<char>>::type,
                  math::sequence<char>>);

    // plus and times.
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::times, math::callable::plus>,
                      int>::type,
                  int>);
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::times, math::callable::plus>,
                      math::cost<float>>::type,
                  math::cost<float>>);
    static_assert(std::is_same_v<
                  math::generalise_type<
                      meta::vector<math::callable::times, math::callable::plus>,
                      math::single_sequence<char>>::type,
                  math::sequence<char>>);
}

BOOST_AUTO_TEST_SUITE_END()
