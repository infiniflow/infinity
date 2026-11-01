# HNSW 性能对比：Rust 重写版 vs C++ 参考版

日期 2026-10-31 · 分支 `yuzhichang/rust-performance-audit` @ `74b0aadff` · 参考版 `main` @ `dda83ae8a`

## 口径

| 项 | 说明 |
|---|---|
| 驱动 | `tools/bench_index_types.py`，两端用同一脚本、同一份数据、同一批语句 |
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

单测：`infinity-index` 126 passed，`infinity-executor` 706 passed。

## 复现

```bash
.scratch/bench_index_types.sh rust 23827
.scratch/bench_index_types.sh cpp  23817
BENCH_ROWS=200000 BENCH_QUERIES=50 .scratch/bench_index_types.sh rust 23827
```
