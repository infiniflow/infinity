# 未落盘内存的周期回收

目标：长期不访问、但仍在内存里持有未落盘数据的表，不再无限占用内存。

这里的"未落盘内存"是两块，合起来是本文的回收对象：

| 内存形态 | 是什么 | 落盘路径 |
|---|---|---|
| append buffer（`TableEntry::pending_rows`） | 已提交、但还没写进块文件的行（参考版 `BufferObject` 的角色） | checkpoint 的块写 |
| 索引 tail（`text` / `sparse` / `mv` / `hnsw_mem` / `ivf_mem` / `spann_mem`） | 已写入索引、但还没封成 chunk 的部分 | `DumpMemIndex` 事务 |

代码里的存档位是 `unsealed_*`（`unsealed_bytes`、`unsealed_marks`、`unsealed_segment`），它指的就是"尚未封存"，两块都算，所以配置键与字段沿用这个前缀，只有本文改了名。

标注：**[实测]** / **[推算]** / **[设计]**。

## 1. 现状：三条缺口

内存索引与 append buffer 只有**写路径**上的门限触发：

| 触发点 | 位置 | 检查的表 |
|---|---|---|
| `enforce_memindex_quota(Some(table))` | `dml.rs:2291` ← 全文索引写入 | 该表 |
| `enforce_quota_after_append(table)` | 提交末尾 | 该表 |
| `seal_pending_over_budget(table)` | `dml.rs:1168`、`import.rs:793` | 该表 |
| `dump_hnsw_over_capacity(table)` | 每批 append 后 | 该表 |

全局判定 `enforce_memindex_quota(None)` 存在，但只有测试在调用。由此有三条缺口：

**A. 闲置表不再被检查。** 表写完就没人写它，此后没有任何路径调用配额检查，它的索引 tail 与 `pending_rows` 一直留在内存里。

**B. 压力被发现后，排水到不了闲置表。** `enforce_memindex_quota(Some(other))` 里 `seal_pressure()` 是**引擎级**的（`index_build.rs:4568`），但 `dump_under_pressure(Some(other))` 把受害者限定在 `other`：

```rust
// index_build.rs:4383
pub(crate) async fn dump_under_pressure(&self, only: Option<&str>)
// index_build.rs:4500 pick_seal_candidate(only) 用 wanted 过滤掉别的表
```

限定写入表本身是对的（不限定会让写入表永远不排水），代价是 A 表超限时只有 A 表挨刀。

**C. 写路径的候选集只有一张表的索引，跨表没有任何排序。** `pick_seal_candidate` 的候选集是 `only` 指定那张表的索引（`index_build.rs:4500-4526`），所以一张已闲置数小时的表根本不在候选集里。`dump_under_pressure` 的生产调用点全都传 `Some(table)`（`statements.rs:841`、`:876`、`state.rs:10256`），`None` 只有测试在用，因此连"跨表挑一个"的路径都不存在。

候选集内部的选择也不是"选最大"：`SealScheduler` 的权重按库名聚合，而候选集必然同属一个库，权重恒等，实际退化为按 `last_dump` 轮转（§4.4 有实测）。

**三条缺口合起来：内存总量没有上界，只有「正在写的那张表」有上界。**

## 2. 现有 1 秒 tick 不排水

`trigger_loop`（`engine/tasks.rs:88`）每秒都在跑，但四件事都不是排水：

| 步骤 | 实际作用 | 能释放索引 tail 吗 |
|---|---|---|
| `checkpoint_wal_async` + `checkpoint`（默认 `86400s`） | 写块文件 | **不能**——`flush_storage` 不 dump 索引（`admin.rs:1311` 注释） |
| `memory_tick()` | `malloc_trim` + `madvise` | **不能**——释放的是页缓存，堆上的 tail 要靠结构体析构 |
| `submit_optimize_indexes` / `notify_compact` / `submit_cleanup` | 各自语义 | 否 |

tick 是**正确的位置**，只是缺少一个步骤。

`enforce_memindex_quota` 里的「不要时间门」注释针对**写路径**（两次连写的结果不该取决于上一条语句多快），不构成「周期检查不能有时钟」——周期触发本身就是时钟。

## 3. 参考版的做法（对照，不作为移植目标）

参考同样只有写路径驱动：

- `MemIndexTracer::IncreaseMemoryUsage` 在**每次内存增长时**比较 `cur_index_memory_ > index_memory_limit_ + proposed_dump_size_`（`memindex_tracer.cppm:114`），靠 `proposed_dump_` 去重，没有时钟。
- `MakeDumpTask` 每次挑**一个**非 EMVB 索引（按 `mem_used_` 降序）+ 所有 EMVB 索引。
- `PeriodicTriggerThread::Run`（`periodic_trigger_thread_impl.cpp:49-74`）只跑 checkpoint / compact / optimize / cleanup 四个 trigger，**不排水**。
- `NewTxn::CommitBottomAppend`（`new_txn_data_impl.cpp:1920-1932`）在段写满时提交 `DumpMemIndexTask`；`CheckpointTable`（同文件 1440 起）不提交。

因此参考也有同样的闲置堆积，只是被 `checkpoint_interval` 与段写满这两个事件稀释。本节方案是**与参考的有意偏离**，在 `GAPS.md` §1 登记。

## 4. 方案：以表为单位的融合排序 + 逐个排水

一条循环：每张表算一个 unsealed 足迹，按**尺寸**与**年龄**两路排名后融合成一个顺序，逐张 dump 直到满足退出条件。

```
reclaim_unsealed(now_ms, target) -> ():
    // 退避判定在入口（§4.2），退避期内直接返回
    loop:
        stats      = 每张表的 (key, footprint, sort_base)  // §4.1，target 决定 sort_base
        candidates = stats.filter(has_unsealed)          // §4.0，不是 footprint > 0
        if candidates.is_empty(): break
        order   = fuse(candidates, target)                // §4.1
        dumped  = 0
        progressed = false
        for key in order:
            if target_met() { return }                   // 达标：本轮结束
            freed = dump_table(key).await                // §4.3
            if freed == false { continue }
            progressed = true
            dumped += 1
            if max_tables_per_round != 0 && dumped >= max_tables_per_round { return }
        if !progressed { break }                          // 整轮无进展，防止空转
    // 出口：本轮有任何进展 → 清除退避；一张都没成功 → 置 now + interval（§4.2）
```

两个触发点只是喂进不同的 `target`：

| 触发 | 调用 | `target_met` |
|---|---|---|
| 压力（每 tick） | `reclaim_unsealed(now, Bytes(soft))` | 重读引擎总量，`<= soft` |
| 陈旧（每 `unsealed_reclaim_interval`） | `reclaim_unsealed(now, Stale(idle_age))` | 剩余候选的 `age` 都 `<= idle_age` |

### 4.0 表级足迹：数据从哪来

选表需要每张表两个数。排序用的两个量都在 `TableEntry` 里；判"有没有数据"需要一次 `list_indexes()`（一次前缀扫描，与表数无关）加一次 `tables` 锁内的原子读取——**这是 §4.0 后半段要新增的结构**，现有的 registry 无法无锁读取，见下。

| 量 | 来源 | 含义 |
|---|---|---|
| `bytes` | `entry.unsealed_bytes() + entry.memindex_bytes` | 排序用的近似足迹；**不是**"有没有数据"的判据（见下） |
| `age` | `now_ms - entry.last_write_ms` | 距最后一次 append（或最后一次回收）多久 |

- `unsealed_bytes()` 是运行总量：行数据（`pending_bytes`）加每列的 `Vec<Value>` 头开销（`state.rs:4857-4866`）；空表返回 0，因此从没写过的表不占候选名额。`memindex_bytes` 是每张表的索引 tail 计费，由 `refresh_memindex_charge`（`index_build.rs:2459`）维护、`recompute_memindex_total`（`index_build.rs:2485`）全量重算。
- **不把索引字节并进 `unsealed_bytes()`。** `budget_bytes()`（`index_build.rs:2448`）是 `全局 memindex_total + Σ unsealed_bytes`，并进去会重复计数。新增独立读法：

```rust
/// 这张表还没落盘的全部字节：append buffer + 索引 tail。
/// 只用于周期回收的选表排序，不参与 `budget_bytes` 的记账。
pub(crate) fn unsealed_footprint(&self) -> usize {
    self.unsealed_bytes() + self.memindex_bytes
}
```

