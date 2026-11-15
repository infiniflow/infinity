# 把常驻线程收进 tokio：方案

> 目标：除了「独占一个 OS 资源、且必须串行」的那几个之外，不再有自建常驻线程；并行与等待一律建在 tokio 上。
>
> 原则（沿用 `PARALLEL_ASYNC.md` §6.6）：
> 1. **转成 task 不会让阻塞消失。** 能被 async 化的是「循环里的等待」；真正阻塞的单元（`fsync`、RocksDB 批写、`mmap` 缺页、HNSW 建图）仍然必须落在 `spawn_blocking` 上。
> 2. **`spawn_blocking` 只包叶子。** 它接受的闭包里不能有 `.await`、不能有 `block_on`、不能是「一个事务 / 一次 dump / 一整张表」。
> 3. **不按函数名推断粒度。** 每条断言都要顺着调用链读到底，看到真实的 `read` / `write` / `fsync` / `.await` / `lock()`。写不出调用链就是没核实。

## 0 改造**前**的线程与 runtime（历史基线，用于对照）

| # | 名字 | 位置 | 形态 | 线程数 |
|---|---|---|---|---|
| 1 | `task-scheduler` | `executor/src/scheduler.rs:46` | `new_multi_thread()`，显式 `worker_threads(available_parallelism)` | 核数 |
| 2 | `fan-out` | `index/src/parallel.rs:68` | 兜底 runtime，惰性创建，`worker_threads(available_parallelism)` | 核数（仅在被用到时） |
| 3 | `infinity-thrift` | `server/src/lib.rs:318` | `new_multi_thread()`，**未显式设 worker_threads** → tokio 默认 = 核数 | 核数 |
| 4 | `infinity-http` | `server/src/http.rs:72` | 同上 | 核数 |
| 5 | `infinity-pg` | `server/src/pg.rs:576` | 同上 | 核数 |
| 6 | `infinity-trigger` | `executor/src/engine/statements.rs:8087` | `std::thread::Builder` | 1 |
| 7 | `wal-writer` | `txn/src/lib.rs:213` | `std::thread::Builder` | 1 |
| 8 | `infinity-bg-0` | `executor/src/bg_task.rs:82` | `std::thread::Builder` | 1 |
| 9 | （无名字） | `executor/src/engine/statements.rs:857` | `scheduler().spawn_worker` → 阻塞池上的常驻消费者 | 占 blocking 池 1 |
| 10 | （无名字） | `cli/src/main.rs:245` | `std::thread::spawn` | 1 |

`BottomExecutor`（`storage/src/wal/bottom_executor.rs`）是第 11 个候选，但生产路径不走它：`Executor::start` 只在它自己的单测里被调用，引擎用的是 `bottom_queues` + `scheduler().spawn_blocking`。

**改造前的常驻线程总量**：`4 × 核数`（scheduler + 三套 server）`+ 4`（trigger / wal / bg / trim）`+ 1 个 blocking 额度`。三套 server runtime 是最大的一块。

**改造后**（§1.1a / 1.2 / 1.3 / 1.5 / 1.6 实施完成）：

| 项 | 改造后 |
|---|---|
| 表 3/4/5 的三套 server runtime | 全部并入表 1 的 `task-scheduler`，各 front end 是一个 task |
| 表 6 `infinity-trigger` | 运行时 task（`scheduler().spawn_task`），tick 用 `tokio::time::sleep` |
| 表 8 `infinity-bg-0` | 运行时 task，`BgTask::run` 为 async |
| 表 9 行号分配器 | **保留 blocking 线程**（§1.2：改成 task 会与 `before_wal` 的同步桥接死锁） |
| 表 10 cli `malloc_trim` | 运行时 task + `spawn_blocking(trim_allocator)` |
| 表 7 `wal-writer` | **保留**（§1.4：净收益 ≤ 0） |

剩余常驻：`1 × 核数`（`task-scheduler`）`+ 1`（`wal-writer`），另加 `fan-out` 兜底 runtime（仅在没有 runtime 的调用方创建）。行号分配器仍占一个 blocking 额度（§1.2）。

## 1 逐项方案

### 1.0 先说一个必须承认的前提：`checkpoint` 链本身是混合负载

这是上一版方案最大的漏洞，写在最前面，因为它决定了 1.1 的改法。

上一版写「1.1 的改动面只有 `tasks.rs` 一个文件加几个 `pub async fn`，测试和 CLI 全部不受影响」。**这是错的。** 真实调用链（全部核实过）：

```
trigger_loop                              std::thread
└─ block_on(engine.checkpoint())          ← 现在的写法
   checkpoint()                            async
   └─ flush_storage()                      async      （flush_lock.write().await 在这里）
      └─ flush_blocks(ts)                  fn   ← 同步
         └─ flush_tables(...)              fn   ← 同步
            └─ for_each_blocking(...)      fn   ← 同步，内部自己 block_on（见 parallel.rs）
               └─ flush_table(...)         fn   ← 同步，混合负载：
                  ├─ block_on(gate.lock())              ← 等待点
                  ├─ write_blocks_bounded → 写块文件 / version / fast_rough_filter
                  ├─ write_hnsw_index → 写 chunk 文件
                  ├─ write_fulltext_index → 写 segment 文件
                  ├─ write_plaid_index / write_ivf_index → 写文件
                  └─ tables.lock()
```

`flush_table` 里既有 `block_on(gate.lock())`（等待），又有 5 类写文件（阻塞），还有一把 `std::sync::Mutex`。这正是「混合负载」，也正是 `flush_blocks` 现在**不能**直接 `.await` 的原因：它没有 async 内体。

所以「把 trigger 改成 async」这件事，实际上分成两个工作量级完全不同的部分：

| 子项 | 内容 | 风险 |
|---|---|---|
| **1.1a** | `sleep` → `tokio::time::sleep`；`checkpoint_wal()` → `.await`；`submit_*` 保持同步 | 低 |
| **1.1b** | `flush_blocks` 整链 async 化：`flush_table` 拆成「等 gate（`.await`）+ 写文件（叶子）」 | **高，等同 1.3** |

1.1a 的收益只有「少 1 个常驻线程」，**不包括**「不再自己等自己」——因为 `checkpoint()` 内部仍然是 `flush_blocks` 那串同步 `block_on`。那个收益要等 1.1b。

