/*
Copyright 2012, 2013-2015 Rogier van Dalen.

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
Define the log_float and signed_log_float classes.
*/

#ifndef MATH_LOG_FLOAT_HPP_INCLUDED
#define MATH_LOG_FLOAT_HPP_INCLUDED

#include <cmath>
#include <cassert>
#include <iosfwd>
#include <stdexcept>
#include <limits>
// For std::min
#include <algorithm>
// For std::pair and std::tie
#include <utility>
#include <type_traits>

#include <boost/utility/enable_if.hpp>
#include <boost/functional/hash_fwd.hpp>

#include "detail/log-float_fwd.hpp"
#include "detail/log-float_base.hpp"
#include "detail/log-float_arithmetic.hpp"
#include "detail/log-float_numeric_limits.hpp"

namespace math {

    namespace detail {
        template <typename Exponent> struct valid_exponent_type {
            constexpr static bool value =
                std::numeric_limits <Exponent>::is_specialized ||
                !std::numeric_limits <Exponent>::is_integer ||
                std::numeric_limits <Exponent>::has_infinity;
        };
    }   // namespace detail

    /** \brief
    A non-negative number that can be very close to zero or very large.
    It is stored as the logarithm of its value.

    Floating-point numbers use a fixed-point significand and an integer
    exponent.
    This class, on the other hand, uses a significand fixed to 1, and a
    floating-point exponent, Exponent.

    To store the value 0, \f$ -\infty \f$ is used.

    To store a number that can be positive or negative, see signed_log_float.
    It is possible to interact with signed_log_float.
    Operations will return signed_log_float if the result can be negative, and
    log_float otherwise.

    The Policy template argument is a Boost.Math policy for error handling.
    The policy is used for error handling in operations.
    Objects of this class that use different policies are not implicitly
    compatible and need to be converted explicitly first.

    Some free functions that are available for floating-point numbers are
    provided: exp(),  log(), pow(),  sqrt().
    exp_() is a shorthand that constructs a log_float from a float.

    Unlike for floating-point numbers, functions that use a break-down in
    integer and fractional parts are not available, nor are trigonometric
    functions.

    log_float supports Boost.Hash if \c boost/functional/hash.hpp is included.
    The hash value for a log_float is the same as the hash value of the
    signed_log_float with the same value.

    \tparam Exponent The type to use to hold the exponent.
    \tparam Policy The Boost.Math policy that defines behaviour under domain
    errors, underflow errors, and overflow errors.

    \sa signed_log_float
    */
    template <class Exponent, class Policy> class log_float
    : public detail::log_float_base <log_float <Exponent, Policy>, Exponent>
    {
        Exponent exponent_;
    public:
        /// \brief The type of the underlying exponent.
        typedef Exponent exponent_type;
        /// \brief The error policy.
        typedef Policy policy_type;

        /// \cond DONT_DOCUMENT
        static_assert (detail::valid_exponent_type <Exponent>::value,
            "log_float cannot be instantiated with this type.");
        /// \endcond

        /// Construct with the value 0.
        log_float() : exponent_ (-std::numeric_limits<Exponent>::infinity()) {}

        /// Construct with value \a p.
        log_float (Exponent p) {
            if (boost::math::sign (p) == -1) {
                exponent_ = raise_domain_error (
                    "log_float<%1%> constructor",
                    "log_float cannot contain negative number %1%",
                    p, Policy());
            } else {
                using std::log;
                exponent_ = log (p);
            }
        }

        /// Construct with value exp(e).
        log_float (Exponent e, as_exponent) : exponent_ (e) {}

        /// Copy-construct.
        log_float (log_float const & other)
        /// \cond DONT_DOCUMENT
        = default
        /// \endcond
        ;

        /// Generalised copy construction with the same policy: implicit.
        template <typename OtherExponentType>
        log_float (log_float <OtherExponentType, Policy> const & other)
        : exponent_ (other.exponent()) {}

        /// Generalised copy construction with a different policy: explicit.
        template <typename OtherExponentType, typename OtherPolicy>
        explicit log_float (
            log_float <OtherExponentType, OtherPolicy> const & other)
        : exponent_ (other.exponent()) {}

        /**
        Copy-construct from signed_log_float.
        This is explicit.
        \pre "other" is positive.
        */
        template <typename OtherExponentType, typename OtherPolicy>
        explicit log_float (
            signed_log_float <OtherExponentType, OtherPolicy> const & other)
        {
            if (other.sign() == -1) {
                exponent_ = raise_domain_error (
                    "log_float<%1%> constructor",
                    "log_float cannot contain negative value -exp(%1%)",
                    other.exponent(), Policy());
            } else
                exponent_ = other.exponent();
        }

        /// \return The exponent.
        Exponent exponent() const { return exponent_; }

        /// \return The sign of the value (always +1 for log_float).
        constexpr int sign() const { return +1; }

        /// Multiply with another log_float in place.
        template <typename OtherExponentType>
            log_float & operator *= (
                const log_float <OtherExponentType, Policy> & other)
        {
            this->exponent_ = detail::multiply_log_float (
                this->exponent_, other.exponent(), Policy());
            return *this;
        }

        /// Divide by another log_float in place.
        template <typename OtherExponentType>
            log_float & operator /= (
                const log_float <OtherExponentType, Policy> & other)
        {
            this->exponent_ = detail::divide_log_float (
                this->exponent_, other.exponent(), Policy());
            return *this;
        }

        /// Add another log_float in place.
        template <typename OtherExponentType>
            log_float & operator += (
                const log_float <OtherExponentType, Policy> & other)
        {
            this->exponent_ = detail::add_log_float (
                this->exponent_, other.exponent(), Policy());
            return *this;
        }

        /*
        Subtraction-assignment is not provided.
        It makes no sense: it should return a signed_log_float, which
        in operator-= is not possible.
        */
    };

