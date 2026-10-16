# Gate 执行脚本（RUST_REWRITE_PLAN.md §6）

一键运行各验收关卡；每个脚本都会打印**可审计的**通过/失败结论。

| 脚本 | 关卡 | 内容 | 当前状态（2026-10-17 复验） |
|---|---|---|---|
| `run_gate_a.sh` | Gate A — SQL 功能 | 现有 174 个 `.slt` 全量比对 | **未达标**：`statement ok` 1752/1752、`statement error` 129/129、`query` **1474/1536**（要求 100%；口径对齐 `sqllogictest-rs v0.28.4`：折叠空白 + 默认按行序）。剩余为全文/FUSION 打分 |
| `run_gate_b.sh` | Gate B — SDK/协议 | `python/test_pysdk`、`go/tests`、HTTP（`tools/run_http_api.py`），**不改 SDK** | **部分**：`infinity-server` 二进制已就位（`cargo build -p infinity-server`），未改动的 Python SDK 可连上 Rust 服务，`test_pysdk`（全量未改动 SDK，2259 例）**1448 passed / 212 failed / 598 errors / 7 skipped**（598 error 多为 `/var/infinity` 夹具权限） |
| `run_gate_c.sh` | Gate C — 持久化/恢复 | workspace 测试、`test/sql/snapshot`、`python/restart_test`、MinIO | **部分**：`test/sql/snapshot` **171/171**、`8/8` error，workspace 测试全绿；MinIO 需 CI 服务；`python/restart_test` 的 runner 以 `<binary> --config=…` 启动并写 `/var/infinity`，需适配 Rust 服务 CLI，且当前环境不可写 |
| `run_gate_d.sh` | Gate D — 并发/稳定性 | loom 模型、miri（纯 Rust 核心）、`python/parallel_test` | **部分**：loom `commit_ts` 唯一性已通过；miri 已在 `nightly-2025-04-04` 上通过（parser/expression/planner 共 38 个用例）；`python/parallel_test`（对 Rust 服务）**18/18**（含并发 INSERT 行号冲突修复） |
| `run_gate_e.sh` | Gate E — 性能 | `infinity bench` + `tools/ci_tools/check_benchmark_result.py`（<7% 门限） | **部分（实测 PASS）**：`infinity bench` 输出 `Total cost : <t> s` 机读行，`run_gate_e.sh` 直接以位置参数调用 checker；未给 C++ golden 时用**自基线**（两次运行互比，<7%）已 PASS。与 C++ golden 对齐需可比硬件 |
| `deny.toml` + CI | Gate F — 工程质量 | fmt / clippy `-D warnings` / test / cargo-deny；工具链钉 `1.94.1` | **达标**：fmt/clippy（`--workspace --all-targets`，0 warning）/`test --workspace`（0 failed）全绿；`cargo deny check licenses bans sources` 通过（补入 `0BSD`/`BSL-1.0` 白名单）；9/9 核心 crate `#![forbid(unsafe_code)]`。注：本地 advisory-db 过新（CVSS 4.0）与旧版 cargo-deny 不兼容，advisories 项需 CI 版工具 |
| `docs/INVARIANT_MATRIX.md` | Gate G — 不变量 | C++ 不变量 → Rust 测试对照 + 显式登记缺口 | **部分**：缺口 **G1/G3/G5 已闭合**（配置值域校验、跨底座宕机一致性、全文打分单调性各带回归测试）；G2（索引文件族契约）与 G4 的 sanitizer 部分仍待 |

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
