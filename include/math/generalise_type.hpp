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

#ifndef MATH_GENERALISE_TYPE_HPP_INCLUDED
#define MATH_GENERALISE_TYPE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>

#include "meta/vector.hpp"

#include "magma.hpp"

namespace math {

namespace detail {

    template <class Operations, class Magma> struct generalise_type_once;

    template <class Magma> struct generalise_type_once <meta::vector<>, Magma>
    { typedef Magma type; };

    template <class FirstOperation, class ... Operations, class Magma>
        struct generalise_type_once <
            meta::vector <FirstOperation, Operations ...>, Magma>
    {
        typedef typename std::result_of <FirstOperation (Magma, Magma)>::type
            operation_result_type;
        typedef typename merge_magma::apply <Magma, operation_result_type>::type
            next_type;
        typedef typename generalise_type_once <
            meta::vector <Operations ...>, next_type>::type type;
    };

} // namespace detail

/**
Convert a magma type to a type in the same magma that can contain the result of
any combination of a number of binary operations applied to it.

For example, a \ref single_sequence under \ref plus will generalise to
\ref optional_sequence; under \ref times (or under both) it will generalise to a
\ref sequence.

\tparam Operations
    A meta::vector of binary operations, usually from namespace callable.
\tparam Magma
    The magma type to be generalised.
*/
template <class Operations, class Magma> struct generalise_type {
    typedef typename std::decay <Magma>::type magma_type;
    typedef typename detail::generalise_type_once <Operations, magma_type>::type
        next_type;

    typedef typename boost::mpl::eval_if <
            std::is_same <Magma, next_type>,
            boost::mpl::identity <Magma>,
            generalise_type <Operations, next_type>
        >::type type;
};

} // namespace math

#endif  // MATH_GENERALISE_TYPE_HPP_INCLUDED
