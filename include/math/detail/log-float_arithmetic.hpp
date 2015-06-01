/*
Copyright 2012, 2013 Rogier van Dalen.

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
Arithmetic operations on values stored as logarithms.
These also take Boost.Math policies with regard to error handling into account.
*/

#ifndef MATH_DETAIL_LOG_FLOAT_ARITHMETIC_HPP
#define MATH_DETAIL_LOG_FLOAT_ARITHMETIC_HPP

#include <cmath>
#include <cassert>
#include <limits>

#include <boost/static_assert.hpp>
#include <boost/math/policies/policy.hpp>
#include <boost/math/tools/promotion.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include "./log-float_fwd.hpp"

namespace math { namespace detail {

    /*
    std::log1p gives exactly the behaviour we need: log1p (-1) should
    evaluate to -infinity, which indicates a stored value of 0.
    A policy could be used with boost::math::log1p, but this is slower.
    */
    using std::log1p;
    using std::exp;
    using std::log;

    /**
    Multiply two numbers represented as their logs.
    This is equivalent to adding them.
    However, this takes care to produce the correct errors where applicable.
    */
    template <class RealType1, class RealType2, class Policy,
            class OverflowPolicy, class IndeterminateResultPolicy>
        inline typename promote_args <RealType1, RealType2>::type
            multiply_log_float (RealType1 const & loga, RealType2 const & logb,
            Policy const & policy,
            OverflowPolicy const &, IndeterminateResultPolicy const &)
    {
        static char const * function_name = "multiplication of log_float<%1%>";
        typedef typename promote_args <RealType1, RealType2>::type
            result_type;
        static constexpr RealType1 a_infinity =
            std::numeric_limits <RealType1>::infinity();
        static constexpr RealType2 b_infinity =
            std::numeric_limits <RealType2>::infinity();
        static constexpr result_type result_infinity =
            std::numeric_limits <result_type>::infinity();
        static constexpr result_type result_nan =
            std::numeric_limits <result_type>::quiet_NaN();

        // This explicitly provides behaviour that IEEE standard floating
        // point numbers should provide.
        if (isnan (loga) || isnan (logb))
            return result_nan;
        else if (isinf (loga) || isinf (logb))
        {
            if (isinf (loga) && isinf (logb))
            {
                if (loga == a_infinity && logb == b_infinity) {
                    // +inf * +inf = +inf.
                    // Any overflow has happened before this operation.
                    return result_infinity;
                } else if (loga == -a_infinity && logb == -b_infinity) {
                    // 0 * 0 = 0.
                    // Any underflow has happened before this operation.
                    return - result_infinity;
                } else {
                    // loga and logb are of opposite signs.
                    return raise_indeterminate_result_error (
                        function_name, "%1% * inf is undefined",
                        result_type(), result_nan, Policy());
                }
            } else {
                // Either loga or logb is infinity,
                // either positive or negative.
                if (loga == -a_infinity || logb == -b_infinity)
                    // 0 * (finite value) = 0
                    return - result_infinity;
                else {
                    assert (loga == a_infinity || logb == b_infinity);
                    // +inf * (finite value) = +inf
                    return result_infinity;
                }
            }
        } else {
            result_type result = loga + logb;
            if (isinf (result)) {
                if (result == -result_infinity) {
                    raise_underflow_error <result_type> (function_name,
                        "Result of multiplication has underflowed",
                        Policy());
                } else {
                    assert (result == result_infinity);
                    raise_overflow_error <result_type> (function_name,
                        "Result of multiplication has overflowed",
                        Policy());
                }
            }
            return result;
        }
    }

    /**
    Multiply two numbers represented as their logs.
    This is equivalent to adding them.
    This implementation is used only when errors are ignored.
    It may be faster than the explicit implementation.
    */
    template <class RealType1, class RealType2, class Policy>
        inline typename promote_args <RealType1, RealType2>::type
            multiply_log_float (RealType1 const & loga, RealType2 const & logb,
                Policy const &,
                boost::math::policies::overflow_error<
                    boost::math::policies::ignore_error> const &,
                boost::math::policies::indeterminate_result_error<
                    boost::math::policies::ignore_error> const &)
    {
        return loga + logb;
    }

