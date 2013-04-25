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
Test log_float.hpp.
Text-search for "****" to find these sections:
- domain error
- overflow error
- underflow error
- basic functionality on corner cases
- binary operations
- functions that mimic the ones from <cmath>
- conversion and interaction

Particular care is taken (hopefully!) to check that everything works with
different policies.
The only error handling policies that are tested are throwing and ignoring.
The implementation uses Boost.Math functions to raise errors so this should
generalise to other forms of behaviour.
Testing the ignoring policies is important if specialisations for error policies
bypass explicit error handling in favour of leaving this to the hardware.
*/

#define BOOST_TEST_MODULE log_float
#include "../boost_unit_test.hpp"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/tools/test.hpp>

#include <iostream>
#include <type_traits>
#include <string>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/not.hpp>

#include "math/log-float.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_log_float)

/**
Check for a command line option --no-long-double, and return false iff it is
found.
This is useful to switch off checking with long double under Valgrind.
Valgrind 3.7.0 uses double-precision computations for long double.
http://valgrind.org/docs/manual/manual-core.html#manual-core.limits
*/
bool enable_long_double() {
    for (int argument_index = 1;
        argument_index <boost::unit_test::framework::master_test_suite().argc;
        ++ argument_index)
    {
        std::string argument = boost::unit_test::framework::master_test_suite()
            .argv [argument_index];
        if (argument == "--no-long-double")
            return false;
    }
    return true;
}

// **** Test domain error handling ****
// Force a domain error.

template <typename RealType, class Policy> void create_domain_error_1() {
    math::log_float <RealType, Policy> w (-1.);
    BOOST_CHECK (w.exponent() != w.exponent());
    math::log_float <RealType, Policy> copy (w);
    BOOST_CHECK (copy.exponent() != copy.exponent());
}

template <typename RealType, class Policy> void create_domain_error_2() {
    math::log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::infinity());
    BOOST_CHECK (w.exponent() != w.exponent());
    math::log_float <RealType, Policy> copy (w);
    BOOST_CHECK (copy.exponent() != copy.exponent());
}

// Construct from signed_log_float with a negative value
template <typename RealType, class Policy> void create_domain_error_3() {
    math::signed_log_float <RealType, Policy> s = -3;
    math::log_float <RealType, Policy> w (s);
    BOOST_CHECK (w.exponent() != w.exponent());
}

// pow (0, -3.)
template <typename RealType, class Policy> void create_domain_error_4() {
    math::log_float <RealType, Policy> w;
    math::log_float <RealType, Policy> result = pow (w, -3);
    BOOST_CHECK (result.exponent() != result.exponent());
}

template <typename RealType, typename Policy>
    void test_log_float_domain_error (
        boost::math::policies::domain_error<
            boost::math::policies::throw_on_error> const &)
{
    BOOST_CHECK_THROW ((create_domain_error_1 <RealType, Policy>()),
        std::domain_error);
    BOOST_CHECK_THROW ((create_domain_error_2 <RealType, Policy>()),
        std::domain_error);
    BOOST_CHECK_THROW ((create_domain_error_3 <RealType, Policy>()),
        std::domain_error);
    BOOST_CHECK_THROW ((create_domain_error_4 <RealType, Policy>()),
        std::domain_error);
}

template <typename RealType, typename Policy>
    void test_log_float_domain_error (
        boost::math::policies::domain_error<
            boost::math::policies::ignore_error> const &)
{
    create_domain_error_1 <RealType, Policy>();
    create_domain_error_2 <RealType, Policy>();
    create_domain_error_3 <RealType, Policy>();
    create_domain_error_4 <RealType, Policy>();
}

template <typename RealType, typename Policy>
    void test_log_float_domain_error (Policy const &)
{
    test_log_float_domain_error <RealType, Policy> (
        typename Policy::domain_error_type());
}

// **** Test overflow error ****

// Force an overflow error.
// (It is impossible to generate overflow using addition.)

