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
Test sequence.hpp: spot checks.

Compiling this takes less memory and time than test-sequence-slow-{left,right}.
This file also comes earlier in a lexicographical sort of file names, so it will
be run first.
*/

#define BOOST_TEST_MODULE test_sequence_fast_right_properties
#include "utility/test/boost_unit_test.hpp"

#include "sequence-tests-fast-properties.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_sequence_fast_right_properties)

BOOST_AUTO_TEST_CASE (test_sequence) {
    test_sequence_spot <math::right, math::left>();
}

BOOST_AUTO_TEST_SUITE_END()
