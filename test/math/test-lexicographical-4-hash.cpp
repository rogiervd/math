/*
Copyright 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_lexicographical_hash
#include "utility/test/boost_unit_test.hpp"

#include "math/lexicographical.hpp"

#include <boost/functional/hash.hpp>

#include "range/tuple.hpp"
#include "range/std/container.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"

#include "math/check/check_hash.hpp"

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

    math::check_hash (examples);
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

    math::check_hash (examples);
    math::check_cast_hash <lexicographical> (examples);
}

BOOST_AUTO_TEST_SUITE_END()
