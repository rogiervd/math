/*
Copyright 2013, 2014 Rogier van Dalen.

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

// \todo The order of arguments in this file needs to good look-over.
// \todo Is is possible to place some checks on what has been tested?

#ifndef MATH_CHECK_CHECK_MAGMA_HPP_INCLUDED
#define MATH_CHECK_CHECK_MAGMA_HPP_INCLUDED

#include <cassert>
#include <iosfwd>
#include <ostream>
#include <iostream>
#include <sstream>
#include <functional>

#include <boost/mpl/bool.hpp>
#include <boost/utility/enable_if.hpp>

#include "meta/any.hpp"
#include "meta/transform.hpp"
#include "meta/vector.hpp"

#include "rime/core.hpp"
#include "rime/if.hpp"
#include "rime/call_if.hpp"
#include "rime/enable_if_constant.hpp"
#include "rime/sign.hpp"
#include "rime/nothing.hpp"

#include "range/core.hpp"
#include "range/for_each.hpp"

#include "../magma.hpp"

namespace math { namespace check_detail {

    /**
    There seems to be a problem with optimised builds on CLang 3.0, which
    causes spurious errors.
    If the Magma is an integer, and NDEBUG is defined, and the compiler is
    CLang 3.0, this will evaluate to true, so some tests can be disabled.
    */
    template <class Operation, class Magma1, class Magma2>
        struct strange_behaviour
    : boost::mpl::false_ {};

#if (NDEBUG && BOOST_CLANG && __clang_major__ == 3 && __clang_minor__ == 0)
    template <class Magma>
        struct strange_behaviour <callable::plus, Magma, Magma>
    : boost::mpl::bool_ <std::numeric_limits <Magma>::is_integer> {};
#endif

}} // namespace math::check_detail

/**
Define a namespace in which confuser operators are defined, so that it can be
checked that magma types define operators that can be found through.
argument-dependent lookup.
*/
namespace math_check_different_namespace {

    /*
    Confuser operator definitions that make sure that the search for operators
    does not go to the global namespace.
    The operator definitions will never match anything, but the compiler does
    not realise that.
    Note that these must be defined before they are used below.
    */
    template <class Type1, class Type2>
        typename boost::enable_if <std::is_same <Type1, void>, bool>::type
        operator == (Type1 const &, Type2 const &);

    template <class Type1, class Type2>
        typename boost::enable_if <std::is_same <Type1, void>, bool>::type
        operator * (Type1 const &, Type2 const &);

    template <class Type1, class Type2>
        typename boost::enable_if <std::is_same <Type1, void>, bool>::type
        operator + (Type1 const &, Type2 const &);

    template <class Type1, class Type2>
        typename boost::enable_if <std::is_same <Type1, void>, bool>::type
        operator / (Type1 const &, Type2 const &);

    template <class Type1, class Type2>
        typename boost::enable_if <std::is_same <Type1, void>, bool>::type
        operator - (Type1 const &, Type2 const &);


    /**
    Check whether operators have been defined and yield the exact same result as
    the classes in math::callable.
    */
    template <class Operation, class Magma1, class Magma2>
    void check_operator (Operation, Magma1 const & m1, Magma2 const & m2) {}

    // Operators can return a different but related type.
    // (Like with operations on char.)
    // So make sure the type is the same with decltype (result_function).
    template <class Magma1, class Magma2> void check_operator (
        math::callable::equal equal, Magma1 const & m1, Magma2 const & m2)
    {
        auto result_function = equal (m1, m2);
        // Also check not_equal, which is automatically defined.
        auto result_not_function = math::not_equal (m1, m2);
        BOOST_CHECK (result_not_function == !result_function);

        decltype (result_function) result_operator = (m1 == m2);
        BOOST_CHECK (result_function == result_operator);

        decltype (result_not_function) result_not_operator = (m1 != m2);
        BOOST_CHECK (result_not_function == result_not_operator);
    }

    template <class Magma1, class Magma2> void check_operator (
        math::callable::times times, Magma1 const & m1, Magma2 const & m2)
    {
        auto result_function = times (m1, m2);
        decltype (result_function) result_operator = (m1 * m2);
        BOOST_CHECK (result_function == result_operator);
    }

    template <class Magma1, class Magma2>
        typename boost::disable_if <math::check_detail::strange_behaviour <
            math::callable::plus, Magma1, Magma2>
        >::type check_operator (
            math::callable::plus plus, Magma1 const & m1, Magma2 const & m2)
    {
        auto result_function = plus (m1, m2);
        decltype (result_function) result_operator = (m1 + m2);
        BOOST_CHECK (result_function == result_operator);
    }

    template <class Magma1, class Magma2> void check_operator (
        math::callable::divide<> divide, Magma1 const & m1, Magma2 const & m2)
    {
        BOOST_TEST_CHECKPOINT ("Checking division operator.");
        auto result_function = divide (m1, m2);
        decltype (result_function) result_operator = (m1 / m2);
        BOOST_CHECK (result_function == result_operator);
    }

    template <class Magma1, class Magma2> void check_operator (
        math::callable::minus<> minus, Magma1 const & m1, Magma2 const & m2)
    {
        auto result_function = minus (m1, m2);
        decltype (result_function) result_operator = (m1 - m2);
        BOOST_CHECK (result_function == result_operator);
    }

