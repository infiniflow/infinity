import test_ndarray_ext as t
import pytest
import warnings
import importlib
from common import collect

try:
    import numpy as np
    def needs_numpy(x):
        return x
except:
    needs_numpy = pytest.mark.skip(reason="NumPy is required")

try:
    import torch
    def needs_torch(x):
        return x
except:
    needs_torch = pytest.mark.skip(reason="PyTorch is required")

try:
    import tensorflow as tf
    def needs_tensorflow(x):
        return x
except:
    needs_tensorflow = pytest.mark.skip(reason="TensorFlow is required")

try:
    import jax.numpy as jnp
    def needs_jax(x):
        return x
except:
    needs_jax = pytest.mark.skip(reason="JAX is required")

try:
    import cupy as cp
    def needs_cupy(x):
        return x
except:
    needs_cupy = pytest.mark.skip(reason="CuPy is required")


@needs_numpy
def test01_metadata():
    a = np.zeros(shape=())
    assert t.get_shape(a) == []

    if hasattr(a, '__dlpack__'):
        b = a.__dlpack__()
        assert t.get_shape(b) == []
    else:
        b = None

    with pytest.raises(TypeError) as excinfo:
        # Capsule can only be consumed once
        assert t.get_shape(b) == []
    assert 'incompatible function arguments' in str(excinfo.value)

    a = np.zeros(shape=(3, 4, 5), dtype=np.float64)
    assert t.get_is_valid(a)
    assert t.get_shape(a) == [3, 4, 5]
    assert t.get_size(a) == 60
    assert t.get_nbytes(a) == 60*8
    assert t.get_itemsize(a) == 8
    assert t.check_shape_ptr(a)
    assert t.check_stride_ptr(a)
    if hasattr(a, '__dlpack__'):
        assert t.get_shape(a.__dlpack__()) == [3, 4, 5]
    assert not t.check_float(np.array([1], dtype=np.bool_)) and \
           not t.check_float(np.array([1], dtype=np.uint32)) and \
               t.check_float(np.array([1], dtype=np.float32))
    assert not t.check_bool(np.array([1], dtype=np.uint32)) and \
           not t.check_bool(np.array([1], dtype=np.float32)) and \
               t.check_bool(np.array([1], dtype=np.bool_))

    assert not t.get_is_valid(None)
    assert t.get_size(None) == 0
    assert t.get_nbytes(None) == 0
    assert t.get_itemsize(None) == 0


def test02_docstr():
    assert t.get_shape.__doc__ == "get_shape(array: ndarray[writable=False]) -> list"
    assert t.pass_uint32.__doc__ == "pass_uint32(array: ndarray[dtype=uint32]) -> None"
    assert t.pass_float32.__doc__ == "pass_float32(array: ndarray[dtype=float32]) -> None"
    assert t.pass_complex64.__doc__ == "pass_complex64(array: ndarray[dtype=complex64]) -> None"
    assert t.pass_bool.__doc__ == "pass_bool(array: ndarray[dtype=bool]) -> None"
    assert t.pass_float32_shaped.__doc__ == "pass_float32_shaped(array: ndarray[dtype=float32, shape=(3, *, 4)]) -> None"
    assert t.pass_float32_shaped_ordered.__doc__ == "pass_float32_shaped_ordered(array: ndarray[dtype=float32, order='C', shape=(*, *, 4)]) -> None"
    assert t.check_device.__doc__ == ("check_device(arg: ndarray[device='cpu'], /) -> str\n"
                                      "check_device(arg: ndarray[device='cuda'], /) -> str")


@needs_numpy
def test03_constrain_dtype():
    a_u32 = np.array([1], dtype=np.uint32)
    a_f32 = np.array([1], dtype=np.float32)
    a_cf64 = np.array([1+1j], dtype=np.complex64)
    a_bool = np.array([1], dtype=np.bool_)

    t.pass_uint32(a_u32)
    t.pass_float32(a_f32)
    t.pass_complex64(a_cf64)
    t.pass_complex64_const(a_cf64)
    t.pass_bool(a_bool)

    a_f32_const = a_f32.copy()
    a_f32_const.flags.writeable = False
    t.pass_float32_const(a_f32_const)

    a_cf64_const = a_cf64.copy()
    a_cf64_const.flags.writeable = False
    t.pass_complex64_const(a_cf64_const)

    with pytest.raises(TypeError) as excinfo:
        t.pass_uint32(a_f32)
    assert 'incompatible function arguments' in str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        t.pass_float32(a_u32)
    assert 'incompatible function arguments' in str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        t.pass_complex64(a_u32)
    assert 'incompatible function arguments' in str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        t.pass_bool(a_u32)
    assert 'incompatible function arguments' in str(excinfo.value)


