# 并行与异步改造计划（Rust 重写版）

> 修订 26（§6.12 第 4 步收口：`perf` 采样 50 万行 bench，97.89% CPU 在语句线程、2.11% 在 `wal-writer` 的真 I/O，无 `spawn_blocking` worker 持有 CPU 热点；A4 四步全部完成）。
> 修订 29（扇出改为 **tokio task + `Semaphore`**：`index::parallel::{for_each_blocking, map_blocking}`，每个项一个 task、许可数 = 调用方 worker 数，项体用 `spawn_blocking` 执行；去掉 scoped 线程与自建池。并行只在 tokio 原生原语上做，`spawn_blocking` 只包拆到叶子后的阻塞单元。§6.6、§6.13 同步改写）。
> 修订 27（§6.12 第 3 步重做：`Engine::execute` **就地改成 `async fn`**，DML/DDL 写路径逐层 async，四个等待点全部 `.await`，计算与阻塞 I/O 由 `spawn_blocking` 派发；会话状态改 task-local；`block_on` 换成可嵌套实现。6.12.3 的 A'/B' 改判作废，见 6.12.3 与 6.12.4）。
> 修订 25（§6.12 第 3 步（旧方案）：`Engine::execute_async` 作为边界入口——已被修订 27 取代）。
> 修订 24（§6.13.3 落地：posting 编码按批并行、写出顺序不变，字节测试锁住三种批大小的等价）。
> 修订 23（§6.12 第 2 步与 §6.13.2 落地：WAL、行分配器、bg_task、bottom 四个队列换 tokio 原语；块解码进 rayon 池）。
> 修订 22（新增 §6.13：块解码、posting 编码、HNSW 建图这三类**纯 CPU 计算**从 async worker 移到专用线程池；`spawn_blocking` 只留给真正会阻塞的调用，边界写进本条）。
> 修订 21（新增 §6.12 整链 async 化设计：`Engine::execute` 改 `async fn`、`Mutex` 激进换 `tokio::sync::Mutex`；分四步、每步独立验收）。
> 修订 20（O15 定为表级锁：`MIRROR_WRITE_LOCK` 收窄成 `table_key → Mutex<()>`，不是 segment/block 级；理由入 §5.1）。每个交付项改为“**产出物 + 验收**”；未决实现选择集中到 §11；§12 列出仍未处理完的条目。
> 标注：**[实测]** / **[推算]** / **[设计]** / **[开放]**。

## 1. 范围

| 项 | 产出物 |
|---|---|
| P1 | 索引点查：设计说明 + 调用清单 + 实现（§4.1） |
| P2 | 配额汇总：增量生命周期契约 + 实现（§4.2） |
| P3 | checkpoint 表级并行（数据 + 索引）+ 尾巴精确清理（§5.1/§5.2） |
| P4 | 启动移植 RecoverMemIndex：加载 chunk + 只补 gap，并按表并行（§5.3） |
| A1 | 每 session `SessionContext`：模型 + 实现（§6.1） |
| A2a | **入站**硬上限（不是可调预算）：协议长度/元素数校验 + 头部上限 + 实现（§6.2.1；volo 下的校验见 O13） |
| A2b | **回复**预算与流式化：**不做**——C++ 参考同样整包物化，见 §6.2.2 |
| A3 | PG → HTTP → Thrift 的 async 化（§6.3-6.5） |
| A4 | **整链 async 化**：4 个等待队列换 tokio 原语、`Engine::execute` 就地 `async fn`、DML/DDL 写路径逐层 async、计算与阻塞 I/O 由 `spawn_blocking` 派发（§6.12，**四步已全部实施**） |
| A5 | **CPU 计算移出 async worker**：块解码、posting 编码、HNSW 建图各有自己的并行单位，全部离开 async worker（§6.13，**三类均已实施**） |

不做：checkpoint 的跨表崩溃原子性（staging/generation）、启动的新快照格式/字节布局、内存容量调参、磁盘格式变更（§5）；回复流式化与大响应保护（§6.2.2）。

目标部署：10k 表 × 1M 行、每表全文 + HNSW + 次级。

## 2. 现状

> 本节是**改造前的基线快照**（计划立项时的状态），不是当前状态；当前实现见 §13。行号也停留在当时。

### 2.1 Tokio

- 依赖 `crates/executor/Cargo.toml:23`；feature 根 `Cargo.toml:34`（仅 `rt-multi-thread`）。
- `tokio::` 路径 3 处（`scheduler.rs:24,33,61`）；`async fn` 0；`server` 不依赖 tokio。
- 全部作业走阻塞池（`operator/mod.rs:600/612`、`statements.rs:618`、`:703`）；共享、无配额。

### 2.2 并发形态

| 子系统 | 机制 |
|---|---|
| Thrift/HTTP/PG | 每连接 `std::thread` + 阻塞读写（`lib.rs:308-328`、`http.rs:65-79`、`pg.rs:439-457`） |
| Peer | 无监听（`peer.rs:2591` 仅测试） |
| 查询/bottom | `scheduler()` + `mpsc`；`BottomWork::wait`（`state.rs:4744`） |
| WAL/后台 | `txn/src/lib.rs:206` 具名线程（`wal-writer`）；`BGTaskProcessor` |

### 2.3 基线

**[实测]** 16 核、release、64 表 × 256 行、dim 32、每表 1 二级 + 1 HNSW（128 索引）：dump 1.51 s（≈11.8 ms/非空索引）、8 线程 1.87 s。探针固定该 workload，并发取自 `INFINITY_FLUSH_DUMP_CONCURRENCY`，单次运行。其余历史数字（16×4096、`flush()` 5.17 s、checkpoint 3.7 s）**[开放]**，不作依据。

### 2.4 规模

10¹⁰ 行、30k 索引、≈1.22×10⁶ 块。单表 HNSW **计费**上限 43.5 MB（65536 × 664 B，dim=128；估算式，不含容量/分层）。全库 435 GB 是极端推算。TB 级单机不现实；只讨论单节点引擎内。

### 2.5 影响

| 项 | 事实 |
|---|---|
| catalog 扫描 | 引擎**始终**是 `catalog_only`（`statements.rs:114`）：`list_indexes` 全扫（`catalog/src/lib.rs:1970`），点查走 `read_layout` + `read_indexes`（`:634-650`）——这是 §4.1 要改成内存索引目录的部分，不是可选分支。**计数规则**：全仓 75 处文本出现 − 28 处测试（24 处 inline `#[cfg(test)]` + 5 处 `*_ut.rs`，1 处重复）− 定义 1 处 = **生产 46 处**；静态清单会漏转调 helper，最终以插桩计数为准 |
| 写路径 | `note_index_insert`（`dml.rs:1571`）每批全扫 |
| 配额 | `memindex_bytes`（`index_build.rs:1089`）遍历内存表；插入侧计费已增量 |
| 启动 | `rebuild_indexes`（`:6`）/`recover_mem_index`（`:75`）逐索引读该列的全表（§5.3：改为加载 chunk + 只补 gap，按表并行） |
| checkpoint | 原地重写、先删后写、标记错误丢弃（§5.1：表级并行，每表内部顺序不变） |

## 3. 判据

| 工作 | 机制 |
|---|---|
| 等外部 IO | A（reactor 内） |
| CPU/本地 IO，失败语义已定义 | P |
| 固定开销、失败窗口无法界定 | 串行 |
| 扫 catalog | 点查 |
| 长期后台循环 | `std::thread` |

## 4. P 交付

### 4.1 P1 索引点查与内存索引目录

**背景**：引擎**始终**用 `Catalog::with_store_catalog_only`（`statements.rs:114`）——持久化只有 C++ 键空间，`idx:{table}\x1f{name}` 这类原生键**根本不存在**（`Catalog::open` 的原生后端只在单测里用）。这不是“可选分支”，而是读路径 O(全库) 的根因：`list_indexes()` 每次 `read_layout` + 扫整个 `catalog|idx|`，而 `index_position` / `hnsw_options` / `dump_version` 被逐表逐次调用。

**不能写原生键**：字节门禁对 catalog 做全量键值比较（`python/restart_test/test_cross_engine_bytes_10_encoding.py` 的 `_raw_catalog_hex`），多一条 `idx:` 键就与 C++ 目录不同。修复必须落在**内存**。

**产出物**

1. **内存索引目录**（对齐已有的 `table_cache`）：`index_cache: Mutex<HashMap<table, Vec<IndexMeta>>>` 加名字索引；首次访问某表做**一次**该表的有界构建（`catalog|idx|{db}|{tbl}|` + 逐名 `index_record` / `index_base`），之后 O(1)。`list_indexes()`（SHOW/管理）仍是全扫，不走缓存。
2. 失效规则与 `table_cache` 完全一致（`catalog/src/lib.rs:1358-1384`）：`begin_ddl` 清空、`ddl_epoch` 只发布同一 epoch 内读到的结果、`capture`/`ddl_depth > 0` 时绕过缓存。
3. API：`index_by_name(table, name) -> Result<Option<IndexMeta>>`、`indexes_of_table(table) -> Result<Vec<IndexMeta>>`；`catalog_only` 与单测用的原生后端语义一致。
4. 清单：46 处生产调用按“精确查找 / 按表列举 / 全量管理 / 转调 helper”分类并替换：`index_position`、`hnsw_options`、`ivf_storage_option`、`plaid_options`、`plaid_mirror`、`dump_version`、`publish_index_chunks_of`、`query.rs` 索引选择、`admin.rs` compaction 列表；以及 checkpoint 里 `write_fulltext_index`/`write_ivf_index` 的二次 `list_indexes()`（当前是 O(N²)）。
5. 语义：表名规范化（限定/短名）、DDL overlay 可见性、缺失时 `NotFound`。

**验收**

- 插桩：预热后精确查找站点的 `scan_prefix` 次数为 **0**；未命中时恰好一次**该表**的前缀扫，不得出现整段 `catalog|idx|` 扫描或 `read_layout`。
- 字节门禁 `_00…_70` 全过（catalog 键集合不变）。
- 并发 DDL：10k 表下 `CREATE/DROP INDEX` 与查询交错，缓存不返回旧 schema（epoch 测试）。
- 成本：10k 表 × 3 索引时，`SELECT`/`INSERT` 路径的每个索引查找从 O(30k) 降到 O(1)（命中）/ O(该表索引数)（未命中）。

### 4.2 P2 配额汇总改增量

现状：`memindex_bytes`（`index_build.rs:1089`）每次锁 `tables`、遍历所有表、克隆 text/sparse/mv 的 `Arc`，再逐索引取锁读 `mem_used`/`tail_bytes`/`approx_bytes`；插入侧已有 `index_pending_bytes`。

**产出物（生命周期契约）**

- 每索引一个 `AtomicUsize`，在同一把表级锁下更新；`append` 加，`dump/seal` 减“本次 dump 覆盖的部分”（不是清零，清零只适用于竞态已修的场景）；`drop table/index` 归零并移除。
- **一个全局原子总量**：每次索引计数变化时按 delta 更新全局（不是每次汇总再求和）；汇总读取 O(1)。数值定义与旧扫描一致（`text.mem_used()`、`tail_bytes`、`approx_bytes`、HNSW 计费、IVF 行）。
- 一致性：任意时刻汇总 ≥ 实际（保守上界），允许短暂偏高，不允许偏低；dump 后 ≤ 配额。
- `memindex_quota == 0` 时仍维护计数，避免开关切换重建。

**验收**：同一 workload 下增量汇总与旧扫描在 100 个随机检查点相等；丢表/删索引/清尾巴后归零；每 10 ms 的汇总不再持索引锁。

## 5. checkpoint、尾巴与启动（本轮改判）

> 上一版把这三块都列进“不做”。本轮复查后拆开：**表级并行 checkpoint** 与**尾巴精确清理**的障碍是全局锁和临界区划分，不是正确性，纳入范围；**启动**改为移植参考的 RecoverMemIndex（加载 chunk + 只补 gap）、并按表并行；只有“新快照格式”“跨表崩溃原子性”“容量调参”仍不做。

### 5.1 checkpoint：做表级并行（数据 + 索引），不做跨表崩溃原子性

**现状**

- `Engine::flush_storage` 取 `flush_lock.write()`（`admin.rs:364-366`），然后调 `flush_blocks(checkpoint_ts)`。
- `flush_blocks`（`blocks/engine.rs:1066-1130`）是三层串行：每张表 `write_blocks_bounded` 后清 `ivf_mem`；全库每个索引串行写 HNSW/fulltext/plaid/ivf；最后再遍历所有表 `publish_index_chunks`。
- 块写本身**已经按表隔离**：`write_blocks_mode` 先取该表的 bottom gate（`blocks/engine.rs:228-236`，注释写明“checkpoint 与同一张表的 append 互斥，不同表可以并行”）。**但**紧接着取进程级 `MIRROR_WRITE_LOCK`（`blocks/engine.rs:360-366`、`misc.rs:22`），把所有表的文件写重新串成一条。表级并行的骨架已经在，缺的是把这两处对齐。
- 索引侧还有乘法成本：每个索引都 `list_indexes()` 全扫（`write_fulltext_index`/`write_ivf_index`/`write_hnsw_chunk`，`blocks/engine.rs:4915/5607/4261`），10k 表 × 3 索引 = 30k 次目录扫描；P1 的 `indexes_of_table` 是前置。

