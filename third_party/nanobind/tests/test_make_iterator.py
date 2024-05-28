import test_make_iterator_ext as t
import pytest

data = [
    {},
    { 'a' : 'b' },
    { str(i) : chr(i) for i in range(1000) }
]


def test01_key_iterator():
    for d in data:
        m = t.StringMap(d)
        assert sorted(list(m)) == sorted(list(d))


def test02_value_iterator():
    types = []
    for d in data:
        m = t.StringMap(d)
        types.append(type(m.values()))
        assert sorted(list(m.values())) == sorted(list(d.values()))
    assert types[0] is types[1] and types[1] is types[2]


def test03_items_iterator():
    for d in data:
        m = t.StringMap(d)
        assert sorted(list(m.items())) == sorted(list(d.items()))


def test04_passthrough_iterator():
    for d in data:
        m = t.StringMap(d)
        assert list(t.iterator_passthrough(m.values())) == list(m.values())


def test05_iterator_returning_temporary():
    im = t.IdentityMap()
    assert list(im) == list(range(10))
    assert list(im.values()) == list(range(10))
    assert list(im.items()) == list(zip(range(10), range(10)))
