import sys
import test_holders_ext as t
import pytest
from common import collect


@pytest.fixture
def clean():
    collect()
    t.reset()

# ------------------------------------------------------------------

def test01_create(clean):
    e = t.Example(123)
    assert e.value == 123
    assert t.query_shared_1(e) == 123
    assert t.query_shared_2(e) == 123
    assert t.query_shared_3(e) == 123
    del e
    collect()
    assert t.stats() == (1, 1)


def test02_sharedptr_from_python(clean):
    e = t.Example(234)
    w = t.SharedWrapper(e)
    assert w.ptr is e
    del e
    collect()
    assert t.stats() == (1, 0)
    del w
    collect()
    assert t.stats() == (1, 1)

    w = t.SharedWrapper(t.Example(234))
    collect()
    assert t.stats() == (2, 1)
    w.ptr = t.Example(0)
    collect()
    assert t.stats() == (3, 2)
    del w
    collect()
    assert t.stats() == (3, 3)


def test03_sharedptr_from_cpp(clean):
    e = t.Example.make(5)
    assert t.passthrough(e) is e
    assert t.passthrough_2(e) is e
    assert t.query_shared_1(e) == 5
    assert t.query_shared_2(e) == 5
    assert t.query_shared_3(e) == 5

    w = t.SharedWrapper(e)
    assert e is not w.value
    assert w.value == 5
    w.value = 6
    assert e.value == 6
    del w, e

    e = t.Example.make_shared(6)
    assert t.query_shared_1(e) == 6
    assert t.query_shared_2(e) == 6
    assert t.query_shared_3(e) == 6
    assert t.passthrough(e) is e
    assert t.passthrough_2(e) is e

    w = t.SharedWrapper(e)
    assert e is not w.value
    assert w.value == 6
    del w, e
    collect()
    assert t.stats() == (2, 2)

# ------------------------------------------------------------------

def test04_uniqueptr_from_cpp(clean):
    a = t.unique_from_cpp()
    b = t.unique_from_cpp_2()
    assert a.value == 1
    assert b.value == 2
    del a, b
    collect()
    assert t.stats() == (2, 2)


def test05a_uniqueptr_from_cpp(clean):
    # Test ownership exchange when the object has been created on the C++ side
    a = t.unique_from_cpp()
    b = t.unique_from_cpp_2()
    wa = t.UniqueWrapper(a)
    wb = t.UniqueWrapper(b)
    with pytest.warns(RuntimeWarning, match='nanobind: attempted to access a relinquished instance of type \'test_holders_ext.Example\'!'):
        with pytest.raises(TypeError) as excinfo:
            assert a.value == 1
        assert 'incompatible function arguments' in str(excinfo.value)
    with pytest.warns(RuntimeWarning, match='nanobind: attempted to access a relinquished instance of type \'test_holders_ext.Example\'!'):
        with pytest.raises(TypeError) as excinfo:
            assert b.value == 2
        assert 'incompatible function arguments' in str(excinfo.value)
    del a, b
    del wa, wb
    collect()
    assert t.stats() == (2, 2)

    t.reset()
    a = t.unique_from_cpp()
    b = t.unique_from_cpp_2()
    wa = t.UniqueWrapper(a)
    wb = t.UniqueWrapper(b)

    a2 = wa.get()
    b2 = wb.get()
    assert a2.value == 1 and b2.value == 2
    assert a2 is a and b2 is b
    assert a.value == 1 and b.value == 2
    collect()
    assert t.stats() == (2, 0)
    del a, b, a2, b2
    collect()
    assert t.stats() == (2, 2)

def test05b_uniqueptr_list(clean):
    t.reset()
    # Test ownership exchange as part of a larger data structure
    k = t.unique_from_cpp(1)
    v = t.unique_from_cpp(2)
    res = t.passthrough_unique_pairs([(k, v)])
    assert res == [(k, v)]
    assert k.value == 1 and v.value == 2

    res = t.passthrough_unique_pairs([(k, v)], clear=True)
    assert res == []
    for obj in (k, v):
        with pytest.warns(RuntimeWarning, match='nanobind: attempted to access a relinquished instance of type \'test_holders_ext.Example\'!'):
            with pytest.raises(TypeError) as excinfo:
                obj.value
    collect()
    assert t.stats() == (2, 2)

def test05c_uniqueptr_structure_duplicate(clean):
    t.reset()
    # Test ownership exchange that fails partway through
    # (can't take ownership from k twice)
    k = t.unique_from_cpp(3)
    with pytest.warns(RuntimeWarning, match=r'nanobind::detail::nb_relinquish_ownership()'):
        with pytest.raises(TypeError):
            t.passthrough_unique_pairs([(k, k)])
    # Ownership passes back to Python
    assert k.value == 3

    del k
    collect()
    assert t.stats() == (1, 1)

