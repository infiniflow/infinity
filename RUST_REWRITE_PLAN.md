# Infinity Rust 重写计划书

> 版本：v0.3（草案，已并入 codex 第 1、2、3 轮评审意见）  
> 日期：2026-10-15  
> 基准代码：infinity `main` @ `32a6afce8`（v0.7.4，nightly tag）  
> 作者：CodeBuddy（owner）  
> 评审：codex（reviewer，独立评审）  
> 状态：✅ **计划已达成一致**（codex round 3：`VERDICT: APPROVE`）。**附录 B 的 PoC 已全部执行**：A/B/C/D/E/H **通过**，F/G **部分通过**（发现 FST 校验和与分词等价性两处关键缺陷，见附录 B）。

---

## 0. 摘要（Executive Summary）

本计划书规划将 Infinity 数据库引擎从 C++23（**引擎约 38.3 万行 / 1,689 个源文件**，其中含 8.61 万行 `.cppm` 模块实现；另有单元测试约 12.8 万行）整体重写为 Rust，目标收益为：内存安全、无数据竞争（`Send`/`Sync` + `unsafe` 收敛）、更低的并发复杂度、更快的迭代与更强的工程可维护性。

三个已明确的技术方向：

1. **并发模型**：以 **Tokio 多线程运行时** 替代现有「固定全局线程池 + 逐 CPU 绑核 + 私有阻塞队列」的 `TaskScheduler`，将查询调度改造为「async 编排 + CPU 计算下沉 Rayon」的两层模型（见 §3 D4、§4.3）。
2. **KV 存储（原 RocksDB）**：RocksDB 在 Infinity 中只承担**元数据**（catalog / 计数器 / 对象清单 / rough filter / drop 墓碑），不承载行数据与索引。因此**纯 Rust 替代完全可行**。推荐抽象 `MetaStore` trait，默认采用纯 Rust LSM 引擎 **`fjall`**，并保留 `rust-rocksdb` 适配器作为回退（见 §3 D2，含完整选型矩阵）。
3. **验收标准**：以现有测试资产为准 —— **173 个 SQL Logic Test（sqllogictest）**、**Python PySDK（313 个 `def test_`，308 有效）**、**Go SDK 375 用例**，外加持久化 / 并发 / 性能等补充关卡（见 §6）；**验收以冻结后的收集清单为准**（见 §2.7 注 2、§6）。由于这些测试**经由网络协议连接真实服务**，**Thrift 线协议必须保持兼容**（`TBinaryProtocol` + `client_version == 37`），未修改的 Python/Go SDK 是硬性验收面。

推荐采用「**洁净室重写 + 持续一致性测试（conformance testing）**」的迁移策略：**新旧二进制并存**，以 C++ 引擎为参考实现（oracle）做差分测试，按垂直切片逐步点亮测试子集（见 §3 D1、§5）。

预计工作量与团队规模见 §8。

---

## 1. 背景与目标

### 1.1 背景

Infinity 是面向 LLM 应用的 AI-native 数据库，提供稠密向量、稀疏向量、张量（多向量）与全文检索的混合搜索。核心以 C++23（Clang 20+）实现，配套 Python / Go / C++ SDK。经过 v0.7.x 的迭代，引擎已具备完整能力，但也积累了以下工程负担：

- **并发模型复杂**：自研线程池、绑核、私有阻塞队列、`mutex`+`condvar` 同步、fragment 树隐式依赖，难以形式化验证，TSAN 用例是常态化的排查成本。
- **内存安全跨层**：53 处 `std::thread` 分布在 WAL / 后台任务 / 索引构建 / 网络等，生命周期与并发正确性靠人工保证。
- **构建与依赖沉重**：vcpkg + CMake + Clang 20 + Bison/Flex，跨平台（x64-v2/v3/v4、arm64）构建链路长。

### 1.2 目标（Goals）

- **G1 内存安全**：核心路径零 `unsafe`（`unsafe` 收敛到 FFI / SIMD 内核等少数模块并集中审计）。
- **G2 并发简化**：用 async/await + 结构化并发替代手工线程与条件变量，消除数据竞争类缺陷。
- **G3 行为等价**：在验收测试上达到与 C++ 引擎**功能等价**（见 §6），线协议兼容。
- **G4 可维护性**：单一 Cargo workspace，模块边界清晰，编译期强制接口契约。
- **G5 性能不退化**：关键基准（sift KNN 查询、全文检索）达到现有 golden log 的水平（允许阈值，见 §6 Gate E）。

### 1.3 非目标（Non-Goals）

- ~~不追求与旧版本**磁盘格式**兼容~~ → **已于 2026-10-16 收紧为「必须兼容」**（全文倒排、向量列存储、向量索引、Catalog KV 均须与 C++ 磁盘格式互通，见 §2.6.1；旧表述见 D2 已被取代）。
- 不在首版重写集群/分布式能力（现有 peer/cluster 为实验性、默认关闭，见 §2.5，建议 M5 之后再评估）。
- 不重写 SDK 的对外 API 形态（保持在 SDK 侧尽量不变，SQL/Python/Go 语义不变）。

---

## 2. 现状评估（As-Is）

### 2.1 规模（不含单元测试）

> 统计口径：`src/**` 下 `.cpp/.h/.hpp`（实现 + 头文件）与 `.cppm`（C++ 模块，**同样含实现代码**）合并计数，`src/unit_test` 单列。`.cppm` 由 CMake 编译（`src/CMakeLists.txt:30`），**不可忽略**。`src/parser` 含 Bison/Flex 生成代码（`parser.cpp`/`lexer.cpp` 等）。

| 模块 | 合计行数 | 其中 `.cppm` | 说明 |
|---|---:|---:|---|
| `src/storage` | 119,194 | 47,852 | 存储引擎：KV / WAL / buffer / catalog / txn / 索引 |
| `src/network` | 70,753 | 1,760 | Thrift / HTTP / PG / peer 协议 |
| `src/parser` | 47,599 | 2,086 | Bison/Flex 语法 + AST（**含生成代码**） |
| `src/common` | 37,816 | 8,391 | 基础设施 / 工具 / 分析器 |
| `src/executor` | 36,781 | 6,921 | 物理算子、fragment 构建 |
| `src/planner` | 29,428 | 6,492 | 逻辑计划、绑定、优化器 |
| `src/function` | 19,202 | 7,538 | 标量 / 聚合函数 |
| `src/main` | 9,826 | 2,006 | 引擎门面 / 配置 / 集群管理 |
| `src/admin` | 4,690 | 67 | 管理接口 |
| `src/expression` | 3,714 | 1,686 | 表达式 AST |
| `src/scheduler` | 2,547 | 534 | 任务调度器 |
| `src/bin` / `src/metrics` | 1,417 | 734 | 入口 / 指标 |
| **引擎合计（不含 UT）** | **382,967** | **86,067** | **1,689 个源文件** |
| `src/unit_test` | 128,345 | 530 | GTest 单元测试（约 1032 个用例） |

> 计数方法：以换行符计数（`xargs cat | wc -l`），`.cppm` 子列**不含** UT（UT 的 530 行已单列，不应计入引擎 `.cppm`）。

### 2.2 三大持久化底座（关键认知）

现有存储层有**三个彼此独立**的持久化介质，重写时**绝不能混为一谈**：

| 底座 | 存储内容 | 位置 | Rust 对应 |
|---|---|---|---|
| **RocksDB（事务 KV）** | Catalog/元数据、全局 ID 计数器、对象存储文件清单、fast-rough-filter、drop 墓碑 | `storage/catalog/kv_store*`、`storage/catalog/meta/*` | `MetaStore` trait（默认 `fjall`，回退 `rust-rocksdb`） |
| **自研 WAL（追加日志）** | 所有已提交 DDL/DML 的 redo log（CRC32 分帧） | `storage/wal/*` | 自定义 `Wal` trait（全量移植格式） |
| **自研块文件（Buffer 管理）** | 列数据文件、MVCC `version` 文件、索引 chunk 文件（见下） | `storage/buffer/*`、`storage/catalog/meta/*_impl.cpp` | `BlockStore` / `BufferManager` |

要点：

- **RocksDB 自身 WAL 并非全局关闭**：默认写入路径 `disable_wal=true`（`kv_store.cppm` / `kv_store_impl.cpp:311`），但**对象清单持久化显式启用**（`KVStore::Put(key, value, false)` → `FlushWAL(false)`，见 `src/storage/catalog/kv_store_impl.cpp:443,450` 与调用点 `src/storage/persistence/persistence_manager_impl.cpp:141`）。重写须**分别保留 catalog 与对象持久化两条耐久性契约**。
- RocksDB **未使用列族（CF）**、**未显式配置比较器 / block cache**，仅用默认单 CF + 字节序比较 + 自定义 merge operator（`String2UInt64AddOperator`，用于计数器）。RocksDB API 仅出现在 4 个文件中，抽象**已经很干净**。
- **MVCC 不依赖 RocksDB 快照**，而是**时间戳数组**模型：`BlockVersion.created_`（`{create_ts,row_count}` 列表）与 `BlockVersion.deleted_`（`commit_ts` 数组）。更新 = 删除 + 追加。
- 磁盘布局：`db_<id>/tbl_<id>/seg_<id>/blk_<id>/`，列文件 `<col>.col` + 变长 `<col>.out`，版本文件 `version`。块容量 8192 行，段容量 8M 行。
- **索引文件布局并非统一单 `.idx`**：向量族使用 per-chunk `chunk_<id>.idx`；**全文倒排使用多个文件**（`.dic`/`.pos`/`.spill`/`.len`，见 `src/storage/invertedindex/index_defines.cppm:55` 与 dump 点 `memory_indexer_impl.cpp:491`）。共性仅是「内存构建 → 落盘 → KV 记录 `ChunkIndexMetaInfo`」，**每个索引的文件集合 / 发布 / 合并 / 清理 / 恢复契约需分别定义**（见 §3 D2、§7 R1）。

### 2.3 并发与调度模型（Tokio 替代的重点）

现状（`src/scheduler/task_scheduler_impl.cpp`）：

- **固定全局线程池**：`worker_count = min(hardware_concurrency, CPULimit)`，**每 worker 一个绑核 OS 线程**（偶数核优先 `ThreadUtil::pin`）。
- **每 worker 私有有界阻塞队列**（`BlockingQueue`：`mutex` + 两个 `condvar`），`FindLeastWorkloadWorker` 按原子 workload 计数做动态分配，**无 work-stealing**。
- **依赖即 fragment 树**：无显式 DAG 调度器。父 fragment 的所有子 fragment 完成后，通过 `unfinished_child_n_` 计数触发 `TryStartFragment` → `ScheduleFragment`。
- **完成屏障**：`Notifier`（`mutex` + `condvar` + 计数），`GetResult()` 阻塞等待。
- **交换模型**：**push-based 物化交换**。父子 fragment 仅通过有界阻塞队列传递 `FragmentData`（整块 `DataBlock`）；`kParallelStream` 的任务可通过 `QuitFromWorkerLoop` 让出后再被重新调度（软背压）。`BlockingQueue.allow_enqueue_` 用于下游满足后停止上游生产。
- **执行路径**：`Parser → LogicalPlanner → Optimizer → PhysicalPlanner → FragmentBuilder → FragmentContext::BuildTask → scheduler.Schedule → GetResult()`。
- **重要现状限制**：join、cross product、union/intersect/except、dummy scan 等算子在 `fragment_builder` 中被显式拒绝（`Not support ...`），即**并行路径实际可用能力**为：scans → filter/project → aggregate/hash → sort/top/limit → fusion/match，外加 DML/DDL。

### 2.4 数据与执行模型

- **自研列式格式（非 Arrow）**：`DataBlock`（`vector<shared_ptr<ColumnVector>>`）、`ColumnVector`（`kFlat/kConstant/kCompactBit/kHeterogeneous` 四种表示 + null bitmask）、`DataTable`（物化结果容器）。
- 算子间数据流为**整块 `DataBlock`**，无 volcano/iterator 模型。
- 表达式/函数分发依赖**模板元编程**（按 `ColumnVectorType` 展开），标量函数按 `(输入类型, 操作符)` 编译期特化，重载解析为基于代价的 `MatchFunctionCost`。

### 2.5 网络与协议

| 监听 | 类 | 默认端口 | 重要性 |
|---|---|---|---|
| **Thrift（主 SDK 协议）** | `PoolThriftServer` | 23817 | **关键**，Python/Go/C++ SDK 全走此协议 |
| HTTP/REST（oatpp） | `HTTPServer` | 23820 | **关键**，`infinity_http.py` + HTTP 测试 |
| PostgreSQL wire | `PGServer` | 5432 | 非关键（仅 Simple Query，无 SSL/认证/扩展协议） |
| Peer/集群 Thrift | `PoolPeerThriftServer` | 23851 | 实验性，默认 `standalone` 关闭 |

- **Thrift 契约**：`thrift/infinity.thrift`，`InfinityService` 约 46 个 RPC，`CommonResponse{error_code,error_msg,session_id}`，列式结果 `ColumnField{column_type, column_vectors, bitmasks}`。生成代码**已入库**，由 **Thrift 0.24.0** 生成；Go 运行时却 pin 在 `thrift v0.22.0`（需注意）。握手要求 `client_version == 37`。
- **HTTP**：全部路由手写在单个 4399 行文件；搜索用专门的 `HTTPSearch` JSON→表达式解析。
- **前端差异**：PG = SQL 文本→解析器；HTTP = JSON 类型化调用 + 内嵌 SQL 混合；Python/Go = **Thrift 类型化 AST（不走文本解析器）**，三条路径最终汇入同一逻辑/物理/执行流水线。

### 2.6 索引清单

> **实现状态**区分「已有可用代码」与「受支持功能」——两者不等价（例如 DiskANN 存在骨架但 populate 分支直接 `LOG_WARN("Not implemented yet")` 并返回成功，见 `src/storage/new_txn/new_txn_index_impl.cpp:1168`）。重写须按状态分别定里程碑与测试矩阵。

| 索引 | 类型 | 状态 | 实现要点 / 依赖 |
|---|---|---|---|
| HNSW / 稠密 KNN | `kHnsw` | 已实现 | 自研；plain/LVQ/Rabitq 编码；MLAS GEMM |
| IVF (flat/PQ/SQ) | `kIVF` | 已实现 | 自研；kmeans；MLAS |
| DiskANN (Vamana) | `kDiskAnn` | **未实现（TODO）** | 已有部分代码与内存结构；populate 分支未接通（`new_txn_index_impl.cpp:1168`） |
| EMVB / PLAID / SMVE | 多向量 | 部分 | Eigen SVD + MLAS |
| 全文 / 倒排 | `kFullText` | 已实现 | 自研（**源自 iresearch**）：PForDelta、BM25/WAND、cppjieba/opencc 分词、re2；**词典 FST 为 `BurntSushi/fst` 的 C++ 移植——PoC F 证实「数据路径兼容、校验和不兼容」，须条件复用（见附录 B）** |
| 二级索引 | `kSecondary` | 已实现 | **PGM-index**（third_party/pgm）+ 低基数变体 + 函数索引 |
| JSON | secondary 变体 | 已实现 | `JsonFlattener` + nlohmann |
| 稀疏 / BMP | `kBMP` | 已实现 | 自研 block-max inverted |

> 待办：在开工前产出一份**「索引 × 操作（create/insert/optimize/compact/dump/restore）× 状态」**矩阵，作为 M3 的验收依据。

### 2.6.1 磁盘格式兼容要求（2026-10-16 收紧，取代 §3 D2 的「不追求磁盘兼容」）

**要求（用户指令）**：本次移植**必须兼容 C++ 的磁盘格式**，范围明确覆盖：**①全文倒排索引**（含词典 FST / postings / block-max / column-length 布局）、**②向量列的存储**（列/段文件）、**③向量索引**（HNSW / IVF / EMVB-PLAID 等的索引文件）。任何**格式不同**的第三方库（即使能力等价）**一律排除**（Tantivy 即因此排除，见附录 B）。

**判定规则**：能否被 C++ 侧按原路径打开并读回（`DESCRIBE/SEARCH/SHOW` 结果一致），而不是「能力等价」。

**据此对现有选型的审计**（诚实记录，均为**未达标**项）：

| 子系统 | C++ 格式 | 当前 Rust 实现 | 判定 |
|---|---|---|---|
| 全文倒排 | 自研 `storage/invertedindex/fst` + `format/` + posting/block-max | 自研 BM25 + 内存匹配（无磁盘倒排） | **不符**：打分/行集已对齐（`fulltext_delete` 7/7），但**未落盘、未复刻文件格式** |
| 向量列存储 | 列/段文件（`storage/segment`、column vector 序列化） | 行文本写入 fjall KV | **不符** |
| 向量索引 | HNSW/IVF 索引文件（表目录下） | 仅内存二级索引，`open` 时重建 | **不符**：既不兼容，也未持久化 |
| Catalog/Meta KV | **RocksDB + 自定义 MergeOperator**（`src/storage/catalog/kv_store*.cppm`、`src/storage/common/rocksdb_merge_operator*`） | fjall（纯 Rust LSM） | **不符**：需改用 `rust-rocksdb`（§4.2 已列为「保留磁盘兼容」回退） |
| 二级索引 | PGM-index 文件 | 内存 BTree 重建 | **不符** |

**结论**：当前的 Rust 引擎满足的是 **SQL 语义 / 协议 / 打分公式**级一致（Gate A 的 statement 面已达 100%，query 面 1394/1532），**但不满足磁盘格式兼容**。要达到本要求，需按上表逐项替换为「读 C++ 格式、写 C++ 格式」的实现（存储内核 M1 需重做列/段编码；M3 需落盘 HNSW/IVF/PGM/倒排；KV 需切到 RocksDB）。**该要求已登记为阻塞项（任务 #67），并覆盖 §3 D2 的旧表述。**

**任务 #67/#68 进展（2026-10-17）**：已产出 `rust_engine/docs/CPP_FORMAT_SPEC.md`，冻结**已核对**的字节级事实：①数据文件/快照文件通用布局（`magic=0x00dd3344`(u64) + `buffer_size`(size_t) + 补零数据区 + `checksum`(u64)，`data_file_worker_impl.cpp:68-116/119-155`，读取侧断言 `:160-172`）；②Catalog KV = RocksDB + 自定义 MergeOperator（`kv_store*.cppm/impl`、`rocksdb_merge_operator*`）；③倒排索引的模块构成与 FST 校验和结论（PoC F）；④BM25 统计量来源（`term_doc_iterator_impl.cpp:57-70`）；⑤SEARCH 语义。**§6 清单的 #1–#5 已全部冻结**（见该文件 §8、§9）：目录/文件命名、段块元数据（**不存在** `SegmentEntry/BlockEntry` 文件；元数据在 RocksDB KV，行数由块 `version` 文件推导）、块内列式编码（`[data_region (capacity*slot)][null_bitmap ((capacity+7)/8, bit=1 非空)]`、Varchar 两字 slot）、HNSW/IVF `chunk_{id}.idx`（**无 magic/version/checksum**）、全文 `.dic`(TermMeta+FST)/`.pos`(逐 term 块)/`.len`(裸 `u32[]`)。#6 的**键空间**亦已冻结（值编码/MergeOperator 待）；#7（WAL）、#8（PGM）仍待。

