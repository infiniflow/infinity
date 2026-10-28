# 移植纲领

`rust_engine` 是 C++ 参考引擎（`src/`）的 Rust 重写版。本文是这次移植的纲领：**目标与判据**（§1）、**方法论**（§2）、**当前状态**（§3）、**当前缺口**（§4），以及把整件事拆成的**子目标**（§5）——每个子目标在 `docs/` 下有一份台账，记录它此刻的状态。

里程碑与排期、决策矩阵（D1–D6）、Gate 的定义、风险登记册在仓库根的 [`RUST_REWRITE_PLAN.md`](../../RUST_REWRITE_PLAN.md)，本文不重复，只记录"现在是什么样、为什么这样判"。

---

## 1 目标

### 1.1 完成的定义

三条同时成立才算移植完成：

1. **格式等价**——同一输入下，两个引擎写出的 `data/`、`catalog/`、`wal/` 逐字节相同（参考自身不确定的部分按内容比，清单见 `FORMAT_EQUIVALENCE_TESTS.md` §5）。
2. **行为等价**——同一 SQL / 协议输入，两个引擎的回答相同：`.slt` 全量（Gate A）、SDK 与 PG wire（Gate B）、重启与崩溃恢复（Gate C）、并发（Gate D）。
3. **可交接**——任一引擎能打开对方留下的目录继续读写，参考能 replay 重写版写的 WAL，重写版能从参考的 WAL 恢复。

判据是**参考**，不是"能跑通"。重写版自己前后一致、或者两版都能读同一份数据，都还不够：`data/` 里一个字节的差别意味着某处的布局、对齐、排序或填充与参考不同。

### 1.2 等价的三个层级

| 层级 | 断言 | 用途 |
|---|---|---|
| 字节 | 同一输入，两引擎写出的文件逐字节相同 | 最强的格式判据，能看见任何布局漂移 |
| 内容 | 不可字节比的文件（参考随机或依赖线程调度）解码后比"索引说了什么" | HNSW 图、IVF 质心、catalog 字节 |
| 行为 | 两引擎回答同一批查询的结果相同 | 语义等价，但不保证格式 |

门禁按这个顺序排：能字节比的绝不只做内容比，能内容比的绝不只做行为比。三层各自的边界与"为什么只能到这一层"登记在 `FORMAT_EQUIVALENCE_TESTS.md` §1、§5。

### 1.3 非目标

- **不移植**：DiskANN、geo 列类型、远程对象存储的 `obj_key` 打包上传（`GAPS.md` §1）。
- **不复刻参考的不确定性**：HNSW 的 `thread_local` 随机图、IVF 的 `random_device` 采样按"参考实际写出什么"写或按内容比，不追求逐字节相等；能靠改参考消除的不确定性不在此列，按 §2.2 处理。
- **不"顺手改进"**：与参考逐行对应的实现不改写成 Rust 惯用法；确需设计变更（调度器、FST、SIMD 原语、RocksDB 事务型 KV）必须登记在 `CLASS_MAP.md`「有意的设计变更」，并说明为什么可以不等价。

---

## 2 方法论

### 2.1 参考是唯一真值，而且必须实测

- 任何格式结论都要能追到 `文件:行号`（`CPP_FORMAT_SPEC.md` 的规矩），不得凭推测实现；没核对的进"待核对清单"。
- **不拿黑盒拟合**：`.slt` 的期望值、SDK 的行为都是跨 revision 生成的黑盒，对不上时先去量参考（容器内构建 C++ 引擎，用 `psql`/SDK 取真值），不要调常数迁就。Gate A 从 1478 推到 1536/1536，靠的是量出 tokenizer 的逐字节分类，而不是拟合 `len/avg`。

### 2.2 参考的不确定性要修参考

同一输入两次跑出不同字节的地方，比较本身就失去意义。遇到这类差异，先问"能不能让参考确定"，而不是给端口留一个随缘的字节：

- **未初始化内存被写进格式**（`IndexIVFOption` 的两段 padding）→ 在参考里补成显式字段，端口按同一布局写 0；
- **注释掉或漏掉的赋值让字段打印未初始化值**（快照的 `max_commit_ts`）→ 在参考里补上；
- **不稳定的排序 / 遍历决定落盘顺序**（稀疏 chunk 的行序）→ 参考改成稳定顺序。

