/*
Copyright 2013-2015 Rogier van Dalen.

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
Define magma operations.

Queries:
- is_member
- equal
- approximately_equal

Produce:
- non_member
- identity
- annihilator
- zero
- one

Binary operations:
- pick
- choose
- times
- plus
- divide
- minus

Unary operations:
- invert
- reverse
- print

Queries about operations:
- associative
- commutative
- idempotent
- path_operation
- distributive
- monoid
- semiring

\todo Closure.
\todo Implement vector space requirements.
\todo Expectation semiring (normalised and unnormalised), requires vector space.
*/

#ifndef MATH_MAGMA_HPP_INCLUDED
#define MATH_MAGMA_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/placeholders.hpp>

#include "utility/returns.hpp"
#include "utility/nested_callable.hpp"

#include "rime/core.hpp"
#include "rime/if.hpp"
#include "rime/always.hpp"

namespace math {

/** \struct math::either
Indicates either left or right direction for operations that take an operation.
This can only be used if the direction is irrelevant.
For example, for real numbers, there is no difference between left and right
division.
This class is used only at compile-time and remains incomplete.
*/
struct either;

/** \struct math::left
Indicates left direction for operations that take an operation.
This class is used only at compile-time and remains incomplete.
*/
struct left;

/** \struct math::right
Indicates right direction for operations that take an operation.
This class is used only at compile-time and remains incomplete.
*/
struct right;

/** \struct opposite_direction
Reverse a direction type.
<c>opposite_direction<math::left>::type</c> is \c math::right and vice versa.
*/
template <class Direction> struct opposite_direction;

template <> struct opposite_direction <left> { typedef right type; };
template <> struct opposite_direction <right> { typedef left type; };

/**
Metafunction.
\return Whether \a Type is a direction.
    This can be useful for SFINAE.
*/
template <class Type> struct is_direction : boost::mpl::false_ {};

template <> struct is_direction <either> : boost::mpl::true_ {};
template <> struct is_direction <left> : boost::mpl::true_ {};
template <> struct is_direction <right> : boost::mpl::true_ {};

namespace apply {

    template <class ... Arguments> struct is_member;
    template <class ... Arguments> struct is_annihilator;
    template <class ... Arguments> struct equal;
    template <class ... Arguments> struct not_equal;
    template <class ... Arguments> struct approximately_equal;

    template <class ... Arguments> struct non_member;
    template <class ... Arguments> struct identity;
    template <class ... Arguments> struct annihilator;
    template <class ... Arguments> struct one;
    template <class ... Arguments> struct zero;

    template <class ... Arguments> struct compare;
    template <class ... Arguments> struct order;

    template <class ... Arguments> struct pick;
    template <class ... Arguments> struct choose;
    template <class ... Arguments> struct times;
    template <class ... Arguments> struct plus;
    template <class ... Arguments> struct divide;
    template <class ... Arguments> struct minus;

    template <class ... Arguments> struct invert;
    template <class ... Arguments> struct reverse;
    template <class ... Arguments> struct closure;

    template <class ... Arguments> struct print;

    template <class ... Arguments> struct inverse_operation;

} // namespace apply

namespace callable {

    /**
    Generic callable that forwards to \a Apply, with the compile-time arguments
    and the function call arguments as template arguments.
    It provides an operator() and a template struct \a apply which derives from
    \a Apply with the compile-time and run-time arguments.
    \tparam Apply a variadic template class.
        When the object is called,
        <c> Apply <CompileTimeArguments ..., Arguments...></c>
        will be instantiated with the exact argument types.
        It is then default-constructed and called.
    \tparam CompileTimeArguments The compile-time arguments.
    */
    template <template <class ...> class Apply, class ... CompileTimeArguments>
        struct generic
    {
        /**
        Derives from <c> Apply <CompileTimeArguments ..., Arguments ...></c>.
        \tparam Arguments The run-time arguments.
        */
        template <class ... Arguments> struct apply
        : Apply <CompileTimeArguments ..., Arguments ...> {};

        /**
        Forward the call to a default-constructed object of type
        <c> apply <Arguments ...></c>.
        */
        template <class ... Arguments> auto
            operator() (Arguments && ... arguments) const
        -> typename std::result_of <apply <Arguments ...> (Arguments ...)>::type
        {
            return apply <Arguments ...>() (
                std::forward <Arguments> (arguments) ...);
        }
    };

    namespace detail {

        template <template <class ...> class Apply, class ... Arguments>
            struct apply_first_argument_as_compile_time
        : Apply <Arguments ...>
        {
            template <class FirstArgument, class ... OtherArguments>
                typename std::result_of <
                    Apply <Arguments ...> const (OtherArguments ...)>::type
            operator() (FirstArgument &&, OtherArguments && ... arguments) const
            {
                return Apply <Arguments ...>()
                    (std::forward <OtherArguments> (arguments) ...);
            }
        };

    } // namespace detail

    template <template <class ...> class Apply>
        struct first_argument_as_compile_time
    {
        template <class ... Arguments> struct apply
        : detail::apply_first_argument_as_compile_time <Apply, Arguments ...>
        {};
    };

    struct is_member : generic <apply::is_member> {};

    template <class Operation = void> struct is_annihilator
    : generic <apply::is_annihilator, Operation> {};
    template <> struct is_annihilator <>
    : generic <first_argument_as_compile_time <apply::is_annihilator>::apply>
    {};

    struct equal : generic <apply::equal> {};
    struct not_equal : generic <apply::not_equal> {};
    struct approximately_equal : generic <apply::approximately_equal> {};

    template <class Magma> struct non_member
    : generic <apply::non_member, Magma> {};

    // identity.
    template <class Magma, class Operation = void> struct identity
    : generic <apply::identity, Magma, Operation> {};
    template <class Magma> struct identity <Magma>
    : generic <
        first_argument_as_compile_time <apply::identity>::apply, Magma> {};

    // annihilator.
    template <class Magma, class Operation = void> struct annihilator
    : generic <apply::annihilator, Magma, Operation> {};
    template <class Magma> struct annihilator <Magma>
    : generic <
        first_argument_as_compile_time <apply::annihilator>::apply, Magma> {};

    template <class Magma> struct one : generic <apply::one, Magma> {};
    template <class Magma> struct zero : generic <apply::zero, Magma> {};

    struct compare : generic <apply::compare> {};

    template <class Operation> struct order
    : generic <apply::order, Operation> {};

    struct pick : generic <apply::pick> {};
    struct choose : generic <apply::choose> {};
    struct times : generic <apply::times> {};
    struct plus : generic <apply::plus> {};

    template <class Direction = either> struct divide
    : generic <apply::divide, Direction> {};

    template <class Direction = either> struct minus
    : generic <apply::minus, Direction> {};

    // invert.
    // Direction is optional.
    // The operation can be given as a run-time argument or as a compile-time
    // argument.
    template <class Direction = void, class Operation = void> struct invert
    : generic <apply::invert, Direction, Operation> {};

    template <class Operation>
        struct invert <Operation,
            typename boost::disable_if <is_direction <Operation>>::type>
    : generic <apply::invert, either, Operation> {};

    template <class Direction>
        struct invert <Direction,
            typename boost::enable_if <is_direction <Direction>>::type>
    : generic <first_argument_as_compile_time <apply::invert>::apply, Direction>
    {};

    template <> struct invert<>
    : generic <first_argument_as_compile_time <apply::invert>::apply, either>
    {};

    // reverse.
    template <class Operation = void> struct reverse
    : generic <apply::reverse, Operation> {};
    template <> struct reverse<>
    : generic <first_argument_as_compile_time <apply::reverse>::apply> {};

    struct print : generic <apply::print> {};

    // inverse_operation.
    // Direction is optional.
    // The operation can be given as a run-time argument or as a compile-time
    // argument.
    template <class Direction = void, class Operation = void>
        struct inverse_operation
    : generic <apply::inverse_operation, Direction, Operation> {};

    template <class Operation>
        struct inverse_operation <Operation,
            typename boost::disable_if <is_direction <Operation>>::type>
    : generic <apply::inverse_operation, either, Operation> {};

    template <class Direction>
        struct inverse_operation <Direction,
            typename boost::enable_if <is_direction <Direction>>::type>
    : generic <first_argument_as_compile_time <apply::inverse_operation>::apply,
        Direction> {};

