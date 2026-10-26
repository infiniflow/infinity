# 并行结构对齐清单

> 口径见 [`PORTING.md`](PORTING.md) §2.7（在哪跑 / 谁等谁 / 锁的层次 / 池尺寸与回退）

状态取值：

| 状态 | 含义 |
|---|---|
| `aligned` | 线程归属、等待点、分片与尺寸与参考一致，行为可观察部分相同 |
| `different` | 结构有意不同，已在 [`GAPS.md`](GAPS.md) 或 [`CLASS_MAP.md`](CLASS_MAP.md)「有意的设计变更」登记，写清差异与影响 |
| `n/a` | 参考有该结构，端口不需要（或反之），不影响等价 |

行内「参考」列给 `文件:行号`，「端口」列给落点，「证据」列给能复现该结论的用例。字节门禁不覆盖并行结构（[`PORTING.md`](PORTING.md) §2.2/§2.7），证据因此分三类：池计数器单测、`EXPLAIN` 类外部口径、并发可见性用例。

## 1 线程、队列与池

| 结构 | 参考 | 端口 | 状态 | 证据 |
|---|---|---|---|---|
| 事务下半部池 | `BottomExecutor`：`max(2, hardware_concurrency()/2)` 条队列，`crc32(table_id) % n` 路由，worker 里跑 `CommitBottom()`（`bottom_executor_impl.cpp:32-78`、`config_impl.cpp:562-567`） | `infinity_storage::wal::bottom_executor::BottomExecutor`（同一个移植）+ `Engine::run_commit_bottom`；`start_background_tasks` 起池（`lib.rs`） | `aligned` | `an_insert_runs_its_bottom_half_on_the_bottom_executor`（`BOTTOM_WORKS` 计数 + 返回即可查询） |
| 索引追加队列 | `MemIndexAppender`：**单线程**，非实时全文走 `Submit`（`mem_index_appender_impl.cpp:58`、`new_txn_index_impl.cpp:891-894`） | `MemIndexAppender`（单 worker + channel）+ `Engine::run_text_append`；插入路径的二级/全文/向量缓存整体交给它 | `aligned` | `a_fulltext_insert_runs_on_the_mem_index_appender`（`TEXT_APPENDS` 计数 + `filter_fulltext` 立刻命中） |
| 后台任务处理器 | `BGTaskProcessor`：**每个实例一个线程**，`DequeueBulk` + `StopProcessorTask`（`background_process_impl.cpp:41-53`）；配额触发的 `DumpMemIndexTask` 走它 | `BGTaskProcessor`（`bg_task.rs:35`，`start_background_tasks` 里 `new(1)`），`DumpMemIndexTask`/`CleanupTask` 经它 | `aligned` | `bg_task_ut`（配额触发、`drain()` 语义） |
| 周期触发 | **一个** `PeriodicTriggerThread`，1s tick 里跑 checkpoint/compact/optimize/cleanup 四个触发器（`periodic_trigger_thread_impl.cpp:49-74`） | `trigger_loop` 只跑 cleanup/compact/optimize（`start_background_tasks`），checkpoint 与 flush 另由 CLI 的两个线程按 `checkpoint_interval` 驱动 | `different` | `compact_replay_ut`、`cleanup_ut`；差异登记在 `GAPS.md` §5 |
| WAL 写入与轮换 | 独立的 WAL 写线程消费 `new_wait_flush_`（`wal_manager_impl.cpp:89`、`NewFlush`），`SwapWalFile` 在 checkpoint 事务里换文件 | 没有独立写线程：`TxnManager::commit_at_deferred` 在 `commit_lock` 下直接 `wal.append`；轮换由 CLI 的 `checkpoint_wal` 线程调 `rotate_wal_if_large` | `different` | `test_cross_engine_bytes_40_wal.py`（帧与文件名逐字节相同）；线程差异登记在 `GAPS.md` §5 |
| 执行期调度 | `TaskScheduler`：`hardware_concurrency()` 个常驻 worker，各带 `FragmentTaskBlockQueue`（`task_scheduler_impl.cpp:61-81`），fragment 之间用队列流水 | 无常驻调度器：`run_tasklets` 每个 fragment 用 `std::thread::scope` 起 `min(cpu, TaskletCount)` 个 tasklet，merge 任务按 tasklet 序从有界通道取 | `different`（`GAPS.md` §5） | `run_operators_splits_a_row_wise_chain_and_limits_after_the_merge`、`the_merge_task_reads_the_tasklets_through_the_exchange_queue` |
| 事务/行号分配 | `TxnAllocator` 单线程（`txn_allocator_impl.cpp:37`） | 无独立线程：行号在事务内直接分配 | `different`（`GAPS.md` §5） | `wal_shape_ut`（事务号序列与参考相同） |

## 2 等待点与可见性

