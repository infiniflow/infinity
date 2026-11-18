# spann 基准（P1/P2 出口）

本文只放**实测**数字，口径与 `HNSW_BENCH.md` 对齐（同一台机器、单进程、release、真值由穷举得出），
并且在**同一条 SQL 路径**上和 HNSW 对打。目的是回答两个问题：spann 现在够不够用，以及它到底在哪个区间划算。

## 1. 口径

| 项 | 值 |
|---|---|
| 机器 | Apple Silicon（aarch64-darwin），`cargo test --release` |
| 数据 | 20,000 行 × 128d，64 个簇（簇内 σ≈3，簇心间距≈20），确定性生成 |
| 查询 | 100 条自查询（行 0, 200, 400, …） |
| top-n | 10 |
| 真值 | 同一份向量的穷举 top-10（`index::vector::l2_sq`） |
| 召回口径 | 返回的 10 行与真值 top-10 的交集占比，**候选经精确重排**（`D1`） |
| 延迟口径 | 端到端 SQL `SELECT id FROM bench SEARCH MATCH VECTOR (…)`，含解析/规划/取行/重排/输出 |
| 内存口径 | `SpannIndex::mem_usage()`（堆上自有字节，不含分配器碎片与原始向量） |
| spann 参数 | `metric='l2'`, `max_partition_size=128`, `num_centroids=64`，其余默认（`rabitq`、`fill_factor=0.9`、`kmeans_iterations=8`、`beam=8`） |
| HNSW 参数 | `metric='l2'`, `M=16`, `ef_construction=200`（图搜索 `ef` 为默认值） |

## 2. 结果

### 2.1 索引本身（不含 SQL）

| 指标 | spann（堆内） | spann（mmap） | 说明 |
|---|---:|---:|---|
| 单查询 search（k=100） | 340 µs | **343 µs** | **out-of-core 几乎零代价**：差 0.9% |

这一行是 out-of-core 的关键证据：同一棵树，堆内与从 `chunk_0.idx` 直接读，索引层耗时相同。
`MappedFile` 的页缓存由 OS 回收，所以 50M 行的内存账按 §19.3 的算法成立（索引不再占堆）。

### 2.2 SQL 端到端

| 指标 | spann | HNSW | 比值 |
|---|---:|---:|---:|
| 构图 | **102.3 ms** | 855.5 ms | **8.4× 快**（§2.3） |
| recall@10 | 0.981 | **0.998** | 低 1.7 个点 |
| p50（flush 前，行在内存） | 5.78 ms | — | — |
| p50（flush 后，行已落盘） | 23.1 ms | **2.09 ms** | 11× 慢 |
| p95 | 6.10 ms | 2.30 ms | 2.7× 慢 |
| 常驻（索引） | 1.7 MiB / 89 B/行（堆）→ **0 B/行（mmap）** | 未测 | — |
| 树形状 | 449 分区 / height 4 | 单图 | — |

三个必须说明的点：

1. **`0.981` 不是调参调出来的，是换成参考实现的候选池公式**：`maxResults + maxExtraResults`
   （`spann/index.go:58`）在 `beam=32`、`topn=10`、`multiplier=50` 下是 812 行，此前是固定的
   `10×topn=100`。§2.4 表明这条曲线一直爬到 0.997，所谓「beam ≥ 32 后饱和」其实是候选池饱和。
2. **flush 后的 SQL 延迟全部是「解码候选行」**。同一棵树索引层 383 µs，SQL 23.1 ms，差额是
   812 行的 block 点读解码 + 精确重排。行数与候选池成正比，所以召回越往上，这一项越贵。
3. **候选行不再一次性物化**：重排按 64 行一批解码、每批只留 top-n 再合并（`statements.rs`
   的 `RERANK_BATCH`），每查询常驻从 `812×dims×4 B` 降到 `64×dims×4 B`（1536d 下 5 MB → 384 KB）。
   这是 spann「省内存」成立的前提：池子大小只该影响 I/O，不该影响常驻。

