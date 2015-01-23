/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_math_alphabet
#include "../boost_unit_test.hpp"

#include "math/alphabet.hpp"

#include <string>

#include <boost/mpl/assert.hpp>
#include <boost/functional/hash.hpp>

#define CHECK_CONSTANT(expression) \
    BOOST_MPL_ASSERT ((rime::equal_constant <decltype (expression), \
        rime::true_type>));

BOOST_AUTO_TEST_SUITE(test_suite_math_alphabet)

template <class Integer> Integer add_one (Integer i) { return i + Integer (1); }

/**
Check whether integer overflows are detected.
This should be the case for GCC with -ftrapv, which is worth switching on for
this test.
In that case, you want to comment out the below, unless you would like to check
that the tests fail.
*/
BOOST_AUTO_TEST_CASE (test_integer_overflow_detection) {
/*    BOOST_TEST_CHECKPOINT ("Check that integer overflows are detected");
    int i = std::numeric_limits <int>::max();
    int j = add_one (i);
    // Use j to prevent compiler warning.
    j += 1;*/
}

struct word;

struct empty {
    rime::true_type operator == (empty const & other) const
    { return rime::true_; }
};

struct phi {
    rime::true_type operator == (phi const & other) const
    { return rime::true_; }
};

struct rho {
    rime::true_type operator == (rho const & other) const
    { return rime::true_; }
};

template <class Symbol1, class Symbol2> inline
    void check_comparison (Symbol1 const & symbol1, Symbol2 const & symbol2)
{
    if (symbol1 == symbol2) {
        BOOST_CHECK (!(symbol1 != symbol2));

        BOOST_CHECK_EQUAL (boost::hash <Symbol1>() (symbol1),
            boost::hash <Symbol2>() (symbol2));

        BOOST_CHECK (!(symbol1 < symbol2));
        BOOST_CHECK (symbol1 <= symbol2);
        BOOST_CHECK (!(symbol1 > symbol2));
        BOOST_CHECK (symbol1 >= symbol2);
    } else {
        BOOST_CHECK (symbol1 != symbol2);

        // The hashes are not guaranteed to be unequal, but it is very unlikely
        // that they are not.
        BOOST_CHECK (boost::hash <Symbol1>() (symbol1)
            != boost::hash <Symbol2>() (symbol2));

        if (symbol1 < symbol2) {
            BOOST_CHECK (symbol1 <= symbol2);
            BOOST_CHECK (!(symbol1 > symbol2));
            BOOST_CHECK (!(symbol1 >= symbol2));
        } else {
            BOOST_CHECK (!(symbol1 <= symbol2));
            BOOST_CHECK (symbol1 > symbol2);
            BOOST_CHECK (symbol1 >= symbol2);
        }
    }
}