    template <> struct inverse_operation<>
    : generic <first_argument_as_compile_time <apply::inverse_operation>::apply,
        either> {};

} // namespace callable

/** \struct math::not_a_magma_tag
Incomplete type that indicates that is the magma tag if a type is not in a
magma.
*/
struct not_a_magma_tag;

/**
Specialise this to assign a magma tag to a type.
It would be possible to specialise math::magma_tag instead.
However, by default that forwards to this, but with qualifiers removed from
\a Magma, so specialising this is easier.
\tparam Magma the unqualified type for which the magma tag is required.
*/
template <class Magma, class Enable = void> struct decayed_magma_tag {
    /**
    Magma tag.
    If this struct is not specialised, then it is math::not_a_magma_tag to
    indicate that the type \a Magma is not a part of a magma.
    */
    typedef not_a_magma_tag type;
};

/**
Metafunction that assigns magma tags to types.
All types that have the same magma tag are in the same magma.
\tparam Magma The type for which the magma tag is required.
\return The magma tag.
*/
template <class Magma> struct magma_tag
: decayed_magma_tag <typename std::decay <Magma>::type> {};

namespace detail {

    /**
    Return the magma tag if all argument types are of the same magma;
    not_a_magma_tag if not.
    */
    template <class ... MagmaTags> struct magma_tag_all_implementation
    { typedef not_a_magma_tag type; };

    template <class MagmaTag> struct magma_tag_all_implementation <MagmaTag>
    { typedef MagmaTag type; };

    template <class MagmaTag, class ... Other>
        struct magma_tag_all_implementation <MagmaTag, MagmaTag, Other ...>
    : magma_tag_all_implementation <MagmaTag, Other ...> {};

} // namespace detail

/**
\return The magma tag if it is equal for all arguments.
Otherwise, not_a_magma_tag.
This is useful for operations that work on multiple arguments that must all be
of the same magma.
*/
template <class ... Magmas> struct magma_tag_all
: detail::magma_tag_all_implementation <typename magma_tag <Magmas>::type ...>
{};

/**
Metafunction that indicates whether a type is in a magma.
\tparam Magma The type for which the status is required.
\return \c true iff \a Magma is in a magma, i.e. its magma tag is not
    not_a_magma_tag.
*/
template <class Magma> struct is_magma
: boost::mpl::not_ <std::is_same <
    typename magma_tag <Magma>::type, not_a_magma_tag>> {};

namespace operation {

    /**
    Inherit from this class to indicate that the operation is not implemented
    (for specific arguments).
    */
    struct unimplemented {/*
        typedef unimplemented type;

        // Make false_type depend on Arguments so the static assertion is
        // triggered only if operator() is actually called.
        template <class ... Arguments> struct false_type : rime::false_type {};

        template <class ... Arguments>
            int operator() (Arguments const & ...) const
        {
            static_assert (false_type <Arguments ...>::value,
                "Magma operation not implemented.");
            return 0;
        }*/
    };

    /**
    Metafunction that indicates whether an operation is implemented.
    \return \c true iff \a Operation is not derived from unimplemented.
    */
    template <class Operation> struct is_implemented
    : boost::mpl::not_ <std::is_base_of <unimplemented, Operation>> {};

    /**
    Inherit from this class to indicate that an operation is approximate.
    This can be used in testing to require that results are not exact, but
    close.
    For example, for floating point values, plus and times are associative only
    approximately, because intermediate values may be rounded.
    */
    struct approximate {};

    /**
    Inherit an inverse operation from this class to indicate that it throws
    operation_undefined iff it is called with arguments that it is not defined
    for.
    In particular, if an attempt is made to take the inverse of an annihilator,
    inverse_of_annihilator (a subclass of operation_undefined) should be thrown.

    For example, an inverse operation (like "divide") or "invert" could derive
    from this.
    */
    struct throw_if_undefined {};

    /**
    Inherit from this class to indicate that an operation is associative.
    */
    struct associative {};

    /**
    Inherit from this class to indicate that an operation is commutative.
    */
    struct commutative {};

    /**
    Inherit from this class to indicate that an operation is idempotent.
    If an operation is marked as aa path operation, it is automatically also
    marked as idempotent.
    */
    struct idempotent {};

    /**
    Inherit from this class to indicate that an operation is path_operation.
    If an operation is marked as aa path operation, it is automatically also
    marked as idempotent.
    */
    struct path_operation {};

    /**
    Meaningless and empty class to derive from to indicate nothing.
    This can be useful in a compile-time expression.
    For example, \c approximate_if uses it.
    */
    struct ignorable_base_class {};

    /**
    Inherit from this class to indicate that an operation is approximate if the
    compile-time constant \a Condition is \c true, and not approximate
    otherwise.
    \tparam Condition The condition under which the operation is approximate.
    */
    template <class Condition> struct approximate_if
    : boost::mpl::if_ <Condition, approximate, ignorable_base_class>::type {};

    /**
    Inherit from this class to indicate that an operation throws when undefined
    for arguments it is passed if the compile-time constant \a Condition is
    \c true, and not otherwise.
    \tparam Condition The compile-time condition under which the operation
    throws when it is undefined.
    */
    template <class Condition> struct throw_if_undefined_if
    : boost::mpl::if_ <Condition, throw_if_undefined, ignorable_base_class
        >::type {};

    /**
    Inherit from this class to indicate that an operation is associative if the
    compile-time constant \a Condition is \c true, and not associative
    otherwise.
    \tparam Condition The condition under which the operation is associative.
    */
    template <class Condition> struct associative_if
    : boost::mpl::if_ <Condition, associative, ignorable_base_class>::type {};

    /**
    Inherit from this class to indicate that an operation is commutative if the
    compile-time constant \a Condition is \c true, and not commutative
    otherwise.
    \tparam Condition The condition under which the operation is commutative.
    */
    template <class Condition> struct commutative_if
    : boost::mpl::if_ <Condition, commutative, ignorable_base_class>::type {};

    /**
    Inherit from this class to indicate that an operation is idempotent if the
    compile-time constant \a Condition is \c true, and not idempotent
    otherwise.
    \tparam Condition The condition under which the operation is idempotent.
    */
    template <class Condition> struct idempotent_if
    : boost::mpl::if_ <Condition, idempotent, ignorable_base_class>::type {};

    /**
    Inherit from this class to indicate that an operation is a path operation if
    the compile-time constant \a Condition is \c true, and not a path operation
    otherwise.
    \tparam Condition The condition under which the operation is a path
    operation.
    */
    template <class Condition> struct path_operation_if
    : boost::mpl::if_ <Condition, path_operation, ignorable_base_class>::type
    {};

    struct return_argument {
        template <class Magma>
            typename std::decay <Magma>::type operator() (Magma && magma) const
        { return std::forward <Magma> (magma); }
    };

    /**
    Metafunction that indicates whether an operation is approximate.
    \return \c true iff \a Operation is derived from
    math::operation::approximate.
    */
    template <class Operation, class Enable = void> struct is_approximate
    : std::is_base_of <approximate, Operation> {};

    /**
    Metafunction that indicates whether an operation throws when it is not
    defined for the arguments it is called with.
    \return \c true iff \a Operation is derived from
    math::operation::throw_if_undefined.
    */
    template <class Operation, class Enable = void> struct throws_if_undefined
    : std::is_base_of <throw_if_undefined, Operation> {};

    /**
    Metafunction that indicates whether an operation is associative.
    \return \c true iff \a Operation is derived from
    math::operation::associative.
    */
    template <class Operation, class Enable = void> struct is_associative
    : std::is_base_of <associative, Operation> {};

    /**
    Metafunction that indicates whether an operation is commutative.
    \return \c true iff \a Operation is derived from
    math::operation::commutative.
    */
    template <class Operation, class Enable = void> struct is_commutative
    : std::is_base_of <commutative, Operation> {};

    template <class Operation, class Enable = void> struct is_path_operation;

    /**
    Metafunction that indicates whether an operation is idempotent.
    \return \c true iff \a Operation is derived from
    math::operation::idempotent.
    */
    template <class Operation, class Enable = void> struct is_idempotent
    : boost::mpl::or_ <
        std::is_base_of <idempotent, Operation>, is_path_operation <Operation>>
    {};

    /**
    Metafunction that indicates whether an operation is a path operation.
    \return \c true iff \a Operation is derived from
    math::operation::path_operation.
    */
    template <class Operation, class Enable> struct is_path_operation
    : std::is_base_of <path_operation, Operation> {};

    template <class MagmaTag, class Direction, class OuterOperation,
            class InnerOperation, class Enable = void>
        struct is_semiring;

