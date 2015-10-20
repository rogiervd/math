/*
Copyright 2013-2015 Rogier van Dalen.

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

#ifndef MATH_CHECK_CHECK_MAGMA_HPP_INCLUDED
#define MATH_CHECK_CHECK_MAGMA_HPP_INCLUDED

#include <cassert>
#include <iosfwd>
#include <ostream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <functional>
#include <string>

#include <boost/mpl/bool.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/format.hpp>

#include "meta/any_of.hpp"
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

    /** \brief
    Base class for checklists for unit tests.

    Each of the properties (of type \a Properties) that can be tested can be
    in various states.
    It can be skipped, or not applicable, or done, or failed.
    Or it can be not tested, in which case success() will return \c false.

    If a failure occurs, call failed(), which calls a customisable function to
    report the error.

    \tparam Properties
        The enum class type that indicates properties to test.
    */
    template <class Properties> class checklist_base {
    protected:
        std::bitset <std::size_t (Properties::number)> to_do_;
        std::bitset <std::size_t (Properties::number)> not_applicable_;
        std::bitset <std::size_t (Properties::number)> done_;
        std::bitset <std::size_t (Properties::number)> failed_;

        std::function <void (std::string)> report_failed_;

    protected:
        void print_element (std::ostream & os, std::string const & prepend,
            char const * description, Properties property) const
        {
            char const * status;
            if (!to_do_ [std::size_t (property)])
                status = "skipped";
            else if (failed_ [std::size_t (property)])
                status = "FAILED";
            else if (done_ [std::size_t (property)])
                status = "success";
            else if (not_applicable_ [std::size_t (property)])
                status = "not applicable";
            else
                status = "NOT PERFORMED";

            os << boost::format ("%s* %-60s %s\n")
                % prepend % description % status;
        }

    public:
        checklist_base() : report_failed_ (rime::nothing)
        { to_do_.set(); }

        /**
        Construct with a function to report failures.
        All properties are set to "to do".
        */
        explicit checklist_base (
            std::function <void (std::string)> report_failed)
        : report_failed_ (report_failed)
        { to_do_.set(); }

        /// Set the function used to report failures.
        void set_report_failed (
            std::function <void (std::string)> report_failed)
        { report_failed_ = report_failed; }

        /// Allow the test for a property to be skipped.
        void do_not_check (Properties p)
        { to_do_.set (std::size_t (p), false); }

        /**
        Set a property as not applicable (because it is known at compile time
        not to be available).
        */
        void not_applicable (Properties p)
        { not_applicable_.set (std::size_t (p)); }

        /**
        Return whether the check must be performed, and if so, note that the
        check is done.
        */
        bool tick_off (Properties p) {
            bool result = to_do_ [std::size_t (p)];
            if (result)
                done_.set (std::size_t (p));
            return result;
        }

        /**
        Mark the test for a property as failed, and call the reporting function
        with the description of the failure.
        */
        void failed (Properties p, std::string const & description) {
            failed_.set (std::size_t (p));
            report_failed_ (description);
        }

        /** \brief
        Return \c true iff all the tests that were supposed to be run were run
        and they were successful.
        */
        bool success() const
        { return (failed_ | (to_do_ & (~done_) & (~not_applicable_))).none(); }
    };

    template <class Properties> class mark_not_applicable {
        Properties property_1_;
        Properties property_2_;
    public:
        mark_not_applicable (Properties property_1, Properties property_2)
        : property_1_ (property_1), property_2_ (property_2) {}

        template <class Checklist, class ... Arguments>
            void operator() (Checklist & checks, Arguments const & ...) const
        {
            checks.not_applicable (property_1_);
            checks.not_applicable (property_2_);
        }
    };

    /// Mark property on a checklist as not applicable.
    template <class Properties> inline
        mark_not_applicable <Properties> not_applicable (Properties property)
    { return mark_not_applicable <Properties> (property, property); }

    template <class Properties> inline
        mark_not_applicable <Properties> not_applicable (
            Properties property_1, Properties property_2)
    { return mark_not_applicable <Properties> (property_1, property_2); }

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

    inline char const * describe_operation (callable::equal)
    { return "equal"; }
    inline char const * describe_operation (callable::not_equal)
    { return "not_equal"; }
    inline char const * describe_operation (callable::approximately_equal)
    { return "approximately_equal"; }

    inline char const * describe_operation (callable::plus)
    { return "plus"; }
    inline char const * describe_operation (callable::minus<>)
    { return "minus"; }
    inline char const * describe_operation (callable::minus <left>)
    { return "left minus"; }
    inline char const * describe_operation (callable::minus <right>)
    { return "right minus"; }

    inline char const * describe_operation (callable::choose)
    { return "choose"; }

    inline char const * describe_operation (callable::times)
    { return "times"; }
    inline char const * describe_operation (callable::divide<>)
    { return "divide"; }
    inline char const * describe_operation (callable::divide <left>)
    { return "left divide"; }
    inline char const * describe_operation (callable::divide <right>)
    { return "right divide"; }

    template <class Operation> inline std::string describe_operation (Operation)
    { return std::string ("[operation ") + typeid (Operation).name() + "]"; }

    } // namespace check_detail

    enum class type_properties {
        equal, equal_transitive, approximately_equal,
        not_equal, equal_operator, not_equal_operator,
        is_member, non_member,
        compare, compare_equal, print_operator,

        number
    };

    class type_checklist : public check_detail::checklist_base <type_properties>
    {
    public:
        type_checklist() {}

        explicit type_checklist (
            std::function <void (std::string)> report_failed)
        : checklist_base <type_properties> (report_failed) {}

        void print_results (std::ostream & os, std::string prepend) const {
            os << prepend << "Properties of magma:\n";

            print_element (os, prepend, "equal",
                type_properties::equal);
            print_element (os, prepend, "'equal' transitive",
                type_properties::equal_transitive);
            print_element (os, prepend, "approximately_equal",
                type_properties::not_equal);
            print_element (os, prepend, "not_equal",
                type_properties::not_equal);
            print_element (os, prepend, "'equal' and operator==",
                type_properties::equal_operator);
            print_element (os, prepend, "'not_equal' and operator!==",
                type_properties::not_equal_operator);
            print_element (os, prepend, "is_member",
                type_properties::is_member);
            print_element (os, prepend, "non_member",
                type_properties::non_member);
            print_element (os, prepend, "compare",
                type_properties::compare);
            print_element (os, prepend, "'compare' consistent with 'equal'",
                type_properties::compare_equal);
            print_element (os, prepend, "print consistent with operator<<",
                type_properties::print_operator);
        }
    };


    enum class operation_properties {
        operator_,
        path_operation, idempotent, associative, commutative,
        identity_left, identity_right,
        is_annihilator, annihilator,
        invert_either, invert_left, invert_right,
        reverse,
        order,
        inverse_either, inverse_operator, inverse_left, inverse_right,

        number
    };

    class operation_checklist
    : public check_detail::checklist_base <operation_properties>
    {
    public:
        operation_checklist() {}

        explicit operation_checklist (
            std::function <void (std::string)> report_failed)
        : checklist_base <operation_properties> (report_failed) {}

        void print_results (std::ostream & os, std::string prepend) const {
            os << prepend << "Properties of magma with one operation:\n";

            print_element (os, prepend, "operator vs. function",
                operation_properties::operator_);
            print_element (os, prepend, "path operation",
                operation_properties::path_operation);
            print_element (os, prepend, "idempotent",
                operation_properties::idempotent);
            print_element (os, prepend, "associative",
                operation_properties::associative);
            print_element (os, prepend, "commutative",
                operation_properties::commutative);
            print_element (os, prepend, "identity() (*) a",
                operation_properties::identity_left);
            print_element (os, prepend, "a (*) identity()",
                operation_properties::identity_right);
            print_element (os, prepend, "is_annihilator()",
                operation_properties::is_annihilator);
            print_element (os, prepend, "annihilator()",
                operation_properties::annihilator);

            print_element (os, prepend, "invert<either>",
                operation_properties::invert_either);
            print_element (os, prepend, "invert<left>",
                operation_properties::invert_left);
            print_element (os, prepend, "invert<right>",
                operation_properties::invert_right);

            print_element (os, prepend, "reverse",
                operation_properties::reverse);
            print_element (os, prepend, "order",
                operation_properties::order);

            print_element (os, prepend, "inverse<either>",
                operation_properties::inverse_either);
            print_element (os, prepend, "inverse<either> and operator",
                operation_properties::inverse_operator);
            print_element (os, prepend, "inverse<left>",
                operation_properties::inverse_left);
            print_element (os, prepend, "inverse<right>",
                operation_properties::inverse_right);
        }
    };

    enum class two_operations_properties {
        distributive_left, distributive_right,
        additive_identity_multiplicative_annihilator,
        number
    };

    class two_operations_checklist
    : public check_detail::checklist_base <two_operations_properties>
    {
    public:
        two_operations_checklist() {}

        explicit two_operations_checklist (
            std::function <void (std::string)> report_failed)
        : checklist_base <two_operations_properties> (report_failed) {}

        void print_results (std::ostream & os, std::string prepend) const {
            os << prepend << "Properties of magma with two operations:\n";

            print_element (os, prepend, "left-distributive",
                two_operations_properties::distributive_left);
            print_element (os, prepend, "right-distributive",
                two_operations_properties::distributive_right);
            print_element (os, prepend,
                "additive identity is multiplicative annihilator",
                two_operations_properties
                    ::additive_identity_multiplicative_annihilator);
        }
    };

} // namespace math

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
    void check_operator (math::operation_checklist & checks,
        Operation, Magma1 const & m1, Magma2 const & m2)
    { checks.not_applicable (math::operation_properties::operator_); }

    // Operators can return a different but related type.
    // (Like with operations on char.)
    // So make sure the type is the same with decltype (result_function).
    template <class Magma1, class Magma2> void check_operator (
        math::type_checklist & checks,
        math::callable::equal equal, Magma1 const & m1, Magma2 const & m2)
    {
        if (checks.tick_off (math::type_properties::equal_operator)) {
            auto result_function = equal (m1, m2);
            decltype (result_function) result_operator = (m1 == m2);
            if (result_function != result_operator) {
                checks.failed (math::type_properties::equal_operator,
                    str (boost::format (
                    "Inconsistency between 'equal' and '==': "
                    "equal (%1%, %2%) returns %3% but == returns %4%.")
                    % math::check_detail::describe_magma (m1)
                    % math::check_detail::describe_magma (m2)
                    % result_function % result_operator));
            }
        }
    }

    /// Check that not_equal is consistent with !=.
    template <class Magma1, class Magma2> void check_operator (
        math::type_checklist & checks, math::callable::not_equal not_equal,
        Magma1 const & m1, Magma2 const & m2)
    {
        if (checks.tick_off (math::type_properties::not_equal_operator)) {
            auto result_not_function = math::not_equal (m1, m2);
            decltype (result_not_function) result_not_operator = (m1 != m2);
            if (result_not_function != result_not_operator) {
                checks.failed (
                    math::type_properties::not_equal_operator,
                    str (boost::format (
                    "Inconsistency between 'not_equal' and '!=': "
                    "not_equal (%1%, %2%) returns %3% but != returns %4%.")
                    % math::check_detail::describe_magma (m1)
                    % math::check_detail::describe_magma (m2)
                    % result_not_function % result_not_operator));
            }
        }
    }

    template <class Magma1, class Magma2> void check_operator (
        math::operation_checklist & checks,
        math::callable::times times, Magma1 const & m1, Magma2 const & m2)
    {
        if (checks.tick_off (math::operation_properties::operator_)) {
            auto result_function = times (m1, m2);
            decltype (result_function) result_operator = (m1 * m2);

            if (! (result_function == result_operator)) {
                checks.failed (math::operation_properties::operator_,
                    str (boost::format (
                    "Inconsistency between 'times' and '*': "
                    "times (%1%, %2%) returns %3% but * returns %4%.")
                    % math::check_detail::describe_magma (m1)
                    % math::check_detail::describe_magma (m2)
                    % result_function % result_operator));
            }
        }
    }

    template <class Magma1, class Magma2>
        typename boost::disable_if <math::check_detail::strange_behaviour <
            math::callable::plus, Magma1, Magma2>
        >::type check_operator (
            math::operation_checklist & checks,
            math::callable::plus plus, Magma1 const & m1, Magma2 const & m2)
    {
        if (checks.tick_off (math::operation_properties::operator_)) {
            auto result_function = plus (m1, m2);
            decltype (result_function) result_operator = (m1 + m2);

            if (! (result_function == result_operator)) {
                checks.failed (math::operation_properties::operator_,
                    str (boost::format (
                    "Inconsistency between 'plus' and '+': "
                    "plus (%1%, %2%) returns %3% but + returns %4%.")
                    % math::check_detail::describe_magma (m1)
                    % math::check_detail::describe_magma (m2)
                    % result_function % result_operator));
            }
        }
    }

    template <class Magma1, class Magma2> void check_operator (
        math::operation_checklist & checks,
        math::callable::divide<> divide, Magma1 const & m1, Magma2 const & m2)
    {
        if (checks.tick_off (math::operation_properties::inverse_operator)) {
            auto result_function = divide (m1, m2);
            decltype (result_function) result_operator = (m1 / m2);

            if (! (result_function == result_operator)) {
                checks.failed (math::operation_properties::operator_,
                    str (boost::format (
                    "Inconsistency between 'divide' and '/': "
                    "divide (%1%, %2%) returns %3% but / returns %4%.")
                    % math::check_detail::describe_magma (m1)
                    % math::check_detail::describe_magma (m2)
                    % result_function % result_operator));
            }
        }
    }

    template <class Magma1, class Magma2> void check_operator (
        math::operation_checklist & checks,
        math::callable::minus<> minus, Magma1 const & m1, Magma2 const & m2)
    {
        if (checks.tick_off (math::operation_properties::inverse_operator)) {
            auto result_function = minus (m1, m2);
            decltype (result_function) result_operator = (m1 - m2);

            if (! (result_function == result_operator)) {
                checks.failed (math::operation_properties::operator_,
                    str (boost::format (
                    "Inconsistency between 'minus' and '-': "
                    "minus (%1%, %2%) returns %3% but - returns %4%.")
                    % math::check_detail::describe_magma (m1)
                    % math::check_detail::describe_magma (m2)
                    % result_function % result_operator));
            }
        }
    }

    template <class Magma> void check_operator (
        math::type_checklist & checks,
        math::callable::print print, Magma const & magma)
    {
        if (checks.tick_off (math::type_properties::print_operator)) {
            std::ostringstream stream_function;
            print (stream_function, magma);
            std::string result_function = stream_function.str();

            std::ostringstream stream_operator;
            stream_operator << magma;
            std::string result_operator = stream_operator.str();

            if (result_function != result_operator) {
                checks.failed (math::type_properties::print_operator,
                    str (boost::format (
                    "'print' produces something different from 'operator <<': "
                    "%1% vs. %2%.")
                    % result_function % result_operator));
            }
        }
    }

} // namespace math_check_different_namespace