    template <class Magma> void check_operator (
        math::callable::print print, Magma const & magma)
    {
        std::ostringstream stream_function;
        print (stream_function, magma);
        stream_function << " (being annoying)";
        std::string result_function = stream_function.str();

        std::ostringstream stream_operator;
        stream_operator << magma;
        std::string result_operator = stream_operator.str();

        BOOST_CHECK_MESSAGE (result_function == result_operator,
            "'print' produces something different from 'operator <<': \""
            << result_function << "\" and \"" << result_operator << "\".");
    }

} // namespace math_check_different_namespace

namespace math {

namespace check_detail {

    template <class Value, class Reference> inline
        void check_same_magma (Value const & value, Reference const & reference,
            bool should_be_member = true)
    {
        static_assert (is_magma <decltype (value)>::value, "");
        static_assert (std::is_same <
            typename magma_tag <decltype (value)>::type,
            typename magma_tag <Reference>::type>::value, "");
        BOOST_CHECK (math::is_member (value) == should_be_member);
    }

    /**
    Check whether equality is transitive.
    */
    struct check_equal_transitive {
        template <class Example1, class Example2, class Example3>
            void operator() (
                Example1 const & example1, Example2 const & example2,
                Example3 const & example3) const
        {
            if (equal (example1, example2) && equal (example1, example3)) {
                BOOST_CHECK (equal (example2, example3));
            }
        }
    };

    /**
    Check a binary predicate that implements a strict weak ordering for internal
    consistency.
    */
    template <class Predicate, class Example1, class Example2, class Example3>
    void check_strict_weak_ordering (Predicate const & predicate,
        Example1 const & example1, Example2 const & example2,
        Example3 const & example3)
    {
        bool order12 = predicate (example1, example2);
        bool order21 = predicate (example2, example1);
        bool order13 = predicate (example1, example3);
        bool order31 = predicate (example3, example1);
        bool order23 = predicate (example2, example3);
        bool order32 = predicate (example3, example2);

        BOOST_CHECK (! (order12 && order21));
        BOOST_CHECK (! (order13 && order31));
        BOOST_CHECK (! (order23 && order32));

        // if a < b and b < c, then a < c.
        if (order13 && order32) { BOOST_CHECK (order12); }
        if (order12 && order23) { BOOST_CHECK (order13); }
        if (order23 && order31) { BOOST_CHECK (order21); }
        if (order21 && order13) { BOOST_CHECK (order23); }
        if (order32 && order21) { BOOST_CHECK (order31); }
        if (order31 && order12) { BOOST_CHECK (order32); }
    }

    /**
    Check whether "compare" is consistent with "equal".
    */
    struct check_equal_and_compare {
        template <class Example1, class Example2>
            void operator() (
                Example1 const & example1, Example2 const & example2) const
        {
            auto is_equal = equal (example1, example2);
            auto is_equal_according_to_compare = rime::and_ (
                !compare (example1, example2), !compare (example2, example1));

            BOOST_CHECK_EQUAL (is_equal, is_equal_according_to_compare);
        }
    };

    /**
    Check whether the strict weak ordering implied by "compare" is consistent.
    */
    struct check_compare {
        template <class Operation,
            class Example1, class Example2, class Example3>
        void operator() (Operation const & operation, Example1 const & example1,
            Example2 const & example2, Example3 const & example3) const
        { check_strict_weak_ordering (compare, example1, example2, example3); }
    };

    struct check_order {
        template <class Operation,
            class Example1, class Example2, class Example3>
        void operator() (Operation const & operation, Example1 const & example1,
            Example2 const & example2, Example3 const & example3) const
        {
            check_strict_weak_ordering (callable::order <Operation>(),
                example1, example2, example3);

            auto result = operation (example1, example2);
            if (order <Operation> (example1, example2)) {
                BOOST_CHECK (result == example1);
            } else {
                BOOST_CHECK (result == example2);
            }
        }
    };

    template <class Magma,
        typename Enable = typename boost::disable_if <
            has <callable::print (std::ostream &, Magma)>>::type>
    inline char const * describe_magma (Magma const & magma, void * = 0)
    { return "[magma value]"; }

    template <class Magma,
        typename Enable = typename boost::enable_if <
            has <callable::print (std::ostream &, Magma)>>::type>
    inline Magma describe_magma (Magma const & magma)
    { return magma; }

    char const * describe_operation (callable::plus) { return "plus"; }
    char const * describe_operation (callable::minus<>) { return "minus"; }
    char const * describe_operation (callable::minus <left>)
    { return "left minus"; }
    char const * describe_operation (callable::minus <right>)
    { return "right minus"; }

    char const * describe_operation (callable::choose) { return "choose"; }

    char const * describe_operation (callable::times) { return "times"; }
    char const * describe_operation (callable::divide<>) { return "divide"; }
    char const * describe_operation (callable::divide <left>)
    { return "left divide"; }
    char const * describe_operation (callable::divide <right>)
    { return "right divide"; }

    template <class Operation> char const * describe_operation (Operation)
    { return "[operation]"; }