    /**
    Specialise this to indicate that an operation is distributive.
    If "is_semiring" is true, this will automatically be true too.

    For OuterOperation=times, InnerOperation=plus:
    Left: a*(b+c) == a*b + a*c.
    Right: (a+b)*c == a*c + b*c.
    By setting this to \c true_ for direction "either", left and right get set
    to \c true_ automatically.
    */
    template <class MagmaTag, class Direction, class OuterOperation,
            class InnerOperation, class Enable = void>
        struct is_distributive
    : boost::mpl::or_ <
        is_semiring <MagmaTag, Direction, OuterOperation, InnerOperation>,
        boost::mpl::and_ <boost::mpl::or_ <
            std::is_same <Direction, left>, std::is_same <Direction, right>>,
                is_distributive <MagmaTag, either,
                OuterOperation, InnerOperation>>
    > {};

    template <class Direction, class OuterOperation, class InnerOperation>
        struct is_distributive <
            not_a_magma_tag, Direction, OuterOperation, InnerOperation> {};

    /**
    Define this for semirings, indicating left, right, or either for Direction.
    By setting this to \c true_ for direction "either", left and right get set
    to \c true_ automatically.
    */
    template <class MagmaTag, class Direction, class OuterOperation,
            class InnerOperation, class Enable>
        struct is_semiring
    : boost::mpl::if_ <boost::mpl::or_ <
        std::is_same <Direction, left>, std::is_same <Direction, right>>,
        is_semiring <MagmaTag, either, OuterOperation, InnerOperation>,
        boost::mpl::false_>::type {};

    template <class Direction, class OuterOperation, class InnerOperation>
        struct is_semiring <
            not_a_magma_tag, Direction, OuterOperation, InnerOperation> {};

    /**
    Evaluate to a type that both Magma1 and Magma2 can be converted to and which
    has the same magma tag as Magma1 and Magma2.

    Specialise this only for magmas with different types, if the \c order
    operation is defined.
    This is used in operations that implement an order.
    This is implemented for the case where Magma1 and Magma2 are the same:
    it then evaluates to Magma1.

    \a Magma1 and \a Magma2 are decayed types.
    */
    template <class MagmaTag, class Magma1, class Magma2, class Enable = void>
        struct unify_type
    : boost::mpl::if_ <std::is_same <Magma1, Magma2>,
        boost::mpl::identity <Magma1>, operation::unimplemented>::type {};

    /* Operations: queries. */

    /**
    Return whether a run-time value is actually a member of the magma.
    Sometimes a C++ type has a specific value that indicates non-membership.
    For example, not-a-number values.
    Implemented as returning rime::true_ by default.
    Specialise this if you implement math::operation::non_member.
    */
    template <class MagmaTag, class Enable = void> struct is_member {
        template <class Magma> rime::true_type operator() (Magma const &) const
        { return rime::true_; }
    };

    /**
    Return whether a value is an annihilator.
    The default implementation returns \c rime::false_ unless annihilator() is
    defined.
    In this case, <c>equal (element, annihilator())</c> is return.

    Specialise this only for speed, or because you would like to use it in the
    implementation of \c equal, or because you would like to have different
    annihilators (though I do not think the last is possible theoretically).

    \internal This is implemented lower down.
    */
    template <class MagmaTag, class Operation, class Enable = void>
        struct is_annihilator;

    /**
    Return whether two elements of the same magma are equal.
    This must be specialised.
    */
    template <class MagmaTag, class Enable = void> struct equal
    : unimplemented {};

    namespace not_equal_detail {

        template <class Operation> struct not_ {
            template <class Left, class Right> auto operator()
                (Left && left, Right && right) const
            RETURNS (!(Operation() (
                std::forward <Left> (left), std::forward <Right> (right))));
        };

    } // namespace not_equal_detail

    /**
    Return whether two elements of the same magma are not equal.
    Implemented by default as !(equal (left, right)).
    Specialise this only if the default implementation does not suffice.
    */
    template <class MagmaTag, class Enable = void> struct not_equal
    : boost::mpl::if_ <is_implemented <equal <MagmaTag>>,
        not_equal_detail::not_ <equal <MagmaTag>>,
        unimplemented>::type {};

    /**
    Return whether two elements of the same magma are approximately equal.
    Implemented by forwarding to equal by default.
    Specialise only if it is necessary for this magma to distinguish between
    exact and approximate equality.
    */
    template <class MagmaTag, class Enable = void> struct approximately_equal
    : equal <MagmaTag> {};

    /* Operations: produce specific values. */

    /**
    Specialise to return a non-member value.
    It is best to return an object of the most specific type, and allow
    conversion to a more general type where possible.
    */
    template <class MagmaTag, class Enable = void>
        struct non_member
    : unimplemented {};

    /**
    Specialise to produce the identity with respect to Operation.
    */
    template <class MagmaTag, class Operation, class Enable = void>
        struct identity
    : unimplemented {};

    /**
    Specialise to produce the annihilator with respect to Operation.
    */
    template <class MagmaTag, class Operation, class Enable = void>
        struct annihilator
    : unimplemented {};

    /* Operations: operations. */

    /**
    Specialise to compare two elements of a magma.

    This must returns \c false for both orders of arguments iff equal returns
    \a true for these arguments.

    Preferably, return \c true if the first value is better, in some sense,
    than the second value.
    */
    template <class MagmaTag, class Enable = void> struct compare
    : unimplemented {};

    /**
    Specialise to indicate that operation Operation returns the extreme element
    using some order.
    This predicate then must implement a strict weak ordering.
    Return true iff the left-hand argument is preferable on this ordering than
    the right-hand argument.
    If one of the arguments to Operation is preferable other than the other one,
    it should return that argument unchanged.
    If <c>order \<MagmaTag, callable::times></c> is implemented, then
    math::operation::times is implemented automatically; similarly for plus.
    */
    template <class MagmaTag, class Operation, class Enable = void>
        struct order
    : unimplemented {};

    /**
    Take a predicate implementing a strict weak ordering, and reverse the order.
    (This involves swapping the order of the arguments. Not very complicated.)

    This is a helper for implementing comparisons.
    It is used, for example, by \c max_semiring, to prefer the largest value
    instead of the smallest.
    */
    template <class Order> struct reverse_order {
        template <class Magma1, class Magma2>
            auto operator() (Magma1 const & magma1, Magma2 const & magma2) const
        RETURNS (Order() (magma2, magma1));
    };

    /**
    Return \a magma1 iff \a condition is true, and \a magma2 otherwise.

    It is not normally necessary to specialise this.
    The default implementation uses \c operation::unify_types to unify the
    return type if necessary.
    \internal
    This is implemented lower down.
    */
    template <class MagmaTag, class Enable = void> struct pick;

    /**
    Take two arguments and select the left one if order (left, right) returns
    true, and the right one otherwise.

    Helper for implementing operations.
    \internal
    This is implemented lower down.
    */
    template <class Order> struct choose_by_order;

    /**
    Implement choosing the most preferable of two values.
    If <c>order \<MagmaTag, callable::choose></c> is implemented, this
    automatically returns the best element according to this order.
    It is therefore normally not necessary to explicitly specialise this.

    It is useful to implement this if at all possible, even if there is no
    meaningful ordering.
    Some composite semirings, like the lexicographical semiring, require that
    all elements define "choose".
    */
    template <class MagmaTag, class Enable = void> struct choose
    : boost::mpl::if_ <
        is_implemented <order <MagmaTag, callable::choose>>,
        choose_by_order <order <MagmaTag, callable::choose>>,
        unimplemented>::type {};

    /**
    Implement multiplication of two values.
    Specialise this if the multiplication operation exists for the magma.
    If <c>order \<MagmaTag, callable::times></c> is implemented, this
    automatically returns the best element according to this order, and it is
    not necessary to specialise this.
    */
    template <class MagmaTag, class Enable = void> struct times
    : boost::mpl::if_ <
        is_implemented <order <MagmaTag, callable::times>>,
        choose_by_order <order <MagmaTag, callable::times>>,
        unimplemented>::type {};

    /**
    Implement addition of two values.
    Specialise this if the addition operation exists for the magma.
    If <c>order \<MagmaTag, callable::plus></c> is implemented, this
    automatically returns the best element according to this order, and it is
    not necessary to specialise this.
    */
    template <class MagmaTag, class Enable = void> struct plus
    : boost::mpl::if_ <
        is_implemented <order <MagmaTag, callable::plus>>,
        choose_by_order <order <MagmaTag, callable::plus>>,
        unimplemented>::type {};