template <typename RealType, class Policy> void create_overflow_1() {
    math::log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    w *= w;
    BOOST_CHECK_EQUAL (w.exponent(),
        std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_2() {
    math::log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    math::log_float <RealType, Policy> w1 = w;
    math::log_float <RealType, Policy> w2 = w;
    w = w1 * w2;
    BOOST_CHECK_EQUAL (w.exponent(),
        std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_3() {
    math::log_float <RealType, Policy> w1 (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    math::log_float <RealType, Policy> w2 (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    w1 /= w2;
    BOOST_CHECK_EQUAL (w1.exponent(),
        std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_4() {
    math::log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    math::log_float <RealType, Policy> w1 = w;
    math::log_float <RealType, Policy> w2 (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    w = w1 / w2;
    BOOST_CHECK_EQUAL (w.exponent(),
        std::numeric_limits <RealType>::infinity());
}

// signed_log_float overflow errors: spot checks

template <typename RealType, class Policy> void create_overflow_5() {
    math::signed_log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    w *= w;
    BOOST_CHECK_EQUAL (w.sign(), +1);
    BOOST_CHECK_EQUAL (w.exponent(),
        std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_6() {
    math::signed_log_float <RealType, Policy> w1 (
        std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    math::signed_log_float <RealType, Policy> w2 (
        std::numeric_limits <RealType>::max(), +1, math::as_exponent());
    math::signed_log_float <RealType, Policy> w = w1 * w2;
    BOOST_CHECK_EQUAL (w.sign(), -1);
    BOOST_CHECK_EQUAL (w.exponent(),
        std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_7() {
    math::signed_log_float <RealType, Policy> w1 (
        std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    math::signed_log_float <RealType, Policy> w2 (
        std::numeric_limits <RealType>::max(), +1, math::as_exponent());
    math::signed_log_float <RealType, Policy> w = w1 * w2;
    BOOST_CHECK_EQUAL (w.sign(), -1);
    BOOST_CHECK_EQUAL (w.exponent(),
        std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_get_1() {
    math::log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    RealType result = w.get();
    BOOST_CHECK_EQUAL (result, std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_get_2() {
    math::signed_log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), +1, math::as_exponent());
    RealType result = w.get();
    BOOST_CHECK_EQUAL (result, std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_overflow_get_3() {
    math::signed_log_float <RealType, Policy> w (
        std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    RealType result = w.get();
    BOOST_CHECK_EQUAL (result, - std::numeric_limits <RealType>::infinity());
}

template <typename RealType, typename Policy>
    void test_log_float_overflow (
        boost::math::policies::overflow_error<
            boost::math::policies::throw_on_error> const &)
{
    BOOST_CHECK_THROW ((create_overflow_1 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_2 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_3 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_4 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_5 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_6 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_7 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_get_1 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_get_2 <RealType, Policy>()),
        std::overflow_error);
    BOOST_CHECK_THROW ((create_overflow_get_3 <RealType, Policy>()),
        std::overflow_error);
}

template <typename RealType, typename Policy>
    void test_log_float_overflow (
        boost::math::policies::overflow_error<
            boost::math::policies::ignore_error> const &)
{
    create_overflow_1 <RealType, Policy>();
    create_overflow_2 <RealType, Policy>();
    create_overflow_3 <RealType, Policy>();
    create_overflow_4 <RealType, Policy>();
    create_overflow_5 <RealType, Policy>();
    create_overflow_6 <RealType, Policy>();
    create_overflow_7 <RealType, Policy>();

    create_overflow_get_1 <RealType, Policy>();
    create_overflow_get_2 <RealType, Policy>();
    create_overflow_get_3 <RealType, Policy>();
}

template <typename RealType, typename Policy>
    void test_log_float_overflow (Policy const &)
{
    test_log_float_overflow <RealType, Policy> (
        typename Policy::overflow_error_type());
}


// **** Test underflow error ****

// Create underflow error.
template <typename RealType, class Policy> void create_underflow_1() {
    math::log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    w *= w;
    BOOST_CHECK_EQUAL (w.exponent(),
        -std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_underflow_2() {
    math::log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    math::log_float <RealType, Policy> w1 = w;
    math::log_float <RealType, Policy> w2 = w;
    w = w1 * w2;
    BOOST_CHECK_EQUAL (w.exponent(),
        -std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_underflow_3() {
    math::log_float <RealType, Policy> w1 (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    math::log_float <RealType, Policy> w2 (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    w1 /= w2;
    BOOST_CHECK_EQUAL (w1.exponent(),
        -std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_underflow_4() {
    math::log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    math::log_float <RealType, Policy> w1 = w;
    math::log_float <RealType, Policy> w2 (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    w = w1 / w2;
    BOOST_CHECK_EQUAL (w.exponent(),
        -std::numeric_limits <RealType>::infinity());
}

// signed_log_float underflow errors: spot checks

template <typename RealType, class Policy> void create_underflow_5() {
    math::signed_log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    math::signed_log_float <RealType, Policy> w1 = w;
    math::signed_log_float <RealType, Policy> w2 (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    w = w1 / w2;
    BOOST_CHECK_EQUAL (w.sign(), +1);
    BOOST_CHECK_EQUAL (w.exponent(),
        -std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_underflow_6() {
    math::signed_log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    math::signed_log_float <RealType, Policy> w1 = w;
    math::signed_log_float <RealType, Policy> w2 (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    w = w1 * w2;
    BOOST_CHECK_EQUAL (w.sign(), -1);
    BOOST_CHECK_EQUAL (w.exponent(),
        -std::numeric_limits <RealType>::infinity());
}

template <typename RealType, class Policy> void create_underflow_7() {
    math::signed_log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    math::signed_log_float <RealType, Policy> w1 = w;
    math::signed_log_float <RealType, Policy> w2 (
        std::numeric_limits <RealType>::max(), math::as_exponent());
    w = w1 / w2;
    BOOST_CHECK_EQUAL (w.sign(), -1);
    BOOST_CHECK_EQUAL (w.exponent(),
        -std::numeric_limits <RealType>::infinity());
}


template <typename RealType, class Policy> void create_underflow_get_1() {
    math::log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    RealType result = w.get();
    BOOST_CHECK_EQUAL (result, 0);
}

template <typename RealType, class Policy> void create_underflow_get_2() {
    math::signed_log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), math::as_exponent());
    RealType result = w.get();
    BOOST_CHECK_EQUAL (result, 0);
    BOOST_CHECK (!boost::math::signbit (result));
}

template <typename RealType, class Policy> void create_underflow_get_3() {
    math::signed_log_float <RealType, Policy> w (
        -std::numeric_limits <RealType>::max(), -1, math::as_exponent());
    RealType result = w.get();
    BOOST_CHECK_EQUAL (result, 0);
    BOOST_CHECK (boost::math::signbit (result));
}

template <typename RealType, typename Policy>
    void test_log_float_underflow (
        boost::math::policies::underflow_error<
            boost::math::policies::throw_on_error> const &)
{
    BOOST_CHECK_THROW ((create_underflow_1 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_2 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_3 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_4 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_5 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_6 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_7 <RealType, Policy>()),
        std::underflow_error);

    BOOST_CHECK_THROW ((create_underflow_get_1 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_get_2 <RealType, Policy>()),
        std::underflow_error);
    BOOST_CHECK_THROW ((create_underflow_get_3 <RealType, Policy>()),
        std::underflow_error);
}

template <typename RealType, typename Policy>
    void test_log_float_underflow (
        boost::math::policies::underflow_error<
            boost::math::policies::ignore_error> const &)
{
    create_underflow_1 <RealType, Policy>();
    create_underflow_2 <RealType, Policy>();
    create_underflow_3 <RealType, Policy>();
    create_underflow_4 <RealType, Policy>();
    create_underflow_5 <RealType, Policy>();
    create_underflow_6 <RealType, Policy>();
    create_underflow_7 <RealType, Policy>();

    create_underflow_get_1 <RealType, Policy>();
    create_underflow_get_2 <RealType, Policy>();
    create_underflow_get_3 <RealType, Policy>();
}

template <typename RealType, typename Policy>
    void test_log_float_underflow (Policy const &)
{
    test_log_float_underflow <RealType, Policy> (
        typename Policy::underflow_error_type());
}


// **** Test basic functionality on corner cases ****

template <typename RealType, class Policy> void test_log_float_unary_impl() {
    typedef std::numeric_limits <RealType> limits;

    typedef math::log_float <RealType, Policy> log_float;
    typedef math::signed_log_float <RealType, Policy> signed_log_float;

    // Default constructor
    {
        log_float w;
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
    }

    // Constructor with 1 parameter.
    {
        log_float w (0);
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
    }
    {
        log_float w (0.);
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
    }
    {
        log_float w (1);
        BOOST_CHECK (w.exponent() == 0.);
    }
    {
        log_float w (1.);
        BOOST_CHECK_EQUAL (w.exponent(), 0.);
    }
    test_log_float_domain_error <RealType, Policy> (Policy());
    RealType examples[] = {
        limits::min(),
        1.032487e-20, .2095768, 1.000001, 1.74810475, 7.34870912, 8.235482e+20,
        std::numeric_limits <RealType>::max() };
    for (RealType example : examples)
    {
        log_float w (example);
        using std::log;
        BOOST_CHECK_EQUAL (w.exponent(), log (RealType (example)));
    }
    {
        log_float w (limits::infinity());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
    }
    {
        log_float w (limits::quiet_NaN());
        BOOST_CHECK (w.exponent() != w.exponent());
    }

    // Explicitly give exponent.
    {
        log_float w (-limits::infinity(), math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(),  -limits::infinity());
    }
    {
        log_float w (0., math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), 0.);
    }
    {
        log_float w (limits::max(), math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), limits::max());
    }
    {
        log_float w (limits::infinity(), math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
    }
    {
        log_float w (limits::quiet_NaN(), math::as_exponent());
        BOOST_CHECK (w.exponent() != w.exponent());
    }

    // For the same cases, test
    // .get(), operator RealType, operator bool, operator!, copy construction
    {
        log_float w;
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.get(), 0.);
        BOOST_CHECK_EQUAL (RealType (w), 0.);
        if (w) { BOOST_CHECK (false); }
        BOOST_CHECK (!w);
        log_float copy (w);
        BOOST_CHECK_EQUAL (copy.exponent(), -limits::infinity());

        signed_log_float signed_copy (-w);
        BOOST_CHECK_EQUAL (signed_copy.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (signed_copy.sign(), -1);
    }
    {
        log_float w (1);
        BOOST_CHECK (w.exponent() == 0.);
        BOOST_CHECK_EQUAL (w.get(), 1.);
        BOOST_CHECK_EQUAL (RealType (w), 1.);
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        log_float copy (w);
        BOOST_CHECK (copy.exponent() == 0.);

        signed_log_float signed_copy (-w);
        BOOST_CHECK_EQUAL (signed_copy.exponent(), 0);
        BOOST_CHECK_EQUAL (signed_copy.sign(), -1);
    }
    {
        log_float w (limits::infinity());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
        // The following should not cause an overflow error, since the overflow
        // has already happened.
        BOOST_CHECK_EQUAL (w.get(), limits::infinity());
        BOOST_CHECK_EQUAL (RealType (w), limits::infinity());
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        log_float copy (w);
        BOOST_CHECK_EQUAL (copy.exponent(), limits::infinity());

        signed_log_float signed_copy (-w);
        BOOST_CHECK_EQUAL (signed_copy.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (signed_copy.sign(), -1);
    }
    {
        log_float w (limits::quiet_NaN());
        BOOST_CHECK (w.exponent() != w.exponent());
        // The following should not cause an indeterminate-value error.
        BOOST_CHECK (w.get() != w.get());
        BOOST_CHECK (RealType (w) != RealType (w));
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        log_float copy (w);
        BOOST_CHECK (copy.exponent() != copy.exponent());

        signed_log_float signed_copy (-w);
        BOOST_CHECK (signed_copy.exponent() != signed_copy.exponent());
    }
}

template <typename RealType, class Policy>
    void test_signed_log_float_unary_impl()
{
    typedef std::numeric_limits <RealType> limits;

    typedef math::signed_log_float <RealType, Policy> signed_log_float;

    // Default constructor
    {
        signed_log_float w;
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }

    // Constructor with 1 parameter.
    {
        signed_log_float w (0);
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (0.);
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (-0.);
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (1);
        BOOST_CHECK (w.exponent() == 0.);
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (1.);
        BOOST_CHECK_EQUAL (w.exponent(), 0.);
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (-1);
        BOOST_CHECK (w.exponent() == 0.);
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (-1.);
        BOOST_CHECK_EQUAL (w.exponent(), 0.);
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    RealType examples[] = {
        std::numeric_limits <RealType>::min(),
        1.032487e-20, .2095768, 1.000001, 1.74810475, 7.34870912, 8.235482e+20,
        std::numeric_limits <RealType>::max() };
    for (RealType example : examples)
    {
        signed_log_float w (example);
        using std::log;
        BOOST_CHECK_EQUAL (w.exponent(), log (RealType (example)));
        BOOST_CHECK_EQUAL (w.sign(), +1);
        signed_log_float w2 (-example);
        BOOST_CHECK_EQUAL (w2.exponent(), log (RealType (example)));
        BOOST_CHECK_EQUAL (w2.sign(), -1);
    }
    {
        signed_log_float w (limits::infinity());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (-limits::infinity());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (limits::quiet_NaN());
        BOOST_CHECK (w.exponent() != w.exponent());
        // Not really required for sign() to be defined for NaN.
        // BOOST_CHECK_EQUAL (w.sign(), +1);
    }

    // Explicitly give exponent.
    {
        signed_log_float w (-limits::infinity(), math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (3, math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), 3);
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (3, -1, math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), 3);
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (-limits::infinity(), +1, math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(),  -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
    }
    {
        signed_log_float w (-limits::infinity(), -1, math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(),  -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (0., -1, math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), 0.);
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (limits::infinity(), -1, math::as_exponent());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), -1);
    }
    {
        signed_log_float w (limits::quiet_NaN(), math::as_exponent());
        BOOST_CHECK (w.exponent() != w.exponent());
    }

    // For the same cases, test
    // .get(), operator RealType, operator bool, operator!, copy construction,
    // operator-()
    {
        signed_log_float w;
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
        BOOST_CHECK_EQUAL (w.get(), 0.);
        BOOST_CHECK_EQUAL (RealType (w), 0.);
        if (w) { BOOST_CHECK (false); }
        BOOST_CHECK (!w);
        signed_log_float copy (w);
        BOOST_CHECK_EQUAL (copy.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (copy.sign(), +1);
        copy = -w;
        BOOST_CHECK_EQUAL (copy.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (copy.sign(), -1);
    }
    {
        signed_log_float w (-0.);
        BOOST_CHECK_EQUAL (w.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (w.get(), -0.);
        BOOST_CHECK (boost::math::signbit (w.get()));
        BOOST_CHECK_EQUAL (RealType (w), -0.);
        BOOST_CHECK (boost::math::signbit (RealType (w)));
        if (w) { BOOST_CHECK (false); }
        BOOST_CHECK (!w);
        signed_log_float copy (w);
        BOOST_CHECK_EQUAL (copy.exponent(), -limits::infinity());
        BOOST_CHECK (boost::math::signbit (w.get()));
        copy = -w;
        BOOST_CHECK_EQUAL (copy.exponent(), -limits::infinity());
        BOOST_CHECK_EQUAL (copy.sign(), +1);
    }
    {
        signed_log_float w (1);
        BOOST_CHECK (w.exponent() == 0.);
        BOOST_CHECK_EQUAL (w.get(), 1.);
        BOOST_CHECK_EQUAL (RealType (w), 1.);
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        signed_log_float copy (w);
        BOOST_CHECK (copy.exponent() == 0.);
        copy = -w;
        BOOST_CHECK_EQUAL (copy.exponent(), 0);
        BOOST_CHECK_EQUAL (copy.sign(), -1);
    }
    {
        signed_log_float w (-1);
        BOOST_CHECK (w.exponent() == 0.);
        BOOST_CHECK_EQUAL (w.sign(), -1);
        BOOST_CHECK_EQUAL (w.get(), -1.);
        BOOST_CHECK_EQUAL (RealType (w), -1.);
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        signed_log_float copy (w);
        BOOST_CHECK (copy.exponent() == 0.);
        BOOST_CHECK_EQUAL (copy.get(), -1.);
        copy = -w;
        BOOST_CHECK_EQUAL (copy.exponent(), 0);
        BOOST_CHECK_EQUAL (copy.sign(), +1);
    }
    {
        signed_log_float w (limits::infinity());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), +1);
        // The following should not cause an overflow error, since the overflow
        // has already happened.
        BOOST_CHECK_EQUAL (w.get(), limits::infinity());
        BOOST_CHECK_EQUAL (RealType (w), limits::infinity());
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        signed_log_float copy (w);
        BOOST_CHECK_EQUAL (copy.exponent(), limits::infinity());
        copy = -w;
        BOOST_CHECK_EQUAL (copy.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (copy.sign(), -1);
    }
    {
        signed_log_float w (-limits::infinity());
        BOOST_CHECK_EQUAL (w.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (w.sign(), -1);
        // The following should not cause an overflow error, since the overflow
        // has already happened.
        BOOST_CHECK_EQUAL (w.get(), -limits::infinity());
        BOOST_CHECK_EQUAL (RealType (w), -limits::infinity());
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        signed_log_float copy (w);
        BOOST_CHECK_EQUAL (copy.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (copy.sign(), -1);
        copy = -w;
        BOOST_CHECK_EQUAL (copy.exponent(), limits::infinity());
        BOOST_CHECK_EQUAL (copy.sign(), +1);
    }
    {
        signed_log_float w (limits::quiet_NaN());
        BOOST_CHECK (w.exponent() != w.exponent());
        // The following should not cause an indeterminate-value error.
        BOOST_CHECK (w.get() != w.get());
        BOOST_CHECK (RealType (w) != RealType (w));
        BOOST_CHECK (w);
        BOOST_CHECK (!!w);
        signed_log_float copy (w);
        BOOST_CHECK (copy.exponent() != copy.exponent());
        copy = -w;
        BOOST_CHECK (copy.exponent() != copy.exponent());
    }
}

typedef boost::math::policies::policy<> policy0;
typedef boost::math::policies::policy <
    boost::math::policies::domain_error <boost::math::policies::throw_on_error>,
    boost::math::policies::overflow_error <
        boost::math::policies::throw_on_error>,
    boost::math::policies::underflow_error <
        boost::math::policies::throw_on_error>,
    boost::math::policies::evaluation_error <
        boost::math::policies::throw_on_error>,
    boost::math::policies::indeterminate_result_error <
        boost::math::policies::throw_on_error>
    > policy1;
typedef boost::math::policies::policy <
    boost::math::policies::domain_error <boost::math::policies::ignore_error>,
    boost::math::policies::overflow_error <boost::math::policies::ignore_error>,
    boost::math::policies::underflow_error <
        boost::math::policies::ignore_error>,
    boost::math::policies::evaluation_error <
        boost::math::policies::ignore_error>,
    boost::math::policies::indeterminate_result_error <
        boost::math::policies::ignore_error>
    > policy2;

template <typename RealType, class Policy>
    void test_both_log_float_unary_impl()
{
    test_log_float_unary_impl <RealType, Policy>();
    test_signed_log_float_unary_impl <RealType, Policy>();
}

BOOST_AUTO_TEST_CASE (test_log_float_unary) {
    test_both_log_float_unary_impl <float, policy0>();
    test_both_log_float_unary_impl <double, policy0>();
    if (enable_long_double())
        test_both_log_float_unary_impl <long double, policy0>();

    test_both_log_float_unary_impl <float, policy1>();
    test_both_log_float_unary_impl <double, policy1>();
    if (enable_long_double())
        test_both_log_float_unary_impl <long double, policy1>();

    test_both_log_float_unary_impl <float, policy2>();
    test_both_log_float_unary_impl <double, policy2>();
    if (enable_long_double())
        test_both_log_float_unary_impl <long double, policy2>();
}

// **** Test binary operations ****

#define DEFINE_ARITHMETIC_FUNCTOR( \
    name, symbol, symbol_assignment, description) \
struct name##_assignment { \
    template <typename T1, typename T2> auto operator () ( \
        T1 const & left, T2 const & right) const \
    -> decltype (left symbol right) \
    { \
        auto result = left symbol right; \
        result = left; \
        result symbol_assignment right; \
        return result; \
    } \
}; \
std::ostream & operator << (std::ostream & os, name##_assignment const &) { \
    return os << description << '='; \
} \
struct name { \
    template <typename T1, typename T2> auto operator () ( \
        T1 const & left, T2 const & right) const \
    -> decltype (left symbol right) \
    { \
        return left symbol right; \
    } \
    \
    name##_assignment assignment() const { \
        return name##_assignment(); \
    } \
}; \
std::ostream & operator << (std::ostream & os, name const &) { \
    return os << description; \
}

DEFINE_ARITHMETIC_FUNCTOR(plus, +, +=, "+")
DEFINE_ARITHMETIC_FUNCTOR(minus, -, -=, "-")
DEFINE_ARITHMETIC_FUNCTOR(times, *, *=, "*")
DEFINE_ARITHMETIC_FUNCTOR(divide, /, /=, "/")

// Test indeterminate_result
template <class LogFloatLeft, class LogFloatRight, class Operation>
    void test_log_float_binary_exact_arithmetic_indeterminate_result (
        LogFloatLeft const & left, LogFloatRight const & right,
        Operation operation,
        boost::math::policies::indeterminate_result_error<
            boost::math::policies::throw_on_error> const &)
{
    BOOST_CHECK_THROW (operation (left, right), std::domain_error);
}

template <class LogFloatLeft, class LogFloatRight, class Operation>
    void test_log_float_binary_exact_arithmetic_indeterminate_result (
        LogFloatLeft const & left, LogFloatRight const & right,
        Operation operation, boost::math::policies::indeterminate_result_error <
            boost::math::policies::ignore_error> const &)
{
    BOOST_CHECK (boost::math::isnan (operation (left, right).exponent()));
}

template <class LogFloatLeft, class LogFloatRight, class Operation,
        class Policy>
    void test_log_float_binary_exact_arithmetic_indeterminate_result (
        LogFloatLeft const & left, LogFloatRight const & right,
        Operation operation, Policy const & policy)
{
    test_log_float_binary_exact_arithmetic_indeterminate_result (
        left, right, operation,
        typename Policy::indeterminate_result_error_type());
}

// Test overflow_error
template <class LogFloatLeft, class LogFloatRight, class Operation>
    void test_log_float_binary_exact_arithmetic_overflow (
        LogFloatLeft const & left, LogFloatRight const & right,
        Operation operation, boost::math::policies::overflow_error <
            boost::math::policies::throw_on_error> const &, int)
{
    BOOST_CHECK_THROW (operation (left, right), std::overflow_error);
}

template <class LogFloatLeft, class LogFloatRight, class Operation>
    void test_log_float_binary_exact_arithmetic_overflow (
        LogFloatLeft const & left, LogFloatRight const & right,
        Operation operation, boost::math::policies::overflow_error <
            boost::math::policies::ignore_error> const &, int sign)
{
    auto result = operation (left, right);
    BOOST_CHECK_EQUAL (result.sign(), sign);
    // The exponent should be +inf even if the sign is negative.
    BOOST_CHECK (boost::math::isinf (result.exponent()));
    BOOST_CHECK (result.exponent() > 0);
}

template <class LogFloatLeft, class LogFloatRight, class Operation,
        class Policy>
    void test_log_float_binary_exact_arithmetic_overflow (
        LogFloatLeft const & left, LogFloatRight const & right,
        Operation operation, Policy const & policy, int sign)
{
    test_log_float_binary_exact_arithmetic_overflow (
        left, right, operation,
        typename Policy::overflow_error_type(), sign);
}

// ** Test binary operations on corner cases (exact)
template <typename LogFloatLeft, typename LogFloatRight,
    typename LogFloatResult, typename RealType, class Operation>
void test_log_float_binary_exact_arithmetic_operation (
    RealType left, RealType right, Operation operation)
{
    typename LogFloatLeft::policy_type policy;
    RealType result = operation (left, right);
    LogFloatLeft w_left (left);
    LogFloatRight w_right (right);
    if (boost::math::isnan (result)) {
        if (boost::math::isnan (left) || boost::math::isnan (right)) {
            // No error expected: one of the operands was NaN already.
//            std::cout << left << operation << right << " = " << result
//                << std::endl;
            BOOST_CHECK (boost::math::isnan (
                operation (w_left, w_right).exponent()));
        } else {
//            std::cout << left << operation << right << " = indeterminate"
//                << std::endl;
            test_log_float_binary_exact_arithmetic_indeterminate_result (
                w_left, w_right, operation, policy);
        }
    } else if (boost::math::isinf (result)) {
        if (boost::math::isinf (left) || boost::math::isinf (right)) {
            // No error expected: one of the operands was infinite already.
//            std::cout << left << operation << right << "=" << result
//                << std::endl;
            // The exponent should be +inf even if the sign is negative.
            auto exponent = operation (w_left, w_right).exponent();
            BOOST_CHECK (boost::math::isinf (exponent));
            BOOST_CHECK (exponent > 0);
        } else {
//            std::cout << left << operation << right << " = overflow"
//                << std::endl;
            test_log_float_binary_exact_arithmetic_overflow (
                w_left, w_right, operation, policy, boost::math::sign (result));
        }
    } else {
        // No special case.
        using std::log;
        using std::modf;
        RealType intpart;
        if (modf (log (result), &intpart) != 0) {
//            std::cout << "Not checking inexact log("
//                << left << operation << right << ")" << std::endl;
        } else {
//            std::cout << left << operation << right << " = " << result
//                << std::endl;
            auto test = operation (w_left, w_right);
            BOOST_MPL_ASSERT ((std::is_same <
                decltype (test), LogFloatResult>));
            BOOST_CHECK_EQUAL (RealType (test), result);
        }
    }
}

template <typename LogFloatLeft, typename LogFloatRight,
    typename LogFloatResult, typename RealType, class Operation>
void test_log_float_binary_exact_arithmetic (
    RealType left, RealType right, Operation operation)
{
    test_log_float_binary_exact_arithmetic_operation <
        LogFloatLeft, LogFloatRight, LogFloatResult>
        (left, right, operation);
    test_log_float_binary_exact_arithmetic_operation <
        LogFloatLeft, LogFloatRight, LogFloatResult>
        (left, right, operation.assignment());
}

template <typename LogFloatLeft, typename LogFloatRight, typename RealType>
    void test_log_float_binary_comparison (RealType left, RealType right)
{
    LogFloatLeft w_left (left);
    LogFloatRight w_right (right);

//    std::cout << left << " vs " << right
//        << " -> " << w_left << " vs " << w_right << std::endl;
    BOOST_CHECK_EQUAL (w_left == w_right, left == right);
    BOOST_CHECK_EQUAL (w_left != w_right, left != right);
    BOOST_CHECK_EQUAL (w_left < w_right, left < right);
    BOOST_CHECK_EQUAL (w_left <= w_right, left <= right);
    BOOST_CHECK_EQUAL (w_left > w_right, left > right);
    BOOST_CHECK_EQUAL (w_left >= w_right, left >= right);
}

template <typename RealType, class Policy>
    void test_log_float_binary_exact_with()
{
    typedef math::log_float <RealType, Policy> log_float;
    typedef math::signed_log_float <RealType, Policy> signed_log_float;
    RealType examples[] = {
        -std::numeric_limits <RealType>::infinity(),
        -1, -RealType (0), RealType (0), 1,
        std::numeric_limits <RealType>::infinity(),
        std::numeric_limits <RealType>::quiet_NaN()};
    for (RealType left : examples) {
        for (RealType right : examples) {
//            std::cout << "Testing " << left << " (op) " << right << std::endl;
            if (!boost::math::signbit (left)) {
                if (!boost::math::signbit (right)) {
                    test_log_float_binary_exact_arithmetic <
                        log_float, log_float, log_float> (
                        left, right, plus());
                    test_log_float_binary_exact_arithmetic <
                        log_float, log_float, signed_log_float> (
                        left, right, minus());
                    test_log_float_binary_exact_arithmetic <
                        log_float, log_float, log_float> (
                        left, right, times());
                    test_log_float_binary_exact_arithmetic <
                        log_float, log_float, log_float> (
                        left, right, divide());

                    test_log_float_binary_comparison <log_float, log_float>
                        (left, right);
                }
                test_log_float_binary_exact_arithmetic <
                    log_float, signed_log_float, signed_log_float> (
                    left, right, plus());
                test_log_float_binary_exact_arithmetic <
                    log_float, signed_log_float, signed_log_float> (
                    left, right, minus());
                test_log_float_binary_exact_arithmetic <
                    log_float, signed_log_float, signed_log_float> (
                    left, right, times());
                test_log_float_binary_exact_arithmetic <
                    log_float, signed_log_float, signed_log_float> (
                    left, right, divide());

                test_log_float_binary_comparison <
                    log_float, signed_log_float> (left, right);
            }

            if (!boost::math::signbit (right)) {
                test_log_float_binary_exact_arithmetic <
                    signed_log_float, log_float, signed_log_float> (
                    left, right, plus());
                test_log_float_binary_exact_arithmetic <
                    signed_log_float, log_float, signed_log_float> (
                    left, right, minus());
                test_log_float_binary_exact_arithmetic <
                    signed_log_float, log_float, signed_log_float> (
                    left, right, times());
                test_log_float_binary_exact_arithmetic <
                    signed_log_float, log_float, signed_log_float> (
                    left, right, divide());

                test_log_float_binary_comparison <
                    signed_log_float, log_float> (left, right);
            }

            test_log_float_binary_exact_arithmetic <
                signed_log_float, signed_log_float, signed_log_float> (
                left, right, plus());
            test_log_float_binary_exact_arithmetic <
                signed_log_float, signed_log_float, signed_log_float> (
                left, right, minus());
            test_log_float_binary_exact_arithmetic <
                signed_log_float, signed_log_float, signed_log_float> (
                left, right, times());
            test_log_float_binary_exact_arithmetic <
                signed_log_float, signed_log_float, signed_log_float> (
                left, right, divide());

            test_log_float_binary_comparison <
                signed_log_float, signed_log_float> (left, right);
        }
    }
}

template <typename RealType, class Policy>
    void test_log_float_binary_exact_impl()
{
    test_log_float_binary_exact_with <RealType, Policy> ();
    test_log_float_overflow <RealType, Policy> (Policy());
    test_log_float_underflow <RealType, Policy> (Policy());
}

BOOST_AUTO_TEST_CASE (test_log_float_binary_exact) {
    test_log_float_binary_exact_impl <float, policy0>();
    test_log_float_binary_exact_impl <double, policy0>();
    if (enable_long_double())
        test_log_float_binary_exact_impl <long double, policy0>();

    test_log_float_binary_exact_impl <float, policy1>();
    test_log_float_binary_exact_impl <double, policy1>();
    if (enable_long_double())
        test_log_float_binary_exact_impl <long double, policy1>();

    test_log_float_binary_exact_impl <float, policy2>();
    test_log_float_binary_exact_impl <double, policy2>();
    if (enable_long_double())
        test_log_float_binary_exact_impl <long double, policy2>();
}

// ** Test binary operations on non-corner cases (approximate)

template <typename Result, typename RealType>
    RealType log_float_addition_error (
        Result const & result,
        RealType const & reference, RealType const & log_reference)
{
    if (-1 < result.exponent() && result.exponent() < 1) {
        // Around result = 1, i.e. log(result) = 0, compare the result in the
        // normal domain.
        // The relative error can be very large close to 1!
        using std::abs;
        return boost::math::tools::relative_error (
            abs (RealType (result)), reference);
    } else {
        // Otherwise, compare in the log-domain
//        std::cout << "   Comparing " << result.exponent()
//            << " and " << log_reference << std::endl;
        return boost::math::tools::relative_error (
            result.exponent(), log_reference);
    }
}

template <typename RealType, class Policy>
    void test_log_float_binary_approximate_arithmetic()
{
    using std::exp;

#define PRECISE_(x) static_cast<RealType> (BOOST_JOIN(x, L))
    // These examples were largely made up by the cat.
    // The results were computed with Wolfram Alpha.
    // a, b,
    // exp(a) + exp (b), log (exp(a) + exp (b)),
    // |exp(a) - exp (b)|, log (|exp(a) - exp (b)|),
    RealType plus_examples [][6] = {
        {PRECISE_(-1.4e19), -std::numeric_limits <RealType>::infinity(),
            exp (PRECISE_(-1.4e19)), PRECISE_(-1.4e19),
            exp (PRECISE_(-1.4e19)), PRECISE_(-1.4e19)},
        {-140, -std::numeric_limits <RealType>::infinity(),
            exp (PRECISE_(-140)), -140.,
            exp (PRECISE_(-140)), -140.},
        {-140, -200,
            PRECISE_(1.580420060273612964829318426391938204436932915e-61),
            PRECISE_(-139.999999999999999999999999991243489237303479661511),
            PRECISE_(1.580420060273612964829318398714007669702182302e-61),
            PRECISE_(-140.000000000000000000000000008756510762696520338488)
            },
        {-140, -160,
            PRECISE_(1.580420063531101497036839674944477476015560562e-61),
            PRECISE_(-139.999999997938846379685619296761017201122084764397),
            PRECISE_(1.580420057016124432621797150161468398123554656e-61),
            PRECISE_(-140.000000002061153624562734958530571803231406957688)},
        {PRECISE_(-3.5678923), PRECISE_(-30.34785),
            PRECISE_(0.0282152603569018759015060421750387678343024081905884),
            PRECISE_(-3.56789229999765786380697933879513892454449767561522),
            PRECISE_(0.0282152603567697079365513950683799983069590113319759),
            PRECISE_(-3.56789230000234213619302614680680774561948957683967)},
        {-3, -4,
            PRECISE_(0.0681027072565981232730604369233030188436116597418988),
            PRECISE_(-2.68673831248177716595100450503214435808471991432965),
            PRECISE_(0.0314714294791297626856243943768205344197875246349476),
            PRECISE_(-3.45867514538708189102164364506732970187697790669219)},
        {-0, PRECISE_(-3.5678923),
            PRECISE_(1.0282152603568357919190287186217093830706307097612822),
            PRECISE_(0.0278245423467085797716637124522770518877240555111055),
            PRECISE_(0.9717847396431642080809712813782906169293692902387177),
            PRECISE_(-0.02862096032033151480238638695510963734393335415391)},
        {0, 0,
            2,
            PRECISE_(0.6931471805599453094172321214581765680755001343602552),
            0, -std::numeric_limits <RealType>::infinity()},
        {0, PRECISE_(0.2),
            PRECISE_(2.2214027581601698339210719946396741703075809415205036),
            PRECISE_(0.7981388693815918396849437125412322904934726704225570),
            PRECISE_(0.2214027581601698339210719946396741703075809415205036),
            PRECISE_(-1.50777180097051991408939807874695454280592308466240)},
        {PRECISE_(-0.693147181), PRECISE_(-0.693147181),
            PRECISE_(0.999999999559945309514056186795921728340745886787329),
            PRECISE_(-4.40054690582767878541823431924499865639744745e-10),
            0, -std::numeric_limits <RealType>::infinity()},
        // .50001 + .49999
        {PRECISE_(-0.69312718075994264279056481480217638522155722032953),
        PRECISE_(-0.69316718075994797612389942813551008426917638172635),
            1.,
            PRECISE_(5.920733347391888767015154285169532380396834606e-51),
            PRECISE_(2e-5),
            PRECISE_(-10.81977828441028311067272515196364446993000731)},
        // .50000001 + .49999999: relative to the correct answer in the
        // log-domain, this has very low accuracy.
        {PRECISE_(-0.69314716055994550941722945479154990140819346770425),
        PRECISE_(-0.69314720055994550941723478812488323474280680103758),
            1.,
            PRECISE_(6.920787313342369584098160170792304276025409086e-51),
            PRECISE_(2.e-8),
            PRECISE_(-17.727533563392420162726699516016737092733312)},
        {PRECISE_(-1.20397280432593599262274621776183850295361093080602),
        PRECISE_(-0.35667494393873237891263871124118447796401675904691),
            1.,
            PRECISE_(9.308591346495920018417785321302722551529633968e-51),
            PRECISE_(0.4),
            PRECISE_(-0.91629073187415506518352721176801107145010121990826)},
        {PRECISE_(15.45943079), PRECISE_(14.805086),
            PRECISE_(7.86551076744115337915452144999047007513739687990e6),
            PRECISE_(15.877998034189300616802813958086030634568403259426740),
            PRECISE_(2.48532650777562751212524021954325218971727730192e6),
            PRECISE_(14.725914600340769465502065991614956986380757313210124)},
        {PRECISE_(15.45943079), PRECISE_(10.805086),
            PRECISE_(5.22468939363583315599145570024412099305509409272e6),
            PRECISE_(15.468905907854054863623409848716036621630375292051239),
            PRECISE_(5.12614788158094773528830596928960127179958008910e6),
            PRECISE_(15.449865034803509638761049327921646243733640726807113)},
        {PRECISE_(1545.943079), PRECISE_(10.805086),
            exp (PRECISE_(1545.943079)), PRECISE_(1545.943079),
            exp (PRECISE_(1545.943079)), PRECISE_(1545.943079)}
        };

    // These are not nearly as interesting as the cases for addition
    RealType times_divide_examples [][4] = {
        // loga, logb, log(a*b), log(a/b)
        {PRECISE_(-1.446846513216542154654e19), PRECISE_(-7.334128723654212e13),
            PRECISE_(-1.446853847345265808866e19),
            PRECISE_(-1.446839179087818500442e19)},
        {-140, -200, -340, 60},
        {-200, -140, -340, -60},
        {PRECISE_(-3.5678923456423135), PRECISE_(-30.3478565123215454),
            PRECISE_(-33.9157488579638589),
            PRECISE_(26.7799641666792319)},
        {-3, -4, -7, 1},
        {-0, PRECISE_(-3.567892320168543213574),
            PRECISE_(-3.567892320168543213574),
            PRECISE_(3.567892320168543213574)},
        {0, PRECISE_(7.327), PRECISE_(7.327), PRECISE_(-7.327)},
        {0, PRECISE_(0.2), PRECISE_(0.2), PRECISE_(-0.2)},
        {PRECISE_(5.124875698723897), PRECISE_(-5.124875698723897),
            PRECISE_(0),
            PRECISE_(10.249751397447794)},
        {PRECISE_(5.124875698723897), PRECISE_(5.124875698723897),
            PRECISE_(10.249751397447794),
            PRECISE_(0)},
        {PRECISE_(15.459430795465132465), PRECISE_(14.805086132135454),
            PRECISE_(30.264516927600586465),
            PRECISE_(0.654344663329678465)},
        {PRECISE_(154594307.95465132465), PRECISE_(10.805086),
            PRECISE_(1.5459431875973732465e8),
            PRECISE_(1.5459429714956532465e8)},
        {PRECISE_(154594307.95465132465), PRECISE_(10805086.23458756789),
            (PRECISE_(1.6539939418923889254e8)),
            PRECISE_(1.4378922172006375676e8)}
    };
#undef PRECISE_

    for (auto const & example : plus_examples) {
        math::log_float <RealType, Policy> left (
            example[0], math::as_exponent());
        math::log_float <RealType, Policy> right (
            example[1], math::as_exponent());

/*        std::cout << "" << left << " + " << right;
        try {
            std::cout << " (i.e. " << RealType (left) << " + "
                << RealType (right) << ")";
        } catch (std::exception &) {}*/

        math::log_float <RealType, Policy> result = left + right;

/*        std::cout << " = " << result;
        try {
            std::cout << " (i.e. " << RealType (result) << ")";
        } catch (std::exception &) {}
        std::cout << "?" << std::endl;
        std::cout << "Should be " << example [2] << " and " << example [3]
            << " (in " << typeid (RealType).name() << ")"
            << std::endl;*/

        RealType error =
            log_float_addition_error (result, example [2], example [3])
            / std::numeric_limits <RealType>::epsilon();
//        std::cout << "Error: " << error << " eps." << std::endl;
        BOOST_CHECK (error < 5);

        {
            math::log_float <RealType, Policy> result2 = right + left;
            BOOST_CHECK_EQUAL (result, result2);
        }
        {
            math::log_float <RealType, Policy> result2 = left;
            result2 += right;
            BOOST_CHECK_EQUAL (result, result2);
        }
        {
            math::log_float <RealType, Policy> result2 = right;
            result2 += left;
            BOOST_CHECK_EQUAL (result, result2);
        }

        math::signed_log_float <RealType, Policy> s_left (left);
        math::signed_log_float <RealType, Policy> s_right (right);
        math::signed_log_float <RealType, Policy> n_left (-left);
        math::signed_log_float <RealType, Policy> n_right (-right);
        int difference_sign = (example[0] >= example[1]) ? +1 : -1;

        math::signed_log_float <RealType, Policy> s_result = s_left + s_right;
        BOOST_CHECK_EQUAL (s_result.sign(), +1);
        error = log_float_addition_error (s_result, example [2], example [3])
            / std::numeric_limits <RealType>::epsilon();
        BOOST_CHECK (error < 5);

        {
            math::signed_log_float <RealType, Policy> s_result2
                = n_left + n_right;
            BOOST_CHECK_EQUAL (s_result2, -s_result);
        }

//        std::cout << "  " << left << " - " << right;
        s_result = left - right;
//        std::cout << " = " << s_result << "?"
//            << " (should be " << example [4]
//            << " = e^" << example [5] << ")" << std::endl;
        BOOST_CHECK_EQUAL (s_result.sign(), difference_sign);
        if (left == right && !s_result)
            // The input values are rounded so that the result must be 0.
            error = 0;
        else {
            error =
                log_float_addition_error (s_result, example [4], example [5])
                / std::numeric_limits <RealType>::epsilon();
//            std::cout << "  Error: " << error << " eps." << std::endl;
        }

        // The rounding error on the representation of the operands is
        // 1 / (left - right) already.
        // If (left - right) is small, therefore, it makes no sense to be
        // particular.
        RealType acceptable_error = (std::max) (
            RealType (5), RealType (2 / example [4]));
//        std::cout << "  Acceptable error: " << acceptable_error << std::endl;

        BOOST_CHECK (error < acceptable_error);

        {
            math::signed_log_float <RealType, Policy> s_result2
                = s_left - right;
            BOOST_CHECK_EQUAL (s_result2, s_result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = left - s_right;
            BOOST_CHECK_EQUAL (s_result2, s_result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = s_left - s_right;
            BOOST_CHECK_EQUAL (s_result2, s_result);
        }
        // Assorted tests
        {
            math::signed_log_float <RealType, Policy> s_result2
                = s_left + n_right;
            BOOST_CHECK_EQUAL (s_result2, s_result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = s_left - n_right;
            BOOST_CHECK_EQUAL (s_result2, result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = left - n_right;
            BOOST_CHECK_EQUAL (s_result2, result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = right - left;
            BOOST_CHECK_EQUAL (s_result2, -s_result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = s_right + n_left;
            BOOST_CHECK_EQUAL (s_result2, -s_result);
        }
        {
            math::signed_log_float <RealType, Policy> s_result2
                = s_right - s_left;
            BOOST_CHECK_EQUAL (s_result2, -s_result);
        }
    }

    for (auto const & example : times_divide_examples) {
        math::log_float <RealType, Policy> left (
            example[0], math::as_exponent());
        math::signed_log_float <RealType, Policy> left2 (
            example[0], math::as_exponent());
        math::signed_log_float <RealType, Policy> left3 (
            example[0], -1, math::as_exponent());

        math::log_float <RealType, Policy> right (
            example[1], math::as_exponent());
        math::signed_log_float <RealType, Policy> right2 (
            example[1], math::as_exponent());
        math::signed_log_float <RealType, Policy> right3 (
            example[1], -1, math::as_exponent());

        RealType error, error_eps;

        /*** Multiplication ***/
//        std::cout << "" << left << " * " << right << std::flush;
        math::log_float <RealType, Policy> times_result = left * right;
//        std::cout << " = " << times_result << "?" << std::endl;

        error = boost::math::tools::relative_error (
            times_result.exponent(), example [2]);
        error_eps = error / std::numeric_limits <RealType>::epsilon();
//        std::cout << "Error: " << error << " = "
//            << error_eps << " eps." << std::endl;
//        std::cout << "Type: " << typeid (RealType).name() << std::endl;
        BOOST_CHECK (error_eps < 6);

        math::signed_log_float <RealType, Policy> times_result_signed;
        times_result_signed = left * right;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), +1);

        times_result_signed = left * right2;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), +1);

        times_result_signed = left * right3;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), -1);

        times_result_signed = left2 * right;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), +1);

        times_result_signed = left2 * right2;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), +1);

        times_result_signed = left2 * right3;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), -1);

        times_result_signed = left3 * right;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), -1);

        times_result_signed = left3 * right2;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), -1);

        times_result_signed = left3 * right3;
        BOOST_CHECK_EQUAL (times_result_signed.exponent(),
            times_result.exponent());
        BOOST_CHECK_EQUAL (times_result_signed.sign(), +1);

        /*** Division ***/
//        std::cout << "" << left << " / " << right << std::flush;
        math::log_float <RealType, Policy> divide_result = left / right;
//        std::cout << " = " << divide_result << "?" << std::endl;

        error = boost::math::tools::relative_error (
            divide_result.exponent(), example [3]);
        error_eps = error / std::numeric_limits <RealType>::epsilon();
//        std::cout << "Error: " << error << " = "
//            << error_eps << " eps." << std::endl;
        BOOST_CHECK (error_eps < 6);

        math::signed_log_float <RealType, Policy> divide_result_signed;
        divide_result_signed = left / right;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), +1);

        divide_result_signed = left / right2;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), +1);

        divide_result_signed = left / right3;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), -1);

        divide_result_signed = left2 / right;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), +1);

        divide_result_signed = left2 / right2;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), +1);

        divide_result_signed = left2 / right3;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), -1);

        divide_result_signed = left3 / right;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), -1);

        divide_result_signed = left3 / right2;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), -1);

        divide_result_signed = left3 / right3;
        BOOST_CHECK_EQUAL (divide_result_signed.exponent(),
            divide_result.exponent());
        BOOST_CHECK_EQUAL (divide_result_signed.sign(), +1);
    }
}

BOOST_AUTO_TEST_CASE (test_log_float_binary_approximate) {
    test_log_float_binary_approximate_arithmetic <float, policy0> ();
    test_log_float_binary_approximate_arithmetic <double, policy0> ();
    if (enable_long_double())
        test_log_float_binary_approximate_arithmetic <long double, policy0> ();

    test_log_float_binary_approximate_arithmetic <float, policy1> ();
    test_log_float_binary_approximate_arithmetic <double, policy1> ();
    if (enable_long_double())
        test_log_float_binary_approximate_arithmetic <long double, policy1> ();

    test_log_float_binary_approximate_arithmetic <float, policy2> ();
    test_log_float_binary_approximate_arithmetic <double, policy2> ();
    if (enable_long_double())
        test_log_float_binary_approximate_arithmetic <long double, policy2> ();
}

// **** Test functions that mimic the ones from <cmath> ****

template <typename RealType, class Policy> void test_log_float_exp_() {
    // Use the default policy.
    typedef math::log_float <RealType/*, Policy*/> log_float;
    log_float w1 (3, math::as_exponent());
    auto w2 = math::exp_ (RealType (3));
    BOOST_MPL_ASSERT ((std::is_same <decltype (w1), decltype (w2)>));
    BOOST_CHECK_EQUAL (w1, w2);
}

template <typename RealType, class Policy> void test_log_float_pow() {
    // Attempt to confuse the compiler.
    using std::pow;
    RealType infinity = std::numeric_limits <RealType>::infinity();
    math::log_float <RealType, Policy> w (0);
    BOOST_CHECK_EQUAL (RealType (pow (w, RealType (0))), pow (0., 0.));
    BOOST_CHECK_EQUAL (RealType (pow (w, RealType (1))), pow (0., 1.));
    BOOST_CHECK_EQUAL (RealType (pow (w, infinity)), pow (0., infinity));

    w = 1;
    BOOST_CHECK_EQUAL (RealType (pow (w, RealType (0))), pow (1., 0.));
    BOOST_CHECK_EQUAL (RealType (pow (w, RealType (1))), pow (1., 1.));
    BOOST_CHECK_EQUAL (RealType (pow (w, infinity)), pow (1., infinity));

    w = 1;
    BOOST_CHECK_EQUAL (pow (w, RealType (2)), w * w);
    w = 2;
    BOOST_CHECK_EQUAL (pow (w, RealType (2)), w * w);
    w = 700;
    BOOST_CHECK_EQUAL (pow (w, RealType (2)), w * w);

    using std::log;
    BOOST_CHECK_EQUAL (pow (w, RealType (2.5)).exponent(),
        log (RealType (700)) * RealType (2.5));
    BOOST_CHECK_EQUAL (pow (w, RealType (-2.5)).exponent(),
        log (RealType (700)) * RealType (-2.5));

    // Conversion when taking the power of a double.
    w = 2;
    auto e2 = pow (w, 2.).exponent();
    auto d2 = RealType (2) * 2.;
    BOOST_MPL_ASSERT ((std::is_same <decltype (e2), decltype (d2)>));
}

template <typename RealType, class Policy> void test_log_float_log() {
    using std::log;
    RealType infinity = std::numeric_limits <RealType>::infinity();
    typedef math::log_float <RealType, Policy> log_float;
    BOOST_CHECK_EQUAL (log (log_float (0)), -infinity);
    BOOST_CHECK_EQUAL (log (log_float (1)), 0);
    BOOST_CHECK_EQUAL (log (log_float (infinity)), infinity);
    BOOST_CHECK_EQUAL (log (log_float (RealType (5.5))), log (RealType (5.5)));
}

template <typename RealType, class Policy> void test_log_float_exp() {
    using std::exp;
    RealType infinity = std::numeric_limits <RealType>::infinity();
    typedef math::log_float <RealType, Policy> log_float;
    BOOST_CHECK_EQUAL (exp (log_float (0)), log_float (1));
    BOOST_CHECK_EQUAL (exp (log_float (1)).exponent(), 1);
    BOOST_CHECK_EQUAL (exp (log_float (infinity)).exponent(), infinity);
    BOOST_CHECK_CLOSE (RealType (exp (log_float (RealType (4)))),
        exp (RealType (4)), std::numeric_limits <RealType>::epsilon() * 5);

    typedef math::signed_log_float <RealType, Policy> signed_log_float;
    BOOST_CHECK_CLOSE (RealType (exp (signed_log_float (RealType (-4)))),
        exp (RealType (-4)),
        std::numeric_limits <RealType>::epsilon() * 5);
    BOOST_CHECK_EQUAL (exp (signed_log_float (-1)).exponent(), -1);
    BOOST_CHECK_EQUAL (exp (signed_log_float (0)), log_float (1));
    BOOST_CHECK_EQUAL (exp (signed_log_float (1)).exponent(), 1);
    BOOST_CHECK_EQUAL (exp (signed_log_float (infinity)).exponent(), infinity);

}

template <typename RealType, class Policy>
    void test_log_float_sqrt()
{
    using std::sqrt;
    RealType infinity = std::numeric_limits <RealType>::infinity();
    typedef math::log_float <RealType, Policy> log_float;
    BOOST_CHECK_EQUAL (RealType (sqrt (log_float (0))), sqrt (0));
    BOOST_CHECK_EQUAL (RealType (sqrt (log_float (1))), sqrt (1));
    BOOST_CHECK_EQUAL (RealType (sqrt (log_float (infinity))), sqrt (infinity));

    using std::log;
    BOOST_CHECK_EQUAL (
        sqrt (log_float (RealType (4))).exponent(), log (RealType (4))/2);
}

template <typename RealType, class Policy>
    void test_log_float_functions_impl()
{
    test_log_float_exp_ <RealType, Policy>();
    test_log_float_pow <RealType, Policy>();
    test_log_float_log <RealType, Policy>();
    test_log_float_exp <RealType, Policy>();
    test_log_float_sqrt <RealType, Policy>();
}

BOOST_AUTO_TEST_CASE (test_log_float_functions) {
    test_log_float_functions_impl <float, policy0>();
    test_log_float_functions_impl <double, policy0>();
    if (enable_long_double())
        test_log_float_functions_impl <long double, policy0>();

    test_log_float_functions_impl <float, policy1>();
    test_log_float_functions_impl <double, policy1>();
    if (enable_long_double())
        test_log_float_functions_impl <long double, policy1>();

    test_log_float_functions_impl <float, policy2>();
    test_log_float_functions_impl <double, policy2>();
    if (enable_long_double())
        test_log_float_functions_impl <long double, policy2>();
}

// **** Test conversion and interaction ****

void test_log_float_conversion() {
    // *** log_float and signed_log_float
    // Implicit conversion log_float to signed_log_float.
    BOOST_MPL_ASSERT ((std::is_convertible <
        math::log_float <double, policy0>,
        math::signed_log_float <double, policy0> >));
    BOOST_MPL_ASSERT ((std::is_convertible <
        math::log_float <double, policy1>,
        math::signed_log_float <double, policy1> >));
    BOOST_MPL_ASSERT ((std::is_convertible <
        math::log_float <double, policy2>,
        math::signed_log_float <double, policy2> >));

    // Converting signed_log_float to log_float requires explicit conversion.
    BOOST_MPL_ASSERT_NOT ((std::is_convertible <
        math::signed_log_float <double>, math::log_float <double> >));
    // These two do not work.
    //math::log_float <double> w2 = sw;
    //w = sw;

    // These do.
    {
        math::signed_log_float <double> sw (2);
        math::log_float <double> w (sw);
        math::log_float <double> w3 = math::log_float <double> (sw);
        BOOST_CHECK_EQUAL (w3.exponent(), log (double (2)));
    }

    // *** Different ExponentType: convertible
    BOOST_MPL_ASSERT ((std::is_convertible <
        math::log_float <float, policy0>, math::log_float <double, policy0>>));
    BOOST_MPL_ASSERT ((std::is_convertible <
        math::signed_log_float <float, policy0>,
            math::signed_log_float <double, policy0> >));
    {
        math::log_float <float, policy0> w1 (7);
        math::log_float <double, policy0> w2 = w1;
        BOOST_CHECK_EQUAL (w2.exponent(), w1.exponent());

        math::signed_log_float <float, policy0> sw1 (-3);
        math::signed_log_float <double, policy0> sw2 = sw1;
        BOOST_CHECK_EQUAL (sw2.exponent(), sw1.exponent());
        BOOST_CHECK_EQUAL (sw2.sign(), -1);
    }

    // *** Different policies: explicitly convertible
    BOOST_MPL_ASSERT_NOT ((std::is_convertible <
        math::log_float <double, policy0>, math::log_float <double, policy1>>));
    BOOST_MPL_ASSERT_NOT ((std::is_convertible <
        math::log_float <double, policy1>, math::log_float <double, policy0>>));
    BOOST_MPL_ASSERT_NOT ((std::is_convertible <
        math::signed_log_float <double, policy0>,
            math::signed_log_float <double, policy1> >));
    BOOST_MPL_ASSERT ((std::is_convertible <
        math::log_float <double, policy0>,
            math::signed_log_float <double, policy0> >));
    BOOST_MPL_ASSERT_NOT ((std::is_convertible <
        math::log_float <double, policy0>,
            math::signed_log_float <double, policy1> >));

    {
        math::log_float <double, policy0> wp0 (
            std::numeric_limits <double>::quiet_NaN());
        // Does not throw
        math::log_float <double, policy1> wp1 (wp0);
        BOOST_CHECK (wp1.exponent() != wp1.exponent());
        wp0 = math::log_float <double, policy0> (wp1);

        math::signed_log_float <double, policy0> swp0 (1.);
        math::signed_log_float <double, policy1> swp1 (swp0);
        BOOST_CHECK_EQUAL (swp1.sign(), +1);
        BOOST_CHECK_EQUAL (swp1.exponent(), 0.);
        swp0 = math::signed_log_float <double, policy0> (swp1);
        BOOST_CHECK_EQUAL (swp0.sign(), +1);
        BOOST_CHECK_EQUAL (swp0.exponent(), 0.);
        // Different ExponentType
        if (enable_long_double()) {
            math::signed_log_float <long double, policy1> swp2 (swp0);
            BOOST_CHECK_EQUAL (swp2.sign(), +1);
            BOOST_CHECK_EQUAL (swp2.exponent(), 0.);
        }
    }

    // *** Conversion from ints
    BOOST_MPL_ASSERT ((std::is_convertible <
        int, math::log_float <double> >));
    {
        math::log_float <double> w (1);
        BOOST_CHECK_EQUAL (w.exponent(), 0);

        math::signed_log_float <double> sw (1);
        BOOST_CHECK_EQUAL (sw.exponent(), 0);
    }
}

/**
Test the interaction with ints and floats, both for arithmetic and
comparison.
These tests are not particularly exhaustive.
They use the knowledge that the implementation of these is by converting the
non-log-float operand to signed_log_float and then calling operations tested
above.
Therefore, only the basics are tested.
It is assumed that if the policy is retained, the behaviour in the face of
unexpected events will be correct.
*/
void test_log_float_operator_scalar() {
    math::log_float <double, policy1> w;

    // Operation-assignment
    {   // log_float
        math::log_float <double, policy1> w1;
        w1 += 1;
        BOOST_CHECK_EQUAL (w1.exponent(), 0);
        w1 *= 3;
        using std::log;
        BOOST_CHECK_EQUAL (w1.exponent(), log (3.));
        w1 /= 3.f;
        // The float is first converted to double and then the division
        // (subtraction, really) takes place.
        BOOST_CHECK_EQUAL (w1.exponent(), 0);
    }
    {   // signed_log_float
        math::signed_log_float <double, policy1> w1;
        w1 += 1;
        BOOST_CHECK_EQUAL (w1.exponent(), 0);
        w1 *= 3;
        using std::log;
        BOOST_CHECK_EQUAL (w1.exponent(), log (3.));
        w1 /= 3.f;
        // The float is first converted to double and then the division
        // (subtraction, really) takes place.
        BOOST_CHECK_EQUAL (w1.exponent(), 0);
        w1 -= 1.;
        BOOST_CHECK (!w1);
    }

    // Operations
    {
        auto one = w + 1;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (one), math::signed_log_float <double, policy1> >));
        BOOST_CHECK_EQUAL (one.exponent(), 0);
    }
    {
        auto one = 1 + w;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (one), math::signed_log_float <double, policy1> >));
        BOOST_CHECK_EQUAL (one.exponent(), 0);
    }
    w = 1;
    BOOST_CHECK_EQUAL (w.exponent(), 0);
    {
        auto zero = -1 + w;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (zero), math::signed_log_float <double, policy1> >));
        BOOST_CHECK (!zero);
    }
    {
        auto zero = 1 + (-w);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (zero), math::signed_log_float <double, policy1> >));
        BOOST_CHECK (!zero);
    }

    if (enable_long_double()) {
        auto zero = w - 1.l;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (zero), math::signed_log_float <long double, policy1> >));
        BOOST_CHECK (!zero);
    }
    {
        auto zero = 1l - w;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (zero), math::signed_log_float <double, policy1> >));
        BOOST_CHECK (!zero);
    }

    w = 3;
    if (enable_long_double()) {
        auto nine = 3.l * w;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (nine), math::signed_log_float <long double, policy1> >));
        using std::log;
        BOOST_CHECK_EQUAL (nine.exponent(), log (3.) + log (3.l));
    }
    {
        auto one = 3 / w;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (one), math::signed_log_float <double, policy1> >));
        BOOST_CHECK_EQUAL (one.exponent(), 0);
    }
    {
        auto one = 3. / (-w);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (one), math::signed_log_float <double, policy1> >));
        BOOST_CHECK_EQUAL (one.exponent(), 0);
    }

    test_log_float_binary_comparison <
        math::log_float <double, policy1>, int> (0, 1);
    test_log_float_binary_comparison <
        int, math::log_float <double, policy1> > (0, 5);
    test_log_float_binary_comparison <
        math::signed_log_float <double, policy1>, int> (-5, 1);
    test_log_float_binary_comparison <
        math::signed_log_float <double, policy1>, int> (-5, -5);
    test_log_float_binary_comparison <
        math::log_float <double, policy1>, int> (5, -4);
    test_log_float_binary_comparison <
        int, math::signed_log_float <double, policy1> > (0, -4);

    test_log_float_binary_comparison <
        float, math::log_float <double, policy1> > (-.3f, 4.342f);
    test_log_float_binary_comparison
        <math::log_float <double, policy1>, float> (.3f, -4.342f);
    test_log_float_binary_comparison <
        math::signed_log_float <double, policy1>, float> (-.3f, -4.342f);
}

BOOST_AUTO_TEST_CASE (test_log_float_interaction) {
    test_log_float_conversion();
    test_log_float_operator_scalar();
}

BOOST_AUTO_TEST_SUITE_END()

