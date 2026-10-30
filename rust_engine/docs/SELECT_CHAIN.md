# SELECT 链路的耗时分布（参考 vs 端口）

> 测量口径：同一台机、同一个 SDK（`.venv/bin/python`）、同一份数据（40,000 行 `INT + VARCHAR`，插入后 `flush_data()` 落到块文件）。
> 客户端耗时用 `perf_counter` 取 20 次均值；服务端 CPU 取 `/proc/<pid>/stat` 的 utime+stime 差值；热点用 `perf record -F 999 -g` 采样。
> 机器上还有 IDE/浏览器在跑（load ≈ 4），绝对值有几成抖动，**同一轮里的引擎间横向比才有效**。

## 1 端到端

| 查询（40,000 行） | 参考 wall | 参考 服务端CPU | 端口 wall | 端口 服务端CPU | 响应载荷 |
|---|---|---|---|---|---|
| `SELECT id` | 1.85 ms | 1.50 ms | 6.58 ms | 5.00 ms | 156 KiB（两边相同） |
| `SELECT id, name` | 14.31 ms | 3.00 ms | 26.41 ms | 14.00 ms | 653 KiB（两边相同） |
| `WHERE id = 39999` | 2.05 ms | 4.50 ms | 1.99 ms | 2.00 ms | ≈0 |

两点先摆明：

- **协议层已经对齐**：两条查询的响应字节数完全相同（156 / 653 KiB），客户端解码成本也相同（40,000 个 varchar 单元两边都要 ≈11–12 ms）。差距全部在服务端。
- **点查端口更快**：参考对未封存段没有块级 prefilter（`blk|…|fast_rough_filter` 只在段封存时写），要把两列都解出来再过谓词；端口只解 `id` 一列，等值谓词在行缓冲上就地判断。

## 2 端口的分段耗时

临时打点（已移除）测得的 `SELECT` 内部：

| 阶段 | `SELECT id` | `SELECT id, name` |
|---|---|---|
| 解析 / 绑定 / 优化 + 调度（`exec` 减下面几项） | 0.74 ms | 0.59 ms |
| 读块 + 解码（`read_cpp_blocks_values`） | 2.12 ms | 7.85 ms |
| **结果渲染成文本**（`into_slt_string`） | **1.51 ms** | **1.48 ms** |
| 打包成 thrift 列块（`rows_to_column_fields_typed`） | 0.42 ms | 0.52 ms |
| 合计（服务端 CPU） | 5.00 ms | 14.00 ms |

`SELECT id` 的服务端时间里，**有 1.9 ms（约 38%）是「值 → 文本 → 二进制」的往返**：执行器把每行渲染成 `String`，协议层再把每个 `String` `parse::<i32>()` 回数字打包。

## 3 参考的分段耗时（perf 归属）

`SELECT id` 的采样分布（`perf report`，去掉重复的 atom/core 变体）：

| 占比 | 符号 | 属于 |
|---|---|---|
| ~60% | `pthread_rwlock_rdlock` / `rwlock_unlock` | 块元数据锁（每块进出一次） |
| ~22% | `BlockVersion::CheckDelete(int, unsigned long)` | 逐行可见性判断 |
| ~7% | `ColumnVector::AppendWith(ColumnVector const&, …)` | 把块的列缓冲拷进结果 `DataBlock` |
| ~5% | `ColumnVector::Initialize(...)` | 结果列缓冲初始化 |

也就是说参考侧**没有「解码」这一步**：块文件里的列缓冲本身就是定长 i32 数组，`ColumnVector` 直接在其上做可见性判断并整体拷贝；渲染只发生在协议层，且是 `memcpy` 到 thrift 的 `binary`。

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

## 5 修复路径（下一步）

1. `QueryResult::Rows` 携带列式数据（`Vec<Value>` 的列向量或新的 `Batch` 变体），文本渲染只在 CLI / `.slt` 出口做；
2. 协议层从列缓冲直接打包（一列一个 `binary` 块，标量 `memcpy`），删掉逐 cell `parse`；
3. 扫描直接产出列缓冲，`direct_projection` 不再逐行分配 `Vec<Value>`。

按第 2 条之外的两条一起做的估计：`SELECT id` 服务端 5.0 → ~1.5 ms（与参考持平），`SELECT id, name` 14 → ~4 ms（剩下的 varchar 解析是格式本身的要求）。