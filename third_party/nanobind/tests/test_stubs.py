import os
import pathlib
import difflib
import sys
import platform
import pytest

is_unsupported = platform.python_implementation() == 'PyPy' or sys.version_info < (3, 10)
skip_on_unsupported = pytest.mark.skipif(
    is_unsupported, reason="Stub generation is only tested on CPython >= 3.10.0")

def remove_platform_dependent(s):
    '''Remove platform-dependent functions from the stubs'''
    s2 = []
    i = 0
    while i < len(s):
        v = s[i]
        if v.startswith('def ret_numpy_half()') or \
           v.startswith('def test_slots()') or \
           v.startswith('TypeAlias'):
            i += 2
        else:
            s2.append(v)
            i += 1
    return s2

ref_paths = list(pathlib.Path(__file__).parent.glob('*.pyi.ref'))
assert len(ref_paths) > 0, "Stub reference files not found!"

@skip_on_unsupported
@pytest.mark.parametrize('p_ref', ref_paths)
def test01_check_stub_refs(p_ref):
    """
    Check that generated stub files match reference input
    """
    p_in = p_ref.with_suffix('')
    with open(p_ref, 'r') as f:
        s_ref = f.read().split('\n')
    with open(p_in, 'r') as f:
        s_in = f.read().split('\n')
    s_in = remove_platform_dependent(s_in)
    s_ref = remove_platform_dependent(s_ref)

    diff = list(difflib.unified_diff(
        s_ref,
        s_in,
        fromfile=str(p_ref),
        tofile=str(p_in)
    ))
    if len(diff):
        for p in diff:
            print(p.rstrip(), file=sys.stderr)
        print(
            '\nWarning: generated stubs do not match their references. If you\n'
            'intentionally changed a test suite extension, it may be necessary\n'
            'to replace the .pyi.ref file with the generated .pyi file. But\n'
            'please double-check that the change makes sense.',
            file=sys.stderr
        )
        assert False