    /**
    Check whether result1 and result2 are equal, using exact or approximate
    equality depending on the operations used to acquire the results.
    \tparam A meta range containing the operations used.
        If any operation is approximate, the equality also will be assumed
        approximate.
    */
    template <class Operations, class Result1, class Result2> inline
        bool check_equality (Result1 const & result1, Result2 const & result2,
            char const * message)
    {
        typedef typename meta::any <meta::transform <
                is::approximate <boost::mpl::_1>, Operations
            >>::type check_approximate;
        static_assert ((!check_approximate::value) ||
            has <callable::approximately_equal (Result1, Result2)>::value,
            "If the computation is approximate, then approximately_equal must "
            "be implemented so the result can be checked.");
        auto const & equality = rime::if_ (check_approximate(),
            math::approximately_equal, math::equal);
        const char * approximate_message = check_approximate() ?
            " should be approximately equal." :
            " should be exactly equal.";
        bool is_equal = equality (result1, result2);
        BOOST_CHECK_MESSAGE (is_equal, message << ": "
            << describe_magma (result1) << " and " << describe_magma (result2)
            << approximate_message);
        return is_equal;
    }

    struct check_commutative {
        template <class Left, class Right, class Operation>
            void operator() (Left const & left, Right const & right,
                Operation const & operation) const
        {
            auto result1 = operation (left, right);
            auto result2 = operation (right, left);
            BOOST_CHECK_MESSAGE (result1 == result2, "Not commutative");
        }
    };

    struct check_associative {
        template <class Example1, class Example2, class Example3,
            class Operation>
        void operator() (Example1 const & example1, Example2 const & example2,
            Example3 const & example3, Operation const & operation) const
        {
            auto result1 = operation (operation (example1, example2), example3);
            auto result2 = operation (example1, operation (example2, example3));

            if (!check_equality <meta::vector <
                Operation (Operation (Example1, Example2), Example3),
                Operation (Example1, Operation (Example2, Example3))>>
                (result1, result2, "Not associative."))
            {
                std::cout << "  with "
                    << describe_operation (operation) << " ("
                    << describe_operation (operation) << " ("
                    << describe_magma (example1) << ", "
                    << describe_magma (example2) << "), "
                    << describe_magma (example3) << ")" << std::endl;
            }
        }
    };

    /* check_inverse_operation: check that the inverse operation actually is. */
    template <class Direction> struct check_inverse_operation_safe {
        template <class Example, class Invertee, class Operation>
            void operator() (Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            BOOST_TEST_CHECKPOINT ("Checking inverse_operation.");
            /*
            If a test fails with essentially a division-by-zero error after this
            checkpoint, check that you have is_annihilator <Magma> defined.
            The inverse of an operation on an annihilator is undefined, so
            check_magma automatically skips invertees that are equal to the
            annihilator.
            */
            auto inverse = inverse_operation <Direction> (operation);
            auto left_operand = rime::if_ (std::is_same <Direction, left>(),
                invertee, example);
            auto right_operand = rime::if_ (std::is_same <Direction, left>(),
                example, invertee);
            auto undone =
                inverse (operation (left_operand, right_operand), invertee);

            if (!check_equality <meta::vector <
                decltype (inverse) (Operation (decltype (left_operand),
                    decltype (right_operand)), Invertee)>>
                (undone, example, "Inverse operation not an inverse."))
            {
                std::cout << "  with " << describe_operation (operation)
                    << " and " << describe_operation (inverse) << " on "
                    << describe_magma (example) << ", "
                    << describe_magma (invertee) << std::endl;
            }

            math_check_different_namespace::check_operator (
                inverse, operation (left_operand, right_operand), invertee);
        }
    };

    template <class Direction> struct check_inverse_operation_throw {
        template <class Example, class Invertee, class Operation>
            void operator() (Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            // Check for "operation_undefined", not "inverse_by_annihilator",
            // because "example" may be an annihilator too.
            BOOST_CHECK_THROW (check_inverse_operation_safe <Direction>() (
                example, invertee, operation), operation_undefined);
        }
    };

    /**
    If the inverse says it throws when it is called with an annihilator, check
    that it actually does.
    If not, do not do anything, because that would probably crash the program.
    */
    template <class Direction> struct check_inverse_operation_annihilator {
        template <class Example, class Invertee, class Operation>
            void operator() (Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            assert (is_annihilator <Operation> (invertee));

            auto inverse = inverse_operation <Direction> (operation);
            auto should_throw = is::throw_if_undefined <
                decltype (inverse) (Example, Invertee)>();
            rime::call_if (should_throw,
                check_inverse_operation_throw <Direction>(),
                rime::nothing,
                example, invertee, operation);
        }
    };

    // General: call implementation.
    template <class Direction> struct check_inverse_operation {
        template <class Example, class Invertee, class Operation>
        void operator() (Example const & example, Invertee const & invertee,
            Operation const & operation) const
        {
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <Direction>(),
                check_inverse_operation_annihilator <Direction>(),
                example, invertee, operation);
        }
    };