- **`memindex_bytes` 是缓存，不能单独用来判断"这张表有没有 unsealed 数据"。** 它初始为 0，只在 `refresh_memindex_charge` 与 `recompute_memindex_total` 里更新；而 `build_index`（`CREATE INDEX` 的建索引路径）不调用这两个函数。所以一张表上 `CREATE INDEX` 之后，它的 tail 已经持有整表的索引，但 `memindex_bytes` 仍是 0。

  `index_memory_bytes` 的注释已经写过这件事（`index_build.rs:4658-4660`：「A graph built by `CREATE INDEX` over an existing table charges nothing, so before `IndexMaintainer` it looked empty here and no quota could ever release it」）。

  因此候选集的判据不能是 `footprint > 0`，而是三个条件的或：

  ```
  has_unsealed(t) = t.unsealed_bytes() > 0
                 || t.memindex_bytes > 0
                 || 该表任一索引的 unsealed_marks[id] > 0     // 见下
  ```

  第三项**不能**直接复用 `index_has_memory_tail`（两个理由见下），要新增无锁标记。但 `index_has_memory_tail` 本身仍要先补一个洞：**它对 SPANN 无条件返回 `true`**（`index_build.rs:1017`）。函数的分支覆盖 secondary/HNSW/IVF/FULLTEXT/BMP/PLAID/SMVE/EMVB，没有 SPANN，最后一行是 `true`——它的 doc 写明这是故意的（"A kind whose tail is not modelled here reports `true`, so the dump keeps its old behaviour"）。

  这仍然必须修，因为它还是 `dump_index_transaction` 的早退判据（`index_build.rs:1254-1259`）与 `dump_every_table_index` 的过滤（`:4554`）：任何有 SPANN 索引的表恒为"有 tail"，每次尝试都会 `reserve_commit_ts()`（1260）并重发一次 chunk 记录。这不是空转，是每轮多一个 commit ts。

  所以**交付项 7 必须给 `index_memory_bytes` 补 SPANN 分支**（`entry.spann_mem` 的 `mem_usage()` 之和 + `entry.spann` 的 `mem_usage()` 之和）。

  **但 `index_has_memory_tail` 的 SPANN 分支要保留兜底 `true`，不能改成「tail 非空」。** SPANN 的 dump 不是由内存 tail 驱动的：`spann_build_in` 从**列**读行建树，所以 `CREATE INDEX` 在一张完全没有 `spann_mem` tail 的表上**照样要 dump**——那正是索引在建语句里被实体化的唯一途径（`query.rs` 在目录记录出现后立刻调 `dump_index_transaction`）。把它改成精确判据会让 SPANN 索引永远建不出来（实测：`a_small_spann_index_stays_flat_until_it_has_enough_rows` 立即失败）。真正防止空转的是另外两道过滤：`pick_seal_candidate` 与 `dump_every_table_index` 都丢掉 `index_memory_bytes == 0` 的索引，而补好的 `index_memory_bytes` 已经能给出 SPANN 的非零值。

  **`index_has_memory_tail` 不能当 `has_unsealed` 的第三项，它有两个问题：**

  1. **它用 `try_lock` 探测，竞争时报"没有数据"。** HNSW 分支是 `m.try_lock().ok().is_some_and(|g| !g.is_empty())`（`index_build.rs:1048-1050`），全文是 `t.try_read().ok().map(...)`（`1065-1067`）。一张正在被查询的表恰恰有 tail、恰恰锁不上，于是被判成"没有 unsealed 数据"、不进候选集，**永久**留在内存里（直到某次探测恰好不竞争）。
  2. **BMP 与多向量用的是阻塞 `std::sync::RwLock::read()`**（`index_build.rs:1085`、`:1101`），而 `index_has_memory_tail` 在 async 路径上被调用。放到每秒一次的 tick 上，等于在 runtime worker 上对每个 BMP/多向量索引做一次可能阻塞的读（写路径 `update_sparse_indexes` 持同一把写锁，`dml.rs:2176`；多向量在 `dml.rs:2134`）——`AGENTS.md` §7.2 禁止的形态。

  所以 `has_unsealed` 不能靠探测索引内部状态，必须**非阻塞**。而"直接在 `tables` 锁内读各 registry 的长度字段"在当前代码里也做不到——**七个 registry 里只有两个的计数是无锁的**：

  | registry | 计数 | 获取方式 | 能在 `tables` 锁内无阻塞读 |
  |---|---|---|---|
  | `secondary.indexes` | `row_count()` | `AtomicUsize`（`index/src/lib.rs:336-338`）；`approx_bytes()` 也是原子（`:237-239`） | **是** |
  | `ivf_mem` | `Vec` 长度 | 普通字段，`tables` 锁即覆盖 | **是** |
  | `hnsw_mem` | `rows.len()` | `HnswIndexInMem` 在 `Arc<tokio::sync::Mutex<…>>` 后（`state.rs:5022`）；`len()` 是普通 `Vec` 读，但**取到对象本身就要 `lock().await`** | **否** |
  | `text` | `pending_docs()` | `Arc<tokio::RwLock<TextIndex>>`（`state.rs:5050`）；方法要遍历 `runs`/`deleted`（`text_index.rs:454-462`） | **否** |
  | `sparse` | `tail.len()` | `Arc<std::sync::RwLock<SparseIndex>>`（`state.rs:5042`）；`tail` 是 `BTreeMap`（`state.rs:4171`） | **否** |
  | `mv` | `tail.len()` | 同上（`state.rs:5044`、`:3298`） | **否** |
  | `spann_mem` | `len()` | `SpannTail::len()` 取 `tree.read()` + `dead.lock()`（`spann_mem.rs:325-329`） | **否** |

  所以**交付项 7 还包含一处结构改动**：给 `TableEntry` 加一个 per-index 的原子计数，由写钩子在增长时维护、由 dump 在清空时归零，让 `has_unsealed` 只读原子：

  ```rust
  /// 每个索引的 tail 行数（或字节数），写钩子加、dump 清零。
  /// 只给周期回收的候选判定用；不参与 `budget_bytes` 的记账。
  pub(crate) unsealed_marks: HashMap<u64, Arc<AtomicU64>>,
  ```

  两条必须遵守的约束：

  - **原子值必须在所有增长点与清零点同处更新**，否则会出现"原子说 0、实际有 tail"（漏候选，正是要修的缺口 A）或反过来（每轮空转）。**这个约束本身就是验收项**：交付项 7 的单测要覆盖"尾非空 → 原子非 0"与"dump 之后 → 原子为 0"两个方向。
    - 增长点（各索引自己的写钩子）：

      | 索引 | 钩子 |
      |---|---|
      | HNSW | `note_hnsw_insert` / `note_hnsw_insert_batch`（`dml.rs:1870`、`:1898`） |
      | IVF | `note_ivf_insert`（`dml.rs:1954`） |
      | 多向量 | `update_multivector_indexes`（`dml.rs:2095`） |
      | BMP | `update_sparse_indexes`（`dml.rs:2150`） |
      | 全文 | `update_text_indexes_batch_inner`（`dml.rs:2295` 起，最终落到 `index_row_prepared`，`state.rs:7087`） |
      | SPANN | `blocks/engine.rs:1779-1791`（`buffer_rows` 路径）与 `:3512-3524`（`apply_replayed_rows` 路径），与 `mirror_epoch += 1`（`:1804`、`:3528`）同处 |
    - 清零点：`seal_index_tail` 的每个分支（`index_build.rs:1552-1600`）、`release_hnsw_memory`（`:1625`）、`hnsw_mem.retain`（`:4804`、`dml.rs:3234`）、`ivf_mem.clear()`（`:1578`）。
  - **它必须允许"宁多勿少"。** 一张表的原子非 0 但 `dump_table` 实际释放 0 字节，只会让这张表多试一次（由 §4.5 的失败退避收敛）；反过来"实际有 tail 但原子为 0"才会让它永久留在内存里。所以漏加比多加严重得多。

  这样 §4.1 的成本才是"一次 `tables` 锁内遍历原子"，而不是"每个索引一次锁"。若实现时觉得 per-index 原子太重，可退而求其次：只维护**每张表一个** `Arc<AtomicU64>`（`TableEntry::unsealed_mark`），由同一组钩子增减——精度更低（一个索引清空不代表整表清空），但候选判定只需要"这张表有没有东西可排"，这个粒度够用，且哈希表从 per-index 降到 per-table。两种写法的验收相同。

  排序用的 `bytes` 仍可以是 `footprint`（缓存值只影响先后，不影响是否回收）。但**判"是否该排"必须用 `has_unsealed`**，否则 `CREATE INDEX` 刚建好的索引永远进不了候选集，而它恰好是内存占用最大、又完全没有写路径会触发 `enforce_memindex_quota` 的情形——`CREATE INDEX` 本身不经过 append 的配额检查。

- `last_write_ms` 需要新增，见 §4.5。

### 4.1 两路排序与融合

尺寸与年龄**不可相加**：字节的量纲随表规模变化，年龄的量纲是时间，任何加权和都要挑系数，而合适的系数随部署变化。所以只融合**名次**：

```
size_rank: 按 bytes 降序，rank 从 1 开始
age_rank:  按 age 降序，rank 从 1 开始，其中
           age = now - sort_base(t)
           sort_base(t) = last_write_ms                        (Stale)
                        = max(last_write_ms, last_attempt_ms)  (Bytes)
score(t) = 1/(K + size_rank(t)) + 1/(K + age_rank(t))     K = 3
order    = score 降序
```

`sort_base` 按 target 分叉：压力路径（`Bytes`）用 `max(...)` 让失败表排到队尾（§4.5），陈旧路径（`Stale`）只用 `last_write_ms`，因为它的语义是"这张表多久没动"，与"上次尝试排水"无关。

这是 RRF（Reciprocal Rank Fusion，Cormack 等 2009）的两路形式。

**`K = 3`：年龄的优先级高于尺寸。** `K` 越小，靠前的名次带来的分差越大，单个维度的领先越难被另一个维度抹平。取 3 时：

| 候选 | size_rank | age_rank | score |
|---|---|---|---|
| 又大又老 | 1 | 1 | 0.500 |
| 最老、很小 | 50 | 1 | 0.269 |
| 最大、很新 | 1 | 50 | 0.269 |
| 中等、中等 | 10 | 10 | 0.154 |

- 第 2 行与第 3 行同分：最老的小表与最大的新表等价优先——年龄与尺寸各自的第一名地位相同，但都低于「两项都靠前」。
- 换成 `K = 60`（RRF 的通用值）时上表四行分为 0.0328 / 0.0255 / 0.0255 / **0.0286**——顺序变成「大而老 > 中中 > 老而小 = 大而新」。注意「中中」（两项都是第 10 名）超过了「单项第一」，这正是把头部差距压平的直接后果：`K` 越大，两个维度的领先越不值钱，名次之和相同的候选越占优。年龄的相对权重因此被摊薄。
- `K = 3` 时年龄维度占优的依据：本方案的首要目标是"长期闲置的表不再占内存"（§1 缺口 A）。尺寸大的表即使暂时没被回收，它的增长会立刻被压力路径看见；而闲置表没有任何路径会发现它。所以排序应偏向年龄。

