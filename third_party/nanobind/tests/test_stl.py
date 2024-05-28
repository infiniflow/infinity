import test_stl_ext as t
import typing
import pytest
import sys
from common import collect, skip_on_pypy

# Some helper declaration to check types across different Python versions
if sys.version_info < (3, 9):
    TYPING_DICT = "typing.Dict"
    TYPING_SET = "typing.Set"
    TYPING_ABC = "typing"
    TYPING_ABSTRACT_SET = "typing.AbstractSet"
else:
    TYPING_DICT = "dict"
    TYPING_SET = "set"
    TYPING_ABC = "collections.abc"
    TYPING_ABSTRACT_SET = "collections.abc.Set"


def optional(arg: str, /) -> str:
    if sys.version_info < (3, 10):
        return "typing.Optional[" + arg + "]"
    else:
        return arg + " | " + "None"


def union(*args: str) -> str:
    if sys.version_info < (3, 10):
        return "typing.Union[" + ", ".join(args) + "]"
    else:
        return " | ".join(args)


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


# ------------------------------------------------------------------
# The following aren't strictly STL tests, but they are helpful in
# ensuring that move constructors/copy constructors of bound C++ types
# are properly triggered, which the STL type casters depend on.
# ------------------------------------------------------------------


def test01_movable_return(clean):
    assert t.return_movable().value == 5
    assert_stats(default_constructed=1, move_constructed=1, destructed=2)


def test02_movable_return_ptr(clean):
    assert t.return_movable_ptr().value == 5
    assert_stats(default_constructed=1, destructed=1)