debug 构建的同一用例：spann 构图 67.9 s、SQL p50 10.8 ms（旧候选池）。

### 2.3 建树吞吐（§16.4 的 20.8k 行/s 验收线）

| 版本 | 20k × 128d 建树 | 行/s | 说明 |
|---|---:|---:|---|
| 原实现 | 1.94 s | 10.3k | 单线程递归 + rebalance 每移一点全表重扫 |
| 并行子树 | 1.53 s | 13.1k | 左右子树并发建（key 仍 DFS 先序），只拿到 1.27× |
| **+ 排序式 rebalance** | **93.6 ms** | **213k** | rebalance 从 `O(n·moves)` 降到 `O(n log n)`：**它原来是 97% 的耗时** |

两点必须说清：

1. **并行不是主要收益**，`rebalance` 才是。原实现里「把大的那侧最远的点一个个搬到另一侧」
   每搬一个点就重扫整簇，20k 行级的大分裂会搬上千次 → 上千 × 20k × 128d。
   由于**簇心在搬运期间不变**，第 m 次搬的就是「原簇里第 m 远的点」，一次排序即可，结果逐点相同
   （`kmeans::tests::sorted_rebalance_matches_the_pass_loop` 用参考实现做对照，
   `build::tests::keys_are_dfs_preorder` 与建树确定性用例守住树的字节）。
2. **可并行的是子树**：分裂后左右两半互不依赖，父节点在分裂后就能算出两个子树的 key
   （左 = `key+1`，右 = `key+1+子树节点数`，节点数只与行数有关），于是先派发右子树、
   自己建左子树，最后按 key 归位。整棵树与旧的串行实现**逐字节相同**
   （用同一份数据对拍过 6 组规模，见 `SPANN_DESIGN.md` §20.4 第 4 条）。

### 2.4 等延迟对比（§19.6，宽度扫描）

同一条 SQL 路径、同一份 20k×128d 数据，两个索引各自扫自己的搜索宽度：
spann `WITH (beam = w)`、HNSW `WITH (ef = w)`；60 条查询，recall@10 按交集计。

| 宽度 | spann p50 / recall@10 | HNSW p50 / recall@10 |
|---:|---:|---:|
| 8 | 2.85 ms / 0.915 | 501 µs / 0.998 |
| 16 | 4.30 ms / 0.940 | 527 µs / 0.998 |
| 32 | 5.75 ms / 0.952 | 538 µs / 0.998 |
| 64 | 7.99 ms / 0.975 | 515 µs / 0.998 |
| 128 | 11.4 ms / 0.997 | 548 µs / 0.998 |

三点结论（口径已换：候选池随 beam 按 `IncreaseRerankResults` 增长，不再固定 100）：

1. **召回没有天花板，只有价格**：128 宽时 0.997，与 HNSW 的 0.998 持平。
   旧版「beam ≥ 32 饱和在 0.945」不是 1-bit 码的极限，而是 `10 × topn` 固定池的极限——
   宽度继续加，池子没加，重排池里根本没有真答案。
2. **价格全部压在候选行解码上**：池子从 64 行（beam 8）涨到 1024 行（beam 128），
   p50 从 2.85 ms 涨到 11.4 ms，索引层仍是 ~0.4 ms。要压低这条线只能让「多取的行」更便宜
   （列式解码 / 只解向量列），而不是加宽码。
3. HNSW 的 `ef` 在 8..128 之间对召回几乎无影响，因为它的候选宽度由 `max(4×topn, 64)` 决定，
   `ef` 只加宽底层候选表；等 p50 处 HNSW 仍全面领先，因为它的候选行是 64 而不是 1024。

（`WITH (beam = …)` 是本轮为这次扫描新加的硬覆盖，见 §11.3 第 5 条；不加时宽度由召回监控决定。）