改过参考必须重编参考二进制，两个引擎的比较才有意义；改动本身在格式文档登记（`FORMAT_EQUIVALENCE_TESTS.md` §6 的 `IndexIVFOption` 一条就是先例）——它正是"这一项现在能逐字节比"的依据。

**测试用例也不能自己引入不确定性**：多客户端并行、后台线程（checkpoint、compact、optimize、cleanup、配额驱动的 memindex dump）都会把写入落在调度决定的时刻上——事务序列、chunk 边界、段布局随之变成"跑几次就几种结果"。所以门禁用单客户端串行，把能关的周期设成 0、把配额设到内存索引达不到的值，让每个事务都由脚本决定（`python/restart_test/bytes_common.py` 的 `_config`）；并发本身要覆盖的功能由 Gate D 单独承担，不进字节门禁。

只有不确定性真的不可消除时才降到内容比较：HNSW 的 `thread_local` 随机图、IVF 质心的 `random_device` 采样、依赖线程调度的后台 chunk 切分。这些在 `GAPS.md` §3 登记原因与门禁的处理方式。

### 2.3 先冻结格式，再写实现

磁盘格式（列块、`version`、outline、倒排三件套、catalog 键空间、WAL 帧）先写成带行号引用的规格（`CPP_FORMAT_SPEC.md`），再照规格实现。反过来做——先写实现、再对齐字节——会把"实现细节"固化成"格式"，且每次差异都要重新读参考源码。

### 2.4 台账驱动：不让"看起来实现了"蒙混

- `CLASS_MAP.md`：参考每个 `export class/struct` 一行（`mapped` / `partial` / `test` / `exempt` / `excluded`），以及关键类的方法级对应与登记过的设计变更。
- `TEST_AUDIT.md`：参考每个 `TEST/TEST_F/TEST_P` 一行，`ported` / `excluded` / `exempt`。

`partial` 不是"差不多"，是"生产路径只走到一部分"，必须写清缺哪一段。两份台账是"覆盖"的唯一口径：没有行的功能不算覆盖，没有对应测试的类不算移植。

### 2.5 门禁分层，修一个差异沉一条测试

每个差异的收口动作固定：**先让门禁红**（能复现的用例）、**再改代码**、**最后让门禁绿**并留下那条用例。门禁分五档（SQL `.slt`、SDK/协议、重启与恢复、并发与稳定性、性能），格式与不变量另有两条（`FORMAT_EQUIVALENCE_TESTS.md`、`INVARIANT_MATRIX.md`），跑法见 §6。

单测与门禁的分工：单测钉住**机制**（一个函数的边界、一个布局的字节），门禁钉住**整体等价**（整目录、整协议、整恢复）。两者都要——缺一个就会出现"单测全绿，但目录读不出来"。

### 2.6 文档只写现状

文档描述"参考怎么工作、重写版怎么对应、哪里不同、怎么验证"。移植过程、修复顺序、评审轮次不写进文档，需要时看 `git log`。差异用对比表；缺口只写"未覆盖什么 + 为什么"，不写"原先如何、现已修复"。

### 2.7 并行结构也是等价的一部分

"结果一样"不等于"结构一样"。线程池、分片键、锁的层次、后台队列和等待点决定了事务何时可见、同表是否有序、跨表是否并行——它们在并发负载下会变成可观察差异，也会让 `EXPLAIN`/统计类输出对不上。对齐时要能对着参考的 `文件:行号` 回答四个问题：

- **在哪跑**：每类工作落在哪个线程上——提交线程、池里的 worker，还是单线程后台队列。参考的下半部是 `BottomExecutor` 的 worker（`bottom_executor_impl.cpp:32-78`，按 `crc32(table_id) % n` 分片），全文倒排是单独的 `MemIndexAppender`（`new_txn_index_impl.cpp:891-894`），执行期是 tasklet（`fragment_context_impl.cpp:1215-1245`）。
- **谁等谁**：等待点决定可见性。上半部在 `commit_cv_` 上等下半部（`new_txn_impl.cpp:2138-2139`），`PostCommit` 等索引写入的 sema；端口对应 `register_bottom`/`finish_bottom` 屏障和 `run_commit_bottom`/`run_text_append` 的等待。语句返回时"什么已经可见"必须一致，否则客户端会看到半成品。
- **锁的层次与顺序**：分片锁在参考里是语义（同表串行、跨表并行）；端口换成一把全局锁就等于把并行度写成 1，要在 `GAPS.md` 里说明这是性能差异还是语义差异。取锁顺序照抄参考，不让"表锁→引擎锁"与"引擎锁→表锁"在同一进程里共存。
- **池的尺寸与回退**：池大小（`max(2, hardware_concurrency()/2)`）、路由键、队列是否有界都按参考；端口在拿不到池时（引擎不在 `Arc` 里）必须能回退到内联执行，回退路径要有人走、有测试守。

