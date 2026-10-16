# Gate 执行脚本（RUST_REWRITE_PLAN.md §6）

一键运行各验收关卡；每个脚本都会打印**可审计的**通过/失败结论。

| 脚本 | 关卡 | 内容 | 当前状态（2026-10-16） |
|---|---|---|---|
| `run_gate_a.sh` | Gate A — SQL 功能 | 现有 173 个 `.slt` 全量比对 | **未达标**：`statement ok` 1745/1746、`query` 1300/1532、`statement error` 45/129（要求 100%） |
| `run_gate_b.sh` | Gate B — SDK/协议 | `python/test_pysdk`、`go/tests`、HTTP（`tools/run_http_api.py`），**不改 SDK** | **未运行**：需先补齐 Thrift 全量 RPC；脚本会在缺依赖时明确报 `SKIP/MISSING` |
| `run_gate_c.sh` | Gate C — 持久化/恢复 | workspace 测试、`test/sql/snapshot`、`python/restart_test`、MinIO | **部分**：快照 `.slt` 与重启测试已在 Rust 引擎上通过；MinIO 需 CI 服务 |
| `run_gate_d.sh` | Gate D — 并发/稳定性 | loom 模型、miri（纯 Rust 核心）、`python/parallel_test` | **部分**：loom `commit_ts` 唯一性已通过；miri 需 nightly 组件 |
| `run_gate_e.sh` | Gate E — 性能 | `infinity bench` + `tools/ci_tools/check_benchmark_result.py`（<7% 门限） | **未达标**：尚无与 golden 对齐的 Rust 基准驱动 |
| `deny.toml` + CI | Gate F — 工程质量 | fmt / clippy `-D warnings` / test / cargo-deny；工具链钉 `1.94.1` | **达标**：CI 六个作业全绿；9/9 核心 crate `#![forbid(unsafe_code)]` |
| `docs/INVARIANT_MATRIX.md` | Gate G — 不变量 | C++ 不变量 → Rust 测试对照 + 显式登记缺口 | **清单已产出**：5 项缺口（G1–G5）已登记，尚未「纳入门槛」 |

## 用法

**前置**：部分 `.slt` 依赖 `tools/generate_*.py` 生成的夹具（`test/data/csv/**/*big*.csv` 等被
`.gitignore` 排除）。运行前先生成，否则相关记录会因夹具缺失而空结果：

```bash
python3 tools/generate_index_scan.py        # test/data/csv/test_big_index_scan.csv
```

```bash
cd rust_engine
./scripts/run_gate_a.sh                    # .slt 一致性基线
./scripts/run_gate_b.sh                    # SDK/协议（需 python/go 环境）
./scripts/run_gate_c.sh
./scripts/run_gate_d.sh
./scripts/run_gate_e.sh [golden_dir]
```

> 结论口径：Gate A 要求 **全部 `.slt` 通过**，当前未达标；Gate B–E 的执行脚本已就绪，
> 但**尚无通过证据**——它们必须在对应子系统（Thrift 全量 RPC、对象存储、基准驱动）完成后才能产出。
