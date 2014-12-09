/*
Copyright 2012, 2013 Rogier van Dalen.

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
Test std::numeric_limits <log_float <...>>.
*/

#define BOOST_TEST_MODULE log_float_limits
#include "../boost_unit_test.hpp"

#include "math/log-float.hpp"

#include <limits>

#include <boost/math/special_functions/fpclassify.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_log_float_limits)

template <class LogFloat, class Underlying> void check_limits() {
    typedef std::numeric_limits <LogFloat> limits;
    typedef std::numeric_limits <Underlying> limits_underlying;

    /* Static properties. */
    static_assert (limits::is_specialized, "");
    static_assert (!limits::is_integer, "");
    static_assert (!limits::is_exact, "");
    static_assert (limits::has_infinity, "");

    static_assert (limits::has_quiet_NaN, "");
    static_assert (limits::has_signaling_NaN, "");
    static_assert (limits::has_denorm == std::denorm_absent, "");
    static_assert (!limits::has_denorm_loss, "");
    static_assert (limits::round_style == std::round_to_nearest, "");

    static_assert (!limits::is_iec559, "");
    static_assert (limits::is_bounded, "");
    static_assert (!limits::is_modulo, "");

    static_assert (!limits::traps, "");
    static_assert (!limits::tinyness_before, "");

    /* Functions. */
    // min.
    {
        auto min = limits::min();
        BOOST_CHECK (min > 0);
        BOOST_CHECK_EQUAL (min.exponent(), limits_underlying::lowest());
    }
    {
        auto max = limits::max();
        BOOST_CHECK (max > 0);
        BOOST_CHECK_EQUAL (max.exponent(), limits_underlying::max());
    }
    {
        // 1+epsilon must have the smallest representable underlying float as
        // its exponent.
        auto epsilon = limits::epsilon();
        decltype (epsilon) one = 1;
        auto one_plus_epsilon = one + epsilon;
        BOOST_CHECK_EQUAL (one_plus_epsilon.exponent(),
            limits_underlying::denorm_min());
    }
    {
        auto infinity = limits::infinity();
        BOOST_CHECK (infinity > 0);
        BOOST_CHECK_EQUAL (infinity.exponent(), limits_underlying::infinity());
    }
    {
        auto quiet_NaN = limits::quiet_NaN();
        BOOST_CHECK (!(quiet_NaN == quiet_NaN));
        using namespace boost::math;
        BOOST_CHECK (isnan (quiet_NaN.exponent()));
    }
    {
        auto signaling_NaN = limits::signaling_NaN();
        BOOST_CHECK (!(signaling_NaN == signaling_NaN));
        using namespace boost::math;
        BOOST_CHECK (isnan (signaling_NaN.exponent()));
    }

    /*
    Not tested because they are not meaningfully implemented:
    digits, digits10, max_digits10, radix
    min_exponent, min_exponent10, max_exponent, max_exponent10
    round_error(), denorm_min()
    */
}

template <class LogFloat, class Underlying> void check_limits_unsigned() {
    check_limits <LogFloat, Underlying>();

    typedef std::numeric_limits <LogFloat> limits;

    static_assert (!limits::is_signed, "");

    BOOST_CHECK_EQUAL (limits::lowest(), limits::min());
}

template <class LogFloat, class Underlying> void check_limits_signed() {
    check_limits <LogFloat, Underlying>();

    typedef std::numeric_limits <LogFloat> limits;

    static_assert (limits::is_signed, "");

    BOOST_CHECK_EQUAL (limits::lowest(), -limits::max());
}

BOOST_AUTO_TEST_CASE (test_log_float_limits) {

    check_limits_unsigned <math::log_float <float>, float>();
    check_limits_unsigned <math::log_float <double> const, double>();
    check_limits_unsigned <math::log_float <double> &, double>();
    check_limits_unsigned <math::log_float <double> const &, double>();

    check_limits_signed <math::signed_log_float <float>, float>();
    check_limits_signed <math::signed_log_float <double> const &, double>();
    check_limits_signed <math::signed_log_float <double> &&, double>();

    // long double does not work under Valgrind.
    // check_limits_signed <
    //     math::signed_log_float <long double>, long double>();
    // check_limits_signed <
    //     math::signed_log_float <long double> const &, long double>();

    using namespace boost::math::policies;
    typedef policy <domain_error <errno_on_error>,
        overflow_error <errno_on_error>> other_policy;

    check_limits_unsigned <math::log_float <float, other_policy>, float>();
    check_limits_unsigned <math::log_float <double, other_policy> &, double>();

    check_limits_signed <math::signed_log_float <float, other_policy>, float>();
}

BOOST_AUTO_TEST_SUITE_END()