- **只在候选集内排名**。`has_unsealed` 为假的表不参与（§4.0），否则会占掉名次、把真正的候选挤到后半段。按 §4.0 的实现（per-index/per-table 原子标记），整个扫描是**一次 `tables` 锁内**读原子，外加一次 `list_indexes()`；不是每个索引一次锁。
- **平局按 `age` 降序、再按 `bytes` 降序、最后按表名升序**，保证顺序确定——测试可以断言"先回收哪张"，而不是靠运气。

  **平局必须先看年龄，这是一个必须显式处理的真实情形，不是理论边界。** 融合分是对称的：`1/(K+a) + 1/(K+b)` 不区分哪个名次来自哪个维度，所以"最大且最新"（`size_rank=1, age_rank=n`）与"最小且最老"（`size_rank=n, age_rank=1`）的分数**恒等**——上面表格的第 2/3 行同分正是这个原因。若平局按 `bytes` 降序，这一对就交给大表，于是 `K = 3` 想表达的"年龄优先于尺寸"被自己的平局规则完全抵消，一张忙碌的大表可以把闲置小表挤出 `unsealed_max_tables_per_round`。所以年龄是第一个平局键。

  实现用整数交叉相乘，不做浮点：`a = K + size_rank`、`b = K + age_rank`，分数即 `(a + b) / (a * b)`；比较两张表就是比较 `num_x * den_y` 与 `num_y * den_x`（`u128`），因此**不会**因为舍入把一对同分表分出先后，也不会因舍入把本应同分的判成不同分。

- 实现是两次排序，`n` 是表数，不是行数。

对比 §1 缺口 C：缺口 C 的实质是"既有的排水路径从不跨表"。`dump_under_pressure` 的三个生产调用点都传 `Some(table)`（`statements.rs:841`、`:876`、`state.rs:10256`），`None` 只有测试在用。

本方案的两条路径都跨表：`reclaim_unsealed` 的候选集包含所有表的 `has_unsealed` 为真的项，排序（本节）决定先排哪张。所以缺口 C 由"新增了跨表的调用者"修掉，融合只负责这个新候选集内部的先后。

### 4.2 排水循环的约束：退出、封顶、退避

- **`Bytes(soft)` 的判据在每个候选之前重读一次 `budget_bytes()`。** dump 会因 import in flight、或另一条路径已排空而释放不了，也会因并发写入让总量反弹；增量记账不可靠。所以 `target_met()` 在 `for key in order` 循环里、每次 `dump_table` 之前调用一次。

  次数是 **`1 + dumped`**（`dumped` 是本轮实际 dump 的张数），**上界是 `1 + unsealed_max_tables_per_round`**。一旦达标立即返回，所以常见的"压力解除"场景只有 1～2 次。按上界估算：10000 表 × `cap = 100` 是每轮 101 次求和与 101 次短锁。这是有意的（总量必须新鲜），但成本要按上界算，不能按"一次求和"。

  **`Bytes` 的终止性靠"每轮至少一张表被清空 + 轮数封顶"，不靠总量单调下降。** 总量可能被并发写入推高到 `soft` 以上，所以"降到 soft 以下"这个条件不保证可达；如果只用它作退出条件，一个持续写入的引擎会让循环一直转到轮数上限。这是有意的：压力路径每次 tick 只做有限的工作，剩下的留给下一次 tick，而不是在一次调用里等到总量回落。因此 `Bytes` 的退出是 `target_met() || dumped >= cap`，两者任一成立就返回。
- **`MAX_TABLES_PER_ROUND` 是必须的护栏。** 没有它，一次压力触发可能在持有 `flush` 串行化的情况下连续写 1000 张表。默认 100（`unsealed_max_tables_per_round`，见 §5），超出的交给下一轮——总量回落渐进，符合后台任务的定位。陈旧路径同样需要它：1000 张表同时超龄时，一次扫描排空全部会在 `flush` 串行化上停留很久。

- **`target_met` 用与 `seal_pressure()` 相同的口径。** `Bytes(soft)` 的判据不是"候选集的 `footprint` 之和"，而是 `budget_bytes()`——`memindex_total + Σ 所有表的 unsealed_bytes()`（`index_build.rs:2448`）。压力本身就是按这个数判出来的，如果退出条件改用另一个口径，两边会对不上：`budget_bytes` 里 `memindex_total` 是全局原子，不等于 `Σ memindex_bytes`（后者是缓存，可能滞后一个批次）。

  `unsealed_footprint()` 只用于**排序**，不用于判断是否已达标。两者故意分开：排序容忍近似，退出条件不能。

- **tick 上的压力判定不能调 `seal_pressure()`，因为它会改 `soft`。** `seal_pressure()` 在算 `level` 之前调 `marks.retune(pending)`（`index_build.rs:4577-4581`），而 `retune` 在超过 80% 预算时把 `soft` 减半（`maintain.rs:121-134`）。今天它只在写路径上被调用（频率 = 写入频率）；一旦提到每 tick，引擎持续超 80% 而**没有写入**时，`soft` 会每秒减半：64 MiB → 32 → 16 → … → 1 字节，约 6 秒触底。之后 `Bytes(1)` 的 `target_met` 要求总量降到 1 字节以下，等于把整池排空。

  所以新增一个**纯读**的 `Watermarks::pressure(pending)`（只 `level`，不 `retune`），tick 上用它；`retune` 仍只由 `seal_pressure()` 的写路径调用驱动。§6 的"水线推导不变"因此要补一句"更新频率也不变"。

- **`soft` 在本轮开始时取一次，不在循环里取。** `Watermarks::soft()` 现在是 `#[cfg(test)]`（`maintain.rs:93-97`），生产路径取不到，所以要先去掉那个属性。取一次之后循环内的 `target_met` 只重读 `budget_bytes()`；否则每迭代一次 `soft` 可能又变，退出线会一路收紧。

`dump_table` 返回 `false` 时 `continue` 而不是 `break`：某张表因 import 暂不可排、或某索引在 dump 前已被别处排空，都不该阻塞排在它后面的表。但整轮都没有进展时必须 `break`，否则死循环。

**`break` 不足以阻止永久 churn，还需要退避，而且 SPANN 的释放空洞必须先补。** `budget_bytes()` 里有一类字节是**任何**排水路径都释放不了的：`memindex_bytes_of` 里有两项 spann：`e.spann_mem`（tail，`index_build.rs:2401-2404`）与 `e.spann`（已发布句柄，`2409-2412`）。后者由 checkpoint 的 `publish_spann_chunks` 与 compaction 的 `rebuild_compacted_spann`（`admin.rs:986-990`）改变。真正释放不掉的是 **`Memory` 形态**的句柄：`SpannHandle::mem_usage` 对 `File` 形态返回 0（`spann_mem.rs:602-609`），因为它的槽位是 page cache，只有 `Memory` 形态（发布前那个，直到 checkpoint 把它换成 `File`）才占堆。

更严重的是 SPANN 的 tail 连 dump 机会都没有：`index_memory_bytes`（`index_build.rs:4641-4726`）没有 SPANN 分支，落到最后的 `mv.and_then(...)` 返回 0，而 `dump_every_table_index` 与 `pick_seal_candidate` 都用 `index_memory_bytes(...) == 0 → continue` 过滤（`index_build.rs:4513`、`:4557`）。`dump_index_transaction` 对 SPANN 本身是有效的（`publish_index_chunks` 走 SPANN 分支并 `entry.spann_mem.remove(id)`）。所以**这是一处必须先修的缺陷**：给 `index_memory_bytes` 补 SPANN 分支（`e.spann_mem` 的 `mem_usage()` 之和）。

补完之后仍可能有释放不掉的项（`e.spann` 那一类），所以还要一条退避规则。

**状态：`Context::unsealed_backoff_until: AtomicU64`，per-engine（不是 per-target）。** 它记录"在此之前不要进入 `reclaim_unsealed`"的单调毫秒。三条状态转移：

| 事件 | 动作 |
|---|---|
| 一轮结束、`progressed == false` | `store(now + unsealed_reclaim_interval)` |
| 一轮结束、`progressed == true` | `store(0)`（立即清除） |
| 退避期间 | tick 上的 `Watermarks::pressure()` 照常判定（纯读），但**不进入** `reclaim_unsealed` |

**两条路径共用一个原子。** 它们的"无进展"含义相同：候选非空、但每张 `dump_table` 都返回 `false`（I/O 失败，或只含释放不掉的 `Memory` 形态 spann 句柄）。"全部超龄表都在 import"不会造成无进展——那些表已被排除在候选集外，`candidates.is_empty()` 先 `break`。所以是 `Context::unsealed_backoff_until` 一个原子。

不这么做的话：每 tick 都会跑满一整轮，每轮最多 `max_tables_per_round` 次 `dump_every_table_index`（每次扫一遍 `catalog.list_indexes()`），即每秒上百次前缀扫描。

**`Stale` 的循环必然终止，靠三条合起来：**

1. `dump_table` 排空了该表的 unsealed 数据（§4.3 的 `unsealed_live` 变小且已无 tail）时，把 `last_write_ms` 设为 `now` 且 `dump_failures = 0`，于是它下一轮不进候选集。
2. `dump_table` 返回 `false` 时 `last_write_ms` 不前进，但 `progressed` 保持假——整轮全是这种表就 `break`。
3. `unsealed_max_tables_per_round` 封顶，即使有源源不断的新超龄表也在一轮后返回。

**并发写入是唯一的例外**：一张表 dump 完、`last_write_ms` 设为 `now`，另一个事务立刻写入让它重新超龄——但它同时把 `last_write_ms` 又往前推了（§4.5 约束 1），所以它不会在**本次**循环里再次满足 `age > idle_age`。`Stale` 的判据是年龄，而写入会重置年龄，这天然把"刚写过"排除在外。

需要注意 `Stale` 与 `Bytes` 的判据不对称：`Bytes` 看全局总量（可能被别的表写高），`Stale` 看逐表年龄（只会被该表自己的写入重置）。所以 `Stale` 不需要像 `Bytes` 那样每轮重读全局值。

**`import_in_flight` 的表直接排除在候选集之外**，而不是让 `dump_table` 返回 0。后者会让它每轮都进排序、每轮都被跳过：陈旧路径 30 秒一次时会一直选中同一张表（它可能恰好排第一），把时间花在无用的尝试上。排除在候选集之外更直接，且与 `dump_index_transaction` 自身的检查（`index_build.rs:1244`）不冲突——那层检查仍然保留，防止检查后到执行之间状态变化。

