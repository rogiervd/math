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