    /** \brief
    Generalisation of log_float that can contain negative values too.

    It stores the logarithm of the absolute value of the contained number, and,
    separately, its sign.

    \tparam Exponent The type to use to hold the exponent.
    \tparam Policy The Boost.Math policy that defines behaviour under domain
    errors, underflow errors, and overflow errors.

    \sa log_float
    */
    template <class Exponent, class Policy> class signed_log_float
    : public detail::log_float_base <
        signed_log_float <Exponent, Policy>, Exponent>
    {
        Exponent exponent_;
        // Either -1 or +1.
        int sign_;
    public:
        /// \brief The type of the underlying exponent.
        typedef Exponent exponent_type;
        /// \brief The error policy.
        typedef Policy policy_type;

        /// \cond DONT_DOCUMENT
        static_assert (detail::valid_exponent_type <Exponent>::value,
            "signed_log_float cannot be instantiated with this type.");
        /// \endcond

        /// Construct with value 0.
        signed_log_float()
        : exponent_ (-std::numeric_limits<Exponent>::infinity()), sign_ (+1) {}

        /// Construct with value \a p.
        signed_log_float (Exponent p) {
            sign_ = boost::math::signbit (p) ? -1 : +1;
            using std::log;
            exponent_ = log (sign_ * p);

            assert (sign_ == -1 || sign_ == +1);
        }

        /// Construct with value exp(e).
        signed_log_float (Exponent e, as_exponent)
        : exponent_ (e), sign_ (+1) {}

        /**
        Construct with value exp(e) and sign s.
        \pre s==-1 || s==+1
        */
        signed_log_float (Exponent e, int s, as_exponent)
        : exponent_ (e), sign_ (s)
        { assert (sign_ == -1 || sign_ == +1); }

        /// Copy-construct.
        signed_log_float (signed_log_float const & other)
        : exponent_ (other.exponent()), sign_ (other.sign())
        { assert (sign_ == -1 || sign_ == +1); }

        /// Generalised copy construction with the same policy: implicit.
        template <typename OtherExponentType>
            signed_log_float (
                signed_log_float <OtherExponentType, Policy> const &other)
        : exponent_ (other.exponent()), sign_ (other.sign())
        { assert (sign_ == -1 || sign_ == +1); }

        /// Generalised copy construction with a different policy: explicit.
        template <typename OtherExponentType, typename OtherPolicy>
        explicit signed_log_float (
            signed_log_float <OtherExponentType, OtherPolicy> const & other)
        : exponent_ (other.exponent()), sign_ (other.sign())
        { assert (sign_ == -1 || sign_ == +1); }

        /// Copy-construct from log_float with the same policy: implicit.
        template <typename OtherExponentType>
        signed_log_float (log_float <OtherExponentType, Policy> const & other)
        : exponent_ (other.exponent()), sign_ (+1)
        { assert (sign_ == -1 || sign_ == +1); }

        /// Copy-construct from log_float with a different policy: explicit.
        template <typename OtherExponentType, typename OtherPolicy>
        explicit signed_log_float (
            log_float <OtherExponentType, OtherPolicy> const & other)
        : exponent_ (other.exponent()), sign_ (+1)
        { assert (sign_ == -1 || sign_ == +1); }

        /// \return The exponent.
        Exponent exponent() const { return exponent_; }

        /// \return The sign (either -1 or +1).
        int sign() const {
            assert (sign_ == -1 || sign_ == +1);
            return sign_;
        }

        /// Multiply with another log_float or signed_log_float in place.
        template <class LogFloat, typename OtherExponentType, class Enable =
            typename boost::enable_if <std::is_same <typename
                LogFloat::policy_type, Policy>>::type>
        signed_log_float & operator *= (
            detail::log_float_base <LogFloat, OtherExponentType> const & other)
        {
            this->exponent_ = detail::multiply_log_float (
                this->exponent_, detail::exponent (other), Policy());
            this->sign_ *= detail::sign (other);
            return *this;
        }

        /// Divide by another log_float or signed_log_float in place.
        template <class LogFloat, typename OtherExponentType, class Enable =
            typename boost::enable_if <std::is_same <typename
                LogFloat::policy_type, Policy>>::type>
        signed_log_float & operator /= (
            detail::log_float_base <LogFloat, OtherExponentType> const & other)
        {
            this->exponent_ = detail::divide_log_float (
                this->exponent_, detail::exponent (other), Policy());
            this->sign_ *= detail::sign (other);
            return *this;
        }

        /// Add another log_float or signed_log_float in place.
        template <class LogFloat, typename OtherExponentType, class Enable =
            typename boost::enable_if <std::is_same <typename
                LogFloat::policy_type, Policy>>::type>
        signed_log_float & operator += (
            detail::log_float_base <LogFloat, OtherExponentType> const & other)
        {
            std::tie (this->exponent_, this->sign_)
                = detail::add_signed_log_float (this->exponent_, this->sign_,
                detail::exponent (other), detail::sign (other), Policy());
            return *this;
        }

        /// Subtract another log_float or signed_log_float in place.
        template <class LogFloat, typename OtherExponentType, class Enable =
            typename boost::enable_if <std::is_same <typename
                LogFloat::policy_type, Policy>>::type>
        signed_log_float & operator -= (
            detail::log_float_base <LogFloat, OtherExponentType> const & other)
        {
            std::tie (this->exponent_, this->sign_) =
                detail::add_signed_log_float (
                    this->exponent_, this->sign_,
                    detail::exponent (other), -detail::sign (other), Policy());
            return *this;
        }

    };