    // Either: check for either, and for left and right.
    template <> struct check_inverse_operation <either> {
        template <class Example, class Invertee, class Operation>
        void operator() (Example const & example, Invertee const & invertee,
            Operation const & operation) const
        {
            // This is not done smartly, because maybe "is_annihilator" will
            // allow a direction to be specified too, in the future.
            // either.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <either>(),
                check_inverse_operation_annihilator <either>(),
                example, invertee, operation);
            // left.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <left>(),
                check_inverse_operation_annihilator <left>(),
                example, invertee, operation);
            // right.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <right>(),
                check_inverse_operation_annihilator <right>(),
                example, invertee, operation);
        }
    };

    struct check_idempotent {
        template <class Example, class Operation>
            void operator() (
                Example const & example, Operation const & operation) const
        {
            auto result = operation (example, example);
            BOOST_CHECK_MESSAGE (result == example, "Not idempotent.");
        }
    };

    struct check_path_operation {
        template <class Example1, class Example2, class Operation>
            void operator() (
                Example1 const & example1, Example2 const & example2,
                Operation const & operation) const
        {
            auto result = operation (example1, example2);
            BOOST_CHECK_MESSAGE ((result == example1) || (result == example2),
                "Not a path operation.");
        }
    };

    class check_identity {
        template <class Identity, class Operation, class Example>
            void check_with (Identity const & identity,
                Operation const & operation, Example const & example) const
        {
            check_same_magma (identity, example);

            auto result_left = operation (identity, example);
            check_same_magma (result_left, example);
            if (!check_equality <meta::vector <>> (result_left, example,
                "Identity element changes the operand from the left"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (identity) << ", "
                    << describe_magma (example) << ")" << std::endl;
            }

            auto result_right = operation (example, identity);
            check_same_magma (result_right, example);
            if (!check_equality <meta::vector <>> (result_right, example,
                "Identity element changes the operand from the right"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (example) << ", "
                    << describe_magma (identity) << ")" << std::endl;
            }
        }

    public:
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            check_with (math::identity <Example, Operation>(),
                operation, example);
        }

        template <class Example> void operator() (
            callable::times const & operation, Example const & example) const
        {
            check_with (math::identity <Example, callable::times>(),
                operation, example);
            check_with (math::one <Example>(), operation, example);
        }

        template <class Example> void operator() (
            callable::plus const & operation, Example const & example) const
        {
            check_with (math::identity <Example, callable::plus>(),
                operation, example);
            check_with (math::zero <Example>(), operation, example);
        }
    };

    struct check_annihilator_with {
        template <class Annihilator, class Operation, class Example>
            void operator() (Annihilator const & annihilator,
                Operation const & operation, Example const & example)
        {
            check_same_magma (annihilator, example);

            bool annihilator_is_annihilator
                = is_annihilator (operation, annihilator);
            BOOST_CHECK (annihilator_is_annihilator);
            if (!annihilator_is_annihilator) {
                std::cout << "  " << describe_magma (annihilator)
                    << " is supposed to be an annihilator under operation '"
                    << describe_operation (operation) << "'." << std::endl;
            }

            auto result_left = operation (annihilator, example);
            check_same_magma (result_left, example);
            if (!check_equality <meta::vector <>> (result_left, annihilator,
                "Annihilator does not annihilate the operand from the left"))
            {
                std::cout << "  with '" << describe_operation (operation)
                    << "'' (" << describe_magma (annihilator) << ", "
                    << describe_magma (example) << ")" << std::endl;
            }

            auto result_right = operation (example, annihilator);
            check_same_magma (result_right, example);
            if (!check_equality <meta::vector <>> (result_right, annihilator,
                "Annihilator does not annihilate the operand from the right"))
            {
                std::cout << "  with '" << describe_operation (operation)
                    << "'' (" << describe_magma (example) << ", "
                    << describe_magma (annihilator) << ")" << std::endl;
            }
        }
    };

    struct check_annihilator {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            auto annihilator = math::annihilator <Example, Operation>();
            BOOST_CHECK (math::is_annihilator <Operation> (annihilator));
            BOOST_CHECK (math::is_annihilator (operation, annihilator));
            check_annihilator_with() (annihilator, operation, example);
        }
    };

    struct check_non_member {
        template <class Example> void operator() (Example const & example) const
        {
            auto result = non_member <Example>();
            check_same_magma (result, example, false);
            BOOST_CHECK (!is_member (result));
        }
    };

    template <class Direction> struct check_invert_safe;
    template <class Direction> struct check_invert;

    template <> struct check_invert_safe <left> {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            auto inverse = math::invert <left, Operation> (example);
            auto undone = operation (inverse, example);

            if (!check_equality <meta::vector <Operation (
                math::callable::invert <left, Operation> (Example), Example)>>
                (undone, math::identity <Example, Operation>(),
                    "The 'invert' operation does not return the left inverse"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (inverse) << ", "
                    << describe_magma (example) << ")" << std::endl;
            }
        }
    };

    template <> struct check_invert_safe <right> {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            auto inverse = math::invert <right, Operation> (example);
            auto undone = operation (example, inverse);

            if (!check_equality <meta::vector <Operation (
                Example, math::callable::invert <right, Operation> (Example))>>
                (undone, math::identity <Example, Operation>(),
                    "The 'invert' operation does not return the right inverse"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (example) << ", "
                    << describe_magma (inverse) << ")" << std::endl;
            }
        }
    };

    template <> struct check_invert_safe <either> {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            // Check that invert <either> returns something that works from
            // either side.
            auto inverse = math::invert <either, Operation> (example);

            auto undone1 = operation (example, inverse);
            if (!check_equality <meta::vector <Operation (
                Example, callable::invert <either, Operation> (Example))>>
                (undone1, math::identity <Example, Operation>(),
                    "The 'invert' operation does not return the inverse"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (inverse) << ", "
                    << describe_magma (example) << ")" << std::endl;
            }

            auto undone2 = operation (inverse, example);
            if (!check_equality <meta::vector <Operation (
                Example, callable::invert <either, Operation> (Example))>>
                (undone2, math::identity <Example, Operation>(),
                    "The 'invert' operation does not return the inverse"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (example) << ", "
                    << describe_magma (inverse) << ")" << std::endl;
            }

            // Check that invert <left> and invert <right> are defined.
            check_invert_safe <left>() (operation, example);
            check_invert_safe <right>() (operation, example);
        }
    };

    template <class Direction> struct check_invert_throw {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            // Check for "operation_undefined", not "inverse_by_annihilator",
            // because "example" may be an annihilator too.
            BOOST_CHECK_THROW (check_invert_safe <Direction>() (
                operation, example), operation_undefined);
        }
    };

    /**
    If "invert" says it throws when it is called with an annihilator, check that
    it actually does.
    If not, do not do anything, because that would probably crash the program.
    */
    template <class Direction> struct check_invert_annihilator {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            assert (is_annihilator <Operation> (example));

            auto should_throw = is::throw_if_undefined <
                callable::invert <Direction, Operation> (Example)>();
            rime::call_if (should_throw,
                check_invert_throw <Direction>(), rime::nothing,
                operation, example);
        }
    };

    template <class Direction> struct check_invert {
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            rime::call_if (!is_annihilator <Operation> (example),
                check_invert_safe <Direction>(),
                check_invert_annihilator <Direction>(),
                operation, example);
        }
    };

    struct check_reverse {
        // Check that reverse (reverse (a)) returns the same magma.
        template <class Operation, class Example> void operator() (
            Operation const & operation, Example const & example) const
        {
            auto result = reverse (operation, reverse (operation, example));
            static_assert (std::is_same <
                typename magma_tag <decltype (result)>::type,
                typename magma_tag <Example>::type>::value, "");

            typedef callable::reverse <Operation> reverse_type;
            if (!check_equality <
                meta::vector <reverse_type (reverse_type (Example))>>
                (result, example,
                "Applying 'reverse' twice does not return the original value"))
            {
                std::cout << "  with " << describe_operation (operation)
                    << " and " << describe_magma (example) << std::endl;
            }
        }

        // Check that reverse (operation (reverse (b), reverse (a))) is of the
        // same magma and has the same value as operation (a, b).
        template <class Operation, class Example1, class Example2>
            void operator() (Operation const & operation,
                Example1 const & example1, Example2 const & example2) const
        {
            typedef callable::reverse <Operation> reverse_type;
            reverse_type reverse;
            auto result = reverse (operation (
                reverse (example2), reverse (example1)));

            static_assert (std::is_same <
                typename magma_tag <decltype (result)>::type,
                typename magma_tag <Example1>::type>::value, "");

            if (!check_equality <meta::vector <reverse_type (Operation (
                    reverse_type (Example2), reverse_type (Example1)))>>
                (result, operation (example1, example2),
                    "Applying the operation with reversing yields a different "
                    "result from applying it without"))
            {
                std::cout << "  with " << describe_operation (operation) << " ("
                    << describe_magma (example1) << ", "
                    << describe_magma (example2) << ")" << std::endl;
            }
        }
    };

    struct check_print {
        // Check that "print" is consistent with operator<<.
        template <class Magma> void operator() (Magma const & magma) const {
            math_check_different_namespace::check_operator (print, magma);
        }
    };

    struct check_left_distributive {
        template <class Multiplication, class Addition,
            class Example1, class Example2, class Example3>
        void operator() (Multiplication const & multiplication,
            Addition const & addition, Example1 const & example1,
            Example2 const & example2, Example3 const & example3)
        {
            // a*(b+c) == a*b + a*c.
            auto result1 =
                multiplication (example1, addition (example2, example3));
            auto result2 =
                addition (multiplication (example1, example2),
                    multiplication (example1, example3));

            if (!check_equality <meta::vector <
                Multiplication (Example1, Addition (Example2, Example3)),
                Addition (Multiplication (Example1, Example2),
                    Multiplication (Example1, Example3))>>
                (result1, result2, "Not left distributive"))
            {
                std::cout << "  with "
                    << describe_operation (multiplication) << " ("
                    << describe_magma (example1) << ", "
                    << describe_operation (addition) << " ("
                    << describe_magma (example2) << ", "
                    << describe_magma (example3) << "))" << std::endl;
            }
        }
    };

    struct check_right_distributive {
        template <class Multiplication, class Addition,
            class Example1, class Example2, class Example3>
        void operator() (Multiplication const & multiplication,
            Addition const & addition, Example1 const & example1,
            Example2 const & example2, Example3 const & example3)
        {
            // (a+b)*c == a*c + b*c.
            auto result1 =
                multiplication (addition (example1, example2), example3);
            auto result2 =
                addition (multiplication (example1, example3),
                    multiplication (example2, example3));

            if (!check_equality <meta::vector <
                Multiplication (Addition (Example1, Example2), Example3),
                Addition (Multiplication (Example1, Example3),
                    Multiplication (Example2, Example3))>>
                (result1, result2, "Not right distributive"))
            {
                std::cout << "  with "
                    << describe_operation (multiplication) << " ("
                    << describe_operation (addition) << " ("
                    << describe_magma (example1) << ", "
                    << describe_magma (example2) << "), "
                    << describe_magma (example3) << ")" << std::endl;
            }
        }
    };

    using std::bind;
    using std::ref;
    using std::placeholders::_1;

    template <class Examples> struct check_equal_on {
        void operator() (Examples const & examples) const
        { range::for_each (bind (ref (*this), ref (examples), _1), examples); }

        template <class Example1>
            void operator() (
                Examples const & examples, Example1 const & example1) const
        { range::for_each (bind (ref (*this), ref (example1), _1), examples); }

        template <class Example1, class Example2>
            void operator() (
                Example1 const & example1, Example2 const & example2) const
        {
            if (static_cast <void const *> (&example1)
                != static_cast <void const *> (&example2))
            {
                BOOST_CHECK (!(equal (example1, example2)));
            } else {
                BOOST_CHECK (equal (example1, example2));
            }
        }
    };

    template <class Magma, class Operation, class Examples>
        struct check_magma_implementation
    {
        typedef typename magma_tag <Magma>::type tag;

        // Zero examples: check basic properties.
        void operator() (Operation operation, Examples const & examples) const {
            static_assert (math::is_magma <Magma>::value, "");
            static_assert (math::is_magma <Magma &>::value, "");
            static_assert (math::is_magma <Magma const>::value, "");
            static_assert (math::is_magma <Magma const &>::value, "");

            static_assert (!std::is_same <tag, not_a_magma_tag>::value, "");

            static_assert (math::has <callable::is_member (Magma)>::value, "");

            // Check with each example.
            range::for_each (bind (ref (*this),
                    ref (operation), ref (examples), _1),
                examples);
        }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
        // One example: check is_member, operation, identity, invert, reverse.
        template <class Example1>
            void operator() (Operation operation, Examples const & examples,
                Example1 const & example1) const
        {
            typedef typename magma_tag <decltype (example1)>::type example_tag;
            static_assert (std::is_same <tag, example_tag>::value, "");
            // The examples must all be members.
            BOOST_CHECK (is_member (example1));

            // Check equality between example1 and a copy of it.
            BOOST_CHECK (example1 == example1);
            {
                // math::approximately_equal, if it exists; otherwise, just
                // math::equal.
                auto const & approximately_equal = rime::if_ (
                    has <callable::approximately_equal (Magma, Magma)>(),
                    math::approximately_equal, math::equal);
                Example1 example1_copy = example1;
                static_assert (has <callable::equal (
                    decltype (example1_copy), decltype (example1))>::value, "");
                BOOST_CHECK (example1_copy == example1);
                BOOST_CHECK (! (example1_copy != example1));
                BOOST_CHECK (equal (example1_copy, example1));
                BOOST_CHECK (!not_equal (example1_copy, example1));
                BOOST_CHECK (approximately_equal (example1_copy, example1));
            }

            rime::call_if (is::idempotent <Operation, Magma>(),
                check_idempotent(), rime::nothing, example1, operation);

            rime::call_if (has <callable::identity <Magma, Operation>()>(),
                check_identity(), rime::nothing, operation, example1);

            rime::call_if (has <callable::non_member <Magma>()>(),
                check_non_member(), rime::nothing, example1);

            rime::call_if (has <callable::annihilator <Magma, Operation>()>(),
                check_annihilator(), rime::nothing, operation, example1);

            /* Check invert. */
            // Left.
            rime::call_if (
                has <callable::invert <math::left, Operation> (Magma)>(),
                check_invert <math::left>(), rime::nothing,
                operation, example1);
            // Right.
            rime::call_if (
                has <callable::invert <math::right, Operation> (Magma)>(),
                check_invert <math::right>(), rime::nothing,
                operation, example1);
            // Either.
            rime::call_if (
                has <callable::invert <math::either, Operation> (Magma)>(),
                check_invert <math::either>(), rime::nothing,
                operation, example1);

            // Check reverse.
            rime::call_if (has <callable::reverse <Operation> (Magma)>(),
                check_reverse(), rime::nothing, operation, example1);

            // Check that print is consistent with operator <<.
            // Not really a requirement.
            // rime::call_if (has <callable::print (std::ostream &, Magma)>(),
            //     check_print(), rime::nothing, example1);

            // Check with each combination of two examples.
            // _1 will be example2.
            range::for_each (bind (ref (*this),
                    ref (operation), ref (examples), ref (example1), _1),
                examples);
        }
#pragma GCC diagnostic pop

        template <class Example1, class Example2>
            void operator() (Operation operation, Examples const & examples,
                Example1 const & example1, Example2 const & example2) const
        {
            auto result = operation (example1, example2);
            static_assert (is_magma <decltype (result)>::value, "");
            static_assert (std::is_same <
                typename magma_tag <decltype (result)>::type, tag>::value,
                "The result of the operation must be the same magma.");
            BOOST_CHECK (is_member (result));

            static_assert (has <callable::equal (Magma, Magma)>::value, "");

            math_check_different_namespace::check_operator (
                equal, example1, example2);

            math_check_different_namespace::check_operator (
                operation, example1, example2);

            rime::call_if (is::path_operation <Operation (Magma, Magma)>(),
                check_path_operation(), rime::nothing,
                example1, example2, operation);

            rime::call_if (is_annihilator <Operation> (example1),
                check_annihilator_with(), rime::nothing,
                example1, operation, example2);

            // If the operation is an order operation, this is checked already.
            // The problem is that a strict weak ordering allows for elements to
            // essentially compare equal even if they are not the same.
            // That break commutativity, technically, but we're not going to
            // complain about it.
            rime::call_if (rime::and_ (is::commutative <Operation, Magma>(),
                !has <callable::order <Operation> (Magma, Magma)>(),
                !strange_behaviour <Operation, Magma, Magma>()),
                check_commutative(), rime::nothing,
                example1, example2, operation);

            rime::call_if (has <callable::reverse <Operation> (Magma)>(),
                check_reverse(), rime::nothing,
                operation, example1, example2);

            // Check inverse operation in either direction.
            // This makes sure that check_inverse_operation also checks whether
            // operator/ is correctly defined.
            {
                typedef callable::inverse_operation <either> (
                    * inverse_operation) (Operation);
                rime::call_if (has <inverse_operation (Magma, Magma)>(),
                    check_inverse_operation <either>(), rime::nothing,
                    example1, example2, operation);
            }

            // Left inverse operation.
            {
                typedef callable::inverse_operation <left> (
                    * inverse_operation) (Operation);
                rime::call_if (has <inverse_operation (Magma, Magma)>(),
                    check_inverse_operation <left>(), rime::nothing,
                    example1, example2, operation);
            }

            // Right inverse operation.
            {
                typedef callable::inverse_operation <right> (
                    * inverse_operation) (Operation);
                rime::call_if (has <inverse_operation (Magma, Magma)>(),
                    check_inverse_operation <right>(), rime::nothing,
                    example1, example2, operation);
            }

            rime::call_if (rime::and_ (
                has <callable::compare (Magma, Magma)>(),
                    has <callable::equal (Magma, Magma)>()),
                check_equal_and_compare(), rime::nothing,
                example1, example2);

            static_assert (has <callable::equal (
                decltype (example1), decltype (example2))>::value, "");

            // math::approximately_equal, if it exists; otherwise, just equal.
            auto const & approximately_equal = rime::if_ (
                has <callable::approximately_equal (Magma, Magma)>(),
                math::approximately_equal, math::equal);

            // Check inequality of example1 and example2.
            if (equal (example1, example2)) {
                BOOST_CHECK (equal (example2, example1));
                BOOST_CHECK (example1 == example2);

                BOOST_CHECK (!not_equal (example1, example2));
                BOOST_CHECK (!not_equal (example2, example1));

                BOOST_CHECK (approximately_equal (example1, example2));
                BOOST_CHECK (approximately_equal (example2, example1));
            } else {
                BOOST_CHECK (!(example1 == example2));
                BOOST_CHECK (example1 != example2);
                BOOST_CHECK (!equal (example1, example2));
                BOOST_CHECK (not_equal (example1, example2));

                BOOST_CHECK (!(example2 == example1));
                BOOST_CHECK (example2 != example1);
                BOOST_CHECK (!equal (example2, example1));
                BOOST_CHECK (not_equal (example2, example1));
            }

            // Check with each combination of three examples.
            range::for_each (bind (ref (*this),
                    ref (operation), ref (example1), ref (example2), _1),
                examples);
        }

        template <class Example1, class Example2, class Example3>
            void operator() (Operation operation,
                Example1 const & example1, Example2 const & example2,
                Example3 const & example3) const
        {
            check_equal_transitive() (example1, example2, example3);

            rime::call_if (is::associative <Operation, Magma>(),
                check_associative(), rime::nothing,
                example1, example2, example3, operation);

            rime::call_if (
                has <callable::compare (Magma, Magma)>(),
                check_compare(), rime::nothing,
                operation, example1, example2, example3);
            rime::call_if (
                has <callable::order <Operation> (Magma, Magma)>(),
                check_order(), rime::nothing,
                operation, example1, example2, example3);
        }
    };

    template <class Magma, class Operation1, class Operation2, class Examples>
        struct check_magma_two_operations_implementation
    {
        typedef typename magma_tag <Magma>::type tag;

        // Zero examples: check basic properties.
        void operator() (Operation1 operation1, Operation2 operation2,
            Examples const & examples) const
        {
            // Check with each example.
            range::for_each (bind (ref (*this),
                    ref (operation1), ref (operation2), ref (examples), _1),
                examples);
        }

        // One example: check is_member, operation, and identity.
        template <class Example1>
            void operator() (Operation1 operation1, Operation2 operation2,
                Examples const & examples, Example1 const & example1) const
        {
            // Check with each combination of two examples.
            range::for_each (bind (ref (*this),
                    ref (operation1), ref (operation2), ref (examples),
                    ref (example1), _1),
                examples);
        }

        template <class Example1, class Example2>
            void operator() (Operation1 operation1, Operation2 operation2,
                Examples const & examples,
                Example1 const & example1, Example2 const & example2) const
        {
            // Check with each combination of three examples.
            range::for_each (bind (ref (*this),
                    ref (operation1), ref (operation2),
                    ref (example1), ref (example2), _1),
                examples);
        }

        template <class Example1, class Example2, class Example3>
            void operator() (Operation1 operation1, Operation2 operation2,
                Example1 const & example1, Example2 const & example2,
                Example3 const & example3) const
        {
            // a*(b+c) == a*b + a*c.
            rime::call_if (
                is::distributive <left, Operation1, Operation2, Magma>(),
                check_left_distributive(), rime::nothing,
                operation1, operation2, example1, example2, example3);

            // (a+b)*c == a*c + b*c.
            rime::call_if (
                is::distributive <right, Operation1, Operation2, Magma>(),
                check_right_distributive(), rime::nothing,
                operation1, operation2, example1, example2, example3);
        }
    };

    template <class Magma, class Direction,
        class Multiplication, class Addition, class Examples>
    struct check_semiring_implementation
    {
        // Zero examples: check basic properties.
        void operator() (Multiplication multiplication, Addition addition,
            Examples const & examples) const
        {
            static_assert (math::is::semiring <
                Direction, Multiplication, Addition, Magma>::value,
                "This must be declared as a semiring. "
                "(Add this to math::property::.)");

            // Commutative monoid under addition.
            static_assert (is::monoid <Addition, Magma>::value,
                "A semiring must be a monoid under addition.");
            static_assert (is::commutative <Addition, Magma>::value,
                "For a semiring, addition must be commutative.");

            // Monoid under multiplication.
            static_assert (is::monoid <Multiplication, Magma>::value,
                "For a semiring, must be a monoid under multiplication.");

            // Multiplication distributes over addition in Direction.
            static_assert (is::distributive <Direction,
                Multiplication, Addition, Magma>::value,
                "For a semiring, multiplication must distribute over addition."
                );

            // Check with each example.
            range::for_each (
                bind (ref (*this), ref (multiplication), ref (addition), _1),
                examples);
        }

        // One example: check that the additive identity is the multiplicative
        // annihilator.
        template <class Example1>
            void operator() (Multiplication multiplication, Addition addition,
                Example1 const & example1) const
        {
            check_annihilator_with() (
                identity <Magma, Addition>(), multiplication, example1);
        }
    };

} // namespace check_detail

