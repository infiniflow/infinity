# 调用探针审计

审计单元是**一个可复现的问题**，不是"所有函数的染色差分"。一个问题一个 probe：固定目标函数、固定语料、固定观测方式，**运行前写死预期结果**。产物是带 build id 与哈希的观测文件，不是一份待实现的仪器规格。

本文登记：已测量的事实（§2）、probe 的契约（§3）、相位分离（§4）、目标账本（§5）、与既有门禁的边界（§6）、未覆盖项（§7）、probe 清单（§8）、复现环境（§9）。

**三态，没有第四态**：`equal`、`different`、`unavailable`。`unavailable` 表示这次观测不可信（挂载失败、进程被信号终止、导出为空、配置压不住后台线程），**不是 0，也不等于 equal**——probe 判失败并重跑，不允许用"没观测到"推出"没有调用"。

## 1 为什么不做全量染色

三个数字决定了它：参考侧 `STT_FUNC` 去重地址 **484,744** 个，重写版 **127,292** 个；bpftrace 的 `max_map_keys` 默认 4096，单进程挂不下，按 512/批需要 ~950 + ~250 批，每批一次引擎启停与数据目录重置；跨批不共享序号，拼不出同一事件流。一轮全量语料是十几个小时，结论仍然只是"哪些函数被调用过"。

更根本的一条：**探针本身有损且静默**——map 会静默挤出、`++` 非原子、脚本不能遍历 map 导出、`uretprobe` 会漏。为这些失败模式写检测规则、再给规则之间定优先级，规格会持续增长而不收敛。所以本机制只回答"一个明确断言能说清"的问题；其余显式登记为未覆盖（§7），不假装有覆盖。

## 2 已测量事实

可信度只有四个值：**实测**（有观测文件/输出）、**读源码**（行号可查）、**已登记**（别的文档里已有的结论）、**未复核**。