BOOST_AUTO_TEST_CASE (test_math_alphabet) {
    // Reserve room for only 2 normal and two special symbols, so that overflow
    // detection can be tested.
    typedef math::alphabet <std::string, word, 2, meta::vector<>, 2>
        alphabet_type;
    alphabet_type alphabet;

    auto test_symbol = alphabet.add_symbol ("test");
    BOOST_CHECK_EQUAL (test_symbol.id(), 0);
    BOOST_CHECK (!alphabet.is_special_symbol (test_symbol));
    BOOST_CHECK (alphabet.is_symbol_type <std::string> (test_symbol));
    BOOST_CHECK_EQUAL (alphabet.get_symbol <std::string> (test_symbol),
        "test");

    auto test_symbol_2 = alphabet.add_symbol ("test");
    BOOST_CHECK (!alphabet.is_special_symbol (test_symbol_2));
    BOOST_CHECK (test_symbol == test_symbol_2);
    check_comparison (test_symbol, test_symbol_2);
    BOOST_CHECK_EQUAL (alphabet.get_symbol <std::string> (test_symbol_2),
        "test");

    auto hello_symbol = alphabet.add_symbol ("hello");
    BOOST_CHECK (!alphabet.is_special_symbol (hello_symbol));
    BOOST_CHECK_EQUAL (hello_symbol.id(), 1);
    BOOST_CHECK (test_symbol != hello_symbol);
    check_comparison (test_symbol, hello_symbol);
    BOOST_CHECK_EQUAL (alphabet.get_symbol <std::string> (hello_symbol),
        "hello");

    // The alphabet can only contain two normal symbols.
    BOOST_CHECK_THROW (
        alphabet.add_symbol ("new_symbol"), math::alphabet_overflow);

    // Other symbols should be fine.
    BOOST_CHECK_EQUAL (alphabet.add_symbol ("hello").id(), 1);
    BOOST_CHECK_EQUAL (alphabet.get_dense ("hello").id(), 1);

    /* Add a special symbol. */
    auto alphabet2 = math::add_special_symbol <empty> (alphabet);

    // Retrieve the compile-time symbol for empty().
    auto dense_symbol_for_empty = alphabet2.get_dense (empty());
    CHECK_CONSTANT (alphabet2.is_special_symbol (dense_symbol_for_empty));
    CHECK_CONSTANT (dense_symbol_for_empty == dense_symbol_for_empty);

    BOOST_CHECK (alphabet2.is_symbol_type <std::string> (test_symbol));
    CHECK_CONSTANT (
        !alphabet2.is_symbol_type <std::string> (dense_symbol_for_empty));
    BOOST_CHECK (!alphabet2.is_symbol_type <empty> (test_symbol));
    CHECK_CONSTANT (
        alphabet2.is_symbol_type <empty> (dense_symbol_for_empty));

    BOOST_CHECK_EQUAL (alphabet2.get_symbol <std::string> (hello_symbol),
        "hello");

    {
        auto empty_again = alphabet2.get_symbol <empty> (
            dense_symbol_for_empty);
        BOOST_MPL_ASSERT ((std::is_same <decltype (empty_again), empty>));
    }

    BOOST_MPL_ASSERT ((
        rime::is_constant <decltype (dense_symbol_for_empty.id())>));
    typedef decltype (dense_symbol_for_empty.id())
        dense_id_for_empty_type;
    static_assert (
        dense_id_for_empty_type::value ==
        dense_id_for_empty_type::value_type (-1),
        "First special symbol must be -1.");

    // Convert empty into a run-time symbol.
    alphabet_type::dense_symbol_type general_symbol_for_empty
        = dense_symbol_for_empty;
    BOOST_CHECK (alphabet2.is_special_symbol (general_symbol_for_empty));
    BOOST_CHECK_EQUAL (general_symbol_for_empty.id(), -1);

    BOOST_CHECK_EQUAL (alphabet2.get_dense ("hello").id(), 1);

    /*
    Trigger a static assertion by initialising from an alphabet with an
    incompatible list of special symbols.
    */
    /*{
        auto alphabet3 = math::add_special_symbol <phi> (alphabet);
        decltype (alphabet3) alphabet4 = alphabet2;
    }*/

    /* Add the same special symbol again; shouldn't make a difference. */
    {
        auto alphabet3 = math::add_special_symbol <empty> (alphabet2);
        auto dense_symbol_for_empty_3 = alphabet2.get_dense (empty());
        CHECK_CONSTANT (
            alphabet3.is_special_symbol (dense_symbol_for_empty_3));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (dense_symbol_for_empty),
            decltype (dense_symbol_for_empty_3)>));
        (void) (dense_symbol_for_empty == dense_symbol_for_empty_3);
        BOOST_MPL_ASSERT ((rime::equal_constant <
            decltype (dense_symbol_for_empty == dense_symbol_for_empty_3),
            rime::true_type>));
    }

    /* Add a new special symbol. */
    auto alphabet4 = math::add_special_symbol <phi> (alphabet2);
    auto dense_symbol_for_phi = alphabet4.get_dense (phi());
    CHECK_CONSTANT (alphabet4.is_special_symbol (dense_symbol_for_phi));
    BOOST_MPL_ASSERT ((rime::is_constant <
        decltype (dense_symbol_for_phi.id())>));
    typedef decltype (dense_symbol_for_phi.id())
        dense_symbol_for_phi_type;
    static_assert (
        dense_symbol_for_phi_type::value ==
        dense_symbol_for_phi_type::value_type (-2),
        "Second special symbol must be -2.");

    // Convert phi into a run-time symbol.
    alphabet_type::dense_symbol_type general_symbol_for_phi
        = dense_symbol_for_phi;
    BOOST_CHECK (alphabet4.is_special_symbol (general_symbol_for_phi));
    BOOST_CHECK_EQUAL (general_symbol_for_phi.id(), -2);

    CHECK_CONSTANT (dense_symbol_for_phi == dense_symbol_for_phi);
    CHECK_CONSTANT (dense_symbol_for_empty != dense_symbol_for_phi);
    CHECK_CONSTANT (dense_symbol_for_phi != dense_symbol_for_empty);

    BOOST_CHECK (dense_symbol_for_phi == general_symbol_for_phi);
    BOOST_CHECK (dense_symbol_for_empty != general_symbol_for_phi);

    // is_symbol_type.
    // test_symbol represents a string.
    BOOST_CHECK (alphabet4.is_symbol_type <std::string> (test_symbol));
    BOOST_CHECK (!alphabet4.is_symbol_type <empty> (test_symbol));
    BOOST_CHECK (!alphabet4.is_symbol_type <phi> (test_symbol));

    // dense_symbol_for_empty.
    CHECK_CONSTANT (
        !alphabet4.is_symbol_type <std::string> (dense_symbol_for_empty));
    CHECK_CONSTANT (
        alphabet4.is_symbol_type <empty> (dense_symbol_for_empty));
    CHECK_CONSTANT (
        !alphabet4.is_symbol_type <phi> (dense_symbol_for_empty));
    // general_symbol_for_empty.
    BOOST_CHECK (
        !alphabet4.is_symbol_type <std::string> (general_symbol_for_empty));
    BOOST_CHECK (alphabet4.is_symbol_type <empty> (general_symbol_for_empty));
    BOOST_CHECK (!alphabet4.is_symbol_type <phi> (general_symbol_for_empty));

    // dense_symbol_for_phi.
    CHECK_CONSTANT (
        !alphabet4.is_symbol_type <std::string> (dense_symbol_for_phi));
    CHECK_CONSTANT (
        !alphabet4.is_symbol_type <empty> (dense_symbol_for_phi));
    CHECK_CONSTANT (alphabet4.is_symbol_type <phi> (dense_symbol_for_phi));
    // general_symbol_for_phi.
    BOOST_CHECK (
        !alphabet4.is_symbol_type <std::string> (general_symbol_for_phi));
    BOOST_CHECK (!alphabet4.is_symbol_type <empty> (general_symbol_for_phi));
    BOOST_CHECK (alphabet4.is_symbol_type <phi> (general_symbol_for_phi));

    BOOST_CHECK_EQUAL (alphabet4.get_dense ("hello").id(), 1);

    {
        auto empty_again = alphabet4.get_symbol <empty> (
            dense_symbol_for_empty);
        BOOST_MPL_ASSERT ((std::is_same <decltype (empty_again), empty>));
    }
    {
        auto empty_again = alphabet4.get_symbol <empty> (
            general_symbol_for_empty);
        BOOST_MPL_ASSERT ((std::is_same <decltype (empty_again), empty>));
    }
    {
        auto phi_again = alphabet4.get_symbol <phi> (dense_symbol_for_phi);
        BOOST_MPL_ASSERT ((std::is_same <decltype (phi_again), phi>));
    }
    {
        auto phi_again = alphabet4.get_symbol <phi> (general_symbol_for_phi);
        BOOST_MPL_ASSERT ((std::is_same <decltype (phi_again), phi>));
    }

    // This triggers a static assertion, because room for only 2 special symbols
    // was reserved.
    // auto alphabet5 = math::add_special_symbol <rho> (alphabet4);
}