namespace math {

namespace check_detail {

    /**
    Check whether a value is of the same magma as a reference value.
    */
    template <class Value, class Reference> inline
        void check_same_magma (type_checklist & checks,
            Value const & value, Reference const & reference,
            bool should_be_member = true)
    {
        static_assert (is_magma <decltype (value)>::value, "");
        static_assert (std::is_same <
            typename magma_tag <decltype (value)>::type,
            typename magma_tag <Reference>::type>::value, "");
        if (checks.tick_off (type_properties::is_member)) {
            if (math::is_member (value) != should_be_member) {
                checks.failed (type_properties::is_member,
                    str (boost::format ("is_member (%1%) should return %2%")
                    % describe_magma (value) % should_be_member));
            }
        }
    }

    /**
    Check whether not_equal is consistent with equal.
    */
    struct check_not_equal {
        template <class Magma1, class Magma2> void operator() (
            math::type_checklist & checks,
            Magma1 const & m1, Magma2 const & m2)
        {
            if (checks.tick_off (math::type_properties::not_equal)) {
                auto result_function = equal (m1, m2);
                // Also check not_equal, which is automatically defined.
                auto result_not_function = math::not_equal (m1, m2);
                if (result_not_function != !result_function) {
                    checks.failed (math::type_properties::not_equal, str (
                        boost::format (
                        "Inconsistency between 'equal' and 'not_equal': "
                        "equal (%1%, %2%) returns %3% "
                        "but not_equal returns %4%.")
                        % math::check_detail::describe_magma (m1)
                        % math::check_detail::describe_magma (m2)
                        % result_function % result_not_function));
                }
            }
        }
    };

