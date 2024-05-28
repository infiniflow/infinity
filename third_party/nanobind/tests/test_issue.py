import test_issue_ext as m
import pytest

# Issue #279: dynamic_attr broken
@pytest.mark.parametrize("variant", [1, 2])
def test01_issue_279(variant):
    def _get_parameter(self: m.Model, key: str):
        p = self._get_param(key)
        if p is not None:  # cache it for fast access later
            setattr(self, key, p)
            return p
        raise AttributeError(f"'key' not found in {self}")

    m.Model.__getattr__ = _get_parameter

    if variant == 2:
        def _print_model(self):
            return f"{self.__class__.__qualname__}()"

        m.Model.__str__ = _print_model

    class Top(m.Model):
        def __init__(self):
            super().__init__()
            self.model_a = m.ModelA()

    top = Top()
    str(top.model_a)
    str(top.model_a.a)


# Issue #307: move constructor unexpectedly called
def test02_issue_307():
    l = [m.Example("A"), m.Example("B")]
    assert str(l) == '[Example("A"), Example("B")]'
    m.process(l)
    assert str(l) == '[Example("A"), Example("B")]'