@needs_numpy
def test04_constrain_shape():
    t.pass_float32_shaped(np.zeros((3, 0, 4), dtype=np.float32))
    t.pass_float32_shaped(np.zeros((3, 5, 4), dtype=np.float32))

    with pytest.raises(TypeError) as excinfo:
        t.pass_float32_shaped(np.zeros((3, 5), dtype=np.float32))

    with pytest.raises(TypeError) as excinfo:
        t.pass_float32_shaped(np.zeros((2, 5, 4), dtype=np.float32))

    with pytest.raises(TypeError) as excinfo:
        t.pass_float32_shaped(np.zeros((3, 5, 6), dtype=np.float32))

    with pytest.raises(TypeError) as excinfo:
        t.pass_float32_shaped(np.zeros((3, 5, 4, 6), dtype=np.float32))


@needs_numpy
def test04_constrain_order():
    assert t.check_order(np.zeros((3, 5, 4, 6), order='C')) == 'C'
    assert t.check_order(np.zeros((3, 5, 4, 6), order='F')) == 'F'
    assert t.check_order(np.zeros((3, 5, 4, 6), order='C')[:, 2, :, :]) == '?'
    assert t.check_order(np.zeros((3, 5, 4, 6), order='F')[:, 2, :, :]) == '?'


@needs_jax
def test05_constrain_order_jax():
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        try:
            c = jnp.zeros((3, 5))
        except:
            pytest.skip('jax is missing')

    z = jnp.zeros((3, 5, 4, 6))
    assert t.check_order(z) == 'C'


@needs_torch
@pytest.mark.filterwarnings
def test06_constrain_order_pytorch():
    try:
        c = torch.zeros(3, 5)
        c.__dlpack__()
    except:
        pytest.skip('pytorch is missing')

    f = c.t().contiguous().t()
    assert t.check_order(c) == 'C'
    assert t.check_order(f) == 'F'
    assert t.check_order(c[:, 2:5]) == '?'
    assert t.check_order(f[1:3, :]) == '?'
    assert t.check_device(torch.zeros(3, 5)) == 'cpu'
    if torch.cuda.is_available():
        assert t.check_device(torch.zeros(3, 5, device='cuda')) == 'cuda'


@needs_tensorflow
def test07_constrain_order_tensorflow():
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        try:
            c = tf.zeros((3, 5))
        except:
            pytest.skip('tensorflow is missing')

    assert t.check_order(c) == 'C'


@needs_numpy
def test08_write_from_cpp():
    x = np.zeros(10, dtype=np.float32)
    t.initialize(x)
    assert np.all(x == np.arange(10, dtype=np.float32))

    x = np.zeros((10, 3), dtype=np.float32)
    t.initialize(x)
    assert np.all(x == np.arange(30, dtype=np.float32).reshape(10, 3))


@needs_numpy
def test09_implicit_conversion():
    t.implicit(np.zeros((2, 2), dtype=np.uint32))
    t.implicit(np.zeros((2, 2, 10), dtype=np.float32)[:, :, 4])
    t.implicit(np.zeros((2, 2, 10), dtype=np.uint32)[:, :, 4])
    t.implicit(np.zeros((2, 2, 10), dtype=np.bool_)[:, :, 4])

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(np.zeros((2, 2), dtype=np.bool_))

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(np.zeros((2, 2), dtype=np.uint32))

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(np.zeros((2, 2, 10), dtype=np.float32)[:, :, 4])


