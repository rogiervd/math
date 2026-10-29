// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test code samples from the documentation.
*/

#include <vector>

#include "range/std/vector.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/check/check_magma.hpp"
#include "math/magma.hpp"

int main()
{
    {
        // [start-callable]
        assert(math::times(3., 5.) == 15.);
        assert(math::plus(3., 5.) == 8.);
        // [end-callable]
    }

    {
        // [start-zero]
        assert((math::identity<double, math::callable::plus>()) == 0.);
        assert(math::zero<double>() == 0.);
        // [end-zero]
    }

    {
        // [start-inverse_operation]
        assert(math::inverse_operation(math::times)(6., 3.) == 2.);
        // [end-inverse_operation]
    }

    {
        // [start-left_right]
        assert(math::divide<math::right>(6., 3.) == 2.);
        assert(math::divide<math::left>(6., 3.) == 2.);
        assert(math::divide<math::either>(6., 3.) == 2.);
        // [end-left_right]
    }

    {
        // [start-properties]
        static_assert(
            math::has<math::callable::identity<
                double, math::callable::times>()>::value,
            "'identity' should be defined for double and operation 'times'.");
        static_assert(
            math::has<math::callable::times(
                math::callable::plus(double, double const), double)>::value,
            "All operations in the sequence of operations should be defined.");
        static_assert(
            math::is::associative<math::callable::times(
                double, double &)>::value,
            "'times' is associative.");
        static_assert(
            math::is::commutative<math::callable::plus(double, double)>::value,
            "'plus' is commutative.");
        static_assert(
            math::is::distributive<math::callable::times(
                math::callable::plus(double, double const), double)>::value,
            "'times' distributes over 'plus'.");
        // [end-properties]
    }

    {
        // [start-check_semiring]
        std::vector<float> examples;
        examples.push_back(-.3f);
        examples.push_back(0.f);
        examples.push_back(4.f);

        math::type_checklist type_checks;
        math::operation_checklist times_checks;
        math::operation_checklist plus_checks;
        math::two_operations_checklist times_plus_checks;
        math::two_operations_checklist plus_times_checks;

        math::check_semiring<float, math::either>(
            math::times, math::plus, examples, type_checks, times_checks,
            plus_checks, times_plus_checks, plus_times_checks);
        // [end-check_semiring]
    }

    return 0;
}