### 4.3 `dump_table` 做什么

```
dump_table(key) -> bool:            // true = 该表本轮确实释放了
    before = unsealed_live(&key)                    // 逐表现算，见下
    // 第一段：块写，需要这张表的 bottom gate
    run_commit_bottom(key, |e| e.write_blocks_bounded_locked(key, checkpoint_ts))
    // 第二段：索引 dump，**必须在 gate 释放之后**，且不能和上面共用一个 bottom half
    dump_every_table_index(Some(key))
    recompute_memindex_total()
    return unsealed_live(&key) < before             // 该表是否真的变小了

unsealed_live(key) -> usize:
    return entry.unsealed_bytes() + memindex_bytes_of(&key)
```

**进度信号必须同时覆盖两块内存，`memindex_bytes_of` 单独不够。** 它只算索引 tail（`index_build.rs:2344`）；一张**没有索引**、只有 append buffer 的表（最常见的形态）无论块写释放了多少行，它前后都是 0——`dump_table` 会把一次成功的块写回报成"没有释放"，于是 `progressed` 恒假、退避被置上，而这张表下次再也进不来。用 `unsealed_bytes()`（运行时维护的精确值，`state.rs:4857`）加 `memindex_bytes_of()`（当场现算）才对。

**两段必须分开，这是死锁边界。** checkpoint 的锁序是 `flush_lock → bottom gate`（`admin.rs:1313` 取 `flush_lock.write()`，随后 `flush_blocks` → `flush_table` 在 `engine.rs:1884-1885` 取 gate），而 `dump_index_transaction` 需要 `flush_lock.write()`（`index_build.rs:1280`）。若把块写和索引 dump 放进**同一个** `run_commit_bottom`，回收线程就是"持 bottom gate 等 `flush_lock`"，与 checkpoint 的方向相反，两个线程永久 park、无报错。

**两阶段之间插入的 append 是无害的**，因为两段各自有独立的保护：

- 块写在 bottom gate 内进行，`bound_ts = checkpoint_ts()`，覆盖 gate 内能看到的全部行；gate 期间提交的 append 排在它之后。
- 索引 dump 在 gate 外进行，但它读的是**当时的 tail**——若第一阶段后、第二阶段前有 append 落进 tail，那一批行会被这次 dump 一并封进 chunk，而它们的块文件要等下一次块写。这不是丢数据：`dump_index_transaction` 的 SPANN/全文分支都从**块文件或 tail** 取行，而 `version.created` 只声明已写进块的行；tail 里的新行仍由 `pending_rows` 覆盖（§4.5 的 T1）。

所以顺序是「先块、后索引」，而不是反过来：反过来（先封索引、再写块）才会让 chunk 声明了块文件还没有的行。

现有的 `dump_under_pressure` 也是两段式：先 `dump_all_sealed_blocks(only)`（内含 gate），再在**没有 gate** 的情况下调 `dump_every_table_index` / `dump_largest_memory_index`（`index_build.rs:4383-4392`）。本方案的 `dump_table` 照抄这个形状。

两点与现有路径不同：

1. **一旦选中一张表，就排空它的所有索引 tail，而不是只排最大的那个。** 现有 `dump_under_pressure` 是 `hard` 才 `dump_every_table_index`、`soft` 走 `dump_largest_memory_index`（`index_build.rs:4383-4392`）。选表判据是**整表足迹**，只排一个索引会让"这张表被选中"与"这张表真的被清空"脱节：剩下的 tail 仍计入 `bytes`，下一轮它还会被选中，于是同一个索引被反复挑中而其余永远不动。
2. **进度信号必须按表取且覆盖两块内存，三种"现成"的取法都错。**

   **不能用 `unsealed_footprint()` 差。** 它的 `entry.memindex_bytes` 是缓存（`state.rs:4958`，初始 0，`CREATE INDEX` 路径不刷新它），而 `recompute_memindex_total()` 会把它校正到当前值。于是差值有两个方向的错：一张刚 `CREATE INDEX` 的表真被排空了却报 0（缓存本来 0、之后仍 0），一张缓存虚高的表什么都没释放却报非 0（校正跳变）。

   **也不能用 `context.memindex_total`（引擎级全局原子）差。** 同一进程里写路径的 `dump_under_pressure`（`index_build.rs:4383`，从 commit bottom 调用，`statements.rs:841`、`:876`）与后台 `DumpMemIndexTask`（`state.rs:10256`）每次成功都会 `recompute_memindex_total()`（`index_build.rs:4405`）把全局值降下来。于是"回收循环在试一张必然失败的表，同时另一个 commit 排空了别的表"会让 `dump_table` 误报成功——退避永不累积。

   **也不能只用 `memindex_bytes_of` 差。** 它不含 append buffer，所以一张没有索引的表再怎么成功块写也报 0（见上）。

   正确做法是 §4.3 伪代码里的 `unsealed_live(&key)` = `unsealed_bytes() + memindex_bytes_of(&key)`，前后各取一次，两者都是当场值、都不经缓存。**不要用 `has_unsealed` 的前后在假做信号**：它的第三项是 §4.0 的原子标记，那个标记按设计允许"宁多勿少"，所以"标记仍非 0 但实际已清空"是允许的中间态——拿它判进度会把一次成功记成失败。

   两份判据因此各有分工：`has_unsealed` 决定**进不进候选集**（允许保守），`unsealed_live` 前后差决定**这次是否算进展**（必须精确）。`last_write_ms` 只在后者显示变小、且该表已无 tail 时才写 `now`；否则只写 `last_attempt_ms`。

`write_blocks_bounded_locked` 要求调用者已持有该表的 bottom gate，所以走 `run_commit_bottom`（与 `dump_all_sealed_blocks` 同一条路，`index_build.rs:4446`）。

`dump_every_table_index(Some(table))` 已经实现了"扫 catalog、跳过无 tail 的索引、逐个 `dump_index_transaction`"（`index_build.rs:4545`），直接复用。

#### 表名形态：传 `table_key` 化的 `db.table`

`key` 直接取自 `tables` 的键，**永远是 `db.table`**（`table_key` 补全过的形态），原样传给下游，不做二次限定。

依据是每个下游函数自己都会 `table_key` 化，而 `table_key` 对已带 `.` 的名字只做小写化、不再补库名（`statements.rs:1500-1506`）：

| 函数 | 表名处理 |
|---|---|
| `run_commit_bottom(table)` | `self.table_key(table)`（`statements.rs:1303`） |
| `write_blocks_bounded_locked` → `write_blocks_mode_with` | `self.qualified_table(table)`（`engine.rs:634`） |
| `dump_every_table_index(Some(t))` | `self.table_key(&im.table) == wanted`（`index_build.rs:4550`） |
| `dump_index_transaction` → `dump_version` | `self.table_key(table)`（`index_build.rs:990`） |
| `import_in_flight(table)` | `self.table_key(table)`（`statements.rs:890`） |

两者对完整名字**都不补库名**，但它们并不完全等价：`table_key` 会 `to_ascii_lowercase()`（`statements.rs:1502`），`qualified_table` 不会（`engine.rs:245`）。因为候选 `key` 取自 `tables` 的键、本来就是小写，所以传下去两边得到同一个字符串。**关键是不能传裸名**：`qualified_table` 用 `self.current_db`（`engine.rs:247`）、`table_key` 用 `self.db()`（会话优先），两者都可能被 `USE` 影响，而后台任务没有会话。传完整的 `db.table` 让这条路径与会话状态无关。

### 4.4 写路径的选索引

写路径（`enforce_memindex_quota(Some(t))`、`enforce_quota_after_append`）的 `only` 永远是具体表名，候选集就是该表的索引。

**当前实现不是"选最大"。** `pick_seal_candidate` 把每个候选交给 `SealScheduler::pick`，而 `SealScheduler` 的权重按 `tenant` 聚合——tenant 取 `im.table.split('.').next()`，即**库名**（`index_build.rs:4517`）。`weight` 每个库一个条目，`round` 是这些条目的和；每次 `pick` 给每个库的 `credit` 加上它自己的字节数、赢家扣掉 `round`。

写路径的候选集是**一张表的索引**（`only` 永远是具体表名），它们必然同属一个库。于是 `weight` 只有一个条目，`credit` 的加减永远相互抵消，所有候选的 `score` 相同。`pick` 退到第二个比较项 `waited = now - last_dump`，字节数完全不参与。

这与部署里有多少个库无关：`CREATE DATABASE a; CREATE DATABASE b;` 可以并存（`sessions_keep_their_own_database` 覆盖了这一点），但一次 `pick` 的候选集只落在一个库里，多库并存改变不了「权重只有一个条目」这个事实。

实测（`SealScheduler::new(1)`，每次 pick 后 `done` 释放槽位）：

| 场景 | 结果 |
|---|---|
| 一张表两个索引 100 MB / 1 MB，`last_dump` 固定 | 列表里排第一的那个被选 9 次，另一个 0 次 |
| 同上，赢家的 `last_dump` 每轮前进（真实路径） | **3 / 3，完全轮转** |
| 拆成两个"库"的同两个候选 | 100 MB 6 次、1 MB 0 次（第三个候选分走 3 次） |

第二行是真实行为：**纯轮转**，100 MB 与 1 MB 得到相同次数。既不是"选最大"，也不是它文档里写的按字节加权——后者只在多库下成立。

**改动**：删除 `SealScheduler` / `SealCandidate` / `SEAL_STARVE_TICKS` / `index_seal_starvation`，写路径改为按 `bytes` 降序、平局取 `last_dump` 更早的那个（`index_build.rs:4624` 已有 `last_dump_ts`）。候选集是一个表的索引，维度只有一个，不需要公平队列。

小索引的饥饿由周期路径覆盖：一旦某张表被选中，`dump_every_table_index` 会排空它的**所有**索引 tail（§4.3），不看大小。

