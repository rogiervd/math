/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_lexicographical_hash
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std/container.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"

#include "math/check/report_check_magma_boost_test.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_lexicographical_hash)

BOOST_AUTO_TEST_CASE (test_hash_homogeneous) {
    typedef math::lexicographical <
            math::over <math::cost <double>, math::sequence <char>>>
        lexicographical;
    std::vector <lexicographical> examples;
    examples.push_back (math::zero <lexicographical>());
    examples.push_back (lexicographical (math::cost <double> (0),
        math::sequence_annihilator <char> ()));
    examples.push_back (lexicographical (math::zero <math::cost <double>>(),
        math::sequence <char> ()));
    examples.push_back (lexicographical (math::cost <double> (4),
        math::sequence_annihilator <char>()));
    examples.push_back (lexicographical (math::zero <math::cost <double>>(),
        math::sequence_annihilator <char>()));
    examples.push_back (lexicographical (math::cost <double> (0),
        math::sequence <char> (std::string ("a"))));
    examples.push_back (lexicographical (math::cost <double> (0),
        math::sequence <char> (std::string ("b"))));
    examples.push_back (lexicographical (math::cost <double> (0),
        math::sequence <char> (std::string ("bcd"))));

    math::report_check_hash (examples);
}

BOOST_AUTO_TEST_CASE (test_hash_heterogeneous) {
    typedef math::lexicographical <math::over <
            math::cost <float>, math::sequence <char>>>
        lexicographical;
    typedef math::lexicographical <math::over <
            math::cost <float>, math::optional_sequence <char>>>
        optional_lexicographical;
    typedef math::lexicographical <math::over <
            math::cost <float>, math::sequence_annihilator <char>>>
        annihilator_lexicographical;

    auto examples = range::make_tuple (
        annihilator_lexicographical (math::cost <float> (0),
            math::sequence_annihilator <char>()),
        annihilator_lexicographical (math::cost <float> (4),
            math::sequence_annihilator <char>()),
        optional_lexicographical (math::cost <float> (0),
            math::optional_sequence <char>()),
        optional_lexicographical (math::cost <float> (1),
            math::optional_sequence <char> ('a')),
        lexicographical (math::cost <float> (5),
            math::sequence <char> (std::string ("abc"))));

    math::report_check_hash (examples);
    math::report_check_cast_hash <lexicographical> (examples);
}

BOOST_AUTO_TEST_SUITE_END()