**已落地的实现**（`infinity-storage` 新模块 + `infinity-executor::cpp_blocks`，均带逐字节单测）：
- `cpp_format`：数据文件框架、块 `version` 文件、列体 `[data_region][null_bitmap]`、`ColumnLayout`/`pack_column`（含 Varchar 两字 slot 与 embedding 内联定宽）、**整块目录读写**、日期时间与 f16/bf16 编解码。
- `cpp_blocks`（执行器）：**把真实表数据镜像成 C++ 块文件**（`{data_dir}/blocks/db_{db}/tbl_{id}/seg_0/blk_{n}/`，每 8192 行一块），`Engine::flush()` 自动对每张可打包的表做镜像，并可**脱离 KV** 从块文件读回。**已覆盖列型**：定宽标量（int/float/date/time/datetime）、`VARCHAR`/`JSON`（两字 slot + outline）、`EMBEDDING`/`VECTOR`（内联定宽）、`TENSOR`/`MULTIVECTOR`、`SPARSE`、`ARRAY(<定宽标量>)`（均为 count/offset slot + outline）。实测字节：`INT` 列 `0.col`=33816、`BOOL` 列=2072、`EMBEDDING(FLOAT,3)` 列 `1.col`=**99352**（16 + 8192·3·4 + 1024 + 8）、`TENSOR(FLOAT,3)`/`SPARSE`/`ARRAY(INT)` 列 `1.col`=66584（8 字节 slot）且 `col_1_out` 分别为 24/12 字节，`version`=65556——全部等于冻结布局算出的精确字节数。**明确不支持**（返回错误而非写错字节）：`EMBEDDING(BIT,*)`、`TENSORARRAY`、嵌套 `ARRAY`、变长元素数组（slot 只记外层计数，内层计数/偏移不可还原）。
- `inverted_format`：LEB128、TermMeta（`.dic` 记录区，含 tf/payload 选项位）、`.len` 裸 `u32[]`、`.pos` 每 term 框架、`fst_len_from_trailer`、**FST 尾块（`u64 len_` + `u64 root_addr` + 掩码 CRC-32/IEEE 校验和）的写入与校验**（校验和覆盖含前两字段的全部字节，仅排除末 4 字节，与 `Fst::Verify` 的 `CRC32IEEE(data_ptr_, data_len_-4)` 一致）。
- `knn_format`：HNSW 头部（56 字节定偏移）、`labels`、整块块文件组装/解析（用 `layer_sum` 与顶点 stride 校验图区长度，对应 C++ 的「读到文件尾」断言）、`get_mmax`/level0/levelx stride、**图区编解码**（`u64 layer_sum` + level0 记录（`layers_p_` 由指针改写为 u64 字节偏移）+ 上层记录，邻居零填充到 Mmax）、**IVF 三种 storage 的 chunk 编解码**：plain（`u32 row_count/embedding_count/dimension/centroid_count` + `f32[]` 质心 + 逐 part `u32 embedding_num` + `u32[]` 段偏移 + 原始向量）、SQ（共享 `common_vec_a/b` + 每 part 的量化码 + `dot_v_e`，8-bit/4-bit）、PQ（`real_subspace_centroid_num` + 码本 + 子空间权重 + 每 part `last_code_id`/`storage_size`/码字）。**关键 VERIFY 已用实测关闭**：用 g++ 按 `graph_store.cppm:32-40` 的字段编译量测得 `sizeof(VertexL0)=24`（`layer_n_`=0、`layers_p_`=8、`neighbor_n_`=16、`neighbors_`=20）、`sizeof(VertexLX)=4` → `M=16` 时 level0 stride 152 / levelx stride 68，Rust 常量 `VERTEX_L0_SIZE`/`VERTEX_LX_SIZE` 已固定并有往返测试。
- **Catalog KV 的 RocksDB 基底（§2.6.1 第 ④ 项，任务 #69 起步）**：新增 `infinity-storage::metastore_rocks::RocksMetaStore`（实现 `MetaStore` trait：`get/put/delete/atomic_write(WriteBatch)/scan_prefix(有序迭代)/flush`），键一律用 `catalog_format` 的 **C++ 键拼写**，因此写入/读取的就是 C++ 引擎能打开的 catalog。**用 `rocksdb-store` feature 门控**（默认构建仍是纯 Rust fjall，不引入 C++ 依赖）。**构建环境的坑已解决（记录）**：`librocksdb-sys` 自带的 RocksDB 用本机 GCC 16 编不过（`trace_replay/trace_record.cc` 失败）；改用系统 RocksDB 10.2（`ROCKSDB_LIB_DIR=/usr/local/lib64`）时其头文件已与 crate 的 FFI 不匹配（`rocksdb_slicetransform_create` 多出 `slice_transform_name_callback` → E0061/E0308）。**最终方案：用 `clang`（23）编译 crate 自带版本**（`librocksdb-sys 0.19.0+11.8.1`，即 RocksDB 11.8.1），已在 `rust_engine/.cargo/config.toml` 的 `[env]` 里设 `CC=clang`/`CXX=clang++`（项目作用域，非行内环境变量）。`cargo build/test -p infinity-storage --features rocksdb-store` **通过**（4 个用例：put/get/delete、前缀有序扫描、原子批写、flush 后重开存活）。**执行器已可跑在 RocksDB 上（2026-10-17）**：`infinity-executor`/`infinity-cli` 新增 `rocksdb-store` feature（`--features rocksdb-store`），`Engine::open` 在该 feature 下改用 `RocksMetaStore`。实测：`cargo test -p infinity-executor --features rocksdb-store` **51 用例全绿**；CLI 在该 feature 下建出真正的 RocksDB 目录（`CURRENT/MANIFEST-*/LOG/LOCK/IDENTITY`）且 `CREATE/INSERT/SELECT` 正常。**同一 revision 背靠背对跑两种基底，Gate A 完全一致（均 1387/1536）**——即基底替换不影响 SQL 语义（此前一次「1398 vs 1387」的差异经复核是**跨 revision** 造成：测量期间并行会话提交了新的 EXPLAIN 实现，HEAD 由 `861f0ef04` 移到 `43b49b87c`）。
**Catalog 的 C++ 键 + 值编码已可读写（2026-10-17，`36736882a`）**：`infinity-catalog` 新增 `value_format`（`DataType::Serialize()`/`ColumnDef::ToJson()` 的逐字段复刻，键序与 nlohmann 一致因为两者都按键排序）与 `cpp_layout`（`Catalog::write_cpp_layout` 把真实 catalog 写成 **C++ 键空间 + C++ 列 JSON**，`read_cpp_layout`/`read_cpp_tables_as_meta` 读回）。实测：含 `INTEGER`/`VARCHAR`/`EMBEDDING(FLOAT,3)` 三列的表写出的键恰为 `catalog|db|default_db|1`、`catalog|tbl|0|t|2`、`tbl|0|1|next_column_id`、`tbl|col|0|1|v|2`、`next_database_id`、`next_table_id`，列值为 `{"column_id":2,"column_name":"v","column_type":{"data_type":12,"type_info":{"dimension":3,"embedding_type":6}}}`，读回后 type_info 完整（测试还查出并修掉两个键解析 bug）。
**索引记录的 `IndexBase` JSON 已可写读（2026-10-17）**：`value_format` 新增 `IndexDefValue`（公共五字段 + 子类 `extra`）+ 各子类构造器 `hnsw`/`hnsw_default`/`full_text`/`secondary`/`ivf_plain`，并**冻结**了此前存疑的两点：`file_name = "{table}_{index}"`（`logical_planner_impl.cpp:859`；VERIFY 已解除）、`flag = OPTION_FLAG_ALL = 63`。`IndexMeta` 增加 `index_type`（大写 C++ 名）与 `with` 选项（`\t` 分隔、`\x1f` 分隔 `k=v`；解码对旧 2 字段记录容忍），`Catalog::create_index_typed` 落库，执行器 `CREATE INDEX` 与 `RENAME TABLE` 两个路径都改为透传类型与选项。`write_cpp_layout` 现按 C++ 的 `next_index_id`（从 0 起）与 `catalog|idx|{db}|{tbl}|{name}|{ts}` → `index_id` 写出索引记录，值写 `idx|…|{index_id}|index_base`；`read_cpp_indexes` 读回。实测：HNSW（`metric_type=l2`、大写 `M` 排在 nlohmann 键序最前）+ FULLTEXT 两条索引写出的 `index_base` JSON 与 C++ 逐字节一致，`next_index_id` tag = `2`；`infinity-catalog` 23 用例、`infinity-executor` 41 用例全绿，clippy 干净。
**仍未做**：①MergeOperator 已查证为**注册但从不调用**（无 `->Merge(` 调用点）→ 对磁盘格式无影响，无需实现；②**引擎已双向接通 C++ catalog**（2026-10-17，`1d95c53f7`）：`Engine::flush` 会把 C++ 布局写进**引擎自己的 store**（`<dir>/kv`，best-effort：未覆盖的列类型保留旧布局而不让 flush 失败），`Engine::open` 在自有 catalog 为空但 store 里有 C++ 键时**导入**其 schema（库/表/列的类型文本、注释、默认值、NOT NULL、索引与选项全部还原；`type_text_of` 是 `data_type_of_text` 的逆）——即引擎能打开 C++ 写出的目录，且自己的目录里就有 C++ 引擎可读的 catalog；③**C++ 布局已落到位**（2026-10-17，`94f1ba5ab`）：C++ 引擎的 catalog 是 `<DataDir>/catalog` 的 RocksDB（`config_impl.cpp:402`、`storage_impl.cpp:295`），而引擎现在把 C++ 键值**写进 catalog store**（不再是 `<dir>/kv`）；在 `rocksdb-store` feature 下该 store 本身就是 `<dir>/catalog` 的 RocksDB（`Catalog::with_store` 取代 fjall），因此 **`<dir>/catalog` 是一个 RocksDB，同时容纳原生键空间与 C++ 引擎可读的 C++ 键空间**（默认构建仍用 fjall，自洽但对 C++ 不可读）。仍待：把 C++ 键空间做成**唯一** catalog（现在两者并存）。**至此 §8.6 的值编码已全部冻结**：`ColumnDef`/`DataType`、`IndexBase` 全部十种类型、`ConstantExpr`、`SparseInfo` 都有逐字段复刻 + pin 测试。
- `catalog_format`：**全部 catalog 键**（`catalog|db|…`、`tbl|col|…`、`catalog|blk|…`、`idx_chunk|…` 等）与对应前缀、drop 墓碑、全局系统键，逐字符串断言。

**尚未落地**：#68 剩余（执行器仍以 fjall 行文本为主存储，块文件是镜像；**列型覆盖已完整**（除位张量 `TENSOR(BIT,d)`/`TENSORARRAY(BIT,d)` 按设计在范围外）——本轮补齐：嵌套 `ARRAY`（`052dc1d0a`：`ArrayInfo::ElemSize` 是内层 `DataType::Size()`，对 `ARRAY(<定宽标量>)` 元素即 8 字节 `ArrayType` 槽，`array_shape` 现返回 `ArrayElem::{Fixed,Nested,VarLen}`）、`ARRAY(VARCHAR)`（`91d7efe65`：元素是 16 字节 `VarcharT` 槽，各自内联或外置）、`TENSORARRAY(<elem>,<dim>)`（`38ac7118d`：`file_offset_` 指向 `tensor_num` 个 8 字节 `TensorT` 元数据记录，各带自己的元素数与数据偏移）、`EMBEDDING(BIT,*)`（位打包 `ceil(d/8)` 字节，`6b8ff9102`）、`SPARSE(BIT,*)`（无值字节、值为隐式 1，`9bd74aaa4`））；**`Varchar` 槽口径已修正**（`320b0670d`：`≤ VARCHAR_INLINE_LENGTH`(13) 字节**内联进槽**，更长的才进 outline（5 字节前缀 + 偏移）；此前镜像**总是**走 outline、读侧对 >13 字节只回 `<outline:N>` 占位符——即长字符串镜像回读是坏的））；#69 剩余（RocksDB 基底 + 列/索引值编码均已就绪；C++ 布局已落到 `<dir>/catalog`（`rocksdb-store` 下即 C++ 引擎所用的那一个 RocksDB），**仍缺**把 C++ 键空间做成**唯一** catalog——现有实现里它与原生键空间并存。**设计草案（下一轮直接照做）**：① C++ 键空间与原生键空间**不同构**（C++ 有 per-db/table 的 `ts` 与 `next_table_id`/`next_column_id`/`next_index_id` 计数器，键形如 `db|{name}|{ts}` / `dbnames|{name}` / `…|tbl|{tbl}|{ts}` / `…|col|{id}` / `idx|…|{name}|{ts}`），所以**不能**把 `table_key`/`name_key`/`db_key`/`index_key` 直接换拼写；② 正确做法是**换持久化语义**：把 `Catalog` 的**权威状态**改成「内存里的 `dbs/tables/indexes` 三张表」，`open` 时用 `read_cpp_layout`（→ `CppTable`，含 db/name/columns/类型）与 `read_cpp_indexes` 装载，任何 DDL 改内存后调用已有的 `write_cpp_layout` **整体重写** C++ 布局（幂等；需补 `drop_*` 的**删除路径**：按当前 `ts` 删掉该实体的 db/table/column/index 键——现在 `write_cpp_layout` 只写不删）；③ 用 `RwLock<Inner>` 保护三张表，`counter_lock` 可并入；④ 落地后用 `enumerate` 断言 store 内**不存在**任何 `tbl:`/`tblname:`/`db:`/`idx:` 前缀键，并跑全量 conformance 确认不回归（当前基线 1468）。**风险**：catalog 在每条查询路径上，改错会整体崩——务必先加 `cpp_only` 开关与上述断言测试，再切 `Engine::open`）。**2026-10-17 落地结果（重要，含实测阻碍）**：`catalog: cpp-only` **模式已实现且测试通过**——`Catalog::with_store_cpp_only` / `open_cpp_only`，内部按「内存权威 + DDL 后整体 `write_cpp_layout_from` 重写 + 先扫 `catalog|`/`db|`/`tbl|`/`col|`/`idx|` 前缀删除」实现（`rewrite_cpp`），`alloc_id` 在 cpp 模式下取「现有表 id 最大值 +1」不再写原生计数器；新增 `index_meta_of` 把 C++ 索引记录映射回 `IndexMeta`；测试 `cpp_only_catalog_writes_only_the_cpp_key_space` 断言 **store 内无任何 `tbl:`/`tblname:`/`db:`/`idx:`/`__counter:table` 键**、读写/重开/三级 drop 均正确（`infinity-catalog` 29 用例全绿）。**实测阻碍（已大幅收窄）**：先把 `Engine::open` 切到该模式，Gate A 一度 `statement ok` 1752→1748、`query` 1468→**1446**（-22）。逐条定位后修掉两类真因：① **C++ catalog 不保存引擎的 SQL 类型拼写**——`type_text_of` 原本回写 C++ 内部名（`FLOAT32`/`FLOAT64`/`UINT8`…），泄漏进引擎类型文本后 `SHOW TABLE` 打印 `Sparse(float32,…)`/`Embedding(float32,…)` 而 `.slt` 期望 `float`/`double`；已把 `element_name` 改成**引擎拼写**（`float`/`double`/`int8`/`uint8`/`int16`/`int32`/`int64`/`float16`/`bfloat16`/`bit`，`data_type_of_text` 读侧本就 `to_ascii_uppercase` 后再匹配、不受影响），并修了 `element_code` 把 `UNSIGNED TINYINT` 误判成 `INT8`（应为 `UINT8`）的 bug——两项合计把回退从 **-22 收窄到 -2**（`query` 1446→**1466**），且对原生路径（`import_cpp_layout`/`read_cpp_tables_as_meta`）是净改进。② **仍差 -2 的根因是函数索引**：`ddl/index/test_secondary_functional_index.slt` 的 `CREATE INDEX … (sqrt(c1))` 把表达式放在 `IndexMeta.column`，而 `index_def_for` 开头 `table.columns.iter().find(|c| c.column_name == im.column)?` 找不到同名列 → **整条索引记录被丢弃**（cpp 模式下索引随之消失，规划退回 `FILTER`+`TABLE SCAN`，6 query + 4 statement 失败）。C++ 侧对应 `SECONDARYFUNCTIONAL`（`value_format::IndexDefValue::secondary_functional` 已有构造器，见 `docs/CPP_FORMAT_SPEC.md` §8.6 第 580 行），但需要 `func_col_params` + `func_return_type`（`DataType::Serialize()` 对象）+ `function_expression`（**内嵌 JSON**，C++ 会 reparse）——即把引擎的表达式文本重新序列化成 C++ 的 `FunctionExpr` JSON，本阶段未做。**② 已闭合（2026-10-17，`445f7bd6a`）**：函数索引现在写成 `SECONDARYFUNCTIONAL`（`column_names`=表达式引用的基列、`func_col_params`=引擎的表达式文本、`function_expression`=`{"name":"<func>"}`、`func_return_type` 暂用基列类型近似），读侧 `index_meta_of` 用 `func_col_params` 还原 `IndexMeta::column`；对引擎**故意不物化**的表达式目标（`CREATE INDEX i ON t (c1 + 5)` 解析出的 `column` 为**空串**，无 C++ `IndexBase` 可写）把**索引名**记进表 tag `rust_name_only_indexes`（C++ 会忽略未知 tag），从而仍存在且可 DROP；顺带补上缺失的 **`HUGEINT`**(LogicalType 5) 映射（服务端线上 `logic_type=5` 发 `HUGEINT`，此前会让 `CREATE TABLE` 失败）。**结果：`Engine::open` 现已切到 `with_store_cpp_only`/`open_cpp_only`——C++ 键空间就是唯一 catalog**；Gate A 与原生基线**逐文件一致**（`statement ok` 1752/1752、`statement error` 129/129、`query` 1478/1536、同样那 68 条失败），workspace 测试 38/38、clippy `-D warnings`、`cargo fmt --check` 全绿。**§2.6.1 ④ 完成。**写侧只落 C++ `DataType`，读侧 `type_text_of` 归一化（`INT`→`INTEGER`、`FLOAT`→`FLOAT32`、`EMBEDDING(FLOAT,3)`→`EMBEDDING(FLOAT32,3)`），于是 `SHOW TABLE`/类型相关语句及 `.slt` 的期望文本对不上（`INT`/`INTEGER` 对引擎是两种拼写）。**结论：④ 与 Gate A 的「类型名逐字一致」存在真实张力**；要让 C++ 键空间成为唯一 catalog，需先让**引擎全链路采用 C++ 的类型拼写**（parser/executor/类型表 + 相应 `.slt` 期望），这是一项独立的、会影响 Gate A 数量的改动。**已按此结论处理**：模式默认**关闭**（`Engine::open` 维持原生 catalog + 镜像 + 空原生时导入 C++ 布局），`cpp_only` 作为可选能力与证据保留。另修了一处顺带发现的真 bug：`default_constant_expr` 只认**带引号**的字符串字面量，而引擎存的是**不带引号**的 `.slt` 文本，导致字符串列 DEFAULT 在 C++ 布局里丢失；现按「非空、非布尔、非数字即字符串常量」处理，`default_text_of` 同步改为**不带引号**回写（与 `default_literal` 的读法一致），并更新了两处测试期望；#70 剩余（**文件格式已齐**：HNSW 头/向量/标签/图区 + IVF plain/SQ/PQ；**HNSW 持久化已接通**（`16565e180`：`Engine::write_cpp_hnsw_index` 用引擎自带 `HnswIndex` 在向量列上建图，按 C++ 布局写成 `db_{db}/tbl_{tbl}/idx_{id}/seg_0/chunk_0.idx`（56 字节头 + plain 向量区 + 图区 + `u32` 标签数组），`read_cpp_hnsw_index` 脱离元数据读回；索引 id 取该表内的索引序号，与 `write_cpp_layout` 一致；`Engine::flush` 现在一并镜像 HNSW 索引列。注意：与 C++ 自建的索引**不逐字节相同**（建图 RNG 不同），但因偏移全部显式而**格式可读**）；**IVF 持久化已补齐**（`36cb35eb0`：`write_cpp_ivf_index`/`read_cpp_ivf_index` 按 `kPlain` 布局写入「4 个 u32 头 + f32 质心 + 每 part（u32 个数 + 段偏移 + 原始向量）」，k-means 由等距取样确定性播种；`Engine::flush` 一并镜像 IVF 列，质心数取索引选项 `centroid_count`（默认 16））；**HNSW 已接进查询路径**（`e83b0b793`：`rebuild_indexes` 从镜像 chunk 文件把 HNSW 图读回内存——`HnswIndex::from_parts` 由序列化的 header / 向量区 / 图区 / labels 重建——`SEARCH MATCH VECTOR` 在 **L2 单向量**列上改用**该图的候选**，候选仍**精确打分**，`k` 取 `max(4*topn,64)` 且以行数封顶（小表被完全覆盖）；**A/B 实测**（同 revision 下把剪枝关掉）Gate A 均为 **1478/1536**，即该改动对一致性**中性**；位向量/多向量/张量/稀疏与 `ip`/`cosine` 仍走全表，因为镜像图是 L2 索引）；#71 **落盘格式已全部完成**（`.dic` 容器、FST 打包原语/`COMMON_INPUTS`/节点状态机、`.pos` 的 **u32 SIMD-BP128 复合**与 **u16 NewPForDelta** 的**编解码**，全部逐位对齐 C++ 真值——FST 真值由 `rust_engine/tools/fst_golden/gen.py` 生成，posting 真值由 `third_party/newpfor`+FastPForLib 编译的 harness 生成，均记入 `docs/CPP_FORMAT_SPEC.md` §9.3.5）；#71 剩余仅为**把索引持久化/查询接进引擎**（执行器侧）。

### 2.7 测试资产（验收面）

| 类型 | 位置 | 规模 | 运行方式 |
|---|---|---|---|
| **SQL Logic Test** | `test/sql/**/*.slt` | **173 文件**（内联字面量结果，`----`，无 hash） | 外部 `sqllogictest` 二进制 v0.28.4（risinglightdb），`tools/sqllogictest.py` 驱动 |
| **Python PySDK** | `python/test_pysdk` | **28 文件 / 313 个 `def test_`（308 有效）** | `uv run pytest`，连真实服务（23817） |
| **Go SDK** | `go/tests` | **31 文件 / 375 用例** | `go test ./tests/...`，连真实服务 |
| Python 重启测试 | `python/restart_test` | 14 文件 / 46 用例 | 需 `--infinity_path` |
| Python 并发测试 | `python/parallel_test` | 8 文件 / 16 用例 | |
| Python 集群测试 | `python/test_cluster` | 16 文件 / 34 个 `def test_`（29 有效） | netns/sudo |
| C++ 单元测试 | `src/unit_test` | **303 文件 / ~1032 用例** | GTest `test_main`（ASAN/TSAN/UBSAN） |
| 性能基准 | `benchmark/`、`python/benchmark/` | KNN/全文/稀疏等 | golden log 比对（`tools/ci_tools/check_benchmark_result.py`） |
| HTTP API | 复用 pysdk 用例 + `--http` | 独立执行模式 | `tools/run_http_api.py`（CI 已跑，`.github/workflows/tests.yml`） |
| CI | `.github/workflows/{tests,slow_tests,release}.yml` | 37 + 多个 job | Clang 20 + vcpkg；x64-v2/v3/v4 + arm64 |

> 注 1：`AGENTS.md` 中引用的 `python/test/cases/...` 路径已过时，实际为 `python/test_pysdk`。
> 注 2：上表 Python 计数为**子串**匹配 `def test_` 的**原始**计数（PySDK **313**、cluster **34**），其中各含 **5 处被注释掉**的定义（如 `python/test_pysdk/test_basic.py:37`、`python/test_cluster/test_insert.py:60`）；**有效定义**为 PySDK **308** / cluster **29**（正则 `^\s*(?:async )?def test_`）。两者都**不等于** pytest 参数化后的**收集用例数**。故「固定数字」不是可执行验收口径——验收须冻结**收集到的 test ID 清单 + marker/skip + 数据集 + 生成的 `.slt` manifest**，以「执行/通过/跳过」为准（见 §6 Gate A/B）。
> 注 3：`.slt` 与部分数据在运行前由 `tools/generate_*.py` 生成（`tools/sqllogictest.py:137`），需一并纳入版本化与校验。

---

## 3. 关键决策（Decision Matrix）

### D1 — 迁移策略：洁净室重写 + 持续一致性测试（推荐）

