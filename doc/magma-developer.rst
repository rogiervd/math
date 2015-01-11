.. _magma_extending:

Extending the sublibrary
========================

.. highlight:: cpp

This section considers two ways of extending the sublibrary: by :ref:`implementing a new magma <magma_extending_magma>` and by :ref:`implementing a new operation <magma_extending_operation>`.

.. _magma_extending_magma:

Implementing a new magma
------------------------

A magma consists of one or more types that together form the magma.
(By using multiple types, it is possible to infer things about a value at compile time.)
All types of the magma must have the same magma tag.
This tag is used to forward to the implementation of the operations.
The structs, in namespace ``operation``, related to operations that apply to the new magma must be specialised.

If your magma consists of different types, you want to consider conversion between them.
If any value of one type is convertible into another type, that conversion should be implicit.
If no value of the type is convertible, the conversion does not have to be possible, but it will often be impossible to disable it at compile time, because the conversion will often happen through another type.
If not all values of the type are convertible, the conversion should be explicit, and if the value cannot be converted at run time, :cpp:class:`math::magma_not_convertible` should be thrown.

It is possible to use existing types as magmas.
For example, this is done for built-in floating-point types, and ``log_float``.

namespace math
^^^^^^^^^^^^^^

You probably want to specialise :cpp:class:`math::decayed_magma_tag` to declare your type as a magma.
Say that you want to implement a magma ``cost``::

    struct cost { /* Implementation */ };

    struct cost_tag;

    template <> struct decayed_magma_tag <cost>
    { typedef cost_tag type; };

The ``cost_tag`` type (which can remain undefined) is used to forward operations to the correct specialisation in :ref:`namespace operation <magma_extending_magma_operation>`.

.. doxygenstruct:: math::decayed_magma_tag
.. doxygenstruct:: math::magma_tag
.. doxygenstruct:: math::not_a_magma_tag

.. _magma_extending_magma_operation:

namespace operation
^^^^^^^^^^^^^^^^^^^

Operations on magmas are forwarded to structs in namespace ``operation`` using their magma tag.
The indirection leads to a slightly convoluted syntax, but it does mean that many :ref:`queries about operations <magma_operations_queries>` are automatically supported.
Say that you want to implement a magma ``cost``, which is a semiring which ``times`` implemented as addition.
The specialisation of ``times`` in namespace ``math::operation`` might look as follows::

    template <> struct times <cost_tag>
    : approximate
    {
        cost operator() (cost const & left, cost const & right) const
        { return cost (left.value() + right.value()); }
    };

This is a specialisation for ``cost_tag``, so when :cpp:type:`math::times` is called with arguments of type ``cost``, it knows to forward to this specialisation.
It defines an ``operator()`` which takes the left and right operand, and returns the result.
In general, the specialisation must have an operator() which takes any arguments that the specialisation is for.
If a multiple types constitute a magma, then the operation must be implemented for all of the types.
If there are three types, for example, the implementation of ``equal`` might have nine overloads for operator().

Note that ``times <cost_tag>`` derives from ``math::operation::approximate``.
This is an empty class that does not do anything, but deriving from it indicates that the operation is approximate.
:cpp:type:`math::is::approximate` uses this information.
If the operation is not approximate, don't derive from ``approximate``.
Some other properties that can be set by deriving from empty types, such as ``associative`` and ``idempotent``.

If an operation is not implemented, i.e. there is no specialisation, for a magma tag, then the default implementation is used.
This default implementation often derives from ``math::operation::unimplemented``.
:cpp:type:`math::has` uses this to figure out whether an operation is implemented in the same way as :cpp:type:`math::is::approximate` uses the base class.
Thus, the example of the specialisation above has a number of ripple effects.

All operations have an ``Enable`` template argument, which can be used to implement an operation only if a certain condition is satisfied.
For example, division is implemented for floating-point numbers but not for integers.
This could be done with a tag ``arithmetic_magma_tag <Type>`` which is templated on the actual type, so that the specialisation can be switched on or off depending on it::

    template <class Type> struct divide <arithmetic_magma_tag <Type>, either,
        typename std::enable_if <!std::numeric_limits <Type>::is_integer>::type>
    : operator_divide <Type>, approximate {};