下面的 1.1 只描述 1.1a；1.1b 单独列为 1.1b。

### 1.1a `infinity-trigger` 的循环

现在（`executor/src/engine/tasks.rs:40-88`）：

```rust
loop {
    if checkpoint due { engine.checkpoint_wal();                  // → block_on(checkpoint_wal_async())
                        block_on(engine.checkpoint()); }
    if optimize due  { engine.submit_optimize_indexes(); }        // 纯入队
    if compact due   { engine.notify_compact(); }                 // ← 内部逐个 block_on(compact_segments)
    if cleanup due   { engine.submit_cleanup(); }                 // 纯入队
    std::thread::sleep(TICK);
}
```

改造：

- `std::thread::sleep(TICK)` → `tokio::time::sleep(TICK).await`
- `engine.checkpoint_wal()` → `engine.checkpoint_wal_async().await`（已存在，`admin.rs:670`）
- `engine.checkpoint()` → `.await`（已经是 `async fn`，`statements.rs:7995`）
- `engine.notify_compact()` → **这个不能简单 `.await`**：它内部是

  ```
  notify_compact()                     fn
  └─ for meta in tables { compact_table(&target) }
     └─ block_on(compact_segments(...))
  ```

  要么保留 `block_on`（那就还是同步等待，只是换个线程等），要么把 `notify_compact` 上浮 async。**建议 1.1a 里保留**，理由是 compact 由独立的 `submit_*` 路径也能触发，把它一起改会把改动面推到 compaction 链。但要在注释里写明「这里仍有一处同步等待」。
- 整段 `spawn` 到 `scheduler()` 的 runtime 上

`checkpoint_wal` / `optimize_indexes` / `compact_table` / `notify_compact` 的同步调用点实测：

| 函数 | 调用点 |
|---|---|
| `checkpoint_wal()` | 21 |
| `optimize_indexes()` | 36 |
| `compact_table(...)` | 10 |
| `notify_compact()` | 3 |

绝大多数在 `*_ut.rs` / `replay_*_ut.rs`。所以**保留同名同步包装**（内部 `block_on`），新增 async 版本：

```rust
pub async fn checkpoint_wal_async(&self) { ... }                       // 已有，改成 pub
pub fn checkpoint_wal(&self) { block_on(self.checkpoint_wal_async()) } // 保留
```

注意其中 2 处 `optimize_indexes()` 和 2 处 `compact_table(..)` 在 **server 的 async 上下文里**（`http.rs:1999/2009` 在 `pub async fn route` 内，`lib.rs:1265/1407` 在 `async fn serve` 内）。它们现在是在 runtime worker 上做同步 `block_on`——这本身就是待修的点，但归 1.1b 一起处理，不要在 1.1a 里动。

**收益**：少 1 个常驻线程。**风险**：低。

### 1.1b `flush_blocks` 链 async 化（真正的「不再嵌套等待」）

这条是上一版完全漏掉的。要做的是：

1. `flush_table` 拆成两段：
   - 外壳 async：`gate.lock().await`、`HeldBottom::enter`、写完后清 `hnsw_mem` / `ivf_mem`（都是内存操作）
   - 叶子：`write_blocks_bounded`、`write_hnsw_index`、`write_fulltext_index`、`write_plaid_index`、`write_ivf_index` —— 每个是「一张表的一个索引写文件」，符合叶子标准，进 `spawn_blocking`
2. `flush_tables` / `flush_blocks` 上浮 async，`for_each_blocking` 换成 async 版本（`index::parallel` 需要一个 async 入口，现在只有 `for_each_blocking`）
3. `flush_storage` 已经是 async，只是 `flush_blocks(checkpoint_ts)` 那一行改成 `.await`
4. `write_blocks_bounded` 内部那处 `block_on(bottom_gate.lock())` **不能靠 `HeldBottom::holds` 跳过**：叶子跑在 blocking 池线程上，那里的 thread-local 是空的，判断会返回 `false` 并第二次取锁 → 自锁（见 §5.8）。改为显式参数（实施后的形状）：

   ```rust
   pub fn write_blocks_bounded(&self, table: &str, ts: u64) -> Result<usize> {
       self.write_blocks_mode(table, &[], false, ts, true)   // 自己取锁
   }
   pub(crate) fn write_blocks_bounded_locked(&self, table: &str, ts: u64) -> Result<usize> {
       self.write_blocks_mode(table, &[], false, ts, false)  // 调用方已持锁
   }
   ```

   `flush_table` 的叶子用 `_locked`；compaction 的 bottom 作业也用 `_locked`（它在之前已经 `block_on(gate.lock())`）。

注意 `flush_table` 里的 `tables.lock()` 是 `std::sync::Mutex`，如果外壳变成 async，**不能跨 `.await` 持有**——必须写完文件后再取锁。

**没改的一处**：`flush_storage` 里的 `pm.persist_directory()`（整棵 `data/` 打包上传）仍然是同步调用。原因是 `PersistenceManager` 不实现 `Clone`，而 `spawn_blocking` 需要 `'static`；改成 `Arc<PersistenceManager>` 会波及它的所有方法。它每次 checkpoint 只调一次（不是每表），是「一次整目录打包」的单次调用，不构成循环里的热点。**留作已知项**，将来若要做，需先把该结构改成 `Arc` 字段。

**收益**：checkpoint 期间 runtime worker 不再被 `block_on` 占住；`for_each_blocking` 里那个「同步 fan-out 套 async 链」的扭曲消失。**风险**：高——checkpoint 是最关键的正确性路径（Gate A、字节门禁、`94 s` 的全量写都要过）。

### 1.2 行号分配器 — **结论：不改，保留 blocking 线程**

> 这一节最初写成「改成 runtime task」。**实施后它挂死了 19 个测试**（`import_then_*`、
`conflict_*_column_compact` 等），回退后全部恢复。原因见下，这是本次改造最重要的一条教训。

现在：`scheduler().spawn_worker(Box::new(move || while let Some(job) = queue.blocking_recv() { ... }))`（`statements.rs:857`）

**为什么不能改成 task**

分配器是被 **`before_wal` 同步闭包**调用的：

