import test_functions_ext as t
import pytest
import sys
import re

# Some helper declaration to check types across different Python versions
if sys.version_info < (3, 9):
    TYPING_TUPLE = "typing.Tuple"
else:
    TYPING_TUPLE = "tuple"


def fail_fn():  # used in test_30
    raise RuntimeError("Foo")


def test01_capture():
    # Functions with and without capture object of different sizes
    assert t.test_01() is None
    assert t.test_02(5, 3) == 2
    assert t.test_03(5, 3) == 44
    assert t.test_04() == 60


def test02_default_args():
    # Default arguments
    assert t.test_02() == 7
    assert t.test_02(7) == 6


def test03_kwargs():
    # Basic use of keyword arguments
    assert t.test_02(3, 5) == -2
    assert t.test_02(3, down=5) == -2
    assert t.test_02(down=5, up=3) == -2

    # Make sure non-interned keyword names work also
    i_cant_believe_its_not_down = "".join("down")
    assert i_cant_believe_its_not_down is not "down"
    assert t.test_02(**{i_cant_believe_its_not_down: 5, "up": 3}) == -2
    assert t.test_02(**{i_cant_believe_its_not_down: 5}) == 3
    with pytest.raises(TypeError):
        t.test_02(unexpected=27)
    with pytest.raises(TypeError):
        t.test_02(**{i_cant_believe_its_not_down: None})


def test04_overloads():
    assert t.test_05(0) == 1
    assert t.test_05(0.0) == 2


def test05_signature():
    assert t.test_01.__doc__ == "test_01() -> None"
    assert t.test_02.__doc__ == "test_02(up: int = 8, down: int = 1) -> int"
    assert t.test_05.__doc__ == (
        "test_05(arg: int, /) -> int\n"
        "test_05(arg: float, /) -> int\n"
        "\n"
        "Overloaded function.\n"
        "\n"
        "1. ``test_05(arg: int, /) -> int``\n"
        "\n"
        "doc_1\n"
        "\n"
        "2. ``test_05(arg: float, /) -> int``\n"
        "\n"
        "doc_2"
    )

    assert t.test_07.__doc__ == (
        f"test_07(arg0: int, arg1: int, /, *args, **kwargs) -> {TYPING_TUPLE}[int, int]\n"
        f"test_07(a: int, b: int, *myargs, **mykwargs) -> {TYPING_TUPLE}[int, int]"
    )


def test06_signature_error():
    with pytest.raises(TypeError) as excinfo:
        t.test_05("x", y=4)
    assert str(excinfo.value) == (
        "test_05(): incompatible function arguments. The "
        "following argument types are supported:\n"
        "    1. test_05(arg: int, /) -> int\n"
        "    2. test_05(arg: float, /) -> int\n\n"
        "Invoked with types: str, kwargs = { y: int }"
    )


def test07_raises():
    with pytest.raises(RuntimeError) as excinfo:
        t.test_06()
    assert str(excinfo.value) == "oops!"


def test08_args_kwargs():
    assert t.test_07(1, 2) == (0, 0)
    assert t.test_07(a=1, b=2) == (0, 0)
    assert t.test_07(a=1, b=2, c=3) == (0, 1)
    assert t.test_07(1, 2, 3, c=4) == (1, 1)
    assert t.test_07(1, 2, 3, 4, c=5, d=5) == (2, 2)


def test09_maketuple():
    assert t.test_tuple() == ("Hello", 123)
    with pytest.raises(RuntimeError) as excinfo:
        assert t.test_bad_tuple()
    value = str(excinfo.value)
    assert value == "std::bad_cast" or value == "bad cast"


def test10_cpp_call_simple():
    result = []

    def my_callable(a, b):
        result.append((a, b))

    t.test_call_2(my_callable)
    assert result == [(1, 2)]

    with pytest.raises(TypeError) as excinfo:
        t.test_call_1(my_callable)
    assert "my_callable() missing 1 required positional argument: 'b'" in str(
        excinfo.value
    )
    assert result == [(1, 2)]