    /**
    Compare two objects of type log_float or signed_log_float.
    \param left The left-hand side argument
    \param right The right-hand side argument
    */
    template <class LogFloatLeft, typename ExponentTypeLeft,
        class LogFloatRight, typename ExponentTypeRight>
    inline bool operator < (
        detail::log_float_base <LogFloatLeft, ExponentTypeLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentTypeRight> const & right)
    {
        if (detail::sign (left) == detail::sign (right)) {
            // The following multiplication makes no mathematical sense, but it
            // implements the correct rule.
            // If the sign of both is positive, return |left| < |right|.
            // If the sign of both is negative, return |left| > |right|.
            return detail::sign (left) * detail::exponent (left) <
                detail::sign (right) * detail::exponent (right);
        } else {
            // (-0 < +0) = false.
            if (!left && !right)
                return false;
            else if (isnan (detail::exponent (left))
                    || isnan (detail::exponent (right)))
                // NaN always compares false to anything.
                return false;
            else
                return detail::sign (left) < detail::sign (right);
        }
    }

    /**
    Compare two objects of type log_float or signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentTypeLeft,
        class LogFloatRight, typename ExponentTypeRight>
    inline bool operator > (
        detail::log_float_base <LogFloatLeft, ExponentTypeLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentTypeRight> const & right)
    {
        if (detail::sign (left) == detail::sign (right)) {
            return detail::sign (left) * detail::exponent (left) >
                detail::sign (right) * detail::exponent (right);
        } else {
            if (!left && !right)
                return false;
            else if (isnan (detail::exponent (left))
                    || isnan (detail::exponent (right)))
                return false;
            else
                return detail::sign (left) > detail::sign (right);
        }
    }

    /**
    Compare two objects of type log_float or signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentTypeLeft,
        class LogFloatRight, typename ExponentTypeRight>
    inline bool operator <= (
        detail::log_float_base <LogFloatLeft, ExponentTypeLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentTypeRight> const & right)
    {
        if (detail::sign (left) == detail::sign (right)) {
            return detail::sign (left) * detail::exponent (left) <=
                detail::sign (right) * detail::exponent (right);
        } else {
            if (!left && !right)
                return true;
            else if (isnan (detail::exponent (left))
                    || isnan (detail::exponent (right)))
                return false;
            else
                return detail::sign (left) <= detail::sign (right);
        }
    }

    /**
    Compare two objects of type log_float or signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentTypeLeft,
        class LogFloatRight, typename ExponentTypeRight>
    inline bool operator >= (
        detail::log_float_base <LogFloatLeft, ExponentTypeLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentTypeRight> const & right)
    {
        if (detail::sign (left) == detail::sign (right)) {
            return detail::sign (left) * detail::exponent (left) >=
                detail::sign (right) * detail::exponent (right);
        } else {
            if (!left && !right)
                return true;
            else if (isnan (detail::exponent (left))
                    || isnan (detail::exponent (right)))
                return false;
            else
                return detail::sign (left) >= detail::sign (right);
        }
    }

    /**
    Compare two objects of type log_float or signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentTypeLeft,
        class LogFloatRight, typename ExponentTypeRight>
    inline bool operator == (
        detail::log_float_base <LogFloatLeft, ExponentTypeLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentTypeRight> const & right)
    {
        if (detail::sign (left) == detail::sign (right)) {
            return detail::exponent (left) == detail::exponent (right);
        } else {
            return !left && !right;
        }
    }

    /**
    Compare two objects of type log_float or signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentTypeLeft,
        class LogFloatRight, typename ExponentTypeRight>
    inline bool operator != (
        detail::log_float_base <LogFloatLeft, ExponentTypeLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentTypeRight> const & right)
    {
        if (detail::sign (left) == detail::sign (right)) {
            return detail::exponent (left) != detail::exponent (right);
        } else {
            return left || right;
        }
    }

    /* Comparison between log_float and normal scalar. */
    /// \cond DONT_DOCUMENT
#define MATH_LOG_FLOAT_DEFINE_COMPARISON(operation) \
    template <class LogFloat, typename Exponent, typename Scalar, class Enable \
        = typename boost::enable_if_c < \
            std::numeric_limits <Scalar>::is_specialized \
            && !is_log_float <Scalar>::value>::type> \
    inline bool operator operation ( \
        detail::log_float_base <LogFloat, Exponent> const & left, \
        Scalar const & right) \
    { \
        typedef typename promote_args <Exponent, Scalar>::type result_float; \
        signed_log_float <result_float, typename LogFloat::policy_type> \
            converted_right (right); \
        return left operation converted_right; \
    } \
    \
    template <class LogFloat, typename Exponent, typename Scalar, class Enable \
        = typename boost::enable_if_c < \
            std::numeric_limits <Scalar>::is_specialized \
            && !is_log_float <Scalar>::value>::type> \
    inline bool operator operation (Scalar const & left, \
        detail::log_float_base <LogFloat, Exponent> const & right) \
    { \
        typedef typename promote_args <Exponent, Scalar>::type result_float; \
        signed_log_float <result_float, typename LogFloat::policy_type> \
            converted_left (left); \
        return converted_left operation right; \
    }