@needs_torch
def test10_implicit_conversion_pytorch():
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        try:
            c = torch.zeros(3, 5)
            c.__dlpack__()
        except:
            pytest.skip('pytorch is missing')

    t.implicit(torch.zeros(2, 2, dtype=torch.int32))
    t.implicit(torch.zeros(2, 2, 10, dtype=torch.float32)[:, :, 4])
    t.implicit(torch.zeros(2, 2, 10, dtype=torch.int32)[:, :, 4])

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(torch.zeros(2, 2, dtype=torch.int32))

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(torch.zeros(2, 2, 10, dtype=torch.float32)[:, :, 4])


@needs_tensorflow
def test11_implicit_conversion_tensorflow():
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        try:
            c = tf.zeros((3, 5))
        except:
            pytest.skip('tensorflow is missing')

        t.implicit(tf.zeros((2, 2), dtype=tf.int32))
        t.implicit(tf.zeros((2, 2, 10), dtype=tf.float32)[:, :, 4])
        t.implicit(tf.zeros((2, 2, 10), dtype=tf.int32)[:, :, 4])
        t.implicit(tf.zeros((2, 2, 10), dtype=tf.bool)[:, :, 4])

        with pytest.raises(TypeError) as excinfo:
            t.noimplicit(tf.zeros((2, 2), dtype=tf.int32))

        with pytest.raises(TypeError) as excinfo:
            t.noimplicit(tf.zeros((2, 2), dtype=tf.bool))


@needs_jax
def test12_implicit_conversion_jax():
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        try:
            c = jnp.zeros((3, 5))
        except:
            pytest.skip('jax is missing')

    t.implicit(jnp.zeros((2, 2), dtype=jnp.int32))
    t.implicit(jnp.zeros((2, 2, 10), dtype=jnp.float32)[:, :, 4])
    t.implicit(jnp.zeros((2, 2, 10), dtype=jnp.int32)[:, :, 4])
    t.implicit(jnp.zeros((2, 2, 10), dtype=jnp.bool_)[:, :, 4])

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(jnp.zeros((2, 2), dtype=jnp.int32))

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(jnp.zeros((2, 2), dtype=jnp.uint8))


def test13_destroy_capsule():
    collect()
    dc = t.destruct_count()
    a = t.return_dlpack()
    assert dc == t.destruct_count()
    del a
    collect()
    assert t.destruct_count() - dc == 1


@needs_numpy
def test14_consume_numpy():
    collect()
    class wrapper:
        def __init__(self, value):
            self.value = value
        def __dlpack__(self):
            return self.value
    dc = t.destruct_count()
    a = t.return_dlpack()
    if hasattr(np, '_from_dlpack'):
        x = np._from_dlpack(wrapper(a))
    elif hasattr(np, 'from_dlpack'):
        x = np.from_dlpack(wrapper(a))
    else:
        pytest.skip('your version of numpy is too old')

    del a
    collect()
    assert x.shape == (2, 4)
    assert np.all(x == [[1, 2, 3, 4], [5, 6, 7, 8]])
    assert dc == t.destruct_count()
    del x
    collect()
    assert t.destruct_count() - dc == 1


@needs_numpy
def test15_passthrough():
    a = t.ret_numpy()
    b = t.passthrough(a)
    assert a is b

    a = np.array([1,2,3])
    b = t.passthrough(a)
    assert a is b

    a = None
    with pytest.raises(TypeError) as excinfo:
        b = t.passthrough(a)
    assert 'incompatible function arguments' in str(excinfo.value)
    b = t.passthrough_arg_none(a)
    assert a is b


@needs_numpy
def test16_return_numpy():
    collect()
    dc = t.destruct_count()
    x = t.ret_numpy()
    assert x.shape == (2, 4)
    assert np.all(x == [[1, 2, 3, 4], [5, 6, 7, 8]])
    del x
    collect()
    assert t.destruct_count() - dc == 1


@needs_torch
def test17_return_pytorch():
    try:
        c = torch.zeros(3, 5)
    except:
        pytest.skip('pytorch is missing')
    collect()
    dc = t.destruct_count()
    x = t.ret_pytorch()
    assert x.shape == (2, 4)
    assert torch.all(x == torch.tensor([[1, 2, 3, 4], [5, 6, 7, 8]]))
    del x
    collect()
    assert t.destruct_count() - dc == 1