**结论**：并行单位是**表**，不是块。每张表内部仍严格 `块 → 本表索引 → chunk 发布 → 清尾巴`，`version` 仍是块目录里最后写的文件；跨表之间只有 per-key 的 catalog 写与最后**一次** `store.flush()`，本来就没有顺序要求。

**做法**

1. `flush_blocks` 改成“一表一任务”：`{write_blocks_bounded, 本表索引, publish_index_chunks, 清尾巴}` 投到有界线程池；并发取 `min(表数, 核数)`，用独立配置（如 `checkpoint_concurrency`，参照 `INFINITY_FLUSH_DUMP_CONCURRENCY` 的写法），不与查询共用 `scheduler()`。
2. `MIRROR_WRITE_LOCK` 收窄成**表级**（O15 已定：**只能是表级**）：`table_key → Arc<Mutex<()>>`，取 map 锁只 clone 一个 `Arc` 就放，出 map 之后再 lock；保持既有锁序——先 `tables` 后 mirror，出 mirror 之后才碰 `tables`（`blocks/engine.rs:836-841` 的注释就是这条），绝不反向。drop table 时删条目，避免反复 DDL 后 map 跟着表数量级增长。
   - **为什么不是 block 级**：块写已经在 bottom gate 内，同一张表不会有两个写者；block 锁带不来任何并行，却保护不了 `refresh_block_mirror` 末尾按**表**扫描的 `seg_*` 陈旧目录清理（它跨段）。
   - **为什么不是 segment 级**：`publish_segments` 一次提交整张表的 `SegmentSet`，陈旧目录清理也以表为单位算 `live` 集合；让同表两个写者分持不同段，会交错“写段 A”与“删陈旧段”，可能删掉另一个写者刚建好的目录。
3. 每表任务开头取该表 bottom gate（`HeldBottom` 语义，非重入），把“写块 + 清尾巴”合成一个临界区——这条同时修 §5.2。
4. 索引列表从“每索引一次 `list_indexes()`”改成“每表一次 `indexes_of_table`”（P1）。
5. 全局段留在并行区之外：`checkpoint_ts`、`reload_vector_indexes`、`drop_index_caches`、对象存储同步、catalog layout rewrite、最后的 `store.flush()`。

**为什么上一版写“不做”**

上一版的理由是“失败窗口 × K”。这条不成立：并行不引入新的失败**模式**，只让更多表同时处在“块已写、元数据未刷”的窗口里；checkpoint 本来就不承诺崩溃原子性（见下）。真正缺的是把全局 `MIRROR_WRITE_LOCK` 收窄，这是上一版没查到的实现细节，不是正确性障碍。

**仍不做：跨表崩溃原子性**

staging + 回滚副本 + 单批次 catalog + 私有 journal 仍需另立项目；加 generation（写进文件名/记录）会改变 C++ 读端认的固定路径（`block_meta_impl.cpp:206-218`），破坏字节门禁。并行与原子性是两件事，这里只做前者。

**验收**

- 字节：`_00…_70` 门禁不变；同一 workload 下 checkpoint 后的 `data/`、`catalog/` 与串行版逐字节相同。
- 时间：10k 表合成 workload（每表 8k 行，或 1k 表 × 1M 行）checkpoint wall time 随核数下降；单表 checkpoint 与现在相同（并发=1 时路径不变）。
- 线程/内存：任务数有界；RSS 不随表数线性增长（每任务只持一个块的解码缓冲）。
- 竞态：checkpoint 与同表 append 并发不丢行；跨表并行时 `catalog|seg|…` 记录无交错（同一表只有一个任务）。
- 锁粒度：10k 表、并发 = 核数时，同时处于 mirror 临界区的**表**数 ≈ 并发数（不是 1，也不随总表数增长）；用带超时的用例或死锁检测验证 `tables` → mirror 单向且无环。

### 5.2 尾巴清理：并入 5.1，不再单列

现状的位置不变：checkpoint 段②清 IVF/HNSW（`blocks/engine.rs:1068-1078,1094-1105`）、显式 dump 的 seal/clear（`index_build.rs:669-675,804-830`）、容量触发（`dml.rs:636-637,1440-1441`、`import.rs:371-372`、`state.rs:4656-4657`）、配额触发（`dml.rs:1634-1641`、`index_build.rs:1888-1893`）、compaction（`engine/admin.rs:162-179`）、启动重置（`index_build.rs:8-14`）。

**问题**：checkpoint 的 `ivf_mem.clear()` 与 HNSW `hnsw_mem.remove` + 计费清零发生在 `write_blocks_mode` 返回**之后**，不持该表 bottom gate；同表 append 可以在这两步之间把新行追加进尾巴，`clear()` 会连新行一起扔掉（行还在块/日志里，搜索却少一段）。上一版把它当成独立重构，其实修法就是 5.1 第 3 条：**写文件与清尾巴在同一个 bottom gate 内**，此时 `clear()` 即精确（镜像覆盖了该表全部行）。

`with_table_bottom_drain` 作为实现点：取 bottom gate + `HeldBottom`，跑非重入的 dump core（写文件、按覆盖范围清尾巴、同临界区扣 `index_pending_bytes`），容量 dump 与 quota dump 走同一个 core；`batch_gate` 与 bottom gate 的次序沿用现有 commit-bottom 的顺序。

**验收**：同表并发“append + checkpoint”重复 1k 次不丢行、计费归零；`kill -9` 后重启，尾巴由 `recover_mem_index` 按 chunk gap 补回，与串行版一致。

### 5.3 启动：移植参考的 RecoverMemIndex（加载 chunk + 只补 gap），并按表并行

**现状（端口）**：`rebuild_indexes`（`index_build.rs:6-57`）对每个二级索引无条件 `build_index`：先 `secondary.disk.remove` 丢掉 reader，再 `fetch_rows_with_ids_projected_all` 全表扫、把全部 chunk 重写一遍；HNSW/IVF 也同样重新加载。10k 表 × 1M 行 = 10k 次串行全表扫。

**参考模型**：`Storage::Init` → `RecoverMemIndex`（`storage_impl.cpp:414`）→ `NewTxn::RecoverMemIndex`（`new_txn_index_impl.cpp:2944`）：每段读 `SegmentIndexMeta` 的 chunk 列表，`CountMemIndexGapInSegment`（`:2792`）把各 chunk 的 `[base, base+row_cnt)` 合并成覆盖区间、取未覆盖的 gap，只对 gap `AppendIndex`；chunk 本身由 `ChunkIndexMeta` 惰性打开。`CREATE INDEX` / `IMPORT` / `COMPACT` 才走 `PopulateIndex`（`:1053`）真正建 chunk。

**产出物**

1. 二级索引加载路径：按 `idx_chunk` 记录打开已发布 chunk（高基数 `SecondarySegmentReader`、低基数 `LowCardReader`、BMP 用它自己的 sparse chunk），填 `secondary.disk` / `low` / `chunk_meta` / `disk_backed`，**启动阶段不重写文件**。`catalog_only` 打开的 C++ 目录只有 `data/db_*/tbl_*/idx_*/…` 副本，loader 必须直接读它。
2. gap 补行：按段合并覆盖区间，未覆盖的行做一次“按索引列投影的扫描 + 建索引”，结果进内存尾巴（之后按容量/配额 dump）；覆盖有洞（中止的 dump、nullable 行）按 gap 处理，不致命。
3. 并行：按表一个任务（与 P3 同一个有界池规则），该表的 chunk 加载与 gap 补行在同一任务内；HNSW/IVF 的 chunk 加载也放进来。
4. 去掉 `rebuild_indexes` 里的全量 `build_index`：`build_index` 只服务 `CREATE INDEX` / 新段 / compaction，与参考的 `PopulateIndex` 对齐。

**仍不做**：新的快照格式/私有侧车（复用已发布的 C++ chunk）；`mem_index_capacity` 调参（等实测）。

**验收**

- 启动阶段不扫全表：插桩显示 `rebuild` 路径的 `fetch_rows_with_ids_projected_all` 次数 = 未覆盖段的 gap 数，而不是表数×索引数；10k 表合成 workload 的启动时间随 gap 行数（而非总行数）增长。
- 构造“部分 dump + 尾部未封存”的目录，重启后每个二级索引的查询答案与全量重建版逐条一致（含 NULL、BOOL、BMP、时间/浮点列）。
- `catalog_only` 打开 C++ 目录同样成立（没有 `<root>/secondary/` 私有副本时从 `idx_*/` 读）。
- 字节：重启前后 `data/` 与 `catalog/` 与串行版一致（chunk 只在 checkpoint 的 publish 阶段按现有逻辑重写，启动不产生新字节）。

## 6. A 交付

### 6.1 A1 每 session 会话

**产出物（模型）**

- `SessionContext { current_db, variables, statement: StatementScope, generation }`，由 `Connect` 创建、登记在 `session_id → SessionContext`，**显式传参**给 `execute`（不再读 `Engine.current_db`，`state.rs:4319-4320`）。
- 生命周期与参考一致：`Connect` 建、`Disconnect` 删；**连接断开不销毁**（参考的 session 在静态 map 里，`infinity_thrift_service_impl.cpp:1936-1942`，重连后旧 id 仍可用），泄漏的 session 只在 `Disconnect` 或进程退出时回收。未知 `session_id` 一律回 `SessionNotFound`，`Connect`/`Disconnect` 除外。
- 两级作用域：`SessionGuard`（每 session，持 `current_db`/变量/连接代）与 `StatementScope`（**每条语句**一个，进入时 `begin_statement`/`ensure_statement_id`，退出时 `end_statement`）。`PENDING_TXN_ID`（`txn/src/lib.rs:525-557`）、`READ_TS`、`HeldBottom`、`ALLOW_ZERO_SPARSE` 等线程局部状态随 `StatementScope` 进入/退出；成功、错误、panic 三路都要清。`READ_TS` 属于语句级，bottom 所有权属于语句的下半部，不跨语句复用。

**实现后的修正（2026-11-10 收尾复核）**：上面的模型比参考实际需要的更重。按参考代码收敛如下——

- **`variables` 不存在，也不需要**：参考的会话变量是 `query_count`、`total_commit_count`、`total_rollbacked_txn_count`、`connected_time`，全部是引擎级只读计数（`SHOW SESSION VARIABLES` 直接读引擎状态）；`SET VARIABLE` 在 Session 作用域一律返回 `read-only`（`physical_command_impl.cpp:68-150`）。所以没有 per-session 变量表可存，`SessionContext` 里放一个 `variables` 字段反而是凭空造状态。
- **`generation` 不做**：会话在静态 map 里按 `session_id` 存活，`Disconnect` 才删；重连沿用旧 id 天然看到同一个 `current_db`，不需要代际号区分。
- **`statement: StatementScope` 收敛为 `PENDING_TXN_ID` 的惰性作用域**：`ensure_statement_id` 在首次使用时分配、`end_statement` 清理；`begin_statement` 因此是 no-op。`READ_TS`/`HeldBottom`/`ALLOW_ZERO_SPARSE` 仍由各自的调用点管理，没有并进语句作用域。

**验收**：并发 `USE db_a`/`USE db_b` 互不影响；同一 session 在断线重连后沿用旧 id 仍看到同一个 `current_db`；`Disconnect` 后旧 id 回 `SessionNotFound`；语句级线程局部成功/错误/panic 三路无残留（`a_panicking_statement_does_not_leak_its_session` 覆盖 panic 一路）。

### 6.2 预算

**只有一个绝对值参数**：`[buffer] memindex_memory_quota`（默认 4 GiB，与参考同名同义）——各内存索引（全文/稀疏/向量/二级）合计超过它就 dump 成 chunk。除此之外没有进程级内存参数：

- **不设 `total`**：不按物理内存比例推总额，也不做请求级内存准入；参考实现同样没有。并发**不做**请求级封顶（§6.5：RPC 绝大多数时间在等 WAL/分配器/bottom/CPU 池，按核数封请求数只会让多客户端排队饿死），实际执行量由阻塞池与语句内部的扇出许可约束。
- **不统计 mmap**：封存块与二级索引 chunk 都是只读映射，64 位地址空间远大于物理内存，映射量不是用量；驻留由内核按压力回收，checkpoint 的 `fadvise(DONTNEED)` 只是提前提示。
- 索引侧另外两个数不是“预算”：`buffer_budget` / `hnsw_budget` 是内置 8 GiB（`buffer_manager_size` 被忽略），`mem_index_capacity` 是行数触发（65536）。

#### 6.2.1 A2a 入站硬上限（交付）

请求侧不设账本，只有协议级硬上限（常量、不可配）：