    /**
    Check whether equality is transitive.
    */
    struct check_equal_transitive {
        template <class Example1, class Example2, class Example3>
            void operator() (type_checklist & checks,
                Example1 const & example1, Example2 const & example2,
                Example3 const & example3) const
        {
            if (checks.tick_off (type_properties::equal_transitive)) {
                if (equal (example1, example2) && equal (example1, example3)) {
                    if (!equal (example2, example3)) {
                        checks.failed (type_properties::equal_transitive,
                            str (boost::format (
                                "Since %1% and %2% are equal and "
                                "so are %1% and %3%, %2% and %3% should also "
                                "compare equal")
                            % describe_magma (example1)
                            % describe_magma (example2)
                            % describe_magma (example3)));
                    }
                }
            }
        }
    };

    /**
    Check a binary predicate that implements a strict weak ordering for internal
    consistency.
    */
    template <class Checklist, class Properties, class Predicate,
        class Example1, class Example2, class Example3>
    void check_strict_weak_ordering (Checklist & checks, Properties p,
        Predicate const & predicate,
        Example1 const & example1, Example2 const & example2,
        Example3 const & example3)
    {
        if (checks.tick_off (p)) {
            bool order12 = predicate (example1, example2);
            bool order21 = predicate (example2, example1);
            bool order13 = predicate (example1, example3);
            bool order31 = predicate (example3, example1);
            bool order23 = predicate (example2, example3);
            bool order32 = predicate (example3, example2);

            auto message1 = boost::format (
                "%1% and %2% cannot be less and greater at the same time");
            if (order12 && order21)
                checks.failed (p, str (message1
                    % describe_magma (example1) % describe_magma (example2)));
            if (order13 && order31)
                checks.failed (p, str (message1
                    % describe_magma (example1) % describe_magma (example3)));
            if (order23 && order32)
                checks.failed (p, str (message1
                    % describe_magma (example2) % describe_magma (example3)));

            auto message2 = boost::format ("%1% is better than %2% and "
                "%2% is better than %3%, so %1% should be better than %3%");
            // if a < b and b < c, then a < c.
            if (order13 && order32 && !order12) {
                checks.failed (p,
                    str (message2 % describe_magma (example1)
                    % describe_magma (example3) % describe_magma (example2)));
            }
            if (order12 && order23 && !order13) {
                checks.failed (p,
                    str (message2 % describe_magma (example1)
                    % describe_magma (example2) % describe_magma (example3)));
            }
            if (order23 && order31 && !order21) {
                checks.failed (p,
                    str (message2 % describe_magma (example2)
                    % describe_magma (example3) % describe_magma (example1)));
            }
            if (order21 && order13 && !order23) {
                checks.failed (p,
                    str (message2 % describe_magma (example2)
                    % describe_magma (example1) % describe_magma (example3)));
            }
            if (order32 && order21 && !order31) {
                checks.failed (p,
                    str (message2 % describe_magma (example3)
                    % describe_magma (example2) % describe_magma (example1)));
            }
            if (order31 && order12 && !order32) {
                checks.failed (p,
                    str (message2 % describe_magma (example3)
                    % describe_magma (example1) % describe_magma (example2)));
            }
        }
    }

    /**
    Check whether "compare" is consistent with "equal".
    */
    struct check_equal_and_compare {
        template <class Example1, class Example2>
            void operator() (type_checklist & checks,
                Example1 const & example1, Example2 const & example2) const
        {
            if (checks.tick_off (type_properties::compare_equal)) {
                auto is_equal = equal (example1, example2);
                auto is_equal_according_to_compare = rime::and_ (
                    !compare (example1, example2),
                    !compare (example2, example1));

                if (is_equal != is_equal_according_to_compare) {
                    checks.failed (type_properties::compare_equal,
                        str (boost::format (
                        "%1% and %2% are unequal, so the one "
                        "should be less than the other")
                        % describe_magma (example1) % describe_magma (example2)
                        ));
                }
            }
        }
    };

    /**
    Check whether the strict weak ordering implied by "compare" is consistent.
    */
    struct check_compare {
        template <class Predicate,
            class Example1, class Example2, class Example3>
        void operator() (type_checklist & checks,
            Predicate const & predicate, Example1 const & example1,
            Example2 const & example2, Example3 const & example3) const
        {
            check_strict_weak_ordering (checks, type_properties::compare,
                predicate, example1, example2, example3);
        }
    };

    struct check_order {
        template <class Operation,
            class Example1, class Example2, class Example3>
        void operator() (operation_checklist & checks,
            Operation const & operation, Example1 const & example1,
            Example2 const & example2, Example3 const & example3) const
        {
            static auto message = boost::format (
                "According to order<%3%>, %1% is better than %2%, "
                "but %4% does not return %1%, but %3%");
            if (checks.tick_off (operation_properties::order)) {
                auto result = operation (example1, example2);
                // Reverse the order of the arguments appropriately in the error
                // message.
                if (order <Operation> (example1, example2)) {
                    if (!(result == example1)) {
                        checks.failed (operation_properties::order, str (message
                            % describe_magma (example1)
                            % describe_magma (example2)
                            % describe_magma (result)
                            % describe_operation (operation)));
                    }
                } else {
                    if (!(result == example2)) {
                        checks.failed (operation_properties::order, str (message
                            % describe_magma (example2)
                            % describe_magma (example1)
                            % describe_magma (result)
                            % describe_operation (operation)));
                    }
                }

                check_strict_weak_ordering (checks, operation_properties::order,
                    callable::order <Operation>(),
                    example1, example2, example3);
            }
        }
    };

    /**
    Check whether result1 and result2 are equal, using exact or approximate
    equality depending on the operations used to acquire the results.
    \tparam A meta range containing the operations used.
        If any operation is approximate, the equality also will be assumed
        approximate.
    */
    template <class Operations, class Result1, class Result2> inline
        std::pair <bool, std::string>
        check_equality (Result1 const & result1, Result2 const & result2)
    {
        typedef typename meta::any_of <meta::transform <
                is::approximate <boost::mpl::_1>, Operations
            >>::type check_approximate;
        static_assert ((!check_approximate::value) ||
            has <callable::approximately_equal (Result1, Result2)>::value,
            "If the computation is approximate, then approximately_equal must "
            "be implemented so the result can be checked.");
        auto const & equality = rime::if_ (check_approximate(),
            math::approximately_equal, math::equal);
        bool is_equal = equality (result1, result2);
        if (is_equal)
            return std::make_pair (true, std::string());
        else
            return std::make_pair (false,
                str (boost::format ("%2% and %3% should be %1%.")
                % (check_approximate() ? "approximately equal" : "equal")
                % describe_magma (result1) % describe_magma (result2)));
    }

    struct check_commutative {
        template <class Example1, class Example2, class Operation>
            void operator() (operation_checklist & checks,
                Example1 const & example1, Example2 const & example2,
                Operation const & operation) const
        {
            if (checks.tick_off (operation_properties::commutative)) {
                auto result1 = operation (example1, example2);
                auto result2 = operation (example2, example1);

                auto answer = check_equality <meta::vector <
                    Operation (Example1, Example2),
                    Operation (Example2, Example1)>>
                    (result1, result2);
                if (!answer.first) {
                    checks.failed (operation_properties::commutative,
                        str (boost::format ("Not commutative: %1%\n"
                            "  with %2% (%3%, %4%) and %2% (%4%, %3%).")
                        % answer.second
                        % describe_operation (operation)
                        % describe_magma (example1)
                        % describe_magma (example2)));
                }
            }
        }
    };

