// SPDX-FileCopyrightText: Copyright 2026 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0


#define BOOST_TEST_MODULE test_math_alphabet_new
#include "gtest/gtest.h"

#include "boost/unordered/unordered_flat_set.hpp"

/**
 * Only normal elements.
 * Only add elements, never remove.
 */
template <class Tag, class UnsignedType> class normal_dense_alphabet
{
public:
    using Unsigned = UnsignedType;
    // TODO what could the symbol type be?

    normal_dense_alphabet() = default;

private:
    boost::unordered_flat_set<Unsigned> elements_;
};


TEST(Alphabet, Basics) {}
