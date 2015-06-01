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
#include "range/for_each.hpp"

namespace math {

namespace check_detail {

    using std::bind;
    using std::ref;
    using std::placeholders::_1;

    template <class Examples> struct check_hash {
        void operator() (Examples const & examples) const
        {
            // Check with each example.
            range::for_each (bind (ref (*this), ref (examples), _1), examples);
        }

        template <class Example1>
            void operator() (
                Examples const & examples, Example1 const & example1) const
        {
            // Check with each example.
            range::for_each (bind (ref (*this), ref (example1), _1), examples);
        }

        template <class Example1, class Example2>
            void operator() (
                Example1 const & example1, Example2 const & example2) const
        {
            boost::hash <Example1> hasher1;
            boost::hash <Example2> hasher2;
            if (example1 == example2) {
                BOOST_CHECK_EQUAL (hasher1 (example1), hasher2 (example2));
            } else {
                BOOST_CHECK (hasher1 (example1) != hasher2 (example2));
            }
        }
    };

    /** \brief
    Check that the hash value for an object is the same as after casting to a
    different type.
    */
    template <class Target> struct check_cast_hash {
        template <class Source> void operator() (Source const & s) const
        {
            std::size_t value = boost::hash <Source>() (s);
            BOOST_CHECK_EQUAL (value, boost::hash <Target>() (Target (s)));
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
template <class Examples> inline void check_hash (Examples const & examples) {
    check_detail::check_hash <Examples>() (examples);
}

/** \brief
Check that the hash value for a range of objects is the same as after casting to
a different type.
*/
template <class Target, class Examples>
    inline void check_cast_hash (Examples const & examples)
{ range::for_each (check_detail::check_cast_hash <Target>(), examples); }

} // namespace math

#endif // MATH_CHECK_CHECK_HASH_HPP_INCLUDED