**并发结构不改变字节，字节门禁不会替你抓错**，所以另用三种证据：

1. 计数器或线程身份断言"确实走了池"——例如 `an_insert_runs_its_bottom_half_on_the_bottom_executor` 里的 `BOTTOM_RUNS`；
2. 与参考比对并行结构的外部口径——`EXPLAIN FRAGMENT` 的 fragment 类型与 sink/source、`EXPLAIN` 里的 merge 算子（`test/sql/explain`，期望值取自参考二进制）；
3. 并发下的可观察顺序单独写用例（commit-ts 顺序发布、同表下半部按提交顺序）。

**实现选择：并发一律走 tokio，不再手写线程池。** 参考的 `TaskScheduler` 是常驻 worker 池，端口用 tokio 多线程 runtime 承担（`infinity-executor::scheduler`）：tasklet 是 `spawn_blocking` 作业（CPU 密集且阻塞在有界交换队列上），sink 任务提交到同一个池并 `block_on` 等待；行号分配器作为常驻阻塞消费者跑在同一个池上。这样"池多大、谁来调度、任务怎么排队"由框架决定，和参考的 `TaskScheduler` 一一对应，也避免每个 fragment 起线程。

只有两类例外保留命名线程，因为它们持有的是进程级阻塞资源、且所在 crate 没有 scheduler：`infinity-txn` 里的 WAL 写线程（独占 `wal.log` 句柄）。判断标准是"这个循环会不会阻塞在 I/O 或队列上、是否活到进程结束"——会，就用 tokio 的阻塞池或专用线程；能 await 的，用 async 任务。

字节门禁仍保持单客户端串行（§2.2），并发本身的覆盖归 Gate D。逐结构的状态、落点与证据见 [`PARALLEL_MAP.md`](PARALLEL_MAP.md)。

---

### 2.8 `unsafe` 的边界

除 `infinity-storage` 与 `infinity-cli`，每个 crate 都是 `#![forbid(unsafe_code)]`。剩下两处 `unsafe` 是刻意的：

- **`memmap2::Mmap::map`**（8 个封存索引段的读者，`infinity-storage`）：`memmap2` 的 API 是 `unsafe`，因为映射可能比 `File` 活得久。改成 `std::fs::read` 虽然能消掉它，但会把整段读进堆（RSS）、失去惰性分页与 page cache 回收——首轮实测这类改动会明显拖慢大表扫描，**不再考虑**。`infinity-storage` 因此保持 `#![deny(unsafe_code)]` + 每处 `#[allow(unsafe_code)]`，并逐处写 SAFETY 注释（段一旦封存不可变，映射期间文件不会被改写）。
- **`malloc_trim` / `posix_fadvise`**（`infinity-storage` 与 CLI 的 `trim_allocator`、`drop_file_cache`）：只能 FFI。它们服务于内存观测与长导入的 page cache 边界（Gate C），作用域极小、无指针、无生命周期，属于可接受的"测试/运维向"unsafe。

判断标准：**性能敏感的映射必须保留**；只有"无指针、无别名、仅调用一个系统函数"的窄 FFI 才允许，且必须写清楚为什么不能用安全 API 表达。

## 3 当前状态

### 3.1 存储形态：catalog 已是参考的键空间，行仍在 KV

**catalog 只写参考版的键**。`Catalog` 以 `with_store_cpp_only` 打开，`catalog/` 里只有
`catalog|…`、`db|…`、`tbl|…`、`col|…`、`idx|…`、`seg|…`、`blk|…`、`idx_seg|…`、
`idx_chunk|…`、`drop|…` 以及 `next_database_id` / `meta|version`。

门禁 `test_the_catalog_key_set_and_values_match_exactly`
（`test_cross_engine_bytes_10_encoding.py`）是这条的口径：同一 workload 下两侧 **key 集合完全
相同，且同 key 的 value 逐字节相同**——不是"我们认识的记录对得上"，而是整个键空间，任何一侧
多出的键都算差异。workload 覆盖：活表（3 行 + 删 1 行，含段层与块镜像）、被 drop 的表、被
drop 的数据库。

