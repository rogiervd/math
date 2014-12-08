/*
Copyright 2013 Rogier van Dalen.

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