    MATH_LOG_FLOAT_DEFINE_COMPARISON( < )
    MATH_LOG_FLOAT_DEFINE_COMPARISON( <= )
    MATH_LOG_FLOAT_DEFINE_COMPARISON( > )
    MATH_LOG_FLOAT_DEFINE_COMPARISON( >= )
    MATH_LOG_FLOAT_DEFINE_COMPARISON( == )
    MATH_LOG_FLOAT_DEFINE_COMPARISON( != )

#undef MATH_LOG_FLOAT_DEFINE_COMPARISON
    /// \endcond

    /**
    Multiply two values represented as log_float.
    */
    template <class ExponentType1, class ExponentType2, class Policy>
        inline log_float <
            typename promote_args <ExponentType1, ExponentType2>::type, Policy>
    operator * (const log_float <ExponentType1, Policy> & l1,
        const log_float <ExponentType2, Policy> & l2)
    {
        return log_float <
            typename promote_args <ExponentType1, ExponentType2>::type, Policy>
            (detail::multiply_log_float (l1.exponent(), l2.exponent(),
                Policy()), as_exponent());
    }

    /**
    Multiply two values represented as log_float and signed_log_float, with at
    least one signed.
    The result is always signed, and therefore of type signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentLeft,
        class LogFloatRight, typename ExponentRight, class Enable =
        typename boost::enable_if <std::is_same <
            typename LogFloatLeft::policy_type,
            typename LogFloatRight::policy_type>>::type>
    inline signed_log_float <
        typename promote_args <ExponentLeft, ExponentLeft>::type,
        typename LogFloatLeft::policy_type>
    operator * (
        detail::log_float_base <LogFloatLeft, ExponentLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentRight> const & right)
    {
        typedef typename LogFloatLeft::policy_type policy_type;
        typedef signed_log_float <typename promote_args <
                ExponentLeft, ExponentLeft>::type, policy_type
            > result_type;
        return result_type (detail::multiply_log_float (
                left.exponent(), right.exponent(), policy_type()),
            detail::sign (left) * detail::sign (right), as_exponent());
    }

    /**
    Divide two values represented as log_float.
    */
    template <class ExponentType1, class ExponentType2, class Policy>
        log_float <typename promote_args <ExponentType1, ExponentType2>::type,
            Policy>
    operator / (const log_float <ExponentType1, Policy> & l1,
        const log_float <ExponentType2, Policy> & l2)
    {
        return log_float <
            typename promote_args <ExponentType1, ExponentType2>::type, Policy>
            (detail::divide_log_float (l1.exponent(), l2.exponent(), Policy()),
                as_exponent());
    }

