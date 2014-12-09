**************************
Mathematical tools library
**************************

This library contains mathematical tools.
The library uses and complements the `Boost.Math`_ library.

Parts of this library are

1.  :ref:`log-float`
    A class that represents numbers by their logarithms, so that (when used on top of floating-point numbers), they provide a large dynamic range.
    There is a class that can represent only non-negative values, and one that adds a sign bit.

.. toctree::
    :maxdepth: 2

    log_float.rst

.. _Boost.Math: http://www.boost.org/libs/math/
