# 并行结构对齐清单

> 口径见 [`PORTING.md`](PORTING.md) §2.7（在哪跑 / 谁等谁 / 锁的层次 / 池尺寸与回退）

每个结构先写**它做什么**（工作的步骤与落到哪），再给参考与端口的落点、状态与证据——只看"有没有对应类"看不出并行结构是否走样。

状态取值：

| 状态 | 含义 |
|---|---|
| `aligned` | 线程归属、等待点、分片与尺寸与参考一致，可观察行为相同 |
| `different` | 结构有意不同，已在 [`GAPS.md`](GAPS.md) 或 [`CLASS_MAP.md`](CLASS_MAP.md)「有意的设计变更」登记，写清差异与影响 |
| `partial` | 机制已有，但只覆盖一部分路径——行内写清还缺哪一段 |
| `n/a` | 参考有该结构、端口不需要（或反之），不影响等价 |

## 1 线程、队列与池

### 1.1 事务下半部（`BottomExecutor` + `CommitBottom`）

**做什么**——一次提交的落地工作，按事务的 WAL 命令逐条执行（`NewTxn::CommitBottom`，`new_txn_impl.cpp:4570-4609`）：

1. `APPEND_V2` → `CommitBottomAppend`（`new_txn_data_impl.cpp:1788-1909`）：
   - 把 `row_ranges_` 按块容量切成块对齐区间；
   - 每个区间按需建段/块元数据（`AddNewSegmentWithID` / `AddNewBlockWithID`）；
   - `AppendInBlock`：逐列 `AppendWith` 写列文件与 NULL 位图，再 `version.Append(commit_ts, 行尾)`；
   - 每个索引 `AppendIndex`：读回该区间 → `AppendMemIndex`（次级/全文/多向量；非实时全文转交给 `MemIndexAppender`）；
   - 段写满时 `DelUnsealedSegmentID`、建 `fast_rough_filter`、给每个索引提交 `DumpMemIndexTask`。
2. `DELETE_V2` → `CommitBottomDelete`：只提交段级 `first_delete_ts`（墓碑与 `version` 的删除槽在上半部 `PrepareCommitDelete`/`DeleteInBlock` 已写）。
3. 其它命令没有底部工作。
4. `keys_wait_for_commit_` 的 kv 键写上 commit ts，再 `txn_mgr_->CommitBottom(this)`：推进 `current_ts_` 并唤醒在 `commit_cv_` 上等的上半部。

| | |
|---|---|
| 参考 | 池：`max(2, hardware_concurrency()/2)` 条队列、`crc32(table_id) % n` 路由（`bottom_executor_impl.cpp:32-78`、`config_impl.cpp:562-567`）；每事务由**一个** worker 跑完上面全部步骤 |
| 端口 | `infinity_storage::wal::bottom_executor::BottomExecutor` + `Engine::run_commit_bottom`（`BottomWork`：own `Arc<Engine>`、staged appends/deletes/dumps、每表 gate，跑 `run_bottom` 后 condvar 回传）；`start_background_tasks` 起池 |
| 状态 | `aligned` |
| 证据 | `an_insert_runs_its_bottom_half_on_the_bottom_executor`（`BOTTOM_WORKS` 计数 + 返回即可查）、字节门禁 `20_table`/`50_mutations` |

### 1.2 索引追加（`MemIndexAppender`）

**做什么**——把一次提交里的倒排写入搬离提交线程（`mem_index_appender_impl.cpp:70-135`，`Process()` 在 :70）：

1. `DequeueBulk` 取出一批 `AppendMemIndexTask`；
2. 按 `MemoryIndexer` 分组，同一个全文索引的多个块合成一个 `AppendMemIndexBatch`；
3. `AsyncInsertBottom`：把列向量倒排进该段的内存索引器（分词、位置、行号）；
4. 批末 `WaitForCompletion` + `CommitSync`；
5. 逐个 task `Complete()`，释放上半部在 `PostCommit` 里等的 sema。

| | |
|---|---|
| 参考 | **单线程**队列（`mem_index_appender_impl.cpp:58`），提交点 `new_txn_index_impl.cpp:891-894`，等待点 `new_txn_impl.cpp:4624-4660` |
| 端口 | `MemIndexAppender`（单 worker + channel）+ `Engine::run_text_append`：插入路径的二级/全文/向量缓存整体投递，等 job 完成（失败上抛） |
| 状态 | `aligned` |
| 证据 | `a_fulltext_insert_runs_on_the_mem_index_appender`（`TEXT_APPENDS` 计数 + 立刻可检索）、`a_tasklet_error_fails_the_statement` |

