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
Define a Cartesian product of magmas.
*/

#ifndef MATH_PRODUCT_HPP_INCLUDED
#define MATH_PRODUCT_HPP_INCLUDED

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/functional/hash_fwd.hpp>

#include "meta/vector.hpp"
#include "meta/all_of_c.hpp"

#include "utility/returns.hpp"
#include "utility/type_sequence_traits.hpp"

#include "rime/core.hpp"

#include "range/core.hpp"
#include "range/tuple.hpp"
#include "range/call_unpack.hpp"
#include "range/equal.hpp"
#include "range/less_lexicographical.hpp"
#include "range/transform.hpp"
#include "range/any.hpp"
#include "range/all.hpp"
#include "range/hash_range.hpp"

#include "magma.hpp"
#include "detail/tuple_helper.hpp"

namespace math {

template <class Operation = void> struct with_inverse;

/**
Magma that is the Cartesian product of a number of magmas.

If \a Inverse is with_inverse with an operation, then the inverse of this
operation is implemented.
In that case, the inverse cannot be defined on that component, and therefore
the inverse cannot be defined for the whole product.
If any component of this product is an annihilator for that operation, the whole
product must therefore be an annihilator.
Therefore, with <c>with_inverse \<Operation></c>, is_annihilator returns \c true
if any component returns \c true for is_annihilator.
Since any annihilator should be equal, \c equal returns \c true for two products
that have any annihilator for that operation.

To produce (as opposed to detect) an annihilator, each of the components must
have an annihilator for that operation.

\c order is not defined for the product: even if an operation implements a
strict weak ordering on each component, that does not implement any ordering on
the product.

Operations on this product are associative, commutative, idempotent,
distributive, and the product is a semiring, if and only if each of the
components is.

Products supports Boost.Hash, if \c boost/functional/hash.hpp is included.
If the hash values of the components of two products are the same, then the hash
value of the two products will be the same.

\todo This is a heterogeneous tuple. Is that good enough? I think so.
OpenFst has Power<W,n> and SparsePower<W>, which hold n or a variable number
of components.
This may be useful for unknown file formats, but we'd need an any_magma, and
the ability to live with a performance hit.

\tparam Components
    The list of components, given as math::over.
\tparam Inverse
    (optional)
    An inverse operation that is allowed, given as math::with_inverse.
*/
template <class Components, class Inverse = with_inverse<>> class product;

template <class ComponentTags, class Inverse> struct product_tag;

template <class ... Components, class Inverses>
    struct decayed_magma_tag <product <over <Components ...>, Inverses>>
{
    typedef product_tag <
            over <typename decayed_magma_tag <Components>::type ...>, Inverses>
        type;
};

template <class ... Components, class Inverse>
    class product <over <Components ...>, Inverse>
{
public:
    typedef meta::vector <Components ...> component_types;
    typedef range::tuple <Components ...> components_type;
    typedef Inverse inverse_specification;

    static_assert (meta::all_of_c <is_magma <Components>::value ...>::value,
        "Not all components passed to math::product are magmas.");

private:
    components_type components_;

    // Private type so that it cannot be constructed exept inside this class.
    struct dummy_type {};

public:
    // All elements explicitly convertible: implicit constructor.
    template <class ... Arguments, class Enable = typename boost::enable_if <
        utility::are_constructible <
            meta::vector <Components ...>, meta::vector <Arguments ...>>>::type>
    explicit product (Arguments && ... arguments)
    : components_ (std::forward <Arguments> (arguments) ...) {}

    product (product const &) = default;
    product (product &&) = default;

    /**
    Construct from a product with different component types, all of which are
    implicitly convertible to the component types of this.
    This constructor is implicit.
    \param other The product to copy.
    */
    template <class ... OtherComponents, class Enable = typename
        boost::enable_if <utility::are_convertible <
            meta::vector <OtherComponents const & ...>,
            meta::vector <Components ...>>
        >::type>
    product (product <over <OtherComponents ...>, Inverse> const & other)
    : components_ (other.components()) {}

    /**
    Construct from a product with different component types, at least one of
    which is explicitly convertible and not implicitly convertible.
    This constructor is explicit.
    \param other The product to copy.
    \internal \param dummy
        A dummy argument to distinguish this constructor from the implicit one.
    */
    template <class ... OtherComponents>
    explicit product (
        product <over <OtherComponents ...>, Inverse> const & other, typename
        boost::enable_if <
            tuple_helper::components_constructible_only <
                meta::vector <Components ...>,
                meta::vector <OtherComponents const & ...>>,
            dummy_type>::type = dummy_type())
    : components_ (other.components()) {}

    product & operator = (product const &) = default;
    product & operator = (product &&) = default;

    components_type & components() { return components_; }
    components_type const & components() const { return components_; }
};

namespace callable {

