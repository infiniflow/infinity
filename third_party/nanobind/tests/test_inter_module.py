import test_inter_module_1_ext as t1
import test_inter_module_2_ext as t2
import test_classes_ext as t3
import pytest
from common import xfail_on_pypy_darwin

@xfail_on_pypy_darwin
def test01_inter_module():
    s = t1.create_shared()
    assert t2.check_shared(s)
    with pytest.raises(TypeError) as excinfo:
        assert t3.check_shared(s)
    assert 'incompatible function arguments' in str(excinfo.value)
