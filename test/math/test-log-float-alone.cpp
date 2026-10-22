// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test log_float.hpp on its own, without including IO headers.
*/

#include <iosfwd>
#include "math/log-float.hpp"

/**
Instantiate the output routine without creating an ostream instance.
*/
void print_log_float(std::ostream & os, math::log_float<float> const & f)
{
    os << f;
}

int main() { return 0; }