def test11_call_complex():
    result = []

    def my_callable(*args, **kwargs):
        result.append((args, kwargs))

    t.test_call_extra(my_callable)
    assert result == [((1, 2), {"extra": 5})]

    result.clear()
    t.test_call_extra(my_callable, 5, 6, hello="world")
    assert result == [((1, 2, 5, 6), {"extra": 5, "hello": "world"})]


def test12_list_tuple_manipulation():
    li = [1, 5, 6, 7]
    t.test_list(li)
    assert li == [1, 5, 123, 7, 19]

    tu = (1, 5, 6, 7)
    assert t.test_tuple(tu) == 19
    assert tu == (1, 5, 6, 7)


def test13_call_guard():
    assert t.call_guard_value() == 0
    assert t.test_call_guard() == 1
    assert t.call_guard_value() == 2
    assert t.test_call_guard_wrapper_rvalue_ref(1) == 1
    assert not t.test_release_gil()


def test14_print(capsys):
    t.test_print()
    captured = capsys.readouterr()
    assert captured.out == "Test 1\nTest 2\n"


def test15_iter():
    assert t.test_iter(()) == []
    assert t.test_iter((1,)) == [1]
    assert t.test_iter((1, 2)) == [1, 2]
    assert t.test_iter((1, 2, 3)) == [1, 2, 3]


def test16_iter_tuple():
    assert t.test_iter_tuple(()) == []
    assert t.test_iter_tuple((1,)) == [1]
    assert t.test_iter_tuple((1, 2)) == [1, 2]
    assert t.test_iter_tuple((1, 2, 3)) == [1, 2, 3]


def test17_iter_tuple():
    assert t.test_iter_list([]) == []
    assert t.test_iter_list([1]) == [1]
    assert t.test_iter_list([1, 2]) == [1, 2]
    assert t.test_iter_list([1, 2, 3]) == [1, 2, 3]


def test18_raw_doc():
    assert (
        t.test_08.__doc__
        == """test_08(arg: int, /) -> int
test_08(x: typing.Annotated[float, 'foo']) -> int

Overloaded function.

1. ``test_08(arg: int, /) -> int``

first docstring

2. ``test_08(x: typing.Annotated[float, 'foo']) -> int``

another docstring"""
    )


def test19_type_check_manual():
    assert t.test_09.__doc__ == "test_09(arg: type, /) -> bool"

    assert t.test_09(bool) is True
    assert t.test_09(int) is False
    with pytest.raises(TypeError) as excinfo:
        assert t.test_09(True)
    assert "incompatible function arguments" in str(excinfo.value)


def test20_dict_iterator():
    assert t.test_10({}) == {}
    assert t.test_10({1: 2}) == {1: 2}
    assert t.test_10({1: 2, 3: 4}) == {1: 2, 3: 4}
    assert t.test_10({1: 2, 3: 4, "a": "b"}) == {1: 2, 3: 4, "a": "b"}


def test21_numpy_overloads():
    try:
        import numpy as np
    except ImportError:
        pytest.skip("numpy is missing")

    assert t.test_05(np.int32(0)) == 1
    assert t.test_05(np.float64(0.1)) == 2
    assert t.test_05(np.float64(0.0)) == 2
    assert t.test_11_sl(np.int32(5)) == 5
    assert t.test_11_ul(np.int32(5)) == 5
    assert t.test_11_sll(np.int32(5)) == 5
    assert t.test_11_ull(np.int32(5)) == 5


def test22_string_return():
    assert t.test_12("hello") == "hello"
    assert t.test_13() == "test"
    assert t.test_14("abc") == "abc"


def test23_byte_return():
    assert t.test_15(b"abc") == "abc"
    assert t.test_15_d(b"abc\x00def\x00ghi") == b"abc\x00def\x00ghi"
    assert t.test_16("hello") == b"hello"
    assert t.test_17(b"four") == 4
    assert t.test_17(b"\x00\x00\x00\x00") == 4
    assert t.test_18("hello world", 5) == b"hello"


