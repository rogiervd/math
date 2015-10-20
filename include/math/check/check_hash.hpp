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

#ifndef MATH_CHECK_CHECK_HASH_HPP_INCLUDED
#define MATH_CHECK_CHECK_HASH_HPP_INCLUDED

#include <functional>
#include <utility>

#include <boost/functional/hash.hpp>

#include "range/core.hpp"
#include "range/transform.hpp"
#include "range/all_of.hpp"

namespace math {

namespace check_detail {

    using std::bind;
    using std::ref;
    using std::placeholders::_1;

    struct check_hash_base {
        template <class ReportHashMismatch, class Example1, class Example2>
            bool operator() (ReportHashMismatch const & report_hash_mismatch,
                Example1 const & example1, Example2 const & example2) const
        {
            boost::hash <Example1> hasher1;
            boost::hash <Example2> hasher2;
            bool equal = (example1 == example2);
            bool hash_equal = (hasher1 (example1) == hasher2 (example2));
            if (equal != hash_equal) {
                report_hash_mismatch (example1, example2, equal, hash_equal);
                return false;
            }
            return true;
        }
    };

    template <class Examples> struct check_hash : check_hash_base {
        template <class ReportHashMismatch>
        bool operator() (ReportHashMismatch const & report_hash_mismatch,
            Examples const & examples) const
        {
            // Check with each example.
            return range::all_of (
                range::transform (examples, bind (ref (*this),
                    ref (report_hash_mismatch), ref (examples), _1)));
        }

        template <class ReportHashMismatch, class Example1>
            bool operator() (ReportHashMismatch const & report_hash_mismatch,
                Examples const & examples, Example1 const & example1) const
        {
            // Check with each example.
            return range::all_of (
                range::transform (examples, bind (ref (*this),
                    ref (report_hash_mismatch), ref (example1), _1)));
        }

        using check_hash_base::operator();
    };

    /** \brief
    Check that the hash value for an object is the same as after casting to a
    different type.
    */
    template <class General> struct check_cast_hash {
        template <class ReportHashMismatch, class Specific>
            bool operator() (ReportHashMismatch const & report_hash_mismatch,
                Specific const & specific) const
        {
            General general (specific);
            return check_hash_base() (report_hash_mismatch, specific, general);
        }
    };

} // namespace check_detail

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
template <class ReportHashMismatch, class Examples>
    inline bool check_hash (
        ReportHashMismatch const & report_hash_mismatch,
        Examples const & examples)
{
    return check_detail::check_hash <Examples>() (
        report_hash_mismatch, examples);
}

/** \brief
Check that the hash value for a range of objects is the same as after casting to
a different type.
*/
template <class General, class ReportHashMismatch, class Examples>
    inline bool check_cast_hash (
        ReportHashMismatch const & report_hash_mismatch,
        Examples const & examples)
{
    using std::bind;
    using std::ref;
    using std::placeholders::_1;

    return range::all_of (range::transform (examples,
        bind (check_detail::check_cast_hash <General>(),
            ref (report_hash_mismatch), _1)));
}

} // namespace math

#endif // MATH_CHECK_CHECK_HASH_HPP_INCLUDED
