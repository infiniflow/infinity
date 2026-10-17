# Gate 执行脚本（RUST_REWRITE_PLAN.md §6）

一键运行各验收关卡；每个脚本都会打印**可审计的**通过/失败结论。

| 脚本 | 关卡 | 内容 | 当前状态（2026-10-17 复验） |
|---|---|---|---|
| `run_gate_a.sh` | Gate A — SQL 功能 | 现有 174 个 `.slt` 全量比对 | **未达标**：`statement ok` 1752/1752、`statement error` 129/129、`query` **1488/1536**（要求 100%；口径对齐 `sqllogictest-rs v0.28.4`：折叠空白 + 默认按行序）。剩余 48 条以全文 BM25/FUSION 打分为主（见 §Gate A 残留） |
| `run_gate_b.sh` | Gate B — SDK/协议 | `python/test_pysdk`、`go/tests`、HTTP（`tools/run_http_api.py`），**不改 SDK** | **部分**：`infinity-server` 二进制已就位（`cargo build -p infinity-server`），未改动的 Python SDK 可连上 Rust 服务，`test_pysdk`（全量未改动 SDK，2259 例）**1448 passed / 212 failed / 598 errors / 7 skipped**（598 error 多为 `/var/infinity` 夹具权限） |
| `run_gate_c.sh` | Gate C — 持久化/恢复 | workspace 测试、`test/sql/snapshot`、`python/restart_test`、MinIO | **部分**：`test/sql/snapshot` **171/171**、`8/8` error，workspace 测试全绿；MinIO 需 CI 服务；`python/restart_test` 的 runner 以 `<binary> --config=…` 启动并写 `/var/infinity`，需适配 Rust 服务 CLI，且当前环境不可写 |
| `run_gate_d.sh` | Gate D — 并发/稳定性 | loom 模型、miri（纯 Rust 核心）、`python/parallel_test` | **部分**：loom `commit_ts` 唯一性已通过；miri 已在 `nightly-2025-04-04` 上通过（parser/expression/planner 共 38 个用例）；`python/parallel_test`（对 Rust 服务）**18/18**（含并发 INSERT 行号冲突修复） |
| `run_gate_e.sh` | Gate E — 性能 | `infinity bench` + `tools/ci_tools/check_benchmark_result.py`（<7% 门限） | **部分（实测 PASS）**：`infinity bench` 输出 `Total cost : <t> s` 机读行，`run_gate_e.sh` 直接以位置参数调用 checker；未给 C++ golden 时用**自基线**（两次运行互比，<7%）。**注意**：默认 2000 行的 select 仅 ~11ms，抖动易超 7%（2026-10-17 实测一次 7.81% FAIL）；用 `INFINITY_BENCH_ROWS=20000`（insert ~30k rows/s、select ~46ms）自基线 **PASS（-2.44%）**。与 C++ golden 对齐需可比硬件与 CI golden 日志（本环境无） |
| `deny.toml` + CI | Gate F — 工程质量 | fmt / clippy `-D warnings` / test / cargo-deny；工具链钉 `1.94.1` | **达标**：fmt/clippy（`--workspace --all-targets`，0 warning）/`test --workspace`（0 failed）全绿；`cargo deny check licenses bans sources` 通过（补入 `0BSD`/`BSL-1.0` 白名单）；9/9 核心 crate `#![forbid(unsafe_code)]`。注：本地 advisory-db 过新（CVSS 4.0）与旧版 cargo-deny 不兼容，advisories 项需 CI 版工具 |
| `docs/INVARIANT_MATRIX.md` | Gate G — 不变量 | C++ 不变量 → Rust 测试对照 + 显式登记缺口 | **达标（除打分对齐外）**：**G1–G5 全部闭合**并各带回归测试（G1 配置值域、G2 索引文件族契约、G3 跨底座宕机一致性、G4 miri+ASan+TSan、G5 全文打分单调性）；唯一范围外的是 §5.1 的**精确分值与 C++ 对齐**（属 Gate A 打分项） |

## Gate A 残留（48 条 `query`，2026-10-17）

- **全文 BM25 打分（约 43 条，未解）**：`fulltext.slt` 19、`enwiki_embedding` 12（`fusion.slt` 的文本分量）、`cached_fulltext`/`cached_limit` 4、`ft_minimum_should_match` 的 3 条 `ROUND(SCORE())`、`highlight` 2、两个 `fusion_rerank_*` 的纯文本查询 2、`fulltext_keyword` 1（`minimum_should_match` 的分组语义）。C++ 公式（`bm25_ranker_impl.cpp`、`term_doc_iterator_impl.cpp`）已逐字复刻：`idf = ln(1+(N-df+0.5)/(df+0.5))`、`tf/(tf+k1(1-b+b·len/avg))`、k1=1.2、b=0.75，且 `N`/`df` 为**整表**口径。唯一未解量是 **`avg_column_len`**（`term_doc_iterator_impl.cpp:65` 的 `doc_len/doc_cnt`）：`len = 词数−1` 被 `fulltext_delete.slt` 印证（0.1679/0.1335/0.1109 逐位命中），但 `ft_minimum_should_match` 需要 `avg≈2.10`（源码口径应为 2.0）、`fusion_rerank_maxsim` 需要 `len = 词数`——**夹具间不一致**（`fulltext_delete` 与 `ft_minimum` 前 3 行文本相同、均值也相同，却要求不同的 `avg`），疑与 iresearch 的**分段/合并**（`fulltext.slt` 每 `COPY` 一次同一查询打分就变：22.2996→23.2411→26.3705，行 id 呈 `seg<<32`）有关。定论前不要改 `len/avg`（`(wc-1,wc-1)` 已是四种组合中实测最好者）。工具：`tools/bm25_probe/gen.py`。
- **`FUSION` 子项被文本打分污染（12 条）**：`fusion.slt` 的文本分量错 → 排位/数值错 → RRF rank、minmax 的 min/max、加权和全变。融合算子本身已逐位对齐（向量分量实测命中），**单独修融合无法提分**。
- **其余 5 条（非打分，可做）**：`filter_fulltext(…, 'threshold=…')`（表达式层拿不到表统计，2）、派生表 `FROM (SELECT … SEARCH …)`（1）、`filter_fulltext('col@sparsegram…')`（1）、链式 `FUSION` 的 `minimum_should_match` 分组语义（1）。已落地：`FUSION('match_tensor')` 的 MaxSim 重排、链式 `FUSION` 解析、`FUSION` 的 `EXPLAIN` 文本（键排序 + 按叶子数编号）、逐字段 `similarity=boolean` 与列无关的 `field:term` 查询。

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
