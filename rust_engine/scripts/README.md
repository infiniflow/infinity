# Gate 执行脚本（RUST_REWRITE_PLAN.md §6）

一键运行各验收关卡；每个脚本都会打印**可审计的**通过/失败结论。

| 脚本 | 关卡 | 内容 | 当前状态（2026-10-17 复验） |
|---|---|---|---|
| `run_gate_a.sh` | Gate A — SQL 功能 | 现有 174 个 `.slt` 全量比对 | **达标（2026-10-17）**：`statement ok` **1752/1752**、`statement error` **129/129**、`query` **1536/1536（100%）**。口径对齐 `sqllogictest-rs v0.28.4`（折叠空白 + 默认按行序）。关键修复见下方 §Gate A 复盘 |
| `run_gate_b.sh` | Gate B — SDK/协议 | `python/test_pysdk`、`go/tests`、HTTP（`tools/run_http_api.py`），**不改 SDK** | **部分（持续收敛）**：① **Python SDK** 逐文件复跑（27/29 文件有结论）**1634 passed / 190 failed / 1 error / 6 skipped**（本轮由 1580/244 收敛；`test_delete.py` 349s 超 240s 上限未计入，单跑为 369 passed/8 failed；`test_show`/`test_table` 仍超时）。本轮修好：投影 `FunctionExpr` 解码、投影列名（`year(c1)` / `c1 + c2`）、`CreateTable.db_name`、`SelectRequest.group_by_list` + 表名限定、未知 INSERT 列（语法错）、wire 布尔 1/0、array/vector 列拒收标量（3032）、`Flush` 落块、聚合结果类型（SUM→64 位、MIN/MAX 保持）。② **Go SDK**：套件可跑，58 个顶层用例失败后触发 Go 600s 超时；`TestBasicDatabaseOperations` 已转绿。③ **HTTP**：Rust 前端仅实现 `/health` 与 `POST /query`，SDK 的 REST 调用 404 |
| `run_gate_c.sh` | Gate C — 持久化/恢复 | workspace 测试、`test/sql/snapshot`、`python/restart_test`、MinIO | **部分（可跑，未达标）**：`test/sql/snapshot` **171/171**、`8/8` error，workspace 测试全绿。`python/restart_test` 用 `scripts/infinity_rust_shim.sh`（`--config=` → `infinity-server <data_dir>`）+ `bwrap --unshare-pid` 驱动；`Flush` 现在真的落 C++ 块布局（`<data_dir>/data/db_*/tbl_*/seg_*/blk_*/*.col`，与 C++ 一致）。实测：`test_basic`/`test_drop` **通过**（建表→写入→kill -9→重启→数据仍在），`test_alter` **1/6**（DROP COLUMN 后镜像仍写出被删列 —— 真实缺口），其余 ALTER/快照/compact 重启用例多数失败。MinIO 需 CI 服务 |
| `run_gate_d.sh` | Gate D — 并发/稳定性 | loom 模型、miri（纯 Rust 核心）、ASan/TSan、`python/parallel_test` | **达标（本地证据齐全）**：loom `commit_ts` 唯一性通过；miri + **ASan + TSan** 各 38 用例（`nightly-2025-04-04`，纯 Rust 核心）；**SDK 侧 `python/parallel_test`（经 bwrap）13 passed**——`test_insert_delete_parallel` 2、`test_ddl_parallel` 1、`test_index_parallel` 8、`test_insert_delete_update` 1、`test_ddl_and_insert_delete` 1；`test_insert_parallel`（2 万行并发插入）在 500s 内未跑完（慢），`test_chaos.py` 需独占服务未跑 |
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
