/*
Copyright 2014, 2015 Rogier van Dalen.

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
Define a magma that is a tuple of other magmas, where "plus" takes the best of
the two values according to a lexicographical ordering.
*/

#ifndef MATH_LEXICOGRAPHICAL_HPP_INCLUDED
#define MATH_LEXICOGRAPHICAL_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/and.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/functional/hash_fwd.hpp>

#include "meta/vector.hpp"
#include "meta/all_of_c.hpp"

#include "utility/type_sequence_traits.hpp"

#include "range/tuple.hpp"
#include "range/call_unpack.hpp"
#include "range/equal.hpp"
#include "range/less_lexicographical.hpp"
#include "range/transform.hpp"
#include "range/all.hpp"
#include "range/hash_range.hpp"

#include "magma.hpp"
#include "detail/tuple_helper.hpp"

namespace math {

/**
The lexicographical semiring.
This is a semiring that has multiple components.
Operations \ref plus and \ref choose are defined as taking the best in a strict
weak ordering.
The ordering is defined by lexicographical comparison: if the first components
are the same, the second components are compared, et cetera.

Often, the user will not care about the ordering of the later components, but it
still needs to be defined to make this a proper semiring.
For example, the first component could be of type \ref cost or
\ref max_semiring, indicating a cost or probability.
The components following it could form the payload, for example, the word
sequence.
Used on the correct automaton, the right algorithm might yield the lowest-cost
or highest-probability word sequence.

All components must be monoids over \ref times, and have \ref choose defined.
The first component must be a semiring over \ref times and \ref choose.
Each of the other elements must almost be a semiring, but does not have to have
a multiplicative annihilator.

The first component is used to indicate the additive identity (and, thus, the
multiplicative annihilator), \ref zero(), of the whole semiring.
When the first component is zero, the meaning of the whole semiring object is
the additive identity, whatever the value of the other components.
Any two objects with the first component equal and the additive identity will
therefore compare equal.
Such objects can, however, have detectably different elements after the first
component.

This type is implicitly convertible from another lexicographical semiring if
all components are.
It is explicitly convertible from if one or more of the components is explicitly
convertible (and the rest is implicitly convertible).

This semiring does not have \ref divide.
This would require each component to be divided, which is impossible if any is
an annihilator.
Then, any element of a lexicographical semiring that has any component that is
an annihilator would have to be an annihilator itself.
This is functionality that could be added, but should not be the default.
It is not currently implemented.

lexicographical objects can be constructed explicitly with a list of argument,
each of which is pairwise convertible to the component.
They can also be constructed from a compatible lexicographical: implicitly if
all components are implicitly convertible; and explicitly if some are only
explicitly convertible.

It has a member function \c components() which returns the range with the
components in order.

The lexicographical semiring supports Boost.Hash, if
\c boost/functional/hash.hpp is included.
If the hash values of the components of two lexicographical semirings are the
same, then the hash value of the two semirings will be the same.

\tparam Components
    Type of the form \ref over\<...> with the magmas that should be contained.
    All the magmas must have an ordered \c choose.
    The first of them must be a semiring over \c times and \c choose.
    The rest must be semirings except for the annihilator.
    That is, each must be a commutative monoid over \c choose; a monoid over
    \c times; and \c times must distribute over \c choose.

\internal
There is some documentation of members in lexicographical <over <...>>, but
Sphinx refuses some of the syntax anyway.
*/
template <class Components> class lexicographical;

template <class ComponentTags> struct lexicographical_tag;

template <class ... Components>
    struct decayed_magma_tag <lexicographical <over <Components ...>>>
{
    typedef lexicographical_tag <
        over <typename magma_tag <Components>::type ...>> type;
};

template <class ... Components> class lexicographical <over <Components ...>> {
public:
    typedef meta::vector <Components ...> component_types;
    typedef range::tuple <Components ...> components_type;

private:
    typedef typename meta::first <component_types>::type first_component_type;
    typedef typename meta::drop <component_types>::type rest_component_type;

    static_assert (meta::all_of_c <std::is_same <Components,
            typename std::decay <Components>::type>::value ...>::value,
        "The components may not be cv- or reference-qualified.");

    static_assert (meta::all_of_c <has <callable::order <callable::choose> (
            Components, Components)>::value...>::value,
        "All components must have an ordered 'choose'.");

    static_assert (meta::all_of_c <
            is::monoid <callable::times, Components>::value ...>::value,
        "All components must be monoids over 'times'.");
    static_assert (meta::all_of_c <
            is::monoid <callable::choose, Components>::value ...>::value,
        "All components must be monoids over 'choose'.");
    static_assert (meta::all_of_c <
            is::commutative <callable::choose, Components>::value ...>::value,
        "For all components, 'choose' must be commutative.");

public:
    /**
    Evaluate to \c true iff this is a semiring in \a Direction.
    For this to be true, the first component must be a semiring in \a Direction,
    and for the other components, \c times is distributive over \c choose in
    \a Direction.
    */
    template <class Direction> struct is_semiring
    : boost::mpl::and_ <
        is::semiring <Direction,
            callable::times, callable::choose, first_component_type>,
        meta::all_of_c <is::distributive <Direction,
            callable::times, callable::choose, Components>::value ...>
    > {};

    static_assert (is_semiring <left>::value || is_semiring <right>::value,
        "The components must allow this to be a semiring in at least one "
        "direction.");

    components_type components_;
public:
    /**
    Construct from arguments that are pairwise convertible to the components.
    This constructor is implicit.
    */
    template <class ... Arguments, class Enable = typename boost::enable_if <
        utility::are_constructible <
            meta::vector <Components ...>, meta::vector <Arguments ...>>>::type>
    explicit lexicographical (Arguments && ... arguments)
    : components_ (std::forward <Arguments> (arguments) ...) {}

    lexicographical (lexicographical const &) = default;
    lexicographical (lexicographical &&) = default;

    /**
    Construct from a lexicographical with different component types, all of
    which are implicitly convertible to the component types of this.
    This constructor is implicit.
    \param other The lexicographical to copy.
    */
    template <class ... OtherComponents>
    lexicographical (lexicographical <over <OtherComponents ...>> const & other,
        typename boost::enable_if <utility::are_convertible <
            meta::vector <OtherComponents const & ...>,
            meta::vector <Components ...>>
        >::type * = 0)
    : components_ (other.components()) {}

    /**
    Construct from a lexicographical with different component types, at least
    one of which is explicitly convertible and not implicitly convertible.
    This constructor is explicit.
    \param other The lexicographical to copy.
    */
    template <class ... OtherComponents>
    explicit lexicographical (
        lexicographical <over <OtherComponents ...>> const & other, typename
        boost::enable_if <
            tuple_helper::components_constructible_only <
                meta::vector <Components ...>,
                meta::vector <OtherComponents const & ...>>
            >::type * = 0)
    : components_ (other.components()) {}

    lexicographical & operator = (lexicographical const &) = default;
    lexicographical & operator = (lexicographical &&) = default;

    components_type & components() { return components_; }
    components_type const & components() const { return components_; }
};

namespace callable {