    template <class Inverses> struct make_product {
        template <class ... Components>
            product <over <Components ...>, Inverses>
            operator() (Components const & ... components) const
        { return product <over <Components ...>, Inverses> (components ...); }
    };

    template <class Inverses> struct make_product_over {
        template <class Components>
            auto operator() (Components && components) const
        RETURNS (range::call_unpack (
            make_product <Inverses>(), std::forward <Components> (components)));
    };

} // namespace callable

template <class Inverses, class ... Components>
    inline auto make_product (Components const & ... components)
RETURNS (callable::make_product <Inverses>() (components ...));

template <class Inverses, class Components>
    inline auto make_product_over (Components && components)
RETURNS (callable::make_product_over <Inverses>() (
    std::forward <Components> (components)));

namespace product_detail {

    template <class Type> struct is_product_tag : std::false_type {};
    template <class ComponentTags, class Inverses>
        struct is_product_tag <product_tag <ComponentTags, Inverses>>
    : std::true_type {};

} // namespace product_detail

MATH_MAGMA_GENERATE_OPERATORS (product_detail::is_product_tag)

namespace operation {

    /* Queries. */

    template <class Tags, class Inverses>
        struct is_member <product_tag <Tags, Inverses>>
    {
        template <class Product> auto operator() (Product const & product) const
        RETURNS (range::all (
            range::transform (math::is_member, product.components())));
    };

    // is_annihilator.
    /*
    If the operation has an inverse: any component being an annihilator makes
    the whole product an annihilator.
    If not, then the default implementation (compare component-per-component
    with the result of annihilator()) works.
    */
    template <class ComponentTags, class Operation>
        struct is_annihilator <
            product_tag <ComponentTags, with_inverse <Operation>>, Operation>
    {
        template <class Product> auto operator() (Product const & product) const
        RETURNS (range::any (range::transform (
            callable::is_annihilator <Operation>(), product.components())));
    };

    // equal.
    // With no inverse: just compare components.
    template <class ComponentTags>
        struct equal <product_tag <ComponentTags, with_inverse<>>>
    : tuple_helper::equal_components <math::callable::equal> {};

    // With inverse: compare annihilators equal, otherwise compare components.
    template <class ComponentTags, class Operation>
        struct equal <product_tag <ComponentTags, with_inverse <Operation>>>
    : tuple_helper::equal_if_annihilator <Operation,
        tuple_helper::equal_components <math::callable::equal>> {};

    // approximately_equal.
    template <class ComponentTags>
        struct approximately_equal <product_tag <ComponentTags, with_inverse<>>>
    : tuple_helper::equal_components <math::callable::approximately_equal> {};

    template <class ComponentTags, class Operation>
        struct approximately_equal <
            product_tag <ComponentTags, with_inverse <Operation>>>
    : tuple_helper::equal_if_annihilator <Operation,
        tuple_helper::equal_components <math::callable::approximately_equal>>
    {};

    // compare.
    template <class ... ComponentTags>
        struct compare <product_tag <over <ComponentTags ...>, with_inverse<>>,
            typename boost::enable_if <meta::all_of_c <
                is_implemented <compare <ComponentTags>>::value ...>>::type>
    : tuple_helper::compare_components <math::callable::compare> {};

    // With inverse: annihilators go at the end.
    template <class ... ComponentTags, class Operation>
        struct compare <product_tag <over <ComponentTags ...>,
            with_inverse <Operation>>,
            typename boost::enable_if <boost::mpl::and_ <
                meta::all_of_c <
                    is_implemented <compare <ComponentTags>>::value ...>,
                // Only instantiate this if the Operation is not void.
                // GCC 4.6 requires this, or it won't realise the the
                // specialisation above is better.
                boost::mpl::not_ <std::is_same <Operation, void>>
            >>::type>
    : tuple_helper::compare_if_annihilator <Operation,
        tuple_helper::compare_components <math::callable::compare>> {};

    namespace tuple_helper {

        template <class ComponentTags, class Inverses>
            struct get_components <product_tag <ComponentTags, Inverses>>
        {
            template <class Product> auto operator() (Product const & p) const
            RETURNS (p.components());
        };

    } // namespace tuple_helper

    /* Produce. */

    // non_member could be implemented if all components implement it.
    // Otherwise, what value to pick for the other components?

    template <class ... Tags, class Inverses, class Operation>
        struct identity <product_tag <over <Tags ...>, Inverses>, Operation>
    : tuple_helper::nullary_operation <callable::make_product <Inverses>,
        meta::vector <identity <Tags, Operation> ...>> {};