// Test with default parameters.
BOOST_AUTO_TEST_CASE (test_math_alphabet_default) {
    typedef math::alphabet <std::string, word> alphabet_type;
    alphabet_type alphabet;

    auto dense_hello = alphabet.add_symbol ("hello");
    static_assert (std::is_same <decltype (dense_hello.id()),
        std::int_least32_t>::value,
        "By default, a 32-bit integer should be used.");
    BOOST_CHECK_EQUAL (dense_hello.id(), 0);

    auto alphabet2 = math::add_special_symbol <empty> (alphabet);
    BOOST_CHECK_EQUAL (alphabet2.get_dense (empty()).id(), -1);

    typedef decltype (alphabet2) alphabet2_type;
    alphabet2_type::dense_symbol_type symbol = dense_hello;

    BOOST_CHECK (symbol == dense_hello);
    BOOST_CHECK_EQUAL (symbol.id(), dense_hello.id());
    check_comparison (symbol, dense_hello);

    symbol = alphabet2.get_dense (empty());
    auto dense_empty = alphabet2.get_dense (empty());
    BOOST_CHECK (symbol == dense_empty);
    BOOST_CHECK_EQUAL (symbol.id(), dense_empty.id());
    check_comparison (symbol, dense_empty);

    BOOST_CHECK (symbol != dense_hello);
    check_comparison (symbol, dense_hello);
    BOOST_CHECK (dense_empty != dense_hello);
    check_comparison (dense_empty, dense_hello);

    // Add symbols to original alphabet and they should go into alphabet2 as
    // well.
    alphabet.add_symbol ("q");
    BOOST_CHECK_EQUAL (alphabet.get_dense ("q").id(), 1);
    BOOST_CHECK_EQUAL (alphabet2.get_dense ("q").id(), 1);
}