def test05d_uniqueptr_reinit(clean):
    x = t.unique_from_cpp()
    assert x.value == 1
    w = t.UniqueWrapper(x)
    with pytest.warns(RuntimeWarning, match='nanobind: attempted to access a relinquished instance of type \'test_holders_ext.Example\'!'):
        with pytest.raises(TypeError):
            x.value
    with pytest.warns(RuntimeWarning, match='nanobind: attempted to access a relinquished instance of type \'test_holders_ext.Example\'!'):
        with pytest.raises(TypeError):
            x.__init__(3)
    y = w.get()
    assert y is x and y.value == 1


def test06_uniqueptr_from_py(clean):
    # Test ownership exchange when the object has been created on the Python side
    a = t.Example(1)
    with pytest.warns(RuntimeWarning, match=r'nanobind::detail::nb_relinquish_ownership()'):
        with pytest.raises(TypeError) as excinfo:
            wa = t.UniqueWrapper(a)
    wa = t.UniqueWrapper2(a)
    with pytest.warns(RuntimeWarning, match='nanobind: attempted to access a relinquished instance of type \'test_holders_ext.Example\'!'):
        with pytest.raises(TypeError) as excinfo:
            assert a.value == 1
        assert 'incompatible function arguments' in str(excinfo.value)
    a2 = wa.get()
    assert a2.value == 1 and a is a2
    del a, a2
    collect()
    assert t.stats() == (1, 1)

    # Test that ownership exchange as part of a larger data structure fails
    # gracefully rather than crashing
    k = t.Example(1)
    v = t.Example(2)
    with pytest.warns(RuntimeWarning, match=r'nanobind::detail::nb_relinquish_ownership()'):
        with pytest.raises(TypeError) as excinfo:
            t.passthrough_unique_pairs([(k, v)])
    assert k.value == 1 and v.value == 2

    # Test the case where the key relinquishes ownership successfully and
    # then the value can't do
    v = t.unique_from_cpp(3)
    with pytest.warns(RuntimeWarning, match=r'nanobind::detail::nb_relinquish_ownership()'):
        with pytest.raises(TypeError) as excinfo:
            t.passthrough_unique_pairs([(k, v)])
    assert k.value == 1 and v.value == 3

    del k, v
    collect()
    assert t.stats() == (4, 4)


def test07_uniqueptr_passthrough(clean):
    assert t.passthrough_unique(None) is None
    assert t.passthrough_unique(t.unique_from_cpp()).value == 1
    assert t.passthrough_unique(t.unique_from_cpp_2()).value == 2
    assert t.passthrough_unique_2(t.unique_from_cpp()).value == 1
    assert t.passthrough_unique_2(t.unique_from_cpp_2()).value == 2
    collect()
    assert t.stats() == (4, 4)
    t.reset()

    with pytest.warns(RuntimeWarning, match=r'nanobind::detail::nb_relinquish_ownership()'):
        with pytest.raises(TypeError):
            assert t.passthrough_unique(t.Example(1)).value == 1
    assert t.passthrough_unique_2(t.Example(1)).value == 1
    collect()
    assert t.stats() == (2, 2)


def test07_polymorphic_downcast_unique():
    assert isinstance(t.u_factory(), t.Base)
    assert isinstance(t.u_factory_2(), t.Base)
    assert isinstance(t.u_polymorphic_factory(), t.PolymorphicSubclass)
    assert isinstance(t.u_polymorphic_factory_2(), t.PolymorphicBase)


def test08_polymorphic_downcast_shared():
    assert isinstance(t.s_factory(), t.Base)
    assert isinstance(t.s_factory_2(), t.Base)
    assert isinstance(t.s_polymorphic_factory(), t.PolymorphicSubclass)
    assert isinstance(t.s_polymorphic_factory_2(), t.PolymorphicBase)

def test09_tag_based():
    assert isinstance(t.make_pet(t.PetKind.Dog), t.Dog)
    assert isinstance(t.make_pet(t.PetKind.Cat), t.Cat)


def test09_tag_based_unique():
    assert isinstance(t.make_pet_u(t.PetKind.Dog), t.Dog)
    assert isinstance(t.make_pet_u(t.PetKind.Cat), t.Cat)

def test09_tag_based_shared():
    assert isinstance(t.make_pet_s(t.PetKind.Dog), t.Dog)
    assert isinstance(t.make_pet_s(t.PetKind.Cat), t.Cat)


def check_shared_from_this_py_owned(ty, factory, value):
    e = ty(value)

    # Creating from Python does not enable shared_from_this
    assert e.value == value
    assert not e.has_shared_from_this()
    assert t.owns_cpp(e)

    # Passing to C++ as a shared_ptr does
    w = t.SharedWrapperST(e)
    assert e.has_shared_from_this()
    assert w.ptr is e

    # Execute shared_from_this on the C++ side
    w2 = t.SharedWrapperST.from_existing(e)
    assert e.use_count() == 2
    assert w.value == w2.value == e.value == value
    assert t.same_owner(w, w2)

    # Returning a raw pointer from C++ locates the existing instance
    assert w2.get_own() is w2.get_ref() is e
    assert t.owns_cpp(e)

    if hasattr(sys, "getrefcount"):
        # One reference is held by the C++ shared_ptr, one by our
        # locals dict, and one by the arg to getrefcount
        rc = sys.getrefcount(e)
        assert rc == 3

    # Dropping the Python object does not actually destroy it, because
    # the shared_ptr holds a reference. There is still a PyObject* at
    # the same address.
    prev_id = id(e)
    del e
    collect()
    assert t.stats() == (1, 0)
    assert id(w.get_ref()) == prev_id
    assert t.owns_cpp(w.get_ref())
    assert type(w.get_ref()) is ty

    # Dropping the wrappers' shared_ptrs drops the PyObject reference and
    # the object is finally GC'ed
    del w, w2
    collect()
    assert t.stats() == (1, 1)


