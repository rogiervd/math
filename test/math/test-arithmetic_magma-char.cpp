/*
Copyright 2012, 2013, 2015 Rogier van Dalen.

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
Test arithmetic_magma.hpp, and, in the process, magma.hpp.
*/

#define BOOST_TEST_MODULE test_arithmetic_magma_char
#include "utility/test/boost_unit_test.hpp"

#include "math/arithmetic_magma.hpp"

#include "arithmetic_magma-tests-integer.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_arithmetic_magma_char)

BOOST_AUTO_TEST_CASE (test_arithmetic_magma_integers) {
    test_arithmetic_magma_integer <char> (
        get_signed_integer_examples <char>(),
        get_signed_integer_examples <char>());
    test_arithmetic_magma_integer <unsigned char> (
        get_unequal_unsigned_integer_examples <unsigned char>(),
        get_unsigned_integer_examples <unsigned char>());
}

BOOST_AUTO_TEST_SUITE_END()