- 容器长度：负值拒（`NEGATIVE_SIZE`）、元素数上限 1,000,000（`SIZE_LIMIT`），有完整缓冲的协议再按“剩余字节 / 最小元素尺寸”校验——已在 pilota fork 实现（O13，`74f14ab`），binary / binary_le / binary_unsafe / compact 全覆盖。async 流式路径只有前两条（unframed 无长度前缀）。
- 分配前校验：生成的 decode 不再是 `Vec::with_capacity(list_ident.size)` 直接吃头部，先过上面三条；`read_bytes` 同样校验长度。
- HTTP：头部滚动上限 **64 KiB**（在 `\r\n\r\n` 之前，当前 `http.rs:2882-2891` 是无上限循环）；body 仍按 `Content-Length` 读入，不加上限（与参考一致）；响应侧不设预算（§6.2.2）。
- 计数在**连接 transport** 上按消息边界重置；不能放在每方法新建的 `TBinaryInputProtocol` 实例上（`lib.rs:461,488,500`）。
- 不设请求级准入：同时在解的请求数没有上限，decoded 峰值不按“核数 × 单请求大小”推算。协议级硬上限（上面几条）约束的是一条消息能有多大，不是同时在处理多少条；实际吞吐由阻塞池线程与语句内部的扇出许可决定。

**验收**：1 B 分片、负长度、`i32::MAX`、100M 元素的 12 字节容器头（拒绝且 RSS 不涨）、巨大 header（拒绝）、慢读客户端；两条请求共用连接时计数只在消息边界重置。

#### 6.2.2 A2b 回复预算与流式化（不做）

C++ 参考也是整包物化：Thrift 的 `Select` 签名就是 `Select(SelectResponse &response, …)`（`infinity_thrift_service_impl.cpp:469`），先填完整结构再交给生成的 processor 序列化；HTTP 各调用点拿到的也是 `const QueryResult`，再拼字符串（`http_server_impl.cpp`）；PG 的 `SendData` 虽然逐行写，行本身来自已物化的 `QueryResult`。Rust 版一样在 `Engine::execute` 返回前就分配完（`query.rs:376-465`），协议层任何 reply 预算都来不及。

所以不做：不加 `for_each_row`/有界 sink（O10），不给 HTTP 大响应做流式或 100 MiB 报错（O6），也不做逐 RPC 的 reply 预算（O7）。峰值内存由引擎侧结果集决定，与参考实现一致；真要改就是引擎的流式结果 API，属于独立项目。

代价写清楚：一条返回巨大结果集的 SELECT 仍可能把进程内存打满——参考实现有同样的问题，这是“与参考一致”的取舍，不是遗漏。

### 6.3 A3 路线

| 协议 | 结论 |
|---|---|
| PG | **已做**：`PgServer::run` 变成 tokio runtime + 每连接一个 task；启动握手与命令解码在 reactor，`simple_query_response_async` 直接 `.await` `Engine::execute`（§6.12.4 第 3 步）。帧构造仍由 `PGProtocolHandler` 提供，同步单测继续覆盖字节 |
| HTTP | **已做**：`HttpServer::run` 同样改成 tokio runtime + 每连接一个 task；头部读取保留 64 KiB 上限（超限回 431），`route` 在调度池上跑，回复在 reactor 写出 |
| Thrift | **(c)** `volo-thrift` 从 `thrift/infinity.thrift` 生成 async service（§6.3.2）：IDL、协议、回复字节三项已通过，采用；**(a)** 手写两阶段解析（§6.3.1）只在运维门禁（§9 的 1000 空闲连接线程数）不达标时回退——volo 下每连接是一个 `tokio` task，该门禁按构造满足，仍按 §9 实测。 |
| S3 | 不 async；后台线程/有界池，保持 `FLUSH DATA` 同步语义 |


#### 6.3.1 备选路线 (a)：手写两阶段解析（未采用）

`enum Request` + 每 RPC 一个 `parse_*`；先解析出自持有请求（帧边界 = `Cursor::position()`），`spawn_blocking` 只做执行 + 编码，编码结果经有界通道交回 reactor 的 `write_all`。工作量在“49 个方法的请求枚举 + 回复编码”，且没有现成的 async 连接管理。

#### 6.3.2 采用路线 (c)：volo-thrift

`thrift/infinity.thrift` 在仓库里（`service InfinityService`，49 个方法）。`volo-thrift` 能从 IDL 生成 async service trait，自带 Tokio 网络、codec、超时与连接管理；采用后 **O9（逐 RPC 解析清单）消失**，手写的 8700 行 handler 变成 49 个 trait 方法 + 共享 helper。

| 维度 | 判断 |
|---|---|
| 协议兼容 | 已确认：`volo-build` 从仓库内 IDL 生成；codec 显式用 `MakeThriftCodec`（unframed binary，与两个 SDK 的 `TBufferedTransport` 一致），strict header 为 `80010002`，pysdk（`strict_read=True`）可直接解析 |
| 回复字节 | 已逐字节对比通过：`CommonResponse` 96 B / 74 B，`SelectResponse` 9 个用例（见下表），回归用例受 git 管理 |
| 错误语义 | **已定（O12）**：协议错误发 volo 的 `TApplicationException`（message type 3，两个金样本见 §6.4），应用错误仍走 `CommonResponse.error_code`；SDK 把 type 3 映射为 `InfinityException` 且不重试 |
| 会话 | `Connect` 返回 session id；A1 按 `session_id` 登记（与参考一致：`Disconnect` 才释放，连接断开不销毁），volo 的共享 service 不需要连接身份（O14 随之关闭） |
| 依赖 | pilota/motore 等，构建时间与 MSRV 要核；license Apache-2.0 |
| 回复内存 | 生成类型仍是整包物化；A2b 不做（§6.2.2），与 C++ 一致 |
| 入站预算（A2a） | **已补（O13）**：fork 的 `read_{list,set,map}_begin` 在分配前校验容器长度（负长度 / 上限 / 剩余字节），volo 的解码路径不再直接 `Vec::with_capacity(list_ident.size)`；async 流式路径只有符号与上限 |

**验证结果（2026-11-09，本机）**