```
commit_deferred_locked                         async
  └─ before_wal(&mut txn)                      同步闭包，持有 commit-ts turn 时调用
     └─ reserve_rows_at → reserve_rows_from
        └─ block_on(reserve_rows_from_async(...))   ← park 当前线程等 oneshot
           └─ allocator.jobs.send(job)              分配器消费它
```

把消费端改成 runtime task，就等于说「这个 `block_on` 卡住的线程，必须由同一个
runtime 的 worker 来推进」。如果 `block_on` 恰好发生在 runtime worker 上（`execute`
链就是这样），那个 worker 被 park，task 永远没机会被 poll → 死锁。

实测现象：测试进程 6 个线程，**一个 `task-scheduler` 都没有**（说明 runtime 尚未
初始化或 worker 全被 park），两个测试线程停在 `futex_do_wait`。

**保留 blocking 线程的正确性依据**

分配器是「发号器」：单消费者、FIFO、每个 job 是纯内存（§核实记录见下）。它作为
blocking 线程只占一个 blocking 额度，换来的是**回执不依赖 runtime 推进**——`block_on`
总能拿到结果。这个交换在「一次死锁 vs 一个常驻额度」之间毫无悬念。

**核实记录**（曾误判为「首次分配要 `spawn_blocking`」，此处留作反例）：

| 疑问 | 实际 |
|---|---|
| `row_counter` 何时是 `None`？ | 只有**空表**。全仓没有 `row_counter = None`；它只在 `statements.rs:344`（启动恢复）和 `:1512`（自己）被设成 `Some`。DROP 表时整个 `TableEntry` 被 `tables.remove()`（`ddl.rs:65`），重建即空表。 |
| 空表会读 `version` 吗？ | 不会。`BlockIndex::open` 在 `read_dir(table_root)` 失败时立刻 `Ok(empty)`（`storage/src/common/block_index.rs:76-78`）——空表没有 `tbl_{id}` 目录。 |
| 有块时会读一遍全部 `version` 吗？ | 不会。`open` 的循环第一个缺失就 `break`；而 `block_segment_row_counts` 走 `visible_block_ids` → `BlockIndex::metas()`，元数据已在内存，`read_block_version` 只在 BlockIndex 未收录该块时才调用。 |

首次分配的开销 = 一次 `read_dir`（空表立即返回）+ 一次内存查表。不是阻塞叶子，所以
job 体里**不需要** `spawn_blocking`——它只是不能是 task。

**保留的另一半**：发送侧的 `reserve_rows_from_async` / `reserve_import_rows`
（`begin_import` 等导入路径）已经是 `.await`，这些路径的调用者都是 async，不需要桥接。
同步的 `reserve_rows_from` 保留给 `before_wal`，内部 `block_on`——此时它已经在
一个「被同步闭包调用」的位置上，那个 `block_on` 是必需的，而且分配器是 blocking
线程，所以它一定返回。

**收益**：0（不改）。**风险**：不改的风险是 0；改的风险是 19 个测试挂死。

### 1.3 `BGTaskProcessor`（`infinity-bg-0`）

现在的形态（`bg_task.rs:82`、`:186`）：

```rust
std::thread::Builder::new().name("infinity-bg-0")
    .spawn(move || worker(&mut queue, &shared))

fn worker(queue, shared) {
    while let Some(task) = queue.blocking_recv() {
        catch_unwind(|| task.run())      // run 内部：block_on(engine.xxx())
    }
}
```

三个生产 `BgTask::run` 内部都是 `block_on(...)`：

| 实现 | 位置 | `run` 内部 |
|---|---|---|
| `DumpMemIndexTask` | `state.rs:7819` | `block_on(engine.dump_all_memory_indexes())` |
| `CleanupTask` | `state.rs:7835` | `block_on(engine.cleanup())` |
| `OptimizeTask` | `state.rs:7849` | `engine.optimize_indexes()` → 内部 `block_on(optimize_index_chunks())` |

**上一版写的是 `spawn_blocking(move || catch_unwind(|| task.run()))`，那是错的**：它把一整个 async 事务流程（`flush_lock.read().await`、`txn.commit_at(..).await`、`run_commit_bottom(..).await`、WAL 的 `oneshot` 回执）丢进一个 blocking 线程，等于在阻塞池里再嵌一层 `block_on`。

正确形态：`run` 上浮 async，等待点保持 `.await`，只有阻塞叶子进池。

```rust
pub trait BgTask: Send + 'static {
    fn name(&self) -> &str;
    fn run(self: Box<Self>) -> Pin<Box<dyn Future<Output = ()> + Send>>;
}
```

（`Box<dyn BgTask>` 决定了不能用 native `async fn` in trait，必须返回 pinned box。）

worker 循环：

```rust
while let Some(task) = queue.recv().await {
    let name = task.name().to_string();
    // panic 与 abort 都变成 JoinError；outstanding 的递减放在 await 之后无条件执行
    if let Err(e) = tokio::spawn(task.run()).await {
        eprintln!("background task '{name}' failed: {e}");
    }
    if shared.outstanding.fetch_sub(1, Ordering::SeqCst) == 1 { wake }
}
```

三个任务各自的叶子：

| 任务 | 阻塞叶子 | 其余（保留 `.await` / 纯内存） |
|---|---|---|
| `DumpMemIndexTask` | `seal_index_tail` 的每个分支：`seal_text_index_unlocked` 的 `job.write()`（`state.rs:5524`，注释明写 "No lock held: the segment files are written here"）、`dump_hnsw_index`（`blocks/engine.rs:4404`）、`write_ivf_index`（`blocks/engine.rs:5786`）、`seal_secondary_index`、`seal_multivector_index` | `reserve_commit_ts`、`flush_lock.read().await`、`txn.commit_at(..).await`、`run_commit_bottom(..).await` |
| `CleanupTask` | `reclaim_deprecated_segments`（`statements.rs:4423`）、`reclaim_dropped_block_columns`（`blocks/engine.rs:2196`）、`cleanup_index_chunks`（`blocks/engine.rs:4868`）——都是删目录 / 删文件 | `applied_ts()`、`append_cleanup().await` |
| `OptimizeTask` | `optimize_index_chunks`（`blocks/engine.rs:4597`）里的 chunk 合并写 | `ensure_writable`、`index_epoch` 递增 |

