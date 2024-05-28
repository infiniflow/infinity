import sys
import test_classes_ext as t
import pytest
from common import skip_on_pypy, collect

# Some helper declaration to check types across different Python versions
if sys.version_info < (3, 9):
    TYPING_TYPE = "typing.Type"
else:
    TYPING_TYPE = "type"


def optional(arg: str, /) -> str:
    if sys.version_info < (3, 10):
        return "typing.Optional[" + arg + "]"
    else:
        return arg + " | " + "None"


@pytest.fixture
def clean():
    collect()
    t.reset()


def assert_stats(**kwargs):
    collect()
    for k, v in t.stats().items():
        fail = False
        if k in kwargs:
            if v != kwargs[k]:
                fail = True
        elif v != 0:
            fail = True
        if fail:
            raise Exception(f"Mismatch for key {k}: {t.stats()}")


def test01_signature():
    assert t.Struct.__init__.__doc__ == (
        "__init__(self) -> None\n" "__init__(self, arg: int, /) -> None"
    )

    assert t.Struct.value.__doc__ == "value(self) -> int"
    assert t.Struct.create_move.__doc__ == "create_move() -> test_classes_ext.Struct"
    assert t.Struct.set_value.__doc__ == "set_value(self, value: int) -> None"
    assert t.Struct().set_value.__doc__ == "set_value(self, value: int) -> None"
    assert t.Struct.__doc__ == "Some documentation"
    assert t.Struct.static_test.__doc__ == (
        "static_test(arg: int, /) -> int\n" "static_test(arg: float, /) -> int"
    )


def test02_static_overload():
    assert t.Struct.static_test(1) == 1
    assert t.Struct.static_test(1.0) == 2


def test03_instantiate(clean):
    s1: t.Struct = t.Struct()
    assert s1.value() == 5
    s2 = t.Struct(10)
    assert s2.value() == 10
    del s1
    del s2
    assert_stats(default_constructed=1, value_constructed=1, destructed=2)


def test04_double_init():
    s = t.Struct()
    with pytest.warns(
        RuntimeWarning,
        match="nanobind: attempted to initialize an already-initialized instance of type",
    ):
        with pytest.raises(TypeError):
            s.__init__(3)


def test05_rv_policy(clean):
    s = t.Struct()
    assert s.self() is s
    assert s.none() is None
    del s
    assert_stats(default_constructed=1, destructed=1)

    # ------

    t.reset()
    assert t.Struct.create_take().value() == 10
    assert_stats(value_constructed=1, destructed=1)

    # ------

    t.reset()
    assert t.Struct.create_move().value() == 11
    assert_stats(value_constructed=1, move_constructed=1, destructed=2)

    # ------

    t.reset()
    assert t.Struct.create_reference().value() == 12
    assert_stats()

    # ------

    t.reset()
    assert t.Struct.create_copy().value() == 12
    assert_stats(copy_constructed=1, destructed=1)


def test06_reference_internal(clean):
    s = t.PairStruct()
    s1 = s.s1
    s2 = s.s2
    del s
    assert_stats(default_constructed=2)
    assert s2.value() == 5
    del s2

    assert_stats(default_constructed=2)

    assert s1.value() == 5
    del s1
    assert_stats(default_constructed=2, destructed=2)

    # ----

    s = t.PairStruct()
    s1 = s.s1
    del s1
    del s


def test07_big():
    x = [t.Big() for i in range(1024)]
    x2 = [t.BigAligned() for i in range(1024)]


def test08_inheritance():
    dog = t.Dog("woof")
    cat = t.Cat("meow")
    assert dog.name() == "Dog"
    assert cat.name() == "Cat"
    assert dog.what() == "woof"
    assert cat.what() == "meow"
    assert isinstance(dog, t.Animal) and isinstance(dog, t.Dog)
    assert isinstance(cat, t.Animal) and isinstance(cat, t.Cat)
    assert t.go(dog) == "Dog says woof"
    assert t.go(cat) == "Cat says meow"

    assert t.animal_passthrough(dog) is dog
    assert t.animal_passthrough(cat) is cat
    assert t.dog_passthrough(dog) is dog

    with pytest.raises(TypeError):
        t.dog_passthrough(cat)