| 方案 | 说明 | 评价 |
|---|---|---|
| A. 一次性 Big-bang | 全部重写完成后切换 | 风险最高，长期无可交付中间态 |
| B. Strangler / FFI 渐进替换 | 保留 C++，逐模块用 `cxx`/FFI 桥接替换 | 与「整体重写」目标不符；FFI 边界多、收益打折 |
| **C. 洁净室重写 + conformance testing（推荐）** | 新建 Rust workspace；**新旧二进制并存**；以 C++ 引擎为参考 oracle 做差分；按垂直切片（建表→写入→查询）逐步点亮 slt/pysdk/go 测试子集 | 每次提交都有可运行产物与测试进度；风险可控；满足「整体重写」 |

**决策**：采用方案 C。**线协议（Thrift/HTTP）必须兼容**，因此整套现有 SDK 测试**无需修改**即可作为验收门槛。

### D2 — KV 存储：RocksDB 的 Rust 替代（用户问题 2）

**先厘清职责**：Infinity 中 RocksDB 只存**元数据**（catalog/meta、ID 计数器、对象清单、rough filter、drop 墓碑），**不存行数据/索引**，负载特征为「低容量 + 事务 + 快照 + 计数器」，且**未用列族 / 自定义比较器 / 显式 block cache**。这大幅降低了替换难度。

选型矩阵：

| 方案 | 类型 | 优势 | 风险 / 局限 | 结论 |
|---|---|---|---|---|
| **`rust-rocksdb`（crate `rocksdb`）** | FFI 绑定 C++ RocksDB | 能力 1:1（`TransactionDB`/Snapshot/MergeOperator/BackupEngine/CF/Iterator/SstFileWriter）；**保留磁盘兼容**；最成熟 | 仍是 C++（需编译 + FFI 边界）；非「纯 Rust」 | **回退方案 / 迁移期默认** |
| **`fjall`** | 纯 Rust LSM-tree | LSM 架构最接近 RocksDB；keyspace/partition ≈ 列族；原子批写、快照读、事务句柄；活跃维护（~2.2k★） | 需验证：跨 partition 原子提交、快照隔离语义能否完整覆盖现有事务模型 | **推荐（新引擎默认）** |
| `redb` | 纯 Rust CoW B-tree（类 LMDB） | 纯 Rust、ACID、MVCC 读、单写多读、简单 | 写吞吐 / 空间放大在高频写入下需验证；批量元数据场景尚可 | 备选 |
| `sled` | 纯 Rust | API 友好 | 长期 beta、1.0 遥遥无期、维护存疑 | **不建议** |
| `persey` / `sanakirja` 等 | 纯 Rust | 小众 | 生态/维护弱 | 不采用 |

**决策**：
- **定义 `MetaStore` trait**（`begin` → 事务句柄；`get/put/delete/iterate`；`snapshot`；`commit/rollback`；`backup/restore`；`flush`）。
- **默认实现：`fjall`**（达成纯 Rust 目标）；**保留 `rust-rocksdb` 实现**（迁移期做数据搬运 / 差分，或作为 fjall 能力缺口时的回退）。
- **需专项验证**（Poc 门槛）：
  1. **KV 能力**：① 计数器（原 merge operator）→ 事务内 read-modify-write；② catalog 一次事务内多 key 的**原子提交**；③ **快照隔离**读（原 `set_snapshot=true`）；④ backup/restore。
  2. **跨底座宕机一致性**（关键，不能只看 KV 单点能力）：明确并验证 **提交应答 / 持久化顺序 / 回放幂等 / checkpoint 发布 / WAL 删除** 的契约，覆盖 catalog（KV）+ WAL + 块文件（含版本文件）+ 对象清单四条路径的**边界宕机注入**（在 ack 前、WAL 落盘后、KV 提交前/后、checkpoint 交换点等）。参考现状：恢复依赖 checkpoint 时间戳（`src/storage/wal/wal_manager_impl.cpp:632`），WAL 处理先于 bottom 提交（`:330`、`:348`）。
  3. **备份/恢复缺失（PoC B 结论）**：`fjall` **无内置 backup/restore**，仅 `persist(SyncAll)` + 目录拷贝可用 → 需 Infinity 自建版本化备份/恢复（对应 R4 余留项）。
- **磁盘不兼容**：新格式与旧版本不互通 → 提供 `infinity-import`（从旧目录读取）或文档要求重灌数据。

### D3 — 列式数据模型：自研紧凑列式（保留） + Arrow 仅用于边界

| 方案 | 评价 |
|---|---|
| **保留自研列式**（移植 `DataBlock`/`ColumnVector`：flat/constant/compact-bit/heterogeneous + null bitmask） | **推荐**。热路径（向量距离、聚合、索引扫描）需要紧凑表示与自定义序列化（WAL/网络），Arrow 的通用性在此是负担 |
| 全面改用 Arrow | 不推荐。会引入表示转换开销，且需重写全部算子与序列化 |

**决策**：核心执行用 Rust 自研列式（`enum ColumnVector`）；**仅**在 Parquet 导入/导出与结果编码等**边界**使用 `arrow` crate（`arrow2` 已非主流，采用官方 `arrow`）。网络结果编码保持现有 `ColumnField` 语义（Thrift 列式），以保证 SDK 兼容。

### D4 — Tokio 替代线程池与调度器（用户问题 1）

采用「**async 编排 + CPU 计算下沉**」两层模型；**不要**把 CPU 密集算子直接塞进 Tokio reactor 线程。

| C++ 现状 | Rust/Tokio 目标 |
|---|---|
| `TaskScheduler` 固定线程池 + 逐 CPU 绑核 | `tokio::runtime::Builder::new_multi_thread().worker_threads(cpus)`（无需手动绑核） |
| Worker 私有 `BlockingQueue<FragmentTask*>` | `tokio::sync::mpsc::channel(cap)`（有界 → 天然背压） |
| fragment 依赖（`unfinished_child_n_` 计数） | **有界队列交接**：消费者必须与生产者**并发**排空（**与消费者类型无关**）；仅**生产者自物化**（无有界交接）时才可「完成后启动」（见下方 ⚠️） |
| `Notifier`（mutex+condvar+计数） | `JoinSet::join_next()` / `futures::try_join_all` / `tokio::sync::Notify` |
| `GetResult()` 阻塞 | `async fn get_result().await` |
| 背压 `allow_enqueue_`（下游满足即停上游） | mpsc 有界通道 + receiver drop 天然背压；必要时 `Semaphore` |
| 阻塞算子（table/index scan、aggregate、knn、sort） | `rayon::par_iter` 或 `spawn_blocking`；向量/矩阵用 SIMD 内核 |
| WAL flush、compaction、bg_task、定时任务 | `tokio::spawn` + `interval` / 专用 runtime；写 WAL 串行化保留（单写者） |
| 本地队列交换 `kLocalQueue` | `mpsc`；远端交换（集群）后续用网络重写 |

**决策**：
- 查询内并发用 **Tokio 结构化并发**（`JoinSet`）替代手工线程/条件变量。
- **CPU 密集内核**（距离计算、聚合、索引扫描、打分）统一下沉到 **Rayon 线程池**，Tokio 仅负责编排与 IO 等待。
- 保留现有**语义**：有界交换 + 背压、物化式 fragment 边界、串行 WAL 提交。
- 目标是把「`kParallelStream/kParallelMaterialize/kSerialMaterialize` + merge 算子的 fragment 切分」映射为一组 async 组合子 + 有界通道。

> ⚠️ **BLOCKER 修正（codex 第 1、2 轮）：并发由「交换 / 生产者」决定，而非「父 fragment 类型」。**
> 有界通道下，只要消费者在生产者结束之后才排空，生产者就可能写满通道而无法结束 → **死锁**。C++ 的触发点在**子（生产者）一侧**：`FragmentContext::TryFinishFragment` 判定 `fragment_type_ == kParallelStream`（`src/scheduler/fragment_context_impl.cpp:592`），一旦该流式子 fragment 已产出数据（`:604`），就**在其结束前**调度父 fragment（`:610`）。而 **Materialize 父也可能拥有流式子**——merge 父为 `kSerialMaterialize`（`src/executor/fragment_builder_impl.cpp:235`），其子若是 `kTableScan/kIndexScan` 则为 `kParallelStream`（`:307`）——故「子完成后再消费」仍会死锁。
> **正确规则**：**凡是经有界队列交接的边，消费者都必须与生产者并发排空**（与消费者 fragment 是 stream 还是 materialize **无关**）。只有**生产者自身完成物化**（不经过有界交接，或另行定义落盘/spill 机制）时，才允许「完成后再启动」。
> **Rust 映射**：队列两端 `send().await` / `recv().await` 并发推进，满则反压挂起；须以**混合类型边 + 容量=1 + 早停 `LIMIT` + 错误传播 + 取消**用例证明可前进、无死锁。

**资源与取消模型（codex 第 1 轮 #6）**：
- **线程预算**：Tokio worker、Rayon 池、专用 blocking 池各自的线程上限显式配置，合计受 `CPULimit` 约束；禁止无界 `spawn_blocking`。
- **准入控制**：每查询的 CPU / 内存上限（信号量 + 内存追踪）；超限拒绝或降级，而非耗尽全局。
- **协作式取消**：用 `tokio_util::sync::CancellationToken` 贯穿 fragment；客户端断开 / 查询超时 / 上游错误时级联取消，确保 `mpsc` 生产者停止、事务回滚、临时文件清理。
- **panic 安全**：任务 panic 必须被 `JoinSet` 捕获并转为查询错误，同时释放事务与缓冲句柄（RAII + drop guard）。

### D5 — 解析器：自研递归下降 + `logos` 词法（推荐）

| 方案 | 评价 |
|---|---|
| 移植 Bison/Flex 到 `lalrpop` | LALR 生成器在 Rust 侧维护成本高；与现有 `.y` 一一对应难 |
| 基于 `sqlparser-rs` 扩展 | Infinity 有大量专有语法（`MATCH TEXT/TENSOR/SPARSE`、`FUSION`、KNN），`sqlparser-rs` 扩展点有限，易与其版本漂移 |
| **`logos` 词法 + 手写递归下降（推荐）** | 完全掌控专有语法与错误；slt 的 `statement error` 仅要求「报错」而非精确文案；可增量实现 |

**决策**：`logos` 词法 + 手写递归下降解析器，AST 用 Rust `enum`；保留现有 `StatementType`/表达式语义。

**语法清点（codex 第 1 轮 #7）**：项目存在**多套文法/入口**，不能只按 SQL 主文法评估：
- `src/parser/parser.y` / `lexer.l`：主 SQL 文法（DDL/DML/DQL；**含 `MATCH`/`FUSION`/KNN 等搜索子句 token**，见 `parser.y:411`）。
- `src/parser/search_parser.y` / `search_lexer.l`：**布尔全文查询文法**（`topLevelQuery/query/clause/term/basic_filter`，见 `search_parser.y:81`）——**不是** MATCH/FUSION。
- `src/parser/expression_parser.y` / `expression_lexer.l`：独立表达式文法（SDK 用），入口 `src/parser/expr_parser.cpp:36`。
- `src/network/http/http_search_impl.cpp:43`：HTTP 侧另一套 JSON→表达式解析。
- `src/storage/invertedindex/search/search_driver_impl.cpp:93`：全文查询**驱动**（消费 `search_parser` 产物，非独立文法）。

**行动**：逐套清点**文法 / 优先级 / 转义 / 拒绝行为**，先建**差分语法测试**（对同一输入，C++ 与 Rust 产出的 AST/错误必须一致），再承诺递归下降方案。

### D6 — 协议兼容：保持 Thrift/HTTP 线协议（强制）

**因为验收测试经由网络协议连接真实服务**，故：

- **保留 `thrift/infinity.thrift` IDL 与 `TBinaryProtocol` + `TBufferedTransport`**，保留 `client_version == 37` 握手 → **未修改的 Python/Go SDK 测试可直接复用**。
- **仅 IDL 兼容不等于线兼容（codex 第 1 轮 #8）**：`ColumnField` 承载**不透明二进制** `column_vectors`（`thrift/infinity.thrift:336`），SDK 侧按**小端整数/浮点**逐字节解码（`python/infinity_sdk/infinity/remote_thrift/types.py:104,116`）。须**逐类型固定**二进制布局、null/bitmask 约定、枚举与 `error_code` 取值、session 行为、缺省字段规则；并建**跨语言 golden 报文**测试与**固定版本 codegen** 的互操作测试（Rust 生成的编解码 vs 已入库的 Python/Go 桩）。
- HTTP 遵循现有路由与 JSON 形状（`docs/references/http_api_reference.mdx`）。
- PG wire 可延后；若实现，采用 `pgwire` crate。
- 若确需升级 Go SDK 的 thrift 运行时（0.22.0 → 0.24.0），须单独评估线兼容。

---

## 4. 目标架构（To-Be）

### 4.1 Cargo workspace 划分

```
infinity/
├── crates/
│   ├── infinity-common/      # 基础类型、错误、配置(toml+serde)、tracing、metrics
│   ├── infinity-lexer/       # logos 词法
│   ├── infinity-parser/      # 递归下降解析器 + AST
│   ├── infinity-expression/  # 表达式 AST + 求值
│   ├── infinity-catalog/     # 元数据模型 + MetaStore trait(默认 fjall)
│   ├── infinity-storage/     # WAL + BufferManager + BlockStore + MVCC
│   ├── infinity-txn/         # 事务/MVCC（时间戳数组）
│   ├── infinity-index/       # 索引 trait + 各索引实现(子 crate)
│   ├── infinity-planner/     # 逻辑计划 + 绑定 + 优化器
│   ├── infinity-executor/    # 物理算子 + fragment + Tokio 调度
│   ├── infinity-function/    # 标量/聚合/表函数
│   ├── infinity-server/      # Thrift/HTTP/PG 前端 + 会话
│   └── infinity-cli/         # 主二进制
└── ...
```

### 4.2 依赖映射表（C++ 依赖 → Rust crate）

| 现依赖 | Rust 方案 | 备注 |
|---|---|---|
| `rocksdb` | `rust-rocksdb` 或 **`fjall` / `redb`** | 见 D2 |
| `thrift` | `thrift`（Apache 官方 Rust） | 保 `TBinaryProtocol`，线兼容 |
| `oatpp`（HTTP） | `axum` + `hyper` + `tower` | 保持路由/JSON 形状 |
| PG wire | `pgwire` | 可选 |
| `nlohmann-json` / `simdjson` | `serde_json` / `simd-json` | |
| `arrow` / `parquet` | `arrow`（官方） | 仅导入导出边界 |
| `zstd`/`lz4`/`bzip2`/`brotli`/`zlib` | `zstd`/`lz4_flex`/`bzip2`/`brotli`/`flate2` | |
| `roaring` | `roaring` | 位图 |
| `fastpfor` / `newpfor` | 移植 或 `bitpacking`/`stream-vbyte` | 倒排 postings 编码 |
| **倒排 FST（`storage/invertedindex/fst`）** | **数据路径可复用 `fst` crate（BurntSushi/fst）；校验和须先统一（PoC F）** | PoC F：结构/读回兼容，但 crate=CRC32C、C++=`CRC32IEEE`，`Verify()` 不互通；边界谓词 `&&` vs `||`。复用前须统一校验和或放弃 `Verify()` |
| `darts`（双数组 trie，分词器用） | `yada` / `cedarwood` / `sucds`，或移植 | 与倒排 FST 用途不同，注意区分 |
| `pgm`（二级索引） | 移植，或 `sucds`/`art` | |
| `re2` | `regex`（线性时）+ 特殊场景 `fancy-regex` | |
| `cppjieba` / `opencc` | ⚠️ **`jieba-rs` 非等价（PoC G：mp 7/10、hmm 8/10）→ 建议移植 cppjieba**；若用现成 crate 须定义差异归一化。`opencc` 用绑定 | 中文分词 / 简繁 |
| `mlas`（GEMM） | `faer` / `nalgebra` + `simsimd` / `wide` | 距离与矩阵内核 |
| `eigen`（SVD） | `faer` / `nalgebra` | 多向量降维 |
| `spdlog` | `tracing` + `tracing-subscriber` | |
| `nlohmann` 配置 / `inih` / `tomlplusplus` | `toml` + `serde` | |
| `abseil` / `boost` / `fmt` | 标准库 + `itertools`/`smallvec` 等 | |
| HNSW | `usearch`（含 SimSIMD，性能佳）或 `hnsw_rs` | 若自定义编码/持久化/MVCC 集成要求高，倾向**移植** |
| 全文倒排 | 参考 `tantivy`（BM25/FST/segment） | Infinity 专有语义多，倾向**在 tantivy 设计参考下移植** |
| 测试 | `cargo test` + `proptest`；**`sqllogictest` crate**（与现运行器同源） | 可直接复用 `.slt` 资产 |
| 并发正确性 | `loom`（并发模型单元测试）+ `miri` | 替代部分 TSAN/UBSAN 覆盖 |

### 4.3 调度器映射示意（概念）

```
QueryContext::query(sql)
  └─ parse → logical plan → optimize → physical plan
       └─ build_fragments() -> FragmentTree
            └─ run(root)                       # 并发编排，规则在「边」上，不在父类型上
                 ├─ 每条「有界队列」边：producer 与 consumer 同时启动
                 │     producer: send().await（满则反压挂起）
                 │     consumer: recv().await 排空      # 与 consumer 是 stream/materialize 无关
                 ├─ 仅当「生产者自物化」(无有界交接 / 有 spill) 时
                 │     materialize/serial 父才可 await children 全部完成后启动
                 └─ leaf fragment: 每并行单元一个 task
                       task: async 编排 + rayon 计算下沉
                 cancel: CancellationToken 级联（断开/超时/错误）
            └─ collect result (DataTable)
```

要点：**有界队列的两端必须并发**（判定依据是「边/生产者」，不是父 fragment 类型，避免死锁）；**只有生产者自物化时才可等待完成**；**有界通道保留背压**；**串行 WAL 单写者保留**；CPU 计算与异步编排物理解耦；**取消可级联**。

---

## 5. 里程碑与阶段计划

> 每阶段以「可运行二进制 + 点亮的测试子集」为交付，**退出准则即对应验收关卡的通过率**。

| 里程碑 | 内容 | 退出准则（Exit Criteria） |
|---|---|---|
| **M0 立项 + 一致性基础设施先行** | Cargo workspace；CI（build/clippy/fmt/test）；Thrift codegen（`thrift` crate）；**conformance harness**：`sqllogictest` 驱动接入、pysdk/go/pytest 测试编排、**冻结测试 ID/marker/数据集/`.slt` manifest**、C++ oracle 差分框架；`MetaStore`/`Wal`/`BlockStore` trait 定稿；**跨底座宕机一致性契约**与**索引状态矩阵**；**确定性兼容规则**（精确 vs 容差）定稿；**四类原型**（存储内核 / 流式执行 / 单索引 / 协议互操作） | `Connect` 握手（`client_version==37`）成功；fake `SELECT 1` 通过；**harness 可对 C++ 引擎跑通全量测试并产出基线报告**；**确定性规则已定稿**；**原型完成并据此给出人月区间 + 应急余量**（作为**授权 M1 的前置门槛**） |
| **M1 存储内核** | `MetaStore`（D2 全部 PoC 过关，含跨底座宕机注入）+ WAL（格式全量移植）+ BufferManager + BlockStore + MVCC（时间戳数组）+ catalog | 移植的存储层 Rust 单测通过；**重启/宕机恢复测试**（`restart_test` 子集）通过 |
| **M2 查询内核** | lexer/parser（含全部文法）/expr/planner/optimizer/physical/executor（Tokio，流式父子并发） | `test/sql` 的 **ddl/dml/dql 无索引子集**全绿；`explain` 结构可产出；**差分语法测试**通过 |
| **M3 索引** | 二级索引（PGM）→ HNSW → 全文倒排 → 稀疏/BMP → **IVF** → 多向量 →（**DiskANN**：按状态矩阵决定是否纳入，当前为未实现） | `test/sql/dql/**`、`test/sql/ddl/index`、`test/sql/snapshot` 全绿；索引状态矩阵中「已实现」项逐个打通 |
| **M4 网络与 SDK 兼容** | Thrift `InfinityService` 全量 RPC（含二进制布局 spec）；HTTP 路由对齐 | **`python/test_pysdk` + `go/tests` 全绿（未修改 SDK）**；**HTTP `--http` 模式**全绿 |
| **M5 并发/持久化/性能** | 并发压测、崩溃恢复、sanitizer/Loom、性能调优、**集群（需先定范围）** | `parallel_test` 通过；基准达标（Gate E）；集群项视范围决定 |
| **M6 发布与数据迁移** | `infinity-import` 迁移工具（含校验/回滚）、打包、文档、灰度 | 发布候选；**迁移演练（含回滚）通过** |

> **范围冻结（codex 第 1 轮 #15）**：本计划默认 **集群/分布式不在首版范围**（v0.2 决策），Gate D 中集群测试**仅作为「若纳入范围」的条件项**；**数据迁移（`infinity-import`）为 M6 必做项**，不是可选项。上述两条须在立项会上钉死，避免文档内自相矛盾。

### 5.1 实现进度（`rust_engine/`）

> 实际代码库：**`rust_engine/`**（Cargo workspace）。C++ 引擎保留为参考 oracle。CI：`.github/workflows/rust_engine.yml`。更新于 2026-10-16。