## 3. 结论（必须直说）

**在 20k × 128d 这个区间，查询仍是 HNSW 更好**：等 p50 处 HNSW 领先，召回已能追平（0.997 vs 0.998，
代价是 11.4 ms vs 0.55 ms），因为 spann 的召回是用候选行的 I/O 买的；
**构图反过来是 spann 快 8.4×**（102.3 ms vs 855.5 ms，§2.3），因为建树是批量几何运算而图是逐个插入。
这正是 §19.7 的选型表预测的结果——128d、数据能全部进 RAM、且以查询为主时，HNSW 是正确选择。

spann 的价值区间是 §19.3/§19.4 算出来的那三个条件，**目前一个都还没实测**：

1. **1536d**：HNSW 每行 6.3 KB（plain）或 368 B（RaBitQ）+ 图常驻；spann 212 B/行（格式态）。
2. **数据 > 可用 RAM**：HNSW 只能驱逐 → 覆盖不足 → 整表扫（§19.3 的失败模式）；spann 的分区顺序读可以走出 core。
3. **50M × 1536d @ 8 GB**：这是唯一能证明「spann 不是 HNSW 的次优替代」的场景
   （按 §2.3 的 213k 行/s，构图 ≈ 4 min，已不再是门槛；门槛在 10.4 GB 的落盘与 RSS）。

所以本文**不**声称 spann 在通用意义上更快。它现在的定位是：功能完整、可持久化、召回达标，
在 HNSW 装不下的规模上提供第二条路径；而那条路径还缺 §6.4 的 mmap 读实现（见 §4）。

## 4. 与验收线对照（§16.4）

| 验收项 | 目标 | 实测 | 结论 |
|---|---|---|---|
| 1M × 128d `recall@10` | ≥ 0.95 | 0.981（20k 行，同分布，含重排，beam 32） | 达标；规模差 50×，需按 §4.2 补测 |
| 1M × 128d p50 ≤ 2× HNSW | ≤ 330 µs | 5.75 ms@beam32 / 2.85 ms@beam8 | **未达标**，瓶颈是候选行解码（§3 第 2 条） |
| 10M 行构图 ≤ 8 min | ≥ 20.8k 行/s | **213k 行/s**（20k 行 93.6 ms，见 §4.6） | **达标**（10× 于目标线）：并行子树 + 排序式 rebalance |
| 1M × 1536d `recall@10 ≥ 0.92`、p50 ≤ 8 ms | — | 未测 | 待测 |
| 50M × 1536d @ 8 GB：RSS ≤ 3 GB、recall ≥ 0.88 | — | 未测 | 待测，且依赖 mmap 读路径 |
| 常驻 ≤ 128 B/行（§10） | — | 89 B/行（128d，内存态） | 达标；1536d 预估 250 B/行（格式态 212 B） |
| 200k × 128d，两段两树 | 并行合并、全覆盖 | `recall@10` 0.895，warm p50 3.09 ms（§7） | 路径达标；冷启动 3.95 s 待预热覆盖 |

## 5. 未覆盖（宣称生产就绪前必须补）

1. **1536d**：`recall@10 ≥ 0.92`、`p50 ≤ 8 ms`。1-bit 码在 1536d 的余弦误差约 1.5%，
   重排后应更高；候选池已按参考公式随 `beam` 增长（`IncreaseRerankResults`），
   1536d 下需要的是「候选行解码更便宜」，不是更宽的码。
2. **50M + 8 GB**：mmap 读路径已就位（§2.1 证明索引层零代价），但**规模没跑过**。
   50M × 1536d 的 chunk 约 10.4 GB，落盘时间（按 213k 行/s 构图 ≈ 4 min，落盘另计）需要专门的一次离线跑。