def test03_movable_in_value(clean):
    s = t.Movable()
    t.movable_in_value(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test04_movable_in_lvalue_ref(clean):
    s = t.Movable()
    t.movable_in_lvalue_ref(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, destructed=1)


def test05_movable_in_ptr(clean):
    s = t.Movable()
    t.movable_in_ptr(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, destructed=1)


def test06_movable_in_rvalue_ref(clean):
    s = t.Movable()
    t.movable_in_rvalue_ref(s)
    assert s.value == 0
    del s
    assert_stats(default_constructed=1, move_constructed=1, destructed=2)


def test07_copyable_return(clean):
    assert t.return_copyable().value == 5
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test08_copyable_return_ptr(clean):
    assert t.return_copyable_ptr().value == 5
    assert_stats(default_constructed=1, destructed=1)


def test09_copyable_in_value(clean):
    s = t.Copyable()
    t.copyable_in_value(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test10_copyable_in_lvalue_ref(clean):
    s = t.Copyable()
    t.copyable_in_lvalue_ref(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, destructed=1)


def test11_copyable_in_ptr(clean):
    s = t.Copyable()
    t.copyable_in_ptr(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, destructed=1)


def test12_copyable_in_rvalue_ref(clean):
    s = t.Copyable()
    t.copyable_in_rvalue_ref(s)
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


# ------------------------------------------------------------------


def test13_tuple_movable_return(clean):
    assert t.tuple_return_movable()[0].value == 5
    assert_stats(default_constructed=1, move_constructed=2, destructed=3)


def test14_tuple_movable_return_ptr(clean):
    assert t.return_movable_ptr().value == 5
    assert_stats(default_constructed=1, destructed=1)


def test15_tuple_movable_in_value(clean):
    s = t.Movable()
    t.tuple_movable_in_value((s,))
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test16_tuple_movable_in_lvalue_ref(clean):
    s = t.Movable()
    t.tuple_movable_in_lvalue_ref((s,))
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, destructed=1)


def test17_tuple_movable_in_lvalue_ref_2(clean):
    s = t.Movable()
    t.tuple_movable_in_lvalue_ref_2((s,))
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test18_tuple_movable_in_ptr(clean):
    s = t.Movable()
    t.tuple_movable_in_ptr((s,))
    assert s.value == 5
    del s
    assert_stats(default_constructed=1, destructed=1)


def test19_tuple_movable_in_rvalue_ref(clean):
    s = t.Movable()
    t.tuple_movable_in_rvalue_ref((s,))
    assert s.value == 0
    del s
    assert_stats(default_constructed=1, move_constructed=1, destructed=2)


def test20_tuple_movable_in_rvalue_ref_2(clean):
    s = t.Movable()
    t.tuple_movable_in_rvalue_ref_2((s,))
    assert s.value == 5
    del s
    assert_stats(
        default_constructed=1, copy_constructed=1, move_constructed=1, destructed=3
    )


# ------------------------------------------------------------------


def test21_tuple_pair_basic():
    assert t.empty_tuple(()) == ()
    assert t.swap_tuple((1, 2.5)) == (2.5, 1)
    assert t.swap_pair((1, 2.5)) == (2.5, 1)


# ------------------------------------------------------------------


def test22_vec_return_movable(clean):
    for i, x in enumerate(t.vec_return_movable()):
        assert x.value == i
    del x
    assert_stats(value_constructed=10, move_constructed=10, destructed=20)


def test23_vec_return_copyable(clean):
    for i, x in enumerate(t.vec_return_copyable()):
        assert x.value == i
    del x
    assert_stats(value_constructed=10, copy_constructed=20, destructed=30)


def test24_vec_movable_in_value(clean):
    t.vec_movable_in_value([t.Movable(i) for i in range(10)])
    assert_stats(value_constructed=10, copy_constructed=10, destructed=20)

    # Test that None values don't cause a crash
    with pytest.raises(TypeError):
        t.vec_movable_in_value([None])


def test25_vec_movable_in_value(clean):
    t.vec_copyable_in_value([t.Copyable(i) for i in range(10)])
    assert_stats(value_constructed=10, copy_constructed=10, destructed=20)


def test26_vec_movable_in_lvalue_ref(clean):
    t.vec_movable_in_lvalue_ref([t.Movable(i) for i in range(10)])
    assert_stats(value_constructed=10, copy_constructed=10, destructed=20)


def test27_vec_movable_in_ptr_2(clean):
    t.vec_movable_in_ptr_2([t.Movable(i) for i in range(10)])
    assert_stats(value_constructed=10, destructed=10)

    # Test that None values are permitted when casting to pointer;
    # instead we reach 'if (x.size() != 10) fail();' in the bound function
    with pytest.raises(RuntimeError):
        t.vec_movable_in_ptr_2([None])


def test28_vec_movable_in_rvalue_ref(clean):
    t.vec_movable_in_rvalue_ref([t.Movable(i) for i in range(10)])
    assert_stats(value_constructed=10, copy_constructed=10, destructed=20)


def test29_opaque_vector():
    f = t.float_vec()
    assert f.size() == 0
    assert isinstance(f, t.float_vec)
    f.push_back(1)
    assert f.size() == 1


def test30_std_function():
    assert t.return_empty_function() is None
    assert t.return_function()(3) == 8
    assert t.call_function(lambda x: 5 + x, 3) == 8

    with pytest.raises(TypeError) as excinfo:
        assert t.call_function(5, 3) == 8
    assert "incompatible function arguments" in str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        assert t.call_function(lambda x, y: x + y, 3) == 8
    assert "missing 1 required positional argument" in str(excinfo.value)

    l = []

    def f():
        l.append(1)

    f2 = t.return_void_function(f)
    f2()
    assert l == [1]


def test31_std_function_roundtrip():
    def f():
        l.append(1)

    f2 = t.return_void_function(f)
    assert f2 is f


# cpyext reference cycles are not supported, see https://foss.heptapod.net/pypy/pypy/-/issues/3849
@skip_on_pypy
def test32_std_function_gc():
    class A(t.FuncWrapper):
        pass

    assert t.FuncWrapper.alive == 0
    a = A()
    assert t.FuncWrapper.alive == 1
    del a
    collect()
    assert t.FuncWrapper.alive == 0

    # A class -> function -> class cyclic reference
    class B(t.FuncWrapper):
        def __init__(self):
            super().__init__()

            def f():
                print(self.f)

            self.f = f

    assert t.FuncWrapper.alive == 0
    b = B()
    assert t.FuncWrapper.alive == 1
    del b
    collect()
    assert t.FuncWrapper.alive == 0


def test33_vec_type_check():
    with pytest.raises(TypeError) as excinfo:
        t.vec_movable_in_value(0)


def test34_list():
    assert t.identity_list([]) == []
    assert t.identity_list([1, 2, 3]) == [1, 2, 3]
    assert t.identity_list(()) == []
    assert t.identity_list((1, 2, 3)) == [1, 2, 3]


def test35_string_and_string_view():
    assert t.identity_string("") == ""
    assert t.identity_string("orange") == "orange"
    assert t.identity_string("橘子") == "橘子"
    assert t.identity_string("ส้ม") == "ส้ม"
    assert t.identity_string("البرتقالي") == "البرتقالي"
    assert t.identity_string("🍊") == "🍊"

    assert t.identity_string_view("") == ""
    assert t.identity_string_view("orange") == "orange"
    assert t.identity_string_view("橘子") == "橘子"
    assert t.identity_string_view("ส้ม") == "ส้ม"
    assert t.identity_string_view("البرتقالي") == "البرتقالي"
    assert t.identity_string_view("🍊") == "🍊"


def test36_std_optional_copyable(clean):
    t.optional_copyable(t.Copyable())
    opt_copyable = optional("test_stl_ext.Copyable")
    assert t.optional_copyable.__doc__ == (
        f"optional_copyable(x: {opt_copyable}) -> None"
    )
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test37_std_optional_copyable_ptr(clean):
    t.optional_copyable_ptr(t.Copyable())
    opt_copyable = optional("test_stl_ext.Copyable")
    assert t.optional_copyable_ptr.__doc__ == (
        f"optional_copyable_ptr(x: {opt_copyable}) -> None"
    )
    assert_stats(default_constructed=1, destructed=1)


def test38_std_optional_none():
    t.optional_none(None)
    assert t.optional_cstr(None) == "none"
    assert t.optional_cstr("hi") == "hi"


def test39_std_optional_ret_opt_movable(clean):
    assert t.optional_ret_opt_movable().value == 5
    opt_movable = optional("test_stl_ext.Movable")
    assert t.optional_ret_opt_movable.__doc__ == (
        f"optional_ret_opt_movable() -> {opt_movable}"
    )
    assert_stats(default_constructed=1, move_constructed=2, destructed=3)


def test40_std_optional_ret_opt_movable_ptr(clean):
    assert t.optional_ret_opt_movable_ptr().value == 5
    assert_stats(default_constructed=1, destructed=1)


def test41_std_optional_ret_opt_none():
    assert t.optional_ret_opt_none() is None


def test42_std_optional_unbound_type():
    for method_name in (
        "optional_unbound_type",
        "optional_unbound_type_with_nullopt_as_default",
    ):
        method = getattr(t, method_name)
        assert method(3) == 3
        assert method(None) is None
        assert method() is None
        opt_int = optional("int")
        assert method.__doc__ == (f"{method_name}(x: {opt_int} = None) -> {opt_int}")


def test42a_std_optional_non_assignable():
    assert t.optional_non_assignable(t.NonAssignable()).value == 5


def test43_std_variant_copyable(clean):
    t.variant_copyable(t.Copyable())
    t.variant_copyable(5)
    arg_t = union('test_stl_ext.Copyable', 'int')
    assert t.variant_copyable.__doc__ == (
        f"variant_copyable(arg: {arg_t}, /) -> None"
    )
    assert_stats(default_constructed=3, copy_assigned=1, destructed=3)


def test44_std_variant_copyable_none(clean):
    t.variant_copyable_none(t.Copyable())
    t.variant_copyable_none(5)
    t.variant_copyable_none(None)
    arg_t = optional(union("int", "test_stl_ext.Copyable"))
    assert t.variant_copyable_none.__doc__ == (
        f"variant_copyable_none(x: {arg_t}) -> None"
    )
    assert_stats(default_constructed=1, copy_constructed=1, destructed=2)


def test45_std_variant_copyable_ptr(clean):
    t.variant_copyable_ptr(t.Copyable())
    t.variant_copyable_ptr(5)
    arg_t = union("test_stl_ext.Copyable", "int")
    assert t.variant_copyable_ptr.__doc__ == (
        f"variant_copyable_ptr(arg: {arg_t}, /) -> None"
    )
    assert_stats(default_constructed=1, destructed=1)


def test46_std_variant_copyable_ptr_none(clean):
    t.variant_copyable_ptr_none(t.Copyable())
    t.variant_copyable_ptr_none(5)
    t.variant_copyable_ptr_none(None)
    arg_t = optional(union("test_stl_ext.Copyable", "int"))
    assert t.variant_copyable_ptr_none.__doc__ == (
        f"variant_copyable_ptr_none(x: {arg_t}) -> None"
    )
    assert_stats(default_constructed=1, destructed=1)


def test47_std_variant_ret_var_copyable():
    assert t.variant_ret_var_copyable().value == 5
    rv_t = union("test_stl_ext.Copyable", "int")
    assert t.variant_ret_var_copyable.__doc__ == (
        f"variant_ret_var_copyable() -> {rv_t}"
    )


def test48_std_variant_ret_var_none():
    assert t.variant_ret_var_none() is None

    rv_t = union("None", "test_stl_ext.Copyable", "int")
    print(t.variant_ret_var_none.__doc__)
    assert t.variant_ret_var_none.__doc__ == (f"variant_ret_var_none() -> {rv_t}")


def test49_std_variant_unbound_type():
    assert t.variant_unbound_type() is None
    assert t.variant_unbound_type(None) is None
    assert t.variant_unbound_type([5]) == [5]
    assert t.variant_unbound_type((1, 2, 3)) == (1, 2, 3)
    assert t.variant_unbound_type(5) == 5
    arg_t = optional(union("list", "tuple", "int"))
    rv_t = union("None", "list", "tuple", "int")
    assert t.variant_unbound_type.__doc__ == (
        f"variant_unbound_type(x: {arg_t} = None) -> {rv_t}"
    )


def test50_map_return_movable_value():
    for i, (k, v) in enumerate(sorted(t.map_return_movable_value().items())):
        assert k == chr(ord("a") + i)
        assert v.value == i
        assert t.map_return_movable_value.__doc__ == (
            f"map_return_movable_value() -> {TYPING_DICT}[str, test_stl_ext.Movable]"
        )


def test51_map_return_copyable_value():
    for i, (k, v) in enumerate(sorted(t.map_return_copyable_value().items())):
        assert k == chr(ord("a") + i)
        assert v.value == i
        assert t.map_return_copyable_value.__doc__ == (
            f"map_return_copyable_value() -> {TYPING_DICT}[str, test_stl_ext.Copyable]"
        )


def test52_map_movable_in_value():
    t.map_movable_in_value(dict([(chr(ord("a") + i), t.Movable(i)) for i in range(10)]))
    assert t.map_movable_in_value.__doc__ == (
        f"map_movable_in_value(x: {TYPING_ABC}.Mapping[str, test_stl_ext.Movable]) -> None"
    )


def test53_map_copyable_in_value():
    t.map_copyable_in_value(
        dict([(chr(ord("a") + i), t.Copyable(i)) for i in range(10)])
    )
    assert t.map_copyable_in_value.__doc__ == (
        f"map_copyable_in_value(x: {TYPING_ABC}.Mapping[str, test_stl_ext.Copyable]) -> None"
    )


def test54_map_movable_in_lvalue_ref():
    t.map_movable_in_lvalue_ref(
        dict([(chr(ord("a") + i), t.Movable(i)) for i in range(10)])
    )
    assert t.map_movable_in_lvalue_ref.__doc__ == (
        f"map_movable_in_lvalue_ref(x: {TYPING_ABC}.Mapping[str, test_stl_ext.Movable]) -> None"
    )


def test55_map_movable_in_rvalue_ref():
    t.map_movable_in_rvalue_ref(
        dict([(chr(ord("a") + i), t.Movable(i)) for i in range(10)])
    )
    assert t.map_movable_in_rvalue_ref.__doc__ == (
        f"map_movable_in_rvalue_ref(x: {TYPING_ABC}.Mapping[str, test_stl_ext.Movable]) -> None"
    )


def test56_map_movable_in_ptr():
    t.map_movable_in_ptr(dict([(chr(ord("a") + i), t.Movable(i)) for i in range(10)]))
    assert t.map_movable_in_ptr.__doc__ == (
        f"map_movable_in_ptr(x: {TYPING_ABC}.Mapping[str, test_stl_ext.Movable]) -> None"
    )


def test57_map_return_readonly_value():
    for i, (k, v) in enumerate(sorted(t.map_return_readonly_value().map.items())):
        assert k == chr(ord("a") + i)
        assert v == i
    assert t.map_return_readonly_value.__doc__ == (
        "map_return_readonly_value() -> test_stl_ext.StructWithReadonlyMap"
    )


def test58_array():
    o = t.array_out()
    assert isinstance(o, list) and o == [1, 2, 3]
    assert t.array_in([1, 2, 3]) == 6
    assert t.array_in((1, 2, 3)) == 6
    with pytest.raises(TypeError) as excinfo:
        assert t.array_in((1, 2, 3, 4)) == 6
    assert "incompatible function arguments" in str(excinfo.value)


def test59_map_movable_in_failure():
    with pytest.raises(TypeError) as excinfo:
        t.map_copyable_in_value({1: 2})
    assert "incompatible function arguments" in str(excinfo.value)


def test60_set_return_value():
    for i, k in enumerate(sorted(t.set_return_value())):
        assert k == chr(ord("a") + i)
    for i, k in enumerate(sorted(t.unordered_set_return_value())):
        assert k == chr(ord("a") + i)
    assert t.set_return_value.__doc__ == (f"set_return_value() -> {TYPING_SET}[str]")
    assert t.unordered_set_return_value.__doc__ == (
        f"unordered_set_return_value() -> {TYPING_SET}[str]"
    )


def test61_set_in_value():
    t.set_in_value(set([chr(ord("a") + i) for i in range(10)]))
    t.unordered_set_in_value(set([chr(ord("a") + i) for i in range(10)]))
    assert t.set_in_value.__doc__ == (
        f"set_in_value(x: {TYPING_ABSTRACT_SET}[str]) -> None"
    )
    assert t.unordered_set_in_value.__doc__ == (
        f"unordered_set_in_value(x: {TYPING_ABSTRACT_SET}[str]) -> None"
    )


def test62_set_in_lvalue_ref():
    t.set_in_lvalue_ref(set([chr(ord("a") + i) for i in range(10)]))
    assert t.set_in_lvalue_ref.__doc__ == (
        f"set_in_lvalue_ref(x: {TYPING_ABSTRACT_SET}[str]) -> None"
    )


def test63_set_in_rvalue_ref():
    t.set_in_rvalue_ref(set([chr(ord("a") + i) for i in range(10)]))
    assert t.set_in_rvalue_ref.__doc__ == (
        f"set_in_rvalue_ref(x: {TYPING_ABSTRACT_SET}[str]) -> None"
    )


def test64_set_in_failure():
    with pytest.raises(TypeError) as excinfo:
        t.set_in_value(set([i for i in range(10)]))
    assert "incompatible function arguments" in str(excinfo.value)


def test65_class_with_movable_field(clean):
    cwmf = t.ClassWithMovableField()
    m1 = t.Movable(1)
    m2 = t.Movable(2)

    assert_stats(value_constructed=2)

    cwmf.movable = [m1, m2]

    assert_stats(value_constructed=2, copy_constructed=2)

    del m1, m2
    collect()

    assert_stats(value_constructed=2, copy_constructed=2, destructed=2)

    del cwmf
    collect()

    assert_stats(value_constructed=2, copy_constructed=2, destructed=4)


def test66_replace_extension():
    from pathlib import Path

    if not hasattr(t, "replace_extension"):
        pytest.skip("filesystem tests not compiled")

    filename = Path("test.txt")
    assert t.replace_extension(filename, ".obj") == filename.with_suffix(".obj")

    filename = Path("🍊.html")
    assert t.replace_extension(filename, ".svg") == filename.with_suffix(".svg")

    class PseudoStrPath:
        def __fspath__(self):
            return "foo/bar"

    class PseudoBytesPath:
        def __fspath__(self):
            return b"foo/bar"

    assert t.parent_path(Path("foo/bar")) == Path("foo")
    assert t.parent_path("foo/bar") == Path("foo")
    assert t.parent_path(b"foo/bar") == Path("foo")
    assert t.parent_path(PseudoStrPath()) == Path("foo")
    assert t.parent_path(PseudoBytesPath()) == Path("foo")


def test67_vector_bool():
    bool_vector = [True, False, True, False]
    result = t.flip_vector_bool(bool_vector)
    assert result == [not x for x in bool_vector]


def test68_complex_value():
    # double: 64bits
    assert t.complex_value_double(1.0) == 1.0
    assert t.complex_value_double(1.0j) == 1.0j
    assert t.complex_value_double(0.0) == 0.0
    assert t.complex_value_double(0.0j) == 0.0j
    assert t.complex_value_double(0) == 0
    assert t.complex_value_float(1.0) == 1.0
    assert t.complex_value_float(1.0j) == 1.0j
    assert t.complex_value_float(0.0) == 0.0
    assert t.complex_value_float(0.0j) == 0.0j
    assert t.complex_value_float(0) == 0

    val_64 = 2.7 - 3.2j
    val_32 = 2.700000047683716 - 3.200000047683716j
    assert val_64 != val_32

    assert t.complex_value_float(val_32) == val_32
    assert t.complex_value_float(val_64) == val_32
    assert t.complex_value_double(val_32) == val_32
    assert t.complex_value_double(val_64) == val_64

    with pytest.raises(TypeError, match="incompatible function arguments"):
        t.complex_value_float([])

    try:
        import numpy as np

        assert t.complex_value_float(np.complex64(val_32)) == val_32
        assert t.complex_value_float(np.complex64(val_64)) == val_32
        assert t.complex_value_double(np.complex64(val_32)) == val_32
        assert t.complex_value_double(np.complex64(val_64)) == val_32
        assert t.complex_value_float(np.complex128(val_32)) == val_32
        assert t.complex_value_float(np.complex128(val_64)) == val_32
        assert t.complex_value_double(np.complex128(val_32)) == val_32
        assert t.complex_value_double(np.complex128(val_64)) == val_64
    except ImportError:
        pass


def test69_complex_array():
    val1_64 = 2.7 - 3.2j
    val1_32 = 2.700000047683716 - 3.200000047683716j
    val2_64 = 3.1415
    val2_32 = 3.1414999961853027 + 0j

    # test 64 bit casts
    assert t.complex_array_double([val1_64, -1j, val2_64]) == [
        val1_64,
        -0 - 1j,
        val2_64,
    ]

    # test 32 bit casts
    assert t.complex_array_float([val1_64, -1j, val2_64]) == [
        val1_32,
        (-0 - 1j),
        val2_32,
    ]

    with pytest.raises(TypeError, match="incompatible function arguments"):
        t.complex_array_float([[]])

    try:
        import numpy as np

        # test 64 bit casts
        assert t.complex_array_double(np.array([val1_64, -1j, val2_64])) == [
            val1_64,
            -0 - 1j,
            val2_64,
        ]
        assert t.complex_array_double(
            np.array([val1_64, -1j, val2_64], dtype=np.complex128)
        ) == [val1_64, -0 - 1j, val2_64]
        assert t.complex_array_double(
            np.array([val1_64, -1j, val2_64], dtype=np.complex64)
        ) == [val1_32, -0 - 1j, val2_32]

        # test 32 bit casts
        assert t.complex_array_float(np.array([val1_64, -1j, val2_64])) == [
            val1_32,
            (-0 - 1j),
            val2_32,
        ]
        assert t.complex_array_float(
            np.array([val1_64, -1j, val2_64], dtype=np.complex128)
        ) == [val1_32, (-0 - 1j), val2_32]
        assert t.complex_array_float(
            np.array([val1_64, -1j, val2_64], dtype=np.complex64)
        ) == [val1_32, (-0 - 1j), val2_32]
    except ImportError:
        pass


def test70_vec_char():
    assert isinstance(t.vector_str("123"), str)
    assert isinstance(t.vector_str(["123", "345"]), list)


def test71_null_input():
    with pytest.raises(TypeError):
        t.vec_movable_in_value([None])
    with pytest.raises(TypeError):
        t.map_copyable_in_value({"a": None})

@skip_on_pypy # PyPy fails this test on Windows :-(
def test72_wstr():
    assert t.pass_wstr('🎈') == '🎈'