/**
Check that equal returns \c false for any two elements in example, except when
they occupy the same memory address, in which case it must return \c true.

All examples must be members that compare unequal.
*/
template <class Examples> void check_equal_on (Examples const & examples) {
    check_detail::check_equal_on <Examples> implementation;
    implementation (examples);
}

/**
Test the implementation of a magma for internal consistency.
Identity and not-a-member are acquired by this function where available.

This function uses properties to determine what to test for.
Before calling this, check whether the following are defined correctly:

- equal.
  Unequal elements must compare unequal (e.g. use check_equal_on()).
  Equal elements must compare equal.
  However, equality is checked for transitivity, so not every pair of elements
  needs to be checked manually.
- has <callable::non_member (Magma)>.
- has <callable::is_member (Magma)>.
- has <callable::times/plus/... ( <Magma, Magma)>.
- has <callable::order (Operation, Magma)>.
- is::approximate <times/plus/... (Magma, Magma)>.
- has <callable::identity (Magma, Operation)>.
- has <callable::annihilator (Magma, Operation)>.
- has <math::callable::invert <left/right/either, Operation> (Magma)>.
- has <math::callable::reverse \<Operation> (Magma)>.
- is::commutative <Magma, Operation>.
- is::associative <Magma, Operation>
- is::idempotent <Magma, Operation>.
- is::path_operation <Magma, Operation>.

\tparam Magma One of the magma types to be tested.
\param operation The operation of the magma, e.g. math::times or math::plus.
\param examples
    The examples, as a range.
*/
template <class Magma, class Operation, class Examples>
    void check_magma (Operation operation, Examples const & examples)
{
    check_detail::check_magma_implementation <Magma, Operation, Examples>
        implementation;
    implementation (operation, examples);
}

