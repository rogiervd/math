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