    struct make_lexicographical {
        template <class ... Components>
            lexicographical <over <Components ...>> operator() (
                Components const & ... components) const
        { return lexicographical <over <Components ...>> (components ...); }
    };

    struct make_lexicographical_over {
        template <class Components>
            auto operator() (Components && components) const
        RETURNS (range::call_unpack (
            make_lexicographical(), std::forward <Components> (components)));
    };

} // namespace callable

static auto constexpr make_lexicographical = callable::make_lexicographical();
static auto constexpr make_lexicographical_over
    = callable::make_lexicographical_over();

namespace detail {

    template <class Type> struct is_lexicographical_tag : boost::mpl::false_ {};
    template <class ... ComponentTags> struct is_lexicographical_tag <
        lexicographical_tag <over <ComponentTags ...>>>
    : boost::mpl::true_ {};

} // namespace detail

namespace operation {

    namespace tuple_helper {

        template <class ... ComponentTags> struct get_components <
            lexicographical_tag <over <ComponentTags ...>>>
        {
            template <class Lexicographical>
                auto operator() (Lexicographical const & l) const
            RETURNS (l.components());
        };

    } // namespace tuple_helper

    /* Queries. */

    template <class ... ComponentTags>
        struct is_member <lexicographical_tag <over <ComponentTags ...>>>
    {
        template <class Lexicographical>
            auto operator() (Lexicographical const & l) const
        RETURNS (range::all (range::transform (
            ::math::is_member, l.components())));
    };

    // is_annihilator.
    /*
    If the operation has an inverse: any component being an annihilator makes
    the whole product an annihilator.
    If not, then the default implementation (compare component-per-component
    with the result of annihilator()) works.
    */
    template <class ComponentTags> struct is_annihilator <
        lexicographical_tag <ComponentTags>, callable::times>
    {
        template <class Lexicographical> auto operator() (
            Lexicographical const & l) const
        RETURNS (math::is_annihilator <callable::times> (
            range::first (l.components())));
    };

    // Compare annihilators equal, otherwise compare components.
    template <class ComponentTags>
        struct equal <lexicographical_tag <ComponentTags>>
    : tuple_helper::equal_if_annihilator <callable::times,
        tuple_helper::equal_components <math::callable::equal>> {};

    // Compare annihilators equal, otherwise compare components.
    template <class ComponentTags>
        struct approximately_equal <lexicographical_tag <ComponentTags>>
    : tuple_helper::equal_if_annihilator <callable::times,
        tuple_helper::equal_components <math::callable::approximately_equal>>
    {};

