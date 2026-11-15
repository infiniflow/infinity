# HNSW 性能对比：Rust 重写版 vs C++ 参考版

日期 2026-10-31 · 分支 `yuzhichang/rust-performance-audit` @ `74b0aadff` · 参考版 `main` @ `dda83ae8a`

## 口径

| 项 | 说明 |
|---|---|
| 驱动 | `tools/bench_index_types.py`（`tools/run_index_bench.sh index-types` 起停两端引擎），两端用同一脚本、同一份数据、同一批语句 |
| 数据 | `id int, title varchar, body varchar, vec vector,4,float, sp sparse`；`vec` 是小整数格点 |
| 索引 | HNSW `M=16, ef_construction=200, metric=l2` |
| 查询 | `match_dense(vec, [0.5,0.5,0.25,0.75], l2, 10)`，warm 1 次后连续 50 次取均值 |
| build | `create_index` 一次调用的墙钟时间，含读列 + 构图 + 落 chunk |
| query | 客户端端到端（含 thrift 往返、解析、序列化） |
| exec | 服务端 `Select` 内部计时段（`INF_SERVER_TIMER=1`），两端同一插桩位置 |
| 机器 | i5-13500H，16 核，测试期间有其他后台负载（load 5~8），故取多次运行区间而非单点 |

## 结果

### 20k 行

| 指标 | Rust | C++ | 比值 |
|---|---|---|---|
| build | 0.225 – 0.244 s | 0.252 – 0.291 s | **0.87×**（Rust 快 ~13%） |
| query（端到端） | 0.50 – 0.55 ms | 0.22 – 0.24 ms | 2.3× |
| query（服务端 exec） | 0.317 ms | 0.058 ms | 5.5× |

### 200k 行

| 指标 | Rust | C++ | 比值 |
|---|---|---|---|
| build | 3.35 – 3.59 s | 2.90 – 2.94 s | 1.18× |
| query（端到端） | 0.69 – 0.90 ms | 0.35 – 1.07 ms | 噪声大于差值 |
| query（服务端 exec） | 0.459 ms | 0.409 ms | 1.12× |

### 结果一致性

两端对同一查询返回的 top-10 完全一致：

```
[825, 2086, 3347, 4608, 5869, 7130, 8391, 9652, 10913, 12174]
```

## 结论

1. **构图已达到参考版水平**。20k 行 Rust 快约 13%；200k 行慢约 18%，差距随数据量上升，落点在反向链接剪枝时的锁竞争（参考版用 `shared_mutex` 的 `UniqueLock`，Rust 侧每个顶点一把 `RwLock`，桶越界越明显）。
2. **查询是小表差距大、大表差距消失**。20k 行服务端 exec 慢 5.5×（0.317 vs 0.058 ms），200k 行只慢 1.12×（0.459 vs 0.409 ms）。20k 时 Rust 的绝对成本是一个与数据量无关的常数（图遍历 + 结果行读取），到 200k 时两端都被块读取主导，固定开销被摊薄。端到端差距（2.3×）小于服务端差距，说明客户端/协议层两端成本相当，差的都在引擎内。
3. **建图与查询是两条独立的路径**，本次改动只动了构图分配，查询侧的常数开销（catalog 解析、块 version 读取、结果行解码）仍在，是下一步的落点。

## 本轮改动

`74b0aadff` — hnsw: stop paying for allocations the build and the index lookup do not need

- `SearchScratch` 拆出 `Visited`，两个 `BinaryHeap` 与邻居缓冲搬到调用方复用（原先每层每顶点新建）
- `SelectNeighborsHeuristic` 的三个 `Vec` 合并成一个复用缓冲
- 构图查询向量改用 `QueryRef::Node` 直接点对点算距离，省掉每顶点一次 `GetVecToQuery` 拷贝
- 二级索引等值查找只解码命中的行（`read_columns_batch_for_rows`），不再整表 decode
- block `version` 文件与解码块一起缓存，所有重写点同步失效

单测：`index` 126 passed，`executor` 706 passed。

## 复现

```bash
# 20k 行：两端各起一次引擎，同一个驱动
tools/run_index_bench.sh index-types 20000 50
# 200k 行
tools/run_index_bench.sh index-types 200000 50
```

驱动本身是 `tools/bench_index_types.py`，它连一个已经在跑的引擎（`127.0.0.1:23817`）；
`run_index_bench.sh` 负责起停两端（各在自己的网络命名空间里）。

## 规模验收：LSG 语义与查询路径开销（2026-11-15）

口径：单进程内建索引后直接查询（不经重启），数据 `id int, v embedding(float,128)`，
1000 个簇 + 每维抖动（SIFT-like），`M=16, ef_construction=200, metric='l2', encode='lvq'`，
`SEARCH MATCH VECTOR` 取 top-10，真值为对同一批向量的穷举 L2 top-10。

```bash
cargo test -p executor --release --lib hnsw_production_scale -- --ignored --nocapture
# 规模/编码/构建类型用 INFINITY_BENCH_ROWS / INFINITY_BENCH_ENCODE / INFINITY_BENCH_BUILD_TYPE
```

