# 重写版文档索引

`rust_engine` 是 C++ 参考引擎（`src/`）的 Rust 重写版。文档只描述**现状**：参考怎么工作、重写版怎么对应、哪些地方不同、怎么验证。移植过程、评审轮次、修复顺序都不在文档里（需要时看 `git log`）。

## 现在是什么状态

| 维度 | 状态 | 验证 |
|---|---|---|
| 磁盘格式（`data/`、`catalog/`、`wal/`） | 块列、`version`、索引 chunk、catalog 键值、WAL 帧（**含 `txn_id` 与 CRC**）与参考逐字节相同 | `python/restart_test/test_cross_engine_bytes.py` |
| WAL | `wal.log` 的文件名、checkpoint 换文件、帧头、帧体全部一致；两引擎可互相 replay | 同上 + `test_cross_engine_compat.py` |
| SQL 行为 | `.slt` 全量一致（Gate A）；SDK/协议（Gate B）、重启/恢复（Gate C）由对应脚本驱动 | `rust_engine/scripts/run_gate_{a,b,c}.sh` |
| 已知缺口 | 见 [`GAPS.md`](GAPS.md) | — |

## 文档

| 文档 | 内容 | 什么时候读 |
|---|---|---|
| [`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md) | 参考的磁盘/序列化格式规格：数据文件、`version`、列体、outline、全文 `.dic/.pos/.len`、次级/稀疏索引、WAL、catalog 键 | 要动格式、或想知道某段字节为什么长这样 |
| [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) | 逐字节门禁：覆盖矩阵、每个用例断言什么、§5 列出格式层面的例外与原因、§6 差异与一致清单、§7 复现步骤 | 跑/改/新增格式门禁 |
| [`GAPS.md`](GAPS.md) | 缺口总表：有意不移植、未实现/未接线、不可逐字节比对、覆盖不到的场景 | 想知道"还差什么" |
| [`CLASS_MAP.md`](CLASS_MAP.md) | C++ 类/方法 → Rust 的映射，以及登记过的设计变更与已知限制 | 想知道某个 C++ 类型在 Rust 里是谁、或某个差异是不是有意的 |
| [`CLASS_AUDIT.md`](CLASS_AUDIT.md) | 参考 `src/**/*.cppm` 每个 `export class/struct` 一行的台账（`mapped`/`partial`/`test`/`exempt`/`excluded`） | 查某个类有没有对应、是不是 `partial` |
| [`TEST_AUDIT.md`](TEST_AUDIT.md) | 参考 `src/unit_test/**` 每个 `TEST/TEST_F/TEST_P` 一行的台账（`ported`/`excluded`/`exempt`） | 查某个 C++ 单测在 Rust 里对应哪条 |
| [`INVARIANT_MATRIX.md`](INVARIANT_MATRIX.md) | Gate G：C++ 内部不变量 → Rust 回归测试的对照 | 想知道某条不变量由谁兜住 |
| [`CALL_PROBE_AUDIT.md`](CALL_PROBE_AUDIT.md) | 用 uprobe 探针回答"某个函数在参考里到底被调用没有"的方法、已测事实与未覆盖项 | 怀疑某个实现是死代码时 |
| [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) | catalog 底座（RocksDB）与跨引擎互操作的常见问题 | 参考打不开目录、或想确认 fsync 点时 |

## 怎么验证

格式门禁（需要参考二进制 `cmake-build-reldeb/src/infinity`）：

```bash
cd rust_engine && cargo build --release -p infinity-cli
cd ..
.venv/bin/python -m pytest python/restart_test/test_cross_engine_bytes.py \
                              python/restart_test/test_cross_engine_compat.py -q
```

单元测试：

```bash
cd rust_engine && cargo test --release --workspace --lib
```

`scripts/run_gate_{a,b,c,d,e}.sh` 分别对应 SQL、SDK/协议、持久化、并发、性能关卡。