| crate | 内容 | 状态 |
|---|---|---|
| `infinity-common` | 基础类型 / 错误 | ✅ 初始 |
| `infinity-storage` | **WAL**（CRC-32/IEEE 分帧 + 撕裂尾容忍 + 回放 + checkpoint 截断）、**MVCC**（时间戳数组 `BlockVersion`）、**`MetaStore` trait + `fjall` 实现**（原子批写 / 快照读 / 前缀扫描 / 刷盘） | ✅ **M1 内核** |
| `infinity-catalog` | 表元数据 + 持久化单调 ID 计数器 + 名称索引 | ✅ 最小可用 |
| `infinity-txn` | 提交管线 `WriteWAL(串行) → KV 原子写` + 幂等 WAL 回放 | ✅ 最小可用 |
| `infinity-parser` | 手写 lexer + 递归下降（`CREATE/DROP TABLE`、`INSERT`（**多行**）、`DELETE … [WHERE]`、`UPDATE … SET … [WHERE]`、`SELECT` 含 `WHERE`/`JOIN…ON`/`GROUP BY`/`ORDER BY`/`LIMIT`/投影/聚合）；**通用表达式层**（函数调用、`AND/OR/NOT` 谓词树、列间比较）；**数组/稀疏/集合字面量**与参数化类型；**`SEARCH MATCH …`（VECTOR/SPARSE/TENSOR/TEXT + FUSION）**；`DROP … [IF EXISTS]`、`COPY`、`SHOW`、`EXPLAIN`、`SET/RESTORE/ALTER/DUMP` 容错 | ✅ M2+ |
| `infinity-expression` | 标量 `Expr`/`Value` + 比较 + **布尔连通** + **函数求值**（`ROUND/SQRT/ABS/…`）+ **聚合求值**（含 `FIRST/LAST`） | ✅ M2+ |
| `infinity-planner` | 逻辑计划（`Scan`/`Join`/`Filter`/`Projection`/`Aggregate`/`Sort`/`Limit`/`Values`）+ 多表列解析（歧义检测）+ **optimizer**（limit/谓词下推、常量折叠）+ 谓词/表达式编译（含 `SEARCH` 伪列映射） | ✅ M2 |
| `infinity-executor` | `Engine` + **Tokio「按边并发」有界管线**（scan 生产 ↔ 算子消费，背压不阻塞；**join/聚合/排序为阻塞算子**）+ **DELETE/UPDATE**（行墓碑 + 可见性过滤；**update = delete+append**）+ **`COPY FROM/TO`（CSV 装载/导出，fixture 路径映射）** + **`SEARCH MATCH VECTOR`**（精确暴力检索：`l2/ip/cosine/hamming`、`threshold`、`WHERE` 下推；伪列 `ROW_ID/SIMILARITY/DISTANCE/SCORE`） | ✅ 最小可用 |
| `infinity-index` | **二级索引**（字节键 multimap：等值/范围/lookup + 持久化）、**向量索引**（`FlatIndex` 精确 + `HnswIndex` 近似，含 recall 测试）、**全文倒排**（BM25 AND 查询）；**已接入 `CREATE INDEX` + 等值索引扫描（重启后从元数据重建）** | ✅ **M3** |
| `infinity-server` | **Thrift `TBinaryProtocol` 前端**：方法分发器 + `Connect`/`Disconnect`/`ListDatabase`/`CreateTable`/`Insert`/`Delete`/`Update`/`ShowTable`/`Select`/异常；列式 `SelectResponse`；有状态 TCP 服务器；**HTTP 前端种子**；**PG wire**；DDL/DML/DQL/Meta 经 Thrift 跑通；跨语言校验 13 项 | ✅ **M4** |
| `infinity-cli` | `infinity` 驱动（create-table / list-tables / put / get / recover / **query** / smoke） | ✅ |
| function / InfinityService 全量 RPC / HTTP·PG 前端 / 磁盘驻留索引 | 完整查询引擎、索引族与协议 | ⬜ M4+ |

**已验证**（2026-10-15）：
- `cargo test --workspace` **89 用例全绿**（common 1 / storage 15 / catalog 4 / txn 5 / parser 17 / expression 6 / planner 8 / index 10 / executor 14 / server 8）；`clippy --all-targets` 干净、`cargo fmt --check` 干净。
- **M5 起步**：①并发提交压力测试（4 线程 × 25 事务）；②**崩溃注入**：WAL 20+ 截断点 → 恢复恰好有效前缀；③**基准**：多行 `INSERT` + 批量提交把 2000 行插入从 371 rows/s 提升到 **≈7551 rows/s**（~20×）；④**Loom 并发模型**：`RUSTFLAGS="--cfg loom" cargo test -p infinity-txn --lib loom` 验证 `commit_ts` 分配在任意交错下**唯一无缺口**。
- **M6 起步**：`infinity import-csv <dir> <table> <file>`——从 CSV 表头建表（全 VARCHAR）并**批量导入**（每 100 行一个事务），导入行可经 `SELECT` 查询。
- **Gate A 基线**（`infinity conformance <test/sql>`，把现有 `.slt` 直接跑在 Rust 引擎上）：**174 文件**、`statement ok` **1752/1752**、`statement error` **129/129**、`query` **1536/1536**（2026-10-17，场景按**容器内自建的 C++ 引擎真值**校准后）。**Gate A 100% 达成**。
  - **评审口径修正（2026-10-17）**：harness 原先把每行当**整串精确比较**并**跳过首部空白**，与本仓库 CI 实际使用的 `sqllogictest-rs v0.28.4` 语义不符。已按参考实现对齐：① `default_normalizer` = **trim + 折叠空白**（`split_ascii_whitespace().join(" ")`）；② 比较**默认按行序（NoSort）**，仅 `query … rowsort` 记录才排序。对齐后 EXPLAIN 期望文本里**跨文件不一致的前导缩进**（` PROJECT` vs `PROJECT`）不再影响判定，`query` 由「宽松口径」的 1398 变为**忠实口径**的 1387——这是**度量修正**而非能力回退（同 revision 两种口径背靠背可比）。
  - **里程碑轨迹**：`statement ok` **404 → 760 → 1696 → 1745/1746**、`query` **71 → 136 → 1146 → 1192 → 1218 → 1239 → 1254 → 1259 → 1271 → 1273 → 1279/1532**。本轮（2026-10-16）新增能力：①**通用表达式层**（`ExprAst`：列/字面量/函数调用/**算术 `+ - * / %`（含 `*`，带词法歧义消解）**/**`CAST`/`TRY_CAST`**）+ 布尔谓词树（`AND/OR/NOT`、括号、列间比较、`IS [NOT] NULL`、`LIKE`、**`[NOT] IN (…)`（数值隐式转换）**）；②**函数/聚合扩容**（`ROUND/SQRT/ABS/CEIL/FLOOR/UPPER/LOWER/LENGTH`、`FIRST/LAST`、**`json_extract*`/`json_contains`/`json_exists_path`**（自研 JSON 解析 + 路径 + 紧凑序列化）、**`MD5`**（自研 RFC 1321）、**`REGEX(col, pattern)`**（自研正则 + 表达式 pattern）；未知函数→`NULL`）；③**数组/稀疏/集合字面量**（`[…]`、`[i:v]`、`{…}`、嵌套 `[[…],[…]]`）、参数化类型、`DATE/TIMESTAMP '…'` 与 `TRUE/FALSE` 字面量；④**`SEARCH MATCH` 端到端**：`VECTOR`（`l2/ip/cosine`）+ **`SPARSE`（内积）** + **`TENSOR`（MaxSim；`TensorArray` 逐张量取 max；`bit` 按命中位求内积）**，均支持 `WITH (threshold=…)`、`WHERE`（外层与括号内）**先过滤后 top-N**、`USING INDEX`，伪列 `ROW_ID()/SIMILARITY()/DISTANCE()/SCORE()`；**张量精度按列/查询的声明元素类型（`float/float16/bfloat16/double`）量化后再算**（f16 就近偶数舍入、bf16 截断，与 C++ `MatchTensor` 对齐）；⑤**`COPY … FROM`** 真实 CSV 装载 / `COPY … TO` 导出（fixture 路径映射、`DELIMITER '\t'`、向量/张量规范化）；⑥**DDL/DML**：`DEFAULT`/`NOT NULL`（校验）、`ALTER TABLE ADD/DROP COLUMN`、`INSERT` 列清单对齐 + 缺列补默认值、`DROP TABLE/INDEX [IF EXISTS]`、`SELECT … AS`、**常量 `SELECT`（无 FROM）**、`CREATE SNAPSHOT`/`ALTER <index>`/`SET/RESTORE/COMPACT/DUMP` 容错、**聚合表达式两阶段求值**（`Aggregate` → `Projection`，支持 `MAX(c1)+SUM(c2)`）、**`SELECT UNNEST(json_array)`** 展开；⑦**NULL 渲染统一为 `null`**（与 `.slt` 字面量一致）。
  - **一致性硬伤修正**：conformance harness 原先会**整条丢弃**以 `#` 注释开头的记录（导致注释紧邻的 `COPY` 未执行），已改为**逐行剔除注释**（记录总数由 1270→1532）；新增**按文件失败归因**输出，并提供**单文件定位**（`infinity conformance <path>.slt`），便于聚焦迭代。
  - **本轮新增能力（2026-10-16 续）**：张量 MaxSim 按列/查询声明元素类型量化（f16 就近偶数、bf16 截断、f32 累加；`bit` 按命中位内积）；整型向量 `l2` 与所有类型一样报**平方距离**（无开方）；新增 `hamming` 度量、`EMBEDDING(BIT,d)` 位串存储与打分；`ROUND(x)`/`ROUND(x,n)`（保留类型/浮点、支持负数位）、`TRUNC(x,n)`；`INTERVAL <n> UNIT` 与 `EXTRACT(<field> FROM <expr>)`（含日历月/年与秒进位）；`UNNEST` 支持 `ARRAY` 大括号字面量与 JSON（对象/标量取自身、JSON null 出一行 `null`）；`json_contains` 仅对数组顶层匹配；`SHOW TABLE … COLUMNS` 四段式（名/类型/默认值/注释）+ `describe_type` 嵌套参数（`Array(Embedding(float,1024))`）+ 稀疏元素名（`INT→int32`）；`SPARSE(BIT)` 索引列表与 `(empty)`；`COPY … TO`（可写路径解析、`OFFSET/LIMIT`、JSONL）与 `COPY … FROM … FORMAT JSONL`；`INSERT` 张量维度校验；**多库支持**（`USE`/`CREATE|DROP DATABASE`/`db.table` 限定名、`DROP DATABASE` 连带清表）；聚合 SELECT 列表恒投影（保留重复聚合与列序）。
  - **本轮新增能力（2026-10-16 三轮）**：`SEARCH MATCH TEXT` 先按词元命中集合选行（`minimum_should_match` 支持 `n`/`p%`/负值，`topn`），命中行再投影（`query` 1326→1331）；`SELECT *` 不再输出检索伪列（`#_row_id/#_similarity/#_distance/#_score`，但 `ROW_ID()/SCORE()` 等仍可按名引用），修复 `ft_minimum_should_match` 等 `SELECT *` 溢出列（+6）；`COPY`/`INSERT` 写入 `DATE/TIME/DATETIME/TIMESTAMP` 时复用解析器的 `normalize_datetime` 做**规范化**（`2024-1-22`→`2024-01-22`、`0:3:55`→`00:03:55`、`1970/5/6 11:45:01`→`1970-05-06 11:45:01`），修复 `test_import_time`；`json_contains` 对**显式加引号**目标（`'"1"'`）只按 JSON 字符串匹配、不再回退到数字紧凑文本（修复 `test_json_index`）；`COPY … FROM … FORMAT JSON` 解析**整个 JSON 数组**（而非按行），修复 `test_import_json`（1326→1341）。
  - **本轮新增能力（2026-10-16 四轮）**：`SEARCH MATCH` 目标列去掉 `^boost` 后缀并把 `doctitle,body^5` 逗号列当作多列（`match_columns`），支持 `WITH (analyzer='whitespace')` 的空白分词（记住每个 `CREATE INDEX … WITH (analyzer=…)`，`whitespace` 保留标点为一个词元）；`filter_fulltext('col@index', …)` 支持 `@索引` 提示（剥离后定位列）；`minimum_should_match` 改按 Lucene 规范（`n<value`、负值/百分比、末条生效）；`ROW_ID()` 在普通扫描里按行序号返回；新增 `infinity-server` 可执行（Thrift 前端的启动器）。`query` 1341→**1368**（并行迭代中，数值持续上升）。
  - **本轮新增能力（2026-10-16 五轮）**：`filter_text`/`filter_fulltext` 默认算子改为 **or**（`operator=and` 或 `minimum_should_match` 才收紧），并把 Lucene `minimum_should_match` 规范（`[n<]value`，`n`/`n%`/负值/百分比）实现为 `infinity_expression::minimum_should_match` 供过滤与检索共用（`filter_fulltext_function_minimum_should_match` 8→13）；`SEARCH MATCH TEXT` 支持 `WITH (analyzer='whitespace-<chars>')` 的「空白 + 指定分隔符」分词（`fulltext_whitespace_sharp` 1→3）；`CREATE TABLE` 的整型元素数组/向量**默认值**在写入时即规范化（`embedding(int,3) default [1.3,4.1,33.7]`→`[1,4,34]`，`test_import_default`）；整型元素取整改为**四舍五入**。`query` 1358→**1368**。
  - **本轮新增能力（2026-10-16 六轮）**：`COPY … FROM` 对**任意分隔符**（含 `\t`）按 RFC 4180 解析引号（引号仅在字段首字符才开启，避免把正文里的裸 `"` 当引号），修复 tab 分隔夹具里被引号包裹的向量列（`"[0, 0, 0, 0]"`→`[0,0,0,0]`），使 `enwiki_embedding` 上的 `MATCH VECTOR` 恢复（`fusion.slt` 19→14、整体 +7）；多 `MATCH` + `FUSION(…)` 返回各子检索**并集**（按行序去重），修复 `test_agg_search`/`drop_column_with_index` 等（+3）。`query` 1368→**1378**。
  - **本轮新增能力（2026-10-17）**：①**harness 忠实化**（见上，对齐 `sqllogictest-rs v0.28.4`）；②**`EXPLAIN` 搜索计划**（新 `explain_search` 模块）：单 `MATCH` 检索计划的 `PROJECT [-> LIMIT] -> MATCH | KNN SCAN | MatchTensorScan | MatchSparseScan`，含 `index filter`/`leftover filter` 的**索引列（`#<表>.<列>`）与残余列（顺序 id）拆分**（`explain_fusion` 6→4，`result_cache` 各 `cached_*` 的 EXPLAIN 记录转绿）；③**`EXPLAIN LOGICAL`**（parser 新增 `ExplainKind`）：逻辑计划的 `PROJECT -> TABLE SCAN`（属性名写作 `output columns`）、**`INTEGER` 列遇 `BIGINT` 字面量的算术提升**（`CAST(c1 (#0) AS BigInt) + 1`）与 `DELETE … -> FILTER -> TABLE SCAN`（`column_pruner` 0→6）；④**`INDEX SCAN` 搜索计划**（`explain_index_scan`）：按顶层 `AND` 拆分可下推谓词（全部列有索引、无 `NOT`/`!=`）下沉为 `INDEX SCAN` 的 `filter`，其余留作 `FILTER`，索引 filter 用限定列号 `#1.<列>` 并支持 `json_extract_*` / `json_contains` / `json_exists_path` / `FILTER_FULLTEXT(…)` 形式，比较按列类型决定是否 `CAST(... AS BigInt)`（整型提升、字符/日期不提升；比较运算仅在**对手方是 INT 字面量**时才给列加 `CAST`）；⑤**`EXPLAIN LOGICAL` 聚合**：`AVG` 展开为 `sum/count`、聚合参数按表达式解析（`MIN(c1 + 1)` 的双括号与 `CAST` 提升）；⑥**排序语义修正**：`ASC` 下 `NULL` 排在非空之前（`NULLS FIRST`）；**`ORDER BY <表达式>`** 真正求值（`OrderKey` 携带完整表达式，planner 编译为 `Expr` 键、executor 逐行求值），修复 `fp16_bf16`/`sort_by_function`/`explain_sort`。`query` 提升 **1387 → 1466**（续做：`EXPLAIN` 融合计划 `FUSION`、物理 `COUNT(*)` 聚合、`EXPLAIN LOGICAL` 连接与点号限定列名、索引 filter 中全文谓词不加括号、`ROW_ID()` 排序键与逐节点输出列、混合类型排序全序、**多段 `MERGE` 计划**（按表的写入批次数触发 `MERGE MatchTensor`/`MERGE MatchSparse`/`Merge aggregate`））。`explain/**` 目录现 **26/26**。
  - **仍待收窄（当前仅剩 `query`）**：`statement ok` 1752/1752、`statement error` 129/129 均已 100%；`query` **1449/1536（94.3%）**（2026-10-17 复测），失败用例经抽样后仍集中在下面几类：
    1. **全文/FUSION 打分（≈120 条）**：**公式已复刻并落地**（`score = weight·idf·(k1+1)tf/(tf+k1(1-b+b·len/avg))`，`idf=ln(1+(N-df+0.5)/(df+0.5))`，k1=1.2/b=0.75；`src/storage/invertedindex/search/bm25_ranker_impl.cpp:25-34`），且 **`fulltext_delete` 已 7/7 逐位命中**（含删除后统计仍计入的语义）。残余差异锁定在**统计口径是段级而非全表**：实测 `ft_minimum_should_match` 中 d2、d4 命中同一词集，但反解出的 idf 之和分别为 **1.1630 / 1.2680**（不相容）→ 说明两者取自不同段（CSV 段 vs INSERT 段）的 `df/total_df/avg_len`。**负面结论（2026-10-17）**：已实现「按写批次分段」的 BM25 段级统计（每段独立 N/df/avg_len），实测**净负**（1468→1460，`fulltext_minimum_should_match` 13→8），已回退——说明参考实现的「段」**不等于**写批次边界（iros 的段来自 flush/commit 生命周期）。另尝试修正 FUSION `weighted_sum` 符号与短语邻接，均**净零**，一并回退。故该缺口必须在真正移植 iros 段模型后才可能收敛。**落地路径**：随任务 #71（C++ 倒排格式 + 段）引入段级统计；`ft_keyword` 的恒定 `1.0/4.5` 亦落在 `analyzer=keyword` 的段/词表行为上，一并由该任务解决。
    2. **`EXPLAIN` 计划文本（约 50 条）**：期望文本中的 `(node_id)` 由 C++ 优化器的**规则改写顺序**决定（例如存在二级索引时 `-> INDEX SCAN (6)` 而 `PROJECT (4)`），并包含按「列裁剪」结果生成的 `output_columns` 列表（如 `output columns: [y, __rowid]` 逐节点变化）。
       更棘手的是**同一计划在不同文件里的文本并不一致**：`explain.slt` 的期望以 1 个空格开头（` PROJECT (3)`），而 `filter_fulltext_function.slt`/`column_pruner.slt` 的同形计划从第 0 列开始（`PROJECT (4)`）；节点 id 也随之不同。
       由于 harness 只裁掉行尾空白，行首空格必须逐字节复刻——这只能来自与 C++ 相同的打印器与优化器状态机，**结论：EXPLAIN 需移植该打印器**，暂不可达。
  - **全文打分调查（2026-10-16 首轮负结果 → 已推进 → 2026-10-17 再澄清 → 2026-10-17 用 C++ 真值收窄）**：以下为首轮结论，其中「公式未知」部分**已被推翻**（BM25 公式已复刻并逐位命中单段用例；剩余为段级统计，见上）：以 `test/sql/dql/fulltext/fulltext_keyword.slt` 反推 `SCORE()` 模型——`doc^4.5` + `'multiple another'` 对 1/2 个命中词均给 **4.5**（疑似按字段封顶），而 `doc2` + `'second default text'`（`similarity=boolean`）对 1/2 个命中词给 **1.0 / 2.0**（疑似命中词数×boost）。两者互相矛盾；默认相似度下按 BM25（k1=1.2，见 `src/planner/bound_select_statement_impl.cpp:237`）手算 0.34，与实测 1.0 不符；`analyzer=keyword` 的分词边界与 block-max/WAND 影响亦无法从测试反推。**结论：必须逐行移植 iresearch 的相似度/分词（R1）才能对齐打分，本阶段不做近似**。已实现的是**非打分**全文路径：`filter_text`/`filter_fulltext` 词元匹配、`regex(col,pat)`（自研正则引擎）、`SEARCH MATCH TEXT` 的解析（不含打分排序）。**2026-10-17 追加澄清（重要，避免重复踩坑）**：①`analyzer=keyword` 实际就是 `WhitespaceAnalyzer`（按空白切词，`src/common/analyzer/analyzer_pool_impl.cpp:341-346`），不是整串 token；②默认相似度确为 `kBM25`（`bound_select_statement_impl.cpp:225-235`），布尔仅显式 `similarity=boolean` 时启用；③**该 slt 的 4.5 不可能是 BM25**：N=5、df=2 时 `idf=ln(1+3.5/2.5)=0.8755`（≠1），故 `idf·tf_norm·4.5 ≠ 4.5`；而期望值恒等于 **boost**（4.5 / 1.0）且与命中词数无关（row5 命中 2 词仍 4.5）→ 等价于「按字段 OR 去重 × boost」，与显式 `similarity=boolean` 的「按词求和」（2.0 / 10.5）不是同一模式。
  - **2026-10-17 用 `tools/bm25_probe/gen.py`（把 `BM25Ranker` 的**逐字**函数体单独编译）把该文件收窄到**一个数**：对 5 文档集（`wc-1` = 1,2,3,3,1、`doc_cnt` = 5），期望的 `row1/row2/row3 = 0.3661/1.1863/0.2448` 在 **`N=5`、`df=2/4`、`len=wc-1`、`avg_column_len = 2.10`** 下**同时精确命中**（数值搜索最优解 err 0.00011，相对 0.01%）；`avg=2.0`（`Σ(wc-1)/doc_cnt=10/5`）只给 0.3616/1.1632/0.2388（各差 ~1%）。**故**：①此前「段级统计」假设**已被推翻**——`idf/df` 是整表口径（与我们实现一致）；②`len` 口径 `wc-1` 与我们的实现一致；③**唯一不符的是 `avg_column_len`**：源码是 `term_cnt/doc_cnt`（`term_doc_iterator_impl.cpp:61-65`，`term_cnt_ += column_length_sum`，后者是 `ColumnInverter::InvertColumn` 返回的每 doc `terms_once_->size()` 之和，`column_inverter_impl.cpp:64-79`），按此该 5 文档应为 **2.0**，而期望值要求 **2.10**（即 `Σlen` 需为 10.5）。**结论**：源码直线路径解释不了这 0.5。对 `N∈[3,9] × df_text × df_second × avg∈[1.00,4.00]` 的**穷举搜索**只找到一个（唯一）精确解：**N=5、df_text=4、df_second=2、avg=2.10、len=(1,2,3)**（err 0.00011），没有任何 `avg=2.0` 的 (N,df) 组合能拟合。既然源码在完全相同的 5 文档/Σlen=10 上只能得到 2.0，而期望值要求 2.10，**最可能是该 `.slt` 由不同 revision（其 `avg_column_len` 口径不同）生成**（与 `fulltext_keyword.slt` 同类），其次是 `SEARCH` 路径的 `avg` 取自 `column_length_io` 读的 **`chunk_info` JSON 里记录的 `doc_cnt/term_cnt`**（催化字段，非内存统计）。**下一步**：核对该 JSON 记录值（catalog 的 C++ 键值编码已可读写，便于检查），或对 `cached_fulltext` 用 probe 反解 avg 看是否同样“非整数和”；**在定论前不要改我们的 `avg_len`**（改错会拖累已通过的 `fulltext_delete` 等）。
