# 调用探针审计（函数级染色差分）

审计单元不是"类是否存在"，而是"一条语句的执行窗口里，每个函数被调用了多少次、带走什么参数、由谁调用"。

`CLASS_MAP.md` / `CLASS_AUDIT.md` 回答存在性；本文的机制回答参与性、粒度、顺序与条件。两者配合：类图提供**聚合键与名称字典**，判决来自**计数与参数**。

## 1 类图覆盖不到的那一维

`TxnManager → infinity_txn::TxnManager` 在类图里是 ✅，它只说明承担物存在。实测的另一面是：

- `CREATE TABLE` 语句在重写版里从不经过 `begin_txn`，因此不消耗时间戳；
- 参考版在同一条语句里消耗一个提交时间戳（`Wallet` 里 `BeginTxn → Commit` 各一次）。

类存在、参与次数为 0，这类差异在类图里没有任何位置可以落地，在字节门禁里又被"ts 列是易变量"的归一化掩掉。类图的判定单元是 (C++ 类, Rust 承担物)，天然只有"存在"这一个维度。

## 2 机制

比对单位：

```
(语句窗口, 归一化函数名) -> 调用次数, 参数元组, 调用者栈签名
```

三条性质决定了它可以机械化：

- **探针表由符号表生成**，不需要事先知道"事务""刷盘"这类关注点。任何一层（catalog、storage、buffer、index、executor）都自动被染色。
- **计数是多重集**，与线程交错无关。并行只影响顺序，不影响每条语句下的调用次数，因此差分在并发下稳定。
- **差分自动排序**，人只看 top-k，不需要逐个函数读代码。

## 3 探针表生成

实测（本机，两个二进制均为 `-O0`、`with debug_info, not stripped`）：

| 项 | 参考 | 重写版 |
|---|---|---|
| 二进制 | `cmake-build-debug/src/infinity`（838 MB） | `rust_engine/target/debug/infinity`（267 MB） |
| `nm --defined-only` 行数 | 605,247 | 161,958 |
| 项目命名空间下 | 32,001（`infinity::` / `infinity_*::`） | 5,103（`infinity_{txn,executor,storage,catalog,common,expression,planner,parser,index,server}::`） |

**归一化规则**（纯字符串处理，两端同一套）：

| 形态 | 归一化后 |
|---|---|
| `infinity::NewTxnManager::BeginTxn(unsigned long, ...)` | `infinity::NewTxnManager::BeginTxn` |
| `infinity::NewTxnManager@infinity_core::BeginTxn(...)`（demangler 带模块名） | `infinity::NewTxnManager::BeginTxn` |
| 模板实参 `Foo<Bar, Baz>::Run` | `Foo::Run` |
| `_ZN12infinity_txn10TxnManager17append_checkpoint17h330fd0380f4886b3E` | `infinity_txn::TxnManager::append_checkpoint` |
| `...17append_checkpoint28_$u7b$$u7b$closure$u7d$$u7d$17h...E` | `infinity_txn::TxnManager::append_checkpoint`（折叠回父函数） |
| 泛型单态化的多份实例 | 折叠为一个键（计数相加） |

**挂载方式**：按文件偏移 `uprobe:/abs/path:0x<offset>`，不用符号名解析。理由是实测 `perf probe --funcs`（DWARF 函数枚举）在这两个二进制上不可用：C++ 侧 5 分钟未跑完，Rust 侧直接返回空。偏移可以从 `nm` 的第一列直接取。

## 4 窗口与并发

- 在语句入口函数进入时写 `@win[tid] = <语句序号>`，同一线程上的所有函数探针只做 `@cnt[@win[tid], <fid>] = count()`。
- 一条语句结束时取出该窗口的向量，重置 `@win[tid]`。
- 语句逐条执行，窗口即语句序号；一个连接一个线程，`tid` 足以隔离并发语句。
- 差分只比多重集：**只在参考侧出现**（0 vs N）判为参与性缺口；**计数比值异常**（1 vs 3、1 vs 0.33）判为粒度或循环层级缺口。

## 5 内容、返回值与输出参数

`write(2)` 边界不可用：它只携带一个指针和长度，看不到语义内容，也无法说明为什么要写。内容要在**持有 buffer 的那一层**取：

| 目标 | 取法 |
|---|---|
| 写入内容 | 在 `Slice` / `MemBuffer` / `WriteBuffer` 的持有者函数上取指针参数，解一层得到长度与前若干字节 |
| 返回值 | `uretprobe` 取返回寄存器；Rust `Result<T, E>` 按 DWARF 布局解判别式与 payload |
| 输出参数（状态被改写） | 返回后对入参指针解引用，与进入时的值做差 |
| 调用原因 | 每个探针采 `ustack`，按调用者链做签名多重集；对计数为 0 的函数，用与参考侧最近的共同祖先解释 |

## 6 已知限制

- **优化级必须一致**。双方都用 `-O0`；debug 与 release 的调用次数不可比。
- **按值传结构体**在 Rust ABI 下可能走间接传递（sret / by-ref），`$argN` 不可靠。只对"标量 / 指针 / 引用"参数的函数做参数级比对；DWARF 里有类型信息，可机械判定。
- **泛型与闭包**必须折叠，否则计数天然不等。
- **计数相同、内容不同**的情况计数抓不到；这是 top-k 参数比对与既有字节门禁的职责，不是本文机制的职责。
- **探针开销**会轻微改变时序，但多重集对时序不敏感。规模上 32k 个探针一次挂载不可行，按命名空间子树分批扫。

## 7 环境

实测：

| 项 | 结果 |
|---|---|
| 宿主 `sudo` | 免密可用 |
| tracefs | `/sys/kernel/tracing` 已挂载，root 可写 `uprobe_events` |
| `perf` / `strace` / `readelf` / `nm` | 均存在 |
| `bpftrace` | openSUSE `repo-oss` 有 0.27.0，未安装 |
| `perf probe --funcs` | 不可用（见 §3） |
| `infinity_builder` 容器 | unprivileged、无追踪工具、tracefs 为空，不使用 |

## 8 待标定

用已知答案验证机制：`CREATE TABLE` 窗口下，参考侧 `NewTxnManager::{BeginTxn, BeginTxnShared, Commit}` 计数应大于 0，重写侧 `TxnManager::*` 应为 0。机制若能自动复现这个手工发现的缺口，才进入全量扫描。

标定结果补充在本节。
