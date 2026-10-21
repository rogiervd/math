// SPDX-FileCopyrightText: Copyright 2012, 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MATH_DETAIL_LOG_FLOAT_FWD_HPP
#define MATH_DETAIL_LOG_FLOAT_FWD_HPP

#include <type_traits>

#include <boost/math/policies/error_handling.hpp>
#include <boost/math/policies/policy.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/math/tools/promotion.hpp>

namespace math {

using boost::math::isinf;
using boost::math::isnan;
using boost::math::sign;
using boost::math::policies::policy;
using boost::math::policies::raise_domain_error;
using boost::math::policies::raise_overflow_error;
using boost::math::policies::raise_underflow_error;
using boost::math::tools::promote_args;

/**
Tag class to indicate that an argument is meant as an exponent.
*/
class as_exponent {};

template <class ExponentType = double, class Policy = policy<>> class log_float;
template <class ExponentType = double, class Policy = policy<>>
class signed_log_float;

/**
Compile-time constant that evaluates to \c true iff \a Type is a log_float
or a signed_log_float (with or without reference- and cv-qualification).
*/
template <class Type> struct is_log_float : std::false_type {};

template <class Type> struct is_log_float<Type &> : is_log_float<Type> {};
template <class Type> struct is_log_float<Type &&> : is_log_float<Type> {};
template <class Type> struct is_log_float<Type const> : is_log_float<Type> {};
template <class Type> struct is_log_float<Type volatile> : is_log_float<Type> {
};

template <class ExponentType, class Policy>
struct is_log_float<log_float<ExponentType, Policy>> : std::true_type {};
template <class ExponentType, class Policy>
struct is_log_float<signed_log_float<ExponentType, Policy>> : std::true_type {};

}  // namespace math

#endif  // MATH_DETAIL_LOG_FLOAT_FWD_HPP