### 1. `build_type=lsg` 与参考实现对齐

| 构建 | recall@10 | 重启后 recall@10 | 构图耗时（200k） |
|---|---:|---:|---:|
| `plain` | 0.998 | 0.978 | 4.3 s |
| `lsg`（缩放真正生效，本仓库旧行为） | 0.674 → 0.860\* | 0.936 | 8.7 s |
| `lsg`（与参考一致：只记录参数） | **0.994** | **0.994** | 4.0 s |

\* 先把采样概率区间和并行构建的贪心下降对齐参考（见下），缩放生效时也只到 0.860，仍显著低于 plain。

1M 行（`INFINITY_BENCH_ROWS=1000000`）下两者已经重合：

| 构建 | recall@10 | 重启后 recall@10 | 图搜索 p50（ef=64） | 构图 |
|---|---:|---:|---:|---:|
| `plain` | 0.906 | 0.852 | 165 µs | 28.1 s |
| `lsg`（与参考一致） | 0.904 | 0.848 | 166 µs | 27.6 s |

结论：参考实现的 `HnswHandler` 会为 `build_type=lsg` 构造 `HnswLSGBuilder`
（`hnsw_handler_impl.cpp:218`），但**任何生产路径都不训练它**——`InsertSampleVecs` /
`InsertLSAvg` / `SetLSGParam`（填写 `avg_` 并装进 `LSGDistWrapper` 的三步）只出现在
`test_lsg_build_ut.cpp`，`HnswHandler::InsertVecs`（`hnsw_handler.cppm:151`）从
`StoreData` 直接到 `Build`，`new_txn_index_impl.cpp:942` 调的就是它。于是
`LSGDistWrapper::avg_` 恒为 `nullptr`，`operator()` 返回原始距离
（`dist_func_lsg_wrapper.cppm:51`：`v1_i == kInvalidVertex` 直接返回 `d`），
参考版的 lsg 索引就是"带 LSG 参数的 plain 索引"。

在缩放真正生效的情况下，构图在缩放空间选边、查询在原始空间排序，两者不一致，
L2 召回必然低于 plain——这不是参数没调好，而是语义错配。因此本仓库按参考实现的
做法处理：`IndexHnsw` 照旧解析/校验/持久化 `lsg_config`，`show_index` 照旧报告，
但构图不做缩放；`LsgIndex`（`HnswLSGBuilder` + `LSGDistWrapper` 的移植）与其单测保留，
`HnswIndex::adjust` 在拿到缩放的图上仍然生效。

同时修掉两处与参考不一致的实现细节（这两处只影响"缩放生效"的分支）：

- 采样概率：`(state >> 33) as u32 / u32::MAX` 只覆盖 `[0, 0.5)`，`sample_ratio=0.01`
  实际采样了 2%（`hnsw_lsg_builder.cppm` 用 `rand() % 1000 / 1000.0`）。改为 `[0, 1)`。
- 并行构建的 `SearchLayerNearest` 传 `None`，而参考的 `Build` 把 `vertex_i` 传给每一次
  取距（`hnsw_alg.cppm:401`），即上层下降也用缩放距离。已对齐。

### 2. 查询路径：去掉与表规模成正比的额外开销

200k 行、同一批 50 次查询：

| 指标 | 修复前 | 修复后 |
|---|---:|---:|
| 服务端 p50（SQL → top-10） | 33.6 ms | **0.58 ms** |
| 冷启动首查 | 500 ms+ | 26 ms（一次性计数） |
| 图搜索本身 p50（`ef=64`，不进 SQL） | 0.09 ms | 0.09 ms |

修复后 p50 不再随表规模增长（同一套语句，LVQ）：

| 行数 | p50 | 冷启动首查 | 图搜索 p50（ef=64） |
|---|---:|---:|---:|
| 50k | 0.53 ms | 7.4 ms | 0.07 ms |
| 200k | 0.58 ms | 23 ms | 0.09 ms |
| 1M | 0.83 ms | 0.38 s | 0.17 ms |

根因不在图搜索，而在两处"按表规模付费"：

1. `read_blocks_values_at` 无条件克隆整个 append buffer（行尚未 checkpoint 时是整表），
   每行是深拷贝的 `Vec<Value>`（含 128 维向量文本）——20 万行约 100 MB/次查询。
   现在候选读取只按 id 探测缓冲（`buffered_rows_for` 改为遍历 ids，走
   `pending_pos`/按 id 有序缓冲的查找）。
2. 覆盖判定（"图是否覆盖整表"）依赖热向量缓存，而缓存未建立时 `live_rows` 也还没发布，
   于是首查判定为"图不完整"→ 回退全表精确扫描，并顺手把整列向量解析进缓存。
   现在覆盖判定用 `live_count`（发布计数，缺失时用 `row_count` 兜底并发布），
   精确扫描才建缓存。

剩余常数开销：候选行按列解码 + 结果行按块读取（10 行可能落在 10 个块里，每块按列解码）。
与参考版 200k 行 0.409 ms（服务端 exec）同一量级。