    struct check_associative {
        template <class Example1, class Example2, class Example3,
            class Operation>
        void operator() (operation_checklist & checks,
            Example1 const & example1, Example2 const & example2,
            Example3 const & example3, Operation const & operation) const
        {
            auto result1 = operation (operation (example1, example2), example3);
            auto result2 = operation (example1, operation (example2, example3));

            if (checks.tick_off (operation_properties::associative)) {
                auto answer = check_equality <meta::vector <
                    Operation (Operation (Example1, Example2), Example3),
                    Operation (Example1, Operation (Example2, Example3))>>
                    (result1, result2);
                if (!answer.first) {
                    checks.failed (operation_properties::associative,
                        str (boost::format ("Not associative: %1%\n"
                            "  with %2% (%2% (%3%, %4%), %5%).")
                        % answer.second
                        % describe_operation (operation)
                        % describe_magma (example1)
                        % describe_magma (example2)
                        % describe_magma (example3)));
                }
            }
        }
    };

    /* check_inverse_operation: check that the inverse operation actually is. */
    template <class Direction> struct check_inverse_operation_safe {
        template <class Example, class Invertee, class Operation>
            void operator() (
                operation_checklist & checks, operation_properties p,
                Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            if (checks.tick_off (p)) {
                /*
                If a test fails with essentially a division-by-zero error after
                this point, check that you have is_annihilator <Magma> defined.
                The inverse of an operation on an annihilator is undefined, so
                check_magma automatically skips invertees that are equal to the
                annihilator.
                */
                auto inverse = inverse_operation <Direction> (operation);
                auto left_operand = rime::if_ (
                    std::is_same <Direction, left>(), invertee, example);
                auto right_operand = rime::if_ (
                    std::is_same <Direction, left>(), example, invertee);
                auto undone =
                    inverse (operation (left_operand, right_operand), invertee);

                auto result = check_equality <meta::vector <
                    decltype (inverse) (Operation (decltype (example),
                        decltype (invertee)), Invertee)>>
                    (undone, example);
                if (!result.first)
                {
                    checks.failed (p, str (boost::format (
                            "Inverse operation not an inverse: %1%\n"
                            "  with %2% and %3% on %4%, %5%.")
                        % result.second
                        % describe_operation (operation)
                        % describe_operation (inverse)
                        % describe_magma (example)
                        % describe_magma (invertee)));
                }
            }
        }
    };

    struct check_inverse_operation_operator {
        template <class Example, class Invertee, class Operation>
            void operator() (operation_checklist & checks,
                Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            auto inverse = inverse_operation <either> (operation);
            math_check_different_namespace::check_operator (checks,
                inverse, example, invertee);
        }
    };

    template <class Direction> struct check_inverse_operation_throw {
        template <class Example, class Invertee, class Operation>
            void operator() (
                operation_checklist & checks, operation_properties p,
                Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            if (checks.tick_off (p)) {
                // Check for "operation_undefined", not
                // "inverse_by_annihilator", because "example" may be an
                // annihilator too.
                try {
                    check_inverse_operation_safe <Direction>() (
                        checks, p, example, invertee, operation);
                } catch (operation_undefined &) {
                    // Success.
                    return;
                }
                catch (...) {}
                checks.failed (p, str (boost::format (
                        "Inverse of %1% should throw with annihilator %2%.")
                    % describe_operation (operation)
                    % describe_magma (invertee)));
            }
        }
    };

    /**
    If the inverse says it throws when it is called with an annihilator, check
    that it actually does.
    If not, do not do anything, because that would probably crash the program.
    */
    template <class Direction> struct check_inverse_operation_annihilator {
        template <class Example, class Invertee, class Operation>
            void operator() (
                operation_checklist & checks,  operation_properties p,
                Example const & example, Invertee const & invertee,
                Operation const & operation)
        {
            // If this is called, is_annihilator has returned true, so it is
            // tested, in a sense.
            checks.tick_off (operation_properties::is_annihilator);

            if (checks.tick_off (p)) {
                assert (is_annihilator <Operation> (invertee));

                auto inverse = inverse_operation <Direction> (operation);
                auto should_throw = is::throw_if_undefined <
                    decltype (inverse) (Example, Invertee)>();
                rime::call_if (should_throw,
                    check_inverse_operation_throw <Direction>(), rime::nothing,
                    checks, p, example, invertee, operation);
            }
        }
    };

    // General: call implementation.
    template <class Direction> struct check_inverse_operation {
        template <class Example, class Invertee, class Operation>
        void operator() (operation_checklist & checks,
            Example const & example, Invertee const & invertee,
            Operation const & operation) const
        {
            static std::is_same <Direction, left> direction_is_left;
            operation_properties p = direction_is_left ?
                operation_properties::inverse_left
                : operation_properties::inverse_right;
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <Direction>(),
                check_inverse_operation_annihilator <Direction>(),
                checks, p, example, invertee, operation);
        }
    };

