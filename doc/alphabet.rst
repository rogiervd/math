.. _alphabet:

Alphabet
========

An alphabet is a set of symbols.
In theory, any object represented in a binary form can be seen as in a set of symbols, with all possible valid bit patterns in the set.
The :cpp:class:`math::alphabet` class represents a bounded set of symbols explicitly.
This makes it possible to

1.  check whether two objects are in the same set, and whether an element supposed to be in the set actually is; and
2.  to represent elements more tersely, which improves performance.
    :cpp:class:`math::alphabet` represents symbols internally by integers.

There is an additional twist to :cpp:class:`math::alphabet`.
The symbols can be of two types:

*   A "normal symbol", which is a value of the main symbol type.
    For example, this may be of type ``std::string``.
    New symbols are added with method :cpp:func:`math::alphabet::add_symbol`.

*   A "special symbol", which is of an empty type other than the symbol type.
    An "empty" type has no value, but any two objects of such a type must compare equal.
    "Must compare equal" means that the type that ``operator==`` returns is a compile-time constant with value ``true``.
    New special symbols can be added with free function (!) :cpp:func:`math::add_special_symbol`.

Classes
^^^^^^^

.. doxygenclass:: math::alphabet
    :members:

.. doxygenclass:: math::dense_symbol
    :members:

.. doxygenfunction:: math::add_special_symbol

.. doxygenclass:: math::alphabet_overflow
    :members:

.. doxygenclass:: math::symbol_not_found
    :members:

.. doxygenclass:: math::symbol_not_found_of
    :members:
