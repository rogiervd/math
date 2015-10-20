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

/** \file
Wrapper for check_magma et al. that produces Boost.Test error messages.
*/

#ifndef MATH_CHECK_REPORT_CHECK_MAGMA_BOOST_TEST
#define MATH_CHECK_REPORT_CHECK_MAGMA_BOOST_TEST

#include <string>
#include <iostream>
#include <ostream>

#include <boost/test/test_tools.hpp>
// For demangling type names.
#include <boost/units/detail/utility.hpp>

#include "math/check/check_magma.hpp"
#include "math/check/check_hash.hpp"

namespace math {

void report_error_boost_test (std::string message)
{ BOOST_ERROR (message); }

template <class Type, class Operation, class UnequalExamples, class Examples>
    inline void report_check_magma (Operation const & operation,
        UnequalExamples const & unequal_examples, Examples const & examples,
        math::type_checklist & type_checks,
        math::operation_checklist & operation_checks)
{
    // Check for consistency.
    type_checks.set_report_failed (report_error_boost_test);
    operation_checks.set_report_failed (report_error_boost_test);

    math::check_equal_on (type_checks, unequal_examples);

    math::check_magma <Type> (
        operation, examples, type_checks, operation_checks);

    auto operation_name = math::check_detail::describe_operation (operation);

    std::cout << "Results of test with type "
        << boost::units::detail::demangle (typeid (Type).name()) << std::endl;
    type_checks.print_results (std::cout, "     ");
    std::cout << "- operation ("<< operation_name <<"):" << std::endl;
    operation_checks.print_results (std::cout, "     ");

    BOOST_CHECK (type_checks.success());
    BOOST_CHECK (operation_checks.success());
}

template <class Type, class Operation, class UnequalExamples, class Examples>
    inline void report_check_magma (Operation const & operation,
        UnequalExamples const & unequal_examples, Examples const & examples)
{
    math::type_checklist type_checks;
    math::operation_checklist operation_checks;
    report_check_magma <Type> (operation, unequal_examples, examples,
        type_checks, operation_checks);
}

template <class Type, class Operation1, class Operation2,
    class UnequalExamples, class Examples>
inline void report_check_magma (
    Operation1 const & operation1, Operation2 const & operation2,
    UnequalExamples const & unequal_examples, Examples const & examples,
    math::type_checklist & type_checks,
    math::operation_checklist & operation1_checks,
    math::operation_checklist & operation2_checks,
    math::two_operations_checklist & operation1_operation2_checks,
    math::two_operations_checklist & operation2_operation1_checks)
{
    // Check for consistency.
    type_checks.set_report_failed (report_error_boost_test);
    operation1_checks.set_report_failed (report_error_boost_test);
    operation2_checks.set_report_failed (report_error_boost_test);
    operation1_operation2_checks.set_report_failed (report_error_boost_test);
    operation2_operation1_checks.set_report_failed (report_error_boost_test);

    math::check_equal_on (type_checks, unequal_examples);

    math::check_magma <Type> (
        operation1, operation2, examples,
        type_checks, operation1_checks, operation2_checks,
        operation1_operation2_checks, operation2_operation1_checks);

    auto type_name = typeid (Type).name();
    auto operation1_name = math::check_detail::describe_operation (operation1);
    auto operation2_name = math::check_detail::describe_operation (operation2);

    std::cout << str (boost::format (
        "Results of testing the magma type %1% (%2%) with %3% and %4%:\n")
        % boost::units::detail::demangle (type_name) % type_name
        % operation1_name % operation2_name);

    type_checks.print_results (std::cout, "     ");
    std::cout << "- Operation1 ("<< operation1_name <<"):" << std::endl;
    operation1_checks.print_results (std::cout, "     ");
    std::cout << "- Operation2 ("<< operation2_name <<"):" << std::endl;
    operation2_checks.print_results (std::cout, "     ");
    std::cout << "- Operation1 (" << operation1_name
        << ") and operation2 (" << operation2_name << "):" << std::endl;
    operation1_operation2_checks.print_results (std::cout, "     ");
    std::cout << "- Operation2 (" << operation2_name
        << ") and operation1 ("<< operation1_name <<"):" << std::endl;
    operation2_operation1_checks.print_results (std::cout, "     ");

    BOOST_CHECK (type_checks.success());
    BOOST_CHECK (operation1_checks.success());
    BOOST_CHECK (operation2_checks.success());
    BOOST_CHECK (operation1_operation2_checks.success());
    BOOST_CHECK (operation2_operation1_checks.success());
}

template <class Type, class Operation1, class Operation2,
    class UnequalExamples, class Examples>
inline void report_check_magma (
    Operation1 const & operation1, Operation2 const & operation2,
    UnequalExamples const & unequal_examples, Examples const & examples)
{
    math::type_checklist type_checks;
    math::operation_checklist operation1_checks;
    math::operation_checklist operation2_checks;
    math::two_operations_checklist operation1_operation2_checks;
    math::two_operations_checklist operation2_operation1_checks;

    report_check_magma <Type> (operation1, operation2,
        unequal_examples, examples,
        type_checks, operation1_checks, operation2_checks,
        operation1_operation2_checks, operation2_operation1_checks);
}

template <class Type, class Direction, class Multiplication, class Addition,
    class UnequalExamples, class Examples>
inline void report_check_semiring (
    Multiplication const & multiplication, Addition const & addition,
    UnequalExamples const & unequal_examples, Examples const & examples,
    math::type_checklist & type_checks,
    math::operation_checklist & multiplication_checks,
    math::operation_checklist & addition_checks,
    math::two_operations_checklist & multiplication_addition_checks,
    math::two_operations_checklist & addition_multiplication_checks)
{
    // Check for consistency.
    type_checks.set_report_failed (report_error_boost_test);
    multiplication_checks.set_report_failed (report_error_boost_test);
    addition_checks.set_report_failed (report_error_boost_test);
    multiplication_addition_checks.set_report_failed (report_error_boost_test);
    addition_multiplication_checks.set_report_failed (report_error_boost_test);

    math::check_equal_on (type_checks, unequal_examples);

    math::check_semiring <Type, Direction> (
        multiplication, addition, examples,
        type_checks, multiplication_checks, addition_checks,
        multiplication_addition_checks, addition_multiplication_checks);

    auto type_name = typeid (Type).name();
    char const * direction_name =
        rime::if_ (std::is_same <Direction, math::left>(), "left ",
            rime::if_ (std::is_same <Direction, math::right>(), "right ", ""));
    auto multiplication_name
        = math::check_detail::describe_operation (multiplication);
    auto addition_name
        = math::check_detail::describe_operation (addition);

    std::cout << str (boost::format (
        "Results of testing the %1%semiring type %2% (%3%) with %4% and %5%:\n")
        % direction_name
        % boost::units::detail::demangle (type_name) % type_name
        % multiplication_name % addition_name);

    type_checks.print_results (std::cout, "     ");
    std::cout << "- Multiplication ("<< multiplication_name <<"):" << std::endl;
    multiplication_checks.print_results (std::cout, "     ");
    std::cout << "- Addition ("<< addition_name <<"):" << std::endl;
    addition_checks.print_results (std::cout, "     ");
    std::cout << "- Multiplication (" << multiplication_name
        << ") and addition (" << addition_name << "):" << std::endl;
    multiplication_addition_checks.print_results (std::cout, "     ");
    std::cout << "- Addition (" << addition_name
        << ") and multiplication ("<< multiplication_name <<"):" << std::endl;
    addition_multiplication_checks.print_results (std::cout, "     ");

    BOOST_CHECK (type_checks.success());
    BOOST_CHECK (multiplication_checks.success());
    BOOST_CHECK (addition_checks.success());
    BOOST_CHECK (multiplication_addition_checks.success());
    BOOST_CHECK (addition_multiplication_checks.success());
}

template <class Type, class Direction, class Multiplication, class Addition,
    class UnequalExamples, class Examples>
inline void report_check_semiring (
    Multiplication const & multiplication, Addition const & addition,
    UnequalExamples const & unequal_examples, Examples const & examples)
{
    math::type_checklist type_checks;
    math::operation_checklist multiplication_checks;
    math::operation_checklist addition_checks;
    math::two_operations_checklist multiplication_addition_checks;
    math::two_operations_checklist addition_multiplication_checks;

    report_check_semiring <Type, Direction> (
        multiplication, addition,
        unequal_examples, examples,
        type_checks, multiplication_checks, addition_checks,
        multiplication_addition_checks, addition_multiplication_checks);
}

/* Checking hashes. */

struct report_hash_mismatch_boost_test {
    template <class Left, class Right>
        void operator() (Left const & left, Right const & right,
            bool equal, bool hash_equal) const
    {
        BOOST_ERROR ("Mismatch between equality and hash function: "
            << math::check_detail::describe_magma (left) << " == "
            << math::check_detail::describe_magma (right) << " returns "
            << equal << ", but hash1 == hash2 returns " << hash_equal << ".");
    }
};

template <class Examples>
    inline void report_check_hash (Examples const & examples)
{
    std::cout << "Results of testing hashing of magma type: ";

    bool check_hash_success = math::check_hash (
        report_hash_mismatch_boost_test(), examples);

    if (check_hash_success)
        std::cout << "success\n";
    else
        std::cout << "FAILED\n";

    BOOST_CHECK (check_hash_success);
}

template <class General, class Examples>
    inline void report_check_cast_hash (Examples const & examples)
{
    auto type_name = typeid (General).name();

    std::cout << str (boost::format (
        "Results of testing hashing of magma type %1% (%2%): ")
        % boost::units::detail::demangle (type_name) % type_name);

    bool check_hash_success = math::check_cast_hash <General> (
        report_hash_mismatch_boost_test(), examples);

    if (check_hash_success)
        std::cout << "success\n";
    else
        std::cout << "FAILED\n";

    BOOST_CHECK (check_hash_success);
}

} // namespace math

#endif // MATH_CHECK_REPORT_CHECK_MAGMA_BOOST_TEST