**顺带的一致性问题**：`dump_index_transaction`（`index_build.rs:843`）现在是在 `flush_lock.read().await` 的临界区（`:866-871`）里**同步**调用 `seal_index_tail`。叶子进池后有两种做法：

- 让读锁跨 `.await` 持有（`tokio::sync::RwLock` 允许），但 checkpoint 的写锁会等更久；
- 改成「取锁 → 快照 → 放锁 → 池里写 → 重新取锁 → 装回」。`seal_text_index_unlocked` 内部本来就是这个两段式（`claim_seal` 锁内、`job.write()` 锁外、`complete_seal` 再进锁），可以复用。

**建议第二种**，否则 1.1b 做完之后 checkpoint 的 `flush_lock.write()` 会和 dump 的读锁互相拉长。

**`submit` / `drain`**

实测（上一版数字有误，这里重数）：

| 项 | 实际（`grep` 逐条核对） |
|---|---|
| `impl BgTask` | **7**：生产 3（`DumpMemIndexTask` / `CleanupTask` / `OptimizeTask`）+ 测试 4（`bg_task.rs:199` 的 `Increment`、`bg_task_ut.rs:199` `CountTask`、`:229` `PanickingTask`、`accessors_ut.rs:43` `BlockingTask`） |
| 生产 `processor.submit` 调用点 | **3**：`bg_task.rs:218`（测试内 `Increment`）、`statements.rs:502`（`enforce_memindex_quota`）、`statements.rs:8036`（`submit_task`）——**生产真实路径是后两个** |
| `processor.submit(` 合计（含测试） | **9** |
| `processor.drain()` 调用点 | **6**（生产 2：`bg_task.rs:220` 的 `Drop`、`statements.rs:8012`；测试 4） |
| `drain_background_tasks` 出现 | **5**（1 处定义 + 4 处测试） |

`enforce_memindex_quota` 自己是同步 `fn`（`statements.rs:442`），但它的调用者 `run_bottom`（`state.rs:4860`）是 `async fn`，`update_text_indexes_batch` 的调用者（`insert_literals_inner` `dml.rs:509`、`copy_from_inner` `import.rs:28`）也都是 `async fn`。也就是说它**已经在 async 上下文里**，现在的 `blocking_send` 正在 runtime worker 上做阻塞发送——这本身就是要修的问题，不是「改成 async 会传染」的风险。

- `submit`：保持同步签名（避免把 `enforce_memindex_quota` 一路改成 async），内部改 `try_send`。队列 1024 满时返回 `false`，调用方已有同步 dump 分支（`statements.rs:504-513`）。
- `drain()`：保留同步签名（内部 `block_on`），因为 6 处调用点里 4 处是测试。

**收益**：少 1 个常驻线程；`BgTask` 的等待点回到 `.await`，不再在阻塞池里嵌套 `block_on`。**风险**：中——trait 签名变化波及 7 个实现 + 9 处 `submit` / 6 处 `drain`。

### 1.4 `wal-writer`（`txn` crate）— 不建议改

现在：`txn/src/lib.rs:213`，`std::thread::Builder::new().name("wal-writer")` + `queue.blocking_recv()`，`WalManager` 活在这个线程里。

**不建议改**，理由：

1. 换文件（`SwapWalFile`）要求「检查大小 + 改名」原子完成，独占线程是最直接的正确性保证；
2. `spawn_blocking` 要求 `'static`，而 `job` 借 `&mut WalManager`。要改只能 `Arc<Mutex<WalManager>>`，那就是把 `&mut` 换成锁——线程数不减（仍需要一个 driver 消费队列），却多一层锁和 panic 传播面；
3. 另一条路「在 `txn` 内部建 `current_thread` runtime 驱动 `recv().await`」只是把同一根线程换个写法，收益为零。

**净收益为 0 或负。** 如果将来必须统一，唯一干净路径是 `Arc<Mutex<WalManager>>` + 单 task，并明确接受多一层锁。

### 1.5 三套 server runtime 合并

现在 `server/src/{lib,http,pg}.rs` 各自建一套 `new_multi_thread()`（`lib.rs:315`、`http.rs:69`、`pg.rs:573`），只设了 `thread_stack_size(8 MiB)`，**没设 `worker_threads`**，即 tokio 默认 = 核数。`run()` 里 `runtime.block_on(serve())` 之后 `std::process::exit(0)`。

改造：用 engine 的 runtime 承载三个 front end。

```rust
// cli
let runtime = <engine runtime handle>;         // 或自建一个，8 MiB 栈
runtime.spawn(http.serve());                   // serve 已经是 async fn
runtime.spawn(pg.serve());
runtime.block_on(server.serve());
```

- `HttpServer::run` / `PgServer::run` / `Server::run` 三个 `fn run(self) -> !` 的 `serve` 去掉，暴露 `pub async fn serve(self)`（三个 `async fn serve` 已存在，只是私有）
- `std::process::exit(0)` 从三处上移到 cli 一处
- **测试也要改**：`server/src/lib.rs:7795/7862/7886`、`http.rs:3535` 等 4 处 `std::thread::spawn(move || server.run())`，`run` 消失后要改成自建 runtime 或 `serve`

**收益**：`3 × 核数` → `1 × 核数`，常驻线程减少 `2 × 核数`。（`fan-out` 兜底 runtime 仍在，它不在这三套里。）**风险**：

1. 三处 8 MiB 栈必须保留（注释记录了 `dispatch_arm` 的 1500 行 match 会栈溢出）；
2. 三个 front end 共用一组 worker，语句本身已经 `.await` 掉 engine 的等待点，CPU 部分在 `spawn_blocking`，理论上不互堵——但这条要实测；
3. 需重跑 Gate A、三协议冒烟、1000 空闲连接线程数（§9 判据 < 50）。

### 1.6 `cli` 的 `malloc_trim` 线程

`cli/src/main.rs:245`：`std::thread::spawn` + `sleep` → `runtime.spawn` + `tokio::time::sleep().await` + `spawn_blocking(trim_allocator)`。unsafe 仍留在 cli（`infinity-executor` 有 `#![forbid(unsafe_code)]`）。**风险**：低。

