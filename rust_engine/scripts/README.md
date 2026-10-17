# Gate 执行脚本（RUST_REWRITE_PLAN.md §6）

一键运行各验收关卡；每个脚本都会打印**可审计的**通过/失败结论。

| 脚本 | 关卡 | 内容 | 当前状态（2026-10-17 复验） |
|---|---|---|---|
| `run_gate_a.sh` | Gate A — SQL 功能 | 现有 174 个 `.slt` 全量比对 | **达标（2026-10-17）**：`statement ok` **1752/1752**、`statement error` **129/129**、`query` **1536/1536（100%）**。口径对齐 `sqllogictest-rs v0.28.4`（折叠空白 + 默认按行序）。关键修复见下方 §Gate A 复盘 |
| `run_gate_b.sh` | Gate B — SDK/协议 | `python/test_pysdk`、`go/tests`、HTTP（`tools/run_http_api.py`），**不改 SDK** | **部分（大幅推进）**：`infinity-server` 已就位，**未改动的** Python SDK 可连上 Rust 服务。2026-10-17 用 `bwrap` 给非 root 用户一个可写 `/var/infinity/test_data`（绑定 `test/data/csv`）后**夹具报错从 592 降到 6**；逐文件复跑（26/29 文件完成）**1927 passed / 274 failed / 6 error / 6 skipped**。`test_show.py`/`test_table.py` 各自 >600s 未跑完（慢，非挂）；`test_delete.py` 349s 跑完（8 failed/369 passed，1024 条 DELETE 每条一次提交，见 Gate E）。`go`/HTTP 未跑（需串行独占端口）。命令：`bwrap --dev-bind / / --tmpfs /var --dir /var/infinity --bind <(cp test/data/csv)> /var/infinity/test_data --dir /var/infinity/log -- .venv/bin/python -m pytest python/test_pysdk` |
| `run_gate_c.sh` | Gate C — 持久化/恢复 | workspace 测试、`test/sql/snapshot`、`python/restart_test`、MinIO | **部分（已可跑，未达标）**：`test/sql/snapshot` **171/171**、`8/8` error，workspace 测试全绿。`python/restart_test` 现在**能驱动 Rust 服务**：`scripts/infinity_rust_shim.sh` 把 `<binary> --config=…` 映射为 `infinity-server <data_dir>`，`run_gate_c.sh` 用 `bwrap`（可写 `/var/infinity` + 夹具，`--unshare-pid` 让 harness 的「kill 所有 infinity 进程」只作用于本沙箱）。实测 `restart_test/test_basic.py` **1 passed**（建表→写入→kill→重启→数据仍在）；整套 restart_test 大多失败（ALTER/快照/compact 等重启路径），且部分用例很慢。MinIO 需 CI 服务 |
| `run_gate_d.sh` | Gate D — 并发/稳定性 | loom 模型、miri（纯 Rust 核心）、`python/parallel_test` | **部分**：loom `commit_ts` 唯一性已通过；miri 已在 `nightly-2025-04-04` 上通过（parser/expression/planner 共 38 个用例）；`python/parallel_test`（对 Rust 服务）**18/18**（含并发 INSERT 行号冲突修复） |
| `run_gate_e.sh` | Gate E — 性能 | `infinity bench` + `tools/ci_tools/check_benchmark_result.py`（<7% 门限） | **部分（实测 PASS）**：`infinity bench` 输出 `Total cost : <t> s` 机读行，`run_gate_e.sh` 直接以位置参数调用 checker；未给 C++ golden 时用**自基线**（两次运行互比，<7%）。**注意**：默认 2000 行的 select 仅 ~11ms，抖动易超 7%（2026-10-17 实测一次 7.81% FAIL）；用 `INFINITY_BENCH_ROWS=20000`（insert ~30k rows/s、select ~46ms）自基线 **PASS（-2.44%）**。与 C++ golden 对齐需可比硬件与 CI golden 日志（本环境无） |
| `deny.toml` + CI | Gate F — 工程质量 | fmt / clippy `-D warnings` / test / cargo-deny；工具链钉 `1.94.1` | **达标**：fmt/clippy（`--workspace --all-targets`，0 warning）/`test --workspace`（0 failed）全绿；`cargo deny check licenses bans sources` 通过（补入 `0BSD`/`BSL-1.0` 白名单）；9/9 核心 crate `#![forbid(unsafe_code)]`。注：本地 advisory-db 过新（CVSS 4.0）与旧版 cargo-deny 不兼容，advisories 项需 CI 版工具 |
| `docs/INVARIANT_MATRIX.md` | Gate G — 不变量 | C++ 不变量 → Rust 测试对照 + 显式登记缺口 | **达标（除打分对齐外）**：**G1–G5 全部闭合**并各带回归测试（G1 配置值域、G2 索引文件族契约、G3 跨底座宕机一致性、G4 miri+ASan+TSan、G5 全文打分单调性）；唯一范围外的是 §5.1 的**精确分值与 C++ 对齐**（属 Gate A 打分项） |

## Gate A 复盘（2026-10-17，1536/1536）

Gate A 从 1478 推到 100%，决定性的两步是**把 C++ 引擎真值测出来**，而不是继续对着 `.slt` 猜：

1. **容器内构建 C++ 引擎**取真值（见 `docs/CPP_FORMAT_SPEC.md` 记录的 recipe），直接量出：
   - `standard` 分析器的 **tokenizer 按字节分类**（`tokenizer_impl.cpp`）：ASCII 字母数字与 **>127 的字节全 ALLOW**（UTF-8 不切、em dash 不切词）、ASCII 空白是 SPACE、其余字节是丢弃的 DELIMITER、`/` 是 **UNITE**（拼进当前 token，但只在首字节之后，故前导 `/` 保留）。`2012/02/14 → 20120214`、`www.example.com/path → www, example, compath`、`a_b`/`a.b`/`a@b#c` 均切分。
   - **`standard` 同时索引 token 与 stem**（`extract_eng_stem_`），因此 `column_len` 与 `df` 都比"只数词"大——这正是此前 `avg_column_len` 在 `.slt` 间"互相矛盾"的真因。实测 enwiki **99/99 文档的 doc_len 完全一致、doc_len_sum = 18475**，`'harmful chemical'` 两边都是 **22.2996**。
2. 按真值重写：tokenizer 逐字节镜像、`token+stem` 索引、短语查询与 `bm25_param_delta_term`/`delta_phrase`、**逐字段** `df`/`len`/`avg`、C++ 风格行 id（`seg<<32|offset`）、`EXPLAIN FRAGMENT`、`HIGHLIGHT`、`^` 词级 boost。

教训（写给后来者）：`.slt` 分值是跨 revision 生成的**黑盒**，若 BM25 对不上，**不要**拟合 `len/avg` 的常数去迁就；先把 C++ 引擎跑起来量一次 tokenizer/列长度，问题会直接消失。早期"`wc-1` 才是对的"结论是错的（那只是在错误 tokenizer 下的局部最优）。

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
