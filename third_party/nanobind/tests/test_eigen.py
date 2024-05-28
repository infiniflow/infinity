import pytest
import gc
import itertools
import re
import sys

try:
    import numpy as np
    from numpy.testing import assert_array_equal
    import test_eigen_ext as t
    def needs_numpy_and_eigen(x):
        return x
except:
    needs_numpy_and_eigen = pytest.mark.skip(reason="NumPy and Eigen are required")


@needs_numpy_and_eigen
def test01_vector_fixed():
    a  = np.array([1, 2, 3],    dtype=np.int32)
    b  = np.array([0, 1, 2],    dtype=np.int32)
    c  = np.array([1, 3, 5],    dtype=np.int32)
    x  = np.array([1, 3, 5, 6], dtype=np.int32)
    af = np.float32(a)
    bf = np.float32(b)

    assert_array_equal(t.addV3i(a, b), c)
    assert_array_equal(t.addR3i(a, b), c)
    assert_array_equal(t.addRefCnstV3i(a, b), c)
    assert_array_equal(t.addRefCnstR3i(a, b), c)
    assert_array_equal(t.addA3i(a, b), c)
    assert_array_equal(t.addA3i_retExpr(a, b), c)

    # Implicit conversion supported for first argument
    assert_array_equal(t.addV3i(af, b), c)
    assert_array_equal(t.addR3i(af, b), c)
    assert_array_equal(t.addRefCnstV3i(af, b), c)
    assert_array_equal(t.addRefCnstR3i(af, b), c)
    assert_array_equal(t.addA3i(af, b), c)

    # But not the second one
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addV3i(a, bf)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addR3i(a, bf)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addRefCnstV3i(a, bf)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addRefCnstR3i(a, bf)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addA3i(a, bf)

    # Catch size errors
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addV3i(x, b)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addR3i(x, b)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addRefCnstV3i(x, b)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.addA3i(x, b)


@needs_numpy_and_eigen
def test02_vector_dynamic():
    a  = np.array([1, 2, 3], dtype=np.int32)
    b  = np.array([0, 1, 2], dtype=np.int32)
    c  = np.array([1, 3, 5], dtype=np.int32)
    x  = np.arange(10000, dtype=np.int32)
    af = np.float32(a)

    # Check call with dynamically sized arrays
    assert_array_equal(t.addVXi(a, b), c)

    # Implicit conversion
    assert_array_equal(t.addVXi(af, b), c)

    # Try with a big array. This will move the result to avoid a copy
    assert_array_equal(t.addVXi(x, x), 2*x)


@needs_numpy_and_eigen
def test03_update_map():
    a = np.array([1, 2, 3], dtype=np.int32)
    b = np.array([1, 2, 123], dtype=np.int32)
    c = a.copy()
    t.updateRefV3i(c)
    assert_array_equal(c, b)

    c = a.copy()
    t.updateRefV3i_nc(c)
    assert_array_equal(c, b)

    c = a.copy()
    t.updateRefVXi(c)
    assert_array_equal(c, b)

    c = a.copy()
    t.updateRefVXi_nc(c)
    assert_array_equal(c, b)

    c = np.float32(a)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.updateRefV3i(c)

    c = np.float32(a)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.updateRefV3i_nc(c)

    c = np.float32(a)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.updateRefVXi(c)

    c = np.float32(a)
    with pytest.raises(TypeError, match='incompatible function arguments'):
        t.updateRefVXi_nc(c)



@needs_numpy_and_eigen
def test04_matrix():
    A = np.vander((1, 2, 3, 4,))
    At = A.T
    assert A.flags['C_CONTIGUOUS']
    assert At.flags['F_CONTIGUOUS']
    base = np.zeros((A.shape[0] * 2, A.shape[1] * 2), A.dtype)
    base[::2, ::2] = A
    Av = base[-2::-2, -2::-2]
    assert Av.base is base
    Avt = Av.T
    assert Avt.base is base
    matrices = A, At, Av, Avt
    for addM in (t.addM4uCC, t.addM4uRR, t.addM4uCR, t.addM4uRC,
                 t.addMXuCC, t.addMXuRR, t.addMXuCR, t.addMXuRC):
        for left, right in itertools.product(matrices, matrices):
            assert_array_equal(addM(left, right), left + right)