### 1.7 SELECT 链：去掉 `run_select` 里最后那次 `block_on`

#### 现状（已实测，不是推断）

`execute` 的 SELECT 分支（`query.rs:2582-2602`）把整个查询交给 `spawn_blocking`：

```rust
let Some(engine) = self.engine_handle() else { ... };
let stmt = stmt.clone();
let (columns, rows) = crate::scheduler::scheduler()
    .spawn_blocking(move || engine.run_select(&stmt))
    .await?;
```

而 `run_select` → `run_select_with_ctes`（603 行）里，**恰好有一处** `block_on`：

```rust
// statements.rs:2115
Some(crate::scheduler::block_on(self.derived_table(f.trim()))?)
```

`derived_table` 是 `async fn`，内部 `self.execute(sql).await` —— 也就是说，**一个完整语句链（parse → `ensure_statement_id` → qualify → 结果缓存 → `execute_stmt_retrying` → `run_select`）被一个 park/unpark 循环在 blocking 池线程上驱动**。

这次审计把整条链都量过了，结论是：

| 函数 | 行数 | `block_on` | `.await` |
|---|---|---|---|
| `run_select_with_ctes` | 603 | **1** | 0 |
| `run_plan` | 138 | 0 | 0 |
| `resolve_stmt_subqueries` | 89 | 0 | 0 |
| `direct_projection` | 161 | 0 | 0 |
| `try_projection` | 94 | 0 | 0 |
| `try_rows_pruned` | 82 | 0 | 0 |
| `search_text_terms` | 839 | 0 | 0 |
| `run_search` | 27 | 0 | 0 |
| `index_lookup` / `index_scan_range_rows` | — | 0 | 0 |
| `fetch_rows` / `fetch_rows_by_ids` / `fetch_rows_with_ids` | 7 / 8 / 32 | 0 | 0 |
| `run_subquery` | 26 | 0 | 0 |

**整条 SELECT 链是纯同步的，唯一的 async 依赖就是那一行 `derived_table`。** 这决定了改法：

- **不是**「把 600 行递归改成 async」——那要改 `run_select` / `run_select_with_ctes` / `run_plan` / 所有算子的签名，而它们全是同步的，改了没有任何收益，只是把同步代码包成 `async` 再 `block_on` 回来；
- **而是**消除那唯一一处跨 async 边界的调用。

#### 为什么当初是 `execute` 而不是 `run_select`

`derived_table` 用 `execute(sql)` 而不是 `parse` + `run_select`，是因为 `execute` 多做四件事，其中三件**不能丢**：

| `execute` 的步骤 | derived table 是否需要 | 说明 |
|---|---|---|
| `txn.ensure_statement_id()` | 需要，但**已经满足** | thread-local 且幂等（`txn/src/lib.rs:555-568`）：外层语句已经 take 过 id，子查询复用同一个。改成 `run_select` 不改变这一点。 |
| `context.query_count += 1` | 不影响结果 | 只用于 `SHOW VARIABLES` 的计数，子查询计不计都可以（当前会计）。 |
| `qualify_tables(&s)` | **需要** | `run_select_with_ctes` 只对 **CTE** 做 `qualify_tables_in`（`statements.rs:2034`），对 derived table 的表名**不做**。直接调用 `run_select` 会让 `FROM (SELECT * FROM t)` 里的 `t` 保持未限定，解析到错误的库。 |
| 结果缓存 (`result_cache`) | 不需要 | 子查询命中缓存只是快一点；不命中走同一条 `run_select`。 |
| `execute_stmt_retrying` | 不需要 | 只读语句不冲突，重试循环一次就过。 |

所以正确改法是：`derived_table` 换成 **parse + qualify + run_select**，保持语义，去掉 async。

#### 改动

```rust
// query.rs —— 由 async 变同步，不再走 execute
pub(crate) fn derived_table(&self, sql: &str) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
    for s in parser::parse(sql)? {
        // 与 `execute` 的同一条限定规则，否则 `FROM (SELECT * FROM t)`
        // 里的 `t` 不会被解析到当前库。
        let qualified = self.qualify_tables(&s);
        if let Statement::Select { .. } = &qualified {
            return self.run_select(&qualified);
        }
    }
    Ok((Vec::new(), Vec::new()))
}
```

```rust
// statements.rs:2115 —— 那一行变成普通调用
Some(self.derived_table(f.trim())?)
```

改完之后 `run_select_with_ctes` 的 `block_on` 计数从 1 变 0，整条 SELECT 链**没有任何跨 runtime 的等待**，`spawn_blocking(move || engine.run_select(&stmt))` 才真正是"一个纯 CPU 阻塞单元"，符合 §6.6 与 §5.7 的粒度标准。

#### 顺带确认的两件事

1. **`derived_table` 的另一处调用点**：全仓只有 `statements.rs:2115` 一处（`grep` 实测），改动面就是这两个文件 + `query.rs` 里那 6 行。
2. **`run_subquery`（标量子查询）已经不走 `execute`**：它直接 `parse` + `run_select_with_ctes`（`query.rs:20-46`），本来就是同步的，不受影响。

#### 验收

| 项 | 判据 |
|---|---|
| 语义不变 | `test/sql` 里所有含 `FROM (SELECT …)` 的用例（`sqllogictest` 覆盖）+ pysdk `test_select` |
| `block_on` 清零 | `run_select_with_ctes` 函数体内 `scheduler::block_on` 出现 0 次（可加一条断言式注释或用 `grep` 校验） |
| Gate A | 208/208 |
| 无回归 | `cargo test -p executor` 722 项 |

**风险**：中。`derived_table` 从"完整语句"降级为"只看 SELECT"——如果将来有 `FROM (WITH ...)` 或 derived table 里出现 DML，会静默返回空。当前 SQL 语法只允许 derived table 是 SELECT（`f.trim_start().to_uppercase().starts_with("SELECT ")` 这个判断本来就已经限定了），所以是安全的；但要保留那条前缀判断作为守卫。

**不做的事**：不把 `run_select` 链改成 `async fn`。它 603 行全是同步递归（`run_select_with_ctes` 自己调用自己），改成 async 会得到一棵 `Box<dyn Future>` 递归，既没有等待点可省，又会让 `spawn_blocking` 那层变得没有意义。

