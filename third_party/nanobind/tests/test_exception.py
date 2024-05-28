import test_exception_ext as t
import pytest

def test01_base():
    with pytest.raises(RuntimeError):
        assert t.raise_generic()

def test02_bad_alloc():
    with pytest.raises(MemoryError):
        assert t.raise_bad_alloc()

def test03_runtime_error():
    with pytest.raises(RuntimeError) as excinfo:
        assert t.raise_runtime_error()
    assert str(excinfo.value) == 'a runtime error'

def test04_domain_error():
    with pytest.raises(ValueError) as excinfo:
        assert t.raise_domain_error()
    assert str(excinfo.value) == 'a domain error'

def test05_invalid_argument():
    with pytest.raises(ValueError) as excinfo:
        assert t.raise_invalid_argument()
    assert str(excinfo.value) == 'an invalid argument error'

def test06_length():
    with pytest.raises(ValueError) as excinfo:
        assert t.raise_length_error()
    assert str(excinfo.value) == 'a length error'

def test07_out_of_range():
    with pytest.raises(IndexError) as excinfo:
        assert t.raise_out_of_range()
    assert str(excinfo.value) == 'an out of range error'

def test08_range_error():
    with pytest.raises(ValueError) as excinfo:
        assert t.raise_range_error()
    assert str(excinfo.value) == 'a range error'

def test09_overflow_error():
    with pytest.raises(OverflowError) as excinfo:
        assert t.raise_overflow_error()
    assert str(excinfo.value) == 'an overflow error'

def test10_index_error():
    with pytest.raises(IndexError) as excinfo:
        assert t.raise_index_error()
    assert str(excinfo.value) == 'an index error'

def test11_key_error():
    with pytest.raises(KeyError) as excinfo:
        assert t.raise_key_error()
    assert str(excinfo.value) == "'a key error'"

def test12_value_error():
    with pytest.raises(ValueError) as excinfo:
        assert t.raise_value_error()
    assert str(excinfo.value) == 'a value error'

def test13_type_error():
    with pytest.raises(TypeError) as excinfo:
        assert t.raise_type_error()
    assert str(excinfo.value) == 'a type error'

def test14_import_error():
    with pytest.raises(ImportError) as excinfo:
        assert t.raise_import_error()
    assert str(excinfo.value) == 'an import error'

def test15_attribute_error():
    with pytest.raises(AttributeError) as excinfo:
        assert t.raise_attribute_error()
    assert str(excinfo.value) == 'an attribute error'

def test16_stop_iteration():
    with pytest.raises(StopIteration) as excinfo:
        assert t.raise_stop_iteration()
    assert str(excinfo.value) == 'a stop iteration error'

def test17_raise_my_error_1():
    with pytest.raises(RuntimeError) as excinfo:
        assert t.raise_my_error_1()
    assert str(excinfo.value) == 'MyError1'

def test18_raise_my_error_2():
    with pytest.raises(IndexError) as excinfo:
        assert t.raise_my_error_2()
    assert str(excinfo.value) == 'MyError2'

def test19_raise_my_error_3():
    with pytest.raises(t.MyError3) as excinfo:
        assert t.raise_my_error_3()
    assert str(excinfo.value) == 'MyError3'

def test20_nested():
    def foo(arg):
        return arg / 0
    with pytest.raises(RuntimeError) as excinfo:
        t.raise_nested(foo)
    assert str(excinfo.value) == 'Call with value 123 failed'
    assert str(excinfo.value.__cause__) == 'division by zero'

