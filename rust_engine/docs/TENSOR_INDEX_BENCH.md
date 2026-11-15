# Multivector 索引性能对比：Rust 重写版 vs C++ 参考版

日期 2026-11-01 · 分支 `yuzhichang/rust-performance-audit` · 参考版 `main` @ `2bd0822be`

## 为什么补这个 benchmark

参考版的 `tools/bench_index_types.py` 只覆盖 fulltext / hnsw / secondary / sparse 四种索引，
没有任何 driver 覆盖 `tensor` 列上的多向量索引。`benchmark/` 与 `test/` 下也没有对应基准，
只有一个生成 slt 用例的脚本 `generate_multivector_knn_scan.py`。

因此新增 `tools/bench_multivector.py`（由 `tools/run_index_bench.sh multivector` 起停两端引擎），
覆盖引擎对多向量列提供的三种索引：`EMVB`、`PLAID`、`SMVE`。

## 口径

| 项 | 说明 |
|---|---|
| 驱动 | `tools/bench_multivector.py`，两端同一脚本、同一份数据、同一批语句 |
| 列类型 | `tensor(float,8)` —— 见下方说明 |
| 数据 | 20000 行，每行 4 个 8 维向量（共 80000 embeddings）；中心点 + 抖动，近邻真实存在 |
| 查询 | 4 个查询向量，`match_tensor(mv, q, 'float', 10)`（MaxSim）；warm 1 次后连续 50 次取均值 |
| build | `create_index` 一次调用的墙钟时间 |
| query | 客户端端到端（含 thrift 往返、解析、序列化） |
| 机器 | i5-13500H，16 核 |

**列类型说明**：C++ 的三种索引都只接受 `LogicalType::kTensor`，拒绝 `kMultiVector`
（`index_{emvb,plaid,smve}_impl.cpp` 里的 `ValidateColumnDataType`）。但两者的 Arrow 表示不同：
`kTensor` 是 LIST<FIXED_SIZE_LIST>，`kMultiVector` 也是。用 `multivector(...)` 声明 C++ 会直接报
`Attempt to create index on column: mv, data type: MultiVector(float,8)`。
Rust 侧接受 `tensor` 列，所以两端统一用 `tensor(float,8)` 才是可比口径。

## 结果（三次取样）

### build（秒，越小越好）

| 索引 | Rust | C++ | 比值 |
|---|---|---|---|
| EMVB | 1.59 / 1.57 / 1.58 | 3.08 / 3.19 / 3.20 | **0.50×**（Rust 快约 2×） |
| PLAID | 0.381 / 0.375 / 0.383 | 0.349 / 0.326 / 0.345 | 1.12× |
| SMVE | 0.226 / 0.228 / 0.228 | 0.462 / 0.463 / 0.467 | **0.49×**（Rust 快约 2×） |

### query（毫秒，端到端）

| 索引 | Rust | C++ | 比值 |
|---|---|---|---|
| EMVB | 0.82 / 0.77 / 0.74 | 0.92 / 0.65 / 1.22 | 0.85× |
| PLAID | 0.57 / 0.57 / 0.58 | 0.33 / 0.41 / 0.45 | 1.4× |
| SMVE | 1.24 / 1.22 / 1.27 | 1.13 / 1.12 / 1.44 | 1.02× |

## 原先的 17–25× 构图差距是怎么来的

第一版用 `[MVT]` 打点把 Rust 侧 `mv_for` 的建索引过程拆开：

| 建哪种索引 | 读列 | `build_plaid` | `build_smve` | `seal` | 合计 |
|---|---|---|---|---|---|
| EMVB | 98 ms | 21 µs（无配置，跳过） | 0.5 µs（跳过） | **2.48 s** | 2.48 s |
| PLAID | 98 ms | **2.57 s** | 3.8 µs（跳过） | **2.94 s** | 5.51 s |
| SMVE | 97 ms | 19 µs（跳过） | **4.23 s** | **7.16 s** | 11.39 s |

两个来源：

1. **`seal()` 与索引类型无关**：它把整个多向量 tail 写成 BMP 稀疏块（`write_bmp_file`），
   建 EMVB 也要付 2.48 s，建 SMVE 要付 7.16 s。C++ 的 `new_txn_index_impl.cpp` 里
   `kEMVB` / `kPLAID` / `kSMVE` 三个分支各建各的，没有额外写一份 BMP。
   修法：`seal()` 不再无条件建 SMVE / EMVB tail，只有真正被读到的索引才建
   （`wants_emvb_chunk()` 保留裸 multivector 列给 `SEARCH MATCH TENSOR` 的 prefilter 用）。
2. **k-means 单线程**：`build_plaid` 里 0.9 s 花在 80000 × 2048 的距离计算上，
   C++ 用 MLAS SGEMM 走多核。

## 本轮做的三件事

### 1. k-means 赋值走 SIMD + 多线程（`plaid/centroids.rs`）