BOOST_AUTO_TEST_CASE (test_math_alphabet_tag) {
    typedef math::alphabet <std::string, word> alphabet_type;
    alphabet_type alphabet;

    typedef math::dense_symbol <std::int_least32_t, word> dense_symbol;
    BOOST_MPL_ASSERT ((std::is_convertible <dense_symbol, dense_symbol>));

    auto dense_1 = alphabet.add_symbol ("four");
    // Use dense_1 to prevent a compiler error.
    dense_1 = alphabet.add_symbol ("five");

    typedef math::dense_symbol <std::int_least32_t, int> dense_symbol_wrong_tag;
    // Symbols with different tags should not be convertible.
    BOOST_MPL_ASSERT_NOT ((
        std::is_convertible <dense_symbol, dense_symbol_wrong_tag>));

    // Try to compare symbols with different tags: compiler error.
    // dense_symbol_wrong_tag() == dense_2;

    // Try to use a symbol with the wrong tag: compiler error.
    // alphabet.template get_symbol <std::string> (dense_symbol_wrong_tag());
}

template <class Alphabet, class Type> struct take_type_only {
    Alphabet const & alphabet;
    int & visit_count;

    take_type_only (Alphabet const & alphabet, int & visit_count)
    : alphabet (alphabet), visit_count (visit_count) {}

    template <class Symbol> void operator() (
        math::symbol_type_tag <Type>, Symbol const & symbol) const
    {
        ++ visit_count;
        alphabet.template get_symbol <Type> (symbol);
    }
};

template <class Alphabet> struct take_type {
    Alphabet const & alphabet;
    int & string_count;
    int & empty_count;
    int & phi_count;

    take_type (Alphabet const & alphabet,
        int & string_count, int & empty_count, int & phi_count)
    : alphabet (alphabet), string_count (string_count),
        empty_count (empty_count), phi_count (phi_count) {}

    template <class Symbol> void operator() (
        math::symbol_type_tag <std::string>, Symbol const & symbol) const
    {
        ++ string_count;
        alphabet.template get_symbol <std::string> (symbol);
    }

    template <class Symbol> void operator() (
        math::symbol_type_tag <empty>, Symbol const & symbol) const
    {
        ++ empty_count;
        alphabet.template get_symbol <empty> (symbol);
    }

    template <class Symbol> void operator() (
            math::symbol_type_tag <phi>, Symbol const & symbol) const
    {
        ++ phi_count;
        alphabet.template get_symbol <phi> (symbol);
    }
};

struct take {
    std::string & last_string;
    int & string_count;
    int & empty_count;
    int & phi_count;

    take (std::string & last_string,
        int & string_count, int & empty_count, int & phi_count)
    : last_string (last_string), string_count (string_count),
        empty_count (empty_count), phi_count (phi_count) {}