### 1.8 `ALTER TABLE ADD COLUMN` 的 commit effects：同类问题，同样处理

审计 1.7 时顺手查了「哪里的 `block_on` 直接写在 async 函数体里」，找到第二处：

```
execute_stmt_inner                        async
└─ ddl_commit_with_effects(...)           async
   └─ effects(plan)                       ← 同步闭包，在 async 体里直接调用
      ├─ patch_buffered_add_columns        纯内存
      └─ add_column_files                  fn
         └─ block_on(bottom_gate.lock())    ← 在 runtime worker 上同步等 tokio 锁
```

`block_on` 在自己就是 runtime worker 的线程上等一把 `tokio::sync::Mutex`，会把那个 worker 停住，而锁的持有者可能是同一 runtime 上别的 task。这和 1.7 是同一类错误，只是入口不同。

改法与 §1.1b 一致——把等待点提到 async 层，叶子留在下面：

```rust
// statements.rs：effects 从「同步闭包」变成「返回 future 的闭包」
pub(crate) async fn ddl_commit_with_effects<T, E, Fut>(
    ...
    effects: impl FnOnce(E) -> Fut,
    ...
) -> Result<T> where Fut: std::future::Future<Output = Result<()>> {
    ...
    let effect_result = effects(plan).await;
```

```rust
// query.rs：ADD COLUMN 的 effects 改成 async 块
|blocks| async move {
    self.patch_buffered_add_columns(table, base_index as usize, &added);
    self.add_column_files_async(table, base_index as usize, &blocks).await
},
```

```rust
// blocks/engine.rs：拆成 async 外壳 + 共用主体
pub(crate) async fn add_column_files_async(...) -> Result<()> {
    let _bottom = (!HeldBottom::holds(...)).then(|| gate.lock());
    let _bottom = match _bottom { Some(t) => Some(t.await), None => None };
    self.add_column_files_inner(table, first, blocks)
}
pub(crate) fn add_column_files(...) -> Result<()> {   // 留给同步的 replay 路径
    let _bottom = ... .then(|| block_on(gate.lock()));
    self.add_column_files_inner(table, first, blocks)
}
```

同步版保留，因为 **`wal_bridge::replay` 是同步函数**（`open_with_options` 在启动时逐帧重放，那里没有 runtime worker），它的 `add_column_files` 调用是合法的。两个调用点各用各的版本，`block_on` 只留在同步那一个里。

**判断标准**（值得记住）：同一个「必须持有某把 tokio 锁」的操作，如果有两个调用者——一个在 async 上下文、一个在同步上下文——就拆成 async / sync 两个入口，共用一个**已经持锁**的主体。**不能让 async 调用者去用同步入口**，那正是 1.7 和 1.8 两次错误的来源。

### 1.9 锁的选型：什么该用 `tokio::sync::Mutex`，什么不该

> 这一节是实测结论。曾经把 `Engine` 的 14 个锁字段一刀全换成 `tokio::sync::Mutex`，
> 结果 19 个测试死锁、2 个 panic，全部回退。

**判据只有一条：这把锁会不会跨 `.await` 持有？**

| 字段 | 使用者 | async 内跨 await 持锁 | 结论 |
|---|---|---|---|
| `ddl_lock` | 4 | **是**（DDL 的整个 capture + commit） | `tokio::sync::Mutex` ✅ |
| 每表 bottom gate / batch gate | 8 | **是**（一次 bottom 半部可以很长） | `tokio::sync::Mutex` ✅ |
| `tables` | **112** | 否（最长是一次 `HashMap` 查找 + clone） | `std::sync::Mutex` |
| `result_cache` | 11 | 否（`get` / `len` / `clear` 各一次） | `std::sync::Mutex` |
| `current_db` / `sessions` | 9 | 否（一次 `String` / `HashMap` 操作） | `std::sync::Mutex` |
| `batch_locks` / `bottom_gates` / `bottom_queues` / `allocator` / `bg_processor` / `self_weak` / `vector_cache` / `cpu_sample` / `queries` | 25 | 否（注册表：一次 entry 查找） | `std::sync::Mutex` |

**为什么 `tables` 不能改成异步锁**：它的 112 个使用者里有 102 个在同步函数里，而
异步调用这些函数的地方有 **99 对**（`execute_stmt_inner` → `invalidate_row_cache`、
`run_bottom` → `note_append`、`compact_segments` → `rebuild_indexes` …）。改成
`tokio::sync::Mutex` 意味着这 74 个同步访问器全部要变 async，进而把 `run_select`
（603 行同步递归）、`run_plan`、全部算子链、`fetch_rows` 一路 async 化，包括那些
**只被同步 bottom 作业调用**的路径（`BottomTask::commit_bottom` 是同步 trait）。

**为什么同步锁在 async 体内是安全的**：`std::sync::Mutex` 只在**争用**时才阻塞。
这些锁的临界区是「一次哈希查找」量级，持有者不可能在里面停留到需要被调度出去；
而每一次 `tokio::sync::Mutex` 的 `lock().await` 都要付一次原子操作和一个可能的
park/unpark 往返。对一个每次查询要取几十次的表注册表，后者的代价明显更高。

**代价与边界**：这条结论依赖「临界区里不调用任何可能重入或阻塞的东西」。实施时
已经核对过：`tables` 的 112 处全部是「lock → 一次 HashMap 操作 → drop」，没有一处
在里面调用其它取锁函数、做 I/O 或 `await`。**改动这些代码时必须保持这个性质**——
一旦有人在一把 `tables` 锁里加了 `.await` 或第二次 lock，就会退化成死锁（§5.8 记录
的 `HeldBottom` 事故就是这一类）。

## 2 不改的

| 站点 | 理由 |
|---|---|
| `wal-writer` | 见 1.4：独占文件句柄 + 换文件原子性；改法净收益 0 或负 |
| `scheduler` runtime 本身 | 这是 engine 的 runtime，本来就是 tokio |
| `fan-out` 兜底 runtime | 只在没有 runtime 的调用方（bench / example / 独立单测）用到 |
| `BottomExecutor`（storage） | 生产不走它（`bottom_queues` + blocking 池），只在自身单测 `start(4)` |

