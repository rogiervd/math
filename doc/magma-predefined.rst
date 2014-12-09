.. _predefined_magmas:

Predefined magmas
=================

.. highlight:: cpp

This library predefines a number of magmas.
In some cases, this is just a wrapper around existing behaviour.

Arithmetic magma
----------------

.. highlight:: cpp

Trivial magmas are real and natural numbers.
To treat these as magmas, indeed, semirings, say ::

    #include "math/arithmetic_magma.hpp"

This will call an arithmetic magma, and allow operations and querying attributes, for which ``std::numeric_limits`` is specialized, except for ``bool``.
This is therefore implemented for :cpp:class:`math::log_float` as well.

Cost and max semirings
----------------------

The :cpp:class:`cost semiring <math::cost>` keeps track of costs that ``times`` adds up.
``plus`` selects the lowest-cost argument.

The :cpp:class:`max semiring <math::max_semiring>` in a sense is the opposite of the cost semiring: ``plus`` prefers the element with the greatest value.
The value could, for example, be a probability.

.. doxygenclass:: math::cost
    :members:

.. doxygenclass:: math::max_semiring
    :members:

Sequence semiring
-----------------

The sequence semiring represents a sequence of symbols.
Two sequences can be concatenated with ``times``.
The longest common prefix (or suffix) can be found with ``plus``.
(This seems a strange operation for ``plus`` but it is useful when moving symbols around finite-state automata.)

There are common use cases for, for example, empty sequences or single-symbol sequences.
To optimise code for these use cases, the semiring consists of five different classes.
Any sequence can be represented with an object of type :cpp:class:`math::sequence`.
All the other types are convertible to that class.
:cpp:class:`math::empty_sequence` always contains an empty sequence.
:cpp:class:`math::single_sequence` always contains a sequence of length 1.
:cpp:class:`math::optional_sequence` contains a sequence of length 0 or 1.
:cpp:class:`math::empty_sequence` and :cpp:class:`math::single_sequence` can always be converted to :cpp:class:`math::optional_sequence`.
:cpp:class:`math::sequence_annihilator` is a special symbol indicating the multiplicative annihilator, which is required for a semiring.
Operations such as ``plus``, ``times``, and ``divide`` return the appropriate type.

The ``Direction`` template parameter to each of the types can be ``left`` or ``right``.
It indicates whether the sequence forms a left or right semiring.
The operation ``plus`` on two elements of a left sequence semiring returns longest common prefix, the longest symbol sequence that both sequences start with.
On a right sequence semiring, the longest common suffix is taken, which is the longest symbol sequence that both sequences end with.

.. doxygenclass:: math::sequence
    :members:

.. doxygenclass:: math::empty_sequence
    :members:

.. doxygenclass:: math::single_sequence
    :members:

.. doxygenclass:: math::optional_sequence
    :members:

.. doxygenclass:: math::sequence_annihilator
    :members:

Composite magmas
----------------

Composite magmas are built out of other magmas.
The :cpp:class:`product magma <math::product>` is a general magma which applies all operations to each element.

The :cpp:class:`lexicographical semiring <math::lexicographical>` is a semiring for which ``plus`` chooses the first in a lexicographical ordering of the components.
It is not always necessary to care about the ordering of later components.
For example, the first component could be a cost, and the second component a word sequence.
This might be useful in finding the lowest-cost word sequence.

.. doxygenclass:: math::product
    :members:

.. doxygenclass:: math::lexicographical
    :members:

.. doxygenstruct:: math::over