    /**
    Multiply two numbers represented as their logs.
    */
    template <class RealType1, class RealType2, class Policy>
        inline typename promote_args <RealType1, RealType2>::type
            multiply_log_float (RealType1 const & loga, RealType2 const & logb,
                Policy const & policy)
    {
        // Forward to an appropriate implementation depending on the
        // error policy.
        return multiply_log_float (loga, logb, policy,
            typename Policy::overflow_error_type(),
            typename Policy::indeterminate_result_error_type());
    }

    /**
    Divide two numbers represented as their logs.
    This is equivalent to subtraction.
    However, this takes care to produce the correct errors where applicable.
    */
    template <class NumeratorType, class DenominatorType, class Policy,
            class OverflowPolicy, class UnderflowPolicy,
            class IndeterminateResultPolicy>
        inline typename promote_args <NumeratorType, DenominatorType>::type
            divide_log_float (NumeratorType const & log_numerator,
                DenominatorType const & log_denominator,
                Policy const & policy,
                OverflowPolicy const &, UnderflowPolicy const &,
                IndeterminateResultPolicy const &)
    {
        static char const * function_name = "division of log_float<%1%>";
        typedef typename promote_args <NumeratorType, DenominatorType>::type
            result_type;
        static constexpr NumeratorType numerator_infinity =
            std::numeric_limits <NumeratorType>::infinity();
        static constexpr DenominatorType denominator_infinity =
            std::numeric_limits <DenominatorType>::infinity();
        static constexpr result_type result_infinity =
            std::numeric_limits <result_type>::infinity();
        static constexpr result_type result_nan =
            std::numeric_limits <result_type>::quiet_NaN();

        // This explicitly provides behaviour that IEEE standard floating
        // point numbers should provide.
        if (isnan (log_numerator) || isnan (log_denominator))
            return result_nan;
        else if (isinf (log_numerator) || isinf (log_denominator)) {
            if (log_denominator == -denominator_infinity) {
                if (log_numerator == -numerator_infinity) {
                    return raise_indeterminate_result_error (
                        function_name, "%1% / 0 is undefined",
                        result_type(), result_nan, Policy());
                } else {
                    if (log_numerator == numerator_infinity)
                        // inf / 0 = inf: not really overflow,
                        // just propagation of infinity.
                        return result_infinity;
                    else
                        return raise_overflow_error <result_type> (
                            function_name, "division by zero causes overflow",
                            Policy());
                }
            } else if (log_denominator == denominator_infinity) {
                if (log_numerator == numerator_infinity) {
                    return raise_indeterminate_result_error (
                        function_name, "%1% / %1% is undefined",
                        result_type (log_numerator), result_nan, Policy());
                } else {
                    // (finite value) / inf = 0.
                    // The overflow has already happened, so this is not
                    // classified as underflow.
                    return -result_infinity;
                }
            } else {
                assert (!isinf (log_denominator));
                assert (isinf (log_numerator));
                if (log_numerator == -numerator_infinity)
                    return -result_infinity;
                else {
                    assert (log_numerator == numerator_infinity);
                    return result_infinity;
                }
            }
        } else {
            result_type result = log_numerator - log_denominator;
            if (isinf (result)) {
                if (result == -result_infinity
                        && log_numerator != -numerator_infinity) {
                    raise_underflow_error <result_type> (function_name,
                        "Result of division has underflowed",
                        Policy());
                } else {
                    assert (result == result_infinity);
                    if (log_numerator != numerator_infinity)
                        raise_overflow_error <result_type> (function_name,
                            "Result of division has overflowed",
                            Policy());
                }
            }
            return result;
        }
    }

    /**
    Divide two numbers represented as their logs.
    This is equivalent to subtraction.
    This implementation is used only when errors are ignored.
    It may be faster than the explicity implementation.
    */
    template <class NumeratorType, class DenominatorType, class Policy>
        inline typename promote_args <NumeratorType, DenominatorType>::type
            divide_log_float (NumeratorType const & log_numerator,
                DenominatorType const & log_denominator,
                Policy const &,
                boost::math::policies::overflow_error<
                    boost::math::policies::ignore_error> const &,
                boost::math::policies::underflow_error<
                    boost::math::policies::ignore_error> const &,
                boost::math::policies::indeterminate_result_error<
                    boost::math::policies::ignore_error> const &)
    { return log_numerator - log_denominator; }

    /**
    Divide two numbers represented as their logs.
    This is equivalent to subtraction.
    */
    template <class NumeratorType, class DenominatorType, class Policy>
        inline typename promote_args <NumeratorType, DenominatorType>::type
            divide_log_float (NumeratorType const & log_numerator,
                DenominatorType const & log_denominator,
                Policy const & policy)
    {
        // Forward to an appropriate implementation depending on the
        // error policy.
        return divide_log_float (log_numerator, log_denominator, policy,
            typename Policy::overflow_error_type(),
            typename Policy::underflow_error_type(),
            typename Policy::indeterminate_result_error_type());
    }

    /**
    Add two numbers represented as their logs.
    This takes care to produce the correct errors where applicable.
    */
    template <class RealType1, class RealType2,
            class Policy, class OverflowPolicy>
        inline typename promote_args <RealType1, RealType2>::type
            add_log_float (RealType1 const & loga, RealType2 const & logb,
            Policy const & policy, OverflowPolicy const &)
    {
        static char const * function_name = "addition of log_float<%1%>";
        typedef typename promote_args <RealType1, RealType2>::type
            result_type;
        static constexpr RealType1 a_infinity =
            std::numeric_limits <RealType1>::infinity();
        static constexpr RealType2 b_infinity =
            std::numeric_limits <RealType2>::infinity();
        static constexpr result_type result_infinity =
            std::numeric_limits <result_type>::infinity();
        static constexpr result_type result_nan =
            std::numeric_limits <result_type>::quiet_NaN();

        // This explicitly provides behaviour that IEEE standard floating
        // point numbers should provide.
        if (isnan (loga) || isnan (logb))
            return result_nan;
        else if (isinf (loga) && isinf (logb)) {
            if (loga == -a_infinity && logb == -b_infinity)
                return -result_infinity;
            else {
                assert (loga == a_infinity || logb == b_infinity);
                // 0 + inf = inf
                // inf + inf = inf
                return result_infinity;
            }
        } else {
            result_type result;
            if (loga > logb)
                result = loga + log1p (exp (logb - loga));
            else
                result = logb + log1p (exp (loga - logb));

            if (result == result_infinity &&
                    loga != a_infinity && logb != b_infinity) {
                raise_overflow_error <result_type> (function_name,
                    "Result of addition has overflowed",
                    Policy());
            }
            return result;
        }
    }

    /**
    Add two numbers represented as their logs.
    This implementation, for policies that ignore errors, is somewhat
    faster.
    */
    template <class RealType1, class RealType2, class Policy>
        inline typename promote_args <RealType1, RealType2>::type
            add_log_float (RealType1 const & loga, RealType2 const & logb,
                Policy const & policy,
                boost::math::policies::overflow_error <
                    boost::math::policies::ignore_error> const &)
    {
        typedef typename promote_args <RealType1, RealType2>::type
            result_type;

        /*
        A normal implementation of addition in the log-domain uses,
        (assuming, without loss of generalisation, that loga >= logb):
        1)  log (a + b) = loga + log (1 + exp (logb - loga))
            where implementing log (1 + ...) with log1p yields marginal
            improvements.
        Corner cases are NaNs and infinities.
        The following implementation aims to use as few cases as possible
        by allowing most corner cases to be dealt with by the IEEE floating
        point operations:
        2)  isnan (loga) || isnan (logb):
            Any computation that involves both loga and logb, such as the
            one for case (1), returns NaN, which is correct.
        Infinities (assume again that loga >= logb):
        3)  If (loga == logb), either both +infinity, or both -infinity:
            logb - loga = NaN
            and the result will be NaN, which is incorrect.
            This requires a special case.
            A solution is to return loga or logb.
        4)  Otherwise, (loga == +infinity || logb == -infinity):
            The default implementation returns loga:
                logb - loga = -infinity
                exp (logb - loga) = 0
                log1p (0) = 0
                loga + 0 = loga
            which is correct.
            No special handling is required.
        Note, however, that for loga >= logb, (3) and (4) are handled by
            return loga
        and vice versa.
        */
        using std::abs;
        using std::max;

        if (!::boost::math::isinf (loga)) {
            result_type difference = -abs (logb - loga);
            result_type greatest = (max) (loga, logb);
            return greatest + log1p (exp (difference));
        } else {
            if (!(loga >= logb))
                return result_type (logb);
            else
                return result_type (loga);
        }
    }