**行只在 `data/` 的块文件里**：提交把行交给块的 append 缓冲区（`TableEntry::pending_rows`，即参考的
`BufferObject`），checkpoint 才写文件；`persistence/` 里只剩 append/delete 历史与侧车文件，
不再有 `row:`/`del:` 键。唯一例外是块格式表达不了的列类型（多值/张量/稀疏）：那类表额外留一份文本行，
因为块解码与 WAL 块解码都还原不了这些列，否则快速重启（checkpoint 没跑过）会丢行；这类表的计数与读
仍以块为准。

**append/delete 历史从文件重建**：`restore_cpp_blocks` 用每个块的 `version`（`created` 的
`(ts, 累计行数)`、`deleted` 槽）重建 `appends:`/`append_ts:`/`append_copy:`/`deletes:`/
`delete_ts:`/`import_rows:`/`import_ts:`，因此这些日志不再需要落盘；`recover_cpp_blocks`
同时返回文件里最新的 ts，作为 WAL 重放的起点（等价于参考的 checkpoint 记录）。
门禁：`cpp_blocks::tests::rehydration_*` 三个测试（普通表 / 带删行 / 压缩段）断言"丢掉行侧
键、重开、再写一遍"后 `data/` 下每个文件逐字节不变。

**读路径已经是块**。扫描按 `visible_block_ids`（块索引 − `drop|seg|` 标出的段）逐块解码，
`version` 的 `created`/`deleted` 决定哪些槽可见；`row_count` 与 `cpp_visible_row_count` 用同一份
`version`，`count(*)` 与整表扫描不会分叉。压缩不再物化行表：`BlockCursor` 用行号直接定位那一个块
（`segment << 32 | block * capacity + slot`），一块一解码、O(1) 内存。`dirty:` 是**持久化**的
"块落后于日志"标记（append/delete/压缩/删列置位，checkpoint 且期间没有新写入才清除），所以崩溃
重放后读侧仍然知道该看块还是该合并日志。门禁：
`cpp_blocks::tests::the_blocks_are_the_row_set_not_just_the_values`（丢掉 `row:` 后扫描/计数仍完整，
含重开）、`a_compaction_reads_the_rows_out_of_the_blocks`（无日志也能压缩）、
`a_crash_after_an_append_keeps_the_row` / `a_crash_after_a_delete_keeps_the_row_hidden`。

读侧没有"只认日志"的路径了：整表/剪枝/投影/点取/含墓碑五种读法都先问块，日志只补 checkpoint 之上
的那一段；`row_count` 数的是块 `version` 里可见的槽（不信任增量计数器——UPDATE 重写行会重新分配 id，
计数器会漏）。行 id 分配（`next_row_seq`）与段布局（`block_segment_row_counts`）也能从块读回。

**写路径按提交期原语落到块上**。一次 append 提交只写本批次行落入的那几个块，并给每个块的 `version`
追加一条 `create`（`BlockVersion::Append(commit_ts, block_offset + rows)`，参考
`NewTxn::AppendInBlock`）；一次 delete 只重写持有被删槽的块，把 `deleted_[slot]` 标成该事务的提交 ts
（`DeleteInBlock`）。其余块一个字节都不动——它们已经是参考留下的那份，重编码只会让 NULL 位图按
"最终行集"重算（实测一次 100 行的 COPY 写了 `0x0f` 的窗口，整表重写后变 `0xff`）。段层
（`catalog|seg|`、`catalog|blk|`、`blk|…|fast_rough_filter`）随同这一次写入增量发布，未触及的块保留
原记录里的 ts 与预过滤。唯一的例外是**被这次写入封段**的段：参考的 `ExecuteOnNewSealedSegment` 要给
段里每个块建预过滤，这条路径会重写整段。门禁
`cpp_blocks::tests::a_commit_writes_only_the_block_it_appends_to`：两次 `INSERT` 后第一块的
`version` 逐字节不变、第二块多一条 `create`，随后一次 `DELETE` 只给第二块加墓碑。

UPDATE 重写、快照还原、压缩、回放都走同一个入口（把行交给 buffer 或整表重写），没有哪条路径再从
`row:` 取行。唯一要守住的是块的寻址方式：块按 **slot** 寻址（行 *n* 的 id 是
`segment << 32 | block * 8192 + n`），所以写入前要校验 `chunk` 的 id 是否从块首连续——带空洞的块宁可
这次不写（行留在 buffer 等下次 checkpoint），也不能紧凑写，否则空洞之后的行会整体挪位。