    // Either: check for either, and for left and right.
    template <> struct check_inverse_operation <either> {
        template <class Example, class Invertee, class Operation>
        void operator() (operation_checklist & checks,
            Example const & example, Invertee const & invertee,
            Operation const & operation) const
        {
            // This is not done smartly, because maybe "is_annihilator" will
            // allow a direction to be specified too, in the future.
            // either.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <either>(),
                check_inverse_operation_annihilator <either>(),
                checks, operation_properties::inverse_either,
                example, invertee, operation);
            // left.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <left>(),
                check_inverse_operation_annihilator <left>(),
                checks, operation_properties::inverse_either,
                example, invertee, operation);
            // right.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_safe <right>(),
                check_inverse_operation_annihilator <right>(),
                checks, operation_properties::inverse_either,
                example, invertee, operation);
            // Operator.
            rime::call_if (!is_annihilator <Operation> (invertee),
                check_inverse_operation_operator(), rime::nothing,
                checks, example, invertee, operation);
        }
    };

    struct check_idempotent {
        template <class Example, class Operation>
            void operator() (operation_checklist & checks,
                Example const & example, Operation const & operation) const
        {
            if (checks.tick_off (operation_properties::idempotent)) {
                auto result = operation (example, example);
                if (result != example) {
                    checks.failed (operation_properties::idempotent, str (
                        boost::format (
                        "Operation %1% should be idempotent. "
                        "When called with %2%, instead of it, it returns %3%.")
                        % describe_operation (operation)
                        % describe_magma (example)
                        % describe_magma (result)));
                }
            }
        }
    };

    struct check_path_operation {
        template <class Example1, class Example2, class Operation>
            void operator() (operation_checklist & checks,
                Example1 const & example1, Example2 const & example2,
                Operation const & operation) const
        {
            if (checks.tick_off (operation_properties::path_operation)) {
                auto result = operation (example1, example2);
                if (result != example1 && result != example2) {
                    checks.failed (operation_properties::path_operation,
                        str (boost::format (
                        "Operation %1% should be a path operation and return "
                        "either the one argument or the other. "
                        "When called with %2% and %3%, it instead returns %4%.")
                        % describe_operation (operation)
                        % describe_magma (example1)
                        % describe_magma (example2)
                        % describe_magma (result)));
                }
            }
        }
    };

    class check_identity {
        template <class Identity, class Operation, class Example>
            void check_with (operation_checklist & checks,
                type_checklist & type_checks,
                Identity const & identity,
                Operation const & operation, Example const & example) const
        {
            check_same_magma (type_checks, identity, example);

            // Apply identity from the left.
            if (checks.tick_off (operation_properties::identity_left)) {
                auto result = operation (identity, example);
                check_same_magma (type_checks, result, example);
                auto correct =
                    check_equality <meta::vector <>> (result, example);
                if (!correct.first) {
                    checks.failed (operation_properties::identity_left, str (
                        boost::format ("Identity element applied on the left "
                            "changes the operand from the left: %1%\n"
                            "  %2% (%3%, %4%) returns %5%.")
                        % correct.second % describe_operation (operation)
                        % describe_magma (identity) % describe_magma (example)
                        % describe_magma (result)));
                }
            }

            // Apply identity from the right.
            if (checks.tick_off (operation_properties::identity_right)) {
                auto result = operation (example, identity);
                check_same_magma (type_checks, result, example);
                auto correct =
                    check_equality <meta::vector <>> (result, example);
                if (!correct.first) {
                    checks.failed (operation_properties::identity_right, str (
                        boost::format ("Identity element applied on the right "
                            "changes the operand from the right: %1%\n"
                            "  %2% (%3%, %4%) returns %5%.")
                        % correct.second % describe_operation (operation)
                        % describe_magma (example) % describe_magma (identity)
                        % describe_magma (result)));
                }
            }
        }

    public:
        template <class Operation, class Example>
            void operator() (operation_checklist & checks,
                // type_checks is the second parameter!
                // This is useful to match parameter order with
                // mark_not_applicable.
                type_checklist & type_checks,
                Operation const & operation, Example const & example) const
        {
            check_with (checks, type_checks,
                math::identity <Example, Operation>(),
                operation, example);
        }

        // Also check with one() and zero() for times and
        template <class Example> void operator() (
            operation_checklist & checks, type_checklist & type_checks,
            callable::times const & operation, Example const & example) const
        {
            check_with (checks, type_checks,
                math::identity <Example, callable::times>(),
                operation, example);
            check_with (checks, type_checks,
                math::one <Example>(), operation, example);
        }

        template <class Example> void operator() (
            operation_checklist & checks, type_checklist & type_checks,
            callable::plus const & operation, Example const & example) const
        {
            check_with (checks, type_checks,
                math::identity <Example, callable::plus>(),
                operation, example);
            check_with (checks, type_checks,
                math::zero <Example>(), operation, example);
        }
    };

    /**
    Check that is_annihilator returns false for the object.
    This is called when <c>operation (example, other)</c> returns something
    not equal to \c example.
    */
    struct check_not_is_annihilator_with {
        template <class Example, class Operation>
            void operator() (operation_checklist & checks,
                Example const & example,
                Operation const & operation)
        {
            if (checks.tick_off (operation_properties::is_annihilator)) {
                bool annihilator_is_annihilator
                    = is_annihilator (operation, example);
                assert (is_annihilator <Operation> (example)
                    == annihilator_is_annihilator);
                if (annihilator_is_annihilator) {
                    checks.failed (operation_properties::is_annihilator,
                        str (boost::format (
                            "is_annihilator <%1%> (%2%) returns true whereas "
                            "it does not annihilate all values.")
                        % describe_operation (operation)
                        % describe_magma (example)));
                }
            }
        }
    };

    /**
    Check that a given object is an annihilator.
    is_annihilator() should return \c true, and it should annihilate the example
    under the operation.
    */
    struct check_annihilator_with {
        /**
        \return
            \c true iff the tests passed.
            \c false iff the tests failed; but \a checks will have been notified
            already.
        */
        template <class Annihilator, class Operation, class Example>
            bool check (operation_checklist & checks,
                type_checklist & type_checks,
                Annihilator const & annihilator,
                Operation const & operation, Example const & example)
        {
            bool success = true;
            check_same_magma (type_checks, annihilator, example);

            if (checks.tick_off (operation_properties::is_annihilator)) {
                bool annihilator_is_annihilator
                    = is_annihilator (operation, annihilator);
                assert (is_annihilator <Operation> (annihilator)
                    == annihilator_is_annihilator);
                if (!annihilator_is_annihilator) {
                    success = false;
                    checks.failed (operation_properties::is_annihilator,
                        str (boost::format (
                            "is_annihilator <%1%> (%2%) returns false.")
                        % describe_operation (operation)
                        % describe_magma (annihilator)));
                }
            }

            auto result_left = operation (annihilator, example);
            check_same_magma (type_checks, result_left, example);

            auto correct_left = check_equality <meta::vector <>> (
                result_left, annihilator);
            if (!correct_left.first) {
                success = false;
                checks.failed (operation_properties::annihilator,
                    str (boost::format (
                        "Annihilator does not annihilate the operand "
                        "from the left: %1%\n"
                        "  with %2% (%3%, %4%).")
                    % correct_left.second % describe_operation (operation)
                    % describe_magma (annihilator)
                    % describe_magma (example)));
            }

            auto result_right = operation (example, annihilator);
            check_same_magma (type_checks, result_right, example);

            auto correct_right = check_equality <meta::vector <>> (
                result_right, annihilator);
            if (!correct_right.first) {
                success = false;
                checks.failed (operation_properties::annihilator,
                    str (boost::format (
                        "Annihilator does not annihilate the operand "
                        "from the right: %1%\n"
                        "  with %2% (%3%, %4%).")
                    % correct_right.second % describe_operation (operation)
                    % describe_magma (example)
                    % describe_magma (annihilator)));
            }
            return success;
        }

        /// Check the annihilator and return \c void.
        template <class Annihilator, class Operation, class Example>
            void operator() (operation_checklist & checks,
                type_checklist & type_checks,
                Annihilator const & annihilator,
                Operation const & operation, Example const & example)
        { check (checks, type_checks, annihilator, operation, example); }
    };

    /**
    Check that math::annihilator returns something that is an annihilator.
    */
    struct check_annihilator {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            // type_checks is the second parameter!
            // This is useful to match parameter order with
            // mark_not_applicable.
            type_checklist & type_checks,
            Operation const & operation, Example const & example) const
        {
            if (checks.tick_off (operation_properties::annihilator)) {
                auto annihilator = math::annihilator <Example, Operation>();
                check_annihilator_with() (checks, type_checks,
                    annihilator, operation, example);
            }
        }
    };

    struct check_non_member {
        template <class Example> void operator() (
            type_checklist & checks, Example const & example) const
        {
            if (checks.tick_off (type_properties::non_member)) {
                auto result = non_member <Example>();
                check_same_magma (checks, result, example, false);
                if (is_member (result)) {
                    checks.failed (type_properties::non_member,
                        str (boost::format ("non_member() returned %1% "
                            "which is_member says is a member")
                            % describe_magma (result)));
                }
            }
        }
    };

    template <class Direction> struct check_invert_when_safe;
    template <class Direction> struct check_invert;

    template <> struct check_invert_when_safe <left> {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            if (checks.tick_off (operation_properties::invert_left)) {
                auto inverse = math::invert <left, Operation> (example);
                auto undone = operation (inverse, example);

                auto correct = check_equality <meta::vector <Operation (
                    math::callable::invert <left, Operation> (Example),
                    Example)>>
                    (undone, math::identity <Example, Operation>());
                if (!correct.first) {
                    checks.failed (operation_properties::invert_left,
                        str (boost::format (
                        "The 'invert' operation does not "
                        "return a left inverse: %1%.\n"
                        "  invert <left> (%2%, %3%) returns %4%;\n"
                        "  %2% (%4%, %3%) returns %5% instead of the identity.")
                        % correct.second
                        % describe_operation (operation)
                        % describe_magma (example)
                        % describe_magma (inverse)
                        % describe_magma (undone)));
                }
            }
        }
    };

    template <> struct check_invert_when_safe <right> {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            if (checks.tick_off (operation_properties::invert_right)) {
                auto inverse = math::invert <right, Operation> (example);
                auto undone = operation (example, inverse);

                auto correct = check_equality <meta::vector <Operation (
                    Example, math::callable::invert <right, Operation> (
                        Example))>>
                    (undone, math::identity <Example, Operation>());
                if (!correct.first) {
                    checks.failed (operation_properties::invert_right,
                        str (boost::format (
                        "The 'invert' operation does not "
                        "return a right inverse: %1%.\n"
                        "  invert <right> (%2%, %3%) returns %4%;\n"
                        "  %2% (%3%, %4%) returns %5% instead of the identity.")
                        % correct.second
                        % describe_operation (operation)
                        % describe_magma (example)
                        % describe_magma (inverse)
                        % describe_magma (undone)));
                }
            }
        }
    };

    template <> struct check_invert_when_safe <either> {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            if (checks.tick_off (operation_properties::invert_either)) {
                // Check that invert <either> returns something that works from
                // either side.
                auto inverse = math::invert <either, Operation> (example);

                auto undone_left = operation (inverse, example);

                auto correct = check_equality <meta::vector <Operation (
                    math::callable::invert <left, Operation> (Example),
                    Example)>>
                    (undone_left, math::identity <Example, Operation>());
                if (!correct.first) {
                    checks.failed (operation_properties::invert_either, str (
                        boost::format (
                        "The 'invert' operation does not "
                        "return a left inverse: %1%.\n"
                        "  invert (%2%, %3%) returns %4%;\n"
                        "  %2% (%4%, %3%) returns %5% instead of the identity.")
                        % correct.second
                        % describe_operation (operation)
                        % describe_magma (example)
                        % describe_magma (inverse)
                        % describe_magma (undone_left)));
                }

                auto undone_right = operation (example, inverse);

                correct = check_equality <meta::vector <Operation (
                    Example, math::callable::invert <right, Operation> (
                        Example))>>
                    (undone_right, math::identity <Example, Operation>());
                if (!correct.first) {
                    checks.failed (operation_properties::invert_either, str (
                        boost::format (
                        "The 'invert' operation does not "
                        "return a right inverse: %1%.\n"
                        "  invert (%2%, %3%) returns %4%;\n"
                        "  %2% (%3%, %4%) returns %5% instead of the identity.")
                        % correct.second
                        % describe_operation (operation)
                        % describe_magma (example)
                        % describe_magma (inverse)
                        % describe_magma (undone_right)));
                }
            }

            // Check that invert <left> and invert <right> are defined.
            check_invert_when_safe <left>() (checks, operation, example);
            check_invert_when_safe <right>() (checks, operation, example);
        }
    };

    template <class Direction> struct check_invert_throw {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            operation_properties property =
                std::is_same <Direction, either>::value ?
                    operation_properties::invert_either
                    : (std::is_same <Direction, left>::value ?
                        operation_properties::invert_left :
                        operation_properties::invert_right);
            if (checks.tick_off (property)) {
                // Check for "operation_undefined", not
                // "inverse_by_annihilator", because "example" may be an
                // annihilator too.
                try {
                    check_invert_when_safe <Direction>() (
                        checks, operation, example);
                    // If no exception has been thrown:
                    checks.failed (property, str (boost::format (
                        "%1% (%2%) should have thrown but does not.")
                        % describe_operation (callable::invert <Direction>())
                        % describe_magma (example)));
                } catch (operation_undefined &) {}
            }
        }
    };

    /**
    If "invert" says it throws when it is called with an annihilator, check that
    it actually does.
    If not, do not do anything, because that would probably crash the program.
    */
    template <class Direction> struct check_invert_annihilator {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            assert (is_annihilator <Operation> (example));

            auto should_throw = is::throw_if_undefined <
                callable::invert <Direction, Operation> (Example)>();
            rime::call_if (should_throw,
                check_invert_throw <Direction>(),
                rime::nothing,
                operation, example);
        }
    };

    template <class Direction> struct check_invert {
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            rime::call_if (!is_annihilator <Operation> (example),
                check_invert_when_safe <Direction>(),
                check_invert_annihilator <Direction>(),
                checks, operation, example);
        }
    };

    struct check_reverse {
        // Check that reverse (reverse (a)) returns the same magma.
        template <class Operation, class Example> void operator() (
            operation_checklist & checks,
            Operation const & operation, Example const & example) const
        {
            if (checks.tick_off (operation_properties::reverse)) {
                auto result = reverse (operation, reverse (operation, example));
                static_assert (std::is_same <
                    typename magma_tag <decltype (result)>::type,
                    typename magma_tag <Example>::type>::value, "");

                typedef callable::reverse <Operation> reverse_type;
                auto correct = check_equality <
                    meta::vector <reverse_type (reverse_type (Example))>>
                    (result, example);
                if (!correct.first) {
                    checks.failed (operation_properties::reverse, str (
                        boost::format (
                        "Applying 'reverse' twice does not return "
                        "the original value: %1%\n"
                        "  reverse <%2%> (%3%).")
                        % correct.second % describe_operation (operation)
                        % describe_magma (example)));
                }
            }
        }

        // Check that reverse (operation (reverse (b), reverse (a))) is of the
        // same magma and has the same value as operation (a, b).
        template <class Operation, class Example1, class Example2>
            void operator() (operation_checklist & checks,
                Operation const & operation,
                Example1 const & example1, Example2 const & example2) const
        {
            if (checks.tick_off (operation_properties::reverse)) {
                typedef callable::reverse <Operation> reverse_type;
                reverse_type reverse;
                auto result = reverse (operation (
                    reverse (example2), reverse (example1)));

                static_assert (std::is_same <
                    typename magma_tag <decltype (result)>::type,
                    typename magma_tag <Example1>::type>::value, "");

                auto correct = check_equality <meta::vector <
                    reverse_type (Operation (
                        reverse_type (Example2), reverse_type (Example1)))>>
                    (result, operation (example1, example2));
                if (!correct.first) {
                    checks.failed (operation_properties::reverse,
                        str (boost::format (
                        "Applying the operation with reversing yields a "
                        "different result from applying it without: %1%\n"
                        "  reverse (%2% (reverse (%4%), reverse (%3%))).")
                        % correct.second % describe_operation (operation)
                        % describe_magma (example1)
                        % describe_magma (example2)));
                }
            }
        }
    };

    struct check_print {
        // Check that "print" is consistent with operator<<.
        template <class Magma> void operator() (
            type_checklist & checks, Magma const & magma) const
        {
            math_check_different_namespace::check_operator (checks,
                print, magma);
        }
    };

    struct check_left_distributive {
        template <class Multiplication, class Addition,
            class Example1, class Example2, class Example3>
        void operator() (two_operations_checklist & checks,
            Multiplication const & multiplication,
            Addition const & addition, Example1 const & example1,
            Example2 const & example2, Example3 const & example3)
        {
            if (checks.tick_off (two_operations_properties::distributive_left))
            {
                // a*(b+c) == a*b + a*c.
                auto result1 =
                    multiplication (example1, addition (example2, example3));
                auto result2 =
                    addition (multiplication (example1, example2),
                        multiplication (example1, example3));

                auto correct = check_equality <meta::vector <
                    Multiplication (Example1, Addition (Example2, Example3)),
                    Addition (Multiplication (Example1, Example2),
                        Multiplication (Example1, Example3))>>
                    (result1, result2);

                if (!correct.first) {
                    checks.failed (two_operations_properties::distributive_left,
                        str (boost::format ("Not left distributive: %1%\n"
                            "  %2% (%4%, %3% (%5%, %6%)).")
                        % describe_operation (multiplication)
                        % describe_operation (addition)
                        % describe_magma (example1) % describe_magma (example2)
                        % describe_magma (example3)));
                }
            }
        }
    };

    struct check_right_distributive {
        template <class Multiplication, class Addition,
            class Example1, class Example2, class Example3>
        void operator() (two_operations_checklist & checks,
            Multiplication const & multiplication,
            Addition const & addition, Example1 const & example1,
            Example2 const & example2, Example3 const & example3)
        {
            if (checks.tick_off (two_operations_properties::distributive_right))
            {
                // (a+b)*c == a*c + b*c.
                auto result1 =
                    multiplication (addition (example1, example2), example3);
                auto result2 =
                    addition (multiplication (example1, example3),
                        multiplication (example2, example3));

                auto correct = check_equality <meta::vector <
                    Multiplication (Addition (Example1, Example2), Example3),
                    Addition (Multiplication (Example1, Example3),
                        Multiplication (Example2, Example3))>>
                    (result1, result2);

                if (!correct.first) {
                    checks.failed (
                        two_operations_properties::distributive_right,
                        str (boost::format ("Not right distributive: %1%\n"
                            "  %2% (%3% (%4%, %5%), %6%)).")
                        % describe_operation (multiplication)
                        % describe_operation (addition)
                        % describe_magma (example1) % describe_magma (example2)
                        % describe_magma (example3)));
                }
            }
        }
    };

    struct check_additive_identity_multiplicative_annihilator {
        template <class Multiplication, class Addition, class Example1>
            void operator() (
                // two_operations_checklist goes first.
                // This way, the call is compatible with not_applicable().
                two_operations_checklist & two_operations_checks,
                type_checklist & type_checks,
                operation_checklist & operation_checks,
                Multiplication multiplication, Addition addition,
                Example1 const & example1) const
        {
            if (two_operations_checks.tick_off (two_operations_properties
                ::additive_identity_multiplicative_annihilator))
            {
                auto success = check_annihilator_with().check (
                    operation_checks, type_checks,
                    identity <Example1, Addition>(), multiplication, example1);
                if (!success)
                    two_operations_checks.failed (two_operations_properties
                        ::additive_identity_multiplicative_annihilator,
                        "The additive identity is not a multiplicative "
                        "annihilator.");
            }
        }
    };

    using std::bind;
    using std::ref;
    using std::placeholders::_1;

    template <class Examples> struct check_equal_on {
        void operator() (type_checklist & checks, Examples const & examples)
            const
        {
            range::for_each (examples, bind (ref (*this),
                ref (checks), ref (examples), _1));
        }

        template <class Example1>
            void operator() (type_checklist & checks,
                Examples const & examples, Example1 const & example1) const
        {
            range::for_each (examples, bind (ref (*this),
                ref (checks), ref (example1), _1));
        }

        template <class Example1, class Example2>
            void operator() (type_checklist & checks,
                Example1 const & example1, Example2 const & example2) const
        {
            // equal should return true iff the objects are the same.
            // (Because all examples passed to check_equal_on must be
            // unequal.)
            bool expected = (static_cast <void const *> (&example1)
                == static_cast <void const *> (&example2));
            auto example1_copy = example1;

            if (checks.tick_off (type_properties::equal)) {
                check_equal_returns (checks, type_properties::equal, equal,
                    example1, example2, expected);
                check_equal_returns (checks, type_properties::equal, equal,
                    example1_copy, example2, expected);
            }
            if (checks.tick_off (type_properties::approximately_equal)) {
                check_equal_returns (
                    checks, type_properties::approximately_equal,
                    approximately_equal,
                    example1, example2, expected);
                check_equal_returns (
                    checks, type_properties::approximately_equal,
                    approximately_equal,
                    example1_copy, example2, expected);
            }
        }

    private:
        template <class Equal, class Example1, class Example2>
            void check_equal_returns (
                type_checklist & checks, type_properties property, Equal equal,
                Example1 const & example1, Example2 const & example2,
                bool expected) const
        {
            bool result = equal (example1, example2);
            if (result != expected) {
                checks.failed (property, str (boost::format (
                    "%1% (%2%, %3%) returns %5% but it should return %4%.")
                    % describe_operation (equal)
                    % describe_magma (example1) % describe_magma (example2)
                    % expected % result));
            }
        }
    };

    template <class Magma, class Operation, class Examples>
        struct check_magma_implementation
    {
        typedef typename magma_tag <Magma>::type tag;

        // Zero examples: check basic properties.
        void operator() (type_checklist & type_checks,
            operation_checklist & operation_checks,
            Operation operation, Examples const & examples) const
        {
            static_assert (math::is_magma <Magma>::value, "");
            static_assert (math::is_magma <Magma &>::value, "");
            static_assert (math::is_magma <Magma const>::value, "");
            static_assert (math::is_magma <Magma const &>::value, "");

            static_assert (!std::is_same <tag, not_a_magma_tag>::value, "");

            static_assert (math::has <callable::is_member (Magma)>::value, "");

            // Check with each example.
            range::for_each (examples, bind (ref (*this),
                ref (type_checks), ref (operation_checks),
                ref (operation), ref (examples), _1));
        }

        // One example: check is_member, operation, identity, invert, reverse.
        template <class Example1>
            void operator() (type_checklist & type_checks,
                operation_checklist & operation_checks,
                Operation operation, Examples const & examples,
                Example1 const & example1) const
        {
            typedef typename magma_tag <decltype (example1)>::type example_tag;
            static_assert (std::is_same <tag, example_tag>::value, "");
            if (type_checks.tick_off (type_properties::is_member)) {
                // The examples must all be members.
                if (!is_member (example1)) {
                    type_checks.failed (type_properties::is_member,
                        str (boost::format ("is_member returns false for %1%")
                            % describe_magma (example1)));
                }
            }

            // equal and approximately_equal should be tested separately, by
            // calling check_equal_on.

            rime::call_if (is::idempotent <Operation, Magma>(),
                check_idempotent(),
                not_applicable (operation_properties::idempotent),
                operation_checks, example1, operation);

            rime::call_if (has <callable::identity <Magma, Operation>()>(),
                check_identity(),
                not_applicable (operation_properties::identity_left,
                    operation_properties::identity_right),
                operation_checks, type_checks, operation, example1);

            rime::call_if (has <callable::non_member <Magma>()>(),
                check_non_member(),
                not_applicable (type_properties::non_member),
                type_checks, example1);

            rime::call_if (has <callable::annihilator <Magma, Operation>()>(),
                check_annihilator(),
                not_applicable (operation_properties::annihilator),
                operation_checks, type_checks, operation, example1);

            /* Check invert. */
            // Left.
            rime::call_if (
                has <callable::invert <math::left, Operation> (Magma)>(),
                check_invert <math::left>(),
                not_applicable (operation_properties::invert_left),
                operation_checks, operation, example1);
            // Right.
            rime::call_if (
                has <callable::invert <math::right, Operation> (Magma)>(),
                check_invert <math::right>(),
                not_applicable (operation_properties::invert_right),
                operation_checks, operation, example1);
            // Either.
            rime::call_if (
                has <callable::invert <math::either, Operation> (Magma)>(),
                check_invert <math::either>(),
                not_applicable (operation_properties::invert_either),
                operation_checks, operation, example1);

            // Check reverse.
            rime::call_if (has <callable::reverse <Operation> (Magma)>(),
                check_reverse(), not_applicable (operation_properties::reverse),
                operation_checks, operation, example1);

            // Check that print is consistent with operator <<.
            // Not really a requirement?
            rime::call_if (has <callable::print (std::ostream &, Magma)>(),
                check_print(), not_applicable (type_properties::print_operator),
                type_checks, example1);

            // Check with each combination of two examples.
            // _1 will be example2.
            range::for_each (examples, bind (ref (*this),
                ref (type_checks), ref (operation_checks),
                ref (operation), ref (examples), ref (example1), _1));
        }

        template <class Example1, class Example2>
            void operator() (type_checklist & type_checks,
                operation_checklist & operation_checks,
                Operation operation, Examples const & examples,
                Example1 const & example1, Example2 const & example2) const
        {
            auto result = operation (example1, example2);
            check_same_magma (type_checks, result, example1);
            if (result != example1)
                check_not_is_annihilator_with() (operation_checks,
                    example1, operation);
            if (result != example2)
                check_not_is_annihilator_with() (operation_checks,
                    example2, operation);

            static_assert (has <callable::equal (Magma, Magma)>::value, "");

            math_check_different_namespace::check_operator (
                type_checks, equal, example1, example2);

            check_not_equal() (type_checks, example1, example2);

            math_check_different_namespace::check_operator (
                type_checks, not_equal, example1, example2);

            math_check_different_namespace::check_operator (
                operation_checks, operation, example1, example2);

            rime::call_if (is::path_operation <Operation (Magma, Magma)>(),
                check_path_operation(),
                not_applicable (operation_properties::path_operation),
                operation_checks, example1, example2, operation);

            rime::call_if (is_annihilator <Operation> (example1),
                check_annihilator_with(),
                rime::nothing,
                operation_checks, type_checks, example1, operation, example2);

            // If the operation is an order operation, this is checked already.
            // The problem is that a strict weak ordering allows for elements to
            // essentially compare equal even if they are not the same.
            // That break commutativity, technically, but we're not going to
            // complain about it.
            rime::call_if (rime::and_ (is::commutative <Operation, Magma>(),
                !has <callable::order <Operation> (Magma, Magma)>(),
                !strange_behaviour <Operation, Magma, Magma>()),
                check_commutative(),
                not_applicable (operation_properties::commutative),
                operation_checks, example1, example2, operation);

            rime::call_if (has <callable::reverse <Operation> (Magma)>(),
                check_reverse(), not_applicable (operation_properties::reverse),
                operation_checks, operation, example1, example2);

            // Check inverse operation in either direction.
            {
                typedef callable::inverse_operation <either> (
                    * inverse_operation) (Operation);
                rime::call_if (has <inverse_operation (Magma, Magma)>(),
                    check_inverse_operation <either>(),
                    not_applicable (operation_properties::inverse_either,
                        operation_properties::inverse_operator),
                    operation_checks, example1, example2, operation);
            }

            // Left inverse operation.
            {
                typedef callable::inverse_operation <left> (
                    * inverse_operation) (Operation);
                rime::call_if (has <inverse_operation (Magma, Magma)>(),
                    check_inverse_operation <left>(),
                    not_applicable (operation_properties::inverse_left),
                    operation_checks, example1, example2, operation);
            }

            // Right inverse operation.
            {
                typedef callable::inverse_operation <right> (
                    * inverse_operation) (Operation);
                rime::call_if (has <inverse_operation (Magma, Magma)>(),
                    check_inverse_operation <right>(),
                    not_applicable (operation_properties::inverse_right),
                    operation_checks, example1, example2, operation);
            }

            rime::call_if (rime::and_ (
                has <callable::compare (Magma, Magma)>(),
                    has <callable::equal (Magma, Magma)>()),
                check_equal_and_compare(),
                not_applicable (type_properties::compare_equal),
                type_checks, example1, example2);

            static_assert (has <callable::equal (
                decltype (example1), decltype (example2))>::value, "");

            // Check with each combination of three examples.
            range::for_each (examples, bind (ref (*this),
                ref (type_checks), ref (operation_checks),
                ref (operation), ref (example1), ref (example2), _1));
        }

        template <class Example1, class Example2, class Example3>
            void operator() (type_checklist & type_checks,
                operation_checklist & operation_checks,
                Operation operation,
                Example1 const & example1, Example2 const & example2,
                Example3 const & example3) const
        {
            check_equal_transitive() (
                type_checks, example1, example2, example3);

            rime::call_if (is::associative <Operation, Magma>(),
                check_associative(),
                not_applicable (operation_properties::associative),
                operation_checks, example1, example2, example3, operation);

            rime::call_if (
                has <callable::compare (Magma, Magma)>(),
                check_compare(), not_applicable (type_properties::compare),
                type_checks, math::compare, example1, example2, example3);

            rime::call_if (
                has <callable::order <Operation> (Magma, Magma)>(),
                check_order(), not_applicable (operation_properties::order),
                operation_checks, operation, example1, example2, example3);
        }
    };

    template <class Magma, class Operation1, class Operation2, class Examples>
        struct check_magma_two_operations_implementation
    {
        typedef typename magma_tag <Magma>::type tag;

        // Zero examples: check basic properties.
        void operator() (type_checklist & type_checks,
            operation_checklist & operation_checks,
            two_operations_checklist & two_operations_checks,
            Operation1 operation1, Operation2 operation2,
            Examples const & examples) const
        {
            // Check with each example.
            range::for_each (examples, bind (ref (*this),
                ref (type_checks), ref (operation_checks),
                ref (two_operations_checks),
                ref (operation1), ref (operation2),
                ref (examples), _1));
        }

        // One example: check is_member, operation, and identity.
        template <class Example1>
            void operator() (type_checklist & type_checks,
                operation_checklist & operation_checks,
                two_operations_checklist & two_operations_checks,
                Operation1 operation1, Operation2 operation2,
                Examples const & examples, Example1 const & example1) const
        {
            rime::call_if (
                is::semiring <left, Operation1, Operation2, Magma>() ||
                is::semiring <right, Operation1, Operation2, Magma>(),
                check_additive_identity_multiplicative_annihilator(),
                not_applicable (two_operations_properties
                    ::additive_identity_multiplicative_annihilator),
                two_operations_checks, type_checks, operation_checks,
                operation1, operation2, example1);

            // Check with each combination of two examples.
            range::for_each (examples, bind (ref (*this),
                ref (two_operations_checks),
                ref (operation1), ref (operation2),
                ref (examples), ref (example1), _1));
        }

        template <class Example1, class Example2>
            void operator() (two_operations_checklist & checks,
                Operation1 operation1, Operation2 operation2,
                Examples const & examples,
                Example1 const & example1, Example2 const & example2) const
        {
            // Check with each combination of three examples.
            range::for_each (examples, bind (ref (*this), ref (checks),
                ref (operation1), ref (operation2),
                ref (example1), ref (example2), _1));
        }

        template <class Example1, class Example2, class Example3>
            void operator() (two_operations_checklist & checks,
                Operation1 operation1, Operation2 operation2,
                Example1 const & example1, Example2 const & example2,
                Example3 const & example3) const
        {
            // a*(b+c) == a*b + a*c.
            rime::call_if (
                is::distributive <left, Operation1, Operation2, Magma>(),
                check_left_distributive(),
                not_applicable (two_operations_properties::distributive_left),
                checks, operation1, operation2, example1, example2, example3);

            // (a+b)*c == a*c + b*c.
            rime::call_if (
                is::distributive <right, Operation1, Operation2, Magma>(),
                check_right_distributive(),
                not_applicable (two_operations_properties::distributive_right),
                checks, operation1, operation2, example1, example2, example3);
        }
    };

    template <class Magma, class Direction,
        class Multiplication, class Addition, class Examples>
    struct check_semiring_implementation
    {
        // Zero examples: check basic properties.
        void operator() (type_checklist & type_checks,
            operation_checklist & operation_checks,
            two_operations_checklist & two_operations_checks,
            Multiplication multiplication, Addition addition,
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
            range::for_each (examples, bind (ref (*this),
                ref (type_checks), ref (operation_checks),
                ref (two_operations_checks),
                ref (multiplication), ref (addition), _1));
        }

        // One example: check that the additive identity is the multiplicative
        // annihilator.
        template <class Example1>
            void operator() (type_checklist & type_checks,
                operation_checklist & operation_checks,
                two_operations_checklist & two_operations_checks,
                Multiplication multiplication, Addition addition,
                Example1 const & example1) const
        {
            if (two_operations_checks.tick_off (two_operations_properties
                ::additive_identity_multiplicative_annihilator))
            {
                auto success = check_annihilator_with().check (
                    operation_checks, type_checks,
                    identity <Magma, Addition>(), multiplication, example1);
                if (!success)
                    two_operations_checks.failed (two_operations_properties
                        ::additive_identity_multiplicative_annihilator,
                        "The additive identity is not a multiplicative "
                        "annihilator.");
            }
        }
    };

} // namespace check_detail