@pytest.mark.skipif(sys.version_info < (3, 9), reason="requires python3.9 or higher")
def test24_pydoc():
    import pydoc

    assert "test_05(arg: int, /)" in pydoc.render_doc(t)


def test25_int():
    assert t.test_19(5) == 128
    assert t.test_20("5") == 128
    assert t.test_21(5) == 5
    assert t.test_19.__doc__ == "test_19(arg: int, /) -> object"


def test26_capsule():
    p = t.test_22()
    assert "capsule" in str(p) and "nb_handle" in str(p)
    assert t.test_24(p) == 1
    p = t.test_23()
    assert p is None
    assert t.test_24(p) == 0


def test27_slice():
    s = slice(1, 10, 2)
    assert t.test_25(s) is s
    assert t.test_25.__doc__ == "test_25(arg: slice, /) -> slice"
    assert t.test_26() == slice(4)
    assert t.test_27() == slice(1, 10)
    assert t.test_28() == slice(5, -5, -2)


def test28_ellipsis():
    assert t.test_29(...) is ...
    assert (
        t.test_29.__doc__ == "test_29(arg: types.EllipsisType, /) -> types.EllipsisType"
    )


def test29_traceback():
    result = t.test_30(fail_fn)
    print(result)
    regexp = r'Traceback \(most recent call last\):\n.*\n  File "[^"]*", line [0-9]*, in fail_fn\n.*RuntimeError: Foo'
    matches = re.findall(regexp, result, re.MULTILINE | re.DOTALL)
    assert len(matches) == 1


def test30_noexcept():
    assert t.test_31(123) == 123
    assert t.test_32(123) == 123


@pytest.mark.parametrize(
    "func_name",
    [
        "identity_i8",
        "identity_u8",
        "identity_i16",
        "identity_u16",
        "identity_i32",
        "identity_u32",
        "identity_i64",
        "identity_u64",
    ],
)
def test31_range(func_name):
    func = getattr(t, func_name)

    values = [
        0,
        -1,
        1,
        2**7,
        2**7 - 1,
        2**8,
        2**8 - 1,
        2**15,
        2**15 - 1,
        2**16,
        2**16 - 1,
        2**29,
        2**29 - 1,
        2**30,
        2**30 - 1,
        2**31,
        2**31 - 1,
        2**32,
        2**32 - 1,
        2**63,
        2**63 - 1,
        2**64,
        2**64 - 1,
        2**127,
        2**127 - 1,
        2**128,
        2**128 - 1,
    ]
    values += [-value for value in values]
    suffix = func.__name__[9:]

    if suffix[0] == "u":
        range_min = 0
        range_max = 2 ** int(suffix[1:]) - 1
    else:
        range_min = -(2 ** (int(suffix[1:]) - 1))
        range_max = -range_min - 1

    for value in values:
        if value < range_min or value > range_max:
            with pytest.raises(TypeError):
                value_out = func(value)
        else:
            value_out = func(value)
            assert value_out == value


def test33_method_on_non_nanobind_class():
    class AClass:
        def __init__(self):
            self.x = 42

    AClass.simple_method = t.test_33
    AClass.complex_method = t.test_34
    a = AClass()
    assert a.simple_method(7) == 49
    assert a.complex_method(y=2) == 84


def test34_module_docstring():
    assert t.__doc__ == "function testcase"


def test35_return_capture():
    x = t.test_35()
    assert x() == "Test Foo"


def test36_test_char():
    assert t.test_cast_char("c") == "c"
    with pytest.raises(TypeError):
        assert t.test_cast_char("abc")
    with pytest.raises(RuntimeError):
        assert t.test_cast_char(123)


def test37_test_str():
    assert t.test_cast_str("c") == "c"
    assert t.test_cast_str("abc") == "abc"
    with pytest.raises(RuntimeError):
        assert t.test_cast_str(123)