- **FUSION 组合算法已复刻（2026-10-17，`488b17b68`）**：`SEARCH` 的多个 `MATCH` 由 `FUSION(...)` 组合；此前实现是「子检索并集去重」。现按 `physical_fusion_impl.cpp` 的 `ExecuteRRFWeighted` 复刻：`rrf` 按排名 `Σ 1/(rank_constant+rank)`（默认 60）；`weighted_sum`/`max` 按每子权重与归一化 `none|minmax（默认）|l2|atan` 组合，且对**距离型**子（L2/Hamming 的向量扫描）在 `none` 下取负、其它归一化下取 `1-s`（`KnnExpression::IsKnnMinHeap`：L2/Hamming=false，IP/Cosine 与全文/张量/稀疏=true）；`topn` 取 `topn`/`window_size`（默认 100）；外层 `WHERE` 下沉进**每个**子检索的排序（此前只对向量子生效）。**校验**：`fusion.slt` 里 `weighted_sum` 的向量分量在 `normalize=none`（`num=1` → `-4`）、`minmax`（`0.75`）、`l2`（`0.7575`）、`atan`（`0.0780`）与 `max`/`rrf` 上**逐位命中**；**整表仍不过**，因为融合结果取决于**上面未解的全文打分**（本机 BM25 文本分与夹具不符，会改变文本子的排名与数值）。Gate A `query` 1466→1467。**`FUSION('match_tensor', …)` 已补齐**（`97ecd8bdd`：按 `PhysicalFusion::ExecuteMatchTensor` —— 各子检索**只贡献行集**（按 row id 去重取并集），再用 `search_tensor` 选项对 `column_name` 列**重排**（复用 `tensor_score`，故位向量/张量数组列与 `MATCH TENSOR` 行为一致），取 `topn`；此前该分支被错误地当作 weighted_sum；`fusion_rerank_maxsim` 7→3、`fusion_rerank_tensorarray_maxsim` 4→2，**Gate A +6**）。
- **`rank_features` 打分已复刻（2026-10-17，`32d806a50`）**：`SEARCH MATCH TEXT` 的 `rank_features=<col>^<tag>^<weight>,…` 选项——`<col>` 列（用 `analyzer=rankfeatures` 索引）存 `[{"<tag>": <number>},…]`，每个列出的 tag 给分数加 `number * weight`（缺该 tag 记 0）。`rank_features.slt` 三行由「恒等 0.2671」变为 `6.2671/5.2671/4.2671`，转绿。Gate A `query` 1467→1468。
- **全文过滤器的索引语义（2026-10-17，`c41939c82`/`bf98950a6`）**：裸的 `filter_text(...)`/`filter_fulltext(...)` 现在按**它需要哪个索引**来解析——① `threshold=` 是**分数**过滤，BM25 需要全表统计量（行内谓词拿不到），故识别后**一次过**用与 `MATCH TEXT` **同一套打分**（其内部已应用 threshold）求出合格 row id，再**按表序**下推（`fulltext_minimum_should_match` 5→3、`fulltext_keyword` 2→1）；② `col@index` 会把索引名解析到其 analyzer，若该索引是 **sparsegram**，过滤器需要查询**连续出现**（各 n-gram 都在文中）＝**子串语义**，而非普通 analyzer 的词元 AND——为此**analyzer 也按索引名登记**（同一列可同时挂 gram 索引与普通索引，按列记会被后者覆盖），`filter_regex_multi_index` 转绿。**Gate A 1487→1491**。
- **派生表 `FROM (SELECT …)`（2026-10-17，`3f693288e`）**：parser 在 `FROM` 后遇到 `(` 时把**子查询的 SQL 文本**（字符串字面量重新加引号）存进 `from`，执行器先跑它、用其列做**合成表**以便列名解析、再把结果行作为扫描源（`derived_table` 把 `QueryResult` 的文本单元格还原成 `Value`）；子查询文本**不做库限定**；识别要求 `SELECT` + 空格，避免把名为 `select3` 的真表当作子查询（我这一版先把 `select3` 误判，导致 6 条回退，收紧后修复）。**Gate A 1491→1492**。
- **BM25 长度口径的**新** A/B（2026-10-17，revision `c39d1db5a`，**推翻旧结论**）**：旧的「四种组合」A/B 是在并行会话「keyword/whitespace 字段记常量 boost」之前测的，已失效。本轮重测：把 `len` 与 `avg` 同时从 `wc-1` 改成 `wc`（即 `dl=词数`、`avg=mean(词数)`），**query 1492→1487（-5）**，逐文件看是 `fulltext_delete` **0→7**、`fusion_rerank_maxsim` 2→1、`fusion_rerank_tensorarray_maxsim` 1→0——即 `fusion_rerank_*` 两族要 `dl=wc`，而 `fulltext_delete` 要 `dl=wc-1`。两条推导都已算到闭式（前者 `dl22/dl77=2`、`avg=1.25=mean(词数)`；后者 `dl3/dl1=3`、`avg=2=mean(词数-1)`，且 `Σ` 与 `doc_cnt` 均自洽），**同一分词器不可能对 `'first text'` 给 1 个词、对 `'tell off'` 给 2 个词**。结论不变：这批 fixture 跨 revision 生成，须用 C++ 引擎取真值；**当前保留 `wc-1` 口径**（全局更优）。
- **Gate A 已 100%（2026-10-17）**：`query` **1536/1536**。全文打分与 C++ **逐位一致**；`HIGHLIGHT`（`<em>` 标注 + 只保留命中句 + ` ... ` 连接 + `SentenceSplitter` 正则）与 `EXPLAIN FRAGMENT`（fragment 标题/SINK/PROJECT/FUSION 链/SOURCE、`FRAGMENT #n` 子片段列表、节点编号 = 叶子 2,3,… → FUSION 按应用序 → PROJECT，sink/source = project+3/+4，子片段各再占 2 个 id，以及每个片段后的空行）均已复刻。其余（派生表、sparsegram/threshold 过滤、`match_tensor` 融合、多字段打分、短语、`^` 权重、bm25 delta、`col@index`、ROW_ID 段编码）均已关闭。
- **C++ 引擎在本机**是**可以**构建的（2026-10-17 更正，旧结论作废）**：不必在本机 bootstrap vcpkg —— `.github/workflows/tests.yml` 用的就是**预构建 builder 镜像**。可复现步骤：① `docker run -d --name infinity_builder -v <repo>:/infinity infiniflow/infinity-x64-x64-v2-builder:latest sleep infinity`（镜像已含 clang-20、`VCPKG_ROOT=/opt/vcpkg` 与全部依赖）；② `docker exec … cp /infinity/.github/workflows/x64-x64-libstdc++.modules.json /x86_64-linux-gnu/lib/gcc/x86_64-linux-gnu/15.2.0/libstdc++.modules.json`（否则 configure 报 `libstdc++.modules.json` 缺失）；③ `cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja -S /infinity -B /infinity/cmake-build-debug`；④ `cmake --build /infinity/cmake-build-debug --target infinity`；⑤ `./cmake-build-debug/src/infinity -f conf/pytest_parallel_infinity_conf.toml`（PG 线 5432，SDK 23817，HTTP 23820），用 `psql -h 127.0.0.1 -p 5432 -U admin -d default_db -f q.sql` 直接跑 SQL。拉镜像需要代理（`HTTP_PROXY=http://127.0.0.1:7890`）。
- **用 C++ 真值校准后的全text打分结论（2026-10-17，逐位对齐）**：
  - **分词器是字节表**（`tokenizer_impl.cpp`）：ASCII 字母数字与 **>127 的字节**都算 `ALLOW`（UTF-8 整段留在词内，`—` 不切词），ASCII 空白是 `SPACE`，其余每个字节是 **单字节 `DELIMITER` 词元并被丢弃**；`/` 配置成 `UNITE`，扫描时**跳过并继续当前词**，但只作用于**首字节之后**（故 `2012/02/14`→`20120214`、`x/y`→`xy`、`www.example.com/path`→`www|example|compath`，而 `a_b`/`a.b`/`a@b#c$d`/`1,234` 都切分）。
  - **索引侧同时写原词与词干**（`common_analyzer_impl.cpp`）：词干与原词**同位置**，`stem != word` 时两者都发；`doc_len` 是发出去的词元数。五条探针表实测 `doc_cnt=5 / doc_len_sum=21 / avg=4.2`、每篇长度 `2,4,6,5,4`（原词数 `2,3,4,4,2`），据此 BM25 复现 `.slt` 的 `1.1863/1.0791/0.3661/0.2448` **逐位一致**。**查询侧只发词干**（`second text multiple` 的 boolean 分是 3 不是 4）。
  - **BM25**（`term_doc_iterator_impl.cpp`）：`idf=ln1p((N-df+0.5)/(df+0.5))`、`score=idf*(k1+1)*tf/(tf+k1*(1-b+b*dl/avg))`，`k1=1.2,b=0.75`；`bm25_param_delta*` 等价于给每个命中单元加 `idf*delta`（`f4=delta/(k1+1)`，`bm25_param_delta` 同时设 term/phrase）。
  - **短语**：按位置连续匹配，得分 = `(Σidf) * tf_norm(phrase_freq)`（Lucene `PhraseWeight`）。
  - **多字段**：`doctitle,body^5` 每个字段**各自一套** `doc_len/avg/df` 后再相加；`col@index` 只是选索引。
  - **ROW_ID**：`segment<<32|offset`，`COPY` 封段、其后的 `INSERT` 开新段、连续 `INSERT` 追加同段（两次 99 行 COPY → `0…98` 与 `4294967296…`；两次单行 INSERT → `0,1`）。
  - enwiki 实测：99 篇 `doc_len` **逐篇相同**、`doc_len_sum=18475` 一致，`harmful chemical` 两边都是 `22.2996`。
- **Gate C 证据刷新（2026-10-17，同一 revision）**：`infinity conformance test/sql/snapshot` = **10 文件 / statement ok 171/171 / statement error 8/8**（与计划表一致）；`infinity smoke <dir>` = **OK**（建表→写入→WAL 回放→重启后读回 3 行）。
- **跨语言 wire 验证（十三项）**：①`Connect`；②`list<ColumnField>`；③`Select` REPLY；④`CreateTable`；⑤`Select *`；⑥`Insert`；⑦`Select` 列投影；⑧`Select` 过滤；⑨`Select` `ORDER BY…LIMIT`；⑩`ListDatabase`；⑪`Delete`；⑫`Update`；⑬**`ShowTable`**（`cols=2 rows=2`）。全部返回 `PY_*_OK`。
- **Gate B–D 证据（2026-10-17，真实 SDK 对跑）**：`cargo build -p infinity-server` 产出 `target/release/infinity-server`（`INFINITY_BIND`/data-dir 可配），启动后**未改动的 Python SDK**（`python/infinity_sdk`）可 `infinity.connect(127.0.0.1:23817)` 成功；`uv run python -m pytest python/test_pysdk` 全量汇总（2026-10-17 复跑）**1448 passed / 212 failed / 598 errors / 7 skipped**（2259 选中；初始仅 361 passed，上一轮 1031 passed）。598 个 error 仍以夹具目录 `/var/infinity` 权限为主，212 个 fail 为 engine 侧未实现语义（如 `AddColumns` 重复列名应报 `DUPLICATE_COLUMN_NAME`）。关键增量：补上 `GetDatabase`/`CreateDatabase`/`DropDatabase`/`GetTable`/`DropTable`/`CreateIndex`/`DropIndex`（`CommonResponse`，含 `ConflictType` 处理）与 `Insert`/`Select`（列式 `SelectResponse`，`select_response_from`）的派发，此前这些一律返回 `unknown method`。剩余失败/错误多为 SDK 断言的前景特性缺失与少量环境项（夹具目录 `/var/infinity` 权限）。Gate C 的 `test/sql/snapshot` **171/171 + 8/8**、workspace 测试全绿；Gate D 的 loom（`commit_ts` 唯一性）通过，**miri 已在 `nightly-2025-04-04` 上跑通**（parser/expression/planner 共 **38** 个用例，0 失败）；**2026-10-17 复跑**：`uv run python -m pytest -q python/parallel_test`（对同一 Rust 服务）**16 passed / 2 failed**；两个失败为 `test_insert_parallel`/`test_insert_and_count_star_parallel`——**根因是并发 INSERT 的行号冲突**（8 个会话都用 `scan_prefix().len()` 当 base，互相覆盖，20000 行只剩 5341）。**已修复**：按表原子预留行号块（`reserve_rows`，INSERT/COPY 共用；`ALTER DROP COLUMN` 重新编号与 `DROP TABLE` 时重置计数器），`test_insert_parallel.py` 复跑 **3 passed**（2001s），Gate D 并发面全绿；`python/restart_test` 的 `InfinityRunner` 以 `<binary> --config=…` 启动并写 `/var/infinity`，需为 Rust 服务适配 CLI 且当前环境 `/var/infinity` 不可写，故未产出结果。Gate G 的跨底座宕机一致性缺口 **G3 已闭合**（`infinity-storage/tests/crash_consistency.rs` 5 用例）。**结论：Gate B–D 均「部分达成」**，继续推进需补齐剩余 Thrift RPC 面、对象存储对照与与 C++ golden 的可比硬件基准。
  - **HTTP 前端（2026-10-17）**：`infinity-server` 现在同时启动 **HTTP** 监听（`INFINITY_HTTP_BIND`，默认 `:23820`），`GET /health` → `ok`、`POST /query`（`{"sql":…}`）→ `{"columns":…,"rows":…}`（实测 `SELECT 1+1` → `{"columns":["c0"],"rows":[["2"]]}`），与 Thrift 共用同一 `Engine`。新增 `ShowCurrentNode`/`ListSnapshots` 两个 RPC（实测：`show_current_node()` → `node_role='leader'`、`list_snapshots()` → `snapshots=[]`）。
  - **Go SDK 对跑 + 已知阻塞（2026-10-17）**：`go test ./tests/` 亦可连上 Rust 服务（无需改 SDK）；补入真实 `ListDatabase`（此前恒返回 `["default_db"]`）、`ShowDatabase`/`ListTable`/`ListIndex`/`ShowColumns` 与 `CreateIndex`（解析嵌套 `IndexInfo` 并真正建索引）后，`TestConnection`、`TestBasicDatabaseOperations`、`TestConnectionPool*`、`TestBasic`、`TestBasicIndexOperations`、`TestDelete*` 等由失败/报错转为**通过**（`go test -v` 在跑到重负载用例前已 **43 PASS**）。
  - **`connection reset` 根因与修复（2026-10-17）**：Go 客户端在 `DROP INDEX`/`SELECT` 前会出现 `connection reset`。定位为 **Thrift 请求体未读满**导致流错位（下一个 `read_message_begin` 读到 `BadVersion`）——两处：①`read_create_index_args` 少了 `<Method>_args` 的收尾（`end_args`）；②`read_select_item` 对嵌套表达式结构体用 `ip.skip` 会**少消费**，改为逐字段显式消费。另加「处理出错回 Thrift 异常而非断连」的兜底。修复后索引与「Insert→Select（输出表达式）」路径均正常。
  - **仍挂住**：`TestDeleteTableWithOneBlock` 等重负载用例（8192 行级 insert/delete）在当前实现下超时 → `go test ./tests/` 会挂住；需要定位是慢还是死锁。**另**：`127.0.0.1:23817` 上若有**残留旧 server**，新实例会 `AddrInUse` 静默退出、旧进程继续应答，务必先释放端口。
- **wire 反同步修复 + 向量列端到端（2026-10-17）**：定位到 Gate B/C 长期残留的**根因**——每个 `read_*_args` 只读到内层 `<Method>Request` 就返回，**未消费外层 `<Method>_args` 的 `stop` 字节**，于是同一连接上的**下一个**报文整体前移 1 字节（服务端 `read_message_begin` 报 `received bad version: [0, 128]` 后 RST）。后果：未改动的 SDK 在**几乎每次 RPC 前**都命中 `TTransportException` → `_reconnect()` 重连重试（即日志刷屏的 `Tried 0 times … unexpected exception`），而 `disconnect()` 内部自行吞掉异常，故直接暴露 `CLIENT_CLOSE`（6003）。**修复**：新增 `end_args()` 并让所有 `read_*_args` 收尾（`infinity-server/src/lib.rs`），回归用例 `args_readers_consume_the_enclosing_struct` 逐字节断言读完后缓冲为空。修复后：同连接连续 `ListDatabase` 正常、`disconnect()` 返回 OK、重连抖动消失（`tests::*` 9 用例全绿）。
  - 同批：`CreateTable` 不再把所有列压成 `VARCHAR`（改用 `logic_type` + `physical_type.embedding_type`）；`read_column_def` 解析 `physical_type`；`Select` 响应对标量列写 SDK 的 `LogicType`（`column_type_to_logic`）、对向量列写 `Embedding(12)` 并用 `SHOW TABLE … COLUMNS` 取声明类型文本；`ValueLit` 新增整型/浮点数组、张量与稀疏，使 `INSERT` 真正携带向量值。**SDK 级实测**：`vector,4,float` / `vector,3,float16` / `vector,3,int8` 三种向量列经未改动的 SDK `insert` → `output(['*']).to_pl()` **逐元素回读一致**（`List(Float64)` / `List(Int64)`），带向量列的投影与过滤正常。
- **非标量/SDK 列类型端到端（2026-10-17 续）**：`classify_column` 按声明类型文本把结果列分派为「标量 / 向量 / 稀疏 / 张量」，`ColumnDefWire.physical` 由 `Option<(dim,elem)>` 泛化为 `PhysicalWire::{None,Embedding,Sparse}`（写 `EmbeddingType` 或 `SparseType`）。经**未改动的官方 SDK** 逐一实测（均在私有端口、`insert` → `output(['*']).to_pl()` 回读断言）：
  - **向量**：`vector,4,float` / `vector,3,float16` / `vector,3,int8` → `List(Float64)`/`List(Int64)`，逐元素一致。
  - **稀疏**：`sparse,100,float,int8`，插入 `{0:1.5,3:2.5}` → SDK 解出 `SparseVector`（`{'0':1.5,'3':2.5}`）；空稀疏发 `nnz=0` 前缀。
  - **日期时间/JSON**：`DATE`→i32 纪元天数、`TIME`→i32 当日秒、`DATETIME`→(i32 天数,i32 秒)、`JSON`→与 Varchar 同的长度前缀 UTF-8；`null`（任意大小写）置零并置**假**位掩码，JSON 的 NULL 解出 `None`（注：SDK 的 `parse_date_bytes` 不消费位图，故 NULL 日期显示为 `1970-01-01`，属 SDK 侧行为）。
  - **张量/张量数组**：`Tensor(FLOAT,3)`、`TensorArray(FLOAT,3)` 解出 `List(List(Float64))` / `List(List(List(Float64)))`，逐元素一致；`INSERT` 的 `ConstantExpr` 字段 10/11（`i64/f64_tensor_array_value`）与三级 `list<list<list<T>>>` 读取补齐，`ValueLit` 增加 `Int/FloatTensorArray`。**多向量** `MULTIVECTOR(FLOAT,3)` 复用张量路径，`[[1,2,3],[4,5,6]]` 回读一致。
  - **数组（含嵌套）**：`PhysicalWire::Array` 携带元素自身的 `DataType`（写/读整棵 `DataType` 树，`array_type{1: element_data_type}`）；打包为 `u32 count` + 逐元素按其 `ColumnType` 编解码，并按声明文本推导**每层**的 `ColumnType` 递归（`array_levels`）。`ValueLit` 新增 `Brackets` 承载 SDK 的 `Array(...)` 包装（`literal_type=18` + 字段 13），渲染为 `{…}`。经未改动的 SDK 实测：`array,int`→`[1,2,3]`、`array,varchar`→`['ab','cd']`、`array,array,array,int`→`[[[1,2],[3,4]],[[5,6]],[[]],[]]`（与 pysdk `test_insert.py::_test_insert_array` 期望完全一致）。
  - 为使张量参数可见，`describe_type` 现在渲染 `Tensor/TensorArray/MultiVector(<elem>,<dim>)`（此前只输出裸 `Tensor`）。Gate A 无回归。
  - **环境限制（诚实记录）**：本机存在**并行会话**，双方都以 `127.0.0.1:23817` 为 SDK 测试端口；我方 `serve` 多次被对方占用/回收（`Address already in use` 或进程被 SIGKILL），故**全量 `python/test_pysdk` 计数本轮无法刷新**（两次试跑分别得 442/1639 与 395/1817，均在服务端被杀后失真，不作为证据）。B 关卡结论仍以**逐类型 SDK 往返 + 干净 `disconnect()`** 为准。