`the_memindex_quota_seals_one_hnsw_chunk_per_batch` 只有单个索引，它断言 `2..=3` 个 chunk（`tests_ut.rs:10455-10458`，注释说明下限是 2、落点取决于检查运行时驻留多少）。单索引下候选集只有一个元素，改为"选最大"不改变候选集，因此这个区间不变。

#### 小索引不会被饿死：每一类索引都有自己的行数上限

"选最大"只决定**顺序**，不决定**是否**。每个索引另有一条独立的行数上限，与字节配额无关，写路径每批检查：

| 索引 | 写路径行数上限 | 位置 |
|---|---|---|
| HNSW | ✅ `len() >= mem_index_capacity`（默认 65536） | `dump_hnsw_over_capacity`（`index_build.rs:4762`），调用点 `dml.rs:1090`、`:2961` |
| 二级索引 | ✅ `row_count() >= mem_index_capacity` | `secondary_columns_over_capacity`（`index_build.rs:5014`），调用点 `dml.rs:1091`、`:2962` |
| SPANN | ✅ `rows >= mem_dump_threshold(opts)` | `dump_spann_over_capacity`（`index_build.rs:4923`），调用点 `dml.rs:1164`、`import.rs:790` |
| 全文 | ✅ `mem_used() >= text_segment_bytes()` 或 `pending_docs() >= capacity` | `should_seal`（`text_index.rs:1942`），调用点 `dml.rs:2441` |
| **IVF** | ❌ 无 | `ivf_mem` 只由 `note_ivf_insert` 追加；清空点只有 `seal_index_tail`、checkpoint、`admin.rs:812` |
| **BMP / sparse** | ❌ 无 | `seal_sparse_index` 的调用点只有 `admin.rs:476`（compaction）与 `index_build.rs:1561`（dump） |
| **多向量 PLAID** | ❌ 无 | 只有 `tail_bytes` 计费（`index_build.rs:4724`） |

HNSW / 二级 / SPANN / 全文这四类不会无限增长：攒够上限就自己封一次。字节配额与 RRF 排序解决的是"内存总量没有上界"，不是"某个索引没有上界"。

**IVF、BMP、PLAID 三类没有行数上限**，只能靠配额压力或周期路径排空。它们必须依赖周期路径；`unsealed_max_tables_per_round = 100` 的默认值保证一轮能覆盖相当多的表。§9 里列为未决的是"是否要给它们补行数触发"，判据见 §9。

### 4.5 `last_write_ms` 与并发安全

`TableEntry` 目前没有"最后一次写入"的时间，需要新增：

```rust
pub(crate) struct TableEntry {
    /// 最后一次 append 的单调毫秒（`elapsed_millis()`）；0 = 本进程内从未写过。
    /// 最后一次改变 unsealed 内容的操作（append / delete / compaction），用时基见下，
    /// **与 `mirror_epoch += 1` 在同一处更新**（现有 7 处，见 §4.5）。
    /// 排水成功后设为 `now`，不清零。读路径不碰。
    pub(crate) last_write_ms: u64,
}
```

三条实现约束：

1. **与 `mirror_epoch += 1` 在同一位置更新。** 全部 7 处（行号是 `mirror_epoch += 1` 那一行，`mirror_dirty = true` 在其上一行）：

   | 位置 | 所在函数 | 语义 |
   |---|---|---|
   | `statements.rs:1461` | `invalidate_block_mirror` | compaction / schema 变更让块必须重写 |
   | `statements.rs:2187` | `reserve_rows_inline` | 分配行号并进入 append 日志 |
   | `statements.rs:2264` | `reserve_import_rows_inline` | IMPORT 的行号分配 |
   | `dml.rs:330` | `note_append` | append 日志的 ts 落定 |
   | `dml.rs:454` | `note_deleted_ids` | **删除**，不是 append |
   | `blocks/engine.rs:1804` | `refresh_block_mirror_rows` | 行真正进入 append buffer |
   | `blocks/engine.rs:3528` | `apply_replayed_rows` | WAL 重放把行放回 buffer |

   两者并排写就从结构上排除了"只更新一个"的漏写——`last_write_ms` 不是新造的信号，而是既有写入信号的另一个投影。

   **因此 `last_write_ms` 的定义不是"最后一次 append"，而是"最后一次改变 unsealed 内容的操作"**，包含删除（`dml.rs:454`）与 compaction（`statements.rs:1461`）。理由是这三类都会让 `version` 或块文件需要重写，与 append 一样让"这份 unsealed 数据是否还新"发生变化。定义若是"最后一次 append"，一张刚被删掉一半的表就会被判成"很久没写"。

   `blocks/engine.rs:1804` 与 `:3528` 这两处是最容易漏的：前者是 `buffer_rows` 真正填充 buffer 的地方（`engine.rs:1797-1804`），后者是 WAL 重放。漏了它们，重放之后的表 `age` 会算成"从未写过"。
2. **排水成功后设为 `now`，不是清零。** `0` 的语义是"本进程内从未写过"，清零会让刚被回收的表在下一轮立刻又满足超龄。
3. **重启后置为启动时刻。** 恢复出来的 tail 没有可用的写入时间；若沿用持久化的 `dump_ts`，重启后第一次扫描会把所有 tail 都视为陈旧，造成启动即集中 dump。`recover_mem_index` 完成时把 `last_write_ms` 设为进程启动的单调毫秒，"闲置"从**能观察到的时间**算起。

**时基：`elapsed_millis()` 现在是 `memory.rs` 的私有函数**（`memory.rs:121`，无 `pub`），而 `TableEntry` 在 `engine/state.rs`、自增点跨 `statements.rs` / `dml.rs` / `blocks/engine.rs`。实现第一步要把它提升为 `pub(crate)`（或加一个 `Engine::elapsed_ms()`）。它本身以首次调用为 0 点并 `+1`，所以"进程启动的单调毫秒"就是它的 0 点，不需要额外的基准。

**失败不更新 `last_write_ms`，下一轮重试。** `dump_table` 因 import in flight 或写盘失败而释放 0 字节时，`last_write_ms` 保持不动——那张表仍然是"最老的"，下一轮还会第一个被选中。这是有意的：陈旧路径的判据是"该表有数据且很久没动"，一次失败不改变这个事实。

代价是一张持续失败的表会每轮都被重试。**但"永远排第一"会让它把后面的表挤出轮次上限**：持续失败的表 `age` 最大、恒居第一，若这类表 ≥ `unsealed_max_tables_per_round`，每轮都只轮到它们，健康表永远排不进去（§4.4 的 SPANN 空洞就是必然持续失败的一类）。

所以失败的表不只要"不前进"，还要**排到队尾**。但**不能**把"上次尝试"的时间戳简单地刷成 `now`：

- 排序键若是 `now - max(last_write_ms, last_attempt_ms)`，而每次失败都把它刷成 `now`，那么对一张闲置表（`last_write_ms` 永不动），`max` 恒等于 `last_attempt_ms` 恒等于"刚刚"，`age ≈ 0` **永远**成立。失败表于是被永久钉在队尾——这正是它要防止的饥饿，只是换了个方向。

改成**连续失败计数 + 指数退避**：

```rust
pub(crate) struct TableEntry {
    /// 连续失败次数；成功一次或表被写入则清零。
    pub(crate) dump_failures: u32,
}

// 压力路径的排序基（Stale 用 last_write_ms，见 §4.1）
sort_base(t) = t.last_write_ms.max(t.last_attempt_ms)
// 只有当 now - sort_base(t) > idle_age 时才允许再试
```

| 规则 | 值 |
|---|---|
| 失败时 | `dump_failures += 1`；`last_attempt_ms = now` |
| 成功时（`unsealed_live` 变小且已无 tail，§4.3） | `dump_failures = 0`；`last_write_ms = now` |
| 写入时 | `dump_failures = 0`（表又有新数据，值得重试） |
| 压力路径的候选门槛 | `dump_failures == 0` → 进；否则 `now - sort_base >= max(unsealed_idle_age, unsealed_reclaim_interval) * 2^min(dump_failures, 6)` |

**门槛只在有失败时生效。** `dump_failures == 0` 时任何 `has_unsealed` 为真的表都进候选集——这正是缺口 B 要修的：一张刚写完、把总量顶过 `soft` 的大表必须能被压力路径选中（回归用例 `a_quota_breach_on_one_table_drains_an_idle_one` 覆盖了"老表"，还要覆盖"刚写的表自己"）。

**基数取 `max(unsealed_idle_age, unsealed_reclaim_interval)`，不是 `idle_age`。** 两个原因：`unsealed_idle_age = 0` 时 `0 × 2^n = 0`，退避会完全失效；而 `idle_age` 按 §5 是"纯阈值，不触发任何东西"，让它当退避基数会让 `0` 意外关掉一套机制。

指数退避让第 n 次失败要等 `基数 × 2^n`（上限 `2^6 = 64` 倍；默认基数 600s 即约 10.7 小时）才再试一次。失败表即使永远失败也只占每十几小时一次的机会，不会挤掉健康表；任何一次成功写入都会把计数清零、立刻回到正常队列。

**三条必须一起满足：**

| 约束 | 原因 |
|---|---|
| `last_attempt_ms` 只在 `dump_table` 返回 `false` 时写 | 部分成功（`unsealed_live` 已变小）不该被降级 |
| 用 `max(...)` 而不是覆盖 | `last_attempt_ms` 可能早于 `last_write_ms`（失败后又被写入），`max` 保证 `age` 不倒退 |
| 陈旧路径（`Stale`）**不**用它 | 陈旧路径的语义是"该表多久没动"，与"上次尝试排水"无关；只有压力路径的排序用它（§4.1 把 `age_rank` 参数化） |

**不加"已 dump"标记。** 判据（`has_unsealed` ∧ 年龄排序）已充分；加第三态只会在两个字段间引入不一致的可能——它们表达的是同一件事。

#### 排水期间有新 APPEND

不新写 flush，复用 `write_blocks_mode_with` 的三道防线：

| 防线 | 位置 | 作用 |
|---|---|---|
| `mirror_epoch` | `blocks/engine.rs:757-763` 读、`1455-1456` 比对 | 写文件前读 epoch，写完后若变了就留 `mirror_dirty = true`，下一次不走 "nothing moved" 短路 |
| 按写入 id 集裁剪 | `blocks/engine.rs:1441-1443` | `retain_buffered` 的谓词是「seq 不在 `written_ids` 里」—— 按 `block_rows`（真正写进块的集合）裁 |
| `pending_bytes` 同步 | `state.rs:4838-4842` | `retain_buffered` 内部按差值修正，配额账目不会虚高 |

两个反例就写在 `engine.rs:1441` 的注释里：**按输入集清**会丢掉窗口没覆盖到的行；**清空整个 buffer** 会丢掉并发提交插进来的行。

三条时间线：

**T1 — 排水期间有新 APPEND（并发）**

```
t0  循环选中表 T，准备 flush
t1  flush 取 bottom gate，读 epoch = N
t2         ← 一个 INSERT 提交，epoch 变 N+1，新行进 buffer
t3  flush 写完块文件，裁剪：只删它写进 block_rows 的那些 id
t4  比对 epoch(N) != N+1 → mirror_dirty = true
```

新行留在 buffer 里，`mirror_dirty` 保持 true，下一轮仍看到 `pending_rows` 非空。不丢数据，也不会误判"已落盘"。

**T2 — 排水之后又 APPEND**

```
t0  flush 完成，last_write_ms := now
t1        ← 新 INSERT，last_write_ms := now
t2  下一轮扫描：now - last_write_ms < idle_age → 不选它
```

新写入推迟回收，正是想要的（刚写过不算闲置）。

**T4 — 第一段（块写）之后、第二段（索引 dump）之前落地的 APPEND**

```
t0  第一段完成，gate 释放，pending_rows 仍是空
t1        ← 一个 INSERT 提交：mirror_epoch += 1，新行进 buffer
t2  第二段开始：dump_every_table_index 把**这些新行**的索引条目一起封进 chunk
t3  这些行的块文件还不存在（mirror_dirty == true）
```

结果：**不丢数据，但索引 chunk 覆盖的行集合可以大于块文件的行集合。** 读侧靠"`mirror_dirty` 时读者必须 merge buffer"（`state.rs:5115-5124`）拿到这些行的 payload；下一轮第一段把它们写进块。

因此有一条硬约束：**任何清 `pending_rows` 的动作都必须以块写为准，不能以"索引已封"为准。** 否则 T4 里那批行的 payload 就没了。

**T3 — 排水完成后一直没人写**

```
t0  flush 完成，last_write_ms := now，pending_rows 与索引 tail 都空
t1  下一轮扫描：footprint == 0 → 不进候选集
```

不会重复 flush。判据第一项必须是"有 unsealed 数据"，年龄只是排序维度；顺序反了会让每张空表都进候选集。

### 4.6 块文件层面的追加语义

被写下去的块文件在第二次 dump 时发生什么。**重写版不实现字节追加，而是整块重写 + `version` 重算**，因此本层不需要任何"追加偏移"逻辑，也不存在"第二次 dump 截断前 100 行"的风险。

#### 实测（`INSERT 100 → checkpoint → INSERT 100 → checkpoint → 重启 → INSERT 100 → checkpoint`）

```
dump 1  version = [(create_ts 10, 100)]
        文件     = [0.col=33816, 1.col=132120, col_1_out=0, version=65556]

dump 2  version = [(10, 100), (14, 200)]                       ← 同一个 blk_0，多一个 create
        文件     = [0.col=33816, 1.col=132120, col_1_out=0, version=65572]   ← 列文件长度不变

重启    n = 200                                                 ← 恰好 200 行，无重无漏

dump 3  version = [(10, 100), (14, 200), (20, 300)]
```

三点观察：

1. **不新建 `blk_1`。** 块号只由段内偏移决定（`(seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY`，`blocks/column.rs:500`），追加的 100 行落在偏移 100..200，仍是 `blk_0`。参考的 `AppendInBlock` 也是同一个块（`new_txn_data_impl.cpp:922`）。
2. **`version` 是重算，不是累加。** `block_version_batches` 从**持久化的 append 日志**（`append_totals` + `append_ts`）重新推导整条 create 列表（`blocks/engine.rs:1471`），不是"内存里已 flush 行数 + 新增行数"。同一状态重复 dump 产出完全相同的字节，没有可漂移的计数器。
3. **未触碰的块不重写。** 回收走 `partial`（`write_blocks_bounded_locked` 传 `full = false`，`engine.rs:408-410`），`seg_partial` 分支对 `!touched` 且非 staged 的块只重发 catalog 记录、沿用原有 ts（`blocks/engine.rs:1017-1046`）。`touched` 来自本次 buffer/`extra` 的行 id（`engine.rs:843`），所以一次回收重写的是**这些行落在的块**——通常 1 个，若 buffer 里的行跨两个 block 窗口就是 2 个以上。不是无条件"只写一个块"。

读取侧靠 `version.created.last().cumulative_row_count` 决定读到第几行（`blocks/engine.rs:4785-4789`），列文件里超出有效行数的槽位被忽略——这也是列文件长度不随行数变化的原因。

#### 与参考的对照

| | 参考 | 重写版 |
|---|---|---|
| 追加位置 | `AppendInColumn` 往 mmap 的列 buffer **原地追加**（`new_txn_data_impl.cpp:960-1000`），随即整份 buffer 重新持久化 | 整份 block 重新编码，temp + `fsync` + `rename`（`definition/format.rs:510`） |
| 写盘字节数 | **整个 block**：`GetColumnSize` 忽略传入行数、固定返回 `capacity * type_size + bitmap`（`column_meta_impl.cpp:240-249`），`DataFileWorker` 再把 `buffer_size - data_size` 补零（`data_file_worker_impl.cpp:105-109`） | 同样**整个 block**（`ColumnBody` 按容量大小写出） |
| 并发读者的保护 | `BlockLock`：每个 block 一把 `shared_mutex`（`new_txn_data_impl.cpp:939`） | `rename` 换 inode：旧 mmap 仍指向旧文件，读到的是一致快照 |

**两者都不只写追加的那段字节。** 实测列文件长度三次 dump 完全一致，正是因为双方都按容量写满。差异只在**怎么得到新内容**（原地改 buffer vs 重新编码整块）和**怎么保证读者看到一致快照**（block 锁 vs rename）：

- 容量固定（8192 槽），有效行数由 `version.created.last().cumulative_row_count` 决定，超出的槽写零且不被读——无 correctness 差异。
- 写放大与参考同量级：成本按块大小计，不按新增行数计。
- 不需要 block 级读写锁，也没有"追加中途崩溃留下半块"的恢复路径——`rename` 之前旧文件一直完整可读。代价是每次重新编码整块（一次 CPU 副本）。

对排水循环的要求：不复用自己实现的追加（用 `write_blocks_mode_with` / `dump_every_table_index`，它们已处理块号计算、`version` 重算、未触碰块的短路）；成本按块大小估；一次只动一张表。

### 4.7 两个触发点的节拍

每 tick（1 秒）：

1. `Watermarks::pressure(budget_bytes())` 判定（**纯读，不 retune**；见 §4.2）
2. 非 `Normal` → `reclaim_unsealed(now, Bytes(soft))`（`import_in_flight` 的表被排除在候选集之外，不影响其他表）

每 `unsealed_reclaim_interval`（默认 30 秒，是 tick 的整数倍）：

3. `reclaim_unsealed(now, Stale(unsealed_idle_age))`

**压力优先，且共享同一次 `Watermarks::pressure()` 读。** 两者节拍不同（1 秒 vs 30 秒），一个 30 秒点撞上压力时会连续做两件事。正确做法不是跳过，而是共享判定：扫描先看压力（纯读），超限时只做压力那一轮并把这次扫描记作"本周期已处理"，否则再做陈旧度。这样一次 30 秒点最多产生一轮排水。

实现即 `tasks.rs` 的 `if pressure.0 != Pressure::Normal { … } else if due_stale { … }`：压力那支无论 `due_stale` 是否成立都先把 `last_reclaim` 推到 `now`，所以陈旧路径不会在同一个 tick 里紧接着再跑一轮。`reclaim_interval` 作为参数传入而**不是**从 `Context` 读，是为了让传四个 `Duration::ZERO` 的单测（`bg_task_ut.rs:148`）保持惰性：`0` 直接关掉陈旧路径。

**同一张表在一轮里被重复处理是可接受的。** 压力路径与陈旧路径可能选中同一张表；`write_blocks_mode_with` 的 `!dirty && block_index 非空` 短路（`blocks/engine.rs:737-745`）与 `dump_index_transaction` 的"无 tail 就跳过"（`index_build.rs:1254`）都会让第二次调用空转。不为此加锁或去重。

**两个 `reclaim_unsealed` 并发调用会串行，不会重入。** `run_commit_bottom` 的 `HeldBottom` 标记是 **task-local**（`statements.rs:1304`），只对"同一条 task 上的嵌套调用"短路。压力路径与陈旧路径若真的并发跑（不同 task），同一张表的 gate 仍会串行：无 bottom pool 时走 `gate.lock().await`（`statements.rs:1310-1312`），有 pool 时由 `BottomWork::gates` 在 worker 上加锁（`state.rs:6584`、`:6634-6635`）。于是两次 `dump_table` 串行执行、第二次空转。这也是为什么本方案不需要一把引擎级的回收锁。

但要注意：**同一个 tick 内两条路径是顺序执行的，不会并发**（§4.7 的"共享同一次 `pressure()` 读"）。真正的并发只可能来自另一个进程或未来的第二个触发源。

