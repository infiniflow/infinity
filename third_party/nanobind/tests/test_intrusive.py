import test_intrusive_ext as t
import pytest
from common import collect

@pytest.fixture
def clean():
    collect()
    t.reset()

def test01_construct(clean):
    o = t.Test()
    assert o.value(0) == 123
    assert t.get_value_1(o) == 124
    assert t.get_value_2(o) == 125
    assert t.get_value_3(o) == 126
    del o
    collect()
    assert t.stats() == (1, 1)


def test02_factory(clean):
    o = t.Test.create_raw()
    assert o.value(0) == 123
    assert t.get_value_1(o) == 124
    assert t.get_value_2(o) == 125
    assert t.get_value_3(o) == 126
    del o
    collect()
    assert t.stats() == (1, 1)


def test03_factory_ref(clean):
    o = t.Test.create_ref()
    assert o.value(0) == 123
    assert t.get_value_1(o) == 124
    assert t.get_value_2(o) == 125
    assert t.get_value_3(o) == 126
    del o
    collect()
    assert t.stats() == (1, 1)

def test04_subclass(clean):
    class MyTest(t.Test):
        def __init__(self, x):
            super().__init__()
            self.x = x

        def value(self, value):
            return self.x * value

    o = MyTest(456)
    assert o.value(0) == 0
    assert t.get_value_1(o) == 456
    assert t.get_value_2(o) == 456*2
    assert t.get_value_3(o) == 456*3
    del o
    collect()
    assert t.stats() == (1, 1)