You will notice that the implementation merely forwards to ``operator_divide``.
This is one of the :ref:`helper classes <magma_extending_magma_helper>` that forward to operators.

Consider implementing each of the following operations:

.. doxygenstruct:: math::operation::is_member
.. doxygenstruct:: math::operation::equal
.. doxygenstruct:: math::operation::not_equal
.. doxygenstruct:: math::operation::approximately_equal

.. doxygenstruct:: math::operation::compare
.. doxygenstruct:: math::operation::order

.. doxygenstruct:: math::operation::non_member
.. doxygenstruct:: math::operation::identity
.. doxygenstruct:: math::operation::annihilator

.. doxygenstruct:: math::operation::pick
.. doxygenstruct:: math::operation::choose
.. doxygenstruct:: math::operation::times
.. doxygenstruct:: math::operation::plus
.. doxygenstruct:: math::operation::divide
.. doxygenstruct:: math::operation::minus

.. doxygenstruct:: math::operation::invert
.. doxygenstruct:: math::operation::reverse
.. doxygenstruct:: math::operation::print


.. _magma_extending_magma_helper:

Helpers for implementing operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenstruct:: math::operation::choose_by_order
.. doxygenstruct:: math::operation::reverse_order

If the type already exists and has operators defined, then predefined structs in namespace ``math::operation`` can be used straightforwardly.
For example, ``arithmetic_magma.hpp`` could define ``equal`` and ``times`` as follows::

    // Forward to operator==.
    template <class Type> struct equal <arithmetic_magma_tag <Type>>
    : operator_equal <bool> {};

    // Use multiple inheritance.
    template <class Type> struct times <arithmetic_magma_tag <Type>>
    // Forward to operator*.
    : operator_times <Type>,
    // Mark as approximate if the type is floating-point.
        approximate_if <boost::mpl::bool_ <
            !std::numeric_limits <Type>::is_exact>> {};

In the opposite case, where you have defined operations in namespace ``math::operation``, and would like to automatically generate operators, you can use ``MATH_MAGMA_GENERATE_OPERATORS``:

.. doxygendefine:: MATH_MAGMA_GENERATE_OPERATORS

Boolean properties
^^^^^^^^^^^^^^^^^^

The properties of a magma are set in namespace ``math::operation``.
This should be straightforward; namespace ``math::is`` tries to conclude things from this.

Setting properties while specialising operations
""""""""""""""""""""""""""""""""""""""""""""""""

The easiest way of setting these properties is often to derive an operation from an empty base class, or multiple base classes, that are automatically detected.

.. doxygenstruct:: math::operation::approximate
.. doxygenstruct:: math::operation::associative
.. doxygenstruct:: math::operation::commutative
.. doxygenstruct:: math::operation::idempotent
.. doxygenstruct:: math::operation::path_operation

.. doxygenstruct:: math::operation::throw_if_undefined

In some cases, you may want to derive from these conditionally.
This can be done by deriving from the following.

.. doxygenstruct:: math::operation::approximate_if
.. doxygenstruct:: math::operation::associative_if
.. doxygenstruct:: math::operation::commutative_if
.. doxygenstruct:: math::operation::idempotent_if
.. doxygenstruct:: math::operation::path_operation_if

If an operation is not approximate/associative/and so on, the above will derive
from

.. doxygenstruct:: math::operation::ignorable_base_class

