/*
Copyright 2015 Rogier van Dalen.

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