def test09_method_vectorcall():
    out = []

    def f(a, b, c, d, e):
        out.append((a, b, c, d, e))

    class MyClass:
        def f(self, a, b, c, d, e):
            self.out = (a, b, c, d, e)

    t.call_function(f)

    i = MyClass()
    t.call_method(i)
    assert out == [(1, 2, "hello", True, 4)]
    assert i.out == (1, 2, "hello", True, 4)


def test10_trampoline(clean):
    for _ in range(10):

        class Dachshund(t.Animal):
            def __init__(self):
                super().__init__()

            def name(self):
                return "Dachshund"

            def what(self):
                return "yap"

        d = Dachshund()
        for _ in range(10):
            assert t.go(d) == "Dachshund says yap"

        assert t.animal_passthrough(d) is d

    a = 0

    class GenericAnimal(t.Animal):
        def what(self):
            return "goo"

        def void_ret(self):
            nonlocal a
            a += 1

        def name(self):
            return "Generic" + super().name()

    ga = GenericAnimal()
    assert t.go(ga) == "GenericAnimal says goo"
    assert t.void_ret(ga) is None
    assert a == 1
    assert t.animal_passthrough(ga) is ga

    del ga
    del d

    assert_stats(default_constructed=11, destructed=11)

    class GenericDog(t.Dog):
        pass

    d = GenericDog("GenericDog")
    assert t.dog_passthrough(d) is d
    assert t.animal_passthrough(d) is d


def test11_trampoline_failures():
    class Incomplete(t.Animal):
        def __init__(self):
            super().__init__()

        def void_ret(self):
            raise TypeError("propagating an exception")

    d = Incomplete()
    with pytest.raises(RuntimeError) as excinfo:
        t.go(d)
    assert (
        "test_classes.Incomplete::what()'): tried to call a pure virtual function!"
        in str(excinfo.value)
    )

    with pytest.raises(TypeError) as excinfo:
        t.void_ret(d)
    assert "propagating an exception" in str(excinfo.value)

    class Incomplete2(t.Animal):
        def __init__(self):
            pass  # Missing call to super().__init__()

        def name(self):
            return "a"

        def what(self):
            return "b"

    with pytest.warns(
        RuntimeWarning,
        match="nanobind: attempted to access an uninitialized instance of type",
    ):
        with pytest.raises(TypeError) as excinfo:
            t.go(Incomplete2())
    assert "incompatible function arguments" in str(excinfo.value)


def test12_large_pointers():
    import struct

    for i in range(1, 10):
        c = t.i2p(i)
        assert isinstance(c, t.Foo)
        assert t.p2i(c) == i

    large = (1 << (struct.calcsize("P") * 8)) - 1
    for i in range(large - 10, large):
        c = t.i2p(i)
        assert isinstance(c, t.Foo)
        assert t.p2i(c) == i


def test13_implicitly_convertible():
    assert t.get_d.__doc__ == "get_d(arg: test_classes_ext.D, /) -> int"
    a = t.A(1)
    b = t.B(2)
    b2 = t.B2(3)
    c = t.C(4)
    i = 5

    with pytest.raises(TypeError) as excinfo:
        t.get_d(c)
    assert str(excinfo.value) == (
        "get_d(): incompatible function arguments. The following argument types are supported:\n"
        "    1. get_d(arg: test_classes_ext.D, /) -> int\n"
        "\n"
        "Invoked with types: test_classes_ext.C"
    )
    with pytest.raises(TypeError):
        t.get_optional_d(c)

    for obj, expected in ((a, 11), (b, 102), (b2, 103), (i, 10005)):
        assert t.get_d(obj) == expected
        assert t.get_optional_d(obj) == expected
        # The -1's here are because nb::cast() won't implicit-convert to a
        # pointer because it would dangle
        assert t.get_d_via_cast(obj) == (expected, -1, expected, -1)
        assert t.get_d_via_try_cast(obj) == (expected, -1, expected, -1)

    d = t.D(5)
    assert t.get_d(d) == 10005
    assert t.get_optional_d(d) == 10005
    assert t.get_d_via_cast(d) == (10005, 10005, 10005, 10005)
    assert t.get_d_via_try_cast(d) == (10005, 10005, 10005, 10005)

    assert t.get_optional_d(None) == -1
    assert t.get_d_via_cast(c) == (-1, -1, -1, -1)
    assert t.get_d_via_try_cast(c) == (-1, -1, -1, -1)


