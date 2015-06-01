/*
Copyright 2013 Rogier van Dalen.

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

#ifndef MATH_DETAIL_IS_CLOSE_HPP
#define MATH_DETAIL_IS_CLOSE_HPP

#include <cmath>

#include <boost/utility/enable_if.hpp>

#include "./log-float_fwd.hpp"

namespace math { namespace detail {

template <class Type, class Tolerance, class Enable =
    typename boost::disable_if <is_log_float <Type>>::type>
inline bool is_close (
    Type const & left, Type const & right, Tolerance const & tolerance)
{
    using std::abs;
    if (left == right)
        return true;
    else {
        auto difference = abs (left - right);
        return difference <= tolerance * abs (left)
            && difference <= tolerance * abs (right);
    }
}

// log_float: compare exponents.
template <class Type, class Tolerance, class Enable =
    typename boost::enable_if <is_log_float <Type>>::type>
inline bool is_close (
    Type const & left, Type const & right, Tolerance const & tolerance,
    // Dummy argument to distinguish this overloads from the one above.
    void * = 0)
{ return is_close (left.exponent(), right.exponent(), tolerance); }

}} // namespace math::detail

#endif // MATH_DETAIL_IS_CLOSE_HPP
