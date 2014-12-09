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

/** \file
Helper for testing math::lexicographical.
*/

#ifndef MATH_TEST_MATH_TEST_LEXICOGRAPHICAL_MAKE_HPP_INCLUDED
#define MATH_TEST_MATH_TEST_LEXICOGRAPHICAL_MAKE_HPP_INCLUDED

#include "math/lexicographical.hpp"

#include <string>

#include "math/cost.hpp"
#include "math/sequence.hpp"

typedef math::cost <float> cost;
typedef math::lexicographical <math::over <cost, math::sequence <char>>>
    lexicographical;

// Normal.
inline lexicographical make_lexicographical (float cost, std::string symbols) {
    return lexicographical (
        math::cost <float> (cost), math::sequence <char> (symbols));
}

// Empty sequence.
inline math::lexicographical <math::over <cost, math::empty_sequence <char>>>
    make_empty_lexicographical (float c)
{
    return math::lexicographical <
        math::over <cost, math::empty_sequence <char>>> (
            cost (c), math::empty_sequence <char>());
}

// Single-symbol sequence.
inline math::lexicographical <math::over <cost, math::single_sequence <char>>>
    make_single_lexicographical (float c, char symbol)
{
    return math::lexicographical <
        math::over <cost, math::single_sequence <char>>> (
            cost (c), math::single_sequence <char> (symbol));
}

// Optional sequence.
inline math::lexicographical <math::over <cost, math::optional_sequence <char>>>
    make_optional_lexicographical (float c)
{
    return math::lexicographical <
        math::over <cost, math::optional_sequence <char>>> (
            cost (c), math::optional_sequence <char>());
}

inline math::lexicographical <math::over <cost, math::optional_sequence <char>>>
    make_optional_lexicographical (float c, char symbol)
{
    return math::lexicographical <
        math::over <cost, math::optional_sequence <char>>> (
            cost (c), math::optional_sequence <char> (symbol));
}

// Annihilator.
inline math::lexicographical <
    math::over <cost, math::sequence_annihilator <char>>>
make_annihilator_lexicographical (float c)
{
    return math::lexicographical <
        math::over <cost, math::sequence_annihilator <char>>> (
            cost (c), math::sequence_annihilator <char>());
}

#endif // MATH_TEST_MATH_TEST_LEXICOGRAPHICAL_MAKE_HPP_INCLUDED