def test14_operators():
    a = t.Int(1)
    b = t.Int(2)
    assert repr(a + b) == "3"
    with pytest.raises(TypeError) as excinfo:
        assert repr(a - b) == "3"
    assert "unsupported operand type" in str(excinfo.value)
    assert repr(a - 2) == "-1"
    a += b
    assert repr(a) == "3"
    assert repr(b) == "2"

    assert a.__add__("test") is NotImplemented


def test15_keep_alive_nbtype(clean):
    t.reset()
    s = t.Struct()
    a = t.Dog("Rufus")
    assert t.keep_alive_arg(s, a) is a
    assert t.keep_alive_arg(s, a) is a
    del s
    assert_stats(default_constructed=1)
    del a
    assert_stats(default_constructed=1, destructed=1)

    t.reset()
    s = t.Struct()
    a = t.Dog("Rufus")
    assert t.keep_alive_ret(a, s) is s
    assert t.keep_alive_ret(a, s) is s
    del a
    assert_stats(default_constructed=1)
    del s
    assert_stats(default_constructed=1, destructed=1)


def test16_keep_alive_custom(clean):
    constructed = 0
    destructed = 0

    class Struct:
        def __init__(self):
            nonlocal constructed
            constructed += 1

        def __del__(self):
            nonlocal destructed
            destructed += 1

    class Struct2:
        def __init__(self):
            pass

    s = Struct()
    a = Struct2()
    assert t.keep_alive_arg(s, a) is a
    del s
    collect()
    assert constructed == 1 and destructed == 0
    del a
    collect()
    assert constructed == 1 and destructed == 1

    s = Struct()
    a = Struct2()
    assert t.keep_alive_ret(a, s) is s
    del a
    collect()
    assert constructed == 2 and destructed == 1
    del s
    collect()
    assert constructed == 2 and destructed == 2

    with pytest.raises(RuntimeError) as excinfo:
        s = Struct()
        x = 5
        t.keep_alive_ret(x, s)

    assert "nanobind::detail::keep_alive(): could not create a weak reference!" in str(
        excinfo.value
    )


def f():
    pass


class MyClass:
    def f(self):
        pass

    class NestedClass:
        def f(self):
            pass


def test17_name_qualname_module():
    # First, check what CPython does
    assert f.__module__ == "test_classes"
    assert f.__name__ == "f"
    assert f.__qualname__ == "f"
    assert MyClass.__name__ == "MyClass"
    assert MyClass.__qualname__ == "MyClass"
    assert MyClass.__module__ == "test_classes"
    assert MyClass.f.__name__ == "f"
    assert MyClass.f.__qualname__ == "MyClass.f"
    assert MyClass.f.__module__ == "test_classes"
    assert MyClass().f.__name__ == "f"
    assert MyClass().f.__qualname__ == "MyClass.f"
    assert MyClass().f.__module__ == "test_classes"
    assert MyClass.NestedClass.__name__ == "NestedClass"
    assert MyClass.NestedClass.__qualname__ == "MyClass.NestedClass"
    assert MyClass.NestedClass.__module__ == "test_classes"
    assert MyClass.NestedClass.f.__name__ == "f"
    assert MyClass.NestedClass.f.__qualname__ == "MyClass.NestedClass.f"
    assert MyClass.NestedClass.f.__module__ == "test_classes"

    # Now, check the extension module
    assert t.f.__module__ == "test_classes_ext"
    assert t.f.__name__ == "f"
    assert t.f.__qualname__ == "f"
    assert type(t.f).__module__ == "nanobind"
    assert type(t.f).__name__ == "nb_func"
    assert type(t.f).__qualname__ == "nb_func"
    assert t.MyClass.__name__ == "MyClass"
    assert t.MyClass.__qualname__ == "MyClass"
    assert t.MyClass.__module__ == "test_classes_ext"
    assert t.MyClass.f.__name__ == "f"
    assert t.MyClass.f.__qualname__ == "MyClass.f"
    assert t.MyClass.f.__module__ == "test_classes_ext"
    assert t.MyClass().f.__name__ == "f"
    assert t.MyClass().f.__qualname__ == "MyClass.f"
    assert t.MyClass().f.__module__ == "test_classes_ext"
    assert type(t.MyClass.f).__module__ == "nanobind"
    assert type(t.MyClass.f).__name__ == "nb_method"
    assert type(t.MyClass.f).__qualname__ == "nb_method"
    assert t.MyClass.NestedClass.__name__ == "NestedClass"
    assert t.MyClass.NestedClass.__qualname__ == "MyClass.NestedClass"
    assert t.MyClass.NestedClass.__module__ == "test_classes_ext"
    assert t.MyClass.NestedClass.f.__name__ == "f"
    assert t.MyClass.NestedClass.f.__qualname__ == "MyClass.NestedClass.f"
    assert t.MyClass.NestedClass.f.__module__ == "test_classes_ext"


