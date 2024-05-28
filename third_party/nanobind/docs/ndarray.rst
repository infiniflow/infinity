.. cpp:namespace:: nanobind

.. _ndarray_class:

The ``nb::ndarray<..>`` class
=============================

nanobind can exchange n-dimensional arrays (henceforth "**ndarrays**") with
popular array programming frameworks including `NumPy
<https://numpy.org>`__, `PyTorch <https://pytorch.org>`__, `TensorFlow
<https://www.tensorflow.org>`__, `JAX <https://jax.readthedocs.io>`__, and `CuPy <https://cupy.dev>`_.
It supports *zero-copy* exchange using two protocols:

-  The classic `buffer
   protocol <https://docs.python.org/3/c-api/buffer.html>`__.

-  `DLPack <https://github.com/dmlc/dlpack>`__, a
   GPU-compatible generalization of the buffer protocol.

nanobind knows how to talk to each framework and takes care
of all the nitty-gritty details.

To use this feature, you must add the include directive

.. code-block:: cpp

   #include <nanobind/ndarray.h>

to your code. Following this, you can bind functions with
:cpp:class:`nb::ndarray\<...\> <ndarray>`-typed parameters and return values.

Binding functions that take arrays as input
-------------------------------------------

A function that accepts a :cpp:class:`nb::ndarray\<\> <ndarray>`-typed parameter
(i.e., *without* template parameters) can be called with *any* array
from any framework regardless of the device on which it is stored. The
following example binding declaration uses this functionality to inspect the
properties of an arbitrary input array:

.. code-block:: cpp

   m.def("inspect", [](nb::ndarray<> a) {
       printf("Array data pointer : %p\n", a.data());
       printf("Array dimension : %zu\n", a.ndim());
       for (size_t i = 0; i < a.ndim(); ++i) {
           printf("Array dimension [%zu] : %zu\n", i, a.shape(i));
           printf("Array stride    [%zu] : %zd\n", i, a.stride(i));
       }
       printf("Device ID = %u (cpu=%i, cuda=%i)\n", a.device_id(),
           int(a.device_type() == nb::device::cpu::value),
           int(a.device_type() == nb::device::cuda::value)
       );
       printf("Array dtype: int16=%i, uint32=%i, float32=%i\n",
           a.dtype() == nb::dtype<int16_t>(),
           a.dtype() == nb::dtype<uint32_t>(),
           a.dtype() == nb::dtype<float>()
       );
   });

Below is an example of what this function does when called with a NumPy
array:

.. code-block:: pycon

   >>> my_module.inspect(np.array([[1,2,3], [3,4,5]], dtype=np.float32))
   Array data pointer : 0x1c30f60
   Array dimension : 2
   Array dimension [0] : 2
   Array stride    [0] : 3
   Array dimension [1] : 3
   Array stride    [1] : 1
   Device ID = 0 (cpu=1, cuda=0)
   Array dtype: int16=0, uint32=0, float32=1

Array constraints
-----------------

In practice, it can often be useful to *constrain* what kinds of arrays
constitute valid inputs to a function. For example, a function expecting CPU
storage would likely crash if given a pointer to GPU memory, and nanobind
should therefore prevent such undefined behavior.
:cpp:class:`nb::ndarray\<...\> <ndarray>` accepts template arguments to
specify such constraints. For example the function interface below
guarantees that the implementation is only invoked when it is provided with
a ``MxNx3`` array of 8-bit unsigned integers.

.. code-block:: cpp

   m.def("process", [](nb::ndarray<uint8_t, nb::shape<-1, -1, 3>,
                                   nb::device::cpu> data) {
       // Double brightness of the MxNx3 RGB image
       for (size_t y = 0; y < data.shape(0); ++y)
           for (size_t x = 0; x < data.shape(1); ++x)
               for (size_t ch = 0; ch < 3; ++ch)
                   data(y, x, ch) = (uint8_t) std::min(255, data(y, x, ch) * 2);

   });

The above example also demonstrates the use of
:cpp:func:`nb::ndarray\<...\>::operator() <ndarray::operator()>`, which
provides direct read/write access to the array contents. Note that this
function is only available when the underlying data type and ndarray dimension
are specified via the :cpp:type:`ndarray\<..\> <ndarray>` template parameters.
It should only be used when the array storage is accessible through the CPU's
virtual memory address space.

