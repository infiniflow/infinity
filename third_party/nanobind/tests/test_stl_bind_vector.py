import pytest
import platform

import test_bind_vector_ext as t

def test01_vector_int(capfd):
    v_int = t.VectorInt([0, 0])
    assert len(v_int) == 2
    assert bool(v_int) is True

    # test construction from a generator
    v_int1 = t.VectorInt(x for x in range(5))
    assert t.VectorInt(v_int1) == t.VectorInt([0, 1, 2, 3, 4])
    assert repr(v_int1) == "test_bind_vector_ext.VectorInt([0, 1, 2, 3, 4])"

    v_int2 = t.VectorInt([0, 0])
    assert v_int == v_int2
    v_int2[1] = 2
    v_int2[1] -= 1
    assert v_int2[1] == 1
    assert v_int != v_int2

    v_int2.append(2)
    v_int2.insert(0, 1)
    v_int2.insert(0, 2)
    v_int2.insert(0, 3)
    v_int2.insert(6, 3)
    with pytest.raises(IndexError):
        v_int2.insert(8, 4)

    v_int.append(99)
    v_int2[2:-2] = v_int
    assert v_int2 == t.VectorInt([3, 2, 0, 0, 99, 2, 3])
    del v_int2[1:3]
    assert v_int2 == t.VectorInt([3, 0, 99, 2, 3])
    del v_int2[0]
    assert v_int2 == t.VectorInt([0, 99, 2, 3])

    v_int2.extend(t.VectorInt([4, 5]))
    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5])

    v_int2.extend([6, 7])
    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5, 6, 7])

    # test error handling, and that the vector is unchanged
    with pytest.raises(TypeError):
        v_int2.extend([8, "a"])

    captured = capfd.readouterr().err.strip()
    ref = "nanobind: implicit conversion from type 'list' to type 'test_bind_vector_ext.VectorInt' failed!"

    # Work around Pytest-related flakiness (https://github.com/pytest-dev/pytest/issues/10843)
    if platform.system() == 'Windows':
        assert captured == ref or captured == ''
    else:
        assert captured == ref

    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5, 6, 7])

    # test extending from a generator
    v_int2.extend(x for x in range(5))
    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4])

    # Test count feature
    assert v_int2.count(2) == 2
    assert v_int2.count(5) == 1
    assert v_int2.count(8) == 0
    assert 2 in v_int2
    assert 5 in v_int2
    assert 8 not in v_int2

    # test negative indexing
    assert v_int2[-1] == 4

    # insert with negative index
    v_int2.insert(-1, 88)
    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 88, 4])

    # delete negative index
    del v_int2[-1]
    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 88])

    assert v_int2.pop() == 88
    assert v_int2 == t.VectorInt([0, 99, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3])

    assert v_int2.pop(1) == 99
    assert v_int2 == t.VectorInt([0, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3])

    v_int2.clear()
    assert len(v_int2) == 0


def test02_vector_bool():
    vv_c = t.VectorBool()
    for i in range(9):
        vv_c.append(i % 2 == 0)
    for i in range(9):
        assert vv_c[i] == (i % 2 == 0)
    assert vv_c.count(True) == 5
    assert vv_c.count(False) == 4


def test03_vector_custom():
    v_a = t.VectorEl()
    v_a.append(t.El(1))
    v_a.append(t.El(2))
    assert len(v_a) == 2 and v_a[0].a == 1 and v_a[1].a == 2

    vv_a = t.VectorVectorEl()
    vv_a.append(v_a)
    v_b = vv_a[0]
    assert len(v_b) == 2 and v_b[0].a == 1 and v_b[1].a == 2


def test04_vector_slicing():
    l1 = list(range(100))
    l2 = t.VectorInt(l1)

    def check_same(s):
        assert l1[s] == l2[s]

    def check_del(s):
        l1c = type(l1)(l1)
        l2c = type(l2)(l2)
        del l1c[s]
        del l2c[s]
        l2c = list(l2c)
        assert l1c == l2c

    check_same(slice(1, 13, 4))
    check_same(slice(1, 14, 4))
    check_same(slice(10, 2000, 1))
    check_same(slice(200, 10, 1))
    check_same(slice(200, 10, -1))
    check_same(slice(200, 10, -3))
    check_del(slice(1, 13, 4))
    check_del(slice(1, 14, 4))
    check_del(slice(10, 2000, 1))
    check_del(slice(200, 10, 1))
    check_del(slice(200, 10, -1))
    check_del(slice(200, 10, -3))


def test05_vector_non_shared():
    v = t.VectorEl()
    v.append(t.El(1))
    v.append(t.El(2))

    v0, v1 = v[0], v[1]
    v0.a, v1.a = 100, 200

    assert v[0].a == 1
    assert v[1].a == 2

    # Check that elements accessed through the iterator *cannot* be modified
    q = next(iter(v))
    q.a = 5
    assert v[0].a == 1

def test06_vector_shared():
    v = t.VectorElShared()
    v.append(t.El(1))
    v.append(t.El(2))

    v0, v1 = v[0], v[1]
    v0.a, v1.a = 100, 200

    assert v[0].a == 100
    assert v[1].a == 200

    # Check that elements accessed through the iterator *can* be modified
    q = next(iter(v))
    q.a = 5
    assert v[0].a == 5


def test07_vector_noncopyable():
    vnc = t.get_vnc(5)
    for i in range(0, 5):
        assert vnc[i].value == i + 1

    for i, j in enumerate(vnc, start=1):
        assert j.value == i

    # Check that elements accessed through the iterator *can* be modified
    q = next(iter(vnc))
    q.value = 5
    assert vnc[0].value == 5
