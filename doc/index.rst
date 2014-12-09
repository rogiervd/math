**************************
Mathematical tools library
**************************

This library contains mathematical tools.
The library uses and complements the `Boost.Math`_ library.

Parts of this library are

1.  :ref:`log-float`
    A class that represents numbers by their logarithms, so that (when used on top of floating-point numbers), they provide a large dynamic range.
    There is a class that can represent only non-negative values, and one that adds a sign bit.

2.  :ref:`Magma <magma>`
    A magma is a set that has a binary operation that returns a value in the set.
    For example, the set of real numbers with the times operation is a magma.
    This can be extended to semirings and groups and whatnots.
    This sublibrary makes it possible to turn any class into a magma, and provides a number of predefined ones.
    For example, standard floating-point number types, and :ref:`log-float`, are defined as magmas.

.. toctree::
    :maxdepth: 2

    log_float.rst
    magma.rst

.. _Boost.Math: http://www.boost.org/libs/math/