    /**
    Add two numbers represented as their logs.
    */
    template <class RealType1, class RealType2, class Policy>
        inline typename promote_args <RealType1, RealType2>::type
            add_log_float (RealType1 const & loga, RealType2 const & logb,
            Policy const & policy)
    {
        return add_log_float (loga, logb, policy,
            typename Policy::overflow_error_type());
    }

    /**
    \pre !(loga < logb)
        (This is not the same as loga >= logb in the face of NaNs.)
    */
    template <class RealType1, class RealType2, class Policy,
            class UnderflowPolicy, class IndeterminateResultPolicy>
        inline typename promote_args <RealType1, RealType2>::type
            subtract_log_float (RealType1 const & loga, RealType2 const & logb,
            Policy const & policy,
            UnderflowPolicy const &, IndeterminateResultPolicy const &)
    {
        assert (!(loga < logb));
        static char const * function_name = "subtraction of log_float<%1%>";
        typedef typename promote_args <RealType1, RealType2>::type
            result_type;
        static constexpr RealType1 a_infinity =
            std::numeric_limits <RealType1>::infinity();
        static constexpr RealType2 b_infinity =
            std::numeric_limits <RealType2>::infinity();
        static constexpr result_type result_infinity =
            std::numeric_limits <result_type>::infinity();
        static constexpr result_type result_nan =
            std::numeric_limits <result_type>::quiet_NaN();

        // This explicitly provides behaviour that IEEE standard floating
        // point numbers should provide.
        if (isnan (loga) || isnan (logb))
            return result_nan;
        else if (loga == a_infinity) {
            if (logb == b_infinity) {
                return raise_indeterminate_result_error (
                    function_name, "%1% - inf is undefined",
                    result_infinity, result_nan, policy);
            } else
                // Any overflow has taken place already.
                return result_infinity;
        } else if (loga == logb) {
            return -result_infinity;
        } else {
            result_type result;
            result = loga + log1p (-exp (logb - loga));

            if (result == - result_infinity) {
                raise_underflow_error <result_type> (function_name,
                    "Result of subtraction has underflowed",
                    Policy());
            }
            return result;
        }
    }

    template <class RealType1, class RealType2, class Policy>
        inline typename promote_args <RealType1, RealType2>::type
            subtract_log_float (RealType1 const & loga, RealType2 const & logb,
            Policy const & policy,
                boost::math::policies::underflow_error<
                    boost::math::policies::ignore_error> const &,
                boost::math::policies::indeterminate_result_error<
                    boost::math::policies::ignore_error> const &)
    {
        assert (!(loga < logb));
        if (loga == -std::numeric_limits <RealType1>::infinity())
            // logb == -infinity or nan.
            return loga + logb;
        else
            return loga + log1p (-exp (logb - loga));
    }

    /**
    \pre !(loga < logb)
        (This is not the same as loga >= logb in the face of NaNs.)
    */
    template <class RealType1, class RealType2, class Policy>
        inline typename promote_args <RealType1, RealType2>::type
            subtract_log_float (RealType1 const & loga, RealType2 const & logb,
            Policy const & policy)
    {
        return subtract_log_float (loga, logb, policy,
            typename Policy::underflow_error_type(),
            typename Policy::indeterminate_result_error_type());
    }

    template <class RealType1, class RealType2, class Policy>
        inline std::pair <
            typename promote_args <RealType1, RealType2>::type, int>
        add_signed_log_float (RealType1 const & loga, int sign_a,
            RealType2 const & logb, int sign_b,
            Policy const & policy)
    {
        if (sign_a == sign_b)
            return std::make_pair (add_log_float (loga, logb, policy), sign_a);
        else {
            if (loga >= logb)
                return std::make_pair (
                    subtract_log_float (loga, logb, policy), sign_a);
            else
                return std::make_pair (
                    subtract_log_float (logb, loga, policy), sign_b);
        }
    }

}} // namespace math::detail

#endif  // MATH_DETAIL_LOG_FLOAT_ARITHMETIC_HPP

