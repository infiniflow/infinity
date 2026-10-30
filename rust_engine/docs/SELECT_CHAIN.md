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

## 5 已修：整条链路不再有行式中间层

两步都落在 `2191fa03e` 与 `a0ee75059`：

1. **结果保持类型化**（`2191fa03e`）：`QueryResult::Rows` 从 `Vec<Vec<String>>` 改成 `Vec<Vec<Value>>`，协议层直接按 `Value` 打包；文本只在 CLI / `.slt` / HTTP / postgres 出口用 `rows_text()` 渲染。原先每 cell 一次渲染（1.5 ms）+ 一次重新解析（0.4 ms）没有了。
2. **纯列投影走列式缓冲**（`a0ee75059`）：`SELECT <列> FROM <表>`（无谓词/连接/表达式/星号）由 `Engine::select_columns` 回答：`decode_block_column` 按**列**解码进 `ColumnData`（`Int`/`Float`/`Bool`/`Text`）+ 每列有效位图，缓冲区里的行追加进同一批缓冲，结果是一个 `Batch`；thrift 侧 `column_fields_from_batch` 每列拼一个连续 chunk，不再逐 cell 走 `Value`。

这条快速路径是窄的：带谓词、连接、表达式、`SELECT *`、`total_hits`，或列是 embedding/tensor/sparse/array/multivector 时一律回落到行式路径（那些编码有各自的 wire 形式）。

## 6 现在的数字

| 查询（40,000 行，端到端 wall） | 参考 | 端口 | 倍数 |
|---|---|---|---|
| `SELECT id` | 2.94 ms | **1.79 ms** | **0.6x（更快）** |
| `SELECT id, name` | 15.66 ms | 22.36 ms | 1.43x |
| `WHERE id = 39999` | 3.06 ms | 1.93 ms | 0.6x（更快） |

服务端分段（端口）：

| 阶段 | `SELECT id` | `SELECT id, name` |
|---|---|---|
| 列式读块 + 解码（`read_columns_batch`） | 0.9 ms | 8.1 ms |
| 打包 thrift 列块（`column_fields_from_batch`） | 0.1 ms | 0.2 ms |
| 服务端合计 | ~1.0 ms | ~8.3 ms |

`SELECT id` 已经快过参考：参考仍要逐行 `CheckDelete` 再过一遍 `ColumnVector::AppendWith`，端口把可见性和解码合成一次列扫描。

## 7 还剩什么

`SELECT id, name` 的 8.1 ms 全在 varchar 列：cell 只有文本形态（16 字节 slot + outline 查表），每 cell 还要一次 `String` 分配。参考读的是块的 varchar 列缓冲（偏移 + 字节），`memcpy` 进 wire。要把这 5.7 ms 也拿掉，得把文本列也按「一段字节 + 偏移」存进 `ColumnData`，并把 wire 打包改成切偏移——这是下一步，不涉及上层接口。