**tick 本身不动。** `trigger_loop` 的 1 秒节拍（`engine/tasks.rs:95` 的 `TICK`）与参考的 `periodic_trigger_thread_impl.cpp:50`（`milliseconds(1000)`）一致，它承载的四个步骤都很轻：

| 每 tick 无条件执行的步骤 | 成本 |
|---|---|
| `memory_tick()` | `cgroup_snapshot()` 有 5 秒缓存（`CGROUP_REFRESH_MS = 5_000`），命中时是一次锁 + 一次比较 |
| `has_spann_fixups()` | `tables` 锁 + 遍历 `spann_mem`，无缓存但只是指针遍历 |
| 四个 `IntervalTrigger` 式的间隔判定 | 各读一次 `Instant::elapsed()` |

四者间隔都 ≥ 10 秒（`optimize=10s` / `cleanup=60s` / `compact=120s` / `checkpoint=86400s`），1 秒只是判定粒度。改 tick 会改变它们的触发精度（tick=N 秒 → 最坏延迟 N 秒），收益只是省掉每秒一次的空转。

新扫描本身是 **O(表数) 的内存加法 + 一次 `list_indexes()`**（§4.0）。`list_indexes()` 是 RocksDB 前缀扫描，代价随索引总数增长而与表数无关；压力路径每 tick 跑一次仍可接受 **[推算]**，但它是这条路径上唯一的 I/O，若实测偏重就改为按 `unsealed_reclaim_interval` 节流。陈旧路径无论如何都需要自己的间隔，因为它会真的产生 dump。

## 5. 配置

三个新键：

```toml
[buffer]
# 陈旧度扫描的周期（唯一的定时器）。0 关闭这条路径。
unsealed_reclaim_interval = "30s"
# 陈旧路径的年龄阈值；也是压力路径失败退避的基数（与 interval 取 max）。
unsealed_idle_age         = "600s"
# 一轮排水最多动几张表。0 = 不限。
unsealed_max_tables_per_round = 100
```

| 项 | 默认 | `0` 的含义 |
|---|---|---|
| `memindex_memory_quota` | 1GB（`conf/infinity_conf.toml:54`；代码默认 4 GiB，`config.rs:157`） | 关闭配额机制（压力路径随之失效） |
| `unsealed_reclaim_interval` | `30s`（新增） | 关闭陈旧路径（压力路径仍由 `memindex_memory_quota` 控制） |
| `unsealed_idle_age` | `600s`（新增） | 陈旧路径：**任何有 unsealed 数据的表都算超龄**，即尽快全量回收（最激进）。它同时是压力路径失败退避的基数，但与 `unsealed_reclaim_interval` 取 `max`，所以 `0` 不会关掉退避 |
| `unsealed_max_tables_per_round` | `100`（新增） | 不限，一轮排到目标为止 |

两条路径各由自己的既有配置控制，互不牵连。`unsealed_idle_age = 0` 的含义是"不设年龄门槛"（全部算超龄），要关闭陈旧路径应设 `unsealed_reclaim_interval = 0`。

默认值的依据：`unsealed_idle_age / unsealed_reclaim_interval = 600 / 30 = 20`，即最坏延迟是回收目标的 5% **[推算]**，见 §9。

**`unsealed_max_tables_per_round` 默认 100。** 它是两道护栏：不长时间占用 `flush` 串行化，以及不阻塞 `trigger_loop` 的其他四步——`trigger_loop` 是**单一顺序任务**（`tasks.rs:88-158`），checkpoint / `memory_tick` / optimize / compact / cleanup 都在同一条 `.await` 链上。`cap = 0` 表示单轮没有时间上界，10000 张有 tail 的表可能跑数分钟，期间 checkpoint 与 cleanup 都不会触发。**只用于测试或明确知道表数很少的部署；生产环境不要设 0。**

配置接线：`crates/cli/src/config.rs`（解析）+ `main.rs`（传给引擎）+ `Engine::set_unsealed_reclaim_interval` / `set_unsealed_idle_age` / `set_unsealed_max_tables_per_round`。

## 6. 必须保持不变

| 约束 | 原因 |
|---|---|
| **写路径的触发间隔一个都不动** | `the_memindex_quota_seals_one_hnsw_chunk_per_batch` 断言 `2..=3` 个 chunk（`tests_ut.rs:10455-10458`），依赖"一次 breach 排一个索引"；§4.4 只改"排哪一个"，候选集不变，区间不变 |
| **`budget_bytes()` 的记账不变** | 新增的 `unsealed_footprint()` 只用于选表排序，不并入 `budget_bytes`，否则与 `memindex_total` 重复计数 |
| **`soft` / `hard` 的推导与更新频率都不变** | 公式仍是 `max(1, min(64MiB, budget/4))` / `max(soft, max(1, min(256MiB, budget/2)))`，`budget == 0` 时都置 0（`maintain.rs:74-87`）；`retune` 仍只由写路径的 `seal_pressure()` 驱动，tick 上的判定改用纯读的 `Watermarks::pressure()`（§4.2） |
| **`FLUSH DATA` 语义不变** | `flush_dumps_mem_index` 仍只控制 `Engine::flush`；周期路径不读这个标志 |
| **IMPORT 中的表单表不排水** | 逐表判断（`statements.rs:886-892` 的 `import_in_flight(table)`），只把那张表移出候选集；其他表照常回收。`dump_index_transaction` 自身的检查（`index_build.rs:1244`）仍保留作二次防护 |
| **单元测试不受影响** | 唯一调用 `start_background_tasks` 的单测（`bg_task_ut.rs:148-154`）传的是四个 `Duration::ZERO`，因此新增步骤也不会触发 |

**需要实现时确认的风险**：跨引擎字节门禁如果起服务端且把配额设得很小，压力路径会比现在多出 dump，字节比对可能变化。实现第一步就确认门禁用的是 `maintenance` 模式还是完整服务。

## 7. 交付项

| # | 产出物 | 验收 |
|---|---|---|
| 1 | `TableEntry::last_write_ms`（表级 `unsealed_footprint` 见 §4.0）+ 前者与 `mirror_epoch += 1` 同处更新（7 处），重启时置为启动时刻 | 一次 INSERT 后前进；7 个点都更新（含 `blocks/engine.rs:1804`、`:3528`）；重启后不是 0 |
| 2 | `rrf_order(candidates)`：两路名次 + `K = 3` + 确定的平局规则 | 单测：构造"大而新"与"小而老"两组，断言年龄维度不被尺寸抹平；平局时按字节再按表名 |
| 3 | `Engine::reclaim_unsealed(now_ms, target)`：候选集（逐表排除 import）、`has_unsealed` 判据、融合排序（`sort_base` 按 target 分叉）、逐个 dump（**两段式**：块写在 `run_commit_bottom` 内、索引 dump 在其外）、两个 target、`unsealed_max_tables_per_round`（0 = 不限）、退避原子 | 单测：`Bytes` 目标按 `budget_bytes()` 退出；`Stale` 排空不超过上限的超龄表（0 时全排）；`cap = 0` 不限；无进展时退避 |
| 4 | 压力路径接入：tick 上 `Watermarks::pressure() != Normal` → `reclaim_unsealed(now, Bytes(soft))` | 单测：只写 A 表把总量顶过 `soft`，B 表（更老）被一起回收；**A 表自己也必须在候选集里** |
| 5 | 陈旧路径接入：`unsealed_reclaim_interval` 到点 → `reclaim_unsealed(now, Stale(idle_age))` | 单测：配额设大、`idle_age` 设小，tail 与 append buffer 都被落盘 |
| 6 | 配置接线（`config.rs` / `main.rs` / `conf/infinity_conf.toml`），三项 | 配置文件里能读到；`interval = 0` 关陈旧路径、`max_tables_per_round = 0` 表示不限、`idle_age = 0` 表示不设年龄门槛 |
| 7 | `index_memory_bytes` 补 SPANN 分支（`index_has_memory_tail` 的 SPANN 兜底 `true` **保留**，见 §4.0）；新增 `has_unsealed`：**七个 registry 里只有 secondary 与 `ivf_mem` 的计数是无锁的**，其余五个（HNSW / 全文 / BMP / 多向量 / SPANN）的计数都在各自内部锁之后，所以要新增 per-index（或 per-table）的 `Arc<AtomicU64>`，由写钩子加、由各 dump 分支清零（§4.0 的表）；`Watermarks::soft()` / `hard()` 去掉 `#[cfg(test)]` + 新增纯读 `pressure()`；`elapsed_millis` 提升为 `pub(crate)` | 单测：SPANN tail 非空时 `index_memory_bytes` 非 0、`has_unsealed` 为真；一张被并发查询的 HNSW/全文表仍被判为有 tail；尾非空时原子非 0、dump 之后原子为 0；tick 上的判定不改变 `soft` |
| 8 | `TableEntry::last_attempt_ms` + `dump_failures`；压力路径的 `sort_base = max(...)`，候选集门槛 `max(idle_age, interval) * 2^min(failures,6)`（`failures == 0` 时不设门槛）；`Context` 退避原子 | 单测：连续失败 3 次的表本轮不进候选集、但它被写入后立刻回到队列；成功一次清零 |
| 9 | 删除 `SealScheduler` / `SealCandidate` / `SEAL_STARVE_TICKS` / `index_seal_starvation`，写路径改为按 `bytes` 降序、平局取更早的 `last_dump` | 三个多库单测一并移除；`the_memindex_quota_seals_one_hnsw_chunk_per_batch` 的 `2..=3` 区间不变 |
| 10 | `GAPS.md` §1 登记「周期回收 + 融合排序是与参考的有意偏离」 | 文档有对照表 |

每一项的验收只跑相关单测（见 §8）；全量测试留到全部完成之后。第 9 项删代码，验收是把 `cargo test -p executor --lib` 跑一遍确认没有别的用例依赖轮转顺序。

## 8. 验证策略

按「逐项只跑 quality/unit，全量留最后」推进：