Setting properties explicitly
"""""""""""""""""""""""""""""

It is also possible to set properties explicitly, by specialising a class with a name starting with ``is_``.
For example, if the ``math::operation::is_distributive <either, ...>`` is ``true``, then ``math::is::distributive <left, ...>`` will be true.
Specialising a property usually means deriving it from ``boost::mpl::true_``.
Sometimes a more complicated expression is necessary.
Properties are mostly false by default (if an operation is not derived from one of the classes above), so they do not have to be specialised if a magma does not have a certain property.

Two properties that concern multiple operations are whether an operation distributes over another, and whether a magma is a semiring (with two operations).
That must be indicated by explicitly setting one of these:

.. doxygenstruct:: math::operation::is_distributive
.. doxygenstruct:: math::operation::is_semiring

The following properties can be specialised explicitly, but their default implementations will detect automatically when operations derive from the base classes above.

.. doxygenstruct:: math::operation::is_approximate
.. doxygenstruct:: math::operation::is_associative
.. doxygenstruct:: math::operation::is_commutative
.. doxygenstruct:: math::operation::is_idempotent
.. doxygenstruct:: math::operation::is_path_operation

Type property
^^^^^^^^^^^^^

.. doxygenstruct:: math::operation::unify_type

Checking
^^^^^^^^

Magmas (and semirings) can be tested.
The properties must be set up (and checked) in advance, examples must be produced, and then the following functions can be used:

.. doxygenfunction:: math::check_equal_on
.. doxygenfunction:: math::check_magma(Operation, Examples const &)
.. doxygenfunction:: math::check_magma(Operation1, Operation2, Examples const &)
.. doxygenfunction:: math::check_semiring

Useful classes
--------------

.. doxygenstruct:: math::is_direction
.. doxygenstruct:: math::operation::unimplemented
.. doxygenstruct:: math::operation::is_implemented

.. _magma_extending_operation:

Implementing a new operation
----------------------------

An operation has a function or callable static object, and a type with an ``operator()`` in namespace ``math::callable``.
For interaction with magmas, the type must completely define the operation.
This way, for example, saying ``identity <double, callable::plus>()`` is possible: ``callable::plus`` completely defines the operation.

The implementation of operations comes in a number of parts, which are in different namespaces.
The namespace ``math::operation`` has been discussed above.
Structs in this namespace are inherited by types in namespace ``math::apply``, which are instantiated by types in ``math::callable``, which are used in namespace ``math``.
The following details these.

namespace apply
^^^^^^^^^^^^^^^

Each Type in namespace ``math::apply`` deals with compile-time and run-time arguments and forwards them to the struct with the same name in namespace ``operation``.
The run-time arguments will often, but not always, be passed through ``std::decay``.
structs in ``operation`` are normally parameterised by the magma tag.
For example, the standard operation ``identity`` could have a definition in namespace ``apply`` like::

    template <class ...> struct identity;

    // The actual implementation specialises the variadic declaration.
    template <class Magma, class Operation> struct identity <Magma, Operation>
    : operation::identity <
        typename magma_tag <Magma>::type,
        typename std::decay <Operation>::type> {};

For some compilers (GCC 4.6) it makes integration into callables easier to have variadic type arguments, but this is by no means mandated.
If there are multiple parameters that must be of the same magma, ``math::apply::detail::magma_tag_all <Magma1, Magma2>`` can be instead of ``magma_tag <Magma>``.

To allow the nested call protocol to work, the class must derive from ``operation::unimplemented`` if unimplemented.
(It can additionally derive from ``operation::approximate`` if approximate, from ``operation::associative`` if associative, et cetera.)
This is often easy to do by always deriving from the appropriate type in namespace ``operation``.

It is possible to forward calls to different operations in this namespace, as long as their ``operator()`` takes the same arguments.
For example, the definition of ``one`` could be::

    template <class Magma> struct one <Magma>
    : identity <Magma, callable::times> {};

.. _nested_callable:

namespace callable
^^^^^^^^^^^^^^^^^^

The namespace ``callable`` contains callable types.
Types in this namespace can have template arguments.
For example::

    template <class Magma> struct one;

``non_member`` must then have an ``operator()``, which in this case takes zero arguments.
In general, it constructs an object of the appropriate type from namespace ``apply`` and then forwards the arguments to it.
Additionally, to adhere to the *nested callable protocol*, information must be provided at compile time.
Any callable must have a class or struct ``apply <RunTimeArguments ...>`` deriving from ``apply::function``.
This allows such functionality as :cpp:class:`math::has` and :cpp:class:`math::is::approximate` to work.
This is easy with
::

    struct times : generic <apply::times> {};

or, if the function has one or more template arguments:
::

    template <class Magma> struct one : generic <apply::one, Magma> {};

.. doxygenstruct:: math::callable::generic

namespace math
^^^^^^^^^^^^^^

Finally, the namespace ``::math`` itself contains things that can be called.
These can be either a function, or a function object.
For example::

    static const auto is_member = callable::is_member();

If the callable type is templated, the thing in namespace ``::math`` cannot be a function object, so::

    template <class Magma> inline auto non_member()
        RETURNS (callable::non_member <Magma>()());

Now, your new operation can be tested!