@needs_numpy
def test18_return_array_scalar():
    collect()
    dc = t.destruct_count()
    x = t.ret_array_scalar()
    assert np.array_equal(x, np.array(1))
    del x
    collect()
    assert t.destruct_count() - dc == 1


# See PR #162
@needs_torch
def test19_single_and_empty_dimension_pytorch():
    a = torch.ones((1,100,1025), dtype=torch.float32)
    t.noop_3d_c_contig(a)
    a = torch.ones((100,1,1025), dtype=torch.float32)
    t.noop_3d_c_contig(a)
    a = torch.ones((0,100,1025), dtype=torch.float32)
    t.noop_3d_c_contig(a)
    a = torch.ones((100,0,1025), dtype=torch.float32)
    t.noop_3d_c_contig(a)
    a = torch.ones((100,1025,0), dtype=torch.float32)
    t.noop_3d_c_contig(a)
    a = torch.ones((100,0,0), dtype=torch.float32)
    t.noop_3d_c_contig(a)
    a = torch.ones((0,0,0), dtype=torch.float32)
    t.noop_3d_c_contig(a)

# See PR #162
@needs_numpy
def test20_single_and_empty_dimension_numpy():
    a = np.ones((1,100,1025), dtype=np.float32)
    t.noop_3d_c_contig(a)
    a = np.ones((100,1,1025), dtype=np.float32)
    t.noop_3d_c_contig(a)
    a = np.ones((0,100,1025), dtype=np.float32)
    t.noop_3d_c_contig(a)
    a = np.ones((100,0,1025), dtype=np.float32)
    t.noop_3d_c_contig(a)
    a = np.ones((100,1025,0), dtype=np.float32)
    t.noop_3d_c_contig(a)
    a = np.ones((100,0,0), dtype=np.float32)
    t.noop_3d_c_contig(a)
    a = np.ones((0,0,0), dtype=np.float32)
    t.noop_3d_c_contig(a)


# See PR #162
@needs_torch
def test21_single_and_empty_dimension_fortran_order_pytorch():
    # This idiom creates a pytorch 2D tensor in column major (aka, 'F') ordering
    a = torch.ones((0,100), dtype=torch.float32).t().contiguous().t()
    t.noop_2d_f_contig(a)
    a = torch.ones((100,0), dtype=torch.float32).t().contiguous().t()
    t.noop_2d_f_contig(a)
    a = torch.ones((1,100), dtype=torch.float32).t().contiguous().t()
    t.noop_2d_f_contig(a)
    a = torch.ones((100,1), dtype=torch.float32).t().contiguous().t()
    t.noop_2d_f_contig(a)


@needs_numpy
def test22_ro_array():
    a = np.array([1, 2], dtype=np.float32)
    assert t.accept_ro(a) == 1
    assert t.accept_rw(a) == 1
    a.setflags(write=False)
    assert t.accept_ro(a) == 1
    with pytest.raises(TypeError) as excinfo:
        t.accept_rw(a)
    assert 'incompatible function arguments' in str(excinfo.value)


@needs_numpy
def test22_return_ro():
    x = t.ret_numpy_const_ref()
    assert t.ret_numpy_const.__doc__  == 'ret_numpy_const() -> numpy.ndarray[dtype=float32, writable=False, shape=(2, 4)]'
    assert x.shape == (2, 4)
    assert np.all(x == [[1, 2, 3, 4], [5, 6, 7, 8]])
    with pytest.raises(ValueError) as excinfo:
        x[0,0] =1
    assert 'read-only' in str(excinfo.value)

@needs_numpy
def test23_check_numpy():
    assert t.check(np.zeros(1))

@needs_torch
def test24_check_torch():
    assert t.check(torch.zeros((1)))

@needs_tensorflow
def test25_check_tensorflow():
    assert t.check(tf.zeros((1)))

@needs_jax
def test26_check_jax():
    assert t.check(jnp.zeros((1)))

@needs_numpy
def test27_rv_policy():
    def p(a):
        return a.__array_interface__['data']

    x1 = t.ret_numpy_const_ref()
    x2 = t.ret_numpy_const_ref()
    y1 = t.ret_numpy_const()
    y2 = t.ret_numpy_const()

    z1 = t.passthrough(y1)
    z2 = t.passthrough(y2)
    q1 = t.passthrough_copy(y1)
    q2 = t.passthrough_copy(y2)

    assert p(x1) == p(x2)
    assert p(y1) != p(y2)

    assert z1 is y1
    assert z2 is y2
    assert q1 is not y1
    assert q2 is not y2
    assert p(q1) != p(y1)
    assert p(q2) != p(y2)