def test18_static_properties():
    assert t.StaticProperties.value == 23
    t.StaticProperties.value += 1
    assert t.StaticProperties.value == 24
    assert t.StaticProperties.get() == 24
    assert t.StaticProperties2.get() == 24
    t.StaticProperties2.value = 50
    assert t.StaticProperties2.get() == 50
    assert t.StaticProperties.get() == 50


@skip_on_pypy
def test19_static_properties_doc():
    import pydoc

    assert "Static property docstring" in pydoc.render_doc(t.StaticProperties2)


def test20_supplement():
    c = t.ClassWithSupplement()
    assert t.check_supplement(c)
    assert not t.check_supplement(t.Struct())


def test21_type_callback():
    o = t.ClassWithLen()
    assert len(o) == 123


def test22_low_level(clean):
    s1, s2, s3, s4 = t.test_lowlevel()
    assert s1.value() == 123 and s2.value() == 0 and s4.value() == 123
    assert s3.s1.value() == 123 and s3.s2.value() == 456
    assert s3.s1 is s4
    del s1
    del s2
    del s3
    collect()
    assert s4.value() == 123
    del s4
    assert_stats(
        value_constructed=3, copy_constructed=1, move_constructed=1, destructed=5
    )


def test23_handle_t(clean):
    assert (
        t.test_handle_t.__doc__
        == "test_handle_t(arg: test_classes_ext.Struct, /) -> object"
    )
    s = t.test_handle_t(t.Struct(5))
    assert s.value() == 5
    del s

    with pytest.raises(TypeError) as excinfo:
        t.test_handle_t("test")
    assert "incompatible function argument" in str(excinfo.value)
    assert_stats(value_constructed=1, destructed=1)


def test24_type_object_t(clean):
    assert (
        t.test_type_object_t.__doc__
        == f"test_type_object_t(arg: {TYPING_TYPE}[test_classes_ext.Struct], /) -> object"
    )

    assert t.test_type_object_t(t.Struct) is t.Struct

    with pytest.raises(TypeError):
        t.test_type_object_t(t.Struct())

    with pytest.raises(TypeError):
        t.test_type_object_t(int)


def test25_none_arg():
    with pytest.raises(TypeError):
        t.none_0(None)
    with pytest.raises(TypeError):
        t.none_1(None)
    with pytest.raises(TypeError):
        t.none_2(arg=None)
    assert t.none_3(None) is True
    assert t.none_4(arg=None) is True
    assert t.none_0.__doc__ == "none_0(arg: test_classes_ext.Struct, /) -> bool"
    assert t.none_1.__doc__ == "none_1(arg: test_classes_ext.Struct) -> bool"
    assert t.none_2.__doc__ == "none_2(arg: test_classes_ext.Struct) -> bool"
    opt_struct = optional("test_classes_ext.Struct")
    assert t.none_3.__doc__ == f"none_3(arg: {opt_struct}) -> bool"
    assert t.none_4.__doc__ == f"none_4(arg: {opt_struct}) -> bool"


def test26_is_final():
    with pytest.raises(TypeError) as excinfo:

        class MyType(t.FinalType):
            pass

    assert "The type 'test_classes_ext.FinalType' prohibits subclassing!" in str(
        excinfo.value
    )