    /**
    Divide two values represented as log_float and signed_log_float, with at
    least one signed.
    The result is always signed, and therefore of type signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentLeft,
        class LogFloatRight, typename ExponentRight, class Enable =
        typename boost::enable_if <std::is_same <
            typename LogFloatLeft::policy_type,
            typename LogFloatRight::policy_type>>::type>
    inline signed_log_float <
        typename promote_args <ExponentLeft, ExponentLeft>::type,
        typename LogFloatLeft::policy_type>
    operator / (
        detail::log_float_base <LogFloatLeft, ExponentLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentRight> const & right)
    {
        typedef typename LogFloatLeft::policy_type policy_type;
        typedef signed_log_float <typename promote_args <
                ExponentLeft, ExponentLeft>::type, policy_type
            > result_type;
        return result_type (detail::divide_log_float (
                left.exponent(), right.exponent(), policy_type()),
            detail::sign (left) * detail::sign (right), as_exponent());
    }

    /**
    Add two values stored as their logs.
    */
    template <class ExponentType1, class ExponentType2, class Policy>
        log_float <typename promote_args <ExponentType1, ExponentType2>::type,
            Policy>
    operator + (const log_float <ExponentType1, Policy> & a,
        const log_float <ExponentType2, Policy> & b)
    {
        return log_float <
            typename promote_args <ExponentType1, ExponentType2>::type, Policy>
            (detail::add_log_float (a.exponent(), b.exponent(), Policy()),
                as_exponent());
    }