/**
Check that equal returns \c false for any two elements in example, except when
they occupy the same memory address, in which case it must return \c true.

All examples must be members that compare unequal.
*/
template <class Examples> void check_equal_on (
    type_checklist & checks, Examples const & examples)
{
    check_detail::check_equal_on <Examples> implementation;
    implementation (checks, examples);
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
    void check_magma (Operation operation, Examples const & examples,
        type_checklist & type_checks, operation_checklist & operation_checks)
{
    check_detail::check_magma_implementation <Magma, Operation, Examples>
        implementation;
    implementation (type_checks, operation_checks, operation, examples);
}

/**
Check that a type is a magma with respect to two operations.
This calls check_magma with the two operations.

Before calling this, check that the properties for the single-operation
check_magma, and the following property are defined correctly:
- is::distributive with the operations in both orders.

\tparam Magma One of the magma types to be tested.
\param operation1 One operation of the magma, e.g. math::times.
\param operation2 The other operation of the magma,
    e.g. math::plus or math::choose.
\param examples
    The examples, as a range.
    They can have different types.
*/
template <class Magma, class Operation1, class Operation2, class Examples>
    void check_magma (Operation1 operation1, Operation2 operation2,
        Examples const & examples,
        type_checklist & type_checks,
        operation_checklist & operation1_checks,
        operation_checklist & operation2_checks,
        two_operations_checklist & two_operation1_checks,
        two_operations_checklist & two_operation2_checks)
{
    check_magma <Magma> (operation1, examples, type_checks, operation1_checks);
    check_magma <Magma> (operation2, examples, type_checks, operation2_checks);

    check_detail::check_magma_two_operations_implementation <
        Magma, Operation1, Operation2, Examples> implementation1;
    implementation1 (type_checks, operation1_checks, two_operation1_checks,
        operation1, operation2, examples);

    // Operations reversed
    check_detail::check_magma_two_operations_implementation <
        Magma, Operation2, Operation1, Examples> implementation2;
    implementation2 (type_checks, operation2_checks, two_operation2_checks,
        operation2, operation1, examples);
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
    Examples const & examples,
    type_checklist & type_checks,
    operation_checklist & operation1_checks,
    operation_checklist & operation2_checks,
    two_operations_checklist & two_operation1_checks,
    two_operations_checklist & two_operation2_checks)
{
    check_magma <Magma> (multiplication, addition, examples,
        type_checks, operation1_checks, operation2_checks,
        two_operation1_checks, two_operation2_checks);

    check_detail::check_semiring_implementation <
        Magma, Direction, Multiplication, Addition, Examples> implementation;
    implementation (type_checks, operation1_checks, two_operation1_checks,
        multiplication, addition, examples);
}

} // namespace math

#endif // MATH_CHECK_CHECK_MAGMA_HPP_INCLUDED
