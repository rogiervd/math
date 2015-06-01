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