    /**
    Implement division of two values.
    Note that the numerator is always the first argument and the denominator
    always the second, no matter what the direction is.
    The default behaviour for left and right division is to forward to general
    division.
    If left and right division are the same, therefore, this class has to be
    specialised once, with "either" as direction.
    If left and right division behave differently, only specialise this for the
    appropriate direction(s).
    \internal
    This is not implemented with specialisation because that makes it hard
    to specialise on tag.
    */
    template <class MagmaTag, class Direction, class Enable = void>
        struct divide
    : boost::mpl::if_ <boost::mpl::or_ <
            std::is_same <Direction, left>, std::is_same <Direction, right>>,
        divide <MagmaTag, either>, unimplemented
    >::type {};

    /**
    Implement subtraction of two values.
    Note that the minuend is always the first argument and the subtrahend
    always the second, no matter what the direction is.
    The default behaviour for left and right subtraction is to forward to
    general subtraction.
    If left and right subtraction are the same, therefore, this class has to be
    specialised once, with "either" as direction.
    If left and right subtraction behave differently, only specialise this for
    the appropriate direction(s).
    \internal
    This is not implemented with specialisation because that makes it hard
    to specialise on tag.
    */
    template <class MagmaTag, class Direction, class Enable = void>
        struct minus
    : boost::mpl::if_ <boost::mpl::or_ <
            std::is_same <Direction, left>, std::is_same <Direction, right>>,
        minus <MagmaTag, either>, unimplemented
    >::type {};

    /**
    Implement to return the inverse of a value with respect to an operation.
    That is, <c> operation (invert (a), a) == identity<T> (operation)</c>.
    */
    template <class MagmaTag, class Direction, class Operation,
        class Enable = void>
    struct invert
    : boost::mpl::if_ <boost::mpl::or_ <
            std::is_same <Direction, left>, std::is_same <Direction, right>>,
        invert <MagmaTag, either, Operation>, unimplemented
    >::type {};

    namespace reverse_detail {

        template <class MagmaTag, class Operation> struct automatic
        : unimplemented {};

        template <class MagmaTag>
            struct automatic <MagmaTag, callable::times>
        : boost::mpl::if_ <is_commutative <times <MagmaTag>>,
            return_argument, unimplemented>::type {};

        template <class MagmaTag>
            struct automatic <MagmaTag, callable::plus>
        : boost::mpl::if_ <is_commutative <plus <MagmaTag>>,
            return_argument, unimplemented>::type {};

    } // namespace reverse_detail

    /**
    Specialise to return the reverse of a value with respect to an operation.
    That is,
    <c>reverse (operation (reverse (a), reverse (b)) == operation (a, b)</c>.
    If the operation is either \c times or \c plus and it is commutative, then
    this is implemented automatically by returning the argument exactly.
    */
    template <class MagmaTag, class Operation, class Enable = void>
        struct reverse
    : reverse_detail::automatic <MagmaTag, Operation> {};

    /**
    Specialise to output a human-readable description of the value of the magma
    to a stream.
    This is not required, but useful for debugging.
    This will normally produce the same output as \c operator<<.
    Indeed, if this operator is defined, derive from operator_shift_left_stream
    to forward to it.
    */
    template <class MagmaTag, class Enable = void> struct print
    : unimplemented {};

    /**
    Give the left or right inverse of an operation.
    That is,
    <c> inverse_operation \<right> (operation) (operation (a, b), b) == a</c>.
    */
    template <class Direction, class Operation> struct inverse_operation
    : unimplemented {};

    // Division is the inverse operation of multiplication.
    template <class Direction>
        struct inverse_operation <Direction, callable::times>
    {
        callable::divide <Direction> operator() () const
        { return callable::divide <Direction>(); }
    };

    // Subtraction is the inverse operation of addition.
    template <class Direction>
        struct inverse_operation <Direction, callable::plus>
    {
        callable::minus <Direction> operator() () const
        { return callable::minus <Direction>(); }
    };

    /* Interaction with operators. */

    // Define utility classes that forward to operators.
    // With a type argument, they compute the return type; otherwise, the
    // return type is the type given.
#define MATH_MAGMA_DEFINE_OPERATOR_FORWARDER(operator_name, operator_symbol) \
    template <class ReturnType = void> struct operator_name { \
        template <class Argument1, class Argument2> ReturnType \
            operator() (Argument1 && argument1, Argument2 && argument2) const \
        { \
            return std::forward <Argument1> (argument1) operator_symbol \
                std::forward <Argument2> (argument2); \
        } \
    }; \
    \
    template <> struct operator_name <void> { \
        template <class Argument1, class Argument2> auto \
            operator() (Argument1 && argument1, Argument2 && argument2) const \
        RETURNS (std::forward <Argument1> (argument1) operator_symbol \
            std::forward <Argument2> (argument2)); \
    };

    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_times, * )
    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_plus, + )
    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_divide, / )
    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_minus, -)
    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_equal, == )
    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_not_equal, == )
    MATH_MAGMA_DEFINE_OPERATOR_FORWARDER (operator_less, < )

#undef MATH_MAGMA_DEFINE_OPERATOR_FORWARDER

    /**
    Utility class to implement "print" by applying <c>stream << magma</c>.
    */
    struct operator_shift_left_stream {
        template <class Stream, class Magma>
            void operator() (Stream & stream, Magma const & magma) const
        { stream << magma; }
    };

} // namespace operation

namespace apply {

    template <class Magma> struct is_member <Magma>
    : operation::is_member <typename magma_tag <Magma>::type> {};

    template <class Operation, class Magma>
        struct is_annihilator <Operation, Magma>
    : operation::is_annihilator <
        typename magma_tag <Magma>::type, typename std::decay <Operation>::type>
    {};

    // Produce.
    // These operations take the (decayed) actual type that they are supposed
    // to produce too.
    template <class Magma> struct non_member <Magma> \
    : operation::non_member <typename magma_tag <Magma>::type> {};

    // Identity also requires an operation.
#define MATH_MAGMA_DEFINE_PRODUCE_WITH_OPERATION(operation_name) \
    template <class Magma, class Operation> \
        struct operation_name <Magma, Operation> \
    : operation::operation_name < \
        typename magma_tag <Magma>::type, \
        typename std::decay <Operation>::type> {};

    MATH_MAGMA_DEFINE_PRODUCE_WITH_OPERATION (identity)
    MATH_MAGMA_DEFINE_PRODUCE_WITH_OPERATION (annihilator)

#undef MATH_MAGMA_DEFINE_PRODUCE_WITH_OPERATION

    template <class Magma> struct zero <Magma>
    : identity <Magma, callable::plus> {};

    template <class Magma> struct one <Magma>
    : identity <Magma, callable::times> {};

    template <class Magma1, class Magma2> struct compare <Magma1, Magma2>
    : operation::compare <typename magma_tag_all <Magma1, Magma2>::type> {};

    template <class Operation, class Magma1, class Magma2>
        struct order <Operation, Magma1, Magma2>
    : operation::order <
        typename magma_tag_all <Magma1, Magma2>::type,
        typename std::decay <Operation>::type> {};

    template <class Condition, class Magma1, class Magma2>
        struct pick <Condition, Magma1, Magma2>
    : operation::pick <typename magma_tag_all <Magma1, Magma2>::type> {};

    // Binary operations.
#define MATH_MAGMA_DEFINE_BINARY_OPERATION(operation_name) \
    template <class Magma1, class Magma2> \
        struct operation_name <Magma1, Magma2> \
    : operation::operation_name < \
        typename magma_tag_all <Magma1, Magma2>::type> \
    {};

    MATH_MAGMA_DEFINE_BINARY_OPERATION(choose)
    MATH_MAGMA_DEFINE_BINARY_OPERATION(times)
    MATH_MAGMA_DEFINE_BINARY_OPERATION(plus)

    MATH_MAGMA_DEFINE_BINARY_OPERATION(equal)
    MATH_MAGMA_DEFINE_BINARY_OPERATION(not_equal)
    MATH_MAGMA_DEFINE_BINARY_OPERATION(approximately_equal)

#undef MATH_MAGMA_DEFINE_BINARY_OPERATION

    template <class Direction, class Magma1, class Magma2>
        struct divide <Direction, Magma1, Magma2>
    : operation::divide <typename magma_tag_all <Magma1, Magma2>::type,
        typename std::decay <Direction>::type>
    {};

    template <class Direction, class Magma1, class Magma2>
        struct minus <Direction, Magma1, Magma2>
    : operation::minus <typename magma_tag_all <Magma1, Magma2>::type,
        typename std::decay <Direction>::type>
    {};