### 1.3 后台任务处理器（`BGTaskProcessor` / `DumpIndexProcessor`）

**做什么**——执行"不挂在某个语句上"的任务（`background_process_impl.cpp:41-53`、`dump_index_process_impl.cpp:143-160`）：

1. 单线程 `DequeueBulk`；
2. 按 `BGTaskType` 分发：`kDumpMemIndex` → `DoDump` 打开一个 `kDumpMemIndex` 事务、`DumpMemIndex(...)`、提交（冲突则回滚重试）；`kCleanup` → 删除弃用段文件；其余同类；
3. `StopProcessorTask` 是同步点：已入队的任务全部跑完才退出。

| | |
|---|---|
| 参考 | `BGTaskProcessor` 每实例一个线程；dump 有自己的 `DumpIndexProcessor`（也是单线程），配额触发的 `DumpMemIndexTask` 走它们 |
| 端口 | `BGTaskProcessor`（`bg_task.rs:35`，`start_background_tasks` 里 `new(1)`）+ `DumpMemIndexTask`/`CleanupTask`（`submit_task`）；`drain()` 对应停止同步点 |
| 状态 | `aligned` |
| 证据 | `bg_task_ut`（配额触发、`drain()` 语义）、`dump_mem_index_ut` |

### 1.4 周期触发

**做什么**——按 `[storage]` 的间隔周期性发起后台工作（`periodic_trigger_thread_impl.cpp:49-74`）：每 1s 醒一次，逐个检查 checkpoint / compact / optimize / cleanup 触发器是否到期，到期就 `Trigger()`（分别发起 checkpoint 事务、`CompactTask`、`OptimizeIndexTask`、`CleanupTask`）。

| | |
|---|---|
| 参考 | **一个** `PeriodicTriggerThread` 管四个触发器 |
| 端口 | 同：`trigger_loop` 一个线程跑四个触发器（checkpoint 到点做 `checkpoint_wal` + `flush`；`start_background_tasks(checkpoint, cleanup, compact, optimize)`）。CLI 另有一个只做 `trim_allocator` 的线程（`malloc_trim` 只能在 CLI 调，不参与调度） |
| 状态 | `aligned` |
| 证据 | `bg_task_ut`、`compact_replay_ut`、`cleanup_ut` |

### 1.5 WAL 写入与轮换

**做什么**——把已取到 commit ts 的事务按提交顺序写进 `wal.log`：写线程消费 `new_wait_flush_`，把每个事务的 `wal_entry_` 追加成帧（帧头含 size/txn_id/CRC）；checkpoint 事务再 `SwapWalFile`，把当前文件改名成 `wal.log.<seal_ts>` 并建新文件。

| | |
|---|---|
| 参考 | 独立写线程 `new_flush_thread_`（`wal_manager_impl.cpp:89`、`NewFlush`），条目从 `new_wait_flush_` 取 |
| 端口 | `WalWriter`：名为 `wal-writer` 的线程独占 `WalManager`，`append`/`flush`/`file_size`/`rotate` 各是一个作业，调用方入队后等回执；`swap_wal_file` 与 `rotate_wal_if_large` 把"看大小 + 轮换"合成一个作业 |
| 状态 | `aligned` |
| 证据 | `test_cross_engine_bytes_40_wal.py`（整份 `wal.log` 逐帧相同，3 passed）、`50_mutations`（8 passed）、`cargo test -p infinity-txn`（18） |

### 1.6 执行期 tasklet

**做什么**——把 fragment 的行区间切成 `min(cpu, TaskletCount())` 份，每份一个 tasklet 跑自己的算子链拷贝，输出经有界交换队列按 tasklet 序汇给 sink（merge 任务）；阻塞算子（LIMIT/SORT/AGGREGATE）在 sink 侧串行。