    // annihilator: implemented if all components have an annihilator...
    // (otherwise, what value to pick for the other components?)
    template <class ... Tags, class Inverses, class Operation>
        struct annihilator <product_tag <over <Tags ...>, Inverses>, Operation,
        typename boost::enable_if <meta::all_of_c <
            is_implemented <annihilator <Tags, Operation>>::value ...
        >>::type>
    : tuple_helper::nullary_operation <callable::make_product <Inverses>,
        meta::vector <annihilator <Tags, Operation> ...>> {};

    // ... and there is at least one component.
    template <class Inverses, class Operation>
        struct annihilator <product_tag <over<>, Inverses>, Operation>
    : unimplemented {};

    /* Binary operations. */

    template <class ... Tags, class Inverses>
        struct times <product_tag <over <Tags ...>, Inverses>>
    : tuple_helper::binary_operation <callable::make_product <Inverses>,
        meta::vector <times <Tags> ...>> {};

    template <class ... Tags, class Inverses>
        struct plus <product_tag <over <Tags ...>, Inverses>>
    : tuple_helper::binary_operation <callable::make_product <Inverses>,
        meta::vector <plus <Tags> ...>> {};

    // is_semiring iff all components are semirings...
    template <class ... Tags, class Inverses,
            class Direction, class Operation1, class Operation2>
        struct is_semiring <product_tag <over <Tags ...>, Inverses>, Direction,
            Operation1, Operation2>
    : meta::all_of_c <
        is_semiring <Tags, Direction, Operation1, Operation2>::value ...> {};

    // ... except when the product is empty.
    template <class Inverses, class Direction,
        class Operation1, class Operation2>
    struct is_semiring <product_tag <over<>, Inverses>, Direction,
        Operation1, Operation2>
    : rime::false_type {};

    template <class ... Tags, class Inverses,
            class Direction, class Operation1, class Operation2>
        struct is_distributive <product_tag <over <Tags ...>, Inverses>,
            Direction, Operation1, Operation2>
    : meta::all_of_c <
        is_distributive <Tags, Direction, Operation1, Operation2>::value ...>
    {};

    template <class ... Tags,
            class ... Components1, class ... Components2, class Inverses>
        struct unify_type <product_tag <over <Tags ...>, Inverses>,
            product <over <Components1 ...>, Inverses>,
            product <over <Components2 ...>, Inverses>>
    {
        // Unify both underlying types separately.
        typedef product <over <
            typename unify_type <Tags, Components1, Components2>::type ...>,
            Inverses> type;
    };

    // divide: only if the template parameter Inverses is with_inverse <times>.
    template <class ... Tags, class Direction>
        struct divide <product_tag <over <Tags ...>,
            with_inverse <callable::times>>, Direction>
    : tuple_helper::binary_operation <
        callable::make_product <with_inverse <callable::times>>,
        meta::vector <divide <Tags, Direction> ...>> {};

    // minus: only if the template parameter Inverses is with_inverse <plus>.
    template <class ... Tags, class Direction>
        struct minus <product_tag <over <Tags ...>,
            with_inverse <callable::plus>>, Direction>
    : tuple_helper::binary_operation <
        callable::make_product <with_inverse <callable::plus>>,
        meta::vector <minus <Tags, Direction> ...>> {};

    template <class ... Tags, class Direction, class Operation>
        struct invert <product_tag <over <Tags ...>, with_inverse <Operation>>,
            Direction, Operation>
    : tuple_helper::unary_operation <
        callable::make_product <with_inverse <Operation>>,
        meta::vector <invert <Tags, Direction, Operation> ...>> {};

    template <class ... Tags, class Inverse, class Operation>
        struct reverse <product_tag <over <Tags ...>, Inverse>, Operation>
    : tuple_helper::unary_operation <
        callable::make_product <Inverse>,
        meta::vector <reverse <Tags, Operation> ...>> {};

    template <class ... Tags, class Inverses>
        struct print <product_tag <over <Tags ...>, Inverses>>
    : tuple_helper::print_components <meta::vector <Tags ...>> {};

} // namespace operation

// Boost.Hash support.

namespace product_detail {

    // Products with inverses need to treat annihilators specially.
    static std::size_t constexpr annihilator_hash =
        std::size_t (0xcba51c150183b7f1 & std::size_t (-1));

} // namespace product_detail

// Without an inverse: just combine the hash values of the components.
template <class Components>
    inline std::size_t hash_value (
        product <Components, with_inverse<>> const & p)
{ return range::hash_range (p.components()); }

// With an inverse: if p is an annihilator, then return a special hash value.
template <class Components, class Operation>
    inline std::size_t hash_value (
        product <Components, with_inverse <Operation>> const & p)
{
    if (is_annihilator <Operation> (p))
        return product_detail::annihilator_hash;
    return range::hash_range (p.components());
}

} // namespace math

#endif // MATH_PRODUCT_HPP_INCLUDED
