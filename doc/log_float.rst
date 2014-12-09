.. _log-float:

*********
Log-float
*********

A class that represents numbers by their logarithms, so that (when used on top of floating-point numbers), they provide a large dynamic range.
The main class, :cpp:class:`math::log_float`, can only represent non-negative values; :cpp:class:`math::signed_log_float` uses an explicit sign.

Classes
=======

.. doxygenclass:: math::log_float
   :members:

.. doxygenclass:: math::signed_log_float
   :members:

.. doxygenclass:: math::as_exponent

Functions
=========

.. doxygenfunction:: math::exp_
.. doxygenfunction:: math::log
.. doxygenfunction:: math::exp
.. doxygenfunction:: math::pow
.. doxygenfunction:: math::sqrt

.. _Boost.Math: http://www.boost.org/libs/math/

