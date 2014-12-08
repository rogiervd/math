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
Define helpers for magmas that consist of other magmas.
*/

#ifndef MATH_DETAIL_TUPLE_HELPER_HPP_INCLUDED
#define MATH_DETAIL_TUPLE_HELPER_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include "meta/vector.hpp"
#include "meta/count.hpp"

#include "range/core.hpp"
#include "range/equal.hpp"
#include "range/less_lexicographical.hpp"

#include "rime/call_if.hpp"
#include "rime/enable_if_constant.hpp"

#include "../magma.hpp"

namespace math {

/** \struct over
Tag to indicate component types of a composite magma.
*/
template <class ... Components> struct over;

namespace operation {

    namespace tuple_helper {

        /**
        Specialise this for a magma type with an operator() that returns the
        components of the magma as a range.
        */
        template <class TupleTag> struct get_components;

        /**
        Compare a tuple for equality by checking the components in parallel.
        \tparam Equality
            The predicate to use for each component.
        */
        template <class Equality> struct equal_components {
            template <class Left, class Right>
                auto operator() (Left const & left, Right const & right) const
            RETURNS (range::equal (Equality(),
                get_components <typename magma_tag <Left>::type>() (left),
                get_components <typename magma_tag <Right>::type>() (right)));
        };

        /**
        Compare two tuples for equality, using is_annihilator <Operation> first.
        If both arguments are annihilators, compare equal.
        If either is an annihilator, compare unequal.
        Otherwise, forward to \a NormalEquality().
        */
        template <class Operation, class NormalEquality>
            class equal_if_annihilator
        {
            struct compare_annihilators {
                template <class Left, class Right>
                auto operator() (Left const & left, Right const & right) const
                RETURNS (math::is_annihilator <Operation> (left) ==
                    math::is_annihilator <Operation> (right));
            };

        public:
            template <class Left, class Right>
            auto operator() (Left const & left, Right const & right) const
            RETURNS (rime::call_if (
                rime::or_ (math::is_annihilator <Operation> (left),
                    math::is_annihilator <Operation> (right)),
                compare_annihilators(), NormalEquality(),
                left, right));
        };

        /**
        Compare two tuples by checking the components in parallel.
        \tparam Compare
            The predicate to use for each component.
        */
        template <class Compare> struct compare_components {
            template <class Left, class Right>
                auto operator() (Left const & left, Right const & right) const
            RETURNS (range::less_lexicographical (Compare(),
                get_components <typename magma_tag <Left>::type>() (left),
                get_components <typename magma_tag <Right>::type>() (right)));
        };

        /**
        Compare two tuples, ordering annihilators last.
        Annihilators for \a Operation are treated separately.
        Other elements are compared using \a NormalCompare().
        */
        template <class Operation, class NormalCompare>
            class compare_if_annihilator
        {
            // If either left or right is an annihilator:
            // true only if left is not an annihilator (and right is).
            struct not_is_annihilator_left {
                template <class Left, class Right>
                auto operator() (Left const & left, Right const &) const
                RETURNS (!math::is_annihilator <Operation> (left));
            };

        public:
            template <class Left, class Right>
            auto operator() (Left const & left, Right const & right) const
            RETURNS (rime::call_if (
                ! rime::or_ (math::is_annihilator <Operation> (left),
                    math::is_annihilator <Operation> (right)),
                NormalCompare(), not_is_annihilator_left(), left, right));
        };

        /**
        Implement a nullary operation which generates a tuple by calling a
        nullary operation for each element.
        */
        template <class Reassemble, class Operations, class Enable = void>
            struct nullary_operation
        : unimplemented {};

        template <class Reassemble, class ... Operations>
            struct nullary_operation <Reassemble, meta::vector <Operations ...>,
                typename boost::enable_if <meta::all <meta::vector <
                    is_implemented <Operations> ...>>>::type>
        : approximate_if <
            meta::any <meta::vector <is_approximate <Operations> ...>>>
        {
            auto operator() () const
            RETURNS (Reassemble() (Operations()() ...));
        };

        /**
        Implement a unary operation which takes the elements of a tuple in
        applies the sequence of operations to each of them, returning the
        results in a new tuple.
        This is implemented iff the operations are all implemented.
        This is marked as approximate iff any of the operations is.

        \tparam Reassemble
            Function class that is passed the new components as arguments.
        \tparam Operations
            Compile-time list of operations that are applied to the elements of
            the tuple.

        \internal
        Indices is a meta::vector of compile-time integrals used by the
        implementation.
        */
        template <class Reassemble, class Operations,
            class Indices =
                typename meta::count <meta::size <Operations>::value>::type,
            class Enable = void>
            struct unary_operation
        : unimplemented {};

