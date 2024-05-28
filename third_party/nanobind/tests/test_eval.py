import os

import pytest

import test_eval_ext as m


def test_evals(capsys):
    assert m.test_eval_statements()
    captured = capsys.readouterr()
    assert captured.out == "Hello World!\n"

    assert m.test_eval()
    assert m.test_eval_single_statement()

    assert m.test_eval_failure()


def test_eval_closure():
    global_, local = m.test_eval_closure()

    assert global_["closure_value"] == 42
    assert local["closure_value"] == 0

    assert "local_value" not in global_
    assert local["local_value"] == 0

    assert "func_global" not in global_
    assert local["func_global"]() == 42

    assert "func_local" not in global_
    with pytest.raises(NameError):
        local["func_local"]()

a = 1

def test_read_globals():
    assert m.globals_contains_a()


def test_write_globals():
    assert "b" not in globals()
    m.globals_add_b()
    assert globals()["b"] == 123