    void operator() (std::string const & s) const {
        ++ string_count;
        last_string = s;
    }

    void operator() (empty) const { ++ empty_count; }

    void operator() (phi) const { ++ phi_count; }
};

template <class Type> struct take_only {
    int & visit_count;

    take_only (int & visit_count)
    : visit_count (visit_count) {}

    void operator() (Type const &) const { ++ visit_count; }
};


BOOST_AUTO_TEST_CASE (test_math_alphabet_visit) {
    auto alphabet = math::add_special_symbol <phi> (
        math::add_special_symbol <empty> (
            math::alphabet <std::string, word>()));
    typedef decltype (alphabet) alphabet_type;

    alphabet.add_symbol ("one");
    alphabet.add_symbol ("two");

    /* Test visit_type. */
    {
        int string_count = 0;
        int empty_count = 0;
        int phi_count = 0;
        take_type <alphabet_type> take (
            alphabet, string_count, empty_count, phi_count);

        auto symbol = alphabet.get_dense ("one");
        alphabet.visit_type (take, symbol);
        BOOST_CHECK_EQUAL (string_count, 1);
        BOOST_CHECK_EQUAL (empty_count, 0);
        BOOST_CHECK_EQUAL (phi_count, 0);

        symbol = alphabet.get_dense (empty());
        alphabet.visit_type (take, symbol);
        BOOST_CHECK_EQUAL (string_count, 1);
        BOOST_CHECK_EQUAL (empty_count, 1);
        BOOST_CHECK_EQUAL (phi_count, 0);

        symbol = alphabet.get_dense (phi());
        alphabet.visit_type (take, symbol);
        BOOST_CHECK_EQUAL (string_count, 1);
        BOOST_CHECK_EQUAL (empty_count, 1);
        BOOST_CHECK_EQUAL (phi_count, 1);
    }

    {
        int visit_count = 0;
        take_type_only <alphabet_type, empty> take (alphabet, visit_count);
        alphabet.visit_type (take, alphabet.get_dense (empty()));
    }
    {
        int visit_count = 0;
        take_type_only <alphabet_type, phi> take (alphabet, visit_count);
        alphabet.visit_type (take, alphabet.get_dense (phi()));
    }

    /* Test visit. */
    {
        std::string last_string;
        int string_count = 0;
        int empty_count = 0;
        int phi_count = 0;
        take t (last_string, string_count, empty_count, phi_count);

        auto symbol = alphabet.get_dense ("one");
        alphabet.visit (t, symbol);
        BOOST_CHECK_EQUAL (string_count, 1);
        BOOST_CHECK_EQUAL (last_string, "one");
        BOOST_CHECK_EQUAL (empty_count, 0);
        BOOST_CHECK_EQUAL (phi_count, 0);

        symbol = alphabet.get_dense (empty());
        alphabet.visit (t, symbol);
        BOOST_CHECK_EQUAL (string_count, 1);
        BOOST_CHECK_EQUAL (empty_count, 1);
        BOOST_CHECK_EQUAL (phi_count, 0);

        symbol = alphabet.get_dense (phi());
        alphabet.visit (t, symbol);
        BOOST_CHECK_EQUAL (string_count, 1);
        BOOST_CHECK_EQUAL (empty_count, 1);
        BOOST_CHECK_EQUAL (phi_count, 1);

        symbol = alphabet.get_dense ("two");
        alphabet.visit (t, symbol);
        BOOST_CHECK_EQUAL (string_count, 2);
        BOOST_CHECK_EQUAL (last_string, "two");
        BOOST_CHECK_EQUAL (empty_count, 1);
        BOOST_CHECK_EQUAL (phi_count, 1);
    }

    {
        int visit_count = 0;
        take_only <empty> t (visit_count);
        alphabet.visit (t, alphabet.get_dense (empty()));
        // This causes a compile-time error:
        // alphabet.visit (t, alphabet.get_dense (phi()));
    }
    {
        int visit_count = 0;
        take_only <phi> t (visit_count);
        alphabet.visit (t, alphabet.get_dense (phi()));
    }
}

BOOST_AUTO_TEST_SUITE_END()
