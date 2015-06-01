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

#define BOOST_TEST_MODULE test_product_hash
#include "utility/test/boost_unit_test.hpp"

#include "math/product.hpp"

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std/container.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"

#include "math/check/check_hash.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_product_hash)

template <class Inverse> void check_hash_homogeneous() {
    typedef math::product <math::over <float, math::cost <double>>, Inverse>
        product;
    std::vector <product> examples;
    examples.push_back (math::zero <product>());
    examples.push_back (product (0, math::cost <double> (0)));
    examples.push_back (product (0, math::cost <double> (4)));
    examples.push_back (product (0, math::zero <math::cost <double>>()));
    examples.push_back (product (1, math::cost <double> (0)));
    examples.push_back (product (1, math::cost <double> (4)));
    examples.push_back (product (1, math::zero <math::cost <double>>()));

    math::check_hash (examples);
}

template <class Direction, class Inverse> void check_hash_heterogeneous() {
    typedef math::product <math::over <
            float, math::sequence <char, Direction>>, Inverse>
        product;
    typedef math::product <math::over <
            float, math::optional_sequence <char, Direction>>, Inverse>
        optional_product;
    typedef math::product <math::over <
            float, math::sequence_annihilator <char, Direction>>, Inverse>
        annihilator_product;

    auto examples = range::make_tuple (
        annihilator_product (0, math::sequence_annihilator <char, Direction>()),
        annihilator_product (4, math::sequence_annihilator <char, Direction>()),
        optional_product (0, math::optional_sequence <char, Direction>()),
        optional_product (1, math::optional_sequence <char, Direction> ('a')),
        product (5, math::sequence <char, Direction> (std::string ("abc"))));

    math::check_hash (examples);
    math::check_cast_hash <product> (examples);
}

BOOST_AUTO_TEST_CASE (test_hash) {
    check_hash_homogeneous <math::with_inverse<>>();
    check_hash_homogeneous <math::with_inverse <math::callable::plus>>();
    check_hash_homogeneous <math::with_inverse <math::callable::times>>();

    check_hash_heterogeneous <math::left, math::with_inverse<>>();
    check_hash_heterogeneous <math::left,
        math::with_inverse <math::callable::plus>>();
    check_hash_heterogeneous <math::left,
        math::with_inverse <math::callable::times>>();

    check_hash_heterogeneous <math::right, math::with_inverse<>>();
    check_hash_heterogeneous <math::right,
        math::with_inverse <math::callable::plus>>();
    check_hash_heterogeneous <math::right,
        math::with_inverse <math::callable::times>>();
}

BOOST_AUTO_TEST_SUITE_END()