    template <class Direction, class Operation>
        struct inverse_operation <Direction, Operation>
    : operation::inverse_operation <typename std::decay <Direction>::type,
        typename std::decay <Operation>::type>
    {};

    template <class Direction, class Operation, class Magma>
        struct invert <Direction, Operation, Magma>
    : operation::invert <typename magma_tag <Magma>::type,
        typename std::decay <Direction>::type,
        typename std::decay <Operation>::type>
    {};

    template <class Operation, class Magma>
        struct reverse <Operation, Magma>
    : operation::reverse <typename magma_tag <Magma>::type,
        typename std::decay <Operation>::type> {};

    template <class Stream, class Magma>
        struct print <Stream, Magma>
    : operation::print <typename magma_tag <Magma>::type> {};

} // namespace apply

namespace callable {

    template <class FunctionExpression> struct is_implemented;
    template <class Callable, class ... Arguments>
        struct is_implemented <Callable (Arguments...)>
    : operation::is_implemented <typename std::decay <Callable>::type::template
        apply <Arguments...>>
    {};

    template <class FunctionExpression> struct is_approximate;
    template <class Callable, class ... Arguments>
        struct is_approximate <Callable (Arguments ...)>
    : operation::is_approximate <typename std::decay <Callable>::type::template
        apply <Arguments...>>
    {};

} // namespace callable


/**
\return Whether \c value is a member of the magma.

Some magmas have values that are not in the magma, like not-a-number values in
standard floating-point types.

\param value.
*/
static const auto is_member = callable::is_member();

/**
\return Whether \c value is an annihilator for \a Operation.
In calling this, it is possible to indicate the operation as the type argument,
or as the first argument.

\tparam Operation
    The callable type that represents the operation.
\param operation (optional if \a Operation is given as a template argument)
    A callable that represents the operation.
\param magma
    The value under consideration.
*/
template <class Operation, class Magma> inline
    auto is_annihilator (Operation const & operation, Magma const & magma)
        RETURNS (callable::is_annihilator<>() (operation, magma));

/// \cond DONT_DOCUMENT
template <class Operation, class Magma> inline
    auto is_annihilator (Magma const & magma)
    RETURNS (callable::is_annihilator <Operation>() (magma));
/// \endcond

/**
\return Whether \a left and \a right are equal.
This should normally correspond to <c> operator==</c>.
\param left
\param right
*/
static const auto equal = callable::equal();

/**
\return Whether \a left and \a right are not equal.
This should normally correspond to <c> operator!=</c>.
\param left
\param right
*/
static const auto not_equal = callable::not_equal();

/**
\return Whether \a left and \a right are approximately equal.
\param left
\param right
\todo Is there a requirement with respect to timing?
*/
static const auto approximately_equal = callable::approximately_equal();

/**
\return A non-member of the same magma as Magma.
If is_member is applied to this value, it returns false.
This operation is not always supplied.
For floating-point values, NaN is returned.

\tparam Magma
    A type that is in a magma that the return type should be in.
    Note that the return type is not necesssarily that type.
*/
template <class Magma> inline
    typename std::result_of <callable::non_member <Magma>()>::type
    non_member()
{ return callable::non_member <Magma>()(); }

/**
\return The identity with respect to Operation of the same magma as Magma.
In calling this, it is possible to indicate the operation as the second type
argument, or as the value argument.
(Or both, redundantly.)
Thus,
<c> identity \<float, callable::times>()</c>
and <c> identity \<float> (times)</c>
return the exact same value.

\tparam Magma
    A type that is in a magma that the return type should be in.
    Note that the return type is not necessarily that type.
\tparam Operation
    The callable type that represents the operation.
\param operation (optional if \a Operation is given as a template argument)
    A callable that represents the operation.
*/
template <class Magma, class Operation> inline
    typename std::result_of <callable::identity <Magma> (Operation)>::type
    identity (Operation const & operation)
{ return callable::identity <Magma>()(operation); }

/// \cond DONT_DOCUMENT
template <class Magma, class Operation> inline
    typename std::result_of <callable::identity <Magma, Operation>()>::type
    identity()
{ return callable::identity <Magma, Operation>()(); }
/// \endcond

/**
\return The annihilator with respect to Operation of the same magma as Magma.
In calling this, it is possible to indicate the operation as the second type
argument, or as the value argument.
(Or both, redundantly.)
Thus,
<c> annihilator \<float, callable::times>()</c>
and <c> annihilator \<float> (times)</c>
return the exact same value.

\tparam Magma
    A type that is in a magma that the return type should be in.
    Note that the return type is not necessarily that type.
\tparam Operation
    The callable type that represents the operation.
\param operation (optional if \a Operation is given as a template argument)
    A callable that represents the operation.
*/
template <class Magma, class Operation> inline
    typename std::result_of <callable::annihilator <Magma> (Operation)>::type
    annihilator (Operation const & operation)
{ return callable::annihilator <Magma>()(operation); }

/// \cond DONT_DOCUMENT
template <class Magma, class Operation> inline
    typename std::result_of <callable::annihilator <Magma, Operation>()>::type
    annihilator()
{ return callable::annihilator <Magma, Operation>()(); }
/// \endcond

/**
\return Generalised zero, i.e. the identity with respect to the operation
math::plus.
Equivalent to <c> identity <callable::plus, Magma>()</c>.

\tparam Magma
    A type that is in a magma that the return type should be in.
    Note that the return type is not necessarily that type.
*/
template <class Magma> inline
    typename std::result_of <callable::zero <Magma>()>::type
    zero()
{ return callable::zero <Magma>()(); }
/**
\return Generalised one, i.e. the identity with respect to the operation
math::times.
Equivalent to <c> identity <callable::times, Magma>()</c>.
\tparam Magma
    A type that is in a magma that the return type should be in.
    Note that the return type is not necessarily that type.
*/
template <class Magma> inline
    typename std::result_of <callable::one <Magma>()>::type
    one()
{ return callable::one <Magma>()(); }

/**
Return whether \a magma1 comes before \a magma2 in a strict weak ordering.
This returns \c false for both orders of arguments iff equal returns \a true.
\param magma1
\param magma2
*/
static const auto compare = callable::compare();

/**
Return whether operation \a Operation will return \a magma1, or \a magma2.
This operation is only available if \a Operation applies a strict weak ordering
and returns either \a magma1 or \a magma2.

\tparam Operation The operation that returns either either \a magma1 or
    \a magma2.
\param magma1
\param magma2
*/
template <class Operation, class Magma1, class Magma2> inline auto
    order (Magma1 && magma1, Magma2 && magma2)
RETURNS (callable::order <Operation>() (
    std::forward <Magma1> (magma1), std::forward <Magma2> (magma2)));

/** \brief
Rime merge policy for magmas.

If you would like to return one value or another based on a run-time or
compile-time condition, then say
\code
rime::if_ <merge_magma> (condition, magma1, magma2)
\endcode

This will automatically return the correctly general type.

This wraps operation::unify_type to form a metafunction implementing a Rime
merge policy.
If Left and Right are the same type, then it is returned unchanged.
Otherwise, some conversion is necessary, so operation::unify_type is called with
the decayed types to find the common type.

\todo If Left and Right are reference types and unify_type returns a
common base, it might be best to return a reference, and prevent an
additional copy.
*/
struct merge_magma {
    template <class Left, class Right> struct apply
    : operation::unify_type <typename magma_tag_all <Left, Right>::type,
        typename std::decay <Left>::type,
        typename std::decay <Right>::type> {};

    template <class Type> struct apply <Type, Type>
    { typedef Type type; };
};

/**
\return \a magma1 iff \a condition is true; \a magma2 otherwise.
When necessary, this selects a return type that is general enough to represent
either argument.
\param condition A boolean, which can be a compile-time or a run-time value.
\param magma1
\param magma2
*/
static const auto pick = callable::pick();

/**
\return The most preferable out of \a magma1 and \a magma2.
\param magma1
\param magma2
*/
static const auto choose = callable::choose();

/**
\return The product of \a magma1 and \a magma2.
\param magma1
\param magma2
*/
static const auto times = callable::times();

/**
\return The sum of \a magma1 and \a magma2.
\param magma1
\param magma2
*/
static const auto plus = callable::plus();