def test38_set():
    x = t.test_set()
    assert isinstance(x, set)
    assert len(x) == 2
    assert 123 in x and "123" in x
    assert t.test_set_contains(x, 123)
    assert t.test_set_contains(x, "123")
    assert not t.test_set_contains(x, "1234")
    assert not t.test_set_contains(x, 1234)


def test39_del():
    l = [0, 1, 2, 3, 4]
    t.test_del_list(l)
    assert l == [0, 1, 3, 4]

    l = {"a": 0, "b": 1}
    t.test_del_dict(l)
    assert l == {"b": 1}

    with pytest.raises(IndexError):
        t.test_del_list([])

    with pytest.raises(KeyError):
        t.test_del_dict({})


def test40_nb_signature():
    assert t.test_01.__nb_signature__ == ((r"def test_01() -> None", None, None),)
    assert t.test_02.__nb_signature__ == (
        (r"def test_02(up: int = \0, down: int = \1) -> int", None, (8, 1)),
    )
    assert t.test_05.__nb_signature__ == (
        (r"def test_05(arg: int, /) -> int", "doc_1", None),
        (r"def test_05(arg: float, /) -> int", "doc_2", None),
    )
    if sys.version_info >= (3, 9):
        assert t.test_07.__nb_signature__ == (
            (
                r"def test_07(arg0: int, arg1: int, /, *args, **kwargs) -> tuple[int, int]",
                None,
                None,
            ),
            (
                r"def test_07(a: int, b: int, *myargs, **mykwargs) -> tuple[int, int]",
                None,
                None,
            ),
        )


