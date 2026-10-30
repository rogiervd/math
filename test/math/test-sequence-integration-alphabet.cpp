// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Integration check to see whether using dense symbols in a sequence works.
*/

#define BOOST_TEST_MODULE test_sequence_alphabet
#include "boost/test/unit_test.hpp"

#include "math/alphabet.hpp"
#include "math/sequence.hpp"

#include <string>

BOOST_AUTO_TEST_SUITE(test_suite_sequence_alphabet)

typedef math::alphabet<std::string> alphabet_type;

BOOST_AUTO_TEST_CASE(test_sequence_alphabet)
{
    alphabet_type alphabet;

    auto symbol1 = alphabet.add_symbol("hello");
    auto symbol2 = alphabet.add_symbol("bye");

    typedef decltype(symbol1) internal_symbol_type;

    math::empty_sequence<internal_symbol_type> empty_sequence;
    math::optional_sequence<internal_symbol_type> sequence1(symbol1);
    math::single_sequence<internal_symbol_type> sequence2(symbol2);

    // The first symbol will be chosen because dense symbols are not in
    // lexicographical order.
    BOOST_CHECK(math::choose(sequence1, sequence2) == sequence1);
    BOOST_CHECK(math::plus(sequence1, sequence2) == empty_sequence);

    auto sequence12 = sequence1 * sequence2;
    BOOST_CHECK_EQUAL(range::size(sequence12.symbols()), 2u);
    BOOST_CHECK(range::first(sequence12.symbols()) == symbol1);
    BOOST_CHECK(range::second(sequence12.symbols()) == symbol2);
}

BOOST_AUTO_TEST_SUITE_END()
