/*
Copyright 2014 Rogier van Dalen.

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
Test magma.hpp.
There is very little to test in magma.hpp without an example magma.
The following therefore end up testing the main parts of magma.hpp:
- test-arithmetic_magma.cpp
- test-best.cpp
- test-magma.cpp
*/

#define BOOST_TEST_MODULE test_magma
#include "../boost_unit_test.hpp"

#include "math/magma.hpp"

BOOST_AUTO_TEST_SUITE (test_magma)

BOOST_AUTO_TEST_CASE (test_magma) {
    // inverse_operation.
    {
        // Check return type: should be "divide" functor.
        // Two variants: with/without direction;
        // with the operation as compile-time/run-time argument.
        BOOST_MPL_ASSERT ((std::is_same <std::decay <math::result_of <
                math::callable::inverse_operation<> (math::callable::times)
            >::type>::type, math::callable::divide<>>));
        BOOST_MPL_ASSERT ((std::is_same <std::decay <math::result_of <
                math::callable::inverse_operation <math::left> (
                    math::callable::times)
            >::type>::type, math::callable::divide <math::left>>));
        BOOST_MPL_ASSERT ((std::is_same <std::decay <math::result_of <
                math::callable::inverse_operation <math::callable::plus> ()
            >::type>::type, math::callable::minus<>>));
        BOOST_MPL_ASSERT ((std::is_same <std::decay <math::result_of <
                math::callable::inverse_operation <
                    math::right, math::callable::plus>()
            >::type>::type, math::callable::minus <math::right>>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