.. _ndarray-constraints-1:

Constraint types
----------------

The following constraints are available

- A scalar type (``float``, ``uint8_t``, etc.) constrains the representation
  of the ndarray.

  Complex arrays (e.g., based on ``std::complex<float>`` or
  ``std::complex<double>``) are supported.

- This scalar type can be further annotated with ``const``, which is necessary
  if you plan to call nanobind functions with arrays that do not permit write
  access.

- The :cpp:class:`nb::shape <shape>` annotation (as in ``nb::shape<-1,
  3>``) simultaneously constrains the number of array dimensions and the size
  per dimension. A value of ``-1`` leaves the corresponding
  dimension unconstrained.

  :cpp:class:`nb::ndim <ndim>` is shorter to write when only the dimension
  should be constrained. For example, ``nb::ndim<3>`` is equivalent to
  ``nb::shape<-1, -1, -1>``.

- Device tags like :cpp:class:`nb::device::cpu <device::cpu>` or
  :cpp:class:`nb::device::cuda <device::cuda>` constrain the source device
  and address space.

- Two ordering tags :cpp:class:`nb::c_contig <c_contig>` and
  :cpp:class:`nb::f_contig <f_contig>` enforce contiguous storage in either
  C or Fortran style. In the case of matrices, C-contiguous corresponds to
  row-major storage, and F-contiguous corresponds to column-major storage.
  Without this tag, non-contiguous representations (e.g. produced by slicing
  operations) and other unusual layouts are permitted.

  This tag is mainly useful when directly accessing the array contents via
  :cpp:func:`nb::ndarray\<...\>::data() <ndarray::data>`.

Passing arrays in C++ code
--------------------------

:cpp:class:`nb::ndarray\<...\> <ndarray>` behaves like a shared pointer with
builtin reference counting: it can be moved or copied within C++ code.
Copies will point to the same underlying buffer and increase the reference
count until they go out of scope. It is legal call
:cpp:class:`nb::ndarray\<...\> <ndarray>` members from multithreaded code even
when the `GIL <https://wiki.python.org/moin/GlobalInterpreterLock>`__ is not
held.

.. _ndarray-views:

Fast array views
----------------

The following advice applies to performance-sensitive CPU code that reads and
writes arrays using loops that invoke :cpp:func:`nb::ndarray\<...\>::operator()
<ndarray::operator()>`. It does not apply to GPU arrays because they are
usually not accessed in this way.

Consider the following snippet, which fills a 2D array with data:

.. code-block:: cpp

   void fill(nb::ndarray<float, nb::ndim<2>, nb::c_contig, nb::device::cpu> arg) {
       for (size_t i = 0; i < arg.shape(0); ++i)
           for (size_t j = 0; j < arg.shape(1); ++j)
               arg(i, j) = /* ... */;
   }

While functional, this code is not perfect. The problem is that to compute the
address of an entry, ``operator()`` accesses the DLPack array descriptor. This
indirection can break certain compiler optimizations.

nanobind provides the method :cpp:func:`ndarray\<...\>::view() <ndarray::view>`
to fix this. It creates a tiny data structure that provides all information
needed to access the array contents, and which can be held within CPU
registers. All relevant compile-time information (:cpp:class:`nb::ndim <ndim>`,
:cpp:class:`nb::shape <shape>`, :cpp:class:`nb::c_contig <c_contig>`,
:cpp:class:`nb::f_contig <f_contig>`) is materialized in this view, which
enables constant propagation, auto-vectorization, and loop unrolling.

An improved version of the example using such a view is shown below:

.. code-block:: cpp

   void fill(nb::ndarray<float, nb::ndim<2>, nb::c_contig, nb::device::cpu> arg) {
       auto v = arg.view(); // <-- new!

       for (size_t i = 0; i < v.shape(0); ++i) // Important; use 'v' instead of 'arg' everywhere in loop
           for (size_t j = 0; j < v.shape(1); ++j)
               v(i, j) = /* ... */;
   }

Note that the view performs no reference counting. You may not store it in a way
that exceeds the lifetime of the original array.

When using OpenMP to parallelize expensive array operations, pass the
``firstprivate(view_1, view_2, ...)`` so that each worker thread can copy the
view into its register file.

