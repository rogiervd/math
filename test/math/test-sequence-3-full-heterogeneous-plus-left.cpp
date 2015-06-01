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
Test sequence.hpp with math::left.

Because the template instantiations for math::left and math::right are mostly
disjoint, instantiating the tests in two different files helps decrease the
amount of memory used for compiling the test.
*/

#define BOOST_TEST_MODULE test_sequence_full_heterogeneous_left
#include "utility/test/boost_unit_test.hpp"

#include "sequence-tests-slow.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_sequence_full_heterogeneous_left)

BOOST_AUTO_TEST_CASE (test_sequence_left) {
    test_sequence_heterogeneous <math::left> (check_plus <math::left>());
}

BOOST_AUTO_TEST_SUITE_END()