    /**
    Add two values represented as log_float and signed_log_float, with at least
    one signed.
    The result is always signed, and therefore of type signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentLeft,
        class LogFloatRight, typename ExponentRight, class Enable =
            typename boost::enable_if <std::is_same <
                typename LogFloatLeft::policy_type,
                typename LogFloatRight::policy_type>>::type>
    inline signed_log_float <
        typename promote_args <ExponentLeft, ExponentLeft>::type,
        typename LogFloatLeft::policy_type>
    operator + (
        detail::log_float_base <LogFloatLeft, ExponentLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentRight> const & right)
    {
        typedef typename promote_args <ExponentLeft, ExponentLeft>::type
            exponent_type;
        typedef typename LogFloatLeft::policy_type policy_type;
        typedef signed_log_float <exponent_type, policy_type> result_type;
        std::pair <exponent_type, int> result =  detail::add_signed_log_float (
                left.exponent(), detail::sign (left),
                right.exponent(), detail::sign (right), policy_type());
        return result_type (result.first, result.second, as_exponent());
    }

    /**
    Subtract two values represented as log_float or signed_log_float.
    The result is always signed, and therefore of type signed_log_float.
    */
    template <class LogFloatLeft, typename ExponentLeft,
        class LogFloatRight, typename ExponentRight, class Enable =
            typename boost::enable_if <std::is_same <
                typename LogFloatLeft::policy_type,
                typename LogFloatRight::policy_type>>::type>
    inline signed_log_float <
        typename promote_args <ExponentLeft, ExponentLeft>::type,
        typename LogFloatLeft::policy_type>
    operator - (
        detail::log_float_base <LogFloatLeft, ExponentLeft> const & left,
        detail::log_float_base <LogFloatRight, ExponentRight> const & right)
    {
        typedef typename promote_args <ExponentLeft, ExponentLeft>::type
            exponent_type;
        typedef typename LogFloatLeft::policy_type policy_type;
        typedef signed_log_float <exponent_type, policy_type> result_type;
        std::pair <exponent_type, int> result =  detail::add_signed_log_float (
                left.exponent(), detail::sign (left),
                // Notice the difference with operator +:
                // the - in front of detail::sign (right).
                right.exponent(), -detail::sign (right), policy_type());
        return result_type (result.first, result.second, as_exponent());
    }

    /**
    Negate a value represented as log_float or signed_log_float.
    The result is always signed, and therefore of type signed_log_float.
    */
    template <class LogFloat, typename Exponent>
    inline signed_log_float <Exponent, typename LogFloat::policy_type>
        operator - (detail::log_float_base <LogFloat, Exponent> const & operand)
    {
        return signed_log_float <Exponent, typename LogFloat::policy_type> (
            operand.exponent(), -operand.sign(), as_exponent());
    }