.. code-block:: cpp

   auto v = arg.view();
   #pragma omp parallel for schedule(static) firstprivate(v)
   for (...) { /* parallel loop */ }

.. _ndarray-runtime-specialization:

Specializing views at runtime
-----------------------------

As mentioned earlier, element access via ``operator()`` only works when both
the array's scalar type and its dimension are specified within the type (i.e.,
when they are known at compile time); the same is also true for array views.
However, sometimes, it is useful that a function can be called with different
array types.

You may use the :cpp:func:`ndarray\<...\>::view() <ndarray::view>` method to
create *specialized* views if a run-time check determines that it is safe to
do so. For example, the function below accepts contiguous CPU arrays and
performs a loop over a specialized 2D ``float`` view when the array is of
this type.

.. code-block:: cpp

   void fill(nb::ndarray<nb::c_contig, nb::device::cpu> arg) {
       if (arg.dtype() == nb::dtype<float>() && arg.ndim() == 2) {
           auto v = arg.view<float, nb::ndim<2>>(); // <-- new!

           for (size_t i = 0; i < v.shape(0); ++i) {
               for (size_t j = 0; j < v.shape(1); ++j) {
                   v(i, j) = /* ... */;
               }
           }
        } else { /* ... */ }
   }

Constraints in type signatures
------------------------------

nanobind displays array constraints in docstrings and error messages. For
example, suppose that we now call the ``process()`` function with an invalid
input. This produces the following error message:

.. code-block:: pycon

   >>> my_module.process(ndarray=np.zeros(1))

   TypeError: process(): incompatible function arguments. The following argument types are supported:
   1. process(arg: ndarray[dtype=uint8, shape=(*, *, 3), order='C', device='cpu'], /) -> None

   Invoked with types: numpy.ndarray

Note that these type annotations are intended for humans–they will not
currently work with automatic type checking tools like `MyPy
<https://mypy.readthedocs.io/en/stable/>`__ (which at least for the time being
don’t provide a portable or sufficiently flexible annotation of n-dimensional
arrays).

Arrays and function overloads
-----------------------------

A bound function taking an ndarray argument can declare multiple overloads
with different constraints (e.g., a CPU and GPU implementation), in which
case the first matching overload will be called. When no perfect
match can be found, nanobind will try each overload once more while
performing basic implicit conversions: it will convert strided arrays
into C- or F-contiguous arrays (if requested) and perform type
conversion. This, e.g., makes it possible to call a function expecting a
``float32`` array with ``float64`` data. Implicit conversions create
temporary ndarrays containing a copy of the data, which can be
undesirable. To suppress them, add an
:cpp:func:`nb::arg("my_array_arg").noconvert() <arg::noconvert>`
or
:cpp:func:`"my_array_arg"_a.noconvert() <arg::noconvert>`
function binding annotation.

Binding functions that return arrays
------------------------------------

To return an ndarray from C++ code, you must indicate its type, shape, a
pointer to CPU/GPU memory, the *owner* of that data, and what framework
(NumPy/..) should be used to encapsulate the array data.

The following simple binding declaration shows how to return a static ``2x4``
NumPy floating point matrix that does not permit write access.

.. code-block:: cpp

   // at top level
   const float data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

   NB_MODULE(my_ext, m) {
       m.def("ret_numpy", []() {
           size_t shape[2] = { 2, 4 };
           return nb::ndarray<nb::numpy, const float, nb::shape<2, -1>>(
               /* data = */ data,
               /* ndim = */ 2,
               /* shape pointer = */ shape,
               /* owner = */ nb::handle());
       });
    }

In this example, ``data`` is a global constant stored in the program's data
segment, which means that it will never be deleted. In this special case, it is
valid to specify a null owner (``nb::handle()``).

In general, the ``owner`` argument must be specify a Python object, whose
continued existence keeps the underlying memory region alive. If your ndarray
bindings lead to undefined behavior (data corruption or crashes), then this is
usually an issue related to incorrect data ownership. Please review the section
on :ref:`data ownership <ndarray-ownership>` for further examples.

The auto-generated docstring of this function is:

.. code-block:: python

   ret_pytorch() -> np.ndarray[float32, writable=False, shape=(2, *)]

Calling it in Python yields