| 事实 | 数据 | 出处 | 可信度 |
|---|---|---|---|
| 参考一次手工观测（窗口标为 `Connect` + 一条 `CREATE TABLE`） | `BeginTxnShared` 2、`NewTxn::Commit` 2、`CommitBottom` 1、`BeginTxn` **0** | 手工 uprobe，`-O0` 构建 | 实测；**窗口的实际成员未记录**，见下三条 |
| `default_db` 只在首次启动创建 | `if (system_start_ts == 0) { CreateDefaultDB(); }` | `Storage::AdminToWriter`，`src/storage/storage_impl.cpp:430`；函数体 `:973-991` | 读源码 |
| `CreateDefaultDB` 走的是 `BeginTxn`（非 Shared） | `new_txn_mgr_->BeginTxn(..., kCreateDB)` | `storage_impl.cpp:974` | 读源码 |
| 每个非 admin 语句各一次 `BeginTxnShared` | 两处调用点（checkpoint/cleanup 分支与常规分支） | `src/main/query_context_impl.cpp:602,613` | 读源码 |
| 因此那 2 次 `BeginTxnShared` **不能**由 `CreateDefaultDB` 解释 | 它既不是 Shared、也只在启动跑 | 同上两条 | 推论：窗口构成**未定**，由 `ddl-txn-participation` 重测（§8） |
| 监听先于存储初始化 | `StartThriftServers()` 在 `InitPhase2()` 之前 12 行 | `src/bin/infinity_main.cpp:273` / `:285` | 读源码：启动期与首个连接可能重叠 |
| 参考只接受单语句 | `if (parsed_result->statements_ptr_->size() != 1) UnrecoverableError` | `query_context_impl.cpp:100-102` | 读源码 |
| 参考在 `kTxnConflict` 上整条语句重试 | `do { QueryStatementInternal } while (... kTxnConflict)` | `query_context_impl.cpp:113-119` | 读源码 |
| 重写版可一请求多语句 | `for s in parse(sql)?` | `rust_engine/crates/infinity-executor/src/lib.rs:6415` | 读源码 |
| 重写版 `TxnManager` 三入口在二进制里 | `begin` `0xf6ca60`、`begin_with_text` `0xf76430`、`commit` `0xf6cef0`（4,670 B） | ELF 查表 | 实测 |
| 重写版 `Engine::begin_txn` 无生产调用者 | 仅 `txn_ut.rs` / `replay_txn_ut.rs` 等测试文件调用 | `lib.rs:7251` + `grep` | 读源码；非 test 构建无调用点 |
| 重写版 DDL 窗口不开事务、不分配提交 ts | — | `FORMAT_EQUIVALENCE_TESTS.md` §6.6 | 已登记 |
| 重写版一次 `CREATE TABLE` 窗口内 `append_checkpoint` 2 次 | 调用者只有两处：`flush_storage` 与 `checkpoint_wal` | `lib.rs:14871` / `:15887` | 计数实测；**哪两次未确认** |
| 二进制规模 | `nm --defined-only` 605,247 / 161,958 行；`STT_FUNC` 且已定义、`st_size > 0` 的条目 488,339 / 127,812；去重地址 484,744 / 127,292 | 本机两二进制，未 strip | 实测 |
| `st_info` 是 bind 与 type 的合成字节 | `GLOBAL FUNC` = `0x12`；按字面比 `== STT_FUNC` 只会留下 LOCAL 函数 | — | 实测：会丢掉 37,295（参考）/ 57,277（重写版）个 `GLOBAL` 且 `st_size > 0` 的函数 |
| 挂载偏移换算 | `file_offset = st_value − p_vaddr + p_offset`，取 `st_value` 落在区间且 `p_flags` 含 `PF_X` 的 `PT_LOAD` | 两个二进制 `.text` 均为 `vaddr − offset = 0x1000`（参考 `0x244d140`/`0x244c140`，重写版 `0x8c1780`/`0x8c0780`） | 实测；直接拿 `nm` 首列当偏移会整体错位 0x1000 |
| mangled 名要带长度前缀 | `infinity_txn::TxnManager::commit` = `_ZN12infinity_txn10TxnManager6commit…`；`*TxnManager6commit*` 匹配不到，`*TxnManager*commit*` 可以 | `perf probe --funcs` 列 demangle 名 | 实测 |
| `symbol-missing` 只能由 ELF 查表产生 | 不能由 `grep`/glob 未命中产生 | — | 已有一次误判（`grep infinity_txn6TxnManager6commit`） |
| bpftrace 接受 mangled 名 | 0.27.0 可 `uprobe:/abs/path:"_ZN…"` | 宿主记录 | **未复核**：首次使用前用 `count == 1` 自检 |
| `ustack` 不作调用者栈来源 | 依赖帧指针，重写版不保证 `-C force-frame-pointers` | — | 读源码/工具约束 |

### 2.1 路径事实

这些是设计 probe 与 quiet 配置时要用到的结构事实，不是待实现的机制。

| 路径 | 事实 | 出处 |
|---|---|---|
| 参考算子执行 | 两条路径：调度（`kSelect`/`kExplain`/`kDelete`/`kUpdate`/`kCompact`/`kIndex` DDL → worker 线程）与内联（其余语句 → 连接线程，正处在语句窗口里） | `task_scheduler_impl.cpp:121-155`、`:180-198` |
| `RunTask` 是内联标记 | private，全仓唯一生产调用点是 `Schedule` 的非调度分支 | `task_scheduler.cppm:59`、`task_scheduler_impl.cpp:154` |
| `CompleteTask` 两种归属 | `WorkerLoop` 分支 `:263`/`:275`（worker 线程）与 `RunTask` 内联分支 `:189`（连接线程） | `task_scheduler_impl.cpp` |
| notifier 类名 | `Notifier`，不是 `FragmentNotifier`；`StartTask`/`SetError`/`FinishTask` 仅 `WorkerLoop` 分支 | `fragment_context.cppm:57`、`task_scheduler_impl.cpp:251/274/279` |
| 提交的 bottom 阶段 | bottom executor 线程：`bottom_executor_impl.cpp:51` → `new_txn_impl.cpp:4565` → `:4603`；恢复路径 `CommitReplay` 在 `:2151` | 读源码 |
| 未捕获异常 | `OnExecute` 抛出 `UnrecoverableException` 时 `uretprobe` 不触发，进程常经 terminate handler 走 `SIGABRT` | `fragment_task_impl.cpp:102-104`、`infinity_main.cpp` 的 `set_terminate` |

