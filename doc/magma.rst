.. _magma:

*****
Magma
*****

.. highlight:: cpp

A magma is a set that has a binary operation that returns a value in the set and possibly has an annihilator (see `Wikipedia`_).
To a programmer, it looks like a class definition with some abstract methods, but then defined by mathematicians.
A *semiring* can be a useful concept, for example in finite-state automata.
A semiring is a magma with a plus operation and a times operation that adheres to certain requirements.

This library uses a few concepts.
They are explained below using ``double`` as an example, but the point is of course that this can be extended to user-defined types.

First, a binary operation must be defined by a type.
This type must be default-constructible.
Examples are :cpp:class:`math::callable::times` and :cpp:class:`math::callable::plus`.
For convenience, static variables of these classes are ``math::times`` and ``math::plus``.
After ``#include "math/arithmetic_magma.hpp"``, this should compile:

.. literalinclude:: code_sample/magma/callable.ipp

You may think that this does not give much of an advantage over using operators that are built-in or overloaded.
However, read on.

An operation can have an identity element.
This is an element that when the operation is applied to it and another element, the result is equal to the other element.
For example, multiplying any number by ``1`` yields the original number.
Similarly, ``0`` is the identity element for addition.
This should compile:

.. literalinclude:: code_sample/magma/zero.ipp

``math::zero`` and ``math::one`` are shorthands for the identity of plus and times.

An operation can have an inverse operation.
This inverse of an operation is the binary operation that undoes it.
For example, since ``2. * 3.`` equals ``6.``:

.. literalinclude:: code_sample/magma/inverse_operation.ipp

``math::inverse_operation (math::times)`` simply returns an object of type :cpp:class:`math::callable::divide`.
Note that this is possible for real numbers.
However, for some operations on some classes, the order of the operands makes a difference.
That is, the operation is not *commutative*.
An example is the :cpp:class:`sequence semiring <math::callable::sequence>`, which is the set of sequences that are concatenated by ``operator*``.
Division removes elements from either the front or the back, which is *left division* or *right division*.
It is possible to differentiate between left and right division with the tag classes :cpp:class:`math::left` and :cpp:class:`math::right`.
For example,

.. literalinclude:: code_sample/magma/left_right.ipp

For real numbers, left and right division perform the same operation, but for sequence semirings they are different.
:cpp:struct:`math::either`, the default argument for direction, indicates that they are the same.

Another use of function classes is to query properties.
Thus,

.. literalinclude:: code_sample/magma/properties.ipp

Notice that a cute syntax is used where classes in namespace ``math::callable`` are used to indicate function calls.
This goes for ``math::has`` and ``math::result_of`` as well.
Internally, this uses the :ref:`nested callable protocol <nested_callable>` for function objects.

All these properties must be implemented explicitly.
To help with this, such functions as :cpp:func:`math::check_semiring` exist (after saying ``#include "math/check/check_magma.hpp"``).
This checks, on a list of examples, that ``float`` is indeed a semiring:

.. literalinclude:: code_sample/magma/check_semiring.ipp

Functions in namespace ``math`` work only on one magma.
E.g. ``math::times (3., 8.f)``, which tries to multiple a ``double`` with a ``float``, does not work.
This does not mean that arguments must be of the same type.
For example, the :cpp:class:`sequence semiring <math::sequence>` can distinguish at compile time between sequences with zero, one, or a variable number of elements.
Any of the types that are in one magma can be used with a binary operation.

Different magma types can often be converted into each other.
If this conversion always succeeds, it is implicit.
If its success depends on the run-time value, then the conversion is explicit, and will throw :cpp:class:`math::magma_not_convertible` if it is not.
(For most types of magma, it is impossible to disable at compile time even transparently impossible conversions, since the compiler will go through an implicit conversion to access the explicit constructor.)

.. toctree::
    :maxdepth: 2

    magma-operations
    magma-predefined
    magma-developer

.. _Wikipedia: http://en.wikipedia.org/wiki/Magma_(algebra)