- **有界容量=1 不做死锁**（50 行扫描 + 早停 `LIMIT`，容量 1 下正常返回）——PoC A 结论落进真实执行器。
- `infinity smoke`：建表 → 提交 KV → 重启 → 元数据持久化 → WAL 回放 → 数据恢复。
- `infinity query` 端到端：
  - `SELECT id,name FROM t WHERE id>=2 LIMIT 1` → `2|b`
  - `SELECT name, COUNT(*) FROM t GROUP BY name ORDER BY name` → `a|2, b|1, c|1`
  - `SELECT COUNT(*), SUM(id), AVG(id), MIN(id), MAX(id) FROM t` → `4, 10, 2.500000, 1, 4`
  - **`SELECT aid, bn FROM a JOIN b ON aid = bid`** → `2|yy`；`… WHERE … ORDER BY …` 亦可
  - 重启后数据持久；列名随表持久化（`["id","name"]`）。

**下一步（M5 续）**：Loom 并发模型测试（WAL/通道/取消状态机）、崩溃注入压测、基准；并行推进 `InfinityService` 的 Show/Delete/Update RPC；随后 M6 发布/迁移（`infinity-import`）。

---

## 6. 验收标准（Acceptance Criteria）

> 核心口径：**以现有 sqllogictest + Python SDK + Go SDK 测试用例为准**（用户要求），并补充持久化/并发/性能关卡。
> **统一原则（codex 第 1 轮 #10）**：所有门槛以**冻结后的执行清单**为准——冻结「收集到的 test ID + marker/skip + 数据集 + 生成的 `.slt` manifest」，报告**执行/通过/跳过**总数，而非引用易变的静态数字。

### Gate 现状（2026-10-17 本地实测，可复现）

> **2026-10-17 复验证据**：`cargo test --workspace` 全绿（各 crate 0 failed）；`test/sql/snapshot` = `statement ok` 171/171、`statement error` 8/8、`query` n/a；`RUSTFLAGS="--cfg loom" cargo test -p infinity-txn --lib loom` 通过。

| 关卡 | 本次实测结果 | 结论 |
|---|---|---|
| **A** | `statement ok` **1752/1752（100%）**、`statement error` **129/129（100%）**、`query` **1536/1536（100%）**（2026-10-17 复测） | **达标**：全文/FUSION/EXPLAIN 全部对齐。关键在于**容器内构建 C++ 引擎取真值**（`docs/CPP_FORMAT_SPEC.md` 的 recipe）——真因是 `standard` 分析器**同时索引 token 与 stem**、tokenizer 按字节分类（`/` UNITE、>127 不切），据此重写后 enwiki 99/99 文档 `doc_len` 与 `doc_len_sum=18475` 逐项相同、`harmful chemical` 两边都是 22.2996。详见 `rust_engine/scripts/README.md` 的 §Gate A 复盘 |
| **B** | **未修改的官方 SDK 直连 Rust 服务**：`python/test_pysdk` 逐文件全绿（`test_knn` 373 passed / 6 skipped、`test_index` 961、`test_select` 66、`test_delete` 377、`test_insert` 51、`test_show` 24 等）；**Go SDK `go/tests` 375 个 `Test*`（31 文件）全绿**，`ok … 359.558s`；HTTP `--http` 面（`test_select` 60、`test_convert` 13、`test_groupby` 3、`test_alter` 5、`test_database` 21、`test_secondary_index` 5、`test_knn` 373/0/6）亦全绿。夹具要求：`/var/infinity/test_data` 需扁平化的 `test/data/{csv,json,jsonl,fvecs}`。 | **基本达标**：Thrift（Python/Go）双 SDK 全绿；**MinIO 对象存储未实现** |
| **C** | `python/restart_test` 未改一行即可驱动 Rust 二进制：`test_basic` 1、`test_drop` 1、`test_alter` 6、`test_cleanup` 10、`test_compact` 3、`test_database_snapshot_restart` 1、`test_snapshot` 7、`test_system_snapshot_restart` 3、**`test_memidx` 10/10**、**`test_fulltext` 6/6**（需本地链接 `enwiki-10w.csv` 夹具）、**`test_insert` 11/11**（25 min）。`test_insert_import`/`test_multiple_index_types_import` 为百万级导入 + 反复 kill 的长用例（见下）。 | **基本达标**：恢复/快照/索引 chunk 生命周期均实测；**MinIO 未实现**（配置项被忽略） |
| **D** | loom `commit_ts` 唯一性 → `test result: ok`；`miri` 通过（38 用例）；**ASan 与 TSAN 各 38 用例通过（`run_gate_d.sh` 新增步骤）**；`python/parallel_test` 缺 `infinity` 模块 | **部分（Rust 侧已足）**：核心 crate 有 loom + miri + ASan + TSAN 四项证据；SDK 侧（parallel_test）仍缺 |
| **E** | `run_gate_e.sh` → **PASS**：Rust 基准驱动输出 `Total cost : <t> s`，脚本直接以位置参数调用 `check_benchmark_result.py`（<7%）；无 C++ golden 时以**自基线**（两次运行互比）判定。insert ~72–80k rows/s、select 2000 行 ~3.9 ms | **部分**：驱动与门限已打通；已用 CI builder 镜像构建同版本 `RelWithDebInfo` C++ 并完成同机 A/B（insert 1.4×、读路径 7–20× 慢），仍缺 CI golden log 工件做 <7% 判定 |
| **F** | `cargo fmt --check`、`clippy --all-targets -D warnings`、`cargo test --workspace` 全绿；`cargo deny check`（cargo-deny **0.20.2**，`deny.toml` schema v2）→ **advisories ok, bans ok, licenses ok, sources ok** | **达标（本地）** |
| **G** | `docs/INVARIANT_MATRIX.md`：G1–G5 缺口 | **已闭合**：G1–G5 全部有回归测试（`config_validation`、`index_file_family`（索引文件族契约，随 #68/#70/#71 的磁盘格式落地而补上）、`crash_consistency`、`fulltext_score`、以及 G4 的 miri+ASan+TSAN（各 38 用例，`run_gate_d.sh`））。**限制**：sanitizer 只覆盖纯 Rust 核心（带 FFI/SIMD 的 crate 依赖 MSRV 高于 pinned nightly）。 |

> 复现：`cd rust_engine && ./scripts/run_gate_{a,b,c,d,e}.sh`（B/C/D 需 SDK 与外部服务，脚本会明确报 `SKIP/MISSING`）。

### Gate A — SQL 功能（硬门槛）
- **全部 `test/sql/**/*.slt` 通过**（内联 `----` 字面量比对，`rowsort` 等修饰符语义一致）；无静默放宽、无 skip 新增。
- 关键点：浮点格式化（如 `ROUND(SCORE(),4)`）、`rowsort` 排序稳定、`statement error`（仅需报错）、`explain` 计划文本结构（含 operator id/内部列，见 Gate D 与 §附录 C）。

### Gate B — SDK 协议兼容（硬门槛，核心验收面）
- **`python/test_pysdk` 全部用例通过**（连 23817，未修改 SDK）。
- **`go/tests` 全部用例通过**（未修改 SDK）。**已达成（2026-10-18）**：`cd go/tests && GOPROXY=off go test -count=1 ./...` → `ok github.com/infiniflow/infinity-go-sdk/tests 359.558s`，**31 个测试文件 / 375 个 `Test*` 全部通过**（bwrap 私有 netns + `/tmp/gbfix` 作为 `/var/infinity/test_data`；首次跑出的 24 个 Import 失败全部是夹具路径问题——该目录需扁平化放 CSV/JSON/FVECS，补齐 `pysdk_test.{csv,json,jsonl}`、`test_empty.*` 后归零）。
- **HTTP 模式（`--http`）全部用例通过**（`tools/run_http_api.py`，与 CI 一致）。
- 前置：Thrift 线兼容（`TBinaryProtocol` + `client_version==37` + **逐类型二进制布局 spec**）；**跨语言 golden 报文**测试通过。

- **Gate B 全绿（2026-10-17 深夜复测）**：`test_show::test_show_big_databases`（10000 库 × 建/查/删）在无并行构建争抢 CPU 时 **147 s 通过**；此前测得的 55–96 ms/次是并行会话跑 C++ 构建时的 CPU 争抢，单次 DDL 实际约 1 ms（`list_databases` 0.1 ms）。**`python/test_pysdk` 逐文件全部通过**。
- **Gate B 收口（2026-10-17 深夜）**：`test_knn.py` **373 passed / 0 failed / 6 skipped**（`test_multivector_bit` 的两个用例修掉：位向量打包字节不能先钳到 0/1，MAXSIM 用逐行最小汉明距离求和，bit multivector 按 bit 行解析），`test_rank_features.py` 4/4（`rank_features=` 现在要求目标列有 `rankfeatures` 全文索引）。至此除 `test_show.py::test_show_big_databases`（3 万次 DDL × ~55–96 ms 的**性能**问题，非死锁）外，**`python/test_pysdk` 逐文件全绿**。Gate A 仍为 1752/1752 + 129/129 + 1536/1536。
- **Gate C 进展（2026-10-17 深夜，真实 restart harness）**：CLI 支持 `infinity --config=<toml>`（从 `persistence_dir` 推导数据根、`INFINITY_BIND` 覆盖监听地址），因此 `python/restart_test` 不改一行即可驱动 Rust 二进制。实测（bwrap、私有 netns、`conf/pytest_parallel_infinity_conf.toml`）：`test_basic` 1/1、`test_drop` 1/1、`test_alter` **6/6**、`test_cleanup` **10/10**、`test_compact` **3/3**、`test_database_snapshot_restart` **1/1**、`test_snapshot` **7/7**、`test_system_snapshot_restart` **3/3**（`test_shutdown_pytest` 被 harness 自身 skip）。其余长跑文件：`test_memidx` **8/10**（`test_mem_hnsw_cos` 已修——补齐 `[wal] checkpoint_interval` 的周期落盘后通过；余 `test_mem_hnsw` 重启后的名次/并列顺序 `[6,6,4,4,4,4]` vs 期望 `[8,6,6,4,4,4]`，以及 `test_optimize_from_different_database` 断言磁盘上的索引目录数量）、`test_fulltext` **3 个用例是缺少夹具** `test/data/benchmark/enwiki/enwiki-10w.csv`（仓库未附带该 10w 文件），另外 3 个通过；`test_insert`/`test_insert_import`/`test_multiple_index_types_import` 属“插入过程中反复 kill -9”的长用例，420 s 上限内未跑完，需要更长预算（2026-10-18 复跑：`test_insert.py` **11/11**，耗时 25 分 13 秒；此前报出的失败全部是缺 `test/data/benchmark/sift_1m/sift_base.fvecs` 夹具——`.gitignore` 忽略、CI 预置，本地符号链接后消失）。过程中修掉三处恢复语义：①WAL 重放改为从 `meta:applied_ts` 开始（此前 `recover(0)` 会把 `DROP`/恢复之后已删的行重新写回）；②快照 blob 的索引列表改用换行分隔（用 SNAP_REC 时第二个索引会被当成数据行）；③`CREATE SYSTEM SNAPSHOT`/`scope=system` 真正实现，`DROP DATABASE default_db` 带 `IF EXISTS` 为 no-op（仅普通 drop 返回 3033）。
- **Gate C 收口（2026-10-17 深夜复核）**：`test_memidx` **10/10**、`test_fulltext` **6/6**，且 `test_basic`/`test_drop`/`test_alter`（6）/`test_cleanup`（10）/`test_compact`（3）同批复跑 **30/30** 无回归。本轮修掉：①重启后把**持久化 HNSW 图**当成候选全集，丢掉了“落盘之后新插入”的行（`[6,6,4,4,4,4]` vs `[8,6,6,4,4,4]`，行其实已恢复、只是向量没进候选）——现在仅当图覆盖全部存活行时才用它剪枝，否则退回全表扫描（`executor` 的 `vector_candidates`）；②补齐内存索引的 chunk 生命周期：每次 dump 追加一个 `chunk_{n}.idx`、`optimize` 写合并 chunk、`cleanup` 只留最新，`[buffer] memindex_memory_quota` 与 `[storage] optimize_interval` 从配置生效（`memindex_memory_quota` 在写入前按 `dim*4+256`/行估算），`Optimize` RPC 真正执行；③数据库目录号改为 C++ 的 1-based（`default_db` = `db_1`）；④**索引名按表作用域**——C++ 允许两张表各建一个同名 `ft_index`，此前全局 key 会互相覆盖，这正是 `test_fulltext` 重启后报 3053 的根因，`DROP INDEX`/`DROP TABLE`/`ALTER`/`RENAME` 现均按表删除；⑤`test_fulltext` 的三个用例依赖夹具 `test/data/benchmark/enwiki/enwiki-10w.csv`（被 `.gitignore` 忽略，CI 侧预先放置；本机以符号链接接入后 **6/6**）。索引名作用域改动后 `python/test_pysdk/test_index.py` 复跑 **961/961**。
- **Gate C 存储放大残项（2026-10-18）**：`test_multiple_index_types_import` 的 5 个档里 **2 个通过、3 个重档失败**（25/5/0 线程 × 120 s），原因是**容量**而非语义：250k 行 ×（2048 维向量 + 1024 维多向量 + 稀疏 + 文本）在本实现中是**文本行存**，实测 `persistence` **12 GB**、WAL 单批 ~450 MB；本机 `/` 仅剩 22 GB、内存 30 GB，跑到第 10 分钟报 `No space left on device`（`/var/infinity` 落在 tmpfs 时则是 OOM，峰值 RSS 11.4 GB + tmpfs 11 GB）。本轮为收敛做的改动：①**WAL 在 checkpoint 按 `applied_ts` 前缀流式截断**（实测 2.29 GB → 183 MB；`meta:applied_ts` 仍是恢复依据，启动时用 `max(WAL, applied_ts)` 续接时间戳）；②**C++ block 镜像按表 4 GiB 预算封顶**（镜像只是互操作副本，不为超大表再复制一份）；③point UPDATE/DELETE 走二级索引 + 行号预留，不再整表物化（顺带修掉 `dml/update.slt` 里 UPDATE 后索引陈旧的问题）；④只有 SECONDARY/SECONDARYFUNCTIONAL 才物化字符串二级索引（向量列不再建 ~20 KB/行的 key）。要在本机跑完这 3 个用例需 ≥30 GB 磁盘 + 更大内存，或改用列存——这正是 Gate E 读路径 7–20× 差距的同一根因。**MinIO 仍是缺口**：`storage_type = "minio"` 只出现在 `test/data/config/test_minio_s3_storage.toml`，由 C++ `test_main` 的 SLOW 单测消费；Rust 侧没有等价用例，引擎也**未实现对象存储后端**（配置项被忽略，数据仍落本地盘），对齐需新增 S3 block/WAL 存储层。
- **Gate F 全绿（2026-10-18）**：升级到 **cargo-deny 0.20.2**（本机 registry 缓存里已有 `.crate`，`cargo install --offline --locked` 即可），并把 `deny.toml` 的 `[advisories]` 迁到 **schema v2**（`version = 2`、去掉 `vulnerability`、`unmaintained = "workspace"`、`yanked = "deny"`）。`CARGO_NET_OFFLINE=true cargo deny check` → **advisories ok, bans ok, licenses ok, sources ok**（仅剩“白名单里若干许可证本轮未命中”的 warning）。此前 0.14.24 无法解析 RustSec 的 **CVSS 4.0** 条目（`unsupported CVSS version: 4.0`）的问题随升级消失。
- **Gate E 证据（2026-10-17）**：`infinity bench <dir> 20000` → **insert 20000 行 / 600.6 ms（33,300 行/s）、select 20000 行 / 129.8 ms**。与 C++ 的对照：本机 `cmake-build-debug/src/infinity` 是 **Debug（-O0）**构建，不能作为 <7% 门槛的基线；需用 CI builder 镜像的 Release 构建在同机复测。
- **Gate E 同机 A/B（2026-10-18，C++ 0.7.4 `RelWithDebInfo` 基线已构建）**：用 CI builder 镜像 `infiniflow/infinity-x64-x64-v2-builder:latest` 按 `tests.yml:940` 的同一套 cmake 参数（`clang-20` + vcpkg `x64-linux-v2` + `RelWithDebInfo`，产物 `cmake-build-reldeb/src/infinity`）。**踩坑**：容器默认 8 MB 栈会让 clang-20.1.8 在 `-O2` 构建两个 implicit module PCM 时 **SIGSEGV**（`hnsw_handler_impl.cpp`、`physical_match_sparse_scan_impl.cpp`，报 `clang frontend command failed due to signal`）；`ulimit -s unlimited` 后一次通过（多次 `-j4` 重跑仍复现，可排除 OOM）。同一份**未修改的 SDK** 脚本（bwrap 私有 netns、`conf/pytest_parallel_infinity_conf.toml`、20000 行 × 1000/批、各跑两轮）结果（C++ / Rust）：**insert 65,606、64,132 / 44,659、46,870 行/s（Rust 约为 C++ 的 68–73%）**；`count(*)` 2.0、2.2 / 27.8、28.2 ms；`SELECT id,name` 20000 行 7.7、7.8 / 55.5、58.8 ms；主键点查 1.3、1.2 / 27.3、37.6 ms。**结论**：写路径已在同一量级（1.4×），读路径慢 7–20×，根因是行存按行解码 + `count(*)`/点查走全表扫描（C++ 有列存与行数统计）——这是 Gate E 的明确残项；官方 <7% 门限仍需 CI 的 golden log 工件（仓库和 `yzc/benchmark_golden` 分支都没有该日志），本机已有同版本 A/B 基线可复跑。
- **Gate B 进展（2026-10-17 晚，逐文件对跑，bwrap 隔离）**：已全绿的文件（每个文件单独起服务、单独数据目录）：`test_py_import` 234/234、`test_index` **961/961**、`test_select` **66/66**、`test_groupby` 3/3、`test_insert` 51/51、`test_limit` 2/2、`test_null` 6/6、`test_database` 22/22、`test_delete` 377/377、`test_compact` 1/1、`test_convert` 28/28、`test_explain`/`test_config`/`test_alter`/`test_export`/`test_basic`/`test_condition`/`test_connection_pool`/`test_infinity` 全绿。本轮补的关键 RPC 面：`GetDatabase` 存在性（3021）、`DROP DATABASE default_db`→3033、重复库名→3016、`Select` 解码错误走 `SelectResponse`（3050/7012）、`Delete` 错误码（3022）、`Compact` 真正合并段、`CreateIndex` 完整解码（`IndexInfo{column,index_type,params,function_expr}` + `CreateOption` + comment）、`SearchExpr`（MATCH TEXT/VECTOR/SPARSE/TENSOR + FUSION）与 `HIGHLIGHT`、`HAVING`/`OFFSET`/`total_hits_count`、投影别名（含 WHERE/ORDER BY 中的别名替换）、`total_hits_count` 回 `extra_result`。列式编码补齐 TINYINT/SMALLINT/FLOAT16/BFLOAT16。错误码表新增 3009/3016/3023/3033/3050/3053/3061/3077。**尚未全绿**：`test_knn.py`（本轮重跑中）。Gate A 无回归：`statement ok 1752/1752`、`statement error 129/129`、`query 1536/1536`。
- **Gate B 进展（2026-10-17 早先一轮）**：`Import` 与 `Explain` 两个 RPC 已实现（此前 `Import` 直接返回 OK、`Explain` 返回 `unknown method`），错误统一按 C++ `ErrorCode` 回（`Error::status_code`：7012 PARSER_ERROR / 3013 / 3014 / 3021–3024），`BOOLEAN` 列的非 true/false 值按 C++ 行为报 `parser: Boolean type should be true or false`。`test_py_import.py` 由 56 failed 降到 **11 failed / 223 passed**，`test_explain.py` 通过。整目录对跑 = 1382 passed / 202 failed / 675 errors（含 `/var/infinity` 权限噪音；用 bwrap `--tmpfs /var --dir /var/infinity` 后该类 ERROR 消失）。**跑法见 `~/.codebuddy/.../memory/reference_pysdk_rust_server.md`：务必 `--unshare-net` 起独立 loopback，避免与并行会话争 23817。** 剩余大头是 `AddColumns/DropColumns`、`Export`、`SetConfig` 等 RPC 与各类负例的具体错误码。

### Gate B — HTTP API 进展（2026-10-17 晚）
- 新增 REST 前端（`crates/infinity-server/src/http.rs`，手写 JSON 解析 + 路由，无新依赖），与 Thrift 共用同一 `Engine`：
  - `POST/DELETE/GET /databases[/{db}]`、`/databases/{db}/tables[/{t}]`、`/columns`、`/indexes[/{i}]`
  - `POST/PUT/DELETE/GET /databases/{db}/tables/{t}/docs`（插入/更新/删除/查询 DSL：`output/filter/sort/group_by/having/limit/offset/highlight/search/fusion/option.total_hits_count`）
  - 冲突选项（create/drop）按 SDK 常量校验（非法→3074），标识符须以字母开头（3003）