所以"一个目录里的键与参考完全一致"现在对 `catalog/`、`data/` 与 `wal/` 三者都成立；`persistence/`
里剩下的每一样（append/delete 历史、行侧车）都是参考版放在同一位置的东西。

**drop 是标记，物理回收在 cleanup**。`DROP DATABASE` / `DROP TABLE` / `DROP INDEX` 只写
`drop|db|{name}/{create_ts}/{db_id}`、`drop|tbl|{db}/{name}/{create_ts}/{table_id}`、
`drop|idx|{db}/{table}/{name}/{create_ts}/{index_id}`，值是该事务的 commit ts；被弃用的记录、
段层与文件**原地保留**，名字的可见性靠标记判断（先取同名记录里 ts 最新的那条，再看它的标记）。
回收是 `Engine::cleanup` 里的 `reclaim_dropped_entities`：删标记、删它弃用的记录与目录，与参考
版的 `NewTxn::PrepareCommitDrop{DB,Table}` → `NewTxn::Cleanup` → `NewCatalog::CleanDB` /
`CleanTable` 一一对应。因此：

- 标记的时间戳在 **value** 里（`KeyEncode::DropTableKey(…) -> to_string(commit_ts)`），
  `Catalog::restamp` 对 `drop|` 开头的键改的是 value，不是 key；
- 布局重写（`rewrite_cpp`）必须**跳过**被标记弃用的记录，否则 `DROP TABLE t; CREATE TABLE t`
  会把旧记录提前抹掉，得到一个参考版不会有的键集（`dropped_record_prefixes`）；
- 表 id 是**按库**计数的，`db_{id}/tbl_{id}` 与 `catalog|seg|{db}|{tbl}|` 都以两个 id 定位，
  所以段层必须按 `(db_id, table_id)` 配对，不能只看表 id（两个库可以有同 id 的表）；
- 索引 id 也是单调的（`TableMeta::GetNextIndexID` 的 `next_index_id` 标签）：`DROP INDEX` 留下
  记录，按"存活索引的最大 id + 1"发号会让新建的索引拿到被 drop 的那个 id，从而继承它的键与
  标记。


代价（SIFT 1M × 128 维 `COPY ... FORMAT fvecs`，`INFINITY_PROFILE_IMPORT=1`）：

| 阶段 | 耗时 | 说明 |
|---|---|---|
| 读 493 MB fvecs | 0.18 s | — |
| **f32 → 十进制文本** | **10.88 s** | 128M 次 `format!`，参考版不存在这一步（它写列式二进制） |
| 编码存储行 | 0.53 s | 又一次文本拼接/解析 |
| 提交（WAL + 1M KV 写） | 1.60 s | — |
| 关闭 flush | 4.49 s | 500 MB 文本落 RocksDB |

参考版同一份数据是 2.14 s。83% 的 import 时间花在"把浮点转成字符串"，这个开销来自行值的文本
编码（不是存储位置），是下一步优化的对象。


| 维度 | 状态 | 验证 |
|---|---|---|
| 磁盘格式（`data/`、`catalog/`、`wal/`） | 块列、`version`、索引 chunk、catalog 键值、WAL 帧（含 `txn_id` 与 CRC）与参考逐字节相同 | `python/restart_test/test_cross_engine_bytes_*.py`（按层分文件，见 `FORMAT_EQUIVALENCE_TESTS.md` §3） |
| WAL | `wal.log` 的文件名、checkpoint 换文件、帧头、帧体全部一致；两引擎可互相 replay | 同上 + `test_cross_engine_compat.py` |
| 跨引擎交接 | 参考读重写版的目录、重写版读参考的目录，数据与各索引的答案一致 | `test_cross_engine_compat.py` |
| SQL 行为 | `.slt` 全量一致（Gate A）；SDK/协议（Gate B）、重启/恢复（Gate C）由对应脚本驱动 | `rust_engine/scripts/run_gate_{a,b,c}.sh` |
| 内部不变量 | 配置校验、存储/事务、索引三组不变量逐条有回归测试（Gate G） | `INVARIANT_MATRIX.md` |
| 已知缺口 | 见 §4 | — |

---

## 4 当前缺口

唯一的缺口总表是 [`GAPS.md`](GAPS.md)，分四类：

