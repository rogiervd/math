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

/** \internal \file
Define std::numeric_limits for log_float and signed_log_float.
*/

#ifndef MATH_DETAIL_LOG_FLOAT_LIMITS_HPP_INCLUDED
#define MATH_DETAIL_LOG_FLOAT_LIMITS_HPP_INCLUDED

#include <limits>

#include "./log-float_fwd.hpp"

namespace math { namespace detail {

    /**
    Provide a base for the specialisation of numeric_limits for log_float and
    signed_log_float.
    */
    template <class LogFloat, class Underlying> class log_float_numeric_limits {
        typedef std::numeric_limits <Underlying> underlying_limits;
    public:
        static constexpr bool is_specialized = true;

        static constexpr LogFloat min() throw()
        { return LogFloat (underlying_limits::lowest(), as_exponent()); }

        static constexpr LogFloat max() throw()
        { return LogFloat (underlying_limits::max(), as_exponent()); }

        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;

        /**
        The difference between 1 and the next representable value.
        1 is represented by 0; the next value by the subnormal value of the
        underlying type; call this m.
        The value of interest is exp(m)-1.
        To represent this as a LogFloat, its exponent must be log (exp(m)-1).
        The series expansion of this is log(m) + m/2 + (m**2)/24 + ... .
        The first term is a good approximation.
        */
        static constexpr LogFloat epsilon() throw() {
            using std::log;
            // Because of
            return LogFloat (
                log (underlying_limits::denorm_min()), as_exponent());
        }

        static constexpr bool has_infinity = true;
        static constexpr bool has_quiet_NaN = underlying_limits::has_quiet_NaN;
        static constexpr bool has_signaling_NaN
            = underlying_limits::has_signaling_NaN;

        static constexpr std::float_denorm_style has_denorm
            = std::denorm_absent;
        static constexpr bool has_denorm_loss = false;

        static constexpr LogFloat infinity() throw()
        { return LogFloat (underlying_limits::infinity(), as_exponent()); }

        static constexpr LogFloat quiet_NaN() throw()
        { return LogFloat (underlying_limits::quiet_NaN(), as_exponent()); }

        static constexpr LogFloat signaling_NaN() throw()
        { return LogFloat (underlying_limits::signaling_NaN(), as_exponent()); }

        // Can't be implemented.
        // static constexpr LogFloat denorm_min() throw();
        // static constexpr LogFloat round_error() throw();

        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;

        static constexpr bool traps = false;

        // The following are essentially unimplemented:
        static constexpr int digits = 0;
        static constexpr int digits10 = 0;
        static constexpr int max_digits10 = 0;
        static constexpr int radix = 0;

        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;

        static constexpr bool tinyness_before = false;
        static constexpr std::float_round_style round_style
            = std::round_to_nearest;
    };

}} // namespace math::detail

namespace std {

    template <class ExponentType, class Policy>
        class numeric_limits <math::log_float <ExponentType, Policy>>
    : public ::math::detail::log_float_numeric_limits <
        math::log_float <ExponentType, Policy>, ExponentType>
    {
        typedef math::log_float <ExponentType, Policy> value_type;
        typedef ::math::detail::log_float_numeric_limits <
            value_type, ExponentType> base_type;
    public:
        static constexpr bool is_signed = false;

        static constexpr value_type lowest() throw()
        { return base_type::min(); }
    };

    template <class ExponentType, class Policy>
        class numeric_limits <math::signed_log_float <ExponentType, Policy>>
    : public ::math::detail::log_float_numeric_limits <
        math::signed_log_float <ExponentType, Policy>, ExponentType>
    {
        typedef math::signed_log_float <ExponentType, Policy> value_type;
        typedef ::math::detail::log_float_numeric_limits <
            value_type, ExponentType> base_type;
    public:
        static constexpr bool is_signed = true;

        static constexpr value_type lowest() throw()
        { return -base_type::max(); }
    };

    // Forward qualified types.
    template <class ExponentType, class Policy>
        class numeric_limits <math::log_float <ExponentType, Policy> &>
    : public numeric_limits <math::log_float <ExponentType, Policy>> {};
    template <class ExponentType, class Policy>
        class numeric_limits <math::signed_log_float <ExponentType, Policy> &>
    : public numeric_limits <math::signed_log_float <ExponentType, Policy>> {};

    template <class ExponentType, class Policy>
        class numeric_limits <math::log_float <ExponentType, Policy> &&>
    : public numeric_limits <math::log_float <ExponentType, Policy>> {};
    template <class ExponentType, class Policy>
        class numeric_limits <math::signed_log_float <ExponentType, Policy> &&>
    : public numeric_limits <math::signed_log_float <ExponentType, Policy>> {};

    template <class ExponentType, class Policy>
        class numeric_limits <math::log_float <ExponentType, Policy> const>
    : public numeric_limits <math::log_float <ExponentType, Policy>> {};
    template <class ExponentType, class Policy>
        class numeric_limits <
            math::signed_log_float <ExponentType, Policy> const>
    : public numeric_limits <math::signed_log_float <ExponentType, Policy>> {};

    template <class ExponentType, class Policy>
        class numeric_limits <math::log_float <ExponentType, Policy> const &>
    : public numeric_limits <math::log_float <ExponentType, Policy>> {};
    template <class ExponentType, class Policy>
        class numeric_limits <
            math::signed_log_float <ExponentType, Policy> const &>
    : public numeric_limits <math::signed_log_float <ExponentType, Policy>> {};

    template <class ExponentType, class Policy>
        class numeric_limits <math::log_float <ExponentType, Policy> const &&>
    : public numeric_limits <math::log_float <ExponentType, Policy>> {};
    template <class ExponentType, class Policy>
        class numeric_limits <
            math::signed_log_float <ExponentType, Policy> const &&>
    : public numeric_limits <math::signed_log_float <ExponentType, Policy>> {};

} // namespace std

#endif  // MATH_DETAIL_LOG_FLOAT_LIMITS_HPP_INCLUDED