- 实测（`pytest --http`，未改 SDK）：`test_database + test_table + test_basic + test_select` 合计 **103 passed / 12 failed / 3 skipped**（`test_select` 53/66，`test_table` 26/27，`test_database` 22/22，`test_basic` 通过）。
- Thrift `test_knn` 由 51 failed 收敛到 **12 failed / 361 passed / 6 skipped**：本轮修复向量元素类型校验、bit 维度按 8 打包、`threshold=` 不被误认作索引名、MULTIVECTOR 的 MAXSIM、伪列命名/类型（SCORE/DISTANCE/SIMILARITY/ROW_ID）、`HAVING`/`OFFSET`/别名替换等。
- 已实现路由：databases/tables/columns/indexes/docs、import（PUT table→COPY FROM）/export（GET table/{t}→COPY TO）/rename/compact/optimize/segments/blocks/add-drop columns/EXPLAIN meta；标识符统一小写（3003/3004）、create/drop 选项 3074/3075、`replace_if_exists`→3032、空列清单 3048、重复表 3017、缺表 3022。
- 已知缺口：HTTP 下 JSON 函数/日期函数的列名与类型推断（`json_extract_*` 值仍按字符串）、`test_select_truncate`/`test_select_aggregate` 的数值渲染、快照路由、`after_disconnect` 用例（新连接走 Thrift 的混用场景）。
- **Gate B — HTTP 逐文件复测（2026-10-17 晚，最新 revision）**：`test_select` **60 passed / 0 failed**（3 个 ERROR 仍是 `/var/infinity` 夹具权限）、`test_convert` **13 passed / 0 failed**、`test_groupby` **3/3**、`test_alter` 5/5、`test_database` 21/21、`test_secondary_index` 5/5；`test_knn`（真 HTTP 模式：`--http` + **服务端与 pytest 同处一个 `bwrap` 沙箱**，使 `/var/infinity/test_data` 与 COPY 共享同一文件系统）**373 passed / 0 failed / 6 skipped**（此前报告的 341/32 实为误连 Thrift 端口，已作废）。（`test_with_fulltext_match_with_valid_columns` / `test_match_text_index_hints` 由“MATCH 级 `filter=` 选项作用于候选 + 与 WHERE 互斥”修复；`test_multivector_bit` 由位向量打包字节与逐行最小汉明 MAXSIM 修复。）本轮修：嵌套 tensor 查询（`sql_value_arr` 递归，`[0,0]` 曾让每行得分 0）、TENSOR vs TENSORARRAY 语义按列声明类型判定、tensor topn 位置参数、`params` 为字符串/标量时的校验、`index_name`/`ignore_index` 提示与冲突（SYNTAX_ERROR）、topn 类型/范围、查询 element_type 校验、伪列名 `SIMILARITY`/`DISTANCE`/`SCORE`/`ROW_ID`、MULTIVECTOR 的 HNSW 镜像不再 panic。**方法论修正：所有 HTTP 计数必须同时满足（a）传 `--http`、（b）服务端与测试同沙箱**，否则 `infinity.connect()` 会静默走 Thrift 打到 23817，得到与 Rust HTTP 无关的结果。本轮修复：`ADD COLUMN` 的数值/JSON 默认值与空串≠NULL（bitmap）、`_row_id` 在 output/sort/filter 的真实隐藏列语义、无类型表达式的结果类型（`trunc`/JSON 函数）与参考列名（`Cast(...)`、`FILTER_FULLTEXT(...)`、`(-(ABS(...)))`、`count(star)`、`(sum(x) / count(x))`）、int 列插入小数按截断、`SELECT … AS alias` 落到输出列、`AND/OR/NOT` 操作数必须是布尔（`WHERE c1` 也拒绝）。Gate A 无回归：`statement ok 1752/1752`、`statement error 129/129`、`query 1536/1536`。

### Gate B — 性能根因（2026-10-17，关键发现）
- 现象：Go SDK 全量跑到 8192 次单行 INSERT 时"卡死"（120s/600s 超时），`test_show`/`test_table`/`test_update`/`test_knn` 在 420s 上限内跑不完，单行 INSERT 实测 **43 ms/次**。
- 根因：**服务端未设置 TCP_NODELAY**，小请求/小回复触发 Nagle + delayed ACK（Linux 40 ms 定时器）。
- 修复：`infinity-server`/HTTP/PG 三个监听在 accept 后 `set_nodelay(true)`；同一条单行 INSERT 从 **43 ms → 0.072 ms**（~600×）。原先"慢/超时"的失败项（Go 全量、多文件 420s 上限）需要用新二进制重新测量，不能再引用旧结论。

### Gate B — Go SDK 复测（2026-10-17 晚，TCP_NODELAY + 协议修复后）
- `cd go && go test ./tests/`（连 Rust 服务，未改 SDK）：**353 passed / 20 failed，~2min**（此前 58 个顶层失败后 600s 超时、840s panic）。本轮再修：`COPY … FROM` 改为 **8k 行分块流式读取 + 分批提交**（此前 326 MB / 8.4M 行夹具整file读入导致 OOM，服务端被杀后整批用例连锁失败——这是 44 个失败里的绝大多数）；`CreateTableSnapshot/DropSnapshot/ListSnapshots` 真正执行；数据库名 3003 校验（含空名）；`TestImportExceedingRows` 由杀进程变为 50 s 通过。
- 本轮修的协议层不变量：`ShowSegments/ShowBlocks/ShowColumns` 必须用各自的方法名回复（thrift-go 会校验，名字不对既报错又让连接错位→后续 `Disconnect` 挂死）；空 `alias_name` 不是别名（`SELECT count(*) AS`）；空的 `IndexInfo.function_expr` 不是函数索引（Go 会发空结构体，曾让全部 CreateIndex 失败）；`GetTable` 缺表返回 3022；建表列名非法 3005/3044、空列名 3042；标识符大小写不敏感（`qualify_tables` 与 server 的存在性检查都转小写）。
- 剩余的 44 个失败里绝大多数是**同一个根因**：`TestImportExceedingRows` 导入 326 MB / 8.4M 行 CSV 时进程被 OOM 杀掉，随后整批用例连锁失败（"EOF"/连接拒绝）。**下一步：COPY 改成分块流式读取 + 分批提交**（当前整文件读入内存），即可解除这批连锁失败。
- 方法论修正：单文件/单用例的"超时"不再当作功能缺失；每次测量必须（a）绑定夹具目录、(b) 检查服务端存活、(c) 以失败签名分组，而不是按用例名逐个修。
- **Import RPC 分隔符口径（2026-10-17 晚）**：Go 侧 `&infinity.ImportOption{CopyFileType: …}` 是零值结构体，`Delimiter rune` 为 0 → `string(0)` 是**单个 NUL 字节**，不是空串。C++ 侧把 `delimiter[0]` 直接交给 zsv，而 `zsv_scanner_init` 有 `opts->delimiter = opts->delimiter ? opts->delimiter : ','`，即 NUL 回退成逗号。Rust 之前把 NUL 当真实分隔符，整行只有一列 → 0 行导入 → `Import` 返回 OK，于是 `TestImportEmbeddingWithNotMatchDefinition`（期望维度不匹配报错）失败、`…CompatibleDefinition` 假通过。修法：RPC 层取 `delimiter[0]`、NUL/空回退 `,`（与 C++ 一致），执行器 `decode_delimiter` 同样把 NUL 与非法分隔符（`\n`/`\r`/`"`）回退成 `,`。修后 `go test -run 'TestImport…'` 全绿（含 FVECS/JSON/JSONL/空文件/10000 列/8.4M 行）。
- **Gate B — Go SDK 已全绿（2026-10-17 夜）**：`go test ./tests/ -count=1`（未改 SDK，私有端口 23900 + 独立数据目录）**374 passed / 0 failed，542 秒**；不含本轮临时诊断用例的基线为 **373/373**。最后三项的根因：① `array,int` 列的 `SelectResponse` 里元素 `DataType` 缺少 `physical_type`，Go 端 `parseSingleArrayElement` 见到 `PhysicalType == nil` 直接返回空数组并把元素字节当长度反复解析（36 行垃圾）→ 新增 `PhysicalWire::Scalar(NumberType/VarcharType)`；② tensor 的 `topn` 只解析不校验（`topn=-1/0/100000000` 全放行）→ 按 C++ 的 u32×100 上限校验并取**最后一个** `topn=`（C++ 是 map 覆盖）；③ SDK 把 `filter("uc2 > 1")` 发成 `WHERE unnest(c2) > 1`，而谓词在展开后的行上求值 → 把谓词/HAVING/GROUP BY 里的 `UNNEST(col)` 改写成展开列的别名。
- **Gate B — Python `test_knn` 已全绿（2026-10-17 夜）**：`pytest test_knn.py -q` **373 passed / 6 skipped / 0 failed，709 秒**（此前 16 failed）。`test_show` **24 passed / 1 skipped**、`test_update` **48 passed**、`test_table` **27 passed**（修：`CreateOption.conflict_type` 的 Ignore 为幂等成功、Replace 返回 3032、空列 3048、列名小写化）。
- **`test_table_snapshot::test_multithread_snapshot_with_modifications` 的阻塞根因（2026-10-17 深夜，已定位为 SDK 侧共享 socket 竞态，不是引擎死锁）**：最小复现（`/tmp/conc2.py`，对 release 服务）：单线程 200 次 INSERT + 20 次 create/drop index **2 秒完成**；**两个线程各用一条连接**同样 **2 秒完成**；**两个线程共用一条连接**时第一个请求就双双报 `Insert failed: unknown result` / `CreateIndex failed: unknown result`，计数停在 0，服务端 0% CPU（空闲）。根因在官方 SDK：`ThriftInfinityClient` 用 `rwlock.RWLockRead().gen_rlock()`（**读者锁可共享**）包住每次调用，而所有线程共用同一个 `TBufferedTransport`——并发 `readMessageBegin` 会互相截断对方的回复，`flush()` 里 `getvalue()` 与缓冲区替换之间排队的写还会被丢弃。C++ 侧同样存在该竞态，只是它更慢、窗口更小。引擎侧能做的都做了（单连接内严格顺序应答、每次回复一次 `write_all`、`set_nodelay`）；要稳定通过只能让 SDK 使用独占锁或每线程一条连接。测试侧的唯一规避：把该用例与并发用例分离，或在该用例上给每条线程单独 `connect()`。


### Gate B — Go SDK 复测（2026-10-17 深夜，Rust 服务 + 私有 netns）
- `go test -c ./tests` 后在同一 `bwrap --unshare-net` 沙箱内启动 Rust 服务（23817）并运行：**`-test.v` 全量 373 PASS / 0 FAIL，整包 PASS**。
- 关键修复：`index_name` + `ignore_index=true` 此前在 **解码阶段**直接返回错误，导致 `SelectRequest` 剩余字段未消费、回包后下一帧解析失败而断开（Go 报 `Connection is closed`）。改为解码阶段不报错、把两个提示一并写进 `MATCH VECTOR` 的选项尾，由执行器在**请求已完整消费后**返回 3013。
- 跑法：`go test -c -o /tmp/go_tests.test ./tests`；沙箱内 `INFINITY_BIND=127.0.0.1:23817`，并把 `test/data/**` 扁平复制到 `/var/infinity/test_data/`（Go 用例按该路径取夹具）。

### Gate C — 持久化与恢复
- **Gate C 复验（2026-10-17 夜，未改 `python/restart_test`，仅把端口改成私有 23910、把 runner 里「kill 掉所有名字含 infinity 的进程」收窄为只 kill 自己启的那个实例，以免误杀并行会话）**：`test_basic` 1/1、`test_alter` **6/6**、`test_drop` 1/1、`test_compact` 3/3、`test_cleanup` 10/10、`test_database_snapshot_restart` 1/1、`test_system_snapshot_restart` 3/3、`test_snapshot` **6/7**。`test_snapshot` 唯一失败 `test_snapshot_large_data` 首次报 `invalid embedding data type 'l2'`，单独重跑 **1 passed**，属偶发（同文件其余 6 例含 `test_snapshot_basic_restart` 均过，此前记录的「恢复后被 WAL 复活 5 行」已不复现）。
- **环境坑（记录）**：`/tmp` 与根盘共 205 GB，历史上 100+ 个 `gb*-data` 测试数据目录把它占满（100%），大导入（311 MB CSV→8.4M 行）因此报 `No space left on device` 并让服务端被杀、后续用例连锁连接失败。**测量前必须 `df -h /` 并让数据目录落在 `/home`**（本机 `/home` 651 GB）。
- **最新进展（2026-10-17 晚，`python/restart_test`，`bwrap --unshare-pid --unshare-net`）**：`test_basic`/`test_alter`（5）/`test_cleanup`（含 index 清理）/`test_compact`（含 repeated）/`test_database_snapshot_restart`/`test_drop`/`test_infinity` 等 **20+ 用例通过**；剩余失败集中在 `test_fulltext`/`test_insert` 的 `enwiki-10w` 数据集缺失（`test/data/benchmark/enwiki/` 不存在，属夹具而非引擎）。本轮修复：列 id 跨 `DROP COLUMN` 稳定（`0.col`/`2.col`，不再复用）、索引 id 1-based 且跨 `DROP INDEX` 稳定、`idx_{id}` 文本索引镜像（`.dic`/`.pos`/`.len`）、表 id 从 0 起、`DROP TABLE` 删除镜像目录、compaction 经 WAL（回放不再复活重编号的行）、行 id 分配跳过 tombstone、`Import` 消费外层 args 的 stop（此前每次导入断流重连）、`Cleanup` 落到 flush、数据库快照 create/restore（`CreateDatabaseSnapshot` + `RestoreSnapshot scope="database"`）、server 数据布局对齐夹具（`catalog`/`data`/`persistence`/`wal` 都在 `clear()` 覆盖范围内）。
- `python/restart_test` 通过；`test/sql/snapshot` 通过；**WAL 崩溃恢复**用例通过。
- **对象存储 / MinIO** 用例通过（对应 CI 的 minio job）。
- **compaction 后恢复**与**恢复期间再次崩溃**用例通过（针对跨底座一致性契约）。

### Gate D — 并发与稳定性
- `python/parallel_test` 通过；`test_cluster` 仅在「集群纳入范围」时作为门槛（见 §5 范围冻结）。
- Rust 侧（**替代不可验证的「cargo test 无数据竞争」表述**）：
  - **有界 `loom` 模型**：覆盖调度/通道/取消的核心状态机（容量=1、早停、错误、取消）。
  - **`miri`**：跑**不依赖 native/FFI** 的纯 Rust 核心单测（明确排除 FFI 依赖项）。
  - **native sanitizer**：对含 FFI/SIMD 的模块保留 TSAN/ASAN 覆盖（与 C++ 现状对齐）。
  - **本轮复验（2026-10-17，`scripts/run_gate_d.sh` 的前四项）**：`cargo test -p infinity-txn --lib loom`（`RUSTFLAGS=--cfg loom`）**1 passed**；`cargo +nightly-2025-04-04 miri test -p infinity-expression -p infinity-parser -p infinity-planner` **13+18+8 = 39 passed**、0 失败；`-Zsanitizer=address`/`-Zsanitizer=thread`（`--target x86_64-unknown-linux-gnu -Zbuild-std`）同一组 **39 passed**、退出码 0、无 `AddressSanitizer`/`ThreadSanitizer` 报告。**Gate D 的 Rust 侧（loom/miri/ASan/TSAN）在本 revision 上全绿**；`python/parallel_test` 见 Gate B–D 证据条（并行会话实测 16 passed / 2 failed）。
  - **压测工作负载**：并行导入/查询/重启混合，明确排除项与已知边界。

### Gate E — 性能（回归门槛）
- sift `knn_query_benchmark`（1/8 线程）与全文基准，对比 golden log；**门限与现有检查器对齐**（`tools/ci_tools/check_benchmark_result.py:44` 为 **< 7%** 的均值回归），而非自定的 15–25%。
- **强制测量控制（即便沿用 7% 门限也必须满足，非「按需」）**：**可比硬件 + 相同数据集/配置**、**等价的 Rust 基准驱动**、**重复次数与可重复性**、**召回率/质量**指标；并**显式保留或移除** P99 / RSS 要求（不得含糊）。若需更细门限，须先修订检查器与 golden 基线。

### Gate F — 工程质量（Rust 特有）
- `cargo clippy -D warnings`；`cargo fmt --check`；`cargo deny`（许可证/漏洞）。
- 核心 crate `#![forbid(unsafe_code)]`，`unsafe` 仅存于 FFI/SIMD 模块并集中审计。
- **依赖与工具链固定（codex 第 1 轮 #16）**：固定 rustc/edition、codegen 版本、全部依赖版本（`Cargo.lock`）、支持的 target（x86_64 v2/v3/v4、aarch64）；对**传递性 native 代码**（`rust-rocksdb`/`fst`/simsimd 等）分类登记 FFI 归属、许可证与回退资格。
- **产物（2026-10-16）**：`rust_engine/rust-toolchain.toml`（钉住 `1.94.1` + rustfmt/clippy）、`rust_engine/deny.toml`（`cargo deny check`：许可证白名单 / RustSec 漏洞 / yanked / 来源限制）、`Cargo.lock` 入库；CI（`.github/workflows/rust_engine.yml`）已含 **fmt / clippy -D warnings / test / loom / smoke / gate-A 基线（信息性）/ cargo-deny** 六个作业。核心 crate **全部 `#![forbid(unsafe_code)]`**（9/9）。

### Gate G — 内部不变量与配置校验覆盖（codex 第 2 轮 #3）
- **必须**产出一份「**C++ 内部不变量 → Rust 测试**」清单并纳入门槛，至少覆盖：
  - **配置校验 / 拒绝行为**（SDK setter 覆盖不到，如 `src/unit_test/main/config_ut.cpp:175`）；
  - **存储/事务**不变量（WAL 回放幂等、checkpoint 发布、MVCC 可见性、跨底座崩一致）；
  - **索引**不变量（各索引文件集合的发布/合并/清理/恢复）。
- 清单外的例外须**显式登记并说明理由**，不得默认遗漏。
- **产物（2026-10-16）**：`rust_engine/docs/INVARIANT_MATRIX.md`——C++ 不变量 → Rust 测试对照表，含「配置校验 / 存储事务 / 索引」三节与 **5 项显式登记缺口**（G1 配置范围校验、G2 索引文件族契约、G3 跨底座崩溃回归、G4 miri/sanitizer、G5 全文打分不变量）。覆盖项引用真实测试名（`cargo test` 输出可核对）。

---

## 7. 风险登记册

| # | 风险 | 等级 | 对策 |
|---|---|---|---|
| R1 | **全文倒排索引**（源自 iresearch：postings 编码/BM25/WAND/分词）是最大、最复杂的自研子系统（词典 FST 经 **PoC F** 证实「数据路径兼容、**校验和不兼容**」；分词经 **PoC G** 证实 `jieba-rs` **非等价**） | 高 | 独立里程碑（M3）；以 tantivy 设计参考但**保留专有语义**；先统一 FST 校验和；**分词须移植 cppjieba 或定义并测试 jieba-rs 差异归一化**；配专项差分测试 |
| R2 | 向量索引（HNSW/IVF/DiskANN/多向量）的**自定义持久化 + MVCC 集成 + 增量追加**，现成 crate 难直接替换 | 高 | 倾向**移植**；`simsimd`/`faer` 替换 MLAS/Eigen；逐索引独立移植 |
| R3 | **Tokio 与 CPU 密集算子**冲突（误将计算放 reactor 线程致延迟抖动） | 中 | 强制两层模型：async 编排 + `rayon`/`spawn_blocking` 计算；压测定界 |
| R4 | 纯 Rust KV（fjall）**事务/快照/多分区原子性**能力未验证，且**跨底座（KV/WAL/块文件/对象清单）宕机一致性**难以仅靠单点 PoC 证明 | 中→低 | **PoC B/C 已通过**（四门槛中 3 项满足；跨底座崩一致 5 场景全过）；**fjall 无内置 backup/restore** → 需 Infinity 自建备份（余留项） |
| R5 | 结果**确定性**（浮点、排序、`EXPLAIN` 的 operator id/内部列、错误码/文案）与 slt 字面量比对不符 | 高 | 见 R11：先定**确定性兼容策略**，再与 C++ oracle 差分 |
| R6 | MLAS/Eigen **数值一致性**（距离、SVD、GEMM） | 中 | 内核级一致性测试；容差与召回率双指标 |
| R7 | 集群/复制语义（默认关闭、`NewLeader` 为空实现） | 低 | **v0.2 决策：首版排除**；范围冻结后再评估 |
| R8 | 工期/团队经验（C++→Rust 转型曲线） | 中 | 见 §8；关键模块配 Rust 熟手；先做纵向切片跑通全链路 |
| R9 | 磁盘格式不兼容导致升级路径缺失 | 中 | `infinity-import` 迁移工具（含校验/回滚）+ 灰度文档；M6 **必做** |
| R10 | Go SDK thrift 运行时 0.22.0 vs 生成器 0.24.0 的不一致 | 低 | 保持线协议一致；单独评估运行时升级 |
| R11 | **确定性缺少可执行策略**：`EXPLAIN` 期望含 operator id/内部列（`test/sql/explain/explain.slt:27`），`ROUND(SCORE(),4)` 为字面量（`test/sql/dql/fusion.slt:86`） | 高 | 定义**精确 vs 容差**比较、并列排序、数值边界、错误码兼容；**禁止**静默放宽既有测试；对不适用项走显式审批 |
| R12 | **有界队列死锁**（消费者晚于生产者排空；判定错放在「父 fragment 类型」上） | 高 | 见 §3 D4 ⚠️：**每条有界队列边的两端必须并发**，判定依据是「边/生产者」；仅生产者自物化时才可完成后启动。容量=1/早停/错误/取消 + **混合类型边**用例证明可前进 |
| R13 | 依赖与工具链**未固定**、传递性 native 代码（FFI）义务不清 | 中 | 固定 rustc/codegen/依赖版本与 target；分类登记 FFI/所有权/许可证/回退（Gate F） |
| R14 | 索引**「有代码」≠「可用」**（如 DiskANN 未接通），导致范围/里程碑高估 | 中 | 产出「索引 × 操作 × 状态」矩阵作为 M3 依据（§2.6） |

---

## 8. 工作量与团队估算（粗估）

- 引擎代码 ≈ **383k LOC C++（含 86.1k / 86,067 行 `.cppm`）**。**「Rust = 0.6–0.8× 行数」缺乏依据，不作为估算基础**（codex 第 1 轮 #17）；工作量与复杂度主要由**语义重设计**决定（模板元编程、手写线程/调度、跨底座一致性），而非行数比。
- **估算方法（v0.2）**：先由 **M0 的纵向切片原型**（存储内核 / 流式执行 / 单个索引 / 协议互操作）产出**实测速率与未知量**，再据此给出 **人月区间 + 依赖 + 应急余量**；在此之前只给**相对成本**，不给绝对值。

相对成本（以 C++ 现有复杂度为基准）：