    /* Interaction between log_float and normal scalar. */
    /// \cond DONT_DOCUMENT
#define MATH_LOG_FLOAT_DEFINE_BINARY_OPERATION(operation) \
    template <class LogFloat, typename Exponent, typename Scalar, class Enable \
        = typename boost::enable_if_c < \
            std::numeric_limits <Scalar>::is_specialized \
            && !is_log_float <Scalar>::value>::type> \
    inline signed_log_float <typename promote_args <Exponent, Scalar>::type, \
        typename LogFloat::policy_type> \
    operator operation ( \
        detail::log_float_base <LogFloat, Exponent> const & left, \
        Scalar const & right) \
    { \
        typedef typename promote_args <Exponent, Scalar>::type result_float; \
        signed_log_float <result_float, typename LogFloat::policy_type> \
            converted_right (right); \
        return left operation converted_right; \
    } \
    \
    template <class LogFloat, typename Exponent, typename Scalar, class Enable \
        = typename boost::enable_if_c < \
            std::numeric_limits <Scalar>::is_specialized \
            && !is_log_float <Scalar>::value>::type> \
    inline signed_log_float <typename promote_args <Exponent, Scalar>::type, \
        typename LogFloat::policy_type> \
    operator operation ( \
        Scalar const & left, \
        detail::log_float_base <LogFloat, Exponent> const & right) \
    { \
        typedef typename promote_args <Exponent, Scalar>::type result_float; \
        signed_log_float <result_float, typename LogFloat::policy_type> \
            converted_left (left); \
        return converted_left operation right; \
    }

    MATH_LOG_FLOAT_DEFINE_BINARY_OPERATION( + )
    MATH_LOG_FLOAT_DEFINE_BINARY_OPERATION( - )
    MATH_LOG_FLOAT_DEFINE_BINARY_OPERATION( / )
    MATH_LOG_FLOAT_DEFINE_BINARY_OPERATION( * )

#undef MATH_LOG_FLOAT_DEFINE_BINARY_OPERATION

#define MATH_LOG_FLOAT_DEFINE_ASSIGNMENT_OPERATION(operation) \
    template <class LogFloat, typename Exponent, typename Other, class Enable \
        = typename boost::enable_if_c < \
            std::numeric_limits <Other>::is_specialized \
            && !is_log_float <Other>::value>::type> \
    inline LogFloat & operator operation ( \
        detail::log_float_base <LogFloat, Exponent> & w, Other const & other) \
    { \
        return (*w.this_()) operation LogFloat (other); \
    }

    MATH_LOG_FLOAT_DEFINE_ASSIGNMENT_OPERATION( += )
    MATH_LOG_FLOAT_DEFINE_ASSIGNMENT_OPERATION( *= )
    MATH_LOG_FLOAT_DEFINE_ASSIGNMENT_OPERATION( /= )
    MATH_LOG_FLOAT_DEFINE_ASSIGNMENT_OPERATION( -= )

#undef MATH_LOG_FLOAT_DEFINE_ASSIGNMENT_OPERATION
    /// \endcond

    /**
    Take a log_float to a power.
    This only takes a log_float, not a signed_log_float, so that this function
    is well-defined.
    */
    template <typename Exponent, typename Policy, typename Exponent2Type>
    inline log_float <
        typename promote_args <Exponent, Exponent2Type>::type, Policy>
    pow (const log_float <Exponent, Policy> & w, Exponent2Type const & d)
    {
        typedef log_float <
                typename promote_args <Exponent, Exponent2Type>::type, Policy>
            result_type;
        if (w) {
            if (w.exponent() == 0)
                // w == 1
                // This is important if p == inf.
                return Exponent (1);
            else
                return result_type (w.exponent() * d, as_exponent());
        } else {
            // w == 0
            if (d > 0)
                return Exponent();
            else if (d == 0) {
                // Special case because log (pow(0,0)) != 0 * log(0)
                // 0 * log (0) is NaN
                // and log (pow(inf,0)).
                return Exponent (1);
            } else { // (d < 0)
                return result_type (raise_domain_error (
                    "pow (log_float<%1%>, d)",
                    "%1% cannot be raised to a negative power",
                    Exponent (0), Policy()), as_exponent());
            }
        }
    }