| 语义 | 参考 | 端口 | 状态 | 证据 |
|---|---|---|---|---|
| 上半部等自己的下半部 | `Commit` 在 `commit_cv_` 上等 worker 跑完 `CommitBottom`（`new_txn_impl.cpp:2138-2139`） | `commit_deferred` 取 ts、底部工作结束后 `finish_bottom`；`run_commit_bottom` 用 condvar 等 worker | `aligned` | `infinity-txn` 单测、`a_commit_runs_...` |
| 按 commit_ts 顺序发布 | `bottom_txns_` 按 ts 排序，队首逐个推进 `current_ts_`（`new_txn_manager_impl.cpp:410-457`） | `register_bottom`/`finish_bottom` 屏障，时钟从队首推进，冲突也走同一屏障 | `aligned` | `infinity-txn` 的发布顺序用例 |
| 行与索引一起可见 | 下半部（行 → 索引）跑完才 `SetTxnCommitted`，`PostCommit` 再等全文追加的 sema（`new_txn_impl.cpp:2138-2141`、`4624-4660`） | `EngineTxn` 路径已延后发布（`commit_deferred` → 索引 → `finish_bottom`）；**语句路径**在 `TxnManager::commit` 时就推进 `current_ts`，行/索引/块镜像在其后补 | `different` | `a_commit_runs_its_bottom_half_on_the_bottom_executor` 只覆盖 `EngineTxn`；语句路径的窗口登记在 `GAPS.md` §2 |
| dump 任务完成 | `StopProcessorTask::Wait()`（`background_process_impl.cpp:46-53`） | `BGTaskProcessor::drain()` | `aligned` | `bg_task_ut` |
| 只读事务不占写 ts | `GetReadCommitTS`（`new_txn_impl.cpp:2051-2059`） | `TxnManager::commit` 的 read-only 分支 | `aligned` | `txn_ut` |

## 3 锁

| 锁 | 参考 | 端口 | 状态 | 证据 |
|---|---|---|---|---|
| 同表下半部顺序 | 由 `BottomExecutor` 分片保证（同表落同一 worker） | `bottom_gates`：`Engine::bottom_gate(table)`，worker 内取该表 gate | `aligned` | `b624fd4a2` 的提交说明与单测 |
| 块级写锁 | 每块一把 `shared_mutex`（`BlockLock`，`new_txn_data_impl.cpp:930-958`），两表可同时写 | 无块级锁：写批次在 `batch_lock` 下落块镜像，checkpoint 在 `flush_lock` 下整表重写；表元数据共用 `tables` 一把互斥 | `different`（`GAPS.md` §5，性能差异，字节不受影响） | `test_cross_engine_bytes_00_premises.py`（同一目录两引擎先后可读） |
| 表元数据 | 参考 `new_catalog` 的元数据缓存按条目加锁 | `tables: Arc<Mutex<HashMap<..>>>` 一把全局锁 | `different`（`GAPS.md` §5，性能差异） | 同上 |
| DDL 串行 | catalog 的 DDL 由目录锁串行 | `ddl_lock` | `aligned` | `test/sql/ddl` |
| WAL / checkpoint | 参考写 WAL 与 checkpoint 互斥 | `flush_lock` | `aligned` | `test_cross_engine_bytes_40_wal.py` |
| 批量写 vs 后台 dump | `DumpMemIndex` 事务与 append 争用段 | `batch_lock` | `aligned` | `dump_mem_index_ut` |

## 4 分片、尺寸与回退

| 项 | 参考 | 端口 | 状态 |
|---|---|---|---|
| 池大小 | `max(2, hardware_concurrency()/2)` | 同（`start_background_tasks`） | `aligned` |
| 下半部路由键 | `crc32(table_id) % n` | 同（`BottomExecutor::submit`） | `aligned` |
| 队列是否有界 | `BlockingQueue` 有上限：`DEFAULT_BLOCKING_QUEUE_SIZE = 1024`，`Enqueue` 在满时阻塞（`blocking_queue.cppm:29-99`、`default_values.cppm`） | 下半部池（`VecDeque`）、后台任务处理器（`VecDeque`）、追加队列（`mpsc::channel`）都是无界 | `different`（`GAPS.md` §5） |
| tasklet 宽度 | `min(cpu_count, first_operator->TaskletCount())`（`fragment_context_impl.cpp:1218`） | 同（`tasklet_count`/`block_tasklets`，`operators/mod.rs`） | `aligned` |
| `TaskletCount` 规则 | 默认 1；filter/project/limit/top/sort 透传；扫描 = 块数（`physical_operator_impl.cpp:47` 起、`physical_scan_base_impl.cpp:86`） | 同（链上解析） | `aligned` |
| 交换队列容量 | `LocalQueue` 的阻塞队列容量由实现决定 | 每 tasklet 一条有界通道，容量 2 个 chunk | `different`（`GAPS.md` §5） |
| 无池回退 | 参考没有（池总是存在） | 引擎不在 `Arc` 里或池未挂载时内联执行（`run_commit_bottom`、`run_text_append` 的 `else` 分支） | `n/a` |

## 5 怎么验证

1. **池计数器**：`BOTTOM_WORKS`（`an_insert_runs_its_bottom_half_on_the_bottom_executor`）、`TEXT_APPENDS`（`a_fulltext_insert_runs_on_the_mem_index_appender`）、`bg_task_ut` 的 `drain()` 断言——证明工作真的换了线程，而不是文档里写了一句。
2. **外部口径**：`test/sql/explain/explain_fragment.slt` 钉住 `FRAGMENT (1, <type>)` 与 sink/source，类型由计划里有无阻塞算子决定（`ParallelStream` / `ParallelMaterialize` / `SerialMaterialize`）；`EXPLAIN` 的 merge 算子（`TOP`/`AGGREGATE`）与参考一致。
3. **可见性单测**：commit-ts 顺序发布、同表下半部顺序、索引写完才返回——都在 `infinity-txn` 与执行器单测里。
4. **并发门禁**：Gate D（`run_gate_d.sh`）覆盖并发本身；字节门禁保持单客户端串行（`PORTING.md` §2.2）。