def test27_dynamic_attr(clean):
    l = [None] * 100
    for i in range(100):
        l[i] = t.StructWithAttr(i)

    # Create a big reference cycle..
    for i in range(100):
        l[i].prev = l[i - 1]
        l[i].next = l[i + 1 if i < 99 else 0]
        l[i].t = t.StructWithAttr
        l[i].self = l[i]

    for i in range(100):
        assert l[i].value() == i
        assert l[i].self.value() == i
        assert l[i].prev.value() == (i - 1 if i > 0 else 99)
        assert l[i].next.value() == (i + 1 if i < 99 else 0)

    del l

    assert_stats(value_constructed=100, destructed=100)


def test28_copy_rvp():
    a = t.Struct.create_reference()
    b = t.Struct.create_copy()
    assert a is not b


def test29_pydoc():
    import pydoc

    assert "Some documentation" in pydoc.render_doc(t)


def test30_property_assignment_instance():
    s = t.PairStruct()
    s1 = t.Struct(123)
    s2 = t.Struct(456)
    s.s1 = s1
    s.s2 = s2
    assert s2 is not s.s2 and s1 is not s.s1
    assert s.s1.value() == 123
    assert s.s2.value() == 456
    assert s1.value() == 123
    assert s2.value() == 456


# cpyext reference cycles are not supported, see https://foss.heptapod.net/pypy/pypy/-/issues/3849
@skip_on_pypy()
def test31_cycle():
    a = t.Wrapper()
    a.value = a
    del a
    collect()


def test32_type_checks():
    v1 = 5
    v2 = t.Struct()

    assert t.is_int_1(v1) and not t.is_int_1(v2)
    assert t.is_int_2(v1) and not t.is_int_2(v2)
    assert not t.is_struct(v1) and t.is_struct(v2)


def test33_polymorphic_downcast():
    assert isinstance(t.factory(), t.Base)
    assert isinstance(t.factory_2(), t.Base)
    assert isinstance(t.polymorphic_factory(), t.PolymorphicSubclass)
    assert isinstance(t.polymorphic_factory_2(), t.PolymorphicBase)


def test34_trampoline_optimization():
    class Rufus(t.Dog):
        def __init__(self):
            super().__init__("woof")

        def name(self):
            return "Rufus"

    for i in range(2):
        d1 = t.Dog("woof")
        d2 = Rufus()

        if i == 0:
            assert t.go(d1) == "Dog says woof"
            assert t.go(d2) == "Rufus says woof"

        old = t.Dog.name
        try:
            t.Dog.name = lambda self: "Max"

            assert t.go(d1) == "Dog says woof"
            assert t.go(d2) == "Rufus says woof"
        finally:
            t.Dog.name = old


def test35_method_introspection():
    obj = t.Struct(5)
    m = obj.value
    assert m() == m.__call__() == 5
    assert hash(m) == m.__hash__()
    assert repr(m) == m.__repr__()
    assert "bound_method" in repr(m)
    assert m.__self__ is obj
    assert m.__func__ is t.Struct.value
    # attributes not defined by nb_bound_method are forwarded to nb_method:
    assert m.__name__ == "value"
    assert m.__qualname__ == "Struct.value"
    assert m.__module__ == t.__name__
    assert m.__doc__ == t.Struct.value.__doc__ == "value(self) -> int"


def test38_pickle(clean):
    import pickle

    s = t.Struct(123)
    s2 = pickle.dumps(s, protocol=pickle.HIGHEST_PROTOCOL)
    s3 = pickle.loads(s2)
    assert s.value() == s3.value()
    del s, s3

    assert_stats(value_constructed=1, pickled=1, unpickled=1, destructed=2)


