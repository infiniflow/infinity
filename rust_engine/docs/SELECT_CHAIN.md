# SELECT 链路的耗时分布（参考 vs 端口）

> 测量口径：同一台机、同一个 SDK（`.venv/bin/python`）、同一份数据（40,000 行 `INT + VARCHAR`，插入后 `flush_data()` 落到块文件）。
> 端到端用客户端 `perf_counter`（20 次，取中位数，重复 3 轮）；端口内部用临时打点（已移除）；参考侧的热点用 `perf record -F 999 -g` 采样占比。
> 机器上还有 IDE/浏览器（load ≈ 4），绝对值有几成抖动，**同一轮里的引擎间横向比才有效**；`/proc/<pid>/stat` 的进程 CPU 含后台线程（compaction / checkpoint），不能当 per-query 服务端时间，下面不用它。

## 1 端到端

| 查询（40,000 行） | 参考 wall | 端口 wall | 倍数 | 响应载荷 |
|---|---|---|---|---|
| `SELECT id` | 1.85 ms | 6.58 ms | 3.6x | 156 KiB（两边逐字节同大） |
| `SELECT id, name` | 14.31 ms | 26.41 ms | 1.8x | 653 KiB（同上） |
| `WHERE id = 39999` | 0.93–1.66 ms | 1.74–2.03 ms | ~2x | ≈0 |

协议层已经对齐：两条查询的响应字节数完全相同（156 / 653 KiB），客户端解码成本也相同（40,000 个 varchar 单元两边都要 ≈11–12 ms）。差距全部在服务端。

## 2 端口的分段耗时（临时打点）

| 阶段 | `SELECT id` | `SELECT id, name` |
|---|---|---|
| 解析 / 绑定 / 优化 + 调度（`exec` 减下面几项） | 0.74 ms | 0.59 ms |
| 读块 + 解码（`read_cpp_blocks_values`） | 2.12 ms | 7.85 ms |
| **结果渲染成文本**（`into_slt_string`） | **1.51 ms** | **1.48 ms** |
| 打包成 thrift 列块（`rows_to_column_fields_typed`） | 0.42 ms | 0.52 ms |
| 服务端合计 | 4.8 ms | 10.4 ms |
| 客户端 + 传输（wall − 服务端打点） | ~1.8 ms | ~16 ms |

`SELECT id` 的服务端时间里，**有 1.9 ms（约 40%）是「值 → 文本 → 二进制」的往返**：执行器把每行渲染成 `String`，协议层再把每个 `String` `parse::<i32>()` 回数字打包。

## 3 参考的耗时归属（perf 采样）

`SELECT id` 的采样分布（`perf report`，去掉重复的 atom/core 变体）：

| 占比 | 符号 | 属于 |
|---|---|---|
| ~60% | `pthread_rwlock_rdlock` / `rwlock_unlock` | 块元数据锁（每块进出一次） |
| ~22% | `BlockVersion::CheckDelete(int, unsigned long)` | 逐行可见性判断 |
| ~7% | `ColumnVector::AppendWith(ColumnVector const&, …)` | 把块的列缓冲拷进结果 `DataBlock` |
| ~5% | `ColumnVector::Initialize(...)` | 结果列缓冲初始化 |

参考侧**没有「解码」这一步**：块文件里的列缓冲本身就是定长 i32 数组，`ColumnVector` 直接在其上做可见性判断并整体拷贝；渲染只发生在协议层，且是 `memcpy` 到 thrift 的 `binary`。
## 4 架构对比：差在哪一层

