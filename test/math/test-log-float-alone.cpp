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