def test41_kw_only():
    # (i, j, *args, z)
    assert t.test_args_kwonly(2, 2.5, z=22) == (2, 2.5, (), 22)
    assert t.test_args_kwonly(2, 2.5, "a", "b", z=22) == (2, 2.5, ("a", "b"), 22)
    assert t.test_args_kwonly(z=22, i=4, j=16) == (4, 16.0, (), 22)
    assert (
        t.test_args_kwonly.__doc__
        == "test_args_kwonly(i: int, j: float, *args, z: int) -> tuple"
    )
    with pytest.raises(TypeError):
        t.test_args_kwonly(2, 2.5, 22)  # missing z= keyword

    # (i, j, *args, z, **kwargs)
    assert t.test_args_kwonly_kwargs(i=1, k=4, j=10, z=-1, y=9) == (
        1, 10, (), -1, {"k": 4, "y": 9}
    )
    assert t.test_args_kwonly_kwargs(1, 2, 3, 4, z=11, y=12) == (
        1, 2, (3, 4), 11, {"y": 12}
    )
    with pytest.raises(TypeError):
        t.test_args_kwonly_kwargs(1, 2, 3, 4, 5)
    assert (
        t.test_args_kwonly_kwargs.__doc__
        == "test_args_kwonly_kwargs(i: int, j: float, *args, z: int, **kwargs) -> tuple"
    )

    # (i, *, j, **kwargs)
    assert t.test_kwonly_kwargs(j=2, i=1) == (1, 2, {})
    assert t.test_kwonly_kwargs(j=2, i=1, z=10) == (1, 2, {"z": 10})
    assert t.test_kwonly_kwargs(1, j=2) == (1, 2, {})
    assert t.test_kwonly_kwargs(1, j=2, z=10) == (1, 2, {"z": 10})
    with pytest.raises(TypeError):
        t.test_kwonly_kwargs(1, 2)
    with pytest.raises(TypeError):
        t.test_kwonly_kwargs(1, 2, j=3)
    with pytest.raises(TypeError):
        t.test_kwonly_kwargs(1, 2, z=10)
    assert (
        t.test_kwonly_kwargs.__doc__
        == "test_kwonly_kwargs(i: int, *, j: float, **kwargs) -> tuple"
    )

    # (*, i, j)
    assert t.test_kw_only_all(i=1, j=2) == (1, 2)
    assert t.test_kw_only_all(j=1, i=2) == (2, 1)
    with pytest.raises(TypeError):
        t.test_kw_only_all(i=1)
    with pytest.raises(TypeError):
        t.test_kw_only_all(1, 2)
    assert (
        t.test_kw_only_all.__doc__
        == "test_kw_only_all(*, i: int, j: int) -> tuple"
    )

    # (__arg0, *, j, k)
    assert t.test_kw_only_some(1, k=3, j=2) == (1, 2, 3)
    assert (
        t.test_kw_only_some.__doc__
        == "test_kw_only_some(arg0: int, *, j: int, k: int) -> tuple"
    )

    # (__arg0=3, j=4, *, k=5, z)
    assert t.test_kw_only_with_defaults(z=8) == (3, 4, 5, 8)
    assert t.test_kw_only_with_defaults(2, z=8) == (2, 4, 5, 8)
    assert t.test_kw_only_with_defaults(2, j=7, k=8, z=9) == (2, 7, 8, 9)
    assert t.test_kw_only_with_defaults(2, 7, z=9, k=8) == (2, 7, 8, 9)
    with pytest.raises(TypeError):
        t.test_kw_only_with_defaults(2, 7, 8, z=9)
    assert (
        t.test_kw_only_with_defaults.__doc__
        == "test_kw_only_with_defaults(arg0: int = 3, j: int = 4, *, k: int = 5, z: int) -> tuple"
    )

    # (i, *, j)
    assert t.test_kw_only_mixed(1, j=2) == (1, 2)
    assert t.test_kw_only_mixed(j=2, i=3) == (3, 2)
    assert t.test_kw_only_mixed(i=2, j=3) == (2, 3)
    with pytest.raises(TypeError):
        t.test_kw_only_mixed(i=1)
    with pytest.raises(TypeError):
        t.test_kw_only_mixed(1, i=2)
    assert (
        t.test_kw_only_mixed.__doc__
        == "test_kw_only_mixed(i: int, *, j: int) -> tuple"
    )

    with pytest.raises(TypeError):
        t.kw_only_methods(42)

    val = t.kw_only_methods(v=42)
    assert val.v == 42

    # (self, *, i, j)
    assert val.method_2k() == (1, 2)
    assert val.method_2k(i=3) == (3, 2)
    assert val.method_2k(j=4) == (1, 4)
    assert val.method_2k(i=3, j=4) == (3, 4)
    assert val.method_2k(j=3, i=4) == (4, 3)
    with pytest.raises(TypeError):
        val.method_2k(1)
    with pytest.raises(TypeError):
        val.method_2k(1, j=2)
    assert (
        t.kw_only_methods.method_2k.__doc__
        == "method_2k(self, *, i: int = 1, j: int = 2) -> tuple"
    )

    # (self, i, *, j)
    assert val.method_1p1k() == (1, 2)
    assert val.method_1p1k(i=3) == (3, 2)
    assert val.method_1p1k(j=4) == (1, 4)
    assert val.method_1p1k(i=3, j=4) == (3, 4)
    assert val.method_1p1k(j=3, i=4) == (4, 3)
    assert val.method_1p1k(3) == (3, 2)
    assert val.method_1p1k(3, j=4) == (3, 4)
    with pytest.raises(TypeError):
        val.method_2k(1, 2)
    assert (
        t.kw_only_methods.method_1p1k.__doc__
        == "method_1p1k(self, i: int = 1, *, j: int = 2) -> tuple"
    )


def test42_ptr_return():
    assert t.test_ptr_return() == (10, 100)

def test41_any():
    s = "hello"
    assert t.test_any(s) is s
    assert t.test_any.__doc__ == "test_any(arg: typing.Any, /) -> typing.Any"

def test42_wrappers_list():
    assert t.test_wrappers_list()

def test43_wrappers_dict():
    assert t.test_wrappers_dict()

def test43_wrappers_set():
    assert t.test_wrappers_set()

def test44_hash():
    value = (1, 2, 3)
    assert t.hash_it(value) == hash(value);
