// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MATH_CHECK_CHECK_HASH_HPP_INCLUDED
#define MATH_CHECK_CHECK_HASH_HPP_INCLUDED

#include <utility>

#include <boost/functional/hash.hpp>

#include "range/all_of.hpp"
#include "range/core.hpp"
#include "range/transform.hpp"

namespace math {

namespace check_detail {

    template <class ReportHashMismatch, class Example1, class Example2>
    inline bool check_hash_pair(
        ReportHashMismatch const & report_hash_mismatch,
        Example1 const & example1, Example2 const & example2)
    {
        boost::hash<Example1> hasher1;
        boost::hash<Example2> hasher2;
        bool equal = (example1 == example2);
        bool hash_equal = (hasher1(example1) == hasher2(example2));
        if (equal != hash_equal) {
            report_hash_mismatch(example1, example2, equal, hash_equal);
            return false;
        }
        return true;
    }

}  // namespace check_detail

/** \brief
Check hash values using a range of examples.

For each pair of examples it is check whether the hash compares equal iff the
values compare equal.
It might be possible that you have legitimately clashing hash values, but this
is exceedingly unlikely.
If that happens, though, it is best to use slightly different values to test on
until the spurious error goes away.

\param examples
    The examples to test.
    Include in particular values that are different somehow but compare equal.
*/
template <class ReportHashMismatch, class Examples> inline bool check_hash(
    ReportHashMismatch const & report_hash_mismatch, Examples const & examples)
{
    // Check with each pair of examples.
    auto results_per_example1 =
        range::transform(examples, [&](auto const & example1) {
            auto results_per_example2 =
                range::transform(examples, [&](auto const & example2) {
                    return check_detail::check_hash_pair(
                        report_hash_mismatch, example1, example2);
                });
            return range::all_of(results_per_example2);
        });
    return range::all_of(results_per_example1);
}

/** \brief
Check that the hash value for a range of objects is the same as after casting to
a different type.
*/
template <class General, class ReportHashMismatch, class Examples>
inline bool check_cast_hash(
    ReportHashMismatch const & report_hash_mismatch, Examples const & examples)
{
    return range::all_of(range::transform(examples, [&](auto const & example) {
        General general(example);
        return check_detail::check_hash_pair(
            report_hash_mismatch, example, general);
    }));
}

}  // namespace math

#endif  // MATH_CHECK_CHECK_HASH_HPP_INCLUDED