| 检查 | 结果 |
|---|---|
| IDL 解析 | **已解决**：`thrift/infinity.thrift` 有 17 处 `&`（Apache Thrift 官方语法，`thriftl.ll:264` 的 `tok_reference`；C++ 生成 `shared_ptr`），pilota 0.13.1 的 `Field::parse` 不接受。补丁只动 parser：把 `&` 解析为 `Attribute::Optional`，不需要改 descriptor/codegen——`FieldKind::Optional` 本就生成 `Option<Box<T>>`（`ArrayType.element_data_type` 实测为 `Option<Box<DataType>>`）。`[patch.crates-io]` 指向 fork 分支（`yuzhichang/pilota` 的 `fix/thrift-reference-field`）后，**未修改的** IDL 生成并编译通过，78 个 parser 测试通过。上游 issue [#378](https://github.com/cloudwego/pilota/issues/378)、PR [#379](https://github.com/cloudwego/pilota/pull/379)（截至 2026-11-09 仍 open；合并或发版后可去掉 patch） |
| 传输封装 | volo 的默认 codec 是 **`TTHeader<Framed<Binary>>`**；必须显式 `DefaultMakeCodec::new(MakeThriftCodec::new())` 才是 **unframed binary**。实测该配置下 pysdk 的 `TBufferedTransport` 客户端可正常连接 |
| 版本 | **volo 0.12.7 + volo-build 0.12.3 + pilota 0.13.1**（2026-11-09 的最新版） |
| 回复字节（`CommonResponse`，成功与错误形状） | **逐字节相同**：`Connect` 96 B；`CreateTable`（error_code=3048、error_msg='no column defined'）74 B；message header（`80010002`）、方法名、seq id、字段序 `0a/0b/0a/00` 全部一致 |
| 未知方法（错误语义） | **已接受（O12）**：volo 的 `TApplicationException` 成为唯一行为（66 B，字段见 §6.4）；现 server 的“静默关闭”被替换 |
| `SelectResponse`（列式/二进制负载） | **逐字节相同**，9 个用例（`tools/volo_wire/test_wire_parity.py`）：`INT`/`VARCHAR` 含 NULL 236 B、全有效 231 B（`bitmasks` 为空列表）；`FLOAT` + `EMBEDDING(FLOAT,4)` 357 B；`DOUBLE` + `TIMESTAMP` 322 B；`DATE` + `TIME` + `DATETIME` + `JSON` 485 B（`ColumnDate=16`/`ColumnTime=17`/`ColumnDateTime=18`/`ColumnJson=22`，`LogicType` 17/18/19/23：日期 i32 days、时间 i32 secs、datetime 4 B days + 4 B secs、JSON 为 u32 长度前缀原文）；`ARRAY(INT)` 264 B（`ColumnArray=21`，元素的 `DataType` 自带空 `NumberType`，u32 个数 + i32）；`SPARSE(1000,FLOAT,INT32)` 278 B（`ColumnSparse=13`，u32 个数 + 全部 i32 下标 + 全部 f32 值，SoA 不是逐对）；`TENSOR(FLOAT,4)` 277 B、`MULTIVECTOR(FLOAT,4)` 293 B（`ColumnTensor=11`/`ColumnMultiVector=14`，u32 字节长度 + f32 数据） |
| strict header / IDL 版本 | 间接验证：pysdk（`strict_read=True`）能解析上述回复，且 header 为 `80010002`；未单独构造非 strict 帧 |
| 仍未覆盖 | 多块/大 payload、`extra_result` 非空、空结果集，以及 `BOOL`/`INT8`/`INT16`/`INT64`/`FLOAT16`/`BFLOAT16`/`INTERVAL`/`ROW_ID` 等其余列类型（1000 空闲连接的线程数已实测：23 线程 / 109 MB，见 §13） |

复现材料：`tools/volo_wire/`（受 git 管理：`volo.yml` 指向**未修改的** `thrift/infinity.thrift`、`src/svc.rs` 49 个方法、`src/main.rs` 的 `MakeThriftCodec` 配置）与 `tools/volo_wire/test_wire_parity.py`（pytest：起 Rust server 建表插行，把同一份 `SelectRequest` 分别发给 Rust server 与 volo harness，断言回复逐字节相同）。运行：先在 `tools/volo_wire` 里 `cargo build`，再在仓库根 `uv run pytest tools/volo_wire/test_wire_parity.py`。
另一条用例 `tools/volo_wire/test_thrift_roundtrip.py`：python 官方编码 → pilota decode → pilota encode，断言字节相同，覆盖 `&` 字段（`ArrayType.element_data_type`）。
**决定**：A3 的 Thrift 采用 (c)。IDL、协议、回复字节三项已通过；实现前先复核 §9 的运维门禁（1000 空闲连接线程数 < 50），不达标则回退 (a)。O13 已在 fork 补上（`74f14ab`）：容器长度在分配前校验，volo 的解码不再按头部 size 直接预分配。
**实现口径（2026-11-10）**

- **请求**：volo 解码出的请求 struct 再编码成参数字节，交给原来那份 `dispatch_arm`。这样 WHERE/SEARCH/KNN/FUSION 的 SQL 渲染仍然只有一份实现（也正是字节门禁验证过的实现），不必重写；请求本身很小（`SelectRequest` 通常 < 1 KiB），回复的大负载不经过这一步。回复走生成 struct：`SelectResponse` 的 `column_vectors` 是 `Vec<u8>` → `Bytes`，零拷贝。
- **会话**：`Connect` 分配 session id 并由客户端在后续每个 RPC 里带回；`Engine::with_session(sid, …)` 在阻塞线程上套作用域，`db()` 因此仍按会话解析（不依赖连接身份，O14 成立）。
- **执行**：handler 不取任何请求许可，直接在 handler task 内 `.await` 语句（`dispatch_arm` / `route` 本身是 async fn）；PG 同样直接 await `Engine::execute`（§6.12.4 第 3 步）。没有 `spawn_blocking` / `block_in_place` 包住语句，reactor 只做编解码。
- **连接数不做上限**：volo 每连接一个 task，空闲连接不占线程（实测 1000 条 → 23 线程 / 109 MB），参考的 `connection_pool_size` 失去作用对象；`ConnGate` 与 `INFINITY_MAX_CONNECTIONS` 删除。
- **SDK 修复**：pysdk 对 VARCHAR 列把 `VarcharType` 直接赋给 `physical_type` 联合，线上是「空 union」；参考按字段号读因而无感，pilota 的 union 解码会拒绝。改为先建 `PhysicalType` 再赋 `varchar_type`（`utils.py`）。

### 6.4 错误与取消

- 协议层错误由 volo 的 codec 发 `TApplicationException`（message type 3），不再手写。已实测两个金样本（2026-11-09，`tools/volo_wire`）：
  - 未知方法：**66 B**，`80010003` + 方法名 + 请求 seq，field 1 = `unknown method <name>`，field 2 = 1（UNKNOWN_METHOD）；
  - 头非法（`de ad be ef`）：**84 B**，`80010003` + 空方法名 + seq 0，field 1 = `invalid thrift version: unknown protocol, first byte: 222`，field 2 = 7（PROTOCOL_ERROR）；
  - 空帧（连上不发）：不回复，连接保持等下一帧。
  其余协议错误（如解码中途失败）的形状在 A3 实现时补样本，以 volo 的行为为准。
- 应用错误仍走 `CommonResponse.error_code` / `SelectResponse.error_code`，不改成异常。
- SDK 契约（随 A3 一起改，**已实现**）：Python/Go 收到 type 3 帧时映射为 `InfinityException(PROTOCOL_ERROR = 9001)`（两个 SDK 的 `errors` 都新增了这个码），**不重试**；`retry_wrapper` 仍只重试 `TTransportException`。Python 侧 `Connect` 不走 `retry_wrapper`，同样的映射在那里也做了一份。回归用例 `python/test_pysdk/test_protocol_error.py`（UNKNOWN_METHOD / PROTOCOL_ERROR 两种帧，断言只调用一次）。现有 server 的“未知方法静默关闭”被替换。
- `JoinError` → 同异常 + 关。
- 取消与重试（O1 已定：照抄参考）：服务端不加 deadline/取消，handler 跑完；写事务可能已提交，断开只让回复失败。写 RPC 不做幂等/去重，语义是**至少一次**：唯一自动重试是 Python SDK 的 `retry_wrapper`（10 次、开新 socket、沿用旧 `session_id`，`client.py:183-207`），保留。逐 RPC 文档化重试后果：`INSERT` 重复、`UPDATE` 多一版、DDL 二次 already exists、`DELETE` 可重入。

### 6.5 原语与限流

**服务端框架由 volo 提供**：`Server::run` 的 accept 循环是 `tokio::spawn(handle_conn(...))`（`volo-thrift-0.12.7/src/server/mod.rs:299`），service 是共享实例，连接只占一个 future，worker 线程数 = 核数；没有 per-connection 线程，也没有 server 自建的执行池。§6.5 因此只剩下面这几条约束：

- **RPC 入口不设请求许可**（实现后修正：取消请求许可）。原先这里是 `Semaphore::new(cores)`，理由写的是“请求是 CPU 任务，多放只会抢核”——这个前提不成立：一次 RPC 绝大部分时间是**在等**（WAL 回复、行分配器、本表 bottom half、CPU 池），等的时候不占核。所以按核数封请求，封住的是**在途请求数**而不是计算量。10 000 个客户端每 500 ms 各发一次 `ListDatabases` 需要 20 000 语句/秒在途，4 核 runner 上 4 个许可让它们排成一队，`TestParallelManyClientsStayOnline` 实测在线数跌到 8663（当时的门槛 10 000）。现在不取许可，请求直接 `.await` 语句；真正约束执行量的是下面的阻塞池和语句内部的扇出许可；
- handler 直接 `.await` `Engine::execute`，不阻塞 reactor worker：等待点是 tokio 原语，计算与阻塞 I/O 都由 `spawn_blocking` 派发。任何新加的等待都必须是 `.await`，不得用 `blocking_recv` 或同步 I/O（§6.12.4 第 3 步）；
- `scheduler()` 的 runtime **不设** `max_blocking_threads`（实测 `cores+4` 会让池成为已由核数约束的工作的瓶颈）。`INFINITY_BLOCKING_THREADS` 仅作为线程预算紧的机器上的逃生阀，默认不设。阻塞池里合法的常驻项只有三样：每表 bottom 排空、行分配器的长驻消费者、fragment 的 sink。
- **请求积压一律有界**：`operator/mod.rs` 的 exchange 用 `tokio::sync::mpsc::channel(1024)`；`bg_task.rs` 的队列 1024；行分配器（`engine/statements.rs` 的 `Allocator`）现为 `tokio::sync::mpsc::channel(ALLOC_QUEUE_CAPACITY = 1024)`——每次 append 都在等自己的答复，所以队列里只有分配器尚未取走的活，上限存在只是为了分配器卡住时不会被灌爆。

**连接不设上限**（实现后修正）：volo 每连接一个 task，空闲连接不占线程（1000 条实测 23 线程 / 109 MB），所以参考的 `connection_pool_size` 和原计划的连接 `Semaphore` 都没有作用对象，A3 里把 `ConnGate` 与 `INFINITY_MAX_CONNECTIONS` 删掉了。会话状态用 `Engine::with_session`（执行线程上的 `SessionScope`，panic 也会恢复）；请求顺序由 tokio 的 per-connection task 保证。feature 需 `rt-multi-thread`、`rt`、`net`、`io-util`、`time`、`sync`（`Cargo.toml` 已补）。`ScanGate`（`state.rs:2960`）不是连接限流。请求侧已无任何许可（见上条）：同时在处理的请求数不设上限，decoded 峰值由协议级硬上限（§6.2.1，单条消息的大小）与阻塞池的线程数约束，而不是按“核数 × 单请求”推算。

### 6.6 CPU 密集型扇出：tokio task + Semaphore（不用 rayon，也不用自建线程池）

P3/P4 的并行单位是**表**（checkpoint 扫表、启动重建索引、批量 dump），块解码的单位是**块**，HNSW 建图的单位是 **bucket**，posting 编码的单位是 **term**。这些都是“对一个独立项列表跑同一个函数”，但它们的**工作体并不纯**：

| 扇出 | 工作体实际做什么 | 为何不能当纯 CPU |
|---|---|---|
| checkpoint 每表 `flush_table` | 写块文件/索引块/目录记录（`write_synced`、RocksDB 批写），取表级 bottom gate | 有 `fsync` 与锁 |
| 启动 `rebuild_indexes` 每表 | `mmap` 打开 chunk 文件，写 `tables` 注册表（持锁） | 有缺页与锁 |
| 块解码 | `block_map` 未命中时 `Mmap` 打开文件；命中时纯解码 | 映射是 I/O，解码是 CPU |
| posting 编码 | 纯内存编码，产出 `Vec<u8>` | **纯 CPU** |
| HNSW `build_range` | 纯图构建 | **纯 CPU** |

**原语**（`index/src/parallel.rs`）：每个项一个 **tokio task**，并发上限是 `tokio::sync::Semaphore` 的许可数（= 调用方给的 `workers`），项体本身用 `tokio::task::spawn_blocking` 执行。没有自建线程池、没有 work-stealing 运行时、没有第二个调度器与请求 runtime 抢核；一次扇出嵌进另一次扇出只是各取各的许可，不会互相饿死。

- `for_each_blocking(items, workers, f)` / `map_blocking(items, workers, f)`：`f` 是**一个阻塞单元**——要么纯 CPU 叶子（块解码、posting 编码、一个 bucket 的建图），要么一次阻塞调用（一张表的 checkpoint）。它跑在 blocking 池上，**只有它**跑在 blocking 池上。
- 任务在提交顺序上 join，所以结果**按输入顺序**收回，与哪个项先完成无关。
- 首个错误记为最终返回值，但**每个项都会跑完**：一次失败的 checkpoint 不能让后面的表没有镜像（否则它留下的半成品文件会被下一次 flush 当作完整镜像）。
- 没有可用 runtime 时（独立单测、example）串行执行，语义相同（每项都跑、首个错误返回）。
- 线程上没有已进入的 runtime 时（checkpoint 触发线程、同步 `Engine::execute_blocking`、CLI），使用 `scheduler()` 安装的 runtime 句柄；`current_thread` runtime 上不并发（单线程就是等待线程，spawn 会死锁），直接串行。

**为什么不用共享 work-stealing 池**：一个在 `fsync` 里阻塞的 worker 会被池当成“正在干活”的线程，而另一个纯解码的项却在等空闲 worker，两边都慢；把一个混合工作体整体丢进池里，等于把“等待”和“计算”混在同一个并发额度里。现在的形状是调用方先把工作**拆到一个阻塞单元**，再交给扇出——混合负载不会被整体搬到别的线程。

**边界**：`spawn_blocking` 只包细粒度纯 CPU 热点函数或单次阻塞调用，不包混合负载；并行本身一律用 tokio task + Semaphore。`workers == 1` 时串行，不做任何派发。

**错误传播**：单测 `a_failure_is_reported_after_every_item_ran` / `map_keeps_input_order` / `the_bound_is_the_callers` / `a_runtime_runs_the_items_on_tasks`（`index/src/parallel.rs`）与 `parallel_block_decode_matches_the_serial_scan`、`a_checkpoint_uses_the_worker_count_it_was_given`（executor）固定这些语义。

### 6.7 全局分配器：一处声明，全仓生效

`#[global_allocator]` 每个程序只能声明一次，但**每个可执行目标**（`infinity`、`src/bin/` 下的 11 个 helper、6 个 example、13 个 crate 的单元测试二进制）都需要它。逐个文件写一遍既重复又容易漏——实测就漏了：example 与单元测试原先都用 glibc，perf 里 37% 的时间落在 `_int_free_chunk` / `__libc_malloc2` / `cfree`，据此得出的“分配器是 checkpoint 瓶颈”是错的。

**做法**：声明放进 `common`（全仓唯一的叶子 crate，所有 crate 都直接或间接依赖它）：

```rust
// crates/common/src/lib.rs
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;
```

- `infinity`、`src/bin/` 的 helper、`catalog`/`index`/`executor` 的 example、各 crate 的 lib test 二进制因此全部自动继承，`cli` 与 `analyzer-ffi` 里原先的重复声明已删除（否则报 `conflicts with global allocator in: common`）。
- 唯一例外是 `cli/src/bin/gen_corpus.rs`：它只用 std，链接器不会把 `common` 拉进来，所以由它自己声明一次。
- `analyzer-ffi` 的 `cdylib`/`staticlib` 通过 `analyzer -> common` 拿到同一个 mimalloc；跨 ABI 的指针仍是 `libc::malloc`/`free`，所有权规则不变。

**效果（8 表 × 20 万行，8 workers）**：malloc/free 占比 **37% → 7.4%**（`mi_malloc_aligned` 3.98% + `mi_free` 3.41%），热点回到真正的工作（`write_blocks_mode` 6.9%、`pending_range` 的 `Value` 深拷贝 6.3%、`embedding_shape` 4.6%）。

### 6.8 全文索引的批量插入

参考实现的形状（`memory_indexer_impl.cpp:104-186`）：`MemoryIndexer::Insert` 把一批行包成 `BatchInvertTask(seq_inserted, ...)` 压进 `inverting_thread_pool_`，各任务并行分词倒排；结果按 `task_seq` 放进 `ring_sorted_`，`CommitSync` 只消费**连续前缀**（`ring.cppm:36-66` 的 `off_filled_` 逻辑），所以生成 posting 严格按行序。这是"并行计算 + 有序提交"。

移植分三段：

| 阶段 | 占比（20k docs，5000/批） | 并行 |
|---|---|---|
| tokenize（分析器） | 14% | ✅ 按块分给 worker（现在走 tokio task + `Semaphore`） |
| intern（词 → id） | 42% | ✅ 每个 worker 一份 `LocalDictionary`，最后一次性合并进全局字典 |
| apply（追加 posting） | 46% | ❌ 必须有序写共享 posting |

**关键取舍**：全局字典只能在应用线程写，所以每个 worker 先 interning 到自己的 `LocalDictionary`，合并时把本地 id 重映射为全局 id（`intern_one`）。这把"每个 token 一次全局哈希"降为"每批一次"——`intern` 占总时间 42%，正是此前 `LocalDictionary` 之前的最大单项。

**实测（20k 文档，5000/批，release）**：

| 词表 | 串行 | 批量并行 | 加速 |
|---|---|---|---|
| 400 | 116–119k docs/s | 119–139k docs/s | ~1.15× |
| 20 000 | 74–83k docs/s | 85–113k docs/s | **~1.32×** |

词表越大收益越明显（本地字典摊销得开）。小词表下本地字典几乎全命中，收益仅来自分词并行，故只有 1.15×。

**顺带修掉的两处纯浪费**（都出在单线程热点里）：
- `TextTokenizer::standard()` 每个文档都新建一个 `StandardAnalyzer`（tokenizer 配置 + stemmer），改为 `OnceLock` 缓存（实测该符号占 3.95%）。
- 分析器的线程本地 stem 缓存 `STEM_MEMO` 用默认 SipHash，改成 FxHasher；`live` 集合同样换成 id hasher（SipHash 合计曾占 11%）。

**不做**：把 apply 阶段也并行。posting 列表必须按 row id 递增，并行追加需要分段合并（参考实现同样是单线程 `CommitSync`），而 `insert_tokens` 已有二分插入兜底顺序，收益不抵复杂度。

### 6.9 次级索引插入与 WAL 编码

次级索引的插入顺序实测（20 万行、5000/批、2 个索引，release）：

| 配置 | rows/s |
|---|---|
| 无索引 | 529k |
| 仅 int 索引 | 427k |
| 仅 varchar 索引 | 415k |
| 两个索引 | 388k |

索引合计约占 28%。profile 里有两处确定的浪费：

- **`SecondaryIndex::row_count` 占 7.9%**：它每次 `map.values().map(Vec::len).sum()` 全表求和，而 `dump_secondary_over_capacity` 每个提交批次都问一次。改为增量 `rows` 字段（`insert` 加一、`take` 归零、`load` 重建）。
- **`SecondaryIndex::insert` 占 5.2%**：键是 `Vec<u8>` 的短串，默认 SipHash。换成 FxHash（与全文侧同一个实现）。

WAL 侧（`crates/storage/src/data_block.rs` 的 `Column::write_adv`）：

- 每个定长类型的值都 `extend_from_slice` 一次，5000 行的列因此反复 realloc 并拷贝。改为按类型宽度一次性 `resize` 再按偏移 `copy_from_slice`，并在列头前预留 `9 + 位图 + 行数×宽度`。
- `Str`/`Bytes` 每行两次 `extend_from_slice`（长度前缀 + 负载），合并成一次 `resize` + 两段写入。
- 空值位图直接在输出缓冲区里构造，不再先建一个临时 `Vec`。

**实测（同样方法各取 9 次）**：基线中位 **370k** rows/s（350–396k），优化后中位 **419k**（407–435k），**+13.3%**，区间不重叠。

**注意**：这些改动只动编码布局的实现方式，字节输出不变——`storage` 的 444 个测试（含逐字节的 `encode_block` 断言）全部通过。

### 6.10 Go SDK 测试与 Python SDK 对齐

两套 SDK 的测试规模接近（Python 308 项、Go 377 项），但 Go 侧有四类**实质性缺口**，本轮补齐：

| 缺口 | 补的内容 | 文件 |
|---|---|---|
| 二级索引只测"能建"，不测"查得对" | 低基数 int/bool/varchar、高基数、建索引后插入、checkpoint+删除 | `go/tests/secondary_index_test.go`（6 项） |
| 无 protocol error 用例 | type-3 帧 → `ErrorCodeProtocolError`，两种金样本 | `go/tests/protocol_error_test.go`（2 项） |
| NULL 只有零散覆盖 | 省略列、显式 NULL、`IS NULL`/`IS NOT NULL`、`count(*)`、NOT NULL 约束、默认值 | `go/tests/null_test.go`（6 项） |
| 索引边界缺失 | 空全文索引、无效选项、update 后建索引、无效 analyzer | `go/tests/index_edge_test.go`（4 项） |

补测试的过程中查出 **3 个真实缺陷**，都已修复：

1. **服务端不校验 session id**（`service.rs`）：`Disconnect` 之后旧 id 仍然可用，`ListDatabase` 等把 id 读完就丢。现在除 `Connect`/`Disconnect` 外一律回 `SessionNotFound`（3034，与参考同名同码）。
2. **Go SDK 丢弃 NULL**（`table.go`）：`parseColumnVectorsJoined` 根本不接收 bitmask，NULL 槽位解码成 0 或 `"null"` 字符串。现在按 mask 置 `nil`。
3. **Go SDK 不支持 `IS NULL`**（`expression_parser.go`）：解析器只有 `LIKE`/`IN`/`BETWEEN`，`IS NULL` 报 3069。现在解析成 `is_null(col)` / `is_not_null(col)`，与 Python SDK 的 AST 一致。

另外修复了一个**引擎缺陷**（`blocks/engine.rs`）：checkpoint 之后插入的行，若其 slot 超出块的 `row_count`，列式批量路径把它当作"被 version 隐藏"返回 NULL——于是带二级索引的查询查不到它，全表扫描却查得到。现在缓冲区的行优先于"隐藏"判定。回归用例 `a_row_inserted_after_a_checkpoint_is_visible_through_its_index`（对旧实现会失败）。

### 6.11 Go SDK 的批量插入为什么慢一半

补测试时发现 Go SDK 插入 2 万行要 1.73 s，而 Python SDK 只要 0.89 s。原因不是客户端语言，是**列顺序**：

- 服务端 `Insert` 处理把**列名列表相同的连续行**合并成一条 `INSERT`（`lib.rs` 的 `req.rows[i].0 == cols`）。
- Go SDK 用 `for columnName, value := range row` 构造列名列表。Go 的 map 遍历顺序是随机的，同一批 1000 行里每行的列序都可能不同，合并条件几乎永不成立。

实测（服务端 `INF_DEBUG_INS` 埋点）：

| 客户端 | SQL 语句数（2 万行） | 耗时 |
|---|---|---|
| Python | **20**（每批 1000 行） | 0.89 s |
| Go（修复前） | **4313**（多数 1–4 行） | 1.73 s |
| Go（列名排序后） | **20** | 0.68 s |

修复是给列名排序（`sort.Strings`）。修好后 Go 比 Python 快 24%——两边的服务端开销相同，差异全在语句数。

同一个 bug 模式在另外两处也出现，一并修掉：

| 位置 | 后果 |
|---|---|
| `Table.Insert` 的列名列表 | 批次被拆散，2 万行发 4313 条语句（已量化） |
| `Table.Update` 的更新表达式列表 | 单条语句语义不变，但列序随机 → 同一逻辑更新每次发出不同字节，WAL 帧不可比 |
| `Table.CreateIndex` 的 `InitParameter` 列表 | 同上：索引选项顺序被记进索引定义，两次相同的 `CREATE INDEX` 写出不同 WAL |

三处都改为按 key 排序后构造。回归：`TestInsertBatchIsOneStatement`（批量 vs 逐行耗时比，排序后 14.6×、去掉排序 7.1×）与 `TestUpdateCoversEveryNamedColumn`（漏一列即失败）。

**回归用例** `TestInsertBatchIsOneStatement`：4 列、4000 行，比较"一次批量插入"与"逐行插入"的耗时比。排序后 14.9×，去掉排序 7.1×，阈值取 10。两列的用例测不出来（Go 的两元素 map 顺序碰撞太频繁），所以用 4 列。

### 6.12 整链 async 化（已实施）

**目标**：请求从 volo handler 到引擎内部**全程 async**，`spawn_blocking` 只出现在少数经 profiling 确认的 CPU 密集函数上，而不是像现在这样把整个 `Engine::execute` 包进去。

#### 6.12.1 现状与两个具体问题

> 以下是**改造前**的快照（第 1、2 步之前），代码里已不存在这些写法；当前实现见 6.12.4。

`service.rs` 的 `run` 尾部：

```rust
let arm = executor::scheduler()
    .submit_async(job)          // runtime.spawn(async { spawn_blocking(job).await })
    .await?;
```

**问题 1（多余的包装）**：`submit_async` 内部是 `runtime.spawn(async move { tokio::task::spawn_blocking(job).await })`。`spawn_blocking` 本身返回 `JoinHandle`，可以直接 await，外面那层 `runtime.spawn` 每次 RPC 多一次 task 分配和一次 future 转发，没有收益。**这一层应当直接 `spawn_blocking(job)`**。

**问题 2（整条链被当成阻塞工作）**：被整个塞进 blocking 线程的 `dispatch_arm` → `Engine::execute` → … → `run_commit_bottom` 里，**大部分是 CPU 计算，但也有真正的等待**：

| 等待点 | 位置 | 性质 |
|---|---|---|
| WAL 顺序写 | `txn::WalWriter::run` | `send` + `recv` 等专用线程 |
| 行分配器 | `statements.rs` `reserve_rows` | `sync_channel` + `recv` 等分配器线程 |
| bottom 排空 | `state.rs` `BottomWork::wait` | `Condvar` 等另一个 bottom 任务 |
| 后台任务 | `bg_task::submit` | 队列满时 `Condvar::wait` |
| 文件 fsync | `write_synced` | 真阻塞 I/O |

前四项都是“等另一个线程给出结果”。它们在 blocking 线程上等待时**占着线程什么都不做**，这正是 async 化能换取的东西：等待改成 `await`，线程可以去做别的。

#### 6.12.2 规模（实测计数）

| 项 | 数量 |
|---|---|
| `Engine::execute` 的生产调用点 | **29**（`http.rs` 6、`server/src/bin/*` 13、`pg.rs` 1、`server/lib.rs` 1、其余 8） |
| 执行器内 `Mutex`/`RwLock`/`Condvar` | **48** |
| 需要 async 化的等待队列 | **4**（WAL、分配器、bg_task、bottom） |

#### 6.12.3 决定

- **A：`Engine::execute` 就地改成 `async fn`**。整条语句链（`execute_stmt_retrying` → `execute_stmt` → `execute_stmt_inner` → dml / statements / snapshot 的写入路径）全部是 `async fn`，四个等待点是真正的 `.await`：WAL 写线程的回复（`oneshot`）、行分配器（`mpsc` + `oneshot`）、bottom half（`oneshot`）、提交序轮次（`Notify`）。

  同步调用方（`cli`、helper 二进制、约 1500 个测试调用点）走 `Engine::execute_blocking`，它就是一个 `block_on` 适配器——同一个 future，不是第二条代码路径。DML / DDL 的写路径必须逐层 async：它们在提交里等 WAL、等分配器、等 bottom，这些等待如果留在同步函数里就还是要占线程。

- **B：只把跨 `.await` 的锁换成 `tokio::sync::Mutex`**。语句链里持有 guard 跨 await 的只有 `ddl_lock`、`commit_lock`、`sync_lock`、`flush_lock`、表级 `batch_gate`/`bottom_gate`；这些全部换掉（`std::sync::MutexGuard` 不是 `Send`，跨 await 会让 future 不是 `Send`，也编不过 `tokio::spawn`）。其余短临界区（`tables`、`result_cache`、`plan_cache` 等）保持 `std::sync::Mutex`——它们从不跨 await，换成 tokio 版本只是把一次原子操作变成一次可能的调度。

  这条边界不是风格选择：编译器的 `future cannot be sent between threads safely` 会精确指出哪把锁跨了 await，改到它不报为止，剩下的自然都是不该动的。

- **C：计算与阻塞 I/O 都派发到调用方 runtime 的 blocking 池**。`scheduler().spawn_blocking(f)` 把不应占用 async worker 的工作（一次 SELECT 的计划与执行、`fsync`、RocksDB 批写、`mmap` 缺页）交给 blocking 池并 await。调用方保留自己的 task、future 与连接，占用的线程来自 tokio 为该 runtime 扩张的 blocking 池——一个**正在执行**的语句一个线程，**在途**语句不占线程，这就是"10 秒请求 × 100K 连接"成立的原因。

  切分粒度：**SELECT 整段是一个扇出单元**（计划、tasklet、merge 全在内存数据上，块解码在内部按块用 tokio task 并行）；写路径按"等待→await，计算/IO→派发"逐步切开。

- **D：`block_on` 必须可嵌套**。`futures_executor::block_on` 在同一线程内重入会 panic（`cannot execute LocalPool executor from within another executor`），而引擎天然嵌套（`execute` → 某个 helper → WAL 回复 / 分配器 / bottom）。改为 `scheduler::block_on`：一个基于 `park`/`unpark` 的 poll 循环，任意深度可用。

#### 6.12.4 分四步，每步独立可验证

**第 1 步：去掉 `submit_async` 的多余包装 —— 已实施**

```rust
// before
self.runtime.spawn(async move { tokio::task::spawn_blocking(job).await.expect(...) })
// after
self.runtime.spawn_blocking(job)
```

零风险，消除每次 RPC 一次多余的 task。验收：`server` 测试；`Gate D` 不回退。

**第 2 步：4 个队列换成 `tokio::sync::mpsc` —— 已实施**

不改变调用结构，只换实现：阻塞的一侧用 `blocking_send` / `blocking_recv`（tokio 允许在非 async 线程上这么做），async 的一侧用 `send().await` / `recv().await`。行为与性能不变，但去掉了“async 上下文误用会死锁”的隐患，为第 3 步铺路。

| 队列 | 之前 | 现在 |
|---|---|---|
| WAL writer（`txn/src/lib.rs`） | `std::sync::mpsc::channel` + 回复 `mpsc::channel` | `tokio::sync::mpsc::unbounded_channel`（写线程 `blocking_recv`）+ `oneshot` 回复 |
| 行分配器（`statements.rs` 的 `Allocator`） | `sync_channel(1024)` + 回复 `mpsc::channel` | `tokio::sync::mpsc::channel(1024)`（消费者 `blocking_recv`，生产者 `blocking_send`）+ `oneshot` 回复 |
| bg_task | `VecDeque + Condvar`，`queued_count` 手工维护 | `tokio::sync::mpsc::channel(1024)`；`outstanding` 原子计数（提交前 +1、任务结束 -1）驱动 `drain` 的 Condvar |
| bottom（`BottomWork`） | `Mutex<BottomState> + Condvar` | `oneshot`：结果随消息走，`wait()` 只剩一次 `blocking_recv` |

验收：`executor` 全量 719 tests、`txn` 32、`server` 47；`Gate D`（并发写）不回退。

**第 3 步：`Engine::execute` 就地改成 `async fn` —— 已实施**

- `Engine::execute` 是 `async fn`；`execute_blocking` 是它的 `block_on` 适配器，供同步调用方使用。
- 语句链逐层 async：`execute_stmt_retrying`、`execute_stmt`、`execute_stmt_inner` 及 dml / statements / snapshot 的写入路径。
- 四个等待点全部 `.await`：`WalWriter::run`（`oneshot`）、`reserve_rows`（`mpsc` + `oneshot`）、`BottomWork::wait`（`oneshot`）、`turn_wait` / `wait_visible`（`Notify`，注册时先 `enable()` 再复查，避免丢唤醒）。
- 跨 await 的锁换 `tokio::sync::Mutex`（`ddl_lock`、`commit_lock`、`sync_lock`、`flush_lock`、表级 gate）；其余保持 `std::sync::Mutex`。
- SELECT 整体交给 `spawn_blocking`；块解码按块、posting 编码按批、HNSW 建图按 bucket，都走同一套 tokio task + `Semaphore`。
- 三个协议 handler 全部 await 语句：Thrift 的 `dispatch_arm` 与 HTTP 的 `route` 本身是 `async fn`，PG 直接 await `execute`。没有任何 handler 用 `block_in_place` 或 `spawn_blocking` 包住整条语句。
- 会话状态从 thread-local 改为 **tokio task-local**（`ACTIVE_SESSION`）：语句现在会跨 await 恢复在另一个 worker 上，thread-local 会在中途丢失会话。派发到池里的工作由 `with_pool_session` 带着会话进入，池线程无 task 上下文时回退到它。

验收（实测）：`cargo test --workspace` 全绿（49 个测试套件，executor 722、server 47、txn 32、storage 465、index 130）；`cargo fmt --check`、`clippy --workspace --all-targets -D warnings` 通过；`Gate A` 208/208；`many_statements_in_flight_do_not_need_a_thread_each`（64 个并发 INSERT+SELECT，runtime 只有 **2 个 worker**，全部完成——同步等待会在 2 个 worker 上死锁，这证明等待是 `.await`）。

**第 4 步：按 profiling 收窄 `spawn_blocking` —— 已实施（结论：只剩长驻消费者）**



第 3 步之后全仓只剩下面几处 `spawn_blocking`，逐条核对 + 一次真实 `perf` 采样，结论是都已归位，没有 CPU 热点留在 blocking 池：

| 剩余调用点 | 性质 |
|---|---|
| `scheduler().submit` / `spawn_worker`（`scheduler.rs`） | 每表 bottom 排空与行分配器的**长驻消费者**，持有队列直到进程结束，是真阻塞 |
| `scheduler().spawn_io`（`scheduler.rs`） | 真阻塞 I/O 的派发入口：`fsync`、RocksDB 批写、`mmap` 缺页。它 await 结果，调用方让出 worker |

**采样**（release，`infinity bench <dir> 500000` = 50 万行 INSERT + 全表 SELECT，`perf record -F 499`，742 samples）：

| 线程 | cpu_core/cycles 占比 | 在做什么 |
|---|---|---|
| `infinity`（语句线程） | **97.89%** | INSERT 编码、`encode_block_columns`、`write_block_column`、词法分析、块镜像写入 |
| `wal-writer` | 2.11% | `try_statx` / `File::write`，真文件 I/O |
| `rocksdb:high` | （cpu_atom 上 2.19%） | KV 后台 |

没有任何 `spawn_blocking` worker 线程持有 CPU 采样；三类计算（§6.13）各自在自己的并行单位上。所以「按 profiling 逐个决定」的结果就是**没有需要搬走的 `spawn_blocking`**，第 4 步收口。Gate D（p99）与 Gate E（<7% 回归）仍由 CI 持续回归，若将来 profile 出现新热点，本节重开。

#### 6.12.5 风险与回退（实施后复核）

| 风险 | 结果 |
|---|---|
| `tokio::sync::Mutex` 全局替换导致短临界区变慢 | **未发生**：按 B' 保留 `std::sync::Mutex`，没有这层性能风险，也不需要回退。Gate D 的 p99 仍随 CI 回归 |
| async 递归（`execute` 内部再调 `execute`） | **已处理**：`SELECT` 交给 `spawn_blocking`，子查询在同一个 `block_on` 里递归执行；`block_on` 是可嵌套实现，不会 panic，也不会多占一个池许可 |
| `block_on` 嵌套（引擎天然嵌套） | **已处理**：`futures_executor::block_on` 重入会 panic（`cannot execute LocalPool executor from within another executor`），改为 `scheduler::block_on`（park/unpark 循环，任意深度可用） |
| 会话状态跨 await 失效 | **已处理**：`ACTIVE_SESSION` 从 thread-local 改为 tokio task-local；派发到池的工作用 `with_pool_session` 带过去 |
| 会话的线程局部变量（`ACTIVE_SESSION` / `HeldBottom` / `PENDING_TXN_ID`）在 await 之间失效 | **未发生**：语句体整段在同一个线程上跑完（`block_in_place` 不迁移任务），线程局部状态没有跨 await 的机会。`SessionScope` 与 `HeldBottom` 的显式传递保留原样 |
| 一次性改 29 个生产调用点容易出错 | **已处理**：`execute` 就地改 async，同步调用方统一走 `execute_blocking`（`block_on` 适配器）；测试与 CLI 的调用点用同一适配器，不走第二条路径 |
| `dispatch_arm` 的 async 状态机过大 | **已处理**：约 1500 行 match 编成一个状态机，超过默认 2 MiB 栈（实测 `stack overflow`）；三个 server runtime 的 `thread_stack_size` 提到 8 MiB |

### 6.13 CPU 计算移出 async worker（已完成）

**原则**：async 层只做编排——读、合并、排序、提交顺序、背压。**计算本身**（块解码、posting 编码、HNSW 建图）不在 runtime worker 上跑，而是交给专用线程池。这跟 §6.12 是一体两面：6.12 把“等待”变成 `await`，6.13 把“计算”从会阻塞的上下文里拿出来，两边都不许占着 reactor worker 做别的事。

**两个池，职责不重叠**：

| 池 | 用途 | 判据 |
|---|---|---|
| tokio 扇出（`index::parallel::{for_each_blocking, map_blocking}`，§6.6） | **独立项**：每项自带输入输出，项之间只共享只读状态 | 项之间没有顺序依赖；一项阻塞不影响另一项 |
| tokio blocking（`spawn_blocking` / `block_in_place`） | **真阻塞**：`fsync`、`mmap` 缺页、写文件 | 主要是内核态时间，或等别的线程交结果 |

判错的代价是双向的：把纯计算塞进 blocking pool 会挤占本该留给 I/O 的线程（§6.5 的 1000 并发正是这么被拖慢的）；把 `fsync` 塞进 CPU 扇出则会占住一个 worker，让等算术吞吐的项空等。

#### 6.13.1 三类计算与现状

| 计算 | 入口 | 现在的执行位置 | 并行单位 | 现状 |
|---|---|---|---|---|
| 块解码 | `blocks/engine.rs` 的 `decode_rows_of_values`（`decode_block_cells` / `decode_block_column` / `decode_rows_of` 都汇到这里） | **runtime worker**：`run_forward` → `PhysicalSource::next` → `Scan::rows` → 解码 | 列之间互不依赖（`prepared` 每列一份，`scratch` 每行复用），块之间也互不依赖 | **已完成（按块）**，见 6.13.2 |
| posting 编码 | `engine/text_index.rs` 的 `SealJob::write` → `write_segment_index_streaming`（`mem.for_each_tail_term` / `for_each_merged_term` 逐 term 编码） | 每个 term 一个 tokio task（`map_blocking`）编码，写线程顺序追加（`disk_segment_reader.rs` 的 `flush_term_batch`） | 每批 `SEAL_BATCH_TERMS = 256` 个 term | **已完成**，见 6.13.3 |
| HNSW 建图 | `index/src/vector.rs` 的 `add_batch` → `parallel_build.rs` 的 `build_range` | 每个 bucket 一个 tokio task（`for_each_blocking`，`index/src/vector.rs`） | 每 bucket | **已完成** |

§6.6 的扇出粒度都是**跨表 / 跨索引 / 跨 bucket**；块解码与 posting 编码是**表内**的，没有被子句覆盖——这就是本节要补的缺口。

#### 6.13.2 块解码（已完成）

`decode_rows_of_values` 的循环是“每块 × 每列 × **每行**”，一块 8192 行、几十列时它是扫描路径的绝对大头。可并行的两个维度：

- **列**：每一列的 `Prepared`（body 切分、outline 种类、layout）互相独立。
- **块**：一个 fragment 的 `Scan` 手里可能有一批块，块之间无共享状态。

**选了按块拆**：列数少（1–3 列）时按列拆没有收益，按块拆在宽表和窄表上都有；一个块内部的列遍历保持原样，行序因此天然不变。

实现（`read_blocks_values`）：

1. 先串行选出候选块——fast rough filter 与索引的 `rows_wanted` 判定各是一次 catalog / set 查询，放在并行里只添乱；
2. `crate::fanout::map_blocking(blocks, workers, ...)` 逐块解码（一项一个 tokio task），结果**按块序**收回，所以行序与串行遍历完全一致；
3. `decode_block_visible` 把“开块 → 解码 → 按 `version` 过滤可见 slot”收成一个函数。它唯一可能碰到文件的地方是 `block_map`（首次映射）；映射命中时只读 `Arc<MappedBlock>` 与不可变的列字节。

**映射与解码分开**：`block_map` 未命中要 `Mmap` 打开文件，是 I/O；命中则只是查表加锁。并行体只做**已映射块**的解码，就保证了扇出里的每个 worker 都是纯 CPU——否则一次缺页就把一个 worker 钉在 D 状态，而另一个等算术吞吐的块在排队。

**有谓词时走串行**：`RowFilter` 是 `&dyn Fn(&[Value]) -> bool`，不是 `Sync`，不能跨线程共享。谓词路径本来也只会用于 `=` 常量（`is_equality_predicate`），它靠提前丢弃行省分配，不是 CPU 热点。

**worker 数**：`available_parallelism()` 与候选块数取小；一块时直接串行，不创建线程。

验收：`parallel_block_decode_matches_the_serial_scan`（3 个块 × 3 列，无谓词并行路径与 `read_blocks_filtered_values` 的串行路径逐行、逐列相等，且顺序即 row-id 顺序）；`executor` 全量 722 tests。

#### 6.13.3 posting 编码（已完成）

`SealJob::write` 的形状是“遍历尾巴的 term → 编码倒排 → 写 `.dic`/`.pos`/`.len`”。编码是纯 CPU，写文件是真 I/O，两者在同一个函数里，所以按段拆。

**实际做法**：`write_segment_index_streaming` 仍是唯一的写入口，内部按 `SEAL_BATCH_TERMS`（256）个 term 一批：

1. `for_each_term` 把 `(term, 命中列表, payload)` 收进 `TermBatch`。payload 在这一步就取好，因为 `payload_of` 借的是调用方的 map，编码时已经在另一个线程上。
2. 一批满就 `flush_term_batch`：`index::parallel::map_blocking(items, workers, encode)` 并行编码，每 term 产出 `(term, frame, TermMeta)`；结果按批内顺序收回。
3. 回到调用线程顺序 `pos.extend_from_slice`，并把该 term 的 `doc_start / pos_start / pos_end` 平移 `base`（本批之前已经写入的字节数）。偏移量只跟写出位置有关，所以并行不改变 `.pos` 的任何字节。

**为什么是分批而不是全量**：`for_each_term` 的流式形状本来就是为了让 seal 大尾巴时不把整段 posting 放进堆（§6.8）。分批保留这个上界（`256 × 单 term posting`），同时给池足够的活。整段一次并行会同时持有所有 term 的编码结果。

**批大小可注入**：`write_segment_index_streaming_with(..., batch_terms, ...)` 让测试能跑“每批 1 个 term”“整段一批”两个极端。**字节验收** `batching_does_not_change_the_bytes`（600 term × 200 行，`tf_flags`）：三种批大小的 `.dic` / `.pos` / `.len` 逐字节相同，且读回的 posting 与写入一致。

**没做全量并行写文件**：写是 `write_synced`（temp + rename），三次顺序写；把它并行化只会让 rename 竞争同目录。

**不进池的前提仍然成立**：`seal_text_index_unlocked` 先把写锁释放再调 `SealJob::write`（§6.9 的注释），所以编码是在无锁状态下派发的。将来若有一条路径在持 `tokio::sync::Mutex` 时编码，必须先释放——`spawn_blocking` 的闭包必须 `Send` 且跨线程，持着 `tokio::sync::Mutex` 的 guard 会破坏第 3 步的语义。

#### 6.13.4 HNSW 建图

Rust 版的批量建图按 bucket 扇出（`vector.rs` 的 `add_batch` + `parallel_build.rs` 的 `buckets_of` / `build_serial`，每 bucket 一个 tokio task），每条向量插入时只更新自己那一层的邻接表，bucket 之间只有读共享、写各自槽位，没有锁竞争。剩下的两个点：

- `note_hnsw_insert` 的**逐行**路径（`mem.insert(seq, &v)`）保留串行：HNSW 插入依赖已有的图，单条插入并行化没有意义，并行度在批量路径上。
- `build_hnsw_index`（`CREATE INDEX` 的 `IndexBuilder`）已经走 `insert_batch(&batch, hnsw_build_workers())`，即同一个有界扇出；`hnsw_build_workers()` 按参考的 `dense_index_building_worker` 取 `hardware_concurrency() / 2`（下限 2），**不另加 knob**——这是参考的固定口径，与 §6.6 那些“调用方自己要几个”的 checkpoint knob 不是一类。

#### 6.13.5 不移动的

- **有序应用**（`index_rows_prepared` 逐 doc 追加 posting）：必须按 row id 递增，参考实现同样是单线程 `CommitSync`（§6.8）。
- **单条语句的解析/规划**：微秒级，搬进池的成本高于收益。
- **任何持锁区间**：扇出会阻塞到所有 worker 收工，持锁进扇出等于把锁的等待时间乘上 worker 数。

#### 6.13.6 验收

| 项 | 判据 |
|---|---|
| 正确性 | `executor` / `index` 全量单测；Gate A 208 条；字节门禁 `_00…_70`（解码结果必须逐字节不变） |
| 计算确实离开 worker | `perf` 采样 runtime worker 线程上 `decode_rows_of_values` / `write_segment_index_streaming` 的占比降到噪声级 |
| 不退化 | `Gate C` 的扫描/建索引耗时、`Gate D` 的 p99 与改造前相比不回退 5% |
| 池不互挤 | 1000 并发下 blocking 池线程数不随语句数增长（§6.5） |

## 7. 衔接

- P/A 不混用；A 的每请求一次 `spawn_blocking`（thread-local → scope，见 A1）。

## 8. 路线

| 阶段 | 内容 | 产出物 |
|---|---|---|
| 1 | P1 | 清单 + API + 实现 + 插桩验收 |
| 2 | P2 | 生命周期契约 + 实现 + 等值验收 |
| 3 | P3 | 表级任务 + 表级 mirror 锁 + 尾巴临界区；字节/时间验收 |
| 4 | P4 | chunk 加载 + gap 补行 + 表级并行；启动时间/一致性验收 |
| 5 | A1 | 模型 + 实现 + 并发 USE 验收 |
| 6 | A2 | 入站硬上限 + pilota 容器校验 + 三协议验收 |
| 7 | A3 PG → HTTP → Thrift | 分阶段验收 |
| 8 | A4 整链 async 化 | 4 个队列换 tokio 原语；`execute` 就地 async；计算/IO 派发；`perf` 收口第 4 步；验收见 §6.12.4 |
| 9 | A5 CPU 计算移出 worker | 块解码 / posting 编码按项扇出；验收见 §6.13.6 |

## 9. 验收

| 项 | P | A |
|---|---|---|
| 正确性 | `cargo test --workspace`；Gate A 208/208；clippy/rustfmt | + sdk/http/pysdk；**显式列出并单独跑 `#[ignore]` 用例** |
| 并发 | — | Gate D/parallel SDK；1000 空闲连接线程数 < 50 |
| 字节 | `_00`/`_10`/`_20`/`_30`/`_40`/`_45`/`_50`/`_70`（`_60` 可选/常跳过） | 错误帧：volo 的 type 3 两个金样本（66 B / 84 B）+ SDK 映射用例 |
| checkpoint | 字节门禁；10k 表 wall time 随核数下降；单表不退化 | — |
| 启动（P4） | 不扫全表；chunk 加载 + gap 补行与参考一致；wall time 随核数下降，且随 gap 行数而非总行数 | — |
| 资源 | P1/P2 的插桩计数 | 6.2 的入站上限测试（12 字节大容器、巨大 header、慢读） |
| 规模 | — | CI：100 表 × 1k 行。手工门禁（10k 表 × 1M 行）：1000 空闲连接线程数 < 50、RSS 与 `memindex_memory_quota` + 索引预算相符（不设 total）、p99 不回退 5%、无 OOM；不承诺吞吐倍数 |

## 10. 风险

| 风险 | 处理 |
|---|---|
| checkpoint 不原子 | 只做表级并行，不动每表内部顺序；跨表原子性另立项目（§5.1） |
| 尾巴清理竞态 | 随 5.1 的 bottom gate 临界区一起修（§5.2） |
| 会话串库 | A1 |
| 协议长度/元素数攻击 | pilota fork 的容器校验（O13）+ A2 的头部上限 |
| 回复物化前已分配 | 与 C++ 一致，不做流式（§6.2.2）；风险保留 |
| 超时后已提交 | 照抄参考：无超时；写为“至少一次”，幂等由调用方承担（§6.4 / O1） |

## 11. 开放问题（未定项不算交付）

| # | 问题 | 需要的决定 |
|---|---|---|
| O1 | 写 RPC 的超时/重试语义 | **已定：选 (a) 照抄参考**。服务端无 deadline/取消（`TThreadPoolServer`，`thrift_server_impl.cpp:92-121`），写 RPC 无幂等键/去重，语义是“至少一次”；唯一自动重试是 Python SDK 的 `retry_wrapper`（10 次、开新 socket、沿用旧 `session_id`，`client.py:84,183-207`），保留。调用方要幂等就自己用 `ConflictType::Ignore` 等。实现侧：逐 RPC 文档化重试后果（INSERT 重复 / UPDATE 多一版 / DDL 二次 already exists / DELETE 可重入）；未知 session 回 `SessionNotFound`（对齐参考，见 A1）；不新增 request id 字段 |
| O2 | `catalog_only` 的索引点查 | **已定：用内存索引目录解决，不接受退化**。不写原生 `idx:` 键（字节门禁比较 catalog 全量键值），也不接受“每次有界前缀扫”。P1 用与 `table_cache` 同构的缓存：每表首次访问做一次 `catalog|idx|{db}|{tbl}|` 有界构建，之后点查 O(1)；DDL epoch 失效、`capture` 期间绕过。验收见 §4.1 |
| O3 | 二级索引快照 | **已定：移植参考的 RecoverMemIndex，纳入本计划（P4）**。启动不重建 chunk：按 `idx_chunk` 记录打开已发布 chunk（含 `catalog_only` 的 `idx_*/` 副本）、填 `secondary.disk/low/chunk_meta`；按段合并 `[base, base+row_cnt)` 求 gap，只对 gap `AppendIndex` 式补行；按表并行。新快照格式/侧车仍不做（复用 C++ chunk）。详见 §5.3 |
| O4 | checkpoint 崩溃一致性 | **现状**：块文件原地重写、`version` 最后（`format.rs:443-465`），catalog 先删 `blk|` 记录再发布（`blocks/engine.rs:529-555`），`append_checkpoint_flushed` 的错误被丢弃（`admin.rs:447-449`），最后才 `store.flush()`。窗口：(1) `.col` 已写、`version` 未写；(2) 旧记录已删、新记录未发布；(3) 旧 `seg_*` 目录已删、新段未发布。参考实现同样不原子，WAL replay 只补事务、不补 checkpoint 的文件写。修法 = staging 目录 + 原子 rename + 单批次 catalog 交换 + intent journal + 启动清理；C++ 读端只认固定路径（`block_meta_impl.cpp:206-218`），不能加 generation；字节门禁还会看到 staging 残留。独立项目 |
| O5 | 内存预算的参数面 | **已定：只保留 `[buffer] memindex_memory_quota` 一个绝对值**（默认 4 GiB，与参考同名同义），管全部内存索引（全文/稀疏/向量/二级）合计，超了就 dump。**不设 `total`**（不按 RAM 比例推总额，也不做请求级内存准入；参考同样没有；请求侧也不设并发许可，§6.5）；**不统计 mmap**（封存块与二级 chunk 是只读映射，64 位地址空间远大于物理内存，映射量不是用量，驻留交给内核回收）。`buffer_budget`/`hnsw_budget` 的内置 8 GiB 与 `mem_index_capacity` 是实现常量/行数触发，不是预算 |
| O6 | HTTP 大响应 | **已关闭（不做）**：C++ 参考同样整包返回，流式化需要引擎侧改造（§6.2.2） |
| O7 | 结果/回复的流式化 | **已关闭（不做）**：整包物化与 C++ 一致，与 O10 一并取消 |
| O8 | `spawn_blocking` 的池归属 | **已关闭：服务端框架就是 volo-thrift，池归属不再是选择题**。accept 循环是 `tokio::spawn(handle_conn(...))`（`volo-thrift/src/server/mod.rs:299`）、service 共享、worker = 核数，连接不占线程、server 不自建执行池——参考的“1 reactor + ThreadManager(pool_size)”被“volo runtime + `scheduler()` 阻塞池”一对一替换。剩下的是 A3 的两条实现约束（§6.5）：handler 把 `Engine::execute` 移出 reactor worker（`scheduler()` 异步桥，退路 `spawn_blocking`）、阻塞池**不设** `max_blocking_threads`（`cores+4` 已废除）。**连接不设上限**：每连接只是一个 task，空闲连接不占线程（实测 1000 条 → 23 线程），参考的 `connection_pool_size` 没有对应物。请求许可已取消（§6.5） |
| O9 | 两阶段解析的 RPC 清单 | **已关闭**：A3 采用 volo（O11），49 个方法由 IDL 生成，不再需要逐 RPC 解析清单 |
| O10 | 引擎侧流式结果 API | **已关闭（不做）**：A2b 取消，不再需要；将来要做作为独立项目重开 |
| O11 | Thrift 是否采用 volo-thrift | **已定：采用**。IDL（含 `&`，走 fork 补丁）可生成、unframed codec 可用、`CommonResponse` 与 `SelectResponse`（9 个列类型用例）逐字节一致，回归用例受 git 管理。实现前复核运维门禁：1000 空闲连接线程数 < 50 |
| O12 | 未知方法/非法头的错误帧 | **已定：接受 volo 的 `TApplicationException`，改 SDK 期望**。金样本（2026-11-09 实测）：未知方法 **66 B**（type 3、field 1 = `unknown method <name>`、field 2 = 1）；头非法 **84 B**（空方法名、seq 0、field 1 = `invalid thrift version: unknown protocol, first byte: N`、field 2 = 7）；空帧不回复、连接保持。SDK 把 type 3 映射成 `InfinityException` 且不重试；不再保留“未知方法静默关闭” |
| O13 | volo 下的容器长度校验 | **已补（fork `74f14ab`）**：pilota 的 `read_{list,set,map}_begin` 现在拒绝负长度（`NEGATIVE_SIZE`）、超过上限的元素数（`SIZE_LIMIT`，默认 1M，`set_max_container_size` 可配，0 关闭），并在有完整缓冲的协议里按“剩余字节 / 最小元素尺寸”校验（官方 `checkReadBytesAvailable` 的等价物）。async 流式协议只有符号与上限两条（unframed 没有长度前缀）。解码侧校验，**合法帧的字节不变**：10 个回归用例对着打了补丁的 crate 跑过 |
| O14 | volo 下 A1 的连接身份 | **已关闭**：A1 改为按 `session_id` 建模（与参考一致，`Disconnect` 才释放），volo 的共享 service 不需要连接身份 |
| O15 | 表级并行的锁 | **已定：表级**。`MIRROR_WRITE_LOCK` 收窄成 `table_key → Arc<Mutex<()>>`（取 map 锁只 clone `Arc`，出 map 再 lock；drop table 删条目），锁序仍是 `tables` → mirror 单向。**block 级无意义**（块写已在 bottom gate 内，同表无第二写者，且保护不了跨段的 `seg_*` 陈旧目录清理）；**segment 级会坏事**（`publish_segments` 整表一次提交、`live` 集合按表算，分持不同段会交错“写段”与“删陈旧段”）。索引写仍在表任务内串行，不需要单独锁 |

## 12. 仍未处理完的条目（诚实清单）

| r9 条目 | 状态 |
|---|---|
| #9 两阶段解析缺 RPC 清单/状态模型 | **已关闭**：A3 采用 volo（§6.3.2），清单由 IDL 生成 |
| #10 错误/取消契约对每个 RPC 未定 | **已定**：应用错误走 `error_code`；协议错误用 volo 的 type 3 帧（O12），SDK 映射且不重试；写 RPC 照抄参考（O1=a） |
| #8 账本的“每次实际分配如何计费、容量何时释放” | **已关闭**：O5 定为不设账本，入站只保留协议硬上限，逐分配计费不再需要 |
| #5 交付验收里混入未决项 | **已改**：`catalog_only` 的条件化（§4.1 第 3 条 + O2） |
| #4 `ALLOW_ZERO_SPARSE` 等语义线程局部 | **不改（复核后撤回）**：这些标志由各自语句的调用点成对设置/恢复，已在语句内闭合；把它们并进一个 `StatementScope` 只增加一层间接，不解决任何残留问题。§6.1 已相应修正为“只有 `PENDING_TXN_ID` 在语句作用域内” |

| r10 条目 | 状态 |
|---|---|
| r10 #1 回复在 `Engine::execute` 内已物化 | **不做**：C++ 参考同样整包物化（§6.2.2），A2a 只承诺入站 |
| r10 #2 session 状态跨 `spawn_blocking`/panic 的所有权 | **已关闭**：会话不放进闭包，`Engine::with_session` 在**执行线程**上装作用域，用 `SessionScope`（`Drop`）恢复，因此 panic 展开也会恢复；回归用例 `a_panicking_statement_does_not_leak_its_session` |
| r10 #3 P1 API 吞掉后端错误、未定义 legacy key | **已关闭**：`indexes_of_table` / `index_by_name` 返回 `Result<Option<..>>`，`catalog_only` 下后端错误原样上抛（不再 `unwrap_or_default`）；legacy `idx:{name}` 键的策略是**写时清理、读时不解析**——`CREATE INDEX` 与 `DROP INDEX` 在同一个 `atomic_batch` 里删掉属于该表的 `idx:{name}` 记录（`create_index_with_a_legacy_record_is_one_batch`），读路径只走 `catalog|idx|{db}|{tbl}|` 这一套键，所以旧记录既不会让索引出现两次，也不会在点查里被当成第二个来源 |
| r10 #4 P2 全局总量的更新点/不变量 | **已关闭**：更新点固定为四处——① 追加/更新后的 bottom（`dml.rs` 的 `refresh_memindex_charge(&bottom_table)`，持该表 bottom gate）；② 每次 dump 完成（`dump_index_meta` / `dump_index_transaction` 各在 seal + publish 之后）；③ 配额检查前（`enforce_memindex_quota`，具名单表则只刷该表，否则全量重算）；④ checkpoint 末尾 `recompute_memindex_total()` 全量对齐。删表在 `drop_table` 里先移除 `TableEntry` 再全量重算（条目连同它的计费一起消失）。不变量：`memindex_total == Σ memindex_bytes_of(tkey)`，由 `memindex_total_matches_a_full_scan` 在追加/checkpoint/删除/删索引/删表五个点逐一断言 |
| r10 #5 每次分配如何计费/嵌套/释放 | **已关闭**：不设账本（O5），改为协议硬上限 |
| r10 #6 Thrift 解析的 RPC 清单/状态机 | **已关闭**：随 O9 取消 |
| r10 #7 写结果/超时的逐 RPC 契约 | **已定**：不做超时；逐 RPC 写清重试后果（O1=a） |
| r10 #8 池与准入的具体数值 | **已定（改判两次）**：服务端框架由 volo 提供（每连接一个 task，不占线程）；**请求侧不设许可**（按核数封在途请求会让高并发客户端排队饿死，§6.5）；阻塞池**不设** `max_blocking_threads`——`cores+4` 会让池成为本已由核数约束的工作的瓶颈，fragment 改 async task 后阻塞项只剩 bottom 排空、分配器、sink 三样。**连接不设上限**（空闲连接不占线程，实测 1000 条 → 23 线程），`ConnGate`/`INFINITY_MAX_CONNECTIONS` 已随 A3 删除（O8） |
| r10 #9 尾巴清单缺 WAL replay 调用 | **已改**：补充 `wal_bridge`/replay 的清理调用点 |
| r10 #10 规模门禁无判据 | **已改**：见 §9 的判据 |

| 后续交付 | 状态 |
|---|---|
| A4 整链 async 化（§6.12） | **四步全部实施**。① ~~去掉 `submit_async` 多余的 `runtime.spawn`~~；② ~~4 个等待队列换 tokio 原语~~（WAL、行分配器、bg_task、bottom）；③ `Engine::execute` 就地改 `async fn`，`execute_stmt_inner` / dml / statements / snapshot 逐层 async，四个等待点全部 `.await`，跨 await 的锁换 `tokio::sync::Mutex`，计算与阻塞 I/O 都由 `spawn_blocking` 派发，会话改 task-local，`block_on` 换成可嵌套实现；④ `perf` 采样确认无 CPU 热点留在 blocking 池。Gate D / Gate E 的 p99 由 CI 持续回归 |
| A5 CPU 计算移出 worker（§6.13） | **三类全部实施**。块解码：`read_blocks_values` 选出候选块后按块 `map_blocking`（一项一个 tokio task，保序收集）；无谓词时并行（`RowFilter` 是 `&dyn Fn`，不 `Sync`）。posting 编码：`write_segment_index_streaming` 按 256 term 一批 `map_blocking` 编码、顺序追加，`TermMeta` 偏移按写出位置平移。HNSW 建图：每个 bucket 一个 tokio task |

这些条目不阻塞本文档作为“计划”使用，但**不能当作已交付**；A3 的错误帧以 O12 的两个金样本为准（已定）；P3/P4 的锁粒度已定（O15 表级），无前置未决项。

## 13. 实现状态（2026-11-10）

| 交付 | 状态 | 代码 | 验收 |
|---|---|---|---|
| P1 | **已实现** | `Catalog::indexes_of_table` / `index_by_name` + `index_cache`（与 `table_cache` 同构：DDL 清空、epoch 发布、capture 绕过）；43 处生产调用点改点查（`list_indexes` 只剩 15 处全量管理场景：OPTIMIZE/CLEANUP/SHOW/SNAPSHOT） | `catalog` 57 tests；`indexes_of_table_matches_list_indexes_and_follows_ddl` 断言预热后 `scans == 0`、DDL 后重建时 `full_index_scans == 0` 且前缀扫 ≤ 8 |
| P2 | **已实现** | `Context::memindex_total` + `TableEntry::memindex_bytes`；append/dump/seal/compaction/drop 按 delta 维护，checkpoint 全量重算 | `memindex_total_matches_a_full_scan`（追加/checkpoint/删除/删索引/删表逐点相等） |
| P3 | **已实现** | `flush_blocks` 一表一任务（`INFINITY_CHECKPOINT_CONCURRENCY`，默认核数）；每表 bottom gate 内写块 + 清尾巴；mirror 锁改表级 | 语义门禁 `concurrent_appends_survive_a_checkpoint_and_reopen` 等 684 tests 全绿 |
| P4 | **已实现** | `rebuild_indexes` 按表并行（`INFINITY_REBUILD_CONCURRENCY`）；二级索引按 `idx_chunk` 映射已发布 chunk、只补 gap | `reopen_loads_secondary_chunks_and_fills_only_the_gap`：chunk 1 个、`chunk_meta=[(0,3)]`、尾巴 1 行、查询正确 |
| A2a | **完成** | pilota fork 的容器长度/元素数校验（O13）；HTTP 头部 64 KiB 上限 | pilota 192 tests；`an_endless_header_is_rejected`（431） |
| A1 | **完成（按参考收敛）** | `open_session`/`close_session`/`session_db`/`execute_in_session`；`Connect`/`Disconnect` 注册；`SessionScope`（Drop 守卫，panic 也恢复）覆盖整条连接。**与 §6.1 的模型说明有出入，以参考实现为准**：`SessionContext` 只有 `current_db`，没有 `variables`/`generation`——参考的会话变量（`query_count`、`total_commit_count`、`connected_time`）是引擎级只读计数，`SET VARIABLE` 在 Session 作用域一律 `read-only`（`physical_command_impl.cpp:68-150`），所以没有 per-session 变量表可存；`statement` 作用域由 `PENDING_TXN_ID` 的惰性分配实现（`ensure_statement_id`），`begin_statement` 是 no-op | `sessions_keep_their_own_database`、`a_panicking_statement_does_not_leak_its_session` |
| A3 | **完成（三协议）** | Thrift：`crates/server/src/service.rs`：`volo-thrift` 从 IDL 生成的 49 个 `InfinityService` 方法；`Server::run` 变成 tokio runtime + `InfinityServiceServer`（unframed `MakeThriftCodec`）；请求由 volo 解码后**重编码成参数字节**交给既有的 `dispatch_arm`（SQL 渲染只有一份实现），回复由生成的 struct 直接构造（`SelectResponse` 的列向量零拷贝搬进 `Bytes`）；`Engine::with_session` 给每条语句套上会话作用域；每个请求不取许可，直接在 handler task 内 `.await` 语句（`dispatch_arm` 本身是 async fn；会话用 task-local 作用域，§6.12.4 第 3 步）。PG 与 HTTP 同步改造：`PgServer::run` / `HttpServer::run` 都是 tokio runtime + 每连接一个 task，语句在调度池上跑、帧在 reactor 写；PG 的帧构造仍由 `PGProtocolHandler` 提供（同步单测继续覆盖字节），HTTP 保留 64 KiB 头部上限 | `server` 46 tests；`tools/volo_wire/test_wire_parity.py` 9/9 字节相同；pysdk 全量（见下）；1000 空闲连接 → 23 线程 / 109 MB（debug） |

| 并行原语收口（2026-11-11） | **完成**：扇出（checkpoint 扫表、启动重建、批量 dump、HNSW 建图、块解码、posting 编码）统一为 `index/src/parallel.rs` 的 `for_each_blocking` / `map_blocking`（每个项一个 tokio task，`Semaphore` 限并发，项体走 `spawn_blocking`），**rayon 依赖已移除**，也不再自建线程池；行分配器队列从无界 `mpsc::channel` 改为 `sync_channel(1024)`；请求许可的取舍写进 §6.5（最终取消：RPC 在等而非在算；阻塞池上限已废除） | `index` 131 tests、`executor` 722、`server` 47、`storage` 465（49 个套件全绿）；`a_checkpoint_uses_the_worker_count_it_was_given` 断言两个 worker 数下每张表都被镜像 |
| 全文批量插入（2026-11-10） | **完成**：`update_text_indexes_batch` + `index_rows`——一批行按 `par_chunks` 并行分词、各 worker 用本地字典并行 interning，再按行序应用；20k 文档 / 5000 一批实测 74–83k → 85–113k docs/s（大词表 **1.32×**）。同时修掉每文档重建分析器（3.95%）与 stem 缓存的 SipHash（11%）| `executor` 684 tests；`a_large_batch_matches_the_row_by_row_postings`（5000 行 × 2000 词表，批量与逐行两条路径对 4 个词的检索结果逐行相等）|
| §9 单元测试 | **已跑（2026-11-10 收尾复跑）** | `cargo test --workspace --lib`：common 89 / storage 444 / catalog 57 / txn 32 / parser 44 / expression 43 / analyzer 63 / analyzer-ffi 0 / planner 73 / index 130 / executor 684 / server 46 全通过（0 failed）；`cargo fmt --all --check`、`cargo clippy --workspace --all-targets` 干净 |
| §9 并发门禁 | **达标（2026-11-10）**：1000 空闲连接 → **23 线程**、RSS 109 MB（debug；A3 之前是 1009 线程 / 205 MB）。连接不再占线程，因此不设连接许可：参考的 `connection_pool_size` 只用于约束线程，`INFINITY_MAX_CONNECTIONS` 随 A3 删除；**请求许可也已取消**（按核数封在途请求会让 10k 客户端排队，见 §6.5）。**同时在线用例**：`go/tests/parallel_clients_test.go::TestParallelManyClientsStayOnline`（`-tags parallel`，与 Gate D 其余用例同一个 job）——每个客户端在自己的 session 上循环 `ListDatabases`（成功即打心跳）后 `sleep 500ms`，采样器统计“最近 5 s 内有过成功语句”的客户端数；**通过条件是 5 000 且在 30 s 内从未跌破**（`INFINITY_CONCURRENCY_CLIENTS` 可调高）。5 000 而非 10 000：客户端与服务端共用 runner 的 4 vCPU，10 000 会话实测只到 8 938（arm64）/7 819（amd64），8 000 也会跌破；结构性的结论（远高于参考的 256 线程池）在 5 000 已成立。用 Go 而不是 pysdk：CPython 的 GIL 让活跃客户端成为测试端瓶颈（实测），会测到 harness 而不是服务端。 |
| §9 正确性门禁（Thrift A3 之后复跑，2026-11-10） | Gate A **208/208 PASS**；字节门禁 `_00/_10/_20/_30/_40/_45/_50/_70` **40 passed / 1 skipped**；pysdk 逐文件全绿（`test_index` 961、`test_knn` 373+6 skipped、`test_delete` 377、`test_py_import` 234、`test_select` 66、`test_insert` 51、`test_update` 48、`test_table` 27、`test_show` 24+1、`test_config`/`test_export`/`test_py_import` 等）；`cargo test --workspace --lib` 全通过（0 failed）；`cargo fmt --all --check`、`cargo clippy --workspace --all-targets` 干净 |
| checkpoint / 启动 wall time | **已测（2026-11-10，16 核）**：`a_checkpoint_scaling_probe`（64 表 × 2 索引 × 2048 行，预热一次全量写块后每次只脏一个块）——1 worker **159 ms**、2 workers **131 ms**、4 workers **129 ms**、8 workers **192 ms**。4 个以内有真实收益，再多则调用方的 store/catalog 串行段占主导、线程只增竞争，因此默认核数但收益上限在 4 左右；全量首写（94 s）不受并行影响。启动侧由 P4 的 `reopen_loads_secondary_chunks_and_fills_only_the_gap` 覆盖（只补 gap，不重建 chunk）。 |
| PG / HTTP 异步化的验收 | `server` 46 tests（`http_over_socket`、`pg_simple_query_over_socket` 等直接跑在 tokio 循环上）；手工冒烟：`curl POST /query` 建表/插入/查询正确，`psql -p <pg>` 查询同一张表返回同一行。门禁复跑（Gate A / 字节门禁 / pysdk）留到最后统一做，本次未逐项跑 |
| §9 期间修掉的两个真实缺陷 | ① 重启后二级索引没有 `secondary.indexes` 条目，`reopen` 之后写入的行永远不进索引、`DumpIndex` 也封不出 chunk（`load_secondary_index` 现在总是登记内存尾）；② `COMPACT` 之后合并段没有次级索引 chunk（参考在压缩事务里 `PopulateIndex`），现在 `compact_segments` 末尾重建该表的次级 spill |

## 附录 A：证据

| 主张 | 证据 |
|---|---|
| dump 1.51 s / 11.8 ms | `flush_dump_wall_time_probe`（64×256，单次） |
| 原地重写/先删后写/标记丢弃 | `format.rs:443-465`、`layout.rs:2226-2293`、`admin.rs:447-449` |
| 进程级 `current_db` | `state.rs:4319-4320`、`query.rs:388-405` |
| 46 处生产调用 | 75 − 28（测试模块）− 1（定义） |