- 单项：`cargo test -p executor --lib <name>` + `cargo clippy --workspace --all-targets` + `cargo fmt --check`
- 全量（全部 10 项完成后一次）：`cargo test -p executor --lib`（现状 959 个用例 / 约 12 分钟）
- 门禁：字节门禁与 Gate C 只在最后跑一次；重点看服务端模式下有没有多出的 dump 记录

**回归测试**：

| 测试 | 断言 |
|---|---|
| `rrf_prefers_a_table_that_is_both_large_and_old` | 三张表：大而新、小而老、中等且中等；融合后中等那张排最后 |
| `rrf_ties_are_broken_by_age_then_bytes_then_name` | 同分时年龄先决，然后字节，最后表名；顺序确定且可重复 |
| `rrf_breaks_a_symmetric_tie_toward_age` | "最大且最新"与"最小且最老"同分，年龄赢下平局（按字节定序会让大表胜出，此测试会失败） |
| `a_quota_breach_on_one_table_drains_an_idle_one` | A 表写入顶过 `soft` 后，B 表（更老、更小）被一起排空 |
| `the_table_that_crossed_soft_is_itself_a_candidate` | A 表刚写完、年龄 0s，仍进候选集并被选中（缺口 B 的核心） |
| `an_append_between_the_two_phases_keeps_its_payload` **[待实现]** | 两段之间插入一行，查询仍返回它；`pending_rows` 未被清 |
| `a_table_is_a_candidate_while_its_index_is_being_read` | 一张被并发查询的 HNSW/全文表仍被判为有 tail（`try_lock` 探测会漏，此测试会失败） |
| `a_concurrent_dump_of_another_table_does_not_mark_this_one_done` **[待实现]** | 回收循环试一张必然失败的表，同时另一张表被 dump：失败表计数仍递增（引擎级信号会误判，此测试会失败） |
| `repeated_failures_back_off_exponentially` | `unsealed_idle_age = 0` 时失败表仍按 `interval × 2^n` 退避 |
| `reclaim_stops_once_the_total_is_below_soft` | `Bytes` 目标下，排空第一张后总量已达标 → 不碰第二张 |
| `reclaim_honours_the_round_cap` | `unsealed_max_tables_per_round = 2`，5 张表都超龄，一轮只动 2 张；下一轮继续 |
| `a_zero_round_cap_means_no_limit` | `unsealed_max_tables_per_round = 0`，5 张超龄表在一轮内全部排空 |
| `reclaim_skips_a_table_an_import_is_staging_into` | 表 A 在 IMPORT 中间，候选集不含 A，排在 A 后面的表照常被回收 |
| `reclaim_retries_a_table_whose_dump_released_nothing` **[待实现]** | 第一次 `dump_table` 返回 0（模拟 I/O 失败），`last_write_ms` 未前进，下一轮仍选中它 |
| `an_indexless_table_reports_progress_when_its_buffer_drains` | 一张**没有索引**、只有 append buffer 的表被排水：`dump_table` 返回 `true`（只用 `memindex_bytes_of` 会返回 `false`，此测试会失败），下一轮不进候选集 |
| `reclaim_stale_drains_without_byte_pressure` | 配额远大于用量，`idle_age` 很短，tail 与 buffer 都被落盘 |
| `a_table_with_an_index_dumps_every_tail_not_just_the_largest` | 一张表两个索引 tail，被选中后两个都排空 |
| `an_append_during_a_reclaim_flush_stays_buffered` **[待实现]** | 在 flush 与裁剪之间插入一行（测试钩子），该行仍在 `pending_rows` 且下一轮被写出；块文件里没有它 |
| `a_reclaim_then_append_is_not_reclaimed_again` | 回收后立刻写入，下一次扫描年龄不足，不触发 |
| `a_reclaimed_table_with_no_new_rows_is_not_flushed_twice` | 回收后不写入，`footprint == 0` 不进候选集 |
| `a_second_dump_appends_into_the_same_block` **[待实现]** | 100 行 → checkpoint → 100 行 → checkpoint：只存在 `blk_0`；`version.created` 为 `[(t1,100),(t2,200)]`；列文件长度不变；重启后读到 200 行 |
| `a_zero_reclaim_interval_does_not_disable_the_pressure_path` | `unsealed_reclaim_interval = 0` 时陈旧路径不发生，压力路径仍工作 |
| `the_write_path_seals_the_largest_index_of_the_table` | 一张表 100 MB + 1 MB 两个索引，多次 breach 都先选 100 MB 那个（当前实现是 3/3 轮转，此测试会失败） |
| `a_spann_tail_is_visible_to_index_memory_bytes` | SPANN tail 非空时 `index_memory_bytes` 返回非 0、`index_has_memory_tail` 为真；tail 空时两者都为假（当前都错，此测试会失败） |
| `an_unsealed_mark_follows_every_dump_branch` **[待实现]** | 只写一张表让某索引的标记非 0，逐个走 `seal_index_tail` 的各分支（HNSW/全文/BMP/多向量/IVF/SPANN），每次之后该索引的标记回到 0（漏一个清零点此测试会失败） |
| `a_drained_table_leaves_the_candidate_set` | SPANN 表被 dump 一次后，下一轮不再进候选集（当前 `index_has_memory_tail` 恒真，此测试会失败） |
| `reclaim_does_not_hold_the_bottom_gate_while_taking_flush_lock` | 并发跑一次 checkpoint 与一次 `reclaim_unsealed`，两者都在超时内完成（顺序反转会挂死） |
| `a_partially_dumped_table_keeps_its_age` **[待实现]** | block 写成功、索引 dump 失败：`unsealed_live` 已变小（`dump_table` 返回 `true`），但该表仍有 tail、`has_unsealed` 仍为真，`last_write_ms` 未前进 |
| `repeated_failures_back_off_exponentially` | 连续失败 1/2/3 次后，候选集门槛分别是 `idle_age × 2/4/8` |
| `the_tick_pressure_check_does_not_move_the_soft_waterline` | 连续 10 个 tick 判定，`Watermarks::soft()` 不变 |
| `a_permanently_failing_table_does_not_starve_the_rest` **[待实现]** | 1 张必然失败的表 + 3 张健康表，上限 2：健康表仍被回收 |
| `a_round_that_releases_nothing_defers_the_next` | 一轮无进展后，下一次压力检查被推迟一个 `unsealed_reclaim_interval` |
| `an_import_in_one_table_does_not_stop_other_tables` | A 表 IMPORT 中，B 表仍被回收（逐表门，不是全局门） |

**[待实现]** 的 8 条依赖测试钩子（在两段之间注入一次 append、让 `dump_table` 必然失败、逐个走 `seal_index_tail` 分支），属验证补强，不影响本项的验收口径；实现它们时优先做 `an_unsealed_mark_follows_every_dump_branch`（漏一个清零点会让表永远留在候选集）。

单测里如何驱动周期逻辑：**不依赖真实时间，也不起 tick**。`trigger_loop` 是私有函数，测试直接调用 `Engine::reclaim_unsealed(now_ms, target)`，把"现在"作为参数传入——600 秒的阈值在单测里是一次数值比较，不是一次 sleep。

## 9. 未决

- **`unsealed_reclaim_interval` 的实测标定**：默认 30 秒是照 `unsealed_idle_age / 20` 推的 **[推算]**。扫描是 O(表数) 次原子读（一次 `tables` 锁内，§4.0）加一次 `list_indexes()`；要在 10000 表 × 3 索引上量一次合计耗时，并确认压力路径每 tick 做一次可以接受。若偏重，就把 `list_indexes()` 的结果缓存到下一轮（索引 DDL 会 bump 一个 epoch，缓存按 epoch 失效）。
- **`unsealed_max_tables_per_round = 100` 的实测标定**：100 是 **[推算]**，依据是"一次后台推进不该长时间占用 `flush` 串行化"。实际取值取决于单表 dump 的典型耗时；100 张表的单轮耗时若超过 `unsealed_reclaim_interval`，就调小。
- **`K = 3` 的实测复核**：`K` 越小年龄越占优（§4.1）。3 是按"年龄优先于尺寸"定的，具体取值需要在一组真实表分布上看融合后的顺序是否符合预期；若发现尺寸完全不起作用（排序等价于纯按年龄），就把 `K` 调大。
- **排水与 `checkpoint` 的协调**：两者都取 `flush_lock`。撞在一起时串行跑通，会重复写同一张表——这是可接受的（§4.7），不做合并。
- **`SealScheduler` 的删除范围（已执行）**：它的 tenant 取库名，而写路径的候选集永远只落在一个库里，权重必然恒等（§4.4）。已删除 `SealScheduler` / `SealCandidate` / `SEAL_STARVE_TICKS` / `Engine::index_seal_starvation` / `Context::seal_scheduler` 与三个多库单测（`the_fair_pick_follows_the_bytes`、`a_tenant_cannot_hold_every_slot`、`a_starved_tail_jumps_the_queue_and_is_counted`）；三个单测构造的都是「多个库各有一个候选」的场景，与本路径的候选集形态不符。写路径改为 `pick_largest_index`（bytes 降序、平局取更早的 `last_dump`），由 `maintain.rs` 的单测覆盖。
- **写路径改为"选最大"是否影响既有测量**：`the_memindex_quota_seals_one_hnsw_chunk_per_batch` 只有单索引，不受影响；但任何依赖"两个索引交替被 dump"的测试都会变化。实现时先跑 `cargo test -p executor --lib` 确认没有这类依赖。
- **IVF / BMP / 多向量是否补行数上限**：这三类没有写路径的行数触发（§4.4 表），只能靠配额压力或周期路径。判据是一个可测的上界：它们的 tail 在"`memindex_memory_quota = 0` 且 `unsealed_reclaim_interval = 0`"下的最大占用。若实测超过 `mem_index_capacity` 行对应的字节量（HNSW 用同一配置作参照），就补一条同样的行数触发；否则保持现状并在文档里写明"关闭两条路径时这三类无界"。
- **tick 本身是否将来还要动**：本方案决定不动（§4.7）。若将来要动，影响的是四个现有 trigger 的触发精度，需要单独评估。
