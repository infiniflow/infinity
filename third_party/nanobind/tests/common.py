import platform
import gc
import pytest

is_pypy = platform.python_implementation() == 'PyPy'
is_darwin = platform.system() == 'Darwin'

def collect() -> None:
    if is_pypy:
        for _ in range(3):
            gc.collect()
    else:
        gc.collect()

skip_on_pypy = pytest.mark.skipif(
    is_pypy, reason="This test currently fails/crashes PyPy")

xfail_on_pypy_darwin = pytest.mark.xfail(
    is_pypy and is_darwin, reason="This test for some reason fails on PyPy/Darwin")