| | |
|---|---|
| 参考 | 常驻 `TaskScheduler`：`hardware_concurrency()`（受 `CPULimit` 限制）个 worker 线程，每个一条任务队列，`ScheduleTask` 交给负载最小的 worker（`task_scheduler_impl.cpp:60-140`）；tasklet 与 sink 都是 `FragmentTask` |
| 端口 | tokio 多线程 runtime（`scheduler.rs`，`worker_threads = available_parallelism()`），进程级只建一次；tasklet 是 `spawn_blocking` 作业（CPU 密集且会阻塞在有界交换队列上，tokio 的阻塞池会按需扩展到 worker 数以上，sink 不会排在阻塞的 tasklet 后面），sink 同样提交到池上并 `block_on` 等它，语句线程的等待方式与参考一致 |
| 状态 | `aligned` |
| 证据 | `jobs_run_on_the_pool`、`the_pool_is_resident`、`concurrent_jobs_all_finish`；`test/sql` 1555/1560 查询（结果顺序与串行一致）、`20_table`（5）、`50_mutations`（8）、`40_wal`（3） |

### 1.7 行号 / 段号分配

**做什么**——每个 append 在写行之前先取一段连续的 ROW_ID（`segment_id << 32 | offset`，段满 8,388,608 行换段；`COPY` 封段、后续 `INSERT` 开新段）。参考把它放在 `TxnAllocator` 线程上：`Submit` 入队，`Process` 逐条取出，按事务类型调 `AllocateRowID`，再把结果回填给提交线程——单线程顺序发号，所以并发 append 拿到的区间不重叠、也不留洞。

| | |
|---|---|
| 参考 | `TxnAllocator::Process` 单线程消费 `task_queue_`（`txn_allocator_impl.cpp:56-110`，线程在 :37 起） |
| 端口 | `Allocator`：名为 `txn-allocator` 的线程消费作业，`reserve_rows_from` 入队 `reserve_rows_inline` 并等回执；引擎不在 `Arc` 里时（多数单测）退回内联执行 |
| 状态 | `aligned` |
| 证据 | `row_ids_are_allocated_on_the_allocator_thread`（断言 `allocator_jobs` 增长，且 4 个并发 INSERT 后行数与 id 数正确）、`test_cross_engine_bytes_20_table.py`（行 id 与块布局逐字节）、`40_wal`/`50_mutations` |

## 2 等待点与可见性

| 等待点 | 参考做的事 | 参考 | 端口 | 状态 | 证据 |
|---|---|---|---|---|---|
| 上半部等自己的下半部 | 提交线程把 txn 交给池后睡在 `commit_cv_`，被 `NotifyTopHalf` 唤醒才 `PostCommit`/`SetTxnCommitted` | `new_txn_impl.cpp:2138-2141` | `commit_deferred` → 底部工作 → `finish_bottom`；`run_commit_bottom` 用 condvar 等 worker | `aligned` | `infinity-txn` 单测、`a_commit_runs_its_bottom_half_on_the_bottom_executor` |
| 按 commit_ts 顺序发布 | `bottom_txns_` 按 ts 排序，队首完成即推进 `current_ts_`；先做完的后序 txn 不越位 | `new_txn_manager_impl.cpp:410-457` | `register_bottom`/`finish_bottom` 屏障（冲突也走同一屏障，ts 空洞保留） | `aligned` | `infinity-txn` 的发布顺序用例 |
| 行与索引一起可见 | 行落在块文件里、由下半部的 `AppendInBlock` 写，`SetTxnCommitted` 之后才可读；删除的墓碑在 `DeleteInBlock`，也在下半部 | `new_txn_impl.cpp:2138-2141`、`4624-4660` | 同：`INSERT` 的 `row:` 与 `DELETE` 的 `del:` 都写在下半部（行 → 墓碑 → 索引 → 块镜像），发布由 `BottomGuard` 在最后触发 | `aligned` | 679 单测、`20_table`（5）与 `50_mutations`（8）字节门禁 |
| dump 任务完成 | `StopProcessorTask::Wait()` 等已入队任务跑完 | `background_process_impl.cpp:46-53` | `BGTaskProcessor::drain()` | `aligned` | `bg_task_ut` |
| 只读事务 | `GetReadCommitTS`：不写 WAL、不占写 ts | `new_txn_impl.cpp:2051-2059` | `TxnManager::commit` 的 read-only 分支 | `aligned` | `txn_ut` |

## 3 锁