/**
\return The division of dividend and divisor.
This can be performed in two directions, but note that the order of the
arguments does not change.
Left division is the inverse of multiplication on the left:
<c> divide \<left> (times (a, b), a) == b</c>.
Right division is the inverse of multiplication on the right:
<c> divide \<right> (times (a, b), b) == a</c>.
If multiplication is commutative, then left division is the same as right
division.
Then, \a Direction can be left out, or given as math::either.

\tparam Direction The direction of the division.

\param dividend Also known as numerator.
\param divisor Also known as denominator.
*/
template <class Direction, class Dividend, class Divisor,
    /// \cond DONT_DOCUMENT
    class Enable = typename boost::enable_if <is_direction <Direction>>::type
    /// \endcond
> inline auto divide (Dividend && dividend, Divisor && divisor)
    /// \cond DONT_DOCUMENT
    -> typename std::result_of <
        callable::divide <Direction> (Dividend, Divisor)>::type
    /// \endcond
{
    return callable::divide <Direction>() (
        std::forward <Dividend> (dividend), std::forward <Divisor> (divisor));
}

/// \cond DONT_DOCUMENT
template <class Dividend, class Divisor,
    class Enable = typename boost::disable_if <is_direction <Dividend>>::type>
inline typename
    std::result_of <callable::divide <either> (Dividend, Divisor)>::type
    divide (Dividend && dividend, Divisor && divisor)
{
    return callable::divide <either>() (
        std::forward <Dividend> (dividend), std::forward <Divisor> (divisor));
}
/// \endcond

/**
\return The difference of minuend and subtrahend.
This can be performed in two directions, but note that the order of the
arguments does not change.
Left subtraction is the inverse of addition on the left:
<c> minus \<left> (plus (a, b), a) == b</c>.
Right subtraction is the inverse of addition on the right:
<c> minus \<right> (plus (a, b), b) == a</c>.
If plus is commutative, then left subtraction is the same as right subtraction.
Then, \a Direction can be left out, or given as math::either.

\tparam Direction The direction of the subtraction.

\param minuend The object that the subtrahend will be subtracted from.
\param subtrahend The object that will be subtracted from the minuend.
*/
template <class Direction, class Minuend, class Subtrahend,
    /// \cond DONT_DOCUMENT
    class Enable = typename boost::enable_if <is_direction <Direction>>::type
    /// \endcond
    > inline auto minus (Minuend && minuend, Subtrahend && subtrahend)
    /// \cond DONT_DOCUMENT
    -> typename std::result_of <
        callable::minus <Direction> (Minuend, Subtrahend)>::type
    /// \endcond
{
    return callable::minus <Direction>() (
        std::forward <Minuend> (minuend),
        std::forward <Subtrahend> (subtrahend));
}

/// \cond DONT_DOCUMENT
template <class Minuend, class Subtrahend,
    class Enable = typename boost::disable_if <is_direction <Minuend>>::type
    > inline typename
    std::result_of <callable::minus <either> (Minuend, Subtrahend)>::type
    minus (Minuend && minuend, Subtrahend && subtrahend)
{
    return callable::minus <either>() (
        std::forward <Minuend> (minuend),
        std::forward <Subtrahend> (subtrahend));
}
/// \endcond

/**
\return The inverse element of magma with respect to the operation.
The operation can be given as a type argument, or as a value argument.

\tparam Direction (optional: the default is math::either)
    The direction of the inverse.
    A left inverse (indicated with math::left) means that, say,
        <c> math::inverse <left, callable::plus> (a) + a</c>
    is equal to the additive identity.

\tparam Operation The operation that the inverse is with respect to.

\param operation (optional if \a Operation is given as a template argument)
    The operation that the inverse is with respect to.

\param magma The value to be inverted.
*/
template <class Direction, class Operation, class Magma,
    /// \cond DONT_DOCUMENT
    class Enable = typename boost::enable_if <is_direction <Direction>>::type
    /// \endcond
    > inline auto invert (Operation const & operation, Magma const & magma)
RETURNS (callable::invert <Direction>() (operation, magma));

/// \cond DONT_DOCUMENT

// Without Direction, and operation as a run-time argument.
template <class Operation, class Magma> inline
// Using lazy_disable_if is necessary for GCC 4.6 to construct the declaration,
// even if SFINAE kicks in.
// Don't use template <class Operation, class Enable = ...>!
    typename boost::lazy_disable_if <is_direction <Operation>,
        std::result_of <callable::invert <Operation> (Magma const &)>>::type
    invert (Operation const & operation, Magma const & magma)
{ return callable::invert<>() (operation, magma); }

// With Direction, and operation as a template argument.
template <class Direction, class Operation, class Magma,
    class Enable = typename boost::enable_if <is_direction <Direction>>::type
    > inline auto invert (Magma const & magma)
RETURNS (callable::invert <Direction, Operation>() (magma));

// Without Direction, and operation as a template argument.
template <class Operation, class Magma> inline
// Using lazy_disable_if is necessary for GCC 4.6 to construct the declaration,
// even if SFINAE kicks in.
// Don't use template <class Operation, class Enable = ...>!
    typename boost::lazy_disable_if <is_direction <Operation>,
        std::result_of <callable::invert <Operation> (Magma const &)>>::type
    invert (Magma const & magma)
{ return callable::invert <Operation>() (magma); }

/// \endcond

/**
Compute the reverse of a value.
It is required that
<c>operation (reverse (a), reverse (b)) == reverse (operation (a, b))</c>.
The return type may be in a different magma.
For example, this may reverse the elements of a left sequence semiring and
return them as a right sequence semiring.
However, it is required that <c>reverse (reverse (a)) == a</c> and that the
result type of applying \c reverse twice returns a type in the same magma.

For commutative operations, the implementation may just return the magma passed
in without any changes.

The operation can be given as a type argument, or as a value argument.

\return The reverse of an element of magma with respect to the operation.

\tparam Operation The operation that the inverse is with respect to.

\param operation (optional if \a Operation is given as a template argument)
    The operation that the inverse is with respect to.

\param magma The value to be inverted.
*/
template <class Operation, class Magma>
    inline auto reverse (Operation const & operation, Magma const & magma)
RETURNS (callable::reverse<>() (operation, magma));

/// \cond DONT_DOCUMENT
template <class Operation, class Magma>
    inline auto reverse (Magma const & magma)
RETURNS (callable::reverse <Operation>() (magma));
/// \endcond

/**
Output a human-readable representation of the magma to a stream.

\return \c void.
\param stream The output stream to write to.
\param magma The value to output the representation of.
*/
static auto const print = callable::print();

/**
Return the inverse of the operation.
For example, <c>inverse_operation (times)</c> will return an object of type
<c>callable::divide\<></c>.
The operation can be given as a type argument, or as a value argument.

\tparam Direction (optional: the default is math::either.)
    The direction of the inverse.
    For example, in a sequence semiring, in which math::times concatenates two
    sequences, left division and right division are different.
\tparam Operation
    The callable type that represents the operation.
\param operation (optional if \a Operation is given as a template argument)
    A callable that represents the operation.
*/
template <class Direction, class Operation,
    /// \cond DONT_DOCUMENT
    class Enable = typename boost::enable_if <is_direction <Direction>>::type
    /// \endcond
> inline auto inverse_operation (Operation const & operation)
    /// \cond DONT_DOCUMENT
    -> typename std::result_of <
        callable::inverse_operation <Direction> (Operation)>::type
    /// \endcond
{ return callable::inverse_operation <Direction>() (operation); }

/// \cond DONT_DOCUMENT

// Direction is not given: default is "either".
template <class Operation> inline
// Using lazy_disable_if is necessary for GCC 4.6 to construct the declaration,
// even if SFINAE kicks in.
// Don't use template <class Operation, class Enable = ...>!
    typename boost::lazy_disable_if <is_direction <Operation>,
        std::result_of <callable::inverse_operation<> (Operation)>>::type
    inverse_operation (Operation const & operation)
{ return callable::inverse_operation<>() (operation); }

// Both Direction and Operation are template arguments.
template <class Direction, class Operation,
    class Enable = typename boost::enable_if <is_direction <Direction>>::type
> inline typename
    std::result_of <callable::inverse_operation <Direction, Operation>()>::type
    inverse_operation()
{ return callable::inverse_operation <Direction, Operation>() (); }

// Direction is not given; Operation is a template argument.
template <class Operation> inline
// Using lazy_disable_if is necessary for GCC 4.6 to construct the declaration,
// even if SFINAE kicks in.
// Don't use template <class Operation, class Enable = ...>!
    typename boost::lazy_disable_if <is_direction <Operation>,
        std::result_of <callable::inverse_operation <Operation>()>>::type
    inverse_operation()
{ return callable::inverse_operation <Operation>() (); }

/// \endcond DONT_DOCUMENT