| 层 | 参考（C++） | 端口（Rust） | 后果 |
|---|---|---|---|
| 扫描产出 | `ColumnVector`：列式、连续、类型化（`i32` 缓冲 + null 位图） | `Vec<Vec<Value>>`：行式，每个 cell 一个 `Value` 枚举，每行一次 `Vec` 分配 | 40,000 行 → 40,000+ 次分配；perf 里 ~55% 在 malloc/free |
| 解码 | 块文件 buffer 直接用（`ColumnVector::Initialize`），逐行只做 `CheckDelete` | `decode_rows_of_values` 逐 cell 构造 `Value`（标量直接读 slot，varchar 走 outline + `parse_owned_value`） | 端口 decode 2.1 ms（`id`）/ 7.9 ms（含 varchar）；参考整体 1.5 ms |
| 谓词 | 扫描内对列向量求值；块级 prefilter 只在段封存后存在 | `Expr::eval_predicate` 逐行解释；等值谓词在行缓冲构造时执行；只解谓词列 ∪ 投影列 | 点查端口 2.0 ms vs 参考 4.5 ms |
| 结果表示 | `DataBlock`（列向量）一路到协议层 | `QueryResult::Rows { rows: Vec<Vec<String>> }` —— 出口把值全渲染成文本（`lib.rs` 的 `into_slt_string`） | `SELECT id` 多出 1.5 ms |
| 协议打包 | `ProcessColumns` → `HandlePodType`：列缓冲 `memcpy` 进 `ColumnField.column_vectors` | `rows_to_column_fields_typed`：每个 cell 的文本再 `parse::<i32>()` / 重新编码 | 多出 0.4 ms（`id`），varchar 列还要再走一遍 |
| 传输/客户端 | thrift binary，一列一个 `binary` 块 | 同左（字节数实测相同） | 无差异 |

结论：**端口比参考多两次「值 ↔ 文本」转换**（执行器出口渲染、协议入口解析），而且扫描与结果都是行式的。参考从扫描到 wire 全程是列缓冲。

## 5 已修：结果不再经过文本（`2191fa03e`）

`QueryResult::Rows` 原先存 `Vec<Vec<String>>`：执行器在出口把每个 cell 渲染成显示文本，协议层再把它解析回要发送的二进制列块。现在存 `Vec<Vec<Value>>`：

- `rows_text()` 按需渲染，只给真正要看文本的地方（CLI、`.slt` runner、HTTP/postgres writer、单测的 `rows()` helper）；
- thrift 打包直接从 `Value` 取数（`cell_int`/`cell_float`/`cell_bool`），只有本来只有文本形态的列（varchar/json/date/array/tensor/sparse）才走 `cell_text`；
- 无声明类型的表达式列（`SELECT 1+1`）改为看值推断列类型，不再解析文本。

服务端每查询（40,000 行）：`SELECT id` **4.79 → 3.77 ms**（exec 4.37 → 2.92，渲染那 1.5 ms 没了），`SELECT id, name` **10.44 → 9.23 ms**。响应字节数不变（156 / 653 KiB），客户端解码成本不变。

## 6 仍未对齐：`Value` 行缓冲 vs 列缓冲

剩下的差距不在边界，而在扫描的产出形式：

- 参考的扫描产出 `ColumnVector`：定长、连续、类型化的缓冲（`i32` 数组 + null 位图），谓词按列求值，协议层整块 `memcpy`；
- 端口产出 `Vec<Vec<Value>>`：每个 cell 一个 24 字节的枚举，每行一次 `Vec` 分配。`SELECT id`（40,000 行）的服务端 3.77 ms 里，解码 2.1 ms、打包 0.85 ms，perf 里约一半在 malloc/free。

这是端口有意的取舍：[`chunk.rs`](../crates/infinity-executor/src/chunk.rs) 记录过——表达式一律在 `Value` 上求值，复用磁盘上的 `BlockValue` 列缓冲会在每个算子边界引入逐 cell 转换，正是算子模型要消除的东西。要再往前一步，得让结果与扫描都改成**类型化列缓冲**（`Vec<i32>` 这类），并把表达式求值改成按列；那是把 `Value` 模型换成 `DataBlock` 模型，属于另一层次的改造，不是 SELECT 链路的接线问题。

### 数字（本轮结束）

| 查询（40,000 行，端到端 wall） | 参考 | 端口 | 倍数 |
|---|---|---|---|
| `SELECT id` | 2.1–2.4 ms | 5.3–5.5 ms | 2.3x |
| `SELECT id, name` | 16.2–16.7 ms | 25.1–26.6 ms | 1.6x |
| `WHERE id = 39999` | 1.3–2.8 ms | 2.1 ms | ~1x |

服务端分段（端口，40,000 行）：

| 阶段 | `SELECT id` | `SELECT id, name` |
|---|---|---|
| 解析/绑定/优化/调度 | 0.7 ms | 0.6 ms |
| 读块 + 解码 | 2.1 ms | 7.9 ms |
| 打包 thrift 列块 | 0.85 ms | 1.08 ms |
| 服务端合计 | 3.77 ms | 9.23 ms |