    /**
    Construct a log_float instance from a value, using it as the value that is
    stored.
    This can be seen as a lazy implementation of computing the exponent of the
    value.
    Hence the spelling "exp_".
    */
    template <typename Exponent, typename Enable =
        typename boost::enable_if <
            detail::valid_exponent_type <Exponent>>::type>
    inline log_float <Exponent> exp_ (Exponent const & d)
    {
        return log_float <Exponent> (d, as_exponent());
    }

    /**
    \return The logarithm of the value contained in a log_float.
        Since log_float stores the contained value as a logarithm, this
        function is trivial.
    */
    template <typename Exponent, class Policy>
        inline Exponent log (log_float <Exponent, Policy> const & p)
    { return p.exponent(); }

    /**
    Exponentiate a log_float or signed_log_float.
    The result is always positive, so it is a log_float.
    */
    template <typename LogFloat, class Exponent>
        inline log_float <Exponent, typename LogFloat::policy_type>
        exp (detail::log_float_base <LogFloat, Exponent> const & w)
    {
        using std::exp;
        // log (exp (a)) = a = exp (log (a)).
        // If a < 0,
        // log (exp (a)) = a = -exp (log (|a|)).
        return log_float <Exponent, typename LogFloat::policy_type> (
            w.sign() * exp (w.exponent()), as_exponent());
    }

    /**
    \brief Compute the square root
    \return the square root of a log_float.
    This involves merely dividing the exponent by 2.
    */
    template <typename Exponent, class Policy>
        inline log_float <Exponent, Policy> sqrt (
            log_float <Exponent, Policy> const & p)
    { return log_float <Exponent, Policy> (p.exponent() / 2, as_exponent()); }

    /** \brief
    Compute a hash value for a log_float or signed_log_float for use with
    Boost.Hash.
    */
    template <class LogFloat, typename Exponent>
        inline std::size_t hash_value (
            const detail::log_float_base <LogFloat, Exponent> & p)
    {
        // Make the hash depend on the sign but only if p is not -0.
        std::size_t seed = (p.sign() == -1 && p) ? 1 : 0;
        boost::hash_combine (seed, p.exponent());
        return seed;
    }

    /**
    \todo Test.
    */
    template <class LogFloat, typename Exponent,
        typename Char, class CharTraits>
    inline std::basic_ostream <Char, CharTraits> & operator << (
        std::basic_ostream <Char, CharTraits> & os,
        const detail::log_float_base <LogFloat, Exponent> & w)
    {
        using std::log; using std::floor; using std::exp;

        unsigned base;
        if (os.flags() & std::ios_base::hex)
            base = 16;
        else if (os.flags() & std::ios_base::oct)
            base = 8;
        else
            base = 10;
        std::streamsize const precision = os.precision();

        Exponent log_base, base_exponent;
        log_base = log (Exponent (base));
        if (w)
            base_exponent = floor (detail::exponent (w) / log_base);
        else
            base_exponent = 0;

        bool scientific;

        if (isinf (detail::exponent (w)))
            scientific = false;
            // Or at least, just represent 0 or +-inf as standard.
        else {
            if ((os.flags() & std::ios_base::floatfield) == 0) {
                // Do something sensible
                int precision_switch = (std::min) (
                    precision, std::streamsize (4));
                scientific = (base_exponent < -precision_switch
                    || precision_switch < base_exponent);
            } else
                scientific = (os.flags() & std::ios_base::scientific) != 0;
        }

        if (!scientific) {
            return os << Exponent (w);
        } else {
            std::ios_base::fmtflags save_flags = os.flags();
            Exponent significand = sign (w) * exp (
                detail::exponent (w) - base_exponent * log_base);
            os.setf (std::ios_base::fixed);
            os << significand;
            os.unsetf (std::ios::floatfield);
            if (-1000 < base_exponent && base_exponent < 1000)
                os << "e" << base_exponent;
            else {
                // Be careful because we might need to use nested "e"s!
                // Therefore, add parentheses.
                os << "e(" << base_exponent << ")";
            }
            os.flags (save_flags);
            return os;
        }
    }

} // namespace math

#endif  // MATH_LOG_FLOAT_HPP_INCLUDED