/**
Compile-time constant that returns whether a nested call expression has been
implemented for a particular set of parameters.
\tparam Expression
    The nested call expression of which the return type is sought.
*/
template <class Expression> struct has
: nested_callable::all <callable::is_implemented <boost::mpl::_1>, Expression>
{};

/** \struct math::result_of
Find the result of a nested call expression.
\tparam Expression
    The nested call expression of which the return type is sought.
*/
using nested_callable::result_of;

/**
Find the result of a nested call expression, or the second argument.
If no second argument is given, do not yield any type (useful to disable a
function overload using SFINAE).
\tparam Expression
    The nested call expression of which the return type is sought.
\tparam Otherwise (optional)
    The type that should be returned if <c>has \<Expression></c> evaluates to
    \c false.
    If this is not given, then \a type will not be defined.
*/
template <class ... Arguments> struct result_of_or {};

template <class Expression> struct result_of_or <Expression>
: boost::lazy_enable_if <has <Expression>, result_of <Expression>> {};

template <class Expression, class Otherwise>
    struct result_of_or <Expression, Otherwise>
: boost::mpl::eval_if <has <Expression>,
    result_of <Expression>,
    boost::mpl::identity <Otherwise>>
{};

namespace operation {

    namespace is_annihilator_detail {

        template <class MagmaTag, class Operation> struct equal_annihilator {
            template <class Magma>
                auto operator() (Magma const & magma) const
            RETURNS (equal <MagmaTag>() (magma,
                annihilator <MagmaTag, Operation>() ()));
        };

    } // namespace is_annihilator_detail

    template <class MagmaTag, class Operation, class Enable>
        struct is_annihilator
    : boost::mpl::if_ <is_implemented <annihilator <MagmaTag, Operation>>,
        is_annihilator_detail::equal_annihilator <MagmaTag, Operation>,
        rime::callable::always_default <rime::false_type>>::type {};

    template <class MagmaTag, class Enable> struct pick {
        template <class Condition, class Left, class Right>
        auto operator() (Condition condition, Left && left, Right && right)
            const
        RETURNS (rime::if_ <math::merge_magma> (condition,
            std::forward <Left> (left), std::forward <Right> (right)));
    };

    template <class Order> struct choose_by_order
    : associative, commutative, path_operation
    {
        template <class Left, class Right>
        auto operator() (Left && left, Right && right) const
        RETURNS (::math::pick (Order() (left, right),
            std::forward <Left> (left), std::forward <Right> (right)));
    };

} // namespace operation

namespace is {

    // Queries about operations.

    namespace detail {

        template <class Base, class ... Magmas> struct derive_if_all_magma_tags
        : std::conditional <
            !(std::is_same <typename magma_tag_all <Magmas ...>::type,
                not_a_magma_tag>::value),
            Base, operation::ignorable_base_class>::type {};

    } // namespace detail

    /**
    Compile-time constant that returns whether an operation is approximate for a
    particular set of parameters.
    \tparam Expression
        The nested call expression.
        If any single operation in the expression is approximate, the whole
        expression is assumed to be approximate.
        (This is the conservative approach.)
    */
    template <class Expression> struct approximate
    : nested_callable::any <callable::is_approximate <boost::mpl::_1>,
        Expression> {};

    /**
    Whether an operation throws an exception when it is undefined.

    E.g.
    throw_if_undefined <callable::inverse (float)>
    throw_if_undefined <callable::divide (float, float)>
    */
    template <class ... Arguments> struct throw_if_undefined;

    template <class Operation, class Magma>
        struct throw_if_undefined <Operation (Magma)>
    : operation::throws_if_undefined <typename
        std::decay <Operation>::type::template apply <Magma>> {};

    template <class Operation, class Magma1, class Magma2>
        struct throw_if_undefined <Operation (Magma1, Magma2)>
    : operation::throws_if_undefined <typename
        std::decay <Operation>::type::template apply <Magma1, Magma2>> {};

    /**
    Whether an operation on a magma is associative.
    That is, whether in an expression that applies the same operation a number
    of times, like <c>1+2+3</c>, changing the position of the brackets does
    not change the result.

    It is possible that the operation is approximate but it is still
    approximately associative.
    In that case, this will evaluate to \c true.

    It is possible to instantiate this with the standard type argument syntax,
    documented below, or with one argument in a cute syntax, one of:
    <c> Operation (Magma1, Magma2)</c>;
    <c> Operation (Magma1, Operation (Magma2, Magma3))</c>;
    <c> Operation (Operation (Magma1, Magma2), Magma3)</c>.

    \tparam Operation
        The operation, normally a type in namespace \c math::callable.
    \tparam Magma
        A type that belongs to the magma of interest.
    */
    template <class ... Arguments> struct associative;

    template <class Operation, class Magma>
        struct associative <Operation, Magma>
    : operation::is_associative <typename
        std::decay <Operation>::type::template apply <Magma, Magma>> {};

    template <class Operation, class Magma1, class Magma2>
        struct associative <Operation (Magma1, Magma2)>
    : detail::derive_if_all_magma_tags <
        associative <Operation, Magma1>, Magma1, Magma2> {};

    template <class Operation, class Magma1, class Magma2, class Magma3>
        struct associative <Operation (Magma1, Operation (Magma2, Magma3))>
    : detail::derive_if_all_magma_tags <
        associative <Operation, Magma1>, Magma1, Magma2, Magma3> {};

    template <class Operation, class Magma1, class Magma2, class Magma3>
        struct associative <Operation (Operation (Magma1, Magma2), Magma3)>
    : detail::derive_if_all_magma_tags <
        associative <Operation, Magma1>, Magma1, Magma2, Magma3> {};

    /**
    Whether an operation on a magma is commutative.
    That is, whether the operands of the operation can be exchanged without
    changing the result.

    It is possible that the operation is approximate but it is still
    approximately associative.
    In that case, this will evaluate to \c true.

    It is possible to instantiate this with the standard type argument syntax,
    documented below, or with one argument in a cute syntax:
    <c> Operation (Magma1, Magma2)</c>.

    \tparam Operation
        The operation, normally a type in namespace \c math::callable.
    \tparam Magma
        A type that belongs to the magma of interest.
    */
    template <class ... Arguments> struct commutative;

    template <class Operation, class Magma>
        struct commutative <Operation, Magma>
    : operation::is_commutative <typename
        std::decay <Operation>::type::template apply <Magma, Magma>> {};

    template <class Operation, class Magma1, class Magma2>
        struct commutative <Operation (Magma1, Magma2)>
    : detail::derive_if_all_magma_tags <
        commutative <Operation, Magma1>, Magma1, Magma2> {};

    /**
    Whether an operation on a magma is a path operation.
    That is, whether the operation returns either the one or the other operand.

    This implies idempotent <Magma, Operation>.

    It is possible to instantiate this with the standard type argument syntax,
    documented below, or with one argument in a cute syntax:
    <c> Operation (Magma1, Magma2)</c>.

    \tparam Operation
        The operation, normally a type in namespace \c math::callable.
    \tparam Magma
        A type that belongs to the magma of interest.
    */
    template <class ... Arguments> struct path_operation;

    template <class Operation, class Magma>
        struct path_operation <Operation, Magma>
    : operation::is_path_operation <typename
        std::decay <Operation>::type::template apply <Magma, Magma>> {};

    template <class Operation, class Magma1, class Magma2>
        struct path_operation <Operation (Magma1, Magma2)>
    : detail::derive_if_all_magma_tags <
        path_operation <Operation, Magma1>, Magma1, Magma2> {};

    /**
    Whether an operation on a magma is idempotent.
    That is, whether the operation applied on the same value, it returns that
    value again.
    For example, if the operation returns the maximum of two elements, it is
    idempotent because <c> max (a, a) == a</c>.

    It is possible to instantiate this with the standard type argument syntax,
    documented below, or with one argument in a cute syntax:
    <c> Operation (Magma1, Magma2)</c>.

    \tparam Operation
        The operation, normally a type in namespace \c math::callable.
    \tparam Magma
        A type that belongs to the magma of interest.
    */
    template <class ... Arguments> struct idempotent;

    template <class Operation, class Magma> struct idempotent <Operation, Magma>
    : operation::is_idempotent <typename
        std::decay <Operation>::type::template apply <Magma, Magma>> {};

    template <class Operation, class Magma1, class Magma2>
        struct idempotent <Operation (Magma1, Magma2)>
    : detail::derive_if_all_magma_tags <
        idempotent <Operation, Magma1>, Magma1, Magma2> {};