## 3 顺序与验收

| 步 | 内容 | 验收 | 收益 |
|---|---|---|---|
| 1a | 1.1a trigger 循环 + 1.6 trim（1.2 **不做**） | `cargo test -p executor`（722）、`-p txn`（32）、`cargo test --workspace`、Gate A 208 | 少 2 个常驻线程 |
| 1b | 1.1b `flush_blocks` 链 async 化 | 同上 + 字节门禁 + `a_checkpoint_scaling_probe` | 去掉 checkpoint 链上的嵌套等待 |
| 2 | 1.3 bg worker（trait 上浮 async，叶子拆池） | `bg_task_ut`、`dump_mem_index_ut`、`compact_replay_ut`、`optimize_replay_ut`、`--workspace` | 少 1 个常驻线程 |
| 3 | 1.5 三套 server runtime 合并 | Gate A + thrift/http/pg 冒烟 + 1000 空闲连接线程数 < 50 + pysdk 全量 | 少 `2 × 核数` 个 worker 线程 |
| 3b | 1.7 SELECT 链去掉 `derived_table` 的 `block_on` | `test/sql` 的 `FROM (SELECT …)` 用例、pysdk `test_select`、Gate A 208、executor 722 | SELECT 的 `spawn_blocking` 变成真正的纯阻塞单元 |
| 3c | 1.8 `ALTER TABLE ADD COLUMN` 的 effects 改 async | `alter_ut`、`compact_replay_ut`、Gate A 的 ALTER 用例、executor 722 | 去掉 async 体里的 `block_on` |
| 4 | 1.4 WAL | `test_cross_engine_bytes_40_wal.py`（逐字节）、`txn` 18 项 | 0（不建议） |

1a / 2 / 3 / 3b 之间没有依赖，可以分开做。1b 依赖 1a（trigger 已经 async 才有意义）且风险最高，建议单独一轮。

3b 的改动面：`query.rs` 的 `derived_table`（6 行）+ `statements.rs:2115`（1 行）。它是本次审计里**唯一**一处「`block_on` 在驱动一个完整 async 语句链」的地方，也是 `spawn_blocking(run_select)` 这个粒度是否成立的最后一环。

## 4 同步边界的处理原则

`scheduler::block_on` 的分布（实测）：

| 范围 | 处数 |
|---|---|
| 生产代码（`statements.rs` 10、`state.rs` 7、`admin.rs` 7、`snapshot.rs` 7、`dml.rs` 3、`blocks/engine.rs` 3、`tasks.rs` 1、`query.rs` 1） | **39** |
| `blocks/misc.rs`（全部在 `mod tests`，第 334 行起） | 29 |
| 其余 `*_ut.rs` / `replay_*_ut.rs` 等测试文件 | 286 |
| 全仓 | **354** |

上表的「39 处」才是改造要处理的量；测试里的 315 处（29 + 286）多数是 `e.flush()` / `checkpoint()` 这类同步适配器调用，保留包装后不用动。留下的都必须满足：

> **同步 `fn` 只服务「测试 / CLI / `execute_blocking` 适配器」**，内部 `block_on`。业务逻辑本身是 `async fn`，同步版本是薄包装。

做法：被改的 `async fn` 旁边保留同名 `pub fn` 包装（内部 `block_on`），测试和 CLI 不动；新增代码一律写 async。

**例外**：`enforce_memindex_quota` 这类「同步 `fn` 但调用者已是 async」的函数，属于上面说的待修项（它在 worker 上 `blocking_send`），不在「薄包装」之列，要按 1.3 的 `try_send` 处理。

## 5 已知的坑

1. **`spawn_blocking` 的额度**：常驻的「长循环」从 `std::thread` 变成 blocking 池的常驻占用。tokio 默认上限 512，与现在等价；但**不要**让嵌套扇出再往 blocking 池派发（`parallel.rs` 已用 `task::try_id().is_none()` 处理）。
2. **`current_thread` runtime**：`block_in_place` 在其中会 panic。`parallel.rs` 已避开；改造 trigger / 分配器时同样不能在 `current_thread` 上假设有第二个 worker。
3. **panic 隔离**：bg worker 现在用 `catch_unwind` 保证 `outstanding` 递减。`run` 上浮 async 后 panic 变成 `JoinError`，递减必须放在外层循环（`await` 之后无条件执行），否则一次 panic 就让 `drain` 永等。
4. **顺序性**：WAL 帧顺序、行号顺序、同表 bottom 顺序，三者都依赖「单消费者 + FIFO」。改成单个 `spawn` 的 `while let Some(..) = recv().await` 保持这一点；**不要**为了让某项「并行」而拆成多个消费者。
5. **进程退出**：`std::process::exit(0)` 跳过 `Drop`。合并 runtime 后，`WalWriter::drop` 的 join（保证 `wal.log` 关在本线程上）需要显式调用，否则重开同一目录可能撞上未关闭的文件。
6. **`std::sync::Mutex` 不能跨 `.await`**：1.1b 把 `flush_table` 外壳改 async 后，`tables.lock()` 必须只在写文件之后、不跨越 `.await` 的段落里持有。同理 1.2 的 `reserve_rows_inline` 目前是安全的，但改动时不能引入跨越 `.await` 的持锁。
7. **粒度检查清单**：新增任何 `spawn_blocking` 前先问四句——

   (a) 闭包里有没有 `.await`？有就是错的，先把等待点提到外面。
   (b) 闭包里有没有 `scheduler::block_on`？有就是错的，说明在池里嵌套了运行时。
   (c) 闭包是「一次文件写 / 一次 fsync / 一次纯计算」还是「一个事务 / 一次 dump / 一整张表」？后者错。
   (d) **这条路径实际执行到哪些 I/O，核实过没有？** 必须顺着调用链看到真实的 `read` / `write` / `fsync`，或确认只在内存数据结构上操作。**不允许按函数名推断**——本文件出现过 5 次同类误判（`BgTask::run`、`dump_index_transaction`、`next_row_seq`、`flush_blocks`「改动面只有一个文件」、各处调用点数），全部是「看到名字 / 看到 `async` 关键字」就下结论造成的。
   (e) **闭包需不需要判断"我是不是已经持锁"？** 如果需要，而那个判断是 **thread-local**，那么在叶子跑在另一个线程时它就是错的——见下面的 §5.8。

   自检时把调用链写下来，写不出来就是没核实。