## 3 probe 契约

```
rust_engine/probes/<id>/
  question.md            问题、targets、expect、runs、语料
  ref_observations.json  参考侧观测（含 build id / sha256 / 命令 / 内核）
  rs_observations.json   重写版观测
  controls.json          手写期望，用来证明仪器本身没坏
  run.sh                 复现（两端同一脚本，参数区分）
  assert.md              断言：读两个观测文件，给 equal / different / unavailable
```

规则：

- **一个问题一个 probe**。不允许一个脚本顺带回答第二个问题；新问题新目录。`assert.md` 里出现 "顺便也看看" 就是把两个结论绑在一起。
- **运行前写死 `expect`**：`equal`、`different`、`unknown`。`unknown` 只用于观察型 probe，必须同时写 `expected_recoveries: 0`；`expect` 允许 `unknown`，但**结果状态**只有 `equal` / `different` / `unavailable` 三种，跑完再补预期等于改答案。
- **targets 是冻结清单**：每个 `(端, 函数名, 归一化名)` 三元组来自 §5 的账本；probe 只能取，不能就地新增。
- **观测文件必须带**：二进制 sha256、build id、语料 sha256、内核版本、bpftrace 版本、挂载方式（符号名 / 偏移）、运行次数。
- **三态判定**：观测缺失、条数不完整、任一侧进程被信号终止 ⇒ `unavailable`，不是 0。`unavailable` 的 probe 在报告里与 `different` 分列，不得合并。
- **`controls.json` 至少两条**：一条参考对参考跑两遍必须零差（仪器自证）；一条手写的已知答案（防 oracle 自己错）。手工预期本身会错——`BeginTxn` 曾被手工写成 1，实测是 0——controls 就是为这类错准备的。
- **不比较跨线程顺序**：没有全序可比。每次运行的观测里 `tid` 只用于分组计数，不用于排序断言。

## 4 相位分离

不做事后归属。要区分"语句路径上的调用"和"后台线程上的调用"，**跑两遍相减**，而不是在飞行期把事件归到某个窗口：

| 运行 | 配置 | 用途 |
|---|---|---|
| quiet | 关掉 cleanup / optimize / 周期 checkpoint 触发（或把间隔设到语料跑不完），只留语句路径 | 语句路径基线 |
| default | 正常配置 | 实际行为 |

`default − quiet` 就是后台贡献。这样不需要窗口 id、不需要上下文反查、不需要判断"这一条 `window = 0` 事件算归属失败还是结构性窗口外"——那对判据从同一份原始数据里长出来是一个样，无法证伪。

约束：

- quiet 配置必须**真的关得掉**，并在 `question.md` 里写明关了哪几个开关、对应的配置项与默认值。关不掉的（例如提交的 bottom 阶段和语句在同一进程里必然启动）在 §7 登记为未覆盖，不写进断言。
- 两个运行必须同二进制、同语料、同数据目录重置方式；只差配置。
- 两侧的 quiet 配置要**同义**：参考的 cleanup 间隔与重写版的周期触发是不同的实现，probe 只需保证"两侧都关掉同类后台"，不要求配置项同名。

## 5 目标账本

`rust_engine/probes/targets.yaml`：参与比较的 `(side, symbol, normalized, module)` 全集，带 sha256，随代码 review。

- probe 的 targets 必须全部来自账本；**账本里缺失或未运行的目标算失败，不记 0**——"没跑"和"跑出 0 次"必须能分开。
- 账本本身是**闭集**：新增目标要写清"为什么这个函数能回答某个问题"，不允许为凑覆盖率批量加。
- 目标集合与语料**解耦**：删语料不会让账本变小，也不会让分母变小。
- 归一化只做机械变换：去模板实参、去 `@模块名` 后缀、去 Rust 的 `17h<16 hex>`、闭包折叠到父函数但保留 `closure(#k)` 序号。**实例签名单独保留**：参考 1 个实例调 3 次与重写版 3 个实例各调 1 次，折叠后总数都是 3，分布不同必须能看见。

