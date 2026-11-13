"""L0 前提：门禁自己依赖的确定性。

两个引擎各自的输入->字节必须是确定的，否则任何“两边相同”的结论都站不住：
`test_the_reference_is_deterministic_where_the_test_assumes_it`（参考对同一输入
多跑几次，只有被声明为确定的文件类必须一致）与 `test_the_port_is_deterministic_too`
（端口跑三次，`data/` 下每个文件都必须相同）。

这一层不比较两个引擎，而是给下面每一层的比较**发许可证**。
"""

import os

import pytest
from bytes_common import (
    DET_ROWS,
    ROOT,
    RUST_BIN,
    _files,
    _run_engine,
)


@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_port_is_deterministic_too():
    """The port's half of the same classification, measured on its own.

    The byte gate compares two engines, so it can only catch a drift the two
    do *differently*: a value that both produce from a clock, a hash map's
    iteration order or a thread schedule would agree today and disagree
    tomorrow. This runs the port three times on the same input and requires
    every file under `data/` to come out identical - including the HNSW
    chunk, whose graph the reference itself cannot reproduce (the port's
    build is a plain insertion in row order, with no randomness to schedule).
    """
    roots = []
    for i in range(3):
        root = os.path.join(ROOT, f"port_determinism{i}")
        _run_engine("rust", root, 5432 + i, row_count=DET_ROWS)
        roots.append(root)

    digests = [_files(root) for root in roots]
    names = sorted(set().union(*[set(d) for d in digests]))
    varying = [
        name for name in names if len({digest.get(name) for digest in digests}) > 1
    ]
    assert not varying, f"the port is not deterministic here: {varying}"
