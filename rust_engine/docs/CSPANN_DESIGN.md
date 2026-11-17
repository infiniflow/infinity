# CSPANN 索引设计（Rust 引擎）

> 状态：设计冻结待评审 ｜ 目标索引名：`cspann`（SQL: `USING CSPANN`）
>
> 参考实现：
>
> - CockroachDB `pkg/sql/vecindex/cspann`（算法权威）
> - `origin/spfresh` 分支的 C++ 实现（`src/storage/knn_index/knn_spfresh/*`，工程化简化版）
> - 本文 §2 明确列出与两者的差异，**未列出的部分不得自行简化**
>
> 相关文档：[`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md)（C++ 格式冻结）、[`PORTING.md`](PORTING.md)（移植纲领）、[`INDEX_PERF.md`](INDEX_PERF.md)

---

## 0. 结论摘要

1. **索引逻辑独立、基础设施共享**：一个索引一棵 K-means 树 + 自己的一组不可变 chunk；catalog / mmap 地址空间 / 元数据缓存 / 任务池 / 预算全局共享。
2. **封存 chunk 不可变**，运行时不改树；再平衡只发生在 build 与 flush 两个时点。
3. **未 flush 数据可搜索**：靠 `PendingTail`（各索引类型形态不同：HNSW=内存图、CSPANN=量化码、Fulltext=posting、Secondary=RCU pending），其生命周期严格绑定 memtable epoch，flush 即消亡并与 chunk 同源发布。
4. **out-of-core**：chunk 内存布局列式 + 4 KB 对齐 slot + mmap 随机访问；DRAM 只驻留 hot meta + 按需的 cold meta LRU。
5. **召回可证**：RaBitQ `(dist, errorBound)` 误差界早停 + 分区半径下界剪枝 + 在线 `RecallMonitor` + CI 曲线门禁。
6. **与 HNSW 是互补而非替代**：dim ≤ 256 且索引放得进 RAM 时用 HNSW 更优；高维（≥512d）或数据 > RAM 才用 CSPANN。定量对比见 §19（占用公式、常驻内存账、单查询成本、基准协议、选型表）。

**已确认的 6 条决策**（不得在实现中推翻）：

| #  | 决策                                                                 |
| -- | -------------------------------------------------------------------- |
| D1 | rerank 走"索引只给候选 row id，执行层精确重打分"，索引不存全精度向量 |
| D2 | 封存 chunk 完全不改（运行时不 split/merge 封存分区）                 |
| D3 | `num_centroids` 自适应（由目标填充率推导），不是硬参数             |
| D4 | 第一版即实现多级树（level/height 与 root split 增层）                |
| D5 | 验收必须覆盖 1536d / cosine                                          |

**D6**：带 filter 的向量查询**必须仍然走索引**——filter 位图下沉进索引搜索，不得退化为全表精确扫描。
参考实现即如此（§12.5 有逐行证据）；Rust 当前 `has_filter → return None` 是移植缺口，按 §12.5 修。

---

## 1. 目标与非目标

### 1.1 目标

- 与 HNSW **查询能力完全等价**：`l2/ip/cosine`、`topn`、多 chunk 合并、`WITH` 选项、`USING INDEX`、多向量列、**带 filter 时仍走索引**（位图下沉，§12.5）。
- **远超内存规模**：数据量 ≥ 10× RAM 时仍能服务，常驻内存与数据量解耦。
- 多租户可扩展：10 万租户 / 100 万索引时，索引侧常驻内存不随索引数线性膨胀到不可接受。
- 召回可度量、可回归：纳入 CI 门禁，而非依赖经验值。

### 1.2 非目标（第一版）

- 原地增量再平衡封存分区（D2）。
- 多租户共享一棵树（Cockroach `TreeKey`/森林）——我们没有分区表索引前缀概念。
- 索引内存储全精度向量（D1），仅保留 debug 开关。
- 跨节点分布式索引。

---

## 2. 与参考实现的差异（必读）

### 2.1 相对 Cockroach C-SPANN 的裁剪

| # | Cockroach 有                                                                                                     | 本设计                                                       | 影响                                                                                 |
| - | ---------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------------------------------ |
| 1 | per-partition 事务 + metadata CAS（`vecstore/store.go:363-384`）+ 13 态状态机，任意 worker 可接管 stalled 操作 | 单写者（`Arc<Mutex<mem>>`）+ 不可变 chunk，无 CAS / 无接管 | 并发度降级，正确性不降级（见 §9.4）                                                 |
| 2 | insert 直接进树、当场可 split                                                                                    | insert 进 `PendingTail`，flush 时建树                      | 树是"滞后视图"，查询需同时扫 tail                                                    |
| 3 | `IndexStats` 全局 + `MergeStats` 合并                                                                        | 仅 chunk 内 per-level 统计（CV），默认关闭自适应             | 与 Cockroach 生产等价（其 `vecstore/store.go:253-261` 的 `MergeStats` 是 no-op） |
| 4 | pacer 令牌桶（`pacer.go:20`）控制 QPS 让 fixup 队列稳定                                                        | `PendingTail` 字节水位 + AIMD                              | 更贴近内存瓶颈                                                                       |
| 5 | 逐向量删除 + dangling + `vectorDeleteFixup`                                                                    | 引擎 row tombstone + 执行层过滤 + `optimize` 回收          | 空间回收延迟（与 HNSW 同）                                                           |
| 6 | `workspace.T` 栈式分配器                                                                                       | 复用 `Vec`/arena，需自行控分配                             | 高频小查询需 benchmark                                                               |
| 7 | store 层 batch Scan 并行读分区                                                                                   | reader 自建 in-flight 上限 + `madvise(WILLNEED)`           | 需实现与调参                                                                         |
| 8 | `MaxInsertAttempts`/`StalledOpTimeout`/`QualitySamples` 等"尝试-接管"参数                                  | 无对应概念                                                   | 是 #1 的后果                                                                         |

### 2.2 相对 spfresh C++（`origin/spfresh`）的继承与修正

**继承**（已被验证可行，直接照做）：`BuildSubtree` 递归多层（`knn_spfresh_index_impl.cpp:761-766`）、root split 时 `height_+1`（`:1642`）、`TreeSnapshot` RCU、mmap 下 centroid 也从映射区读（`knn_spfresh_index.cppm` 的 `mmap_centroid_base_`）、`LoadFromMmap`（`:2276`）、`Save`/`Load`（`:2159`/`:2199`）、`SPFreshVectorFetcher`（re-encode 取原向量或退化为解码）、`AdaptBeamSize`（`:968`）。

**修正**（spfresh 的简化点，本设计不继承）：

| # | spfresh 的做法                                                                               | 本设计                                              | 理由                                          |
| - | -------------------------------------------------------------------------------------------- | --------------------------------------------------- | --------------------------------------------- |
| 1 | 4 态状态机 `{Ready,Splitting,Merging,Deleting}`                                            | chunk 级提交 + tail 消亡即恢复                      | 不需要"接管"，恢复粒度从分区改为 chunk        |
| 2 | delta 双缓冲 + `Compact()` 累积（`max_delta_mb` 默认 512 MB）                            | `PendingTail` 生命周期 = memtable epoch           | 去掉累积型 L1 的记账/双缓冲/gap 回填          |
| 3 | `min_partition_size = max/8`、merge 靠 `EnqueueMergeSweep` 轮询                          | merge 仅在 build/flush 时按阈值判定                 | 无运行时再平衡（D2）                          |
| 4 | DDL 参数沿用 IVF：`num_centroids` 默认 1000 固定、`bucket_size_limit`、`replica_count` | 见 §15：`num_centroids` 自适应、参数面向 C-SPANN | 固定分区数在数据量变化时掉 recall             |
| 5 | 文件头 `#pragma pack(1)`，无 CRC                                                           | 显式 header + 分节 CRC32C                           | 损坏检测                                      |
| 6 | slot 内 meta 与 code 混排（`QuantizedVectorMeta` 前置 12 B）                               | **列式** slot（见 §6.3）                     | 扫描只读所需列，SIMD 友好                     |
| 7 | `adaptive_beam_` 默认 true（本地统计）                                                     | 默认关闭，`WITH (adaptive_beam=true)` 开启        | 无全局统计支撑时抖动（同 Cockroach 生产选择） |

### 2.3 相对 HNSW 的不对称（为什么 CSPANN 的 tail 不建树）

|          | HNSW                                                              | CSPANN                                                               |
| -------- | ----------------------------------------------------------------- | -------------------------------------------------------------------- |
| 插入一行 | 图插入 `O(log n·M)`（`crates/index/src/vector.rs:1273 add`） | 需定位 leaf（读分区）→ 满则 split（k-means + 重编码，O(分区×dim)） |
| 结构变更 | 只改邻居链表                                                      | parent child 列表、level、centroid                                   |
| 结论     | **内存段用图**（现状 `hnsw_mem.rs:11`，不改）             | **内存段用量化码**（无树），flush 时建树                       |

上表只说明"尾段"的不对称；两者的**存储形态、常驻内存与查询成本**的定量对比见 §19（1536d 下每行
368 B vs 208 B、8 GiB 预算装得下多少行、单查询读多少字节、基准怎么测）。

---

## 3. 术语与不变式

- `PartitionKey = u32`，`0 = root`，非 0 递增且**永不复用**。
- `Level = u32`，`1 = leaf`，向上递增；`root.level == height`。
- **I1**：分区 centroid 创建后永不改变，区内向量都相对它编码。
- **I2**：每个向量只属于一个 leaf 分区（无副本）。
- **I3**：只有 root split 才 `height += 1`；merge 不降 level（树只长高）。
- **I4**：一个分区 = 一段连续 slot，一次顺序读可取全分区。
- **I5**：chunk 文件自包含（header 内含 dim/metric/seed/code_width/分节布局/CRC），可独立搬迁与校验。

---

## 4. 架构与 crate 布局

```
crates/index/src/cspann/          纯计算，无 I/O（可在内存 store 上跑全部算法单测）
  mod.rs        类型：PartitionKey/Level/State/PartitionMeta/Options/Error
  quantize.rs   trait Quantizer {RaBitQ, None}
  rot.rs        RandomOrtho {None, Givens, Matrix}（只持久化 seed+algo+dims）
  kmeans.rs     BalancedKMeans(k=2) + kmeans++（对照 cspann/kmeans.go）
  partition.rs  Partition（列式向量集）+ 分区内 Search
  search.rs     Searcher / SearchSet / MaybeCloser / 剪枝 / 早停
  stats.rs      per-level CV 统计（自适应 beam，默认关）
  store.rs      trait CspannStore / CspannMutStore（§7）

crates/storage/src/knn_index/cspann/   磁盘格式与 Store 实现
  format.rs     header / 分节 / slot 布局 / CRC / 版本
  writer.rs     流式构建 + 原子发布（tmp → fsync → rename → catalog commit）
  reader.rs     mmap 只读视图、分区零拷贝 view、WILLNEED 预取
  store.rs      CspannStore 的 chunk 实现

crates/executor/src/cspann_mem.rs      CspannIndexInMem（PendingTail 实现，对照 hnsw_mem.rs）
crates/executor/src/blocks/cspann_chunk.rs  chunk 编解码（对照 index_chunk.rs:196）
```

分层原则：`crates/index/src/cspann` **只依赖 trait**，因此可以在纯内存 `Vec` store 上跑 conformance 测试（对照 Cockroach `memstore`）。

---

## 5. 数据模型

```rust
pub struct PartitionMeta {
    pub level: u32,
    pub state: PartitionState,   // Ready 为常态；其余仅供崩溃恢复识别
    pub centroid_off: u32,       // Sec B 偏移
    pub slot_off: u64,           // Sec C 偏移（4 KB 对齐）
    pub count: u32,
    pub flags: u32,              // bit0 tombstone, bit1 overflow
    pub version: u64,            // chunk 内单调版本，用于调试与断言
    pub radius: f32,             // R_partition = max‖x-c‖，用于下界剪枝（§8.3）
}
```

- root 分区不量化（`UnQuantizer`），其余用 `RaBitQ`；`encode = none` 时全树不量化（用于 dim≤128 或回归对照）。
- `PartitionState` 只保留 `Ready/Splitting/Merging/Deleting` 四态，且**只在本 chunk 构建过程中**有意义；封存 chunk 落盘后应全为 `Ready`。

---

## 6. 磁盘格式（`chunk_{n}.cspan`）

### 6.1 分节

```
[Header 128 B] magic="CSPA" ver=1 dim metric code_width
               partition_count partition_capacity num_vectors root_key height
               rot_algo seed  section_off[6] section_len[6] crc32c(header)
[Sec A] PartitionTable : PartitionMeta[cap]                48 B/entry
[Sec B] Centroids      : f32[cap][dim]                     64 B 对齐
[Sec C] Slots          : 每分区 4 KB 对齐，列式（见 6.3）
[Sec D] Children       : CSR(offset[] + keys[])，仅 branch/root
[Sec E] (预留) FullVectors : 默认空；debug 开关才写
[Sec F] Footer         : crc32c(Sec A..E) + 版本 + 每 slot 可选校验和
```

### 6.2 分区 slot（Sec C，列式）

```
row_ids   u32[count]
cent_dist f32[count]        ‖o - c‖
qdot      f32[count]        1/⟨ō,o⟩（倒数，避免除法与除零）
code_cnt  u32[count]        popcount(codes)，估算 term2 用
codes     u64[count][ceil(dim/64)]
```

列式的意义：扫描时只读需要的列；`code_cnt` 独立成列使 term2 无需解 code；按 dim 顺序拼接的 code 可用 popcount 向量化。

> 1536d：`code_width = 24`，每向量 = 4+4+4+4+192 = **208 B**；1 亿行 ≈ 20.8 GB（不进 DRAM）。

### 6.3 原子发布

1. 写 `chunk_{n}.cspan.tmp`（流式，边写边算 CRC）→ `fsync`；
2. `rename` 成正式名；
3. 在 catalog 写 `idx_chunk|{db}|{tbl}|{idx}|{seg}|{chunk_id} = commit_ts` 与 `chunk_info{base_name,base_row_id,row_count,bytes,crc,commit_ts}`（沿用 `crates/catalog/src/layout.rs:2709-2757` 的键格式）；
4. 发布 `Arc<LoadedCspannIndex>` 替换 tail 覆盖范围。

两个 worker 同时写同一 range：先完成第 3 步者赢，后者删除自己的文件（chunk 级 CAS，替代 Cockroach 的 partition 级 CAS）。

### 6.4 out-of-core

- Sec A/B（≈ `cap*(48 + 4*dim)`）走 **cold meta LRU**（默认 64 MB），hot meta（dim/metric/root/count/seed）常驻 registry（≈ 64 B/索引）。
- Sec C 通过全局 `MmapArena` 映射（固定 VMA 数），只触碰被探测分区的页；`madvise(WILLNEED)` 预取下一批 beam 分区，`MADV_RANDOM` 关闭自动预读。
- 整 chunk 可被预算驱逐（drop 映射），重载即解码，无跨 chunk 依赖。

---

## 7. Store trait

```rust
pub trait CspannStore: Send + Sync {
    fn hot_meta(&self) -> &HotMeta;                                   // dim/metric/seed/root/count
    fn get_metadata(&self, key: PartitionKey) -> Result<PartitionMeta>;
    fn get_partition(&self, key: PartitionKey) -> Result<PartitionView<'_>>; // 零拷贝（mmap）
    fn search_partitions(&self, keys: &[PartitionKey], q: &QueryVector,
                         out: &mut SearchSet, opts: &SearchOpts) -> Result<()>;
    fn get_full_vectors(&self, rows: &[u64], out: &mut [f32]) -> Result<()>;  // 仅 debug/校验用
    fn covered_ranges(&self) -> &[Range<u64>];
}

pub trait CspannMutStore: CspannStore {
    fn create_partition(&mut self, meta: PartitionMeta) -> Result<PartitionKey>;
    fn add_to_partition(&mut self, key: PartitionKey, rows: &[u64], codes: &[Quantized]) -> Result<()>;
    fn remove_from_partition(&mut self, key: PartitionKey, rows: &[u64]) -> Result<()>;
    fn clear_partition(&mut self, key: PartitionKey) -> Result<()>;
    fn delete_partition(&mut self, key: PartitionKey) -> Result<()>;
}
```

`crates/index/src/cspann` 的算法只依赖这两个 trait；`crates/storage/.../cspann` 提供 file 实现，测试提供 mem 实现。

---

## 8. 查询路径

### 8.1 契约（与 HNSW 一致）

```rust
search_with_ef(q, k, ef: Option<usize>) -> Vec<(row_id, dist)>
search_candidates(q, ef) -> Vec<(row_id, dist)>       // 不截断
rows() -> &[u64]; coverable() -> bool                  // 覆盖度自证
```

- 索引只返回候选 `(row_id, dist)`；列值由执行层 `read_blocks_candidates_values` 回表；
- 最终 `#_distance/#_similarity` 由执行层用 f64 精确重算（`knn_scan.rs:476`、`statements.rs:9736-9739`）；
- 多 chunk（含 tail）各自搜索后 `MergeKnn`（`crates/index/src/merge_knn.rs`）合并去重。

### 8.2 分层 beam 下降

```
searcher.search_root(q):
  root = store.get_metadata(RootKey)      // root 不量化，精确 centroid 距离
  for level in (root.level-1)..=leaf:      // 每层一个 levelSearcher
    candidates = parent_searchset.pop(beam)
    store.search_partitions(candidates, q, leaf_searchset)
  return leaf_searchset
```

- `beam` 来源优先级：`WITH (nprobe|ef = N)` > `recall_target`/`latency_budget_ms` 反查 > 默认 `base_beam = 8`；
- 高层 beam 收窄：`beam_level = max(1, beam >> (level-1))`（分区数指数级减少）。

### 8.3 误差界早停（无损）

```
堆内第 k 名最坏情况: d_k+ = dist_k + err_k
下一个候选分区最好情况: lb = dist_next - err_next
若 d_k+ <= lb  ⇒  剩余分区不可能产生更好的真实 top-k，停止
```

- `err` 来自 RaBitQ 估计（Cockroach `quantize/rabitq.go:277-293` 的 4 项式 + 误差界）；
- 分区半径下界：`lb_part(p) = max(0, ‖q - c_p‖ - R_p)`，`R_p` 在 header 中（§5）；
- **单位**：`lb_part` 是距离，而 L2 的分数是**平方**距离，所以判据是 `lb² ≤ d_k+`
  （`search::l2_can_beat`）。直接比 `lb ≤ d_k+` 会剪掉 `lb ∈ (d_k, √d_k)` 的分区——
  归一化向量下 `d² < d < 1` 是常态，这条带几乎覆盖全部候选，实测就是 cosine 索引掉召回、
  L2 fixture 却看不出来的原因（`tight_cluster_reproduces_the_exact_top_k` 守这条）；
- `nprobe` 仅作上限兜底（默认 `2 * base_beam`），防止退化为全扫。

### 8.4 SearchSet 与剪枝

- 排序键 `(distance, errorBound, childKey)`（Cockroach `search_set.go:63-93`）；
- **攒满阈值才 prune**：`pruning_threshold = max(1024, (k + extra) * 2)`（`search_set.go:389-477`）；
- `MaybeCloser(r2) = dist - err <= r2.dist + r2.err`（`search_set.go:54-56`）；
- 去重：row_id 用 `HashSet`/Roaring，分区键用 `HashSet<PartitionKey>`；`SearchSet::clear()` 必须同时清 deDuper（否则跨批漏结果）。

### 8.5 metric 语义

| metric | centroid                 | 估计式                                                               | 对外 dist            |
| ------ | ------------------------ | -------------------------------------------------------------------- | -------------------- |
| l2     | mean                     | `‖o-c‖² + ‖q-c‖² - 2‖o-c‖‖q-c‖⟨ō,q̄⟩`                | 平方距离（越小越好） |
| cosine | spherical（`‖c‖=1`） | IP 式 + clamp 到 [0,2]，error bound 同步调整                         | 相似度（越大越好）   |
| ip     | spherical centroid       | `⟨o,q⟩ = ‖o-c‖‖q-c‖⟨ō,q̄⟩ + ⟨o,c⟩ + ⟨q,c⟩ - ‖c‖²` | 相似度（越大越好）   |

---

## 9. 写入与维护模型（所有索引统一）

### 9.1 分层

```
INSERT  → WAL + pending_rows（引擎已有，与索引无关）
            │
            ▼
   PendingTail（per index，生命周期 = memtable epoch）
            │  字节水位触发 handoff
            ▼
   FLUSH → 写数据 block ＋ 由 tail 同源生成 sealed chunk（同一 commit 发布）
            │
            ▼
   Maintenance（全局任务池：compaction / optimize / rebuild）
```

### 9.2 `PendingTail` 抽象（各索引形态不同）

```rust
pub trait PendingTail: Send + Sync {
    fn insert(&mut self, rows: &RowBatch) -> Result<()>;
    fn search(&self, q: &Query, k: usize, ef: Option<usize>) -> Vec<(u64, f32)>;
    fn bytes(&self) -> u64;
    fn take_for_flush(&mut self) -> FlushHandoff;   // 原子抽干，交给 flush
}
```

| 索引类型   | tail 形态                                                                         | 查询成本（10k 行 @1536d）             | 说明                                     |
| ---------- | --------------------------------------------------------------------------------- | ------------------------------------- | ---------------------------------------- |
| HNSW       | **内存图**（现状 `hnsw_mem.rs:11`，`insert` → `vector.rs:1273 add`） | 几十 µs                              | **不改**：增量图插入 + dump 序列化 |
| CSPANN     | **量化码 tail**（单质心或少量质心）                                         | 2 MB 顺序读 + popcount ≈ 0.2–0.4 ms | 不维护增量树（§2.3）                    |
| Fulltext   | **posting tail**（现状 `state.rs:4912` `MemoryIndexer`）                | µs                                   | **不改**                           |
| Sparse/BMP | 增量 posting                                                                      | µs                                   | 保持现状                                 |
| Secondary  | RCU pending map（`crates/index/src/lib.rs:111-128`）                            | O(log n)                              | 保持现状                                 |

**关键约束**：tail 容量上限 = memtable 上限（不设独立 `memindex_quota` 触发点），tail 不跨 flush 存活。

### 9.3 flush 与 tail 同源

- flush 直接消费 `take_for_flush()`（HNSW 序列化内存图；CSPANN 用码建树；Fulltext dump `MemoryIndexer`），**不重新读行**；
- 因此删除以下既有状态与流程：`recover_mem_index` 的 gap 回填（`index_build.rs:255`）、`index_has_memory_tail`（`:754`）、`hnsw_probed/hnsw_topup`（`state.rs:4898/4902`）、`index_pending_bytes`（`:4894`）；
- 崩溃语义：数据未 flush 则未持久化，tail 一起丢失，重启从 WAL 重放行 → 索引侧无 redo/undo。

### 9.4 chunk 级提交（替代 partition 级 CAS）

- 提交点 = §6.3 第 3 步；同一 range 只允许一个赢家；
- 恢复：`read_index_chunks` + `merge_covered_ranges` 求 gap，只把 gap 行重新进 tail；
- 无"接管 stalled fixup"概念（因为封存 chunk 不可改）。

### 9.5 背压水位（字节为主轴）

> **已实施**（2026-11-17）：`crates/executor/src/engine/maintain.rs` 的 `Watermarks`
> （soft = quota/4 ≤ 64 MiB、hard = quota/2 ≤ 256 MiB、AIMD 在 >80% 预算时对半下调、
> <50% 恢复）与 `Pressure`；判定由 `Engine::seal_pressure()` 一处给出，两条配额路径
> （写入触发与显式检查）都读它。**降级语义有一处偏差**：本引擎的 dump 池是多 worker
> （`BGTaskProcessor`），不需要阻塞写线程，所以 `hard` 的做法是**把该表的 seal 从
> 「最大一个」升级为「全部」**（`dump_every_table_index`），而不是同步执行。

| 水位                                                    | 触发     | 动作                                                                        |
| ------------------------------------------------------- | -------- | --------------------------------------------------------------------------- |
| `tail_bytes > soft`（`min(64 MB, per_db_quota/4)`） | 高优先级 | seal 任务插队；同租户新任务降权                                             |
| `tail_bytes > hard`（256 MB 或配额 50%）              | 内存压力 | 写线程降级执行 seal（degrade-to-sync）                                      |
| `global_pending > 80% 预算`                           | 全局紧张 | soft 水位下调 50%，对最老 tail 强制 seal（AIMD：>目标×0.8 / <目标/2 调节） |
| seal 等待 > 2 s                                         | 饥饿     | 提升该租户权重，记 `index_seal_starvation`                                |

**软约束退化，不返回错误**；只有 hard 水位 + 未完成 WAL fsync 才短暂等待（有超时）。

### 9.6 公平调度

> **已实施**（按本引擎只有一种维护任务「seal」裁剪）：`maintain.rs` 的 `SealScheduler`，
> 按**数据库**的 pending 字节做加权公平队列（WFQ：每轮按权重记账、赢家扣整轮），
> 每租户 in-flight ≤ `max_inflight`，超过 `SEAL_STARVE_TICKS`（1000 个 commit-ts tick）
> 没被 dump 的尾会插队并计入 `index_seal_starvation`（`Engine::index_seal_starvation()`）。
> 选受害者从「最大者胜」换成它（`index_build.rs::pick_seal_candidate`）。
> 没有单独的任务队列：dump 任务的排队/合并仍由 `MemIndexPending` + `BGTaskProcessor` 负责。

- 全局唯一 `MaintainQueue`：`heads: DashMap<DbId, SegQueue<Task>>` + 权重（由 pending_bytes 归一化）+ 单租户 in-flight ≤ 2 + 全局 worker ≤ `min(cores, 8)`；
- 任务**可切片让出**：compaction/rebuild 每 N 行或 1 个分区批次检查公平/预算/deadline，保存进度续跑；
- 去重键 `(index_id, kind, row_range)`。

### 9.7 覆盖度契约（区间级回退，改动 HNSW/IVF 也生效）

现状：`all_covered` 失败 → `return None` → 上层整表精确扫（`knn_scan.rs:87-98`、`:239-242`）。
改为：

```
candidates = ⋃(每个 sealed chunk 的索引候选)
           ∪ (uncovered 行区间上的精确扫描)         // uncovered = [0,max_row) \ covered_ranges
           ∪ (tail 候选)
```

- `covered_ranges` 来自 catalog 的 chunk 记录，`merge_covered_ranges` 现成可用；
- 代价与"未覆盖字节"成正比，而非整表；这是 tail 容量 = memtable 能成立的前提；
- 对 HNSW/IVF 同样是性能改善（当前删除一行即整表扫）。

---

## 10. 多租户资源模型

| 资源                        | 粒度                                           | 说明                                              |
| --------------------------- | ---------------------------------------------- | ------------------------------------------------- |
| 树 / 分区 / centroid / seed | per index 独立                                 | 一个租户 rebuild 不影响他人                       |
| chunk 文件                  | per index 目录（bucket 化 `b{hash%1024}/`）  | 可单租户搬迁/备份                                 |
| catalog 记录                | 全局单 KV，键含 `db_id/table_id/index_id`    | 现成 prefix                                       |
| **mmap 地址空间**     | 全局 `MmapArena`（少数大文件 + offset 切片） | 避免百万文件撞 `vm.max_map_count`（默认 65530） |
| **cold meta**         | 全局 LRU（默认 64 MB），key 含 `db_id`       | 否则 100 万索引 × 31–350 KB 无法常驻            |
| **hot meta**          | 常驻 registry，≈64 B/索引                     | 打开即可服务                                      |
| **后台任务**          | 全局池（≤8 worker）+ 租户轮转                 | 不 per-index 起线程                               |
| **预算**              | 全局上限 + per-db 软配额                       | 防 noisy neighbor，超配额只淘汰自己               |

**100 万索引的常驻账**：hot meta ≈ 64 MB；cold meta LRU 固定 64 MB；VMA 固定（arena）；worker ≤ 8；索引侧 tail 内存 = Σ memtable（已有预算）。

**必须遵守的 4 条边界**：

1. chunk 自包含（catalog 只是目录，不是数据真相）；
2. 共享层皆可丢弃（缓存/队列/映射丢了重读）；
3. 锁序：任务池 → 租户队列 → 单索引锁；任何任务只持一个索引锁，不做持锁 I/O；
4. 配额是软约束（退化而非报错）。

---

## 11. 参数自适应与召回保证

### 11.1 三层参数

| 层     | 参数                                                              | 决定时机                       | 在线可改                       |
| ------ | ----------------------------------------------------------------- | ------------------------------ | ------------------------------ |
| 不变量 | `dim, metric, seed, rot_algo, code_width, max_partition_size`   | `CREATE INDEX` + 写入 header | ❌（改了码失效，只能 rebuild） |
| 构建期 | `num_centroids, kmeans_iterations, encode, height, R_partition` | 每次 build/flush 推导          | ✅ 下次生效（chunk 自包含）    |
| 查询期 | `beam/nprobe, rerank 数量, 是否用索引, 早停阈值`                | 每次查询                       | ✅                             |

### 11.2 构建期公式

```
fill          = 0.7
num_centroids = clamp(ceil(n / (fill * max_partition_size)), 64, 65536)
height        = ceil(log(num_centroids, max_partition_size)) + 1
encode        = if dim <= 128 && target_recall >= 0.99 { none } else { rabitq }
```

三条"只增不减"规则防震荡：`num_centroids` 仅在 n 翻倍时上调；树高只增；`encode` 变更必须 rebuild。

**构建期内存（已实施）**：建树只经 `VectorSource` 一次随机读一条向量
（`index/src/cspann/source.rs`），执行层先把列流式写成 `f32` scratch 文件再 mmap
（`Engine::cspann_build` → `storage::knn_index::cspann::build_from_scratch`）。
峰值因此是「树 + 一个 block」，与 `n × dims × 4` 无关：1536d 下旧实现同时持有
整列文本（~15 KB/行）和整列 `f32`（6 KB/行），1M 行约 21 GB，50M 行不可行。
代价是一次顺序写盘和 ~10% 构建时间（20k×128d：93.0 → 102.3 ms）。
**写入路径（已实施）**：`INSERT` **不碰树**——行写入 block 追加缓冲的同时，把
`(row id, vector)` 追加到该索引的 mem index（`TableEntry::cspann_mem`，O(1) 一次向量拷贝）。
查询把「各段 chunk 的合并结果」与「mem index 的行」一起当候选，mem index 的行同时算作
已覆盖，所以新行立即可查、且**不会**退化成整表精确扫描。下一次 dump 才建树：按段建
`chunk_{n}.idx`，记录写进 catalog 之后清空 mem index。逐行插入树里被否掉的原因是叶子只存
1-bit 码，一旦要 split 就得回表取原始向量，等于把 O(1) 变成随机 I/O。

序列化随即改成了流式（`storage/src/knn_index/cspann/writer.rs` 的 `write_to`，与内存版
`write` 逐字节相同，含每节增量 CRC），并按分段落盘：每个 `seg_s` 写自己的
`chunk_{n}.idx`，查询侧加载**每一段的最新 chunk** 并合并结果（`CspannHandle::File(Vec<_>)`）。
峰值因此是「单个分段的树 + 一次顺序流」：既不是整表文本，也不是整表的树，
dump 也只重建被 checkpoint 触碰的那一段。

### 11.3 召回保证

1. **误差界早停**（§8.3，无损）；
2. **分区半径下界**（§8.3）；
3. **CV 缩放只用于提速**：`beam = clamp(base * 2^(-z), base, 2*base)`，下界是 `base`，不允许用降召回换延迟；
4. **在线 `RecallMonitor`**（per index；**已实施**：`engine/recall.rs`，状态在内存不在 catalog，见 §20.2 第 9 条）：
   - 采样率默认 1%（debug 可 100%），对候选集精确 rerank 后与暴力结果比对；
   - 7 天滑窗 `est_recall < target` → `base_beam *= 1.5`（上限 +400%）并置 `rebuild_suggested`；
   - `est_recall > target + 0.03` 且 p99 超预算 → `base_beam *= 0.9`（下界 base）；
5. **预算式查询接口**（**已实施**）：`WITH (recall_target = 0.95)` / `WITH (latency_budget_ms = 5)` /
   `WITH (beam = 32)`（硬覆盖，压过监控）/ `WITH (nprobe = 32)`（IVF）。`recall_target` 与
   `latency_budget_ms` 进入该索引的 `RecallMonitor`，`beam` 只作用于本条语句。

### 11.4 其它索引复用

同一 `RecallMonitor` + 曲线门禁用于 HNSW 的 `ef` 自适应、Fulltext 的候选提前终止、Sparse 的 `nprobe`。

---

## 12. 其它索引需要的改动

### 12.1 HNSW：**架构不变**，三处改动

| 改动             | 现状                                                                                         | 目标                                |
| ---------------- | -------------------------------------------------------------------------------------------- | ----------------------------------- |
| 覆盖度回退区间化 | `all_covered` 失败 → 整表扫（`knn_scan.rs:239-242`）                                    | 只扫 uncovered ranges（§9.7）      |
| 记账统一（可选） | ~~`memindex_quota` + `mem_index_capacity` + `hnsw_pending_bytes` + `topup/probed` 四套~~ **已统一**：一套字节账 + `Watermarks` 三档 | tail 容量 = memtable 上限，一套记账 |
| **filter 下沉（回归修复）** | `has_filter` → `return None` → 整表精确扫（`statements.rs:9499`、`:9526-9531`、`:9606`） | 位图下沉进图搜索（§12.5，对齐 C++ `physical_knn_scan_impl.cpp:607-622`） |

**明确不改**：`hnsw_mem` 的增量图（`hnsw_mem.rs:47` → `vector.rs:1273`）、dump 序列化（`blocks/engine.rs:4887`）、多 chunk `MergeKnn`。

### 12.2 Fulltext：**保留 posting tail**，新增能力

| 改动                                        | 说明                                                               |
| ------------------------------------------- | ------------------------------------------------------------------ |
| 保留 `state.rs:4912` 的 `MemoryIndexer` | 未 flush 文本必须可检索                                            |
| flush 同源                                  | dump 直接消费 tail（现有 `dump_text_index` 路径），删除 gap 回填 |
| 召回监控                                    | 复用 §11.4                                                        |

### 12.3 IVF / Sparse / MV / Secondary：仅记账与回退

- IVF：覆盖度判定沿用现集合形式（`knn_scan.rs` 的 `all_covered` 显式集合分支），改为区间回退；
- Sparse/BMP、MV：接入统一记账（不含逻辑改动）——**已实施**：seal 选择器统一问
  `IndexMaintainer::pending_bytes()`（`crates/index/src/maintain.rs`），Sparse/MV/Secondary/Fulltext
  的 `tail_bytes` / `approx_bytes` / `mem_used` 都在这个接口后面；HNSW 的
  `HnswIndexInMem::maintainer()` 还顺带修掉一个漏洞（见 §20.2 第 10 条）；
- Secondary：保持 RCU pending → fold；仅纳入统一任务池。

### 12.4 统一新增的公共件

| 新增                                   | 位置                                       | 状态与作用                                                            |
| -------------------------------------- | ------------------------------------------ | --------------------------------------------------------------- |
| `IndexMaintainer` trait              | `crates/index/src/maintain.rs`           | ✅ `pending_bytes/resident_bytes/optimize_cost/can_degrade`，seal 选择器与配额都读它 |
| `Watermarks` + `Pressure`            | `crates/executor/src/engine/maintain.rs` | ✅ §9.5 的三档水位与 AIMD                                        |
| `SealScheduler`（原 `MaintainQueue`）| `crates/executor/src/engine/maintain.rs` | ✅ §9.6 的加权公平 + 每租户 in-flight + 饥饿计数。缩到一个任务种类（seal），没有独立队列 |
| `RecallMonitor`                      | `crates/executor/src/engine/recall.rs`   | ✅ 7 天窗口、1% 抽样、加宽/收窄规则                              |
| `Coverage::uncovered_ranges()`       | `crates/executor/src/knn_scan.rs`        | ✅ 实际落成 `KnnProbe::uncovered`（行号集合，比区间精确）        |
| `PendingTail` trait                  | ~~`crates/index/src/pending.rs`~~        | ⚠️ **不做**，理由见 §12.6 末：cspann 走 rebuild；HNSW 的 tail 由 dump 在原地消费 |

### 12.5 filter 下沉（**回归修复**：带 filter 时索引被绕过）

> **已实施**（2026-11-17）：下面「现状」一节记录的是修复前的行为，保留作对照；
> 落地形状、计算位置和验收用例见 §12.6。

**现状（Rust，修复前）**：只要有 filter，向量索引就被直接放弃，退化为整表精确扫：

| 环节 | 代码 | 行为 |
|---|---|---|
| 判定 | `statements.rs:9499` | `has_filter = pred \|\| in_filter \|\| search_filter`（WHERE 谓词、`IN`、`SEARCH … FILTER` 三种都算） |
| 放弃索引 | `statements.rs:9526-9531` | `if has_filter { return None; }`，注释称"the reference re-scores every row the filter admits" —— **与参考实现不符** |
| 后果 | `statements.rs:9606` | `fetch_rows_with_ids(table)` 读全表 → 全表算距离 → 排序 |

判定位置在 HNSW/IVF 选索引之前（`:9534-9544` 同时接受两种 kind），**所以两种索引一起被绕过**；
与 filter 的选择性无关（过滤后只剩 3 行也照样扫全表）。

**参考实现（C++）**：filter 被压进索引搜索，分三层。

| 层 | 做法 | 证据 |
|---|---|---|
| 逐 segment 位图 | `CommonQueryFilter` 三级构建：minmax/bloom → 二级索引/全文正排 → leftover 逐行求值；产物 `std::map<SegmentID, Bitmask> filter_result_`，`TryFinishBuild` 支持并行构建 | `src/storage/secondary_index/common_query_filter.cppm:31-115` |
| 段级剪枝 | 本段无位图 → 整段不搜；位图全 true → 无过滤搜索；否则带位图搜索（`use_bitmask = !bitmask.IsAllTrue()`） | `physical_knn_scan_impl.cpp:607-622` |
| HNSW | `SearchIndex(query, topk, BitmaskFilter<SegmentOffset>, option)`；过滤发生在 `SearchLayer` 的**入库处**，被过滤顶点**仍然被遍历**（不破坏连通性），只是不进结果堆 | `physical_knn_scan_impl.cpp:893-920`、`hnsw_alg.cppm:133-148`、`knn_filter.cppm:22-31` |
| IVF | `IVF_Filter<true>`：命中位图的 segment offset 才进结果 | `knn_ivf/ivf_index_search.cppm:80-95` |
| 暴力兜底 | 同一 bitmask 传给 `BruteForceBlockScan`（nulls 也并入位图） | `physical_knn_scan_impl.cpp:563-591` |
| 其它索引 | EMVB / SMVE 同构（`BitmaskFilter` vs `AppendFilter` 二选一） | `emvb_search_impl.cpp:407-413`、`smve_index_impl.cpp:209-215` |

**Rust 侧能力其实已经移植，缺的只是接线**：

| 环节 | 状态 | 证据 |
|---|---|---|
| 图搜索带过滤 | **已实现**（语义与参考一致） | `crates/index/src/vector.rs:1737-1761` `HnswIndex::search_filtered` |
| 底图层过滤 | **已实现**（`allow: Option<&dyn Fn(usize) -> bool>`，只影响入库不影响遍历） | `vector.rs:1567-1606` `search_layer_filtered` |
| 对照单测 | **已有**（3 组 bitmask 断言） | `vector.rs:2522-2559` |
| `HnswHandler` 封装 | **缺** filtered 重载（`search_index` 注释写明 "without a filter"） | `crates/index/src/hnsw_handler.rs:67-76` |
| 执行层 | **断线**（见上表） | `statements.rs:9526-9531` |
| `CommonQueryFilter` 等价物 | **缺**：只有单条 minmax/bloom 规则，没有 per-segment 位图聚合 | `crates/executor/src/fast_rough_filter.rs` |

**修复步骤**（与 cspann 解耦，列 P0.5 回归项）：

1. `HnswHandler::search_filtered_index(q, k, ef, allow)`：两行转发到 `HnswIndex::search_filtered`；
2. 建位图：谓词只涉及非向量列时用 `columnar_filter::predicate_mask` **按列**求值（不读向量列）；能走二级索引/粗过滤的先缩段；
3. `knn_scan.rs`：`KnnCandidateScan` 增 `allowed: Option<&Bitmask>`，tail 与每个 sealed chunk 的搜索都透传（chunk 内部顶点下标经 labels 换成 row_id 再问位图）；
4. `statements.rs`：删掉 `if has_filter { return None; }`，`has_filter` 从"禁用条件"改为"是否建位图"；
5. 低选择性兜底：`ef' = clamp(ef / max(sel, 1e-3), ef, ef_cap)` —— 参考实现没有这一步，1% 选择性下它会凑不满 k，我们补上。

**验收**：

- SLT：`WHERE` + `MATCH VECTOR` 在"过滤后行数 ≥ k"时必须与精确扫结果**行集相同**；
- `EXPLAIN` 显示走索引；带 filter 的查询 `candidates_per_query` 远小于表行数（指标已在 §16.4 监控清单）；
- 选择性 1% 时 recall@10 ≥ 无过滤基线的 95%，且 p50 不随表行数线性增长（对比现在的整表扫）。

**cspann 的对应做法（更便宜，P3 接同一套 `allowed`）**：slot 本来就有 `row_ids u32[count]` 列（§6.2），
位图求交可以放进读取 slot 的同一次扫描；分区级先判"命中数 = 0"则整块 4 KB 都不读。
HNSW 做不到这一点——它的顶点里没有 row_id，只能靠尾部 labels 数组反查。

### 12.6 实施状态（2026-11-17 核实）

§12 的改动按「是否已落地」重新核对了一遍，结论如下（**没有只改文档不改代码**：下表每一行都能在代码里查到）：

| 索引 | §12 的要求 | 状态 | 核查点 |
|---|---|---|---|
| HNSW | 架构不变 | ✅ 无改动 | 图、dump、`MergeKnn` 均未动 |
| HNSW | 覆盖度回退**区间化** | ✅ 已做 | `knn_candidates` 返回 `KnnProbe { candidates, uncovered }`；`uncovered` 由 `Engine::live_row_ids`（block 版本走一遍，不解码）减去 labels 得到 |
| HNSW | 记账统一 | ✅ 已做 | 一套字节账 + `Watermarks`（`engine/maintain.rs`）；行上限 `mem_index_capacity` 只约束单个索引，不再决定何时 dump |
| HNSW | **filter 下沉（D6 回归修复）** | ✅ 已做 | `statements.rs` 的 `if has_filter { return None; }` 已删除；掩码经 `HnswHandler::search_index_filtered` → `HnswIndex::search_filtered` 进图，tail 走 `HnswIndexInMem::search_filtered_with_ef` |
| Fulltext | 保留 `MemoryIndexer` | ✅ 无需改动 | `state.rs` 的 `text` 表未动，`threshold` 仍由索引回答 |
| IVF | 覆盖度回退区间化 | ✅ 已做 | IVF 分支同样返回 `KnnProbe`，`covers_set` 失败只列未覆盖行 |
| IVF | filter 下沉 | ✅ 已做 | `candidates()` 之后按掩码 retain（`IVF_Filter` 的等价位置），候选预算按选择性放大 |
| cspann | 覆盖度回退区间化 | ✅ 已做 | `CspannHandle::row_ids()` 给出树覆盖的行；差集进 `uncovered` |
| cspann | filter 下沉 | ✅ 已做 | `CspannHandle::search_filtered`：按选择性放大候选后在 `row_id` 上求交（`§12.5` 的 slot 级比对等价的出口） |
| 多向量 / Sparse / Secondary | 仅记录与回退 | ✅ 无需改动 | 三者本来就带 `allowed` 集合与 chunk 记录，回归全绿 |
| 公共件 | `IndexMaintainer` / `Watermarks` / `SealScheduler` / `RecallMonitor` | ✅ 已做（4 个中 4 个） | `index/src/maintain.rs`、`engine/maintain.rs`、`engine/recall.rs` |
| 公共件 | `PendingTail` | ⚠️ 不做 | cspann 用 checkpoint 重建（§20.2 第 6 条），没有 tail 可抽干；HNSW 的 tail 由 dump 事务在原地读（`hnsw_chunk` 吃 `&HnswIndexInMem`），一个「交出所有权」的 `take_for_flush` 只会多一次拷贝，不改变提交边界 |

**落地形状**（两项是同一个根因，一起改的）：

1. `crates/executor/src/knn_scan.rs`：`KnnProbe { candidates, uncovered }` + `KnnProbe::rows()`；
   `KnnCandidateScan` 增 `allowed: Option<&HashSet<usize>>`；覆盖不全时不再 `return None`，
   而是 `uncovered = live - covered`（`covered` 由各 segment 的 labels / IVF offsets / 树的 `row_ids()` 给出）。
2. `crates/executor/src/blocks/engine.rs`：`block_visible_row_count` 被抽成
   `walk_visible_rows`，新增 `live_row_ids`——和 `count(*)` 同一次遍历，**不解码任何 cell**。
3. `crates/executor/src/engine/statements.rs`：
   - `columnar_mask_rows`：只解码谓词涉及的列（`fetch_rows_with_ids_projected_some`，cell 走
     `decode_cell` 以免把文本 `null` 读成 NULL）→ `columnar_filter::predicate_mask` → `matches`；
     谓词形状不被列式过滤器支持（LIKE/IN/函数/列比列）时返回 `None`，此时**仍然**回退整表。
   - 有 filter 且没有掩码 → 精确扫（原行为）；有掩码 → 索引用掩码（`§12.5` 第 5 步的
     `ef' = clamp(ef / max(sel, 1e-3), ef, ef_cap)` 落在 `Engine::candidate_budget`）。
   - 读行：`candidates ∪ uncovered` 走 `fetch_some_rows_with_ids`（`BlockCursor::rows`，
     **block + 写缓冲合并**，并带 tombstone 与 `read_ts` 过滤）——这就是上面那个陷阱的解。
     无 filter 时仍投影到向量列；结果行后续照旧单独读全列。
4. `crates/index/src/hnsw_handler.rs` + `vector.rs`：`search_index_filtered` / `ef_search()`；
   `crates/executor/src/hnsw_mem.rs`：`search_filtered_with_ef`（入参是 **row id**，不是 node）。
5. `crates/storage/src/knn_index/cspann/mod.rs`：`FileIndex::row_ids`（section F）。

**验收（本机实跑 `cargo test -p executor --lib`）**：

| 用例 | 断言 |
|---|---|
| `hnsw_filter_is_pushed_into_the_graph` | `WHERE tag = 4` 带索引与 `DROP INDEX` 后行集完全相同；`allowed` 下 probe 的 candidates 全在掩码内，不传掩码则出现掩码外的行 |
| `hnsw_scores_uncovered_rows_without_a_table_scan` | 无向量的行是唯一 `uncovered`（不再整表回退）；`DELETE` 后 `uncovered` 为空、索引仍在回答 |
| `cspann_filter_and_uncovered_tail_answer_together` | 建树后写入的行 + `WHERE tag = 7`：未覆盖行胜出、树答其余，且与 `DROP INDEX` 一致 |
| `ivf_filter_matches_the_exact_scan` | 带掩码的 IVF 只返回掩码内的行，且与精确扫一致 |
| 既有回归 | `cargo test -p executor --lib` **844 passed**；7 个失败是 `INFINITY_RESOURCE_PATH` 缺失的 analyzer 资源问题，与本改动无关 |
| SLT | `infinity conformance test/sql/dql/knn`：38 个文件全通过（含 `test_knn_hnsw_{l2,ip}_filter`、`test_knn_ivf_{l2,ip}_filter`） |

**仍未做**：只有 `PendingTail`（理由见上表：没有 tail 可抽干）。
§12.4 的 `Coverage::uncovered_ranges()` 实际落成了 `KnnProbe::uncovered` 的**行号集合**——
比区间更精确（未覆盖的往往不是连续区间），回退的代价依旧只与「未覆盖行数」成正比。

§12 之外的两项（并行构图、字节级格式文档）见 §20.4 第 4/5 条，均已完成。

### 12.7 记账与调度的落地形状（2026-11-17）

| 件 | 位置 | 读什么 / 做什么 |
|---|---|---|
| 一套字节账 | `MemoryContext::{memindex_total, memindex_quota}` | 已有的两个数，没有新增第二本账 |
| 三档水位 | `maintain::Watermarks` | `soft = quota/4 ≤ 64 MiB`、`hard = quota/2 ≤ 256 MiB`、AIMD：>80% 预算时 soft 对半（下限 1 B），<50% 恢复 |
| 判定入口 | `Engine::seal_pressure()` | 唯一一处：两条配额路径（`enforce_memindex_quota` / `enforce_quota_after_append`）都只问它 |
| `hard` 的动作 | `Engine::dump_every_table_index` | 该表**每个**有尾的索引都 seal（`soft` 仍只 seal 最大的一个） |
| 受害者选择 | `Engine::pick_seal_candidate` + `maintain::SealScheduler` | 按库加权公平、每库 in-flight ≤ N、饥饿插队并计数 |
| 每索引口径 | `index::maintain::IndexMaintainer` | `pending_bytes / resident_bytes / optimize_cost / can_degrade`；HNSW 用图自身的 `mem_usage()` |
| 召回闭环 | `engine::recall::RecallMonitor` | 7 天窗口均值；`est < target` → beam ×1.5（≤4×）并置 `rebuild_suggested`；`est > target+0.03` 且 p99 超预算 → beam ×0.9（不低于 base） |

验收用例：`the_pressure_level_comes_from_the_byte_ledger`（三档由字节账推出）、
`hard_pressure_seals_every_tail_of_the_table`（soft 一个 / hard 全部）、
`a_graph_built_by_create_index_is_visible_to_the_quota`（`CREATE INDEX` 建的图可被回收）、
`maintain::tests::*`（水位、WFQ、in-flight、饥饿）、`recall::tests::*`（抽样、加宽/收窄、窗口）。
`cargo test -p executor --lib` 844 passed（7 个失败仍是 analyzer 资源缺失），`-p index` 164 / `-p storage` 503 全绿。

---

## 13. 集成改动点（文件级）

**新增**：`crates/index/src/cspann/*`、`crates/storage/src/knn_index/cspann/*`、`crates/executor/src/cspann_mem.rs`、`crates/executor/src/blocks/cspann_chunk.rs`、`test/sql/dql/knn/embedding/test_knn_cspann_{l2,cosine,ip,filter,given_index}.slt`。

**修改**：

| 文件                                    | 位置                                                                                                                                                                                | 改什么                                                            |
| --------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------- |
| `crates/storage/src/wal/index_def.rs` | `:19-31`, `:34-49`                                                                                                                                                              | `IndexType::Cspann`（**新值，不得复用/重排既有值**）      |
| `crates/executor/src/wal_bridge.rs`   | `:252-259`, `:1121-1129`                                                                                                                                                        | `"CSPANN"` ↔ 枚举                                              |
| `engine/statements.rs`                | `validate_index:6433/6498`（对照 HNSW `:6528-6570`）、`build_*_index_of:3770`、`*_mem_segments:3828`、`run_one_match:8796`/索引选择 `:9464`、`vector_candidates:9439` | 新 kind 分支 + 选项白名单                                         |
| `engine/query.rs`                     | `CreateIndex:2019-2308`                                                                                                                                                           | build + 注册 tail                                                 |
| `engine/state.rs`                     | `TableEntry:4802`                                                                                                                                                                 | `cspann: HashMap<u64, Arc<LoadedCspannIndex>>`、`cspann_tail` |
| `engine/dml.rs`                       | `:1393`, `:1509`, `:2343`                                                                                                                                                     | 写路径喂 tail、配额 handoff                                       |
| `engine/index_build.rs`               | `:19`, `:255`, `:754`, `:630-720`, `:1208`, `:949`                                                                                                                      | 恢复/预算/dump 分支                                               |
| `knn_scan.rs`                         | `:122-244`                                                                                                                                                                        | cspann 探针 + 区间回退（§9.7）                                   |
| `blocks/engine.rs`                    | `:4887`, `:5295`, `:6042`, `:5994`                                                                                                                                          | dump/load/optimize 同构函数                                       |
| `engine/ddl.rs` / `admin.rs`        | `:353`, `:494` / `:304`, `:779`                                                                                                                                             | kind 清单与统计                                                   |

---

## 14. 交付物清单

- 本文档（格式与参数冻结）
- `docs/CSPANN_FORMAT.md`（字节级规格，含 CRC 与分节偏移，供跨语言只读解析）
- conformance 测试套件：同一套算法用例分别在 `MemStore` 与 `MmapStore` 上跑
- 召回曲线快照（`recall/latency` 对 `beam`、`npartitions`）
- 压测报告（§16.4）

---

## 15. DDL 与参数

```sql
CREATE INDEX idx ON t (vec) USING CSPANN
  WITH (metric = 'cosine',          -- l2 | ip | cosine
        max_partition_size = 128,   -- 超过则 split（构建期）
        min_partition_size = 16,    -- 低于则 merge（构建期）
        fill_factor = 0.7,          -- num_centroids 推导用
        encode = 'rabitq',          -- rabitq | none
        kmeans_iterations = 16,
        seed = 42,
        adaptive_beam = false);     -- 默认 false
```

查询期：`WITH (nprobe = 32)` / `WITH (recall_target = 0.95)` / `WITH (latency_budget_ms = 5)`。

校验规则：未知键报 `InvalidIndexParam`（对照 HNSW 白名单 `statements.rs:6544-6553`）；`dim` 与列定义不符报错；`metric` 与索引/metric 组合校验。

---

## 16. 实施计划

| 阶段                 | 内容                                                                                      | 出口标准                                                                                                        |
| -------------------- | ----------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| **P0**（3d）   | 本文档 + 格式文档骨架、类型/`CspannStore` trait、选项解析、DDL 注册（可建、走暴力扫）   | `CREATE INDEX … USING CSPANN` 成功；`EXPLAIN` 可见                                                         |
| **P0.5**（2d） | filter 下沉回归修复（§12.5）：per-segment 位图 + `HnswHandler::search_filtered_index` + 执行层接线 + `ef` 放大 | 带 filter 的查询 `EXPLAIN` 走索引；与精确扫行集一致；1% 选择性下 recall ≥ 无过滤基线的 95% |
| **P1**（1.5w） | 纯内存算法：rotation、RaBitQ、BalancedKMeans(k=2)、多层 build、beam + 早停 + 剪枝         | 1k×128d `recall@10 ≥ 0.95`（beam=8）；`encode=none` 时与暴力扫 top-k 完全一致；kmeans 对齐 Cockroach 用例 |
| **P2**（1.5w） | 磁盘格式：writer/reader/mmap/CRC/原子发布、`from_chunk_parts`、`rebuild_indexes` 恢复 | save/load 往返 100% 自召回；写 chunk 中 kill -9 可恢复；截断文件被 CRC 拒绝                                     |
| **P3**（1w）   | 写路径生命周期：`PendingTail`（CodesTail）、handoff、flush 同源、区间回退               | 连续插入 10 万行多次 flush，结果与暴力扫一致；`uncovered` 窗口有界                                            |
| **P4**（1w）   | 能力对齐：多 chunk + `MergeKnn`、cosine/ip、filter 下沉（复用 P0.5 的 `allowed`）、`USING INDEX`、多向量列 | `test_knn_hnsw_*.slt` 复制为 `cspann` 版全部通过                                                            |
| **P5**（1.5w） | out-of-core 与多租户压测：预算驱逐、WILLNEED、cold meta LRU、公平队列、切片 compaction    | §16.4 指标达标                                                                                                 |
| **P6**（2d）   | C++ 改名（附录 A）+ SDK/文档同步                                                          | thrift 数值不变；Go/Python 全绿                                                                                 |

### 16.4 性能与规模验收

| 场景                                               | 目标                                                                                                |
| -------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| 1M × 128d（l2）                                   | `recall@10 ≥ 0.95`（nprobe=8）；p50 ≤ HNSW 的 2×                                               |
| 1M × 128d 等延迟对比（§19.6）                    | p50 对齐 HNSW 的 165 µs 时，recall@10 ≥ HNSW 的 95%（HNSW 基准 0.906 → ≥ 0.86）；禁止降召回换延迟 |
| **1M × 1536d（cosine）**                    | `recall@10 ≥ 0.92`（rabitq）/ `≥ 0.97`（none）；p50 ≤ 8 ms                                   |
| 50M × 1536d（≈ 300 GB 原始）跑在 8 GB RAM 机器上 | RSS ≤ 3 GB 常驻；不 OOM；`recall@10 ≥ 0.88`                                                     |
| 冷缓存（drop_caches）                              | p99 ≤ 10× 热缓存；无"半张图"式失败                                                                |
| 删除 30% 后查询                                    | 结果正确；候选浪费 ≤ 2×                                                                           |
| `optimize` 期间并发查询                          | 无阻塞（`Arc` 快照）                                                                              |
| 1000 万行 build                                    | ≤ 8 min（16 core），内存 ≤ 2×（分区表+centroid）                                                 |
| 1M 索引（10 万租户）                               | hot meta ≤ 64 MB；cold meta LRU ≤ 64 MB；VMA ≤ 4096；worker ≤ 8；索引侧 tail 内存 = Σ memtable |

监控指标：`cspann_candidates_per_query`、`cspann_beam_actual`、`cspann_early_stop_ratio`、`cspann_recall_estimate`、`cspann_compaction_backlog`、`index_seal_wait_p99{db}`、`index_force_sync_seals`、`cspann_degraded_indexes`。

---

## 17. 测试计划

1. **单元**（`crates/index/src/cspann/*`）：rotation 由 seed 重建逐字节一致；RaBitQ 估计误差 ≤ 界；BalancedKMeans 收敛与平衡；SearchSet 剪枝等价暴力 top-k。
2. **Conformance**：同一套用例跑 `MemStore` 与 `MmapStore`（易错路径：空分区、root split 增层、单向量、重复 row_id、超大 topn、`encode=none` 与 `rabitq` 对照）。
3. **SLT**：`test/sql/dql/knn/embedding/test_knn_cspann_*.slt`，每个文件断言索引确实生效（`EXPLAIN` 含 `CSPANN` 或候选数断言），避免"建了索引但走暴力扫"的假通过。
4. **召回门禁**（CI）：128d/l2 与 1536d/cosine 两套曲线的快照测试，recall 下降 > 1% 即 fail。
5. **故障注入**：写 chunk 中 kill -9、`rename` 后 commit 前崩溃、文件截断、catalog 记录缺失。
6. **规模**：§16.4。

---

## 18. 风险与待决

| 风险                          | 影响                   | 对策                                                        |
| ----------------------------- | ---------------------- | ----------------------------------------------------------- |
| RaBitQ seed / rotation 不一致 | 距离全变噪声，不报错   | seed+algo+dims 入 header；单测断言重建一致                  |
| mmap 页错                     | 冷查询 p99 抖动        | WILLNEED 预取、4 KB 对齐、只读所需列                        |
| flush 编码成本                | flush p99 上升         | 后台线程；失败降级为"该批不建 chunk"（走 uncovered 扫描）   |
| chunk 碎片                    | 查询扇出变大           | `max_open_chunks_per_index=6` 强制 compaction             |
| compaction 落后               | 查询变慢（正确性不变） | 优先级：uncovered > chunk 数 > optimize                     |
| 小索引                        | 固定成本浪费 inode/页  | `rows < min_rows_for_cspann`(1024) 标记 `degraded=flat` |
| WAL `IndexType` 取值        | 破坏跨版本恢复         | 只用新值                                                    |
| 与 HNSW 行为漂移              | SLT 假通过             | 每个 SLT 断言索引生效（§17.3）                             |

**待决**：`store_full_vectors` 是否保留为 debug 开关；`min_rows_for_cspann` 默认值；`rotate` 是否允许 `Matrix`（第一版建议 `Givens`）。

---

## 19. 与 HNSW 的对比：基准、内存与选型

> 结论先行：**两者不是替代关系**。dim ≤ 256 且索引放得进 RAM 时 HNSW 更快、召回更高、构建更简单；
> 只有「高维（≥512d）」或「索引 > 可用 RAM」时 CSPANN 才成立（§19.7）。
> 本节所有 HNSW 数字要么来自 `HNSW_BENCH.md`（已测），要么由代码里的字节布局推出（标注"公式"）；
> **CSPANN 一列目前全是目标值**（§16.4 的出口标准），P1 之后才允许填实测数。

### 19.1 单向量占用（公式 + 数表）

HNSW 每向量的字节数由三部分组成（`crates/executor/src/blocks/index_chunk.rs:32-39`、
`crates/storage/src/knn_index/knn_format.rs:31-33`、`:368-385`）：

```
record   = dim*4                    // Plain
         = 16 + dim                 // LVQ
         = 16 + align8(dim)/8       // RaBitQ（16 B = ‖o-c‖ 等标量前缀）
graph_l0 = 24 + 4*Mmax0             // Mmax0 = 2M（:31 get_mmax）；M=16 → 152 B
graph_lx = (4 + 4*Mmax) * layer_sum/n   // layer_sum/n ≈ 1/(M-1) ≈ 0.067；M=16 → 4.5 B
labels   = 4
```

CSPANN 每向量（§6.2）：`4 (row_id) + 4 (cent_dist) + 4 (qdot) + 4 (code_cnt) + 8*ceil(dim/64)`。

| 编码 | 1536d HNSW | 128d HNSW | 1536d CSPANN | 128d CSPANN |
|---|---:|---:|---:|---:|
| plain f32 | 6144 + 160 ≈ **6.30 KB** | 512 + 160 ≈ **672 B** | —（D1 不存全精度） | —（同左） |
| LVQ | 1552 + 160 ≈ **1.67 KB** | 144 + 160 ≈ **304 B** | — | — |
| RaBitQ | 208 + 160 ≈ **368 B** | 32 + 160 ≈ **192 B** | **208 B** | **32 B** |

两条要点：

1. **1536d 下码宽已经打平**（都是 208 B），CSPANN 省的是那 160 B 的图和它**必须常驻**这件事（§19.3），
   不是编码本身；相对于 HNSW plain 是 **30×**，相对于 HNSW RaBitQ 是 **1.77×**。
2. **128d 下差距是 6.0×**（32 B vs 192 B），因为此时图开销（160 B）比码（32 B）还大 5 倍——低维小数据是 HNSW 的舒适区。

落盘体积（同一行数）：

| 1M / 50M × 1536d | HNSW plain | HNSW RaBitQ | CSPANN |
|---|---:|---:|---:|
| 1M | 6.30 GB | 368 MB | **208 MB** |
| 50M | 315 GB | 18.4 GB | **10.4 GB** |

另有一项 HNSW 独有的 per-chunk 常驻：RaBitQ 的旋转矩阵按 `4*d*d` 存（`index_chunk.rs:24-27`），
1536d 即 **9.0 MiB/块**；按 `memindex_capacity=65536` 切块时 1M 行 = 16 块 ≈ 144 MB。
CSPANN 只存 `seed + algo + dims`（§4 `rot.rs`），header 里 **128 B**。

### 19.2 落盘格式差异（为什么 CSPANN 能 mmap 而 HNSW 不能）

| | HNSW（`knn_format.rs:10-26`） | CSPANN（§6） |
|---|---|---|
| 自描述 | **无** magic / 无 version / 无 CRC，靠 catalog 里的 JSON 补类型 | header 含 dim/metric/seed/code_width/分节偏移/CRC |
| 单向量寻址 | 变长记录（record_size 依赖编码）+ 邻居是**顶点下标**，跨块无意义 | 固定 4 KB 对齐 slot + 列式（§6.2），`(block, slot)` 可直接寻址 |
| 访问模式 | 图遍历随机跳，一次只用一个顶点，4 KB 读放大 → 不值得 mmap | 一个分区一段连续 slot，顺序读全分区 |
| 装载方式 | `read_file` 整块读入再解码（`blocks/engine.rs:5934`） | `mmap` 只触碰被探测分区的页（§6.4） |

这不是"顺手做成 mmap"，而是格式决定的：HNSW 的随机跳 + 变长记录一旦走缺页就是每跳一次 page fault，
所以它只能整块解码进堆；CSPANN 的分区连续布局才让 out-of-core 有意义。

### 19.3 常驻内存模型对照

**HNSW：整个解码后的图常驻堆内存。** 装载路径 `blocks/engine.rs:5934` → `:5977-5993` 把邻居建成
`Vec<Vec<Vec<usize>>>`，`state.rs:4936` 用 `Vec<Arc<LoadedVectorIndex>>` 全量持有，只有一个全局预算：

- 默认 **8 GiB**（`statements.rs:380` 的 `hnsw_budget`，注释见 `state.rs:5162-5166`），并且**按索引数均分**
  （`index_build.rs:642-647` 的 `hnsw_budget / hnsw_count`）；
- 超预算按 `bytes` 从大到小驱逐（`index_build.rs:692-707`），被驱逐的块使标签覆盖不全 →
  `knn_scan.rs:239-242` **回退整表精确扫**（`index_build.rs:636-637` 的注释即此意）。

每行常驻字节 ≈ `record + 24 + 8*d0 + levels(8) + labels(4)`，`d0 ≤ Mmax0 = 32`：

| 场景（d0=32 上界） | 每行 RAM | 8 GiB 能装 | 8 GiB / 1000 索引（8.6 MB） |
|---|---:|---:|---:|
| 1536d plain | 6.45 KB | 1.33M 行 | 1.3k 行 |
| 1536d RaBitQ | 510 B | 16.8M 行 | 16.8k 行 |
| 128d plain | 814 B | 10.5M 行 | 10.3k 行 |
| 128d RaBitQ | 334 B | 25.7M 行 | 25.7k 行 |

注意两处记账偏差，做对比时必须写进报告：
（a）`engine.rs:5995-6000` 的 `bytes` 按 **4 B/邻居** 记账，实际 `Vec<usize>` 是 **8 B/邻居**，
预算内真正装得下的比它声称的少 15–30%；
（b）图部分 RAM 是磁盘的 1.5–1.9×（磁盘邻居 4 B，内存 8 B + 每层 24 B Vec 头）。

**CSPANN：常驻与数据量解耦。** 常驻 = hot meta（≈64 B/索引，§10）+ cold meta LRU（≤64 MB）
+ 被触碰的 slot 页（属 `RssFile`，可回收）；`RssAnon` 只有 beam 缓冲与结果集。

### 19.4 单查询成本模型

| 项 | HNSW（`ef=64`） | CSPANN（`beam=8`, `max_partition_size=128`） |
|---|---|---|
| 距离计算次数 | ≈ 访问顶点数（数百～数千），每次一次码/全精度距离 | 8×128 = **1024** 次码估计（popcount，§6.2） |
| 每查询读字节 | 访问顶点数 × record（RaBitQ 208 B / plain 6 KB） | 1024 × 208 B ≈ **213 KB** |
| 访问模式 | **随机跳**（顶点分散在整块/多块） | 分区内**顺序**，`madvise(WILLNEED)` 预取（§6.4） |
| 冷缓存 | 每跳一次缺页，p99 不可控 | 预取掩盖，p99 ≈ 热缓存的 10× 以内（§16.4 目标） |
| 超预算时 | 覆盖不全 → **整表精确扫**（秒级） | 只多扫 uncovered 区间（§9.7） |
| 精确重打分 | plain 模式下自带全精度；码模式要回表 | 一律执行层回表（D1） |

### 19.5 实测基线（HNSW 已测 / CSPANN 待测）

HNSW 一列取自 [`HNSW_BENCH.md`](HNSW_BENCH.md)（单进程 release，`M=16, ef_construction=200`，
数据 1000 簇 + 抖动 SIFT-like，真值为穷举 top-10）：

| 指标 | HNSW（已测） | CSPANN（目标，§16.4） |
|---|---|---|
| 1M×128d 构图 | plain **28.1 s**、lsg 27.6 s | 未测；§16.4 出口是 10M 行 ≤ 8 min（线性折算 1M ≈ 48 s） |
| 1M×128d 重启后 recall@10 | plain 0.906 / 0.852（重启） | ≥ 0.95（nprobe=8，**P1 出口**） |
| 1M×128d 图搜索 p50 | **165 µs**（ef=64，不进 SQL） | 未测；要求 p50 ≤ 2× HNSW |
| 200k×128d 构图 / 服务端 p50 | 4.3 s（plain）/ 4.0 s（lsg）；**0.58 ms** | — |
| 1M×1536d | 未测；plain 每行 6.45 KB → 1M 需 6.45 GB，占 8 GiB 预算 **75%**，装不下第二个索引 | recall@10 ≥ 0.92，p50 ≤ 8 ms |
| 50M×1536d @ 8 GB RAM | **不可用**：18.4 GB（RaBitQ）> 8 GiB → 驱逐 → 整表扫 | RSS ≤ 3 GB，recall@10 ≥ 0.88 |

C++ 侧对照（`HNSW_BENCH.md` §结果，只说明 Rust 图实现没有明显落后）：20k 行 build 0.87×（Rust 快 13%）、
200k 行 1.18×；服务端 exec 20k 5.5×、200k 1.12×。

**不要拿上表两列直接比 recall**：HNSW 是 `ef=64` 的实测，CSPANN 是 `nprobe=8` 的目标，口径不同。

> 补充（2026-11-17）：20k × 128d、release、**同一条 SQL 路径**上的第一个对照（详见 `docs/CSPANN_BENCH.md`）：
>
> | | cspann | HNSW |
> |---|---:|---:|
> | 构图 | **93.6 ms** | 784.6 ms |
> | recall@10 | 0.957 | **0.997** |
> | p50 | 892 µs | **440 µs** |
> | p99 | 3.79 ms | **551 µs** |
> | 常驻 | 1.7 MiB（89 B/行） | 未测 |
>
> 构图一行取自 §20.4 第 4 条落地后同一用例的一次运行（cspann 建树是批量几何运算，
> HNSW 是逐点插入）；其余各行是更早一次运行的值，两次的查询延迟有 ±10% 抖动。
>
> **在这个区间 HNSW 全面更好**，与 §19.7 的选型表一致：128d、数据全进 RAM 时不该用 cspann。
> cspann 的三个价值条件（1536d / 超出 RAM / 50M×8 GB）尚未实测，其中最后一条还依赖 §6.4 的 mmap 读。
公平比较只有一种：**等延迟比召回**（§19.6）。

### 19.6 对比协议（新增基准时必须遵守）

1. **等延迟比召回**：对每种索引扫一遍自己的旋钮（HNSW: `ef`, CSPANN: `nprobe/beam`），
   在 **相同的 p50 延迟**上比 recall@10；禁止拿一方的默认档对另一方的调优档。
2. **三档规模 × 两档缓存**：20k / 1M / 50M；热缓存 + `drop_caches` 冷缓存各一次（§16.4 有冷缓存行）。
3. **必测"超预算"档**：50M×1536d 跑在 8 GB RAM 机器上，显式记录 HNSW 的退化形态
   （覆盖不全 → 整表扫 → 秒级 p50）；这是 CSPANN 唯一无法用别的方式证明的价值。
4. **内存分账**：用 [`MANUAL_ACCEPTANCE.md`](MANUAL_ACCEPTANCE.md) §3.3 的采样脚本，
   分别报 `VmHWM` / `RssAnon` / `RssFile`。CSPANN 的索引页走 `RssFile`，只报 `VmRSS` 会把 mmap 优势
   记成"省内存"，也会把页缓存命中误判成泄漏。
5. **驱动**：`tools/bench_index_types.py` 的 `INDEXES` 加一行
   `("cspann", "vec", IndexInfo("vec", IndexType.Cspann, {...}))`，即可用现有的
   `tools/run_index_bench.sh index-types` 两端跑（P6 之前 C++ 侧没有 CSPANN，只能 Rust 自比）。
6. **报告**：新建 `docs/CSPANN_BENCH.md`，章节与 `HNSW_BENCH.md` 对齐（口径 / 结果 / 结论 / 复现），
   并在 [`INDEX_PERF.md`](INDEX_PERF.md) §1 的表里登记。

### 19.7 选型指引（什么时候用哪个）

| 场景 | 用哪个 | 依据 |
|---|---|---|
| dim ≤ 256 且 `行数 × 每行字节 ≤ 1/4 RAM` | **HNSW** | 无量化损失、构图 28 s/1M、召回 0.906 起步（§19.5）；CSPANN 在此规模只有固定开销 |
| dim ≥ 512（1536d embedding） | **CSPANN** | 高维下 HNSW 的图常驻成本与 plain 记录（6.3 KB/行）都顶到预算（§19.3） |
| 索引 > 可用 RAM（10× 以上） | **CSPANN** | out-of-core（§6.4），HNSW 只能靠驱逐换"整表扫" |
| 多租户 1000+ 索引 | **CSPANN** | HNSW 预算按索引数均分，8.6 MB/索引 ≈ 16.8k 行（§19.3）；CSPANN 常驻 64 B/索引 |
| 写入后立刻要能搜到 | HNSW 更快 | tail 是图，插入即入图（`hnsw_mem.rs:47`）；CSPANN 的 tail 是码，树在 flush 才建（§2.3） |
| 带 filter 的查询 | 都走索引 | HNSW 靠 `BitmaskFilter` + `ef` 放大，cspann 靠 slot 的 `row_ids` 列掩码；参考实现即如此（§12.5）。**Rust 现状是缺口，不是设计** |

### 19.8 不承诺 / 风险

| 项 | 说明 |
|---|---|
| 小数据不快 | < 10k 行时 CSPANN 有固定成本（header + 分区表 + 逐层 beam 下降），不会比 HNSW 快；`rows < min_rows_for_cspann`(1024) 直接标 `degraded=flat`（§18） |
| 低维不省 | 128d 下 CSPANN 32 B/行仍要一次码估计 + 回表，HNSW 的 192 B/行在 RAM 里也就 0.2 GB/1M，省不出来 |
| 高维召回天花板 | 1536d 下两者都受维度灾难影响，CSPANN 的目标 0.92 已含 RaBitQ 误差；`encode=none` 可换 0.97 但代价是 6.3 KB/行（§16.4） |
| 前提会变 | 若将来 HNSW 改成"mmap 图 + 逐跳读"，内存这一项的优势会收窄到 160 B/行的图开销；但随机跳的缺页问题不消失，out-of-core 仍不可用 |
| 公平性 | 任何"CSPANN 比 HNSW 快 N×"的结论，必须附 §19.6 的等延迟口径；否则视为无效结论 |

---

## 20. 实现现状（2026-11-17）

### 20.1 已完成并有测试

| 层 | 文件 | 验证 |
|---|---|---|
| 算法层（P1） | `crates/index/src/cspann/{mod,quantize,kmeans,partition,search,build,store}.rs` | `cargo test -p index cspann --lib` → **24 passed**：树不变式、`encode=none` 与暴力 top-k 逐行一致、rabitq 重排召回 ≥0.9、建树确定性、种子不复用、误差界 |
| 格式层（P2） | `crates/storage/src/knn_index/cspann/mod.rs`（`Chunk::parse` / `write` / `to_index`） | `cargo test -p storage cspann --lib` → **7 passed**：往返一致、slot 页 4 KB 对齐、翻 1 字节被 CRC 拒、截断/魔数错/越界被拒、目录与 chunk 的 metric/encode/seed 不符被拒 |
| 执行层（P0/P4 子集） | `crates/executor/src/cspann_mem.rs` + `state.rs`（`TableEntry::cspann`）+ `query.rs`（`CREATE INDEX` 分支）+ `knn_scan.rs`（候选探测）+ `statements.rs`（`validate_index` 与索引选择） | `cargo test -p executor cspann --lib` → **9 passed**，其中 3 个端到端：自查询 recall、覆盖不足回退精确扫、非法 DDL 拒绝 |
| 目录记录 | `crates/catalog/src/value_format.rs`（`index_type::CSPANN`、`IndexDefValue::cspann`）、`layout.rs` | `CREATE INDEX … USING CSPANN` 可提交，`SHOW INDEX` 可读回 |
| SLT | `test/sql/dql/knn/embedding/test_knn_cspann_l2.slt` | `infinity conformance <file>` → **1 file(s), 0 failed** |
| 持久化（P2↔P3 之间） | `blocks/engine.rs::cspann_chunk` + `publish_index_chunks_of` 的 CSPANN 分支（checkpoint 触发，见 `admin.rs::publish_cspann_chunks`）；`index_build.rs::load_cspann_index` + `cspann_lazy_load` | `cspann_survives_a_restart_and_answers_from_its_chunk`：重启后**没有**内存索引，查询时从 `chunk_0.idx` 懒加载，答案与重启前逐行一致，并断言 `TableEntry::cspann` 确实被填上（证明确实走了索引而非整表扫） |
| 基准（P1 出口） | `docs/CSPANN_BENCH.md`，用例 `cspann_bench_128d`（`#[ignore]`） | 20k×128d release：构图 **93.6 ms（213k 行/s）**、recall@10 **0.957**、端到端 p50 **885 µs**、**89 B/行** |
| **out-of-core 读（§6.4）** | `storage::knn_index::cspann::FileIndex`（`MappedFile` + 已验证 `Layout`），执行层 `CspannHandle::{Memory,File}`；flush 后自动换成 File，堆占用归零 | `file_index_matches_the_in_memory_tree`（两种读法逐行同答案、目录与 chunk 不符即拒）；基准：索引层 **堆 340 µs vs mmap 343 µs（差 0.9%）**，flush 后 `file-backed true / heap 0 B/行` |

回归：`cargo test -p executor --lib` → 825 passed / 7 failed，7 个失败全是 analyzer/fulltext/highlighter
用例，原因是本机没有 `/usr/share/infinity/resource`（需 `INFINITY_RESOURCE_PATH`），与本改动无关。

### 20.2 实现偏差（正文数字按此修正）

| # | 设计 | 实际 | 原因 |
|---|---|---|---|
| 1 | `Rot` 为 `RandomOrtho{None,Givens,Matrix}` | **分块 Hadamard + 符号翻转**，`rot_algo = 1` | 参考的 `d×d` 矩阵在 1536d 是 **9 MiB/chunk**，且每向量 O(d²)（1M 行约 30 min）；RHT 是 O(d log d)、零持久化，只留 `seed`。已对拍「与稠密随机正交矩阵同为等距」 |
| 2 | slot 208 B @1536d（`row_id u32`） | **216 B**（`row_id u64`），列组 `row_ids/norm/raw_norm/sum/error/codes` | 行号是引擎全局序，不是块内下标；`u32` 会静默截断。列组仍按列存，页 4 KB 对齐 |
| 3 | `num_centroids` 由填充率推导，根为宽分区 | 二分建树后 **`collapse_top` 折叠顶部**为宽根（`root_fanout`，默认 64） | 不做折叠时 beam=8 在第 5 层只能覆盖 8/2^k 个叶子，实测召回上限 **0.35** |
| 4 | `ef` = 候选表大小 | 执行层按参考的 **`IncreaseRerankResults`**：`maxResults = max(ceil(1.2·topn), 10)`、`maxExtraResults = ⌊log2(beam)·log2(topn)⌋ × multiplier`（默认 50，`WITH (rerank_multiplier=…)` 可覆盖），上限 1024 行（`cspann_mem::candidate_budget`） | 固定 `10×topn` 只在 `beam=8` 附近等于参考值，无法表达「加宽 beam 而不是加宽池」；实测固定池让召回在 beam≥32 处假性饱和于 0.945，换公式后 beam=128 到 **0.997**（`CSPANN_BENCH.md` §2.4）。池子只影响 I/O——重排按 64 行一批流式解码，常驻是 `O(批·dim)` |
| 5 | `(dist, errorBound)` 早停 | 界为 `2·‖o−c‖·‖q−c‖·√(2(1−error))`（由 `⟨u,û⟩` 直接推出） | 查询端 255 桶量化的贡献未计入；实测比基底误差小两个数量级，记录在 `quantize::error_bound` 的文档里 |
| 6 | §9.2 的 `PendingTail` | **取 §9.4 的 `rebuild` 模式**：没有 tail，checkpoint 时按整列重建一棵树并发布新 chunk | 少一套 tail 记账与 flush 同源逻辑；代价是每次 checkpoint 重建一次（20k 行 **93.6 ms**，1M 行按 213k 行/s 外推 ≈ 4.7 s，`CSPANN_BENCH.md` §2.3） |
| 7 | chunk 落盘/装载 | **已接通**：checkpoint 写 `idx_{id}/seg_0/chunk_{n}.idx`（`publish_index_chunks_of` 的 CSPANN 分支），重启后查询时 `cspann_lazy_load` → `Chunk::parse` → `to_index` | 惰性加载而不是开库时全量装载：索引只在有人查询时才占内存（§10 的 100 万索引场景） |
| 8 | `min_partition_size` 默认 32 | DDL 侧若 `max_partition_size` 更小则自动收敛为 `max/2` | 避免「只写 `max_partition_size=8`」被默认值判为非法 |
| 9 | §11.3 的 `RecallMonitor` 配置**存 catalog** | 状态在内存（`MemoryContext::recall`），目标/budget 由语句的 `WITH` 覆盖 | 一个只调搜索宽度的数不值得改目录格式；重启后几条抽样查询就重新收敛 |
| 10 | 配额只看 `index_pending_bytes`（写路径记账） | HNSW 的口径改为 `max(charge, 图自身 mem_usage())` | `CREATE INDEX` 直接从块建图，不走写路径，charge 恒为 0 → 图再大也回收不了（`a_graph_built_by_create_index_is_visible_to_the_quota`） |
| 11 | §9.5 `hard` → 写线程同步 seal | `hard` → 该表所有尾都 seal，但不阻塞写线程 | 本引擎的 dump 池是多 worker，阻塞写线程省不下内存，级联范围才是有效手段 |
| 12 | §11.4 同一 `RecallMonitor` 也驱动 HNSW 的 `ef` | 只驱动 cspann 的 `beam`；HNSW 仍用 `WITH (ef = …)` 硬指定 | 两者的「宽度」量纲不同（cspann 的 beam 是分区数、HNSW 的 ef 是候选表长度），换算系数没有实测依据，宁可先不自动调 |

WAL：`wal_bridge::index_def_from_json` 对 `CSPANN` 返回 `None`，帧回落到 `KvBatch`（与 DiskANN 现在的处理一致），
回放仍由目录 JSON 恢复；目录记录的 `index_type` 用**新值 `"CSPANN"`**（§13），C++ 引擎读到会拒绝而不是误读成 HNSW。

### 20.3 复现命令

```bash
cd rust_engine
env -u CXX -u CC cargo test -p index   cspann --lib      # 24
env -u CXX -u CC cargo test -p storage cspann --lib      # 7
env -u CXX -u CC cargo test -p executor cspann --lib     # 9
env -u CXX -u CC cargo run -q -p cli --bin infinity -- \
    conformance ../test/sql/dql/knn/embedding/test_knn_cspann_l2.slt
```

（`env -u CXX -u CC` 只为本机服务：shell 里导出的 `CC/CXX` 指向已删除的 Homebrew llvm，
仓库 `.cargo/config.toml` 自带的 `clang/clang++` 才是对的。）

### 20.4 剩余工作（按依赖排序）

1. ~~dump / load 接线~~（已完成，见 §20.1 的持久化行）。
2. ~~mmap 读路径~~（已完成：`FileIndex` + `CspannHandle::File`，索引层耗时与堆内相差 0.9%，
       见 `CSPANN_BENCH.md` §2.1）。剩下的只是**规模实测**（50M 需要离线跑一次）。
3. **filter 下沉（§12.5）**：与 HNSW 共用同一套 `allowed` 位图，cspann 用 slot 的 `row_ids` 列求交。
4. ~~并行构图~~（已完成：**93.6 ms / 20k 行 = 213k 行/s**，10× 于 §16.4 的 20.8k 行/s 验收线）。
   两处改动：①左右子树并发建（key 在分裂后即可算出，仍是 DFS 先序）；
   ②`rebalance` 从「每移一点重扫整簇」改成「一次排序取前 m 远」，`O(n·moves) → O(n log n)`——
   它原来是 **97%** 的建树时间。整棵树与旧的串行实现**逐字节相同**（6 组规模对拍 + 
   `build::tests::keys_are_dfs_preorder`），`CSPANN_BENCH.md` §2.3 有对照表。
5. ~~`docs/CSPANN_FORMAT.md`~~（已完成：头部 128 B 逐字段、分节表、Section A 32 B 字段表、
   slot 页列组、trailer CRC、读取期校验清单、与设计正文的 4 条字节级偏差）。
   ~~§19.6 的 HNSW 等延迟对比~~（已完成：`CSPANN_BENCH.md` §2.4，宽度 8..128 扫描）。
   该扫描第二轮推翻了第一轮的结论：旧版「beam ≥ 32 饱和在 0.945」是固定候选池
   `10 × topn` 造成的假饱和；换成参考公式并在 beam=128 取到 0.997 后，剩下的差距
   全在候选行解码（11.4 ms vs 0.55 ms），不在索引，也不在码宽。
6. ~~§12.1/§12.3 记账统一、§9.5/§9.6 水位与公平调度、§11.3 的 `RecallMonitor` 与预算式查询~~
   （已完成，落地形状见 §12.7）。

---

## 附录 A：C++ 侧改名 CSPANN

1. **thrift 枚举值必须保持不变**：`enum IndexType` 中 `SPFresh`(=10) 改名 `CSPANN`，数值 10 不动；C++/Go/Python 三份生成代码同 PR 重生成。
2. 同时修复 Python SDK 已知不一致：`IndexType` 枚举值（0-based）与 `http_utils.index_type_transfrom`（1-based，`infinity_http.py:718` 按 `.value` 查表）错位 → 改为**按枚举成员映射**。
3. 字符串别名：一个 release 内 `USING SPFRESH` 仍可解析，catalog 统一写 `CSPANN`。
4. 重命名：`kSPFresh→kCSPANN`、`IndexSPFresh→IndexCSPANN`、`SPFreshIndexInMem→CSPANNIndexInMem`、`src/storage/knn_index/knn_spfresh/→knn_cspann/`、`spfresh_index_file_worker*→cspann_index_file_worker*`、相关 SLT/UT 文件名。
5. 文件 magic 已是 `"CSPA"`，只 bump version；旧 `kSPFresh` 文件按同格式读入。

## 附录 B：现有 Rust 代码对照

| 概念         | HNSW 现状                                           | CSPANN 对应                                             |
| ------------ | --------------------------------------------------- | ------------------------------------------------------- |
| 内存段       | `crates/executor/src/hnsw_mem.rs:11`              | `crates/executor/src/cspann_mem.rs`                   |
| 图/树        | `crates/index/src/vector.rs:574`                  | `crates/index/src/cspann/partition.rs`                |
| chunk 编解码 | `crates/executor/src/blocks/index_chunk.rs:196`   | `crates/executor/src/blocks/cspann_chunk.rs`          |
| 磁盘格式     | `crates/storage/src/knn_index/knn_format.rs:1-58` | `crates/storage/src/knn_index/cspann/format.rs`       |
| dump/load    | `blocks/engine.rs:4887` / `:6042`               | `crates/storage/.../cspann/writer.rs` / `reader.rs` |
| 选项解析     | `blocks/index_chunk.rs:42-92`                     | `crates/index/src/cspann/mod.rs::Options`             |
| 候选探针     | `knn_scan.rs:122`                                 | 同文件新增分支                                          |
| 恢复         | `index_build.rs:19/255/754`                       | 同文件新增分支                                          |

---

## 变更记录

| 日期       | 内容                                                                                                                                                                                                                                   |
| ---------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 2026-11-16 | 初版。确定 D1–D5；将"取消 L1"修正为"`PendingTail` 生命周期绑定 memtable + flush 同源"，明确 HNSW/Fulltext 保留各自 tail（依 `hnsw_mem.rs:47`、`state.rs:4912` 的既有实现）；新增区间级覆盖回退（§9.7）与多租户资源模型（§10） |
| 2026-11-16 | 新增 §19「与 HNSW 的对比」：单向量占用公式与数表（1536d 368 B vs 208 B、128d 192 B vs 32 B）、落盘体积、落盘格式差异（为何 HNSW 不能 mmap）、常驻内存账（HNSW 整图解码常驻、8 GiB 预算按索引数均分、驱逐回退整表扫；CSPANN 与数据量解耦）、单查询成本模型、HNSW 实测基线 vs CSPANN 目标、等延迟比召回的对比协议、选型指引与不承诺项 |
| 2026-11-17 | 维护层收口：一套字节账 + `Watermarks` 三档水位（`hard` 升级为「该表所有尾」）、`SealScheduler` 加权公平 + `index_seal_starvation`、`IndexMaintainer`（修掉 `CREATE INDEX` 建图不计费的漏洞）、`RecallMonitor` + `WITH (recall_target/latency_budget_ms/beam=…)`；`CSPANN_BENCH.md` 增 §2.4 等延迟扫描 |
| 2026-11-17 | 建树吞吐 10.3k → **213k 行/s**（并行子树 + 排序式 rebalance，树逐字节不变），20k×128d 构图 1.94 s → 93.6 ms；新增 `docs/CSPANN_FORMAT.md`（字节级规格）；filter 下沉与区间级覆盖回退落地（§12.5/§12.6） |
| 2026-11-17 | 打通持久化与基准：checkpoint 写 cspann chunk（`publish_cspann_chunks`）、重启后查询惰性装载（`cspann_lazy_load`）、目录选项 round-trip（`layout.rs` 的 CSPANN 分支）；新增 `docs/CSPANN_BENCH.md`（20k×128d：recall@10 0.957、p50 885 µs、构图 1.94 s、89 B/行）并回填 §19.5 |
| 2026-11-17 | 落地 P1（算法层）、P2（磁盘格式）、P0/P4 子集（SQL 端到端）：新增 `crates/index/src/cspann/`（7 文件，24 测试）、`crates/storage/src/knn_index/cspann/`（7 测试）、`crates/executor/src/cspann_mem.rs` 与执行层接线（9 测试，含 3 个端到端）、SLT 一条；新增 §20 记录实现现状与 8 条实现偏差（RHT 旋转、slot 216 B、宽根折叠、`ef≈10×topn`、误差界、无 tail、未接 dump/load、`min_partition_size` 收敛） |
| 2026-11-17 | 纠正一条错误认知：原文 §1.1「filter 时禁用索引（语义等价）」不成立。C++ 参考实现把 filter 以 per-segment `Bitmask` 下沉进 HNSW/IVF 图搜索（`physical_knn_scan_impl.cpp:607-622`、`:893-920`、`hnsw_alg.cppm:133-148`），Rust 侧 `has_filter → return None`（`statements.rs:9526-9531`）是移植缺口。新增决策 **D6** 与 §12.5（证据、缺口、5 步修复、验收标准），并同步修正 §1.1、§12.1 改动表、§19.7 选型表 |