## 6 与已有门禁的边界

| 门禁 | 回答什么 | 不回答什么 |
|---|---|---|
| 行为门禁（SQL 结果、错误码、跨引擎） | 相同输入是否产出相同结果 | 结果相同是否走过相同路径 |
| 字节门禁（落盘格式逐字节） | 落盘是否等价（ts 等易变量归一化） | 归一的那些列是否真的等价；不落盘的路径 |
| 本文的 probe | 指定函数的参与性与频次 | 全量覆盖；跨线程顺序；未登记的函数 |

probe **不重算**结果差异，也不替代字节比对。它只补一格：**结果一样、落盘一样、但路径不一样**。反之，probe 判 `equal` 也**不**说明两个引擎等价，只说明被问的那一个问题在被用的那份语料上一致。

## 7 未覆盖项

| 项 | 为什么没覆盖 | 会漏掉什么 |
|---|---|---|
| 全函数参与性（484k / 127k） | ~950 + ~250 批，每批一次启停；成本与结论不成比例 | 未登记函数的"少调一次" |
| 跨线程调用顺序 | 没有全序；`ts_ns` 只是墙钟 | 并发路径上的次序差异 |
| 参数与返回值（全量） | DWARF 选型 + sret 读返回槽，每个函数一套；只在 probe 明确需要时做 | 次数相同、参数不同的差异 |
| 后台调用归属 | 用 §4 相位分离；关不掉的线程无法归因 | 与语句同进程必然启动的后台调用 |
| 启动期与首个连接的相对时序 | 监听早于 `InitPhase2`（`:273` vs `:285`），未测 | `default_db` 建库与首条语句的交界 |
| 重写版多语句窗口 | 参考只接受单语句，没有可配对的对端 | `multi` 窗口的内部路径 |
| `GetWriteCommitTS` 的等价采集点 | 参考有该符号，重写版无同名；推断是 `TxnManager::commit` 的返回值（sret），未用 ts 序列验证 | 提交时间戳的分配次数 |

## 8 probe 清单

| id | 问题 | 状态 |
|---|---|---|
| `ddl-txn-participation` | 参考一条 `CREATE TABLE` 窗口里是否开事务、分配提交 ts？那 2 次 `BeginTxnShared` 与 1 次 `CommitBottom` 分别落在哪条语句上？ | 未实现（§2 的前六行是它的输入） |
| `checkpoint-append-count` | 重写版一次 `CREATE TABLE` 窗口内 `append_checkpoint` 的 2 次分别来自 `flush_storage` 还是 `checkpoint_wal`？参考的对应次数是多少？ | 未实现；计数侧已实测（§2） |
| `drop-table-participation` | `DROP TABLE` 是否与 `CREATE TABLE` 一样不开事务？ | 未实现 |
| `scheduler-path-completetask` | 同一条语句，参考内联 1 / `WorkerLoop` 0，重写版内联 1 / `WorkerLoop` 1——调度路径多出来的那次是否稳定复现？ | 未实现；单次手工观测 |

每个 probe 的 `question.md` 要能独立读懂：问题、targets、expect、quiet 配置、语料、判定规则、未覆盖的邻近问题。

## 9 复现环境

| 项 | 参考 | 重写版 |
|---|---|---|
| 二进制 | `cmake-build-debug/src/infinity`（838 MB，`-O0 -g`，未 strip） | `rust_engine/target/debug/infinity`（267 MB，`-O0 -g`，未 strip） |
| 构建要求 | 调试信息（DWARF 选参数）、`.text` 唯一可执行 `PT_LOAD` 的假设不成立时按段选 | 同上 |
| 观测 | uprobe/uretprobe，需 root；`perf probe --funcs` 可交叉校验（符号数随构建变化，以当次 `perf probe --funcs` 为准） | 同左 |

运行约定：每次运行前记录两个二进制的 sha256 与 build id；每次重置数据目录；结果放 `target/call_probe/<id>/<run-id>/`（不进 git），`run-id` 由二进制 sha256 + 语料 sha256 + 配置摘要派生；重跑用新目录，不覆盖旧结果。挂载前用一个已知函数做 `count == 1` 自检。
