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

/** \file
Integration check to see whether using dense symbols in a sequence works.
*/

#define BOOST_TEST_MODULE test_sequence_alphabet
#include "utility/test/boost_unit_test.hpp"

#include "math/sequence.hpp"
#include "math/alphabet.hpp"

#include <string>

BOOST_AUTO_TEST_SUITE (test_suite_sequence_alphabet)

typedef math::alphabet <std::string> alphabet_type;

BOOST_AUTO_TEST_CASE (test_sequence_alphabet) {
    alphabet_type alphabet;

    auto symbol1 = alphabet.add_symbol ("hello");
    auto symbol2 = alphabet.add_symbol ("bye");

    typedef decltype (symbol1) internal_symbol_type;

    math::empty_sequence <internal_symbol_type> empty_sequence;
    math::optional_sequence <internal_symbol_type> sequence1 (symbol1);
    math::single_sequence <internal_symbol_type> sequence2 (symbol2);

    // The first symbol will be chosen because dense symbols are not in
    // lexicographical order.
    BOOST_CHECK (math::choose (sequence1, sequence2) == sequence1);
    BOOST_CHECK (math::plus (sequence1, sequence2) == empty_sequence);

    auto sequence12 = sequence1 * sequence2;
    BOOST_CHECK_EQUAL (range::size (sequence12.symbols()), 2);
    BOOST_CHECK (range::first (sequence12.symbols()) == symbol1);
    BOOST_CHECK (range::second (sequence12.symbols()) == symbol2);
}

BOOST_AUTO_TEST_SUITE_END()