.. code-block:: python

   array([[1., 2., 3., 4.],
          [5., 6., 7., 8.]], dtype=float32)

The following additional ndarray declarations are possible for return
values:

-  :cpp:class:`nb::numpy <numpy>`. Returns the ndarray as a ``numpy.ndarray``.
-  :cpp:class:`nb::pytorch <pytorch>`. Returns the ndarray as a ``torch.Tensor``.
-  :cpp:class:`nb::tensorflow <tensorflow>`. Returns the ndarray as a
   ``tensorflow.python.framework.ops.EagerTensor``.
-  :cpp:class:`nb::jax <jax>`. Returns the ndarray as a
   ``jaxlib.xla_extension.DeviceArray``.
-  :cpp:class:`nb::cupy <cupy>`. Returns the ndarray as a ``cupy.ndarray``.
-  No framework annotation. In this case, nanobind will return a raw
   Python ``dltensor``
   `capsule <https://docs.python.org/3/c-api/capsule.html>`__
   representing the `DLPack <https://github.com/dmlc/dlpack>`__
   metadata.

When returning arrays, nanobind will not perform implicit conversions. Shape
and order annotations like :cpp:class:`nb::shape <shape>`, :cpp:class:`nb::ndim
<ndim>`, :cpp:class:`nb::c_contig <c_contig>`, and :cpp:class:`nb::f_contig
<f_contig>`, are shown in the docstring, but nanobind won't check that they are
actually satisfied. It will never convert an incompatible result into the right
format.

Furthermore, non-CPU nd-arrays must be explicitly indicate the
device type and device ID using special parameters of the :cpp:func:`ndarray()
<ndarray::ndarray()>` constructor shown below. Device types indicated via
template arguments, e.g., ``nb::ndarray<..., nb::device::cuda>``, are only used
for decorative purposes to generate an informative function docstring.

The full signature of the ndarray constructor is:

.. code-block:: cpp

   ndarray(void *data,
           size_t ndim,
           const size_t *shape,
           handle owner,
           const int64_t *strides = nullptr,
           dlpack::dtype dtype = nb::dtype<Scalar>(),
           int32_t device_type = nb::device::cpu::value,
           int32_t device_id = 0) { .. }

If no ``strides`` parameter is provided, the implementation will assume a
C-style ordering. Both ``strides`` and ``shape`` will be copied by the
constructor, hence the targets of these pointers don't need to remain valid
following the call.

An alternative form of the constructor takes ``std::initializer_list`` instead
of shape/stride arrays for brace-initialization and infers ``ndim``:

.. code-block:: cpp

   ndarray(void *data,
           std::initializer_list<size_t> shape,
           handle owner,
           st::initializer_list<int64_t> strides = { },
           dlpack::dtype dtype = nb::dtype<Scalar>(),
           int32_t device_type = nb::device::cpu::value,
           int32_t device_id = 0) { .. }

.. _ndarray-ownership:

Data ownership
--------------