@needs_numpy_and_eigen
@pytest.mark.parametrize("rowStart", (0, 1))
@pytest.mark.parametrize("colStart", (0, 2))
@pytest.mark.parametrize("rowStep", (1, 2, -2))
@pytest.mark.parametrize("colStep", (1, 3, -3))
@pytest.mark.parametrize("transpose", (False, True))
def test05_matrix_large_nonsymm(rowStart, colStart, rowStep, colStep, transpose):
    A = np.uint32(np.vander(np.arange(80)))
    if rowStep < 0:
        rowStart = -rowStart - 1
    if colStep < 0:
        colStart = -colStart - 1
    A = A[rowStart::rowStep, colStart::colStep]
    if transpose:
        A = A.T
    A2 = A + A
    assert_array_equal(t.addMXuCC(A, A), A2)
    assert_array_equal(t.addMXuRR(A, A), A2)
    assert_array_equal(t.addMXuCR(A, A), A2)
    assert_array_equal(t.addMXuRC(A, A), A2)
    assert_array_equal(t.addDRefMXuCC_nc(A, A), A2)
    assert_array_equal(t.addDRefMXuRR_nc(A, A), A2)
    if A.flags['C_CONTIGUOUS']:
        assert_array_equal(t.addMapMXuRR(A, A), A2)
        assert_array_equal(t.addMapCnstMXuRR(A, A), A2)
    else:
        with pytest.raises(TypeError, match="incompatible function arguments"):
            t.addMapMXuRR(A, A)
        with pytest.raises(TypeError, match="incompatible function arguments"):
            t.addMapCnstMXuRR(A, A)

    assert_array_equal(t.addRefCnstMXuRR(A, A), A2)
    assert_array_equal(t.addRefCnstMXuRR(A.view(np.int32), A), A2)
    assert_array_equal(t.addRefCnstMXuRR_nc(A, A), A2)
    with pytest.raises(TypeError, match="incompatible function arguments"):
        t.addRefCnstMXuRR_nc(A.view(np.int32), A)
    if A.strides[1] == A.itemsize:
        assert_array_equal(t.addRefMXuRR(A, A), A2)
    else:
        with pytest.raises(TypeError, match="incompatible function arguments"):
            t.addRefMXuRR(A, A)
    if A.flags['F_CONTIGUOUS']:
        assert_array_equal(t.addMapMXuCC(A, A), A2)
        assert_array_equal(t.addMapCnstMXuCC(A, A), A2)
    else:
        with pytest.raises(TypeError, match="incompatible function arguments"):
            t.addMapMXuCC(A, A)
        with pytest.raises(TypeError, match="incompatible function arguments"):
            t.addMapCnstMXuCC(A, A)

    assert_array_equal(t.addRefCnstMXuCC(A, A), A2)
    assert_array_equal(t.addRefCnstMXuCC(A.view(np.int32), A), A2)
    assert_array_equal(t.addRefCnstMXuCC_nc(A, A), A2)
    with pytest.raises(TypeError, match="incompatible function arguments"):
        t.addRefCnstMXuCC_nc(A.view(np.int32), A)
    if A.strides[0] == A.itemsize:
        assert_array_equal(t.addRefMXuCC(A, A), A2)
    else:
        with pytest.raises(TypeError, match="incompatible function arguments"):
            t.addRefMXuCC(A, A)
    A = np.ascontiguousarray(A)
    assert A.flags['C_CONTIGUOUS']
    assert_array_equal(t.addMXuRR_nc(A, A), A2)
    A = np.asfortranarray(A)
    assert A.flags['F_CONTIGUOUS']
    assert_array_equal(t.addMXuCC_nc(A, A), A2)


@needs_numpy_and_eigen
def test06_map():
    b = t.Buffer()
    m = b.map()
    dm = b.dmap()
    for i in range(10):
        for j in range(3):
            m[i, j] = i*3+j
    for i in range(10):
        for j in range(3):
            assert dm[i, j] == i*3+j
    del dm
    del b
    gc.collect()
    gc.collect()
    for i in range(10):
        for j in range(3):
            assert m[i, j] == i*3+j


@needs_numpy_and_eigen
def test07_mutate_arg():
    A = np.uint32(np.vander(np.arange(10)))
    A2 = A.copy()
    t.mutate_DRefMXuC(A)
    assert_array_equal(A, 2*A2)


