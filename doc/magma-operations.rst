.. _magma-operations:

Operations
==========

Querying properties of objects:

*   :cpp:type:`math::is_member`: test wether an object of the magma type is in the magma.
*   :cpp:type:`math::equal`: test whether two objects are equal.
*   :cpp:type:`math::not_equal`: test whether two objects are not equal.
*   :cpp:type:`math::approximately_equal`: test whether two objects are approximately equal.
*   :cpp:type:`math::compare`: compare two objects with a strict weak ordering.
*   :cpp:func:`math::order`: compare two objects with an ordering implied by a path operation.

Producing objects:

*   :cpp:type:`math::non_member`: return an object of the magma type that is not in the magma.
    E.g. not-a-number for floating-point types.
*   :cpp:func:`math::identity`: return the identity element for a magma and operation.
    E.g. ``1`` for ``times`` on number types.
*   :cpp:func:`math::annihilator`: return the annihilator for a magma and operations.
    E.g. ``0`` for ``times`` on number types.
*   :cpp:func:`math::zero`: return the identity for ``times``.
*   :cpp:func:`math::one`: return the identity for ``plus``.

Binary operations:

..  There seems to be no role ":cpp:variable:"!

*   :cpp:type:`math::pick`: return one value or the other, depending on a condition.
*   :cpp:type:`math::choose`: return the most preferable of two values.
*   :cpp:type:`math::times`: multiply two values.
*   :cpp:type:`math::plus`: add two values.
*   :cpp:func:`math::divide`: divide one value by another.
    For some magmas, left and right division are distinguished.
*   :cpp:func:`math::minus`: subtract one value from another.
    For some magmas, left and right subtraction are distinguished.

Unary operations:

*   :cpp:func:`math::invert`: return the inverse of an element under an operation.
    This is not available for all magmas, even if the inverse operation is available.
*   :cpp:func:`math::reverse`: return the reverse of an element under an operation.
*   :cpp:type:`math::print`: output the element to a stream.

.. *   :cpp:func:`math::closure` - with operation

Operation on operations:

*   :cpp:func:`math::inverse_operation`: return the inverse of an operation.
    The result is also an operation.
    For example, ``inverse_operation <math::left> (math::times)`` returns an object of type ``math::callable::divide <math::left>``.

.. _magma_operations_queries:

Queries about operations:

*   :cpp:class:`math::is::associative`: whether an operation is associative.
*   :cpp:class:`math::is::commutative`: whether an operation is commutative.
*   :cpp:class:`math::is::idempotent`: whether an operation is idempotent.
*   :cpp:class:`math::is::path_operation`: whether an operation is a path operation, i.e. whether it returns either the left or the right argument.
*   :cpp:class:`math::is::distributive`: whether one operation distributions over another.
*   :cpp:class:`math::is::monoid`: whether the magma with operation is a monoid, i.e. whether the operation is associative and has an identity.
*   :cpp:class:`math::is::semiring`: whether the magma is a semiring with multiplication and addition operators.

*   :cpp:class:`math::is::throw_if_undefined`: whether an operation throws if it is undefined on the arguments given.

Compile-time queries about nested call expressions:

*   :cpp:class:`math::has`: whether a nested operation is implemented for specific parameters.
*   :cpp:class:`math::result_of`: the result of a nested operation.
*   :cpp:class:`math::result_of_or`: the result of a nested operation, with back-off in case the operation is not implemented.
*   :cpp:class:`math::is::approximate`: whether the result of an operation is approximate.

Reference
---------

Compile-time helpers
^^^^^^^^^^^^^^^^^^^^

.. doxygenstruct:: math::left
.. doxygenstruct:: math::right
.. doxygenstruct:: math::either

.. doxygenstruct:: math::magma_tag
.. doxygenstruct:: math::is_magma

.. doxygenstruct:: math::merge_magma
.. doxygenstruct:: math::generalise_type

Operations on magmas
^^^^^^^^^^^^^^^^^^^^

.. doxygenvariable:: math::is_member
.. doxygenvariable:: math::equal
.. doxygenvariable:: math::not_equal
.. doxygenvariable:: math::approximately_equal

To support composite magmas (such as :cpp:class:`math::lexicographical`), different comparison operations are available.
:cpp:func:`math::compare`, if implemented, gives a strict weak ordering, and is often equaivalent to ``operator<``.
``order <...>`` also implements a comparison, which may be related to ``compare``, but this is not necessary.
``order <choose>``, if defined, returns ``true`` if the left value is better, in some sense, than the right value.
If it is defined, then :cpp:func:`math::choose` will return the better of two values.

Some magmas, ``cost``, for example, implement the same behaviour with the operation ``plus``.
``order <plus>`` then compares two values; ``plus`` itself returns the best (in this case, lowest-cost) of two.

.. doxygenvariable:: math::compare
.. doxygenfunction:: math::order

Producing a value
"""""""""""""""""

.. doxygenfunction:: math::non_member
.. doxygenfunction:: math::identity
.. doxygenfunction:: math::annihilator
.. doxygenfunction:: math::zero
.. doxygenfunction:: math::one

Binary operations
"""""""""""""""""

.. doxygenvariable:: math::pick
.. doxygenvariable:: math::choose
.. doxygenvariable:: math::times
.. doxygenvariable:: math::plus

.. doxygenfunction:: math::divide
.. doxygenfunction:: math::minus

Unary operations
""""""""""""""""

.. doxygenfunction:: math::invert
.. doxygenfunction:: math::reverse
.. doxygenvariable:: math::print

Operations on operations
^^^^^^^^^^^^^^^^^^^^^^^^

The following are operations on operations.

Run-time
""""""""

.. doxygenfunction:: math::inverse_operation

Compile-time queries about operations
"""""""""""""""""""""""""""""""""""""

.. doxygenstruct:: math::is::associative
.. doxygenstruct:: math::is::commutative
.. doxygenstruct:: math::is::idempotent
.. doxygenstruct:: math::is::path_operation
.. doxygenstruct:: math::is::distributive
.. doxygenstruct:: math::is::monoid
.. doxygenstruct:: math::is::semiring

Compile-time queries about nested call expressions
""""""""""""""""""""""""""""""""""""""""""""""""""

These are convenient ways of querying the availability or the results of an expression composed of magma operations.
Internally, this uses the :ref:`nested callable protocol <nested_callable>` for function objects.

.. doxygenstruct:: math::has
.. doxygenstruct:: math::result_of
.. doxygenstruct:: math::result_of_or
.. doxygenstruct:: math::is::approximate

Exception classes
^^^^^^^^^^^^^^^^^

The following types of exception can be thrown:

.. doxygenclass:: math::magma_not_convertible
.. doxygenclass:: math::operation_error
.. doxygenclass:: math::operation_undefined
.. doxygenclass:: math::inverse_of_annihilator
.. doxygenclass:: math::divide_by_zero