/**
Check that a type is a magma with respect to two operations.
This calls check_magma with the two operations.

Before calling this, check that the properties for the single-operation
check_magma, and the following property are defined correctly:
- is::distributive with the operations in both orders.

\tparam Magma One of the magma types to be tested.
\param operation1 One operation of the magma, e.g. math::times.
\param operation2 The other operation of the magma,e
    e.g. math::plus or math::choose.
\param examples
    The examples, as a range.
    They can have different types.
*/
template <class Magma, class Operation1, class Operation2, class Examples>
    void check_magma (Operation1 operation1, Operation2 operation2,
        Examples const & examples)
{
    check_magma <Magma> (operation1, examples);
    check_magma <Magma> (operation2, examples);

    check_detail::check_magma_two_operations_implementation <
        Magma, Operation1, Operation2, Examples> implementation1;
    implementation1 (operation1, operation2, examples);

    // Operations reversed
    check_detail::check_magma_two_operations_implementation <
        Magma, Operation2, Operation1, Examples> implementation2;
    implementation2 (operation2, operation1, examples);
}

/**
Check that a type is a semiring.
This explicitly checks is::semiring in the correct direction.
It also calls check_magma with the two operations.

\tparam Magma One of the magma types to be tested.
\tparam Direction
    The direction of the semiring.
    For a normal semiring, this should be set to math::either.
    A left semiring is indicated with math::left; a right semiring with
    math::right.
\param multiplication The multiplication operation, e.g. math::times.
\param addition The addition operation, e.g. math::plus.
\param examples
    The examples, as a range.
*/
template <class Magma, class Direction,
    class Multiplication, class Addition, class Examples>
void check_semiring (Multiplication multiplication, Addition addition,
    Examples const & examples)
{
    check_magma <Magma> (multiplication, addition, examples);

    check_detail::check_semiring_implementation <
        Magma, Direction, Multiplication, Addition, Examples> implementation;
    implementation (multiplication, addition, examples);
}

} // namespace math

#endif // MATH_CHECK_CHECK_MAGMA_HPP_INCLUDED