| 类别 | 现在有什么 | 是否待办 |
|---|---|---|
| §1 有意不移植 | 对象存储 `obj_key` 打包、DiskANN、geo 类型 | 否，是决定 |
| §2 未实现 / 未接线 | **两项并发待办**：执行器的多 fragment / tasklet（D4）、事务下半部的执行者与按 commit_ts 推进可见性；其余接线项（相关子查询、压缩/IMPORT 的 WAL 帧、`IndexScanBuilder`、`ResultCacheGetter`、`MATCH` 走索引、`fast_rough_filter`、分析器家族、稀疏检索选项、快照）已接线并有门禁 | 是，待办 |
| §3 不可逐字节比对 | HNSW 图区、IVF 质心、`catalog/` 的 RocksDB 文件字节（键与值已逐键比对）、NULL slot 与内联 padding、后台 dump 的 chunk 切分 | 否，参考自身不确定 |
| §4 覆盖不到的输入空间 | 带索引的 8M 行表、三段以上的容量切分、PLAID/EMVB/MULTIVECTOR chunk 字节 | 是，但代价高 |

已登记的性能/口径差异（相关子查询的作用域边界、`CREATE TABLE … AS SELECT`、视图、`INSERT … SELECT`、`standard-porter` 的词干）在 `GAPS.md` §5，不是待办。

---

## 5 子目标与步骤

移植拆成下面这些子目标。每个子目标一份台账，记录它**此刻**的状态（不是历史）；本文只留一句摘要，细节在各自的文件里。

| # | 子目标 | 台账 | 记录的状态 | 什么时候读 |
|---|---|---|---|---|
| S1 | 冻结磁盘格式 | [`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md) | 数据文件、`version`、列体、outline、倒排三件套、次级/稀疏索引、WAL、catalog 键的字节级规格，逐条带 `文件:行` | 要动格式，或想知道某段字节为什么长这样 |
| S2 | 做到逐字节等价 | [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) | 三层口径的边界、门禁的分层与覆盖矩阵、格式层面的例外及原因、复现步骤 | 跑/改/新增格式门禁 |
| S3 | 类级覆盖与方法对应 | [`CLASS_MAP.md`](CLASS_MAP.md) | 参考每个 `export class/struct` 的状态与 Rust 落点；关键类的方法级对应；有意的设计变更；已/未移植的单测 | 查某个类有没有对应、某个差异是不是故意的 |
| S4 | 用例级覆盖不漏 | [`TEST_AUDIT.md`](TEST_AUDIT.md) | 参考每个 `TEST/TEST_F/TEST_P` 在 Rust 的落点，或排除理由 | 查某个 C++ 单测对应哪条 |
| S5 | 内部不变量有测试兜住 | [`INVARIANT_MATRIX.md`](INVARIANT_MATRIX.md) | C++ 不变量 → Rust 回归测试的对照；Gate G 的三组（配置、存储/事务、索引） | 想知道某条不变量由谁兜住 |
| S6 | catalog 底座与互操作 | [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) | 唯一后端（RocksDB）、与参考目录互读的条件、fsync 点 | 参考打不开目录、或想确认落盘点 |
| S7 | 缺口收口 | [`GAPS.md`](GAPS.md) | 唯一缺口总表：有意不移植 / 未实现 / 不可字节比 / 覆盖不到 | 想知道"还差什么" |

子目标之间的依赖：S1 是 S2 的输入；S2 的例外决定 S7 §3；S3/S4 是"覆盖"的口径，S5 是"不变量"的口径；S6 是跨引擎互操作的前提（两引擎共用同一个 catalog 键空间）。

---

## 6 怎么跑

格式门禁（需要参考二进制 `cmake-build-reldeb/src/infinity`）：

```bash
cd rust_engine && cargo build --release -p infinity-cli
cd ..
.venv/bin/python -m pytest python/restart_test/test_cross_engine_bytes_*.py \
                              python/restart_test/test_cross_engine_compat.py -q
```

这两组用例把两个引擎的根目录都放在 `/var/infinity/bytes`，并且会用参考再读一次目录；非 root 用户需要可写的 `/var/infinity`（`scripts/run_gate_c.sh` 用 `bwrap` 提供，且必须是磁盘目录，不能是 tmpfs）。

单元测试：

```bash
cd rust_engine && cargo test --release --workspace --lib
```

`scripts/run_gate_{a,b,c,d,e}.sh` 分别对应 SQL、SDK/协议、持久化、并发、性能关卡；用法与各自最近一次实测结论（表内标日期）见 `rust_engine/scripts/README.md`。