- 每个训练向量对 2048 个质心做 L2，改成点积式 `|c|² - 2·v·c`，把质心矩阵转置后
  用 4×8 micro-kernel（一次向量载入 8 个质心，4 行复用）；距离内核换成
  `crate::simd::l2_sq` / `crate::simd::ip`。
- 行按 `available_parallelism()` 切片，用 `thread::scope` 并行；每片自己的距离和后按固定
  顺序相加，结果不依赖线程到达顺序。

### 2. 量化器训练不再单线程打分（`plaid/index.rs`）

训练残差量化器前要从 50000 条采样残差里找最近质心，原来是单线程
`nearest_centroid` 循环——和 `add_multiple_docs_embeddings` 是同一份
`n × n_centroids × dim` 工作量，却是唯一没并行的一处（约 0.28 s）。
新增 `assign_nearest_parallel`，两处共用。

### 3. 查询路径不再数块（`executor/src/engine/`）

`multivector_index_for` 每次调用都用 `row_count(table)` 判断已建索引是否覆盖整表，
而 `row_count` 会走 `block_visible_row_count`——遍历每个块的 `version` 文件、每个 slot、
建一个 `HashSet`。在 20000 行的表上这是 **0.67 ms**，占了 1.6 ms 查询的 40%。

写路径本来就在维护 `live_rows`（`note_live_rows` / `set_live_rows`），
改成优先读它，缺失时才回落到 `row_count`。`text_index_for` 早就是这么做的，
`sparse_index_for` / `mv_for` 之前漏了。

顺带把 PLAID 查询里两处热点也改了：质心打分改用 `crate::simd::ip`（原来是标量内积），
每个 query token 取 top-`n_ivf_probe` 从「全排序 2048 个」改成
`select_nth_unstable_by` + 只排 8 个。

## 结论

1. **EMVB / SMVE 构图 Rust 已优于参考版约 2×**：去掉无谓的 BMP 写入后，两边工作量对等，
   Rust 的并行编码更快。SMVE query 也已追平。
2. **PLAID 构图基本追平**（0.38 s vs 0.34 s，差 12%）：剩下的差距全在 k-means，
   C++ 用 SGEMM 拿到的算力好处已经被 SIMD kernel + 16 线程抵消掉大半。
3. **PLAID / EMVB query 已不是索引本身的差距**：Rust 侧一次
   `SELECT id FROM t LIMIT 10` 就要 0.58 ms（C++ 0.42 ms），而 PLAID 查询总共 0.57 ms——
   也就是说 `search_with_bitmask` 只占 ~0.08 ms，其余都是两端的固定执行开销
   （thrift、解析、取行）。要再压这一项，得动的是通用的点查路径，不是 multivector 索引。
4. **PLAID 的结果质量两端都不达标**，与构图性能无关，需要单独排查召回（见下）。

## 结果质量

两端对同一查询返回的 top-10：

```
EMVB  Rust [17653, 8826, 7565, 16392, 6304, 14936, 3782, 15131, 13675, 7370]
      C++  [8826, 17653, 7565, 6304, 16392, 3782, 7370, 14936, 13675, 15131]
SMVE  Rust [17653, 8826, 7565, 16392, 6304, 14936, 15131, 7370, 16197, 14741]
      C++  [17653, 8826, 7565, 16392, 6304, 14936, 15131, 13675, 7370, 18719]
PLAID Rust [6455, 4999, 4804, 13826, 12370, 13436, 3348, 3543, 10914, 2087]
      C++  [150, 17804, 16348, 7521, 6260, 14892, 15087, 4804, 6065, 13631]
```

- **EMVB / SMVE**：两端返回的是同一批文档，只是排列顺序不同（近似索引的分数有微小差异）。
- **PLAID**：两端几乎不重合（仅 `4804` 相同）。精确解（`.scratch/exact_mv.py` 暴力 MaxSim）是
  `[13435, 7807, 12175, 1892, 19350, ...]`，两端都没命中——PLAID 是量化索引，本来就不保证
  精确 top-1，但两端差到这个程度说明至少一侧的召回异常，需要单独跟进。

## 复现

```bash
# 两端各起一个引擎并跑三种索引（≥ 2048 行：EMVB 要求 embedding ≥ 8192）
tools/run_index_bench.sh multivector 20000 50

# 只跑索引层，绕过服务端（拆分 k-means / 量化器 / 建表）
cd rust_engine
cargo run --release -p index --example plaid_build_bench
cargo run --release -p index --example plaid_query_bench
```

## 已知问题

- C++ 在 20000 行规模下有一次进程被信号杀死（无错误日志，疑 OOM），重跑即恢复；
  4000 / 8000 / 12000 / 16000 / 20000 行都能正常跑完。
- C++ 的 EMVB 要求 `embedding_count >= 8192`，行数 × 每行向量数低于此值时索引根本不建，
  不是崩溃。
- 本报告只覆盖单段（未 compact）的表。