        template <class Reassemble, class ... Operations, class ... Indices>
            struct unary_operation <Reassemble,
                meta::vector <Operations ...>, meta::vector <Indices ...>,
                typename boost::enable_if <meta::all <meta::vector <
                    is_implemented <Operations> ...>>>::type>
        : approximate_if <
            meta::any <meta::vector <is_approximate <Operations> ...>>>
        {
        private:
            // GCC 4.6 crashes if this is a static function instead of a class.
            struct compute {
                /*
                This is a telescoping implementation.
                "compute" is called with the tuple
                Operations is a list of operation functors.
                Indices is a list of compile-time constants 0, 1, 2...
                In the return value this is then expanded as
                Operation0() (components() [0]),
                Operation1() (components() [1]),
                Operation2() (components() [2])
                and so on.
                */
                template <class Components>
                    auto operator() (Components const & components) const
                RETURNS (Reassemble() (
                    Operations() (range::at (Indices(), components)) ...));
            };

        public:
            template <class Tuple> auto operator() (Tuple const & tuple) const
            RETURNS (compute() (
                get_components <typename magma_tag <Tuple>::type>() (tuple)));
        };

        /**
        Implement a binary operation which takes the elements of two tuples in
        parallel and applies the sequence of operations to each of them,
        returning the results in a new tuple.
        This is implemented iff the operations are all implemented.
        This is marked as approximate iff any of the operations is.

        \tparam Reassemble
            Function class that is passed the new components as arguments.
        \tparam Operations
            Compile-time list of operations that are applied to the elements of
            the tuples in parallel.

        \internal
        Indices is a meta::vector of compile-time integrals used by the
        implementation.
        */
        template <class Reassemble, class Operations,
            class Indices =
                typename meta::count <meta::size <Operations>::value>::type,
            class Enable = void>
        struct binary_operation
        : unimplemented {};

        template <class Reassemble, class ... Operations, class ... Indices>
            struct binary_operation <Reassemble,
                meta::vector <Operations ...>, meta::vector <Indices ...>,
                typename boost::enable_if <meta::all <meta::vector <
                    is_implemented <Operations> ...>>>::type>
        : approximate_if <
            meta::any <meta::vector <is_approximate <Operations> ...>>>,
        associative_if <
            meta::all <meta::vector <is_associative <Operations> ...>>>,
        commutative_if <
            meta::all <meta::vector <is_commutative <Operations> ...>>>,
        idempotent_if <
            meta::all <meta::vector <is_idempotent <Operations> ...>>>,
        path_operation_if <
            meta::all <meta::vector <is_path_operation <Operations> ...>>>
        {
        private:
            // GCC 4.6 crashes if this is a static function instead of a class.
            struct compute {
                /*
                This implementation is similar to that of unary_operation above,
                but now the left and right components are extracted in parallel.
                */
                template <class LeftComponents, class RightComponents>
                    auto operator() (LeftComponents const & left_components,
                        RightComponents const & right_components) const
                RETURNS (Reassemble() (
                    Operations() (
                        range::at (Indices(), left_components),
                        range::at (Indices(), right_components)) ...));
            };

        public:
            template <class Left, class Right> auto operator() (
                Left const & left, Right const & right) const
            RETURNS (compute() (
                get_components <typename magma_tag <Left>::type>() (left),
                get_components <typename magma_tag <Right>::type>() (right)));
        };

        /**
        Implement an operation that takes a stream and tuple, and outputs the
        elements of the tuple to the stream.
        Useful for implementing "print".
        */
        template <class ComponentTags, class Enable = void>
            class print_components
        : unimplemented {};

        template <class ... ComponentTags>
            class print_components <meta::vector <ComponentTags ...>, typename
                boost::enable_if <
                    meta::all <meta::vector <
                        is_implemented <operation::print <ComponentTags>> ...>>
                >::type>
        {
            // Known to be empty.
            template <class Stream, class Components, class Enable = typename
                rime::enable_if_constant_true <typename
                    range::result_of <range::callable::empty (Components)>::type
                >::type>
            void print_elements (Stream &, Components const &, bool) const {}

            // Not known to be empty at compile time.
            template <class Stream, class Components, class Enable = typename
                rime::disable_if_constant_true <typename
                    range::result_of <range::callable::empty (Components)>::type
                >::type>
            void print_elements (
                Stream & stream, Components const & components, bool first,
                void * = 0) const
            {
                if (!range::empty (components)) {
                    if (!first)
                        stream << ", ";
                    math::print (stream, range::first (components));
                    print_elements (stream, range::drop (components), false);
                }
            }

        public:
            template <class Stream, class Tuple>
                void operator() (Stream & stream, Tuple const & tuple) const
            {
                stream << '(';
                print_elements (stream, tuple.components(), true);
                stream << ')';
            }
        };

    } // namespace tuple_helper

} // namespace operation

namespace tuple_helper {

    /**
    Evaluate to \c true iff the two meta::vector's of components are of the
    same length, and the types in OtherComponents are explicitly convertible
    from the types in Components, but at least one is not implicitly
    convertible.
    This is useful for providing two overloads of a constructor, one of which
    is implicit and is only available if the conversion of all components is
    implicit, and one which is explicit and only available if the conversion
    is explicit for at least one component.
    */
    template <class Components, class OtherComponents>
        struct components_constructible_only
    : boost::mpl::and_ <
        utility::are_constructible <Components, OtherComponents>,
        boost::mpl::not_ <
            utility::are_convertible <OtherComponents, Components>>>
    {};

} // namespace tuple_helper

} // namespace math

#endif // MATH_DETAIL_TUPLE_HELPER_HPP_INCLUDED