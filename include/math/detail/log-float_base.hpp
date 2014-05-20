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

#ifndef MATH_LOG_FLOAT_DETAIL_BASE_HPP_INCLUDED
#define MATH_LOG_FLOAT_DETAIL_BASE_HPP_INCLUDED

#include "log-float_fwd.hpp"

namespace math { namespace detail {

    /**
    Base class for both log_float and signed_log_float.
    */
    template <class Implementation, typename ExponentType> struct log_float_base
    {
        /// \internal
        Implementation * this_()
        { return static_cast <Implementation *> (this); }
        /// \internal
        Implementation const * this_() const
        { return static_cast <Implementation const *> (this); }
        /// \internal
        ExponentType exponent() const { return this_()->exponent(); }

        int sign() const { return this_()->sign(); }

        // Things that are the same between log_float and signed_log_float.
        /**
        \return the value as a double.
        \note This may underflow or overflow, with the behaviour specified by
        the \c Policy.
        */
        ExponentType get() const {
            typedef typename Implementation::policy_type policy_type;
            static const char * function_name = "log_float<%1%>::get";
            constexpr ExponentType infinity =
                std::numeric_limits<ExponentType>::infinity();
            using std::exp;
            ExponentType result = exp (exponent());
            if (result == 0 && exponent() != -infinity) {
                raise_underflow_error <ExponentType> (function_name,
                    "value cannot be represented except by 0",
                    policy_type());
            }
            if (result == infinity && exponent() != infinity) {
                raise_overflow_error <ExponentType> (function_name,
                    "value cannot be represented except by infinity",
                    policy_type());
            }
            return sign() * result;
        }

        /**
        \return the value as a double.
        \note This may underflow or overflow, with the behaviour specified by
        the \c Policy.
        */
        explicit operator ExponentType() const { return this->get(); }

        /**
        \return false iff the value is equal to 0, and otherwise true.
        */
        explicit operator bool() const {
            return this_()->exponent() !=
                -std::numeric_limits <ExponentType>::infinity();
        }

        /**
        \return true iff the value is equal to 0, and otherwise false.
        */
        bool operator ! () const {
            return this_()->exponent() ==
                -std::numeric_limits <ExponentType>::infinity();
        }
    };

    template <typename ExponentType, class Policy>
    inline constexpr int sign (
        log_float_base <log_float <ExponentType, Policy>, ExponentType> const &)
    { return +1; }

    template <typename ExponentType, class Policy>
    inline int sign (
        log_float_base <signed_log_float <ExponentType, Policy>, ExponentType>
            const & w)
    { return w.this_()->sign(); }

    template <typename Wide, typename ExponentType>
    inline ExponentType exponent (log_float_base <Wide, ExponentType> const & w)
    { return w.this_()->exponent(); }

}} // namespace math::detail

#endif  // MATH_LOG_FLOAT_DETAIL_BASE_HPP_INCLUDED