    // Compare annihilators equal, otherwise compare components.
    template <class ComponentTags>
        struct compare <lexicographical_tag <ComponentTags>>
    : tuple_helper::compare_if_annihilator <callable::times,
        tuple_helper::compare_components <math::callable::compare>> {};

    /* Produce. */

    template <class ... ComponentTags>
        struct identity <lexicographical_tag <over <ComponentTags ...>>,
            callable::times>
    {
        auto operator() () const
        RETURNS (make_lexicographical (
            identity <ComponentTags, callable::times>()() ...));
    };

    /**
    Generalised 0.
    This is (0, 1 ...) because apart from for the first component, the plus
    operation is not guaranteed to be defined.
    */
    template <class FirstComponentTag, class ... ComponentTags>
        struct identity <lexicographical_tag <over <
            FirstComponentTag, ComponentTags ...>>, callable::choose>
    {
        auto operator() () const
        RETURNS (make_lexicographical (
            identity <FirstComponentTag, callable::choose>()(),
            identity <ComponentTags, callable::times>()() ...));
    };

    // plus: forward to implementation for "choose".
    template <class Tags>
        struct identity <lexicographical_tag <Tags>, callable::plus>
    : identity <lexicographical_tag <Tags>, callable::choose> {};

    /**
    Multiplicative annihilator: equal to the additive identity.
    */
    template <class FirstComponentTag, class ... ComponentTags>
        struct annihilator <lexicographical_tag <over <
            FirstComponentTag, ComponentTags ...>>, callable::times>
    {
        auto operator() () const
        RETURNS (make_lexicographical (
            annihilator <FirstComponentTag, callable::times>()(),
            identity <ComponentTags, callable::times>()() ...));
    };

    /* Operations. */

    template <class Tags>
        struct order <lexicographical_tag <Tags>, callable::choose>
    : tuple_helper::compare_components <
        math::callable::order <callable::choose>> {};

    // order <plus>: forward to order <choose>.
    template <class Tags>
        struct order <lexicographical_tag <Tags>, callable::plus>
    : order <lexicographical_tag <Tags>, callable::choose> {};

    /**
    Apply the multiplication operation on the weights, and on the values.
    */
    template <class ... ComponentTags>
        struct times <lexicographical_tag <over <ComponentTags ...>>>
    : tuple_helper::binary_operation <callable::make_lexicographical,
        meta::vector <times <ComponentTags> ...>> {};

    // Semiring under times and choose/plus: depends on the direction.
    template <class FirstComponentTag, class ... ComponentTags, class Direction>
        struct is_semiring <
            lexicographical_tag <over <FirstComponentTag, ComponentTags ...>>,
                Direction, callable::times, callable::choose>
    : meta::all_of_c <
        is_semiring <FirstComponentTag,
            Direction, callable::times, callable::choose>::value,
        is_distributive <ComponentTags,
            Direction, callable::times, callable::choose>::value ...> {};

    template <class ComponentTags, class Direction> struct is_semiring <
        lexicographical_tag <ComponentTags>,
            Direction, callable::times, callable::plus>
    : is_semiring <lexicographical_tag <ComponentTags>,
        Direction, callable::times, callable::choose> {};

    /*
    divide is undefined.
    It would have to call divide on all its components, and if one of them was
    an annihilator, the whole lexicographical semiring would have to be an
    annihilator.
    This would probably require an interface similar to product's.
    */

    template <class ... Tags>
        struct print <lexicographical_tag <over <Tags ...>>>
    : tuple_helper::print_components <meta::vector <Tags ...>> {};

    template <class ... ComponentTags,
            class ... Components1, class ... Components2>
        struct unify_type <lexicographical_tag <over <ComponentTags ...>>,
            lexicographical <over <Components1 ...>>,
            lexicographical <over <Components2 ...>>>
    {
        // Unify both underlying types in parallel.
        typedef lexicographical <over <typename unify_type <
            ComponentTags, Components1, Components2>::type ...>> type;
    };

} // namespace operation

MATH_MAGMA_GENERATE_OPERATORS (detail::is_lexicographical_tag)

// Boost.Hash support.

namespace lexicographical_detail {

    // Annihilators are treated specially.
    static std::size_t constexpr annihilator_hash =
        std::size_t (0xa5e33b35c473015b  & std::size_t (-1));

} // namespace lexicographical_detail

// Without an inverse: just combine the hash values of the components.
template <class Components>
    inline std::size_t hash_value (lexicographical <Components> const & l)
{
    if (is_annihilator <callable::times> (l))
        return lexicographical_detail::annihilator_hash;
    else
        return range::hash_range (l.components());
}

} // namespace math

#endif // MATH_LEXICOGRAPHICAL_HPP_INCLUDED
