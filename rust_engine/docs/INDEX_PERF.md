# 索引性能：脚本与报告索引

> 这份文档只做索引：**每个索引类型测什么、用哪个脚本、报告在哪、现在与参考差多少**。
> 具体的优化过程与逐项数字在各报告里，这里不重复。

## 1 一览

| 索引类型 | 驱动脚本 | 报告 | 复现命令 |
|---|---|---|---|
| 全文（fulltext） | `tools/bench_index_types.py`（跨引擎）／`infinity fulltext-bench`（进程内，参考 `fulltext_benchmark` 的移植） | [`FULLTEXT_BENCH.md`](FULLTEXT_BENCH.md) | `tools/run_index_bench.sh index-types` |
| 向量（HNSW） | `tools/bench_index_types.py` | [`HNSW_BENCH.md`](HNSW_BENCH.md) | `tools/run_index_bench.sh index-types` |
| 二级（secondary） | `tools/bench_secondary_lookup.py`、`tools/bench_secondary_maint.py` | [`SECONDARY_INDEX_BENCH.md`](SECONDARY_INDEX_BENCH.md) | `tools/run_index_bench.sh secondary-lookup` / `secondary-maint` |
| 张量（EMVB／PLAID／SMVE） | `tools/bench_multivector.py` | [`TENSOR_INDEX_BENCH.md`](TENSOR_INDEX_BENCH.md) | `tools/run_index_bench.sh multivector` |
| 稀疏（BMP） | `tools/bench_index_types.py` 的 `sparse` 一行 | 无专项报告（见 §4） | `tools/run_index_bench.sh index-types` |
| 聚合（insert／select／点查） | `tools/bench_sdk_driver.py` | `rust_engine/scripts/run_gate_e.sh` 的基线日志（`.scratch/infinity-bench*.log`） | `tools/run_index_bench.sh sdk` |

## 2 怎么跑

统一入口：`tools/run_index_bench.sh <workload> [rows] [queries]`。

| workload | 跑什么 | 输出行 |
|---|---|---|
| `index-types` | fulltext／hnsw／secondary／sparse：建索引 + 查询 | `INDEX <kind> build <s>`、`INDEX <kind> query <s>` |
| `multivector` | 张量列上的 EMVB／PLAID／SMVE | 同上（`kind` = `emvb`／`plaid`／`smve`） |
| `secondary-lookup` | 二级索引：建索引、点查／范围、带索引与不带索引的 DML | `SEC <phase> <s>` |
| `secondary-maint` | 每多一个在线索引，DML 每条语句贵多少 | `MAINT <phase> <s>  # <us/row>` |
| `sdk` | 聚合 workload（Gate E 用的那个驱动） | `Total cost : <s> s` |

约定：

* **两端同一个脚本、同一批 SQL、同一台机**；每个引擎单独跑（先 cpp 后 rust）。
* 每个引擎在 `bwrap --unshare-net` 的**私有网络命名空间**里启动，端口 23817 属于本次运行；
  别的会话同时开着引擎也不会被连上（这是之前吃过的亏：测的其实是别人的库）。
* 起停与配置集中在 `tools/run_index_bench_one.sh`：数据目录在 `.scratch/bench-<workload>-<engine>-<pid>/`，
  `optimize/cleanup/compact/checkpoint` 间隔全 `0s`、`result_cache = off`——测的是语句路径，不是恰好跑起来的后台线程。
* 环境变量：`BENCH_ROWS`（默认 10 万）、`BENCH_QUERIES`（默认 50）；两个 secondary workload 读 `SEC_ROWS`。
* `tools/run_secondary_bench.sh lookup|maint` 是 `secondary-lookup`／`secondary-maint` 的别名，报告里引用了它。

**下限提醒**：EMVB 要求 embedding 数 ≥ 8192（`emvb_index_impl.cpp:165`），按每行 4 个向量算就是 **≥ 2048 行**；
低于这个数参考引擎会以 `UnrecoverableError` 退出，`multivector` 这一项会只剩 `emvb` 之前的输出。

## 3 各索引的现状

| 索引 | 建索引 | 查询 | 备注 |
|---|---|---|---|
| 全文 | — | 1–2 词的查询反超参考；13／14 词 1.0–1.7× | 写作 `IMPORT` 已反超，`INSERT` 的 2× 是计时口径（`FULLTEXT_BENCH.md` §7） |
| HNSW | 20k 行 **0.87×**（快 13%）、200k 行 1.18× | 端到端 2.3×、服务端 exec 5.5×（20k）；200k 服务端 1.12× | 差距是每跳的图遍历常数，数据量变大后被块读取摊薄 |
| 二级 | 1.7–1.8×（绝对值 53–60 ms / 10 万行） | 点查／范围 **0.68–1.04×** | DML 维护成本已落到噪声；见 `SECONDARY_INDEX_BENCH.md` §2–§3 |
| 张量 | EMVB **0.50×**、SMVE **0.49×**、PLAID 1.12× | EMVB 0.85×、SMVE 1.02×、PLAID 1.4× | `TENSOR_INDEX_BENCH.md`；PLAID 的 query 差距来自每段打分 |
| 稀疏 | 见 `INDEX sparse build` | 见 `INDEX sparse query` | 只有 `index-types` 的一行覆盖，没有专项调优 |

## 4 缺口与维护约定

* **稀疏（BMP）没有专项报告**：`index-types` 只覆盖「建索引 + 一次查询」，`benchmark/local_infinity/sparse/` 下的
  参考基准（`bmp_benchmark`、`sparse_benchmark`）还没移植。要补就先移植这两个，再按本节下面的约定放。
* **各报告的数据规模不同**（全文 20k 文档、HNSW 20k/200k、二级 10 万行、张量 20k 行），比值只在本报告的口径内可比；
  跨报告不要横向比较绝对值。
* **新增一个基准时**：驱动放 `tools/bench_<x>.py`（只吃 `host/port/rows/queries`），在 `tools/run_index_bench.sh` 里加一个
  `case` 分支，报告放 `rust_engine/docs/<X>_BENCH.md` 并在本文件的 §1 表里登记。
* **不要把起停脚本放 `.scratch`**：那是一次性的，别人复现不了（`bench_multivector.py` 早期就是这样，本文件落地时并进了 runner）。
* **大数据量手工验收**（enwiki 全文 33M、SIFT HNSW 1M、峰值内存、8.39M 全文逐字节与跨引擎互读）见 [`MANUAL_ACCEPTANCE.md`](MANUAL_ACCEPTANCE.md)。