@needs_numpy
def test28_reference_internal():
    collect()
    dc = t.destruct_count()
    c = t.Cls()

    v1_a = c.f1()
    v1_b = c.f1()
    v2_a = c.f2()
    v2_b = c.f2()
    del c

    assert np.all(v1_a == np.arange(10, dtype=np.float32))
    assert np.all(v1_b == np.arange(10, dtype=np.float32))

    v1_a += 1
    v1_b += 2

    assert np.all(v1_a == np.arange(10, dtype=np.float32) + 1)
    assert np.all(v1_b == np.arange(10, dtype=np.float32) + 2)
    del v1_a
    del v1_b

    assert np.all(v2_a == np.arange(10, dtype=np.float32))
    assert np.all(v2_b == np.arange(10, dtype=np.float32))

    v2_a += 1
    v2_b += 2

    assert np.all(v2_a == np.arange(10, dtype=np.float32) + 3)
    assert np.all(v2_b == np.arange(10, dtype=np.float32) + 3)

    del v2_a
    collect()
    assert t.destruct_count() == dc

    del v2_b
    collect()
    dc += 1
    assert t.destruct_count() == dc

    for i in range(2):
        c2 = t.Cls()

        if i == 0:
            v3_a = c2.f1_ri()
            v3_b = c2.f1_ri()
        else:
            v3_a = c2.f2_ri()
            v3_b = c2.f2_ri()
        del c2

        assert np.all(v3_a == np.arange(10, dtype=np.float32))
        assert np.all(v3_b == np.arange(10, dtype=np.float32))

        v3_a += 1
        v3_b += 2

        assert np.all(v3_a == np.arange(10, dtype=np.float32) + 3)
        assert np.all(v3_b == np.arange(10, dtype=np.float32) + 3)
        del v3_a

        collect()
        assert t.destruct_count() == dc

        del v3_b
        collect()
        dc += 1
        assert t.destruct_count() == dc

    c3 = t.Cls()
    c3_t = (c3,)
    with pytest.raises(RuntimeError) as excinfo:
        c3.f3_ri(c3_t)

    msg = 'nanobind::detail::ndarray_wrap(): reference_internal policy cannot be applied (ndarray already has an owner)'
    assert msg in str(excinfo.value)

@needs_numpy
def test29_force_contig_numpy():
    a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    b = t.make_contig(a)
    assert b is a
    a = a.T
    b = t.make_contig(a)
    assert b is not a
    assert np.all(b == a)

