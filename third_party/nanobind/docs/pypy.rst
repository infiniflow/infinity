:orphan:

.. cpp:namespace:: nanobind

.. _pypy_issues:

PyPy support
------------

PyPy 7.3.10 and newer versions are supported, though with some limitations:

1. When nanobind types occur in reference cycles, then those cycles are not
   collectable and will be leaked. This is a limitation of PyPy's ``cpyext``
   layer that was reported in `PyPy issue #3849
   <https://foss.heptapod.net/pypy/pypy/-/issues/3849>`_. Note that this would
   only affects users that use the :cpp:class:`nb::type_slots() <type_slots>`
   feature to implement a custom ``Py_tp_traverse`` or ``Py_tp_clear`` slot.

2. nanobind normally complains about any reference leaks involving instances,
   functions, and types when the interpreter shuts down. PyPy lacks the final
   garbage collection step that is needed to identify such leaks See `PyPy
   issue #3855 <https://foss.heptapod.net/pypy/pypy/-/issues/3855>`_. Those
   those checks therefore had to be disabled when compiling for PyPy.