@needs_numpy_and_eigen
def test08_sparse():
    pytest.importorskip("scipy")
    import scipy.sparse

    # no isinstance here because we want strict type equivalence
    assert type(t.sparse_r()) is scipy.sparse.csr_matrix
    assert type(t.sparse_c()) is scipy.sparse.csc_matrix
    assert type(t.sparse_copy_r(t.sparse_r())) is scipy.sparse.csr_matrix
    assert type(t.sparse_copy_c(t.sparse_c())) is scipy.sparse.csc_matrix
    assert type(t.sparse_copy_r(t.sparse_c())) is scipy.sparse.csr_matrix
    assert type(t.sparse_copy_c(t.sparse_r())) is scipy.sparse.csc_matrix

    def assert_sparse_equal_ref(sparse_mat):
        ref = np.array(
            [
                [0.0, 3, 0, 0, 0, 11],
                [22, 0, 0, 0, 17, 11],
                [7, 5, 0, 1, 0, 11],
                [0, 0, 0, 0, 0, 11],
                [0, 0, 14, 0, 8, 11],
            ]
        )
        assert_array_equal(sparse_mat.toarray(), ref)

    assert_sparse_equal_ref(t.sparse_r())
    assert_sparse_equal_ref(t.sparse_c())
    assert_sparse_equal_ref(t.sparse_copy_r(t.sparse_r()))
    assert_sparse_equal_ref(t.sparse_copy_c(t.sparse_c()))
    assert_sparse_equal_ref(t.sparse_copy_r(t.sparse_c()))
    assert_sparse_equal_ref(t.sparse_copy_c(t.sparse_r()))


@needs_numpy_and_eigen
def test09_sparse_failures():
    pytest.importorskip("scipy")
    import scipy

    with pytest.raises(
        ValueError,
        match=re.escape(
            "nanobind: unable to return an Eigen sparse matrix that is not in a compressed format. Please call `.makeCompressed()` before returning the value on the C++ end."
        ),
    ):
        t.sparse_r_uncompressed()

    csr_matrix = scipy.sparse.csr_matrix
    scipy.sparse.csr_matrix = None
    with pytest.raises(TypeError, match=re.escape("'NoneType' object is not callable")):
        t.sparse_r()

    del scipy.sparse.csr_matrix
    with pytest.raises(
        AttributeError,
        match=re.escape("module 'scipy.sparse' has no attribute 'csr_matrix'"),
    ):
        t.sparse_r()

    sys_path = sys.path
    sys.path = []
    del sys.modules["scipy"]
    with pytest.raises(ModuleNotFoundError, match=re.escape("No module named 'scipy'")):
        t.sparse_r()

    # undo sabotage of the module
    sys.path = sys_path
    scipy.sparse.csr_matrix = csr_matrix

@needs_numpy_and_eigen
def test10_eigen_scalar_default():
    x = t.default_arg()
    assert x==0

@needs_numpy_and_eigen
def test11_prop():
    for j in range(3):
        c = t.ClassWithEigenMember()
        ref = np.ones((2, 2))
        if j == 0:
            c.member = ref

        for i in range(2):
            member = c.member
            if j == 2 and i == 0:
                member[0, 0] = 10
                ref[0, 0] = 10
            assert_array_equal(member, ref)
            del member
            gc.collect()
            gc.collect()

        member = c.member
        del c
        gc.collect()
        gc.collect()
        assert_array_equal(member, ref)

@needs_numpy_and_eigen
def test12_cast():
    vec = np.arange(1000, dtype=np.int32)
    vec2 = vec[::2]
    vecf = np.float32(vec)
    assert_array_equal(t.castToMapVXi(vec), vec)
    assert_array_equal(t.castToMapCnstVXi(vec), vec)
    assert_array_equal(t.castToRefVXi(vec), vec)
    assert_array_equal(t.castToRefCnstVXi(vec), vec)
    assert t.castToMapVXi(vec).flags.writeable
    assert not t.castToMapCnstVXi(vec).flags.writeable
    assert_array_equal(t.castToDRefCnstVXi(vec), vec)
    for v in vec2, vecf:
        with pytest.raises(RuntimeError, match="bad[_ ]cast"):
            t.castToMapVXi(v)
        with pytest.raises(RuntimeError, match="bad[_ ]cast"):
            t.castToRefVXi(v)
        assert_array_equal(t.castToRefCnstVXi(v), v)
    assert_array_equal(t.castToDRefCnstVXi(vec2), vec2)
    with pytest.raises(RuntimeError, match="bad[_ ]cast"):
        t.castToDRefCnstVXi(vecf)
    for v in vec, vec2, vecf:
        with pytest.raises(RuntimeError, match='bad[_ ]cast'):
            t.castToRef03CnstVXi(v)

@needs_numpy_and_eigen
def test13_mutate_python():
    class Derived(t.Base):
        def modRefData(self, input):
            input[0] = 3.0

        def modRefDataConst(self, input):
            input[0] = 3.0

    vecRef = np.array([3.0, 2.0])
    der = Derived()
    assert_array_equal(t.modifyRef(der), vecRef)
    with pytest.raises(ValueError):
        t.modifyRefConst(der)


@needs_numpy_and_eigen
def test14_single_element():
    a = np.array([[1]], dtype=np.uint32)
    assert a.ndim == 2 and a.shape == (1, 1)
    t.addMXuCC(a, a)