The ``owner`` argument of the :cpp:class:`ndarray`` constructor must specify a
Python object that keeps the underlying memory region alive.

A common use case entails returning an nd-array view of an existing C++
container. In this case, you could construct a :cpp:class:`nb::capsule
<capsule>` to take ownership of this container. A capsule is an opaque pointer
with a destructor callback. In this case, its destructor would call the
C++ ``delete`` operator. An example is shown below:

.. code-block:: cpp

   m.def("ret_pytorch", []() {
       // Dynamically allocate 'data'
       float *data = new float[8] { 1, 2, 3, 4, 5, 6, 7, 8 };

       // Delete 'data' when the 'owner' capsule expires
       nb::capsule owner(data, [](void *p) noexcept {
          delete[] (float *) p;
       });

       return nb::ndarray<nb::pytorch, float>(data, { 2, 4 }, owner);
   });

In method bindings, you can use the
:cpp:enumerator:`rv_policy::reference_internal` return value policy to set the
owner to the ``self`` argument of the method so that the nd-array will keep the
associated Python/C++ instance alive. It is fine to specify a null owner in
this case.

.. code-block:: cpp

   struct Vector {
       float pos[3];
   };

   nb::class_<Vector>(m, "Vector")
      .def("pos",
           [](Vector &v) {
               return nb::ndarray<nb::numpy, float>(
                   /* data = */ v.pos,
                   /* shape = */ { 3 },
                   /* owner = */ nb::handle()
               );
           }, nb::rv_policy::reference_internal);

In other situations, it may be helpful to have a capsule that manages the
lifetime of data structures containing *multiple* containers. The same capsule
can be referenced from different nd-arrays and will call the deleter when all
of them have expired:

.. code-block:: cpp

   m.def("return_multiple", []() {
       struct Temp {
           std::vector<float> vec_1;
           std::vector<float> vec_2;
       };

       Temp *temp = new Temp();
       temp->vec_1 = std::move(...);
       temp->vec_2 = std::move(...);

       nb::capsule deleter(temp, [](void *p) noexcept {
           delete (Temp *) p;
       });

       size_t size_1 = temp->vec_1.size();
       size_t size_2 = temp->vec_2.size();

       return std::make_pair(
           nb::ndarray<nb::pytorch, float>(temp->vec_1.data(), { size_1 }, deleter),
           nb::ndarray<nb::pytorch, float>(temp->vec_2.data(), { size_2 }, deleter)
       );
   });

Return value policies
---------------------

Function bindings that return nd-arrays admit additional return value policy
annotations to determine whether or not a copy should be made. They are
interpreted as follows:

- :cpp:enumerator:`rv_policy::automatic` causes the array to be copied when it
  has no owner and when it is not already associated with a Python object.

- :cpp:enumerator:`rv_policy::automatic_reference` and
  :cpp:enumerator:`rv_policy::reference`
  ``automatic_reference`` and ``reference`` never copy.

- :cpp:enumerator:`rv_policy::copy` always copies.

- :cpp:enumerator:`rv_policy::none` refuses the cast unless the array is
  already associated with an existing Python object (e.g. a NumPy array), in
  which case that object is returned.

- :cpp:enumerator:`rv_policy::reference_internal` retroactively sets the
  ndarray's ``owner`` field to a method's ``self`` argument. It fails with an
  error if there is already a different owner.

- :cpp:enumerator:`rv_policy::move` is unsupported and demoted to
  :cpp:enumerator:`rv_policy::copy`.

.. _ndarray-nonstandard:

Nonstandard arithmetic types
----------------------------

Low or extended-precision arithmetic types (e.g., ``int128``, ``float16``,
``bfloat``) are sometimes used but don't have standardized C++ equivalents. If
you wish to exchange arrays based on such types, you must register a partial
overload of ``nanobind::ndarray_traits`` to inform nanobind about it.

For example, the following snippet makes ``__fp16`` (half-precision type on
``aarch64``) available:

.. code-block:: cpp

   namespace nanobind {
       template <> struct ndarray_traits<__fp16> {
           static constexpr bool is_complex = false;
           static constexpr bool is_float   = true;
           static constexpr bool is_bool    = false;
           static constexpr bool is_int     = false;
           static constexpr bool is_signed  = true;
       };
   }

Frequently asked questions
--------------------------

Why does nanobind not accept my NumPy array?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When binding a function that takes an ``nb::ndarray<T, ...>`` as input, nanobind
will by default require that array to be writable. This means that the function
cannot be called using NumPy arrays that are marked as constant.

If you wish your function to be callable with constant input, either change the
parameter to ``nb::ndarray<const T, ...>`` (if the array is parameterized by
type), or write ``nb::ndarray<nb::ro>`` to accept a read-only array of any
type.

Limitations related to ``dtypes``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _dtype_restrictions:

Libraries like `NumPy <https://numpy.org>`__ support arrays with flexible
internal representations (*dtypes*), including

- Floating point and integer arrays with various bit depths

- Null-terminated strings

- Arbitrary Python objects

- Heterogeneous data structures composed of multiple fields

nanobind's :cpp:class:`nb::ndarray\<...\> <ndarray>` is based on the `DLPack
<https://github.com/dmlc/dlpack>`__ array exchange protocol, which causes it to
be more restrictive. Presently supported dtypes include signed/unsigned
integers, floating point values, and boolean values. Some :ref:`nonstandard
arithmetic types <ndarray-nonstandard>` can be supported as well.

Nanobind can receive and return read-only arrays via the buffer protocol used
to exchange data with NumPy. The DLPack interface currently ignores this
annotation.