    /**
    Whether an operation on a magma distributes over another operation on the
    same magma.
    For example, a semiring requires that
    <c> a * (b + c) == (a * b) + (a * c)</c>.
    This is left distributivity; the mirror image is right distributivity.

    It is possible to instantiate this with the standard type argument syntax,
    documented below, or with one argument in a cute syntax:
    <c> OuterOperation (Magma1, InnerOperation (Magma2, Magma3))</c>
    (for left distributivity) or
    <c> OuterOperation (InnerOperation (Magma1, Magma2), Magma3)</c>
    (for right distributivity).

    \tparam Direction
        The direction of the distributivity: math::left, math::right, or
        math::either.
    \tparam OuterOperation
        The outer operation: in the example, times.
    \tparam InnerOperation
        The inner operation: in the example, plus.
    \tparam Magma
        A type that belongs to the magma of interest.
    */
    template <class ... Arguments> struct distributive;

    template <class Direction, class OuterOperation, class InnerOperation,
        class Magma>
    struct distributive <Direction, OuterOperation, InnerOperation, Magma>
    : operation::is_distributive <typename magma_tag <Magma>::type,
        Direction, OuterOperation, InnerOperation> {};

    template <class OuterOperation, class InnerOperation,
        class Magma1, class Magma2, class Magma3>
    struct distributive <
        OuterOperation (Magma1, InnerOperation (Magma2, Magma3))>
    : detail::derive_if_all_magma_tags <
        distributive <left, OuterOperation, InnerOperation, Magma1>,
        Magma1, Magma2, Magma3> {};

    template <class OuterOperation, class InnerOperation,
        class Magma1, class Magma2, class Magma3>
    struct distributive <
        OuterOperation (InnerOperation (Magma1, Magma2), Magma3)>
    : detail::derive_if_all_magma_tags <
        distributive <right, OuterOperation, InnerOperation, Magma1>,
        Magma1, Magma2, Magma3> {};

    /**
    Indicate whether a magma is a monoid.
    A monoid is a magma with an associative operation and an identity.

    \tparam Operation
        The operation, normally a type in namespace \c math::callable.
    \tparam Magma
        A type that belongs to the magma of interest.

    \internal
    This is automatically determined.
    */
    template <class Operation, class Magma> struct monoid
    : boost::mpl::and_ <
        associative <Operation, Magma>,
        has <callable::identity <Magma, Operation>()>> {};

    /**
    Indicate whether a magma is a semiring.
    This describes a number of properties related to the magma's behaviour under
    two operations, normally multiplication and addition.

    \tparam Direction
        The direction of interest.
        math::either makes this check whether \a Magma is a normal semiring.
        math::left or math::right can be used to check whether \a Magma is a
        left of right semiring.
    \tparam Multiplication
        The "multiply" operation, normally math::callable::times.
    \tparam Addition
        The "add" operation, normally math::callable::plus.
    \tparam Magma
        A type that belongs to the magma of interest.

    \internal
    This is specified explicitly in namespace <c>property</c>, because it is not
    fully checkable at compile time.
    */
    template <class ... Arguments> struct semiring;

    namespace detail {

        /**
        Compile-time constant that evaluates to true, but asserts that Magma
        is indeed a semiring, to the extent possible at compile-time.
        */
        template <class Direction, class Multiplication, class Addition,
            class Magma>
        struct assert_semiring : boost::mpl::true_ {
            // Commutative monoid under addition.
            static_assert (is::monoid <Addition, Magma>::value,
                "A semiring must be a monoid under addition.");
            static_assert (is::commutative <Addition, Magma>::value,
                "For a semiring, addition must be commutative.");

            // Monoid under multiplication.
            static_assert (is::monoid <Multiplication, Magma>::value,
                "For a semiring, must be a monoid under addition.");

            // Multiplication distributes over addition in Direction.
            static_assert (distributive <Direction,
                Multiplication, Addition, Magma>::value,
                "For a semiring, multiplication must distribute over addition."
                );

            // The additive identity is a multiplicative annihilator.
            // This can only be checked at run-time.
        };

    } // namespace detail

    template <class Direction, class Multiplication, class Addition,
        class Magma>
    struct semiring <Direction, Multiplication, Addition, Magma>
    // Forward to operation::is_semiring, but if that is true_, check.
    : boost::mpl::if_ <
        operation::is_semiring <typename magma_tag <Magma>::type,
            Direction, Multiplication, Addition>,
        detail::assert_semiring <
            Direction, Multiplication, Addition, Magma>,
        boost::mpl::false_>::type {};

} // namespace is

/**
Exception that is thrown by the explicit constructor of a magma if the value
passed in cannot be contained by the magma type to be constructed.
*/
class magma_not_convertible : public std::logic_error {
public:
    magma_not_convertible()
    : std::logic_error ("Magma value not convertible") {}

    /**
    Construct the exception with a message explaining that a value described by
    \a source could not be converted to type described by \a target.
    \param source A string describing the original value
    \param target A string describing the target type.
    */
    magma_not_convertible (
        std::string const & source, std::string const & target)
    : std::logic_error ("Magma value not convertible from " + target
        + " to " + target) {}
};

/**
Exception that indicates an error while performing an inverse operation.
*/
class operation_error : public std::runtime_error {
public:
    operation_error()
    : std::runtime_error ("Error while performing inverse operation on magma")
    {}

    operation_error (std::string const & message)
    : std::runtime_error (message) {}
};

/**
Exception that indicates that an inverse operation is not defined for the two
values that it is called with.
*/
class operation_undefined : public operation_error {
    const char * default_message() {
        static const char * message = "Attempt to perform inverse operation"
            " on two values for which this is undefined";
            return message;
    }
public:
    operation_undefined() : operation_error (default_message()) {}

    /**
    Construct the exception with a message explaining what it is about the two
    values that caused the inverse to be undefined.
    */
    operation_undefined (std::string const & message)
    : operation_error (default_message() + (": " + message)) {}
};

/**
Exception that indicates that the inverse of an annihilator was required.
For example, an inverse operation was called with an annihilator as its second
argument.
Division by zero is an example of this.
*/
class inverse_of_annihilator : public operation_undefined {
public:
    inverse_of_annihilator()
    : operation_undefined ("the second argument is an annihilator") {}

    inverse_of_annihilator (std::string const & message)
    : operation_undefined (message) {}
};

/**
Exception that indicates that \c divide was called with (generalised) zero as
its second argument.
*/
class divide_by_zero : public inverse_of_annihilator {
public:
    divide_by_zero()
    : inverse_of_annihilator ("divide by generalised zero") {}
};

} // namespace math

/* Generate operators. */

#define MATH_MAGMA_GENERATE_OPERATOR(tag_predicate, name, symbol) \
    template <class Magma1, class Magma2> inline \
    typename ::boost::lazy_enable_if < ::boost::mpl::and_ < \
        tag_predicate <typename \
            ::math::magma_tag_all <Magma1, Magma2>::type>, \
        ::math::has < ::math::callable::name (Magma1, Magma2)>>, \
        ::math::result_of < ::math::callable::name (Magma1, Magma2)>>::type \
    operator symbol (Magma1 const & magma1, Magma2 const & magma2) \
    { \
        ::math::callable::name implementation; \
        return implementation (magma1, magma2); \
    }

/**
Generate operator*, operator+, operator-, operator/, operator==, operator!=,
operator<, and operator<< as far as the operations are defined (in
math::operation).
The tag_predicate is a predicate that gets applied to the tag to determine
whether to switch on the operators.
Put this in the namespace where the class is, so that argument-dependent lookup
works properly.

If <c> math::operation::generate_operators \<MagmaTag></c> evaluate to true, and
the operation is defined in math::callable::, then switch on the operator and
forward to the operation.
*/
#define MATH_MAGMA_GENERATE_OPERATORS(tag_predicate) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, times, *) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, plus, +) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, minus<>, -) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, divide<>, /) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, equal, ==) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, not_equal, !=) \
    MATH_MAGMA_GENERATE_OPERATOR (tag_predicate, compare, <) \
    /* print */ \
    template <class Magma> inline \
        typename ::boost::enable_if < ::boost::mpl::and_ < \
            tag_predicate <typename ::math::magma_tag <Magma>::type>, \
            ::math::has < ::math::callable::print (std::ostream &, Magma)>>, \
            std::ostream & \
    >::type operator<< (std::ostream & stream, Magma const & magma) \
    { \
        ::math::callable::print implementation; \
        implementation (stream, magma); \
        return stream; \
    }

#endif // MATH_MAGMA_HPP_INCLUDED
