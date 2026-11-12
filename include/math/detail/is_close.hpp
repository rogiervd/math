// SPDX-FileCopyrightText: Copyright 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MATH_DETAIL_IS_CLOSE_HPP
#define MATH_DETAIL_IS_CLOSE_HPP

#include <type_traits>

#include <cmath>


#include "./log-float_fwd.hpp"

namespace math { namespace detail {

    template <
        class Type, class Tolerance,
        class Enable = std::enable_if_t<!is_log_float<Type>::value>>
    inline bool is_close(
        Type const & left, Type const & right, Tolerance const & tolerance)
    {
        using std::abs;
        if (left == right) {
            return true;
        } else {
            auto difference = abs(left - right);
            return difference <= tolerance * abs(left)
                && difference <= tolerance * abs(right);
        }
    }

    // log_float: compare exponents.
    template <
        class Type, class Tolerance,
        class Enable = std::enable_if_t<is_log_float<Type>::value>>
    inline bool is_close(
        Type const & left, Type const & right, Tolerance const & tolerance,
        // Dummy argument to distinguish this overloads from the one above.
        void * = 0)
    {
        return is_close(left.exponent(), right.exponent(), tolerance);
    }

}}  // namespace math::detail

#endif  // MATH_DETAIL_IS_CLOSE_HPP
