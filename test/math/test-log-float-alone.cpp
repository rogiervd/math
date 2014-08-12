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
Test log_float.hpp on its own, without including IO headers.
*/

#include <iosfwd>
#include "math/log-float.hpp"

/**
Instantiate the output routine without creating an ostream instance.
*/
void print_log_float (std::ostream & os, math::log_float <float> const & f)
{ os << f; }

int main() { return 0; }