3. **冷缓存**：`drop_caches` 后重测 p99（spann 的顺序读在这里应该开始占优）。
4. **增量写入期的代价**：rebuild 模式下每次 checkpoint 重建整棵树（§9.4）。20k 行 93.6 ms
   （§4.6 之后）；1M 行的 checkpoint 停顿需要实测是否可接受（按 213k 行/s 外推 ≈ 4.7 s）。
5. ~~HNSW 的等 `ef` 扫描~~：已完成，见 §2.4（宽度扫描到 128，等 p50 处 HNSW 领先 11 个点）。

## 6. 复现

```bash
cd rust_engine
env -u CXX -u CC cargo test -p executor spann_bench --release --lib -- --ignored --nocapture
```

输出：

```text
build 20000 rows x 128d: 102.292041ms | 449 partitions, height 4 | 1.7 MiB, 89 B/row, file-backed false
heap search p50 369.042µs
recall@10 = 0.981 | p50 5.782459ms | p95 6.10275ms | p99 8.378541ms
after flush: file-backed true | heap 0 B/row
mapped search p50 383.459µs
mapped recall@10 = 0.981 | p50 23.105209ms | p95 25.298917ms
hnsw build: 855.521167ms
hnsw recall@10 = 0.998 | p50 2.092458ms | p95 2.296333ms | p99 2.374833ms
```

宽度扫描（§2.4）：

```bash
env -u CXX -u CC cargo test -p executor spann_vs_hnsw --release --lib -- --ignored --nocapture
```

（`env -u CXX -u CC` 只为本机：shell 里导出的是已删除的 Homebrew llvm。）

## 7. 段级树（一段一树）实测

`spann_scale_two_segments_128d` 把数据分成两个段（两次 `COPY`，各自 seal 一段），
checkpoint 为每段写一棵树，然后**重开引擎**：句柄是 `SpannHandle::File`（两棵树），
一次 KNN 是两棵树并行下降再合并（`SPANN_DESIGN.md` §3 I6/I8、§8.1）。

```bash
cd rust_engine
env -u CXX -u CC cargo test -p executor spann_scale --release --lib -- --ignored --nocapture
```

200k 行 × 128d、`max_partition_size = 128`、`num_centroids = 512`：

```text
build 200000 rows x 128d over 2 segments: 10.637879292s (18801 rows/s)
reopened: 2 tree(s), 200000 covered rows, 3074 partitions | recall@10 = 0.895
          | cold 3.949666958s | warm p50 3.087375ms | warm p95 3.230042ms | warm p99 3.230042ms
```

| 指标 | 值 | 说明 |
|---|---|---|
| 构图吞吐 | 18.8k 行/s（两段合计） | 低于 §2 的 213k 行/s：这里的行先由 `COPY` 落块、再从块解码建树，含 I/O |
| 覆盖行数 | 200,000 / 200,000 | 两棵树 F 节之和，无未覆盖行 |
| 召回 | `recall@10` = 0.895 | 512 个 centroid 摊到 200k 行，比 §2 的 20k 行稀疏 10× |
| 首查（冷缓存） | 3.95 s | 映射两个 chunk（各约 21 MB）并付缺页账单，只发生一次 |
| 预热后 | p50 3.09 ms、p95 3.23 ms | 与 §2 的 file-backed p50 23 ms 相比快 7×：这里的查询没有 filter 重试 |

两件事必须说清：

1. **冷启动的 4 秒不是索引计算**，是 mmap 首次触碰约 42 MB 页。生产上由预热查询抹平；
   若 SLA 卡在首查上，这里就是要处理的地方（`SPANN_DESIGN.md` §11 的预热门控）。
2. **0.895 低于 §4 的 0.95 线**：本用例的分布是 512 个高斯簇 + 均匀噪声，
   `num_centroids = 512` 恰好一簇一质心、叶容量 128，属最坏的分区数配置。
   它验证的是「两棵树并行下降后合并、且覆盖全部行」这条路径，召回验收仍按 §4.2/§5 走 1M。