| 锁 | 参考保的是什么 | 参考 | 端口 | 状态 | 证据 |
|---|---|---|---|---|---|
| 块级写锁 | 每块一把 `shared_mutex`，列文件写入 + `version.Append` 在同一临界区，不同表可同时写 | `new_txn_data_impl.cpp:930-958` | 无块级锁：写批次在 `batch_lock` 下落块镜像，checkpoint 在 `flush_lock` 下整表重写；表元数据共用 `tables` 一把互斥 | `different`（`GAPS.md` §5，性能差异） | `test_cross_engine_bytes_00_premises.py` |
| 表元数据 | 元数据缓存按条目加锁，读多写少 | `new_catalog` 的 meta 缓存锁 | `tables: Arc<Mutex<HashMap<..>>>` 一把全局锁 | `different`（`GAPS.md` §5，性能差异） | 同上 |
| 同表下半部顺序 | 同表事务落同一个 worker，天然串行；跨表并行 | `BottomExecutor::Submit` | `bottom_gates`：`Engine::bottom_gate(table)`，worker 内取该表 gate | `aligned` | 提交 `b624fd4a2` 的单测 |
| DDL 串行 | DDL 事务串行，避免两次布局重写交错 | catalog 的 DDL 锁 | `ddl_lock` | `aligned` | `test/sql/ddl` |
| WAL / checkpoint | 写 WAL 与整表 checkpoint 互斥 | `WalManager` 的 flush/checkpoint 串行 | `flush_lock` | `aligned` | `test_cross_engine_bytes_40_wal.py` |
| 批量写 vs 后台 dump | dump 事务与 append 争用段，靠重试而不是全局锁 | `DumpIndexProcessor::DoDump` 的重试 | `batch_lock`（写批次与 `DumpMemIndexTask` 互斥） | `aligned` | `dump_mem_index_ut` |

## 4 分片、尺寸与回退

| 项 | 参考的取值 | 端口 | 状态 |
|---|---|---|---|
| 池大小 | `max(2, hardware_concurrency()/2)`，上限 cpu | 同 | `aligned` |
| 下半部路由键 | `crc32(table_id) % n` | 同（`BottomExecutor::submit`） | `aligned` |
| 队列是否有界 | `BlockingQueue` 上限 `DEFAULT_BLOCKING_QUEUE_SIZE = 1024`，满则 `Enqueue` 阻塞（`blocking_queue.cppm:29-99`） | 同：下半部池的 `BlockingQueue`、`BGTaskProcessor` 的队列、追加队列（`sync_channel`）都是 1024 上限，满了生产者在 condvar 上等 | `aligned` | `bg_task_ut`、`test_cross_engine_bytes_20_table.py`（单客户端不会触发反压，行为不变） |
| tasklet 宽度 | `min(cpu_count, TaskletCount())`（`fragment_context_impl.cpp:1218`） | 同（`tasklet_count`/`block_tasklets`，`operators/mod.rs`） | `aligned` |
| `TaskletCount` 规则 | 默认 1；filter/project/limit/top/sort 透传；扫描 = 块数（`physical_operator_impl.cpp:47` 起、`physical_scan_base_impl.cpp:86`） | 同（链上解析） | `aligned` |
| 交换队列容量 | `LocalQueue` 容量由实现决定 | 每 tasklet 一条有界通道，容量 2 个 chunk | `different`（`GAPS.md` §5） |
| 无池回退 | 参考没有（池总是存在） | 引擎不在 `Arc` 里或池未挂载时内联执行（`run_commit_bottom`、`run_text_append` 的 `else` 分支） | `n/a` |

## 5 怎么验证

1. **池计数器**：`BOTTOM_WORKS`、`TEXT_APPENDS`、`bg_task_ut` 的 `drain()` 断言——证明工作真的换了线程，而不是文档里写了一句。
2. **外部口径**：`test/sql/explain/explain_fragment.slt` 钉住 `FRAGMENT (1, <type>)` 与 sink/source，类型由计划里有无阻塞算子决定（`ParallelStream` / `ParallelMaterialize` / `SerialMaterialize`）；`EXPLAIN` 的 merge 算子（`TOP`/`AGGREGATE`）与参考一致。
3. **可见性单测**：commit-ts 顺序发布、同表下半部顺序、索引写完才返回——都在 `infinity-txn` 与执行器单测里；语句路径的窗口是 `GAPS.md` §2 的待办。
4. **并发门禁**：Gate D（`run_gate_d.sh`）覆盖并发本身；字节门禁保持单客户端串行（`PORTING.md` §2.2）。