def test10_shared_from_this_create_in_python(clean):
    check_shared_from_this_py_owned(t.ExampleST, t.ExampleST, 42)

    # Subclass in C++
    t.reset()
    check_shared_from_this_py_owned(t.DerivedST, t.DerivedST, 30)

    # Subclass in Python
    class SubST(t.ExampleST):
        pass

    t.reset()
    check_shared_from_this_py_owned(SubST, SubST, 20)


def test11_shared_from_this_create_raw_in_cpp(clean):
    # Creating a raw pointer from C++ does not enable shared_from_this;
    # although the object is held by pointer rather than value, the logical
    # ownership transfers to Python and the behavior is equivalent to test10.
    # Once we get a shared_ptr it owns a reference to the Python object.
    check_shared_from_this_py_owned(t.ExampleST, t.ExampleST.make, 10)

    # Subclass in C++
    t.reset()
    check_shared_from_this_py_owned(t.DerivedST, t.DerivedST.make, 5)


def test12_shared_from_this_create_shared_in_cpp(clean):
    # Creating a shared_ptr from C++ enables shared_from_this. Now the
    # shared_ptr does not keep the Python object alive; it's directly
    # owning the ExampleST object on the C++ side.
    e = t.ExampleST.make_shared(10)
    assert e.value == 10
    assert e.has_shared_from_this()
    assert e.shared_from_this() is e  # same instance
    assert e.use_count() == 1
    assert not t.owns_cpp(e)
    if hasattr(sys, "getrefcount"):
        # One reference is held by our locals dict and one by the
        # arg to getrefcount
        rc = sys.getrefcount(e)
        assert rc == 2

    w = t.SharedWrapperST.from_existing(e)
    assert w.ptr is e
    # One shared_ptr whose lifetime is tied to e. And one inside the wrapper
    assert e.use_count() == 2

    # Drop the Python object; C++ object still remains owned by the wrapper
    del e
    collect()
    assert t.stats() == (1, 0)
    assert w.use_count() == 1

    # Get a new Python object reference; it will share ownership of the
    # same underlying C++ object
    e2 = w.get_own()
    assert not t.owns_cpp(e2)
    assert w.ptr is e2
    assert w.use_count() == 2

    del e2
    collect()
    assert t.stats() == (1, 0)
    assert w.use_count() == 1

    # Get a new C++-side reference
    w2 = t.SharedWrapperST.from_wrapper(w)
    assert w2.use_count() == 2
    assert t.same_owner(w, w2)

    # Get another one by roundtripping through Python.
    # The nanobind conversion to shared_ptr<ExampleST> should use the
    # existing shared_from_this shared_ptr
    w3 = t.SharedWrapperST(w.ptr)
    collect()  # on pypy the w.ptr temporary can stay alive
    assert w3.use_count() == 3
    assert t.same_owner(w2, w3)

    # Destroy everything
    assert t.stats() == (1, 0)
    del w, w2, w3
    collect()
    assert t.stats() == (1, 1)


def test13_shared_from_this_create_derived_in_cpp(clean):
    # This tests that keep_shared_from_this_alive is inherited by
    # derived classes properly

    # Pass shared_ptr<T> to Python
    e = t.DerivedST.make_shared(20)
    assert type(e) is t.DerivedST
    assert e.value == 20
    assert e.has_shared_from_this()
    assert not t.owns_cpp(e)
    assert e.use_count() == 1

    # Pass it back to C++
    w = t.SharedWrapperST(e)
    assert e.use_count() == w.use_count() == 2

    del e
    collect()
    assert t.stats() == (1, 0)
    assert w.use_count() == 1

    # Pass it back to Python as a raw pointer
    e = w.get_own()
    # ExampleST is not polymorphic, so the derived-class identity is
    # lost once the Python instance is destroyed
    assert type(e) is t.ExampleST
    assert not t.owns_cpp(e)
    assert w.use_count() == 2
    assert w.get_own() is e

    del e, w
    collect()
    assert t.stats() == (1, 1)


def test14_nullable_properties():
    a = t.ExampleWrapper()
    assert a.value is not None
    assert a.value.value == 5
    assert a.value_nullable is None
    a.value = t.Example(123)
    a.value_nullable = t.Example(456)
    assert a.value.value == 123
    assert a.value_nullable.value == 456
    a.value_nullable = None
    assert a.value_nullable is None
    with pytest.raises(TypeError):
        a.value = None