@needs_torch
@pytest.mark.filterwarnings
def test30_force_contig_pytorch():
    a = torch.tensor([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
    b = t.make_contig(a)
    assert b is a
    a = a.T
    b = t.make_contig(a)
    assert b is not a
    assert torch.all(b == a)

@needs_numpy
def test31_view():
    # 1
    x1 = np.array([[1,2],[3,4]], dtype=np.float32)
    x2 = np.array([[1,2],[3,4]], dtype=np.float64)
    assert np.allclose(x1, x2)
    t.fill_view_1(x1)
    assert np.allclose(x1, x2*2)
    t.fill_view_1(x2)
    assert np.allclose(x1, x2*2)

    # 2
    x1 = np.zeros((3, 4), dtype=np.float32, order='C')
    x2 = np.zeros((3, 4), dtype=np.float32, order='F')
    t.fill_view_2(x1)
    t.fill_view_2(x2)
    x3 = np.zeros((3, 4), dtype=np.float32, order='C')
    t.fill_view_3(x3)
    x4 = np.zeros((3, 4), dtype=np.float32, order='F')
    t.fill_view_4(x4)

    assert np.all(x1 == x2) and np.all(x2 == x3) and np.all(x3 == x4)

    # 3
    x1 = np.array([[1+2j, 3+4j], [5+6j, 7+8j]], dtype=np.complex64)
    x2 = x1 * 2
    t.fill_view_1(x1.view(np.float32))
    assert np.allclose(x1, x2)
    x2 = x1 * (-1+2j)
    t.fill_view_5(x1)
    assert np.allclose(x1, x2)

@needs_numpy
def test32_half():
    if not hasattr(t, 'ret_numpy_half'):
        pytest.skip('half precision test is missing')
    x = t.ret_numpy_half()
    assert x.dtype == np.float16
    assert x.shape == (2, 4)
    assert np.all(x == [[1, 2, 3, 4], [5, 6, 7, 8]])

@needs_numpy
def test33_cast():
    a = t.cast(False)
    b = t.cast(True)
    assert a.ndim == 0 and b.ndim == 0
    assert a.dtype == np.int32 and b.dtype == np.float32
    assert a == 1 and b == 1

@needs_numpy
def test34_complex_decompose():
    x1 = np.array([1 + 2j, 3 + 4j, 5 + 6j], dtype=np.complex64)

    assert np.all(x1.real == np.array([1, 3, 5], dtype=np.float32))
    assert np.all(x1.imag == np.array([2, 4, 6], dtype=np.float32))

@needs_numpy
@pytest.mark.parametrize("variant", [1, 2])
def test_uint32_complex_do_not_convert(variant):
    if variant == 1:
        arg = 1
    else:
        arg = np.uint32(1)
    data = np.array([1.0 + 2.0j, 3.0 + 4.0j])
    t.set_item(data, arg)
    data2 = np.array([123, 3.0 + 4.0j])
    assert np.all(data == data2)

@needs_numpy
def test36_check_generic():
    class DLPackWrapper:
        def __init__(self, o):
            self.o = o
        def __dlpack__(self):
            return self.o.__dlpack__()

    arr = DLPackWrapper(np.zeros((1)))
    assert t.check(arr)

@needs_numpy
def test37_noninteger_stride():
    a = np.array([[1, 2, 3, 4, 0, 0], [5, 6, 7, 8, 0, 0]], dtype=np.float32)
    s = a[:, 0:4]  # slice
    t.pass_float32(s)
    assert t.get_stride(s, 0) == 6;
    assert t.get_stride(s, 1) == 1;
    v = s.view(np.complex64)
    t.pass_complex64(v)
    assert t.get_stride(v, 0) == 3;
    assert t.get_stride(v, 1) == 1;

    a = np.array([[1, 2, 3, 4, 0], [5, 6, 7, 8, 0]], dtype=np.float32)
    s = a[:, 0:4]  # slice
    t.pass_float32(s)
    assert t.get_stride(s, 0) == 5;
    assert t.get_stride(s, 1) == 1;
    v = s.view(np.complex64)
    with pytest.raises(TypeError) as excinfo:
        t.pass_complex64(v)
    assert 'incompatible function arguments' in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        t.get_stride(v, 0);
    assert 'incompatible function arguments' in str(excinfo.value)

@needs_cupy
@pytest.mark.filterwarnings
def test38_constrain_order_cupy():
    try:
        c = cp.zeros((3, 5))
        c.__dlpack__()
    except:
        pytest.skip('cupy is missing')

    f = cp.asarray(c, order="F")
    assert t.check_order(c) == 'C'
    assert t.check_order(f) == 'F'
    assert t.check_order(c[:, 2:5]) == '?'
    assert t.check_order(f[1:3, :]) == '?'
    assert t.check_device(cp.zeros((3, 5))) == 'cuda'


@needs_cupy
def test39_implicit_conversion_cupy():
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        try:
            c = cp.zeros((3, 5))
        except:
            pytest.skip('cupy is missing')

    t.implicit(cp.zeros((2, 2), dtype=cp.int32))
    t.implicit(cp.zeros((2, 2, 10), dtype=cp.float32)[:, :, 4])
    t.implicit(cp.zeros((2, 2, 10), dtype=cp.int32)[:, :, 4])
    t.implicit(cp.zeros((2, 2, 10), dtype=cp.bool_)[:, :, 4])

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(cp.zeros((2, 2), dtype=cp.int32))

    with pytest.raises(TypeError) as excinfo:
        t.noimplicit(cp.zeros((2, 2), dtype=cp.uint8))