| 模块 | 相对工作量 | 难度 |
|---|---|---|
| 存储内核（KV/WAL/Buffer/MVCC + 跨底座一致性） | 高 | 高 |
| 全文倒排 + 多套解析器 | 很高 | 很高 |
| 向量索引族（HNSW/IVF/DiskANN/多向量） | 高 | 很高 |
| 执行/调度（Tokio 重设计 + 取消/资源模型） | 中高 | 高 |
| planner/optimizer/function | 中 | 中 |
| 网络/协议（含二进制布局 spec + golden 报文） | 中 | 中 |
| conformance harness 与测试移植 | 中高 | 中 |

- 建议团队：**6–10 名**（存储 2–3、查询/执行 2、索引 2–3、网络/平台 1–2），配 1 名架构负责人；关键子系统需 Rust 熟手。
- **不建议在原型完成前给出日历工期**；上文里程碑表达关键路径（**M0 一致性基础设施 → M1 存储 → M2 查询 → M3 索引 → M4 协议兼容**）。注意 M1–M3 的兼容性验证依赖 M0 的 harness，故 harness 必须前置。

---

## 附录 A：核心不变量（移植时必须保持）

1. **三个持久化底座彼此独立**：元数据（KV）/ redo（WAL）/ 数据与索引（块文件）不可混用；且**耐久性契约不一**——KV 写入默认 `disable_wal=true`，但对象清单路径启用 RocksDB WAL（`FlushWAL(false)`）。
2. **MVCC = 时间戳数组**（`BlockVersion.created_/deleted_`），更新 = 删除 + 追加；**非** KV 快照。
3. **索引共用「构建→落盘→KV 记录 `ChunkIndexMetaInfo`」骨架，但文件集合各异**：向量族为 `chunk_<id>.idx`；全文为 `.dic/.pos/.spill/.len` 等。**逐索引**定义文件/发布/合并/清理/恢复契约（可逐个独立移植）。
4. **提交管线**：`PrepareCommit`（并行）→ `WriteWAL`（串行，单写者）→ `CommitBottom`（并行）。
5. **交换为有界、物化、带背压**；**凡经有界队列交接的边，消费者与生产者必须并发**（判定在「边/生产者」，与消费者是 stream 还是 materialize 无关；否则死锁）；仅**生产者自物化**（无有界交接 / 有 spill）时才可「完成后启动」；fragment 依赖为树形计数。
6. **`DataBlock` 四种列表示**（flat/constant/compact-bit/heterogeneous）+ null bitmask 语义。
7. **线协议**：`TBinaryProtocol` + `client_version==37` + 列式 `ColumnField`（**逐类型固定二进制布局**）。

## 附录 B：验证项（PoC）——执行结果

> PoC 代码位于 `rust_poc/`（Cargo workspace，每个 PoC 一个 crate）；本地 `.cargo/config.toml` 配置代理（配置文件，非内联环境变量）。执行日期 2026-10-15。

| PoC | 状态 | 结果 / 证据 | 对计划的影响 |
|---|---|---|---|
| **A. Tokio 有界队列无死锁** | ✅ PASS 5/5 | `cargo run -p tokio-deadlock`：容量=1 下，顺序消费**死锁**（如期被 timeout 捕获），「按边/生产者并发」**可前进**；早停 `LIMIT`、错误传播、取消均通过 | 验证 §3 D4 ⚠️ 与 R12 的修正规则成立 |
| **B. fjall KV 四门槛** | ✅ PASS（含 1 处 gap） | ①跨 keyspace **原子批写** `db.batch()` ✓（未 commit 的批不落盘）；②`db.snapshot()` **可重复读** ✓；③`SingleWriterTxDatabase::write_tx().update_fetch()` **原子计数器 RMW** ✓（8 线程 ×25 次，final=200）；④**无内置 backup/restore** → 仅 `persist(SyncAll)` + 目录拷贝 workaround | D2：①–③ 满足；**④ 需 Infinity 自建备份**（列入 D2 门槛与 R4） |
| **C. 跨底座宕机一致性** | ✅ PASS 5/5 | 模拟 KV(fjall) + WAL(分帧 CRC) + 块文件；在**块写入后 / WAL 后 / 全提交 / checkpoint+tail / WAL 撕裂**五个边界注入崩溃：未落 WAL 的不算提交；WAL 回放**幂等**；checkpoint 覆盖 + 尾部回放正确；撕裂尾部被忽略 | 验证 §3 D2-2 与 §附录 A.4 的跨底座契约可落地 |
| **D. Thrift 线协议 golden** | ✅ PASS（双向） | Rust `thrift` crate vs **仓库权威 Python codec（0.24.0）**：①`CommonResponse` **32 字节逐字节 golden 相等**；②Rust 编码→Python 解码 ✓；③Python 编码→Rust 解码 ✓；`ColumnField`（`list<binary>`+`list<bool>`）双向 MATCH | 验证 §3 D6 可行性；**二进制布局须逐类型固定**（已示范） |
| **E. `.slt` 资产可消费** | ✅ PASS | 自研记录解析器扫描 `test/sql`：**173 文件**、1535 `statement ok`、104 `statement error`、1270 `query`（1254 带 `----`）、**0 结构错误**；类型 `I/II/III/T/IR/IT/B/TR`，修饰符 `rowsort/nosort` | 验证 §6 Gate A 资产可被 Rust 工具链消费（真正执行需引擎） |
| **F. `fst` crate 格式兼容** | ⚠️ **部分** | 数据路径兼容（结构校验通过、读回 7 键、前缀区间正确）；**但校验和算法不一致**（详见下） | **修正 §2.6 / §4.2 / R1 的 FST 复用结论** |
| **G. 中文分词一致性** | ⚠️ **部分**（mp 7/10、hmm 8/10） | `cppjieba`（C++，用 Infinity 同款 dict）vs `jieba-rs`，见下 | **`jieba-rs` 非等价替代**，须移植或定义归一化 |
| **H. 确定性规则** | ✅ PASS | ①C `%f` 与 Rust `{:.6}` 在 20 个边界值（含 ties/`1e20`/负零）**逐字节一致**；②`.slt` 中 **1351 个 6 位小数字面量全部精确复现**；③EXPLAIN 结构比较器（剥离 `(id)`/`#id`）演示 exact=false / structural=true | 落成 R11 的**可执行规则** |

**PoC F 关键发现（校验和）**：Rust `fst` crate 的校验和为 **CRC32C（Castagnoli）+ Snappy 掩码**（`fst-0.4.7/src/raw/crc32.rs:21-24`），而 Infinity 的 `CRC32IEEE` 是 **CRC-32/IEEE 802.3**（`src/common/utility/crc.cppm:49`）。实测：`stored==mask(CRC32C)` 为真、`stored==mask(CRC32/IEEE)` 为假 → **C++ `Fst::Verify()` 会拒绝 crate 写出的 fst（反之亦然）**；此外边界谓词也不同（crate `&&` 见 `fst-0.4.7/src/raw/mod.rs:399-401` vs C++ `||` 见 `fst.cppm:104`）。**结论：读写数据路径可直接复用 crate，但复用前必须统一校验和（或放弃 `Verify()`）**，故 §2.6/§4.2/R1 的表述收紧为「**条件复用**」。

**Tantivy 评估（2026-10-16，结论：排除）**：Tantivy 0.24.2 已在本地 registry 缓存（可离线引入），其 BM25 与 Infinity 的公式**同源**：`K1=1.2`、`B=0.75`（`tantivy-0.24.2/src/query/bm25.rs:8-9`）、`idf = ln(1 + (N - n + 0.5)/(n + 0.5))`（同文件 `:52-56`）、tf 分量 `K1*(1-B+B*dl/avgdl)`（`:57-59`）——与 `src/storage/invertedindex/search/bm25_ranker_impl.cpp:25-34` 逐项一致。**但按「须兼容 C++ 全文索引格式」这一硬约束，Tantivy 必须排除**：① 存储格式完全不同——Infinity 自带 `src/storage/invertedindex/fst/`（自研 FST，`dict_reader.cppm:19 import :fst.fst`）+ 自研 `format/`、posting/block-max、column-length 布局；Tantivy 用 `tantivy-fst` fork + bitpacked postings/skip list + `.fieldnorm`/`.term`/`.pos`/SSTable + `meta.json` 段清单，两者**互不可读**；② 即使只看打分，Tantivy 的**文档长度被量化成 1 字节 `fieldnorm_id`**（`FieldNormReader::fieldnorm_to_id`，长文有损压缩），而 Infinity 用精确 `u32` 列长（且实测 `len` 口径为「词数−1」）——分数无法逐位对齐；③ 引入它会把 ~60 个新 crate 带进 Gate F 的 `cargo-deny`/许可证面。**决策：继续自研（严格复刻 `BM25Ranker` + 段/统计口径），Tantivy 仅作设计参考**；本项与 §2.6/R1 「以 tantivy 设计参考但保留专有语义」一致。

**PoC G 关键发现（中文分词）**：以 **Infinity 同款词典**（`/usr/share/infinity/resource/jieba/dict/`）在同一语料上对比，`jieba-rs` 与 `cppjieba` **非逐字等价**（MP-only **7/10**、HMM **8/10**）。三类差异：① **全角标点**——`jieba-rs` 输出 `，`，`cppjieba` 丢弃；② **HMM 未登录词**——`cppjieba` 合并 `小明`，`jieba-rs` 拆 `小 明`；③ **ASCII**——`cppjieba` 的 MP 逐字母切分（`I n f i n i t y`），`jieba-rs` 成词（`Infinity`）。此外 `jieba-rs` 词典解析更严格：Infinity 的 `jieba.dict.utf8` 有 **2 行**被拒（含非 ASCII 空白），需预处理。**结论：不能直接以 `jieba-rs` 替代 `cppjieba`**；选项为 (a) 直接移植 cppjieba，或 (b) 采用 `jieba-rs` 但**定义并测试归一化/接受差异**，并以现有 analyzer 单测为准。复现：`bash rust_poc/jieba-consistency/run.sh`。

**PoC H 关键发现（确定性规则）**：`.slt` 结果路径经 PG 文本编码，REAL/DOUBLE 用 `ColumnVector::ToString` → `std::to_string(double)`（`src/storage/column_vector/column_vector_impl.cpp:971-975`）= printf **`%f`（固定 6 位小数）**——**注意与 `Value::ToString()` 的 `std::to_chars` 最短往返不同**（`value_impl.cpp:1423-1439`）。实测 Rust `format!("{:.6}", x)` 与 C `%f` 在 20 个边界值上**逐字节一致**，且 `.slt` 全部 **1351** 个 6 位小数字面量为 `{:.6}` 的不动点。**规则落地**：Rust 侧结果 REAL/DOUBLE 一律 `{:.6}`；`EXPLAIN` 采用「稳定 id 精确比较，否则剥离 `(id)`/`#id` 的结构比较 + 显式允许清单」。

---

## 附录 C：评审响应记录（codex round 1）

> reviewer = `codex`（独立评审，只读仓库）；owner = CodeBuddy。owner 对每条意见**先核实再处置**；无法复核或与事实不符者标注争议。

| # | 严重度 | 意见摘要 | 处置 | 证据 / 去向 |
|---|---|---|---|---|
| A1 | MAJOR | LOC 基线漏计 `.cppm` 实现代码 | **采纳** | 复核：storage `.cpp` 71,291 + `.cppm` 47,852；全仓 `.cppm` 86,597。§2.1 已重算 |
| A2 | MAJOR | 「RocksDB WAL 全局关闭」不成立 | **采纳** | `persistence_manager_impl.cpp:141` 传 `false` → `kv_store_impl.cpp:443,450` `FlushWAL(false)`。§2.2 / 附录A1 已改 |
| A3 | MAJOR | 全文索引非单一 `.idx` 生命周期 | **采纳** | `index_defines.cppm:55` 后缀 `.dic/.pos/.spill/.len`；`memory_indexer_impl.cpp:491`。§2.2 / 附录A3 已改 |
| B4 | **BLOCKER** | 父子队列表述会导致有界通道死锁 | **采纳** | `fragment_context_impl.cpp:610` 父先于子调度。§3 D4 ⚠️ + §4.3 + R12 已改 |
| B5 | MAJOR | D2 PoC 未覆盖跨底座耐久性 | **采纳** | D2 新增「跨底座宕机一致性」Poc 门；R4 升级为高 |
| B6 | MAJOR | D4 缺资源/取消模型 | **采纳** | §3 D4 新增线程预算/准入/协作取消/panic 安全 |
| B7 | MAJOR | D5 未覆盖多套文法 | **采纳** | `expr_parser.cpp:36`、`http_search_impl.cpp:43`、`search_driver_impl.cpp:93`。D5 已列 5 套入口 + 差分语法测试 |
| B8 | MAJOR | IDL 兼容 ≠ 线兼容（二进制布局） | **采纳** | `infinity.thrift:336` + `types.py:104,116`。D6 新增布局 spec + golden 报文 |
| B9 | MAJOR | 索引「有代码」≠「可用」 | **采纳** | `new_txn_index_impl.cpp:1168` DiskANN 未实现。§2.6 加状态列 + R14 |
| C10 | MAJOR | 固定测试数不构成可执行覆盖 | **采纳** | 冻结 ID/marker/数据集/manifest（§2.7 注 2、§6）。**计数已复核**：raw 313/34 各含 5 处注释定义，**有效 308/29**（正则 `^\s*(?:async )?def test_`），二者均 ≠ pytest 收集数。**codex 正确，owner 原「争议」判断已更正** |
| C11 | MAJOR | HTTP / 内部行为未作硬门槛 | **采纳** | Gate B 增 HTTP `--http`；Gate C 增 MinIO + compaction 后恢复 |
| C12 | MAJOR | Gate D「无数据竞争」不可验证 | **采纳** | Gate D 改为 loom 模型 + miri（排除 FFI）+ native sanitizer + 压测 |
| C13 | MAJOR | Gate E 与既有检查器不一致（<7% vs 15–25%） | **采纳** | `check_benchmark_result.py:44` 为 <7%。Gate E 已对齐 |
| D14 | MAJOR | 确定性缺可执行策略 | **采纳** | R11 + Gate A；`explain.slt:27`、`fusion.slt:86` 引为证据 |
| D15 | MAJOR | 迁移/集群范围自相矛盾 | **采纳** | §5 新增「范围冻结」：集群默认排除；迁移为 M6 必做 |
| D16 | MAJOR | 依赖/unsafe 策略未落地 | **采纳** | Gate F 固定工具链/依赖/target + native 代码登记；R13 |
| E17 | MAJOR | §8 非可辩护估算 | **采纳** | §8 改为「原型驱动 + 人月区间 + 应急余量」，撤销无依据的 0.6–0.8× 比 |

**结论（round 1）**：17 条全部纳入处置（16 条采纳；C10 数字按复核结果更正为 codex 口径）。

### C.2 round 2 响应（v0.3）

| 项 | 处置 | 去向 |
|---|---|---|
| **BLOCKER B4**：并发判定应在「边/生产者」，而非「父 fragment 类型」 | **采纳** | 复核 `fragment_context_impl.cpp:592`（判定 `fragment_type_==kParallelStream`，在生产者一侧）+ `:307`（scan 子为 stream）+ `:235`（merge 父为 serial-materialize，可有流式子）。§3 D4 ⚠️ / §4.3 / R12 / 附录A5 全部改为「**凡有界队列交接，两端必须并发**」 |
| A1 / 新#7：引擎 `.cppm` 子列误含 UT | **采纳** | 引擎 `.cppm` = **86,067**（UT 的 530 行单列）；§0/§2.1 已改，并补计数方法 |
| B7 / 新#8：语法标注错误 | **采纳** | `search_parser.y` = **布尔全文查询文法**（`:81`）；`MATCH`/`FUSION`/KNN 在主文法 `parser.y:411`。D5 已改 |
| B9 / 新#6：FST 兼容性自相矛盾 | **采纳** | 依赖表与附录 B 全部改为「**待 PoC 验证**」，不再作为既成结论 |
| C10：Python 计数误读 | **采纳（owner 更正）** | 复核确认 raw 313/34 含注释定义，有效 308/29；§2.7 注 2 / 附录 C 已改 |
| C11：内部不变量未入门槛 | **采纳** | 新增 **Gate G**（配置校验 + 存储/事务 + 索引不变量清单，例外须登记） |
| C13：Gate E 控制项「按需」 | **采纳** | 改为**强制**（硬件/数据/驱动/重复/质量；P99、RSS 显式保留或移除） |
| D14 / E17 / 新#5：确定性规则与估算缺里程碑强制 | **采纳** | M0 退出准则新增「**确定性兼容规则定稿**」+「**四类原型**」+「**人月区间与应急余量**」，并作为**授权 M1 的前置门槛** |

**结论（round 2）**：全部剩余项已采纳并落到具体章节，无「拒绝」项。BLOCKER（B4）已按 C++ 在**生产者一侧**的语义重写。待 round 3 复评。

### C.3 round 3 响应（终局）

codex round 3 给出 **`VERDICT: APPROVE`**，判定 B4 已在设计层正确解决且 D4/§4.3/R12/附录A 一致；其余实质项均已闭环。另提 3 处 MINOR，owner 已闭合：

| MINOR | 处置 | 去向 |
|---|---|---|
| R1 风险表仍无条件断言 FST 可直接复用 | **已改** | R1 改为「互操作 PoC 通过前仍按可能需移植冗余准备」 |
| §8 仍保留旧 `.cppm` 舍入值 86.6k | **已改** | 改为 `86.1k / 86,067` |
| 摘要仍把子串计数当作可执行用例数 | **已改** | 摘要标注「313 个定义，308 有效」，并指向冻结收集清单 |

**双方一致结论**：计划书在**设计层**已达成一致（owner 修订 ↔ reviewer 复核 共 3 轮，1 BLOCKER + 3 MAJOR→MINOR 全部闭合）。**注意**：批准对象是**计划**，不代表其 §附录 B 的 PoC 已完成；M0 的 PoC/原型是人月估算与授权 M1 的前置门槛。

## 附录 D：EXPLAIN 打印器规则（2026-10-17 逐行读源码得出，供后续移植）

Gate A 的 `query` 残余中约 50 条是 `EXPLAIN`。当前 Rust 侧 `Statement::Explain` 是**桩**：只做 `table_schema` 校验后返回空 `Rows`（`infinity-executor/src/lib.rs` 的 `Statement::Explain` 分支），因此这些记录全部 `got []`。要达标必须移植打印器；规则已在 C++ 源码中确认，记录如下以免重复摸索。

**来源**：`src/planner/explain_logical_plan_impl.cpp`（`ExplainLogicalPlan::Explain` 的各 overload）。

**缩进与箭头（关键）**：每个节点一个 `intent_size`，根为 `0`，子节点为 `intent_size + 2`。
- 节点行：`intent_size == 0` → `"{NAME} ({node_id})"`（**无箭头、无前导空格**）；否则 → `"{(intent_size-2) 个空格}-> {NAME} ({node_id})"`。
- 属性行：`"{(intent_size) 个空格} - {attr}: {value}"`（注意属性行在 `intent_size` 个空格**之后还有一个空格**，所以 `-` 出现在第 `intent_size+1` 列）。
- 例：`PROJECT (4)` / ` - table index: #4` / ` - expressions: [x (#1)]` / `-> SORT (3)` / `    - expressions: [y (#0) ASC]` / `   -> TABLE SCAN (2)` / `      - table name: explain2(default_db.explain2)`。

**节点名与属性（已见）**：`CREATE TABLE`/`CREATE SCHEMA`/`DROP TABLE`/`INSERT`/`PROJECT`/`FILTER`/`SORT`/`LIMIT`/`AGGREGATE`/`TABLE SCAN`/`INDEX SCAN`/`MATCH`/`Merge aggregate`/`UNNEST` 等；属性含 `table name: {name}({db}.{name})`、`table index: #{n}`、`output_columns: […]`（扫描侧，带 `__rowid`）、`output columns: […]`（算子侧）、`expressions: […]`、`filter: …`、`aggregate: […]`、`aggregate table index: #{n}`、`limit: {n}`、`match expression: …`、`index filter: None`/`leftover filter: None` 等。

**节点 id**：由 binder 的 `GetNewLogicalNodeId()` 单调计数，**自底向上**：先给绑定阶段的基础节点（通常是 1），再给扫描（2），然后每个上层算子 +1。实测：`SELECT * FROM t` → `TABLE SCAN (2)`、`PROJECT (3)`；`SELECT COUNT(*) … WHERE c2=5` → `TABLE SCAN (2)`、`FILTER (3)`、`AGGREGATE (4)`、`PROJECT (5)`。**但**用到索引时会跳号（`INDEX SCAN (6)` 与 `PROJECT (4)` 并存），说明绑定索引会额外占用 id——**不能用「层数+1」线性外推**。

**两处已记录的不一致（移植时需留意，不能靠猜）**：
1. `explain/explain.slt` 的期望块像是 C++ 输出**整体加了一个前导空格**（` PROJECT (3)`、`  - table index: #4`、` -> TABLE SCAN (2)`、`    - table name: …`），而 `dql/aggregate/test_agg_load_meta.slt`、`dql/rbo_rule/column_pruner.slt` 等与源码规则**逐字节一致**（`PROJECT (5)`、` - table index: #4`）。按源码规则实现只能与其一相符。
2. 搜索结果类计划的期望块在部分文件里是**自底向上**书写的（`dql/result_cache/cached_limit.slt`、`test_agg_load_meta.slt` 的 MISMATCH 摘要即按文件顺序列出，首行为最深属性行），而 `explain.slt` 是自顶向下；harness 按文件顺序比较，故同一打印器在两类文件里都需要不同的行序。

**结论（与 §6 一致）**：EXPLAIN 需**整体移植 binder 的节点 id 分配 + `explain_logical_plan_impl.cpp` 的打印器**（含表达式渲染 `x (#1)`、`CAST(c2 (#0) AS BigInt) = 5`、列裁剪后的 `output_columns`）才可能达标；本阶段未实现，故这 ~50 条至今为失败。
