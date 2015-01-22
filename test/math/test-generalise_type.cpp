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

#define BOOST_TEST_MODULE test_generalise_type
#include "utility/test/boost_unit_test.hpp"

#include "math/generalise_type.hpp"

#include "math/arithmetic_magma.hpp"
#include "math/cost.hpp"
#include "math/sequence.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_generalise_type)

BOOST_AUTO_TEST_CASE (test_generalise_type) {
    // No operations.
    static_assert (std::is_same <math::generalise_type <
            meta::vector<>,
            int
        >::type, int>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector<>, math::cost <float>>::type,
        math::cost <float>>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector<>,
            math::single_sequence <char>
        >::type, math::single_sequence <char>>::value, "");

    // plus.
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::plus>,
            int
        >::type, int>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::plus>,
            math::cost <float> const &
        >::type, math::cost <float>>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::plus>,
            math::single_sequence <char> const
        >::type, math::optional_sequence <char>>::value, "");

    // times.
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::times>,
            int
        >::type, int>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::times>,
            math::cost <float>
        >::type, math::cost <float>>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::times>,
            math::single_sequence <char>
        >::type, math::sequence <char>>::value, "");

    // plus and times.
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::times, math::callable::plus>,
            int
        >::type, int>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::times, math::callable::plus>,
            math::cost <float>
        >::type, math::cost <float>>::value, "");
    static_assert (std::is_same <math::generalise_type <
            meta::vector <math::callable::times, math::callable::plus>,
            math::single_sequence <char>
        >::type, math::sequence <char>>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
