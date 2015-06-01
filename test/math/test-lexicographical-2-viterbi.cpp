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
Test lexicographical.hpp with an example: the Viterbi semiring.
*/

#define BOOST_TEST_MODULE test_math_lexicographical_fast
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <string>
#include <ostream>
#include <iostream>

#include <boost/mpl/assert.hpp>

#include "range/std/container.hpp"

#include "math/max_semiring.hpp"
#include "math/sequence.hpp"
#include "math/cost.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_lexicographical_viterbi)

/**
Two paths diverge in a wood.
We are a computer, so we can travel both.
One path has (1, a) and (1/2, b) as labels.
This is the path more travelled by.
The other path has (1/4, c) and (1, d) as labels.
This is the path less travelled by.
Way leads on to way, and both paths lead to the same place.
This is a transition with (1/4, e) as a label.

The Viterbi semiring, summing over all paths, should return the complete path
more travelled by.
That complete path has (1/8, abe) as its label.
The other path, at (1/16, cde), is worse.
*/
BOOST_AUTO_TEST_CASE (test_viterbi) {
    typedef math::lexicographical <math::over <
        math::max_semiring <float>,
        math::single_sequence <char>>> viterbi_semiring;

    viterbi_semiring v_1_a (1, 'a');
    viterbi_semiring v_0_5_b (0.5, 'b');
    viterbi_semiring v_0_25_c (0.25, 'c');
    viterbi_semiring v_1_d (1, 'd');
    viterbi_semiring v_0_25_e (0.25, 'e');

    auto result = ((v_1_a * v_0_5_b) + (v_0_25_c * v_1_d)) * v_0_25_e;
    std::cout << result << std::endl;
    BOOST_MPL_ASSERT ((std::is_same <decltype (result),
        math::lexicographical <math::over <
            math::max_semiring <float>, math::sequence <char>>>>));
    BOOST_CHECK_EQUAL (range::at_c <0> (result.components()).value(), .125);
    BOOST_CHECK (range::at_c <1> (result.components())
        == math::sequence <char> (std::string ("abe")));
}

/**
Now we are Frost, and we use a cost.
That makes all the difference.
We then take the path less travelled by.
Its label is (1.5, cde).
That is better than (1.75, abe).
*/
BOOST_AUTO_TEST_CASE (test_cost) {
    typedef math::lexicographical <math::over <
        math::cost <float>,
        math::single_sequence <char>>> cost_semiring;

    cost_semiring v_1_a (1, 'a');
    cost_semiring v_0_5_b (0.5, 'b');
    cost_semiring v_0_25_c (0.25, 'c');
    cost_semiring v_1_d (1, 'd');
    cost_semiring v_0_25_e (0.25, 'e');

    auto result = ((v_1_a * v_0_5_b) + (v_0_25_c * v_1_d)) * v_0_25_e;
    std::cout << result << std::endl;
    BOOST_MPL_ASSERT ((std::is_same <decltype (result),
        math::lexicographical <math::over <
            math::cost <float>, math::sequence <char>>>>));
    BOOST_CHECK_EQUAL (range::at_c <0> (result.components()).value(), 1.5);
    BOOST_CHECK (range::at_c <1> (result.components())
        == math::sequence <char> (std::string ("cde")));
}

BOOST_AUTO_TEST_SUITE_END()