8. **thread-local 的"重入标记"不能跨线程**（本轮实现踩到的真 bug）：

   `HeldBottom`（`statements.rs:29-56`）是 `thread_local!` 的 `HashSet<String>`，用来回答
   "本线程是否已经持有这张表的 bottom gate"，从而让
   `write_blocks_mode`（`blocks/engine.rs:241`）在重入时跳过取锁。

   1.1b 把 `flush_table` 改成 async 之后，`HeldBottom::enter` 仍在外壳（runtime worker）
   上执行，而写文件跑在 **blocking 池线程**上。那个线程的 `BOTTOM_HELD` 是空的，于是
   `HeldBottom::holds` 返回 `false`，`write_blocks_mode` **第二次**获取 bottom gate——
   `tokio::sync::Mutex` 不可重入，直接自锁。

   修法：把"要不要取锁"从 thread-local 推断改成**调用方显式声明**。

   ```rust
   // blocks/engine.rs
   pub fn write_blocks_bounded(&self, table: &str, ts: u64) -> Result<usize> {
       self.write_blocks_mode(table, &[], false, ts, true)   // 自己取锁
   }
   pub(crate) fn write_blocks_bounded_locked(&self, table: &str, ts: u64) -> Result<usize> {
       self.write_blocks_mode(table, &[], false, ts, false)  // 调用方已持锁
   }
   ```

   `flush_table` 的叶子用 `_locked`（它在外壳里 `gate.lock().await` 过），compaction 的
   bottom 作业也用它（在那之前已经 `block_on(gate.lock())`）。**凡是跨了线程边界的
   重入判断，都必须换成显式参数或显式传递的 guard。**

9. **两个真实的死锁（本轮实测，各挂死 19 个测试）**

   **(a) `write_blocks_commit` 重复取 gate。** 1.1b 给 `write_blocks_mode` 加了
   `take_gate` 参数后，`write_blocks_commit` 一度传 `true`。但它的**两个调用者**
   （`dml.rs` 的 `commit_import_chunk`、`import.rs` 的 import 尾巴）在调用之前都已经
   `block_on(gate.lock())` 并 `HeldBottom::enter`。于是同一个 `tokio::sync::Mutex`
   被同一线程取第二次 → 自锁。栈：

   ```
   block_on(tokio::sync::Mutex::lock)
   write_blocks_mode
   write_blocks_commit
   commit_import_chunk 的 before_wal 闭包
   ```

   **教训**：`take_gate` 这种参数必须逐个调用点核对「它是不是已经持锁」，不能按函数名
   想当然。`write_blocks_commit` 的语义就是「commit 的 bottom 写块」，bottom 一定持锁，
   所以它的默认值必须是 `false`。

   **(b) 分配器改成 task 后，`before_wal` 的同步桥接到死锁。** 1.2 把行号分配器从
   「blocking 线程 + `blocking_recv`」改成「runtime task + `recv().await`」。但分配器
   是被 **`before_wal` 同步闭包**调用的（`commit_deferred_locked` 在持有 commit turn
   时同步调用它），而那条路径用 `block_on` 桥接。`block_on` 会 park 当前 runtime
   worker，而分配器的任务需要同一批 worker 来 poll → 永久等待。

   **结论：分配器必须保留为 blocking 线程**（`scheduler().spawn_worker`），它的回执
   不能依赖 runtime 推进。计划书 §1.2 的「改成 task」是错的，已回退。

   **通用判断**：**凡是「被同步闭包经 `block_on` 调用」的服务（分配器、WAL 写线程），
   其消费端都不能是 runtime task。** 判断方法：问「这个 `block_on` 卡住时，谁来推进
   它等待的东西？」如果答案是「同一个 runtime 的 worker」，那就是死锁。

   同样的形状还有 `run_commit_bottom` 里的 `BOTTOM_HELD.with(...)`（`statements.rs:800`）：
   它之所以还能用，是因为那段代码与 `HeldBottom::enter` 在**同一个** blocking 线程上
   顺序执行（`wrapped` 闭包里先 `enter` 再 `work`）。**一旦把 bottom 作业也改成 task，
   这条就会以同样的方式失效。**

## 6 本文件修订记录

| 修订 | 内容 |
|---|---|
| 1 | 初版 |
| 2 | 撤回 1.3 的 `spawn_blocking(move || task.run())`——那是把整个 async 事务流程丢进阻塞池 |
| 3 | 撤回 1.2 的「首次分配进 `spawn_blocking`」——空表不读任何 `version` 文件 |
| 4 | 补充 1.0 / 1.1b：`flush_blocks` 链是同步混合负载，1.1 不可能只改 `tasks.rs`；修正调用点数、`BgTask` 实现数、`submit` / `drain` 调用点数、server `worker_threads` 的表述、`block_on` 的分布口径 |
| 5 | 落地后审计：新增 §1.7（SELECT 链唯一一处跨 async 边界的 `block_on`）、§5.8（`HeldBottom` 是 thread-local，异步化后不能再靠它判断"是否已持 gate"）；补 3b 到顺序表。§5.7 的粒度清单新增第 5 问 |
| 6 | 再次审计「`block_on` 直接写在 async 体内」：新增 §1.8（`ALTER TABLE ADD COLUMN` 的 commit effects 在 worker 上等 tokio 锁），顺序表补 3c。给出通用的判断标准：同一操作若同时有 async / 同步调用者，拆成两个入口共用一个已持锁的主体 |
| 7 | 实测反馈：§1.2 回退（分配器改 task 导致 19 个测试死锁）、§5.9 记录两个真实死锁（`write_blocks_commit` 重复取 gate、分配器 task 化）。修正 §0 改造后状态表 |
| 8 | 新增 §1.9：锁的选型判据（跨不跨 `.await`）。14 个字段中只有 `ddl_lock` 与每表 gate 需要 `tokio::sync::Mutex`；`tables` 等 12 个是「一次哈希操作」的短临界区，改异步会强制 74 个同步访问器 async 化。全量切换的实测代价：19 个测试死锁 + 2 个 panic，已回退 |
