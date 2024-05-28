import pytest
import sys
import platform

import test_bind_map_ext as t


def test_map_string_double(capfd):
    mm = t.MapStringDouble()
    mm["a"] = 1
    mm["b"] = 2.5

    assert list(mm) == ["a", "b"]
    assert "b" in mm
    assert 123 not in mm
    assert mm["b"] == 2.5

    assert "c" not in mm
    with pytest.raises(KeyError):
        mm["c"]
    assert "c" not in mm

    # Copy constructor
    mm2 = t.MapStringDouble(mm)
    assert list(mm2) == ["a", "b"]
    assert "b" in mm2
    assert "c" not in mm2
    assert 123 not in mm2
    assert mm2["b"] == 2.5
    assert mm == mm2
    assert not (mm != mm2)
    del mm2["b"]
    assert not (mm == mm2)
    assert mm != mm2
    assert len(mm2) == 1
    mm2.clear()
    assert len(mm2) == 0
    assert repr(mm) == "test_bind_map_ext.MapStringDouble({'a': 1.0, 'b': 2.5})"

    with pytest.raises(TypeError):
        mm2.update({"a" : "b"})
    captured = capfd.readouterr().err.strip()
    ref = "nanobind: implicit conversion from type 'dict' to type 'test_bind_map_ext.MapStringDouble' failed!"

    # Work around Pytest-related flakiness (https://github.com/pytest-dev/pytest/issues/10843)
    if platform.system() == 'Windows':
        assert captured == ref or captured == ''
    else:
        assert captured == ref

    mm2.update({"a" : 2.5})
    assert len(mm2) == 1

    # Construction from an iterable
    mm3 = t.MapStringDouble(
        { "a" : 1, "b" : 2.5 })
    assert list(mm3) == ["a", "b"]
    assert mm3["b"] == 2.5

    # Check that keys, values, items are views, not merely iterable
    keys = mm.keys()
    values = mm.values()
    items = mm.items()
    assert list(keys) == ["a", "b"]
    assert len(keys) == 2
    assert "a" in keys
    assert "c" not in keys
    assert 123 not in keys
    assert list(items) == [("a", 1), ("b", 2.5)]
    assert len(items) == 2
    assert ("b", 2.5) in items
    assert "hello" not in items
    assert ("b", 2.5, None) not in items
    assert list(values) == [1, 2.5]
    assert len(values) == 2
    assert 1 in values
    assert 2 not in values

    # Check that views update when the map is updated
    mm["c"] = -1
    assert list(keys) == ["a", "b", "c"]
    assert list(values) == [1, 2.5, -1]
    assert list(items) == [("a", 1), ("b", 2.5), ("c", -1)]

    um = t.UnorderedMapStringDouble()
    um["ua"] = 1.1
    um["ub"] = 2.6

    assert sorted(list(um)) == ["ua", "ub"]
    assert list(um.keys()) == list(um)
    assert sorted(list(um.items())) == [("ua", 1.1), ("ub", 2.6)]
    assert list(zip(um.keys(), um.values())) == list(um.items())

    assert type(keys).__qualname__ == 'MapStringDouble.KeyView'
    assert type(values).__qualname__ == 'MapStringDouble.ValueView'
    assert type(items).__qualname__ == 'MapStringDouble.ItemView'

    if sys.version_info < (3, 9):
        d = "typing.Dict"
    else:
        d = "dict"

    assert t.MapStringDouble.__init__.__doc__ == \
"""__init__(self) -> None
__init__(self, arg: test_bind_map_ext.MapStringDouble) -> None
__init__(self, arg: %s[str, float], /) -> None

Overloaded function.

1. ``__init__(self) -> None``

Default constructor

2. ``__init__(self, arg: test_bind_map_ext.MapStringDouble) -> None``

Copy constructor

3. ``__init__(self, arg: %s[str, float], /) -> None``

Construct from a dictionary""" % (d, d)


def test_map_string_double_const():
    mc = t.MapStringDoubleConst()
    mc["a"] = 10
    mc["b"] = 20.5

    umc = t.UnorderedMapStringDoubleConst()
    umc["a"] = 11
    umc["b"] = 21.5

    str(umc)


def test_maps_with_noncopyable_values():
    # std::map
    mnc = t.get_mnc(5)
    for i in range(1, 6):
        assert mnc[i].value == 10 * i

    vsum = 0
    for k, v in mnc.items():
        assert v.value == 10 * k
        vsum += v.value

    assert vsum == 150

    # std::unordered_map
    mnc = t.get_umnc(5)
    for i in range(1, 6):
        assert mnc[i].value == 10 * i

    vsum = 0
    for k, v in mnc.items():
        assert v.value == 10 * k
        vsum += v.value

    assert vsum == 150

    # nested std::map<std::vector>
    nvnc = t.get_nvnc(5)
    for i in range(1, 6):
        for j in range(0, 5):
            assert nvnc[i][j].value == j + 1

    # Note: maps do not have .values()
    for _, v in nvnc.items():
        for i, j in enumerate(v, start=1):
            assert j.value == i

    # nested std::map<std::map>
    nmnc = t.get_nmnc(5)
    for i in range(1, 6):
        for j in range(10, 60, 10):
            assert nmnc[i][j].value == 10 * j

    vsum = 0
    for _, v_o in nmnc.items():
        for k_i, v_i in v_o.items():
            assert v_i.value == 10 * k_i
            vsum += v_i.value

    assert vsum == 7500

    # nested std::unordered_map<std::unordered_map>
    numnc = t.get_numnc(5)
    for i in range(1, 6):
        for j in range(10, 60, 10):
            assert numnc[i][j].value == 10 * j

    vsum = 0
    for _, v_o in numnc.items():
        for k_i, v_i in v_o.items():
            assert v_i.value == 10 * k_i
            vsum += v_i.value

    assert vsum == 7500


def test_map_delitem():
    mm = t.MapStringDouble()
    mm["a"] = 1
    mm["b"] = 2.5

    assert list(mm) == ["a", "b"]
    assert list(mm.items()) == [("a", 1), ("b", 2.5)]
    del mm["a"]
    assert list(mm) == ["b"]
    assert list(mm.items()) == [("b", 2.5)]

    um = t.UnorderedMapStringDouble()
    um["ua"] = 1.1
    um["ub"] = 2.6

    assert sorted(list(um)) == ["ua", "ub"]
    assert sorted(list(um.items())) == [("ua", 1.1), ("ub", 2.6)]
    del um["ua"]
    assert sorted(list(um)) == ["ub"]
    assert sorted(list(um.items())) == [("ub", 2.6)]