def test39_try_cast(clean):
    s = t.Struct(123)

    assert_stats(value_constructed=1)
    t.reset()

    rv, s2 = t.try_cast_1(s)
    assert rv is True and s2 is not s and s.value() == 123 and s2.value() == 123
    del s2
    assert_stats(
        default_constructed=1, move_constructed=2, copy_assigned=1, destructed=3
    )
    t.reset()

    rv, s2 = t.try_cast_1(None)
    assert rv is False and s2 is not s and s2.value() == 5
    del s2
    assert_stats(
        default_constructed=1, move_constructed=2, copy_assigned=0, destructed=3
    )
    t.reset()

    rv, s2 = t.try_cast_2(s)
    assert rv is True and s2 is not s and s.value() == 123 and s2.value() == 123
    del s2
    assert_stats(
        default_constructed=1, move_constructed=2, copy_assigned=1, destructed=3
    )
    t.reset()

    rv, s2 = t.try_cast_2(None)
    assert rv is False and s2 is not s and s2.value() == 5
    del s2
    assert_stats(
        default_constructed=1, move_constructed=2, copy_assigned=0, destructed=3
    )
    t.reset()

    rv, s2 = t.try_cast_3(s)
    assert rv is True and s2 is s and s.value() == 123
    del s2
    assert_stats()
    t.reset()

    rv, s2 = t.try_cast_3(None)
    assert rv is True and s2 is None
    del s2
    assert_stats(
        default_constructed=0, move_constructed=0, copy_assigned=0, destructed=0
    )
    t.reset()

    rv, s2 = t.try_cast_2(1)
    assert rv is False
    del s2
    assert_stats(default_constructed=1, move_constructed=2, destructed=3)
    t.reset()

    rv, s2 = t.try_cast_3(1)
    assert rv is False and s2 is None
    del s2
    assert_stats()
    t.reset()

    rv, s2 = t.try_cast_4(s)
    assert rv is False and s2 == 0
    rv, s2 = t.try_cast_4(123)
    assert rv is True and s2 == 123
    del s, s2

    assert_stats(destructed=1)


def test40_slots():
    if not hasattr(t, "test_slots"):
        pytest.skip()
    assert t.test_slots() == (True, True, True)


def test41_implicit_conversion_keep_alive():
    # Check that keep_alive references implicitly constructed arguments
    # as opposed to the original function arguments
    collect()
    t.get_destructed()

    a = t.Struct(5)
    b = t.get_incrementing_struct_value(a)
    d1 = t.get_destructed()
    assert b.value() == 106
    del a
    collect()
    d2 = t.get_destructed()
    collect()
    del b
    collect()
    d3 = t.get_destructed()
    assert d1 == []
    assert d2 == [5]
    assert d3 == [106, 6]


def test42_weak_references():
    import weakref
    import gc
    import time

    o = t.StructWithWeakrefs(42)
    w = weakref.ref(o)
    assert w() is o
    del o
    gc.collect()
    gc.collect()
    assert w() is None

    p = t.StructWithWeakrefsAndDynamicAttrs(43)
    p.a_dynamic_attr = 101
    w = weakref.ref(p)
    assert w() is p
    assert w().a_dynamic_attr == 101
    del p
    gc.collect()
    gc.collect()
    assert w() is None


def test43_union():
    u = t.Union()
    u.i = 42
    assert u.i == 42

    u.f = 2.125
    assert u.f == 2.125


def test44_dynamic_attr_has_dict():
    s = t.StructWithAttr(5)
    assert s.__dict__ == {}
    s.a_dynamic_attr = 101
    assert s.__dict__ == {"a_dynamic_attr": 101}


def test45_hidden_base():
    s = t.BoundDerived()
    assert s.value == 10
    s.value = 5
    assert s.prop == 5
    s.prop = 20
    assert s.value == 20
    assert s.get_answer() == 200
    assert s.polymorphic() == 20


def test46_custom_new():
    import gc

    u1 = t.UniqueInt(10)
    assert u1.value() == 10 and u1.lookups() == 1

    u2 = t.UniqueInt(10)
    assert u1 is u2
    assert u1.lookups() == 2

    # test alternate constructor
    assert t.UniqueInt("10") is u1
    assert u1.lookups() == 3

    u3 = t.UniqueInt(20)
    assert u1 is not u3
    assert u3.value() == 20 and u3.lookups() == 1

    del u1
    assert u2.lookups() == 3
    assert u2 is t.UniqueInt(10)
    assert u2.lookups() == 4

    del u2
    gc.collect()
    gc.collect()

    u4 = t.UniqueInt(10)
    assert u4.value() == 10 and u4.lookups() == 1

    # As if unpickling:
    empty = t.UniqueInt.__new__(t.UniqueInt)
    with pytest.warns(RuntimeWarning, match="access an uninitialized instance"):
        with pytest.raises(TypeError):
            empty.value()

    # Make sure pickle support doesn't allow no-args construction by mistake
    with pytest.raises(TypeError):
        t.UniqueInt()

    with pytest.raises(RuntimeError):
        t.UniqueInt.__new__(int)
