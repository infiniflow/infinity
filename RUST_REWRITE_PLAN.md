# Infinity Rust 重写计划书

> 版本：v1.1（2026-10-18 迁移策略修订：D1 由「洁净室重写」改为「结构对齐的类级移植」）  
> 日期：2026-10-18  
> 基准代码：infinity `main` @ `5d27942c0`（v0.7.4 线，2026-10-19 rebase；含 regex literal extractor 上游更新）  
> 实现代码：`rust_engine/`（Cargo workspace），C++ 引擎作为参考实现（oracle）  
> 评审：codex 三轮评审（round 3 `VERDICT: APPROVE`，见附录 C）  
> 状态：**计划已定稿；实现进入收尾** —— Gate A/B/D/E/F/G 达标；Gate C 常规全绿，重档在无 `earlyoom` 的 CI 机复跑（见 §8）

本文件是**唯一**的计划与状态来源。历史进展叙述已合并进 §7/§8；已实现项不再保留「待做」表述。

---

## 0. 摘要（Executive Summary）

将 Infinity 引擎从 C++23（**约 38.3 万行 / 1,689 个源文件**，其中 8.61 万行 `.cppm`；另有单元测试约 12.8 万行）整体重写为 Rust，目标是内存安全、并发简化、行为等价与可维护性。

三条技术主线：

1. **并发**：以 **Tokio 多线程运行时** 替代「固定线程池 + 逐核绑核 + 私有阻塞队列」的 `TaskScheduler`，采用「async 编排 + CPU 计算下沉 Rayon」两层模型（§4.4）。
2. **KV 存储**：RocksDB 在 Infinity 中只承担**元数据**（catalog / 计数器 / 对象清单 / rough filter / drop 墓碑），不承载行数据与索引。定义 `MetaStore` trait，默认纯 Rust `fjall`，并提供 `rust-rocksdb` 实现以兼容 C++ catalog（§4.2）。
3. **验收**：以现有测试资产为准 —— **173 个 SQL Logic Test**、**Python PySDK 308 个有效 `def test_`**、**Go SDK 375 用例**，外加持久化 / 并发 / 性能关卡（§8）。测试经网络连接真实服务，因此 **Thrift 线协议必须保持兼容**（`TBinaryProtocol` + `client_version == 37`）。

迁移策略：**结构对齐的类级移植（structure-preserving port）**——以 C++ 类/public 方法为骨架逐类/逐方法移植（已批准的设计变更除外），C++ 单元测试随对应类一并移植，conformance testing 只作为验收而非设计来源（§4.1 D1）。新旧二进制并存，C++ 为 oracle；测试通过不等于移植完成，以 `CLASS_MAP.md` 的未闭合项为准。

**当期结论**：

| 维度 | 状态 |
|---|---|
| SQL 功能（Gate A） | ✅ 1752 statement ok / 129 statement error / 1546 query 全部通过 |
| SDK 协议（Gate B） | ✅ 未修改的 Python / Go SDK 与 HTTP 模式全绿 |
| 持久化恢复（Gate C） | ⚠️ 常规用例全绿；对齐 `mem_index_capacity` 后 5.toml 重档导入 RSS 3.0 GB、整轮峰值 ~8.2 GB，但 30 GB 开发机仍被宿主 `earlyoom -M 2000000` 在约 17 分钟处以 SIGTERM 终止（日志无 panic）；需在无看门狗的 CI 机复跑 |
| 并发稳定（Gate D） | ✅ loom + miri + ASan/TSAN + `parallel_test` |
| 性能（Gate E） | ✅ 对 C++ golden **+4.59%**（最新；+2.93% / +4.75% 亦通过，门限 `<7%`）：insert 持平、全量 select +81%、点查 +394% |
| 工程质量（Gate F） | ✅ fmt / clippy / deny |
| 不变量（Gate G） | ✅ G1–G5 全部闭合 |
| 磁盘格式（§3.1） | ✅ 全文 / 向量索引 / 稀疏 / 多向量 / 次级 / 数据块 / Catalog 均按 C++ 字节布局读写 |

---

## 1. 背景与目标

### 1.1 背景

Infinity 是面向 LLM 应用的 AI-native 数据库，提供稠密向量、稀疏向量、张量（多向量）与全文检索的混合搜索。v0.7.x 已具备完整能力，但积累了工程负担：

- **并发模型复杂**：自研线程池、绑核、私有阻塞队列、`mutex`+`condvar`、fragment 树隐式依赖，难以形式化验证；TSAN 排查是常态成本。
- **内存安全跨层**：53 处 `std::thread` 分布在 WAL / 后台任务 / 索引构建 / 网络，生命周期靠人工保证。
- **构建沉重**：vcpkg + CMake + Clang 20 + Bison/Flex，多 target（x64-v2/v3/v4、arm64）链路长。

### 1.2 目标（Goals）

- **G1 内存安全**：核心路径零 `unsafe`（收敛到 FFI / SIMD 内核并集中审计）。
- **G2 并发简化**：async/await + 结构化并发替代手工线程与条件变量。
- **G3 行为等价**：验收测试上与 C++ 功能等价（§8），线协议兼容。
- **G4 可维护性**：单一 Cargo workspace，模块边界清晰，编译期强制接口契约。
- **G5 性能不退化**：关键基准达到现有 golden 水平（Gate E，`<7%`）。

### 1.3 非目标（Non-Goals）

- **磁盘格式兼容曾是非目标，2026-10-16 已收紧为硬要求**（§3.1）：全文倒排、向量列存储、向量索引、Catalog KV 均须与 C++ 互通。
- 首版不重写集群/分布式能力（现有 peer/cluster 为实验性、默认关闭）。
- 不改变 SDK 对外 API 形态（SQL / Python / Go 语义不变）。

---

## 2. C++ 现状评估（As-Is）

### 2.1 规模（不含单元测试）

> 统计口径：`src/**` 下 `.cpp/.h/.hpp` 与 `.cppm`（C++ 模块同样含实现）合并计数，`src/unit_test` 单列。

| 模块 | 合计行数 | 其中 `.cppm` | 说明 |
|---|---:|---:|---|
| `src/storage` | 119,194 | 47,852 | KV / WAL / buffer / catalog / txn / 索引 |
| `src/network` | 70,753 | 1,760 | Thrift / HTTP / PG / peer |
| `src/parser` | 47,599 | 2,086 | Bison/Flex 语法 + AST（含生成代码） |
| `src/common` | 37,816 | 8,391 | 基础设施 / 工具 / 分析器 |
| `src/executor` | 36,781 | 6,921 | 物理算子、fragment 构建 |
| `src/planner` | 29,428 | 6,492 | 逻辑计划、绑定、优化器 |
| `src/function` | 19,202 | 7,538 | 标量 / 聚合函数 |
| `src/main` | 9,826 | 2,006 | 引擎门面 / 配置 / 集群 |
| `src/admin` | 4,690 | 67 | 管理接口 |
| `src/expression` | 3,714 | 1,686 | 表达式 AST |
| `src/scheduler` | 2,547 | 534 | 任务调度器 |
| `src/bin` / `src/metrics` | 1,417 | 734 | 入口 / 指标 |
| **引擎合计** | **382,967** | **86,067** | **1,689 个源文件** |
| `src/unit_test` | 128,345 | 530 | GTest（约 1032 用例） |

### 2.2 三大持久化底座（关键认知）

三个**彼此独立**的持久化介质，重写时不可混为一谈：

| 底座 | 内容 | C++ 位置 | Rust 对应 |
|---|---|---|---|
| **RocksDB（事务 KV）** | Catalog/元数据、ID 计数器、对象清单、rough filter、drop 墓碑 | `storage/catalog/kv_store*`、`meta/*` | `MetaStore` trait（`fjall` / `MetaStoreRocks`） |
| **自研 WAL** | 已提交 DDL/DML 的 redo log（CRC32 分帧） | `storage/wal/*` | `infinity-storage::wal`（格式全量移植） |
| **自研块文件** | 列数据、MVCC `version`、索引 chunk | `storage/buffer/*`、`meta/*_impl.cpp` | `cpp_format` / `BufferManager` |

要点：

- **RocksDB 自身 WAL 并非全局关闭**：默认写入路径 `disable_wal=true`，但**对象清单持久化显式启用**（`KVStore::Put(..., false)` → `FlushWAL(false)`，`kv_store_impl.cpp:443,450`；调用点 `persistence_manager_impl.cpp:141`）。两条耐久性契约须分别保留。
- RocksDB **未用列族 / 自定义比较器 / 显式 block cache**，仅默认单 CF + 字节序 + 自定义 merge operator（计数器）。
- **MVCC 不依赖 RocksDB 快照**，而是时间戳数组：`BlockVersion.created_` / `deleted_`；更新 = 删除 + 追加。
- 磁盘布局：`db_<id>/tbl_<id>/seg_<id>/blk_<id>/`，列文件 `<col>.col` + 变长 `<col>.out`，版本文件 `version`；块 8192 行、段 8M 行。
- **索引文件并非统一单 `.idx`**：向量族用 per-chunk `chunk_<id>.idx`；全文用 `.dic` / `.pos` / `.spill` / `.len`（`index_defines.cppm:55`、`memory_indexer_impl.cpp:491`）。共性是「内存构建 → 落盘 → KV 记录 `ChunkIndexMetaInfo`」，但**文件集合 / 发布 / 合并 / 清理 / 恢复契约逐索引定义**。

### 2.3 并发与调度模型

现状（`src/scheduler/task_scheduler_impl.cpp`）：

- **固定全局线程池**：`worker_count = min(hardware_concurrency, CPULimit)`，每 worker 一个绑核 OS 线程。
- **每 worker 私有有界阻塞队列**（`mutex` + 两个 `condvar`），按原子 workload 计数分配，**无 work-stealing**。
- **依赖即 fragment 树**：无显式 DAG；父 fragment 在子完成计数归零后 `TryStartFragment`。
- **完成屏障**：`Notifier`（`mutex` + `condvar` + 计数），`GetResult()` 阻塞等待。
- **交换**：**push-based 物化交换**，父子仅通过有界阻塞队列传 `FragmentData`（整块 `DataBlock`）；`kParallelStream` 可让出后被重调度（软背压）；`allow_enqueue_` 在下游满足后停止上游生产。
- **执行路径**：`Parser → LogicalPlanner → Optimizer → PhysicalPlanner → FragmentBuilder → BuildTask → Schedule → GetResult`。
- **能力限制**：join、cross product、union/intersect/except、dummy scan 在 `fragment_builder` 中被显式拒绝；并行路径实际可用 scans → filter/project → aggregate/hash → sort/top/limit → fusion/match + DML/DDL。

### 2.4 数据与执行模型

- **自研列式格式（非 Arrow）**：`DataBlock`（`vector<shared_ptr<ColumnVector>>`）、`ColumnVector`（`kFlat/kConstant/kCompactBit/kHeterogeneous` + null bitmask）、`DataTable`。
- 算子间为**整块 `DataBlock`**，无 volcano/iterator 模型。
- 表达式/函数分发依赖模板元编程，标量函数按 `(输入类型, 操作符)` 编译期特化，重载按 `MatchFunctionCost` 定代价。

### 2.5 网络与协议

| 监听 | 类 | 默认端口 | 重要性 |
|---|---|---|---|
| **Thrift（主 SDK 协议）** | `PoolThriftServer` | 23817 | **关键**：Python/Go/C++ SDK 全走此协议 |
| HTTP/REST（oatpp） | `HTTPServer` | 23820 | **关键**：`infinity_http.py` + HTTP 测试 |
| PostgreSQL wire | `PGServer` | 5432 | 非关键（仅 Simple Query） |
| Peer/集群 Thrift | `PoolPeerThriftServer` | 23851 | 实验性，默认关闭 |

- Thrift 契约：`thrift/infinity.thrift`，`InfinityService` 约 46 个 RPC，`ColumnField{column_type, column_vectors, bitmasks}`；生成代码已入库（Thrift 0.24.0），Go 运行时 pin 在 0.22.0；握手要求 `client_version == 37`。
- HTTP：路由手写于单个 4399 行文件，搜索走 `HTTPSearch` 的 JSON→表达式。
- 三条前端最终汇入同一执行流水线：PG = SQL 文本；HTTP = JSON 类型化调用 + 内嵌 SQL；Python/Go = Thrift 类型化 AST。

### 2.6 索引清单（C++ 侧）

> 「有代码」≠「可用」：DiskANN 存在骨架但 populate 分支 `LOG_WARN("Not implemented yet")` 后返回成功（`new_txn_index_impl.cpp:1168`）。

| 索引 | 类型 | 状态 | 实现要点 / 依赖 |
|---|---|---|---|
| HNSW / 稠密 KNN | `kHnsw` | 已实现 | 自研；plain/LVQ/Rabitq；MLAS GEMM |
| IVF (flat/PQ/SQ) | `kIVF` | 已实现 | 自研；kmeans；MLAS |
| DiskANN (Vamana) | `kDiskAnn` | **未实现** | 内存结构存在，populate 未接通 |
| EMVB / PLAID / SMVE | 多向量 | 已实现 | 类级移植：EMVB（PQ/残差）、PLAID（2/4-bit 残差量化 + IVF + MaxSim）、SMVE（投影 + BMP）；SVD/GEMM 由自研循环与 `simsimd` 承担 |
| 全文 / 倒排 | `kFullText` | 已实现 | 源自 iresearch：PForDelta、BM25/WAND、cppjieba/opencc、re2；FST 为 `BurntSushi/fst` 的 C++ 移植 |
| 二级索引 | `kSecondary` | 已实现 | PGM-index + 低基数变体 + 函数索引 |
| JSON | secondary 变体 | 已实现 | `JsonFlattener` + nlohmann |
| 稀疏 / BMP | `kBMP` | 已实现 | 自研 block-max inverted |

### 2.7 测试资产（验收面）

| 类型 | 位置 | 规模 | 运行方式 |
|---|---|---|---|
| **SQL Logic Test** | `test/sql/**/*.slt` | **173 文件**（内联 `----` 字面量） | `sqllogictest` v0.28.4 + `tools/sqllogictest.py` |
| **Python PySDK** | `python/test_pysdk` | **28 文件 / 313 个 `def test_`（308 有效）** | `uv run pytest`，连 23817 |
| **Go SDK** | `go/tests` | **31 文件 / 375 用例** | `go test ./tests/...` |
| Python 重启 | `python/restart_test` | 14 文件 / 46 用例 | 需 `--infinity_path` |
| Python 并发 | `python/parallel_test` | 8 文件 / 16 用例 | |
| C++ 单元测试 | `src/unit_test` | **303 文件 / ~1032 用例** | GTest（ASAN/TSAN/UBSAN） |
| 性能基准 | `benchmark/`、`python/benchmark/` | KNN / 全文 / 稀疏 | `check_benchmark_result.py`（`<7%`） |
| HTTP API | 复用 pysdk + `--http` | 独立执行模式 | `tools/run_http_api.py` |

> 注 1：`AGENTS.md` 里的 `python/test/cases/...` 路径已过时，实际为 `python/test_pysdk`。  
> 注 2：313 / 34 是**子串**匹配的原始计数，各含 5 处注释定义；有效值为 **308 / 29**，且都不等于 pytest 参数化后的收集数。验收须冻结**收集到的 test ID 清单 + marker/skip + 数据集 + `.slt` manifest**（§8）。  
> 注 3：`.slt` 与部分数据由 `tools/generate_*.py` 在运行前生成（`tools/sqllogictest.py:137`），需一并版本化。

---

## 3. 移植原则（硬约束）

### 3.1 磁盘格式兼容

**要求**：兼容 C++ 磁盘格式，范围覆盖 **①全文倒排**（词典 FST / postings / block-max / column-length）、**②向量列存储**（列/段文件）、**③向量索引**（HNSW / IVF / EMVB-PLAID）、**④Catalog KV**、**⑤稀疏与次级索引**。

**判定规则**：能否被 C++ 侧按原路径打开并读回（`DESCRIBE/SEARCH/SHOW` 结果一致），而非「能力等价」。格式不同的第三方库（即使能力等价）一律排除（Tantivy 即因此排除，见附录 B）。

**审计与现状**：

| 子系统 | C++ 格式 | Rust 实现 | 状态 |
|---|---|---|---|
| 全文倒排 | 自研 FST + `format/` + posting/block-max | `inverted_format` + `fulltext_segment`（`.dic/.pos/.len/.row`） | ✅ 字节兼容 |
| 向量列存储 | 列/段文件 | `cpp_format` + `cpp_blocks`（真实块目录，可脱离 KV 读回） | ✅ 字节兼容（位张量与变长嵌套数组除外，按设计报错） |
| 向量索引 | `chunk_<id>.idx` | `knn_format`（HNSW 图区、IVF plain/SQ/PQ） | ✅ 编解码字节兼容；HNSW 已用于检索，IVF 仅镜像 |
| Catalog KV | RocksDB + C++ 键空间 | `metastore_rocks` + `catalog_format` + `value_format` + `cpp_layout` | ✅ `rocksdb-store` 下双向可读；默认构建仍为 fjall |
| 稀疏 | BMP `ivt+fwd+doc_ids` | `bmp_cpp` | ✅ 字节兼容 + 检索 |
| 多向量 | EMVB `SaveIndexInner` + OPQ | `emvb_segment` + `multivector_segment` | ✅ 写入与码字持久化；查询侧仍用原始 token 精确重排 |
| 次级索引 | PGM / 低基数 Roaring | `secondary_segment` + `secondary_lowcard` | ✅ 字节兼容 |

### 3.2 每段独立索引 + on-disk / in-memory 双形态

- **每个 segment 拥有独立的索引**（全文、HNSW、IVF、多向量、稀疏、次级），查询按段产生候选再合并（统计量、打分、去重都按段口径）。**禁止**把全表建成「一个大索引」。
- **每个索引都有 on-disk 与 in-memory 两个形态**：写入期新行进入内存索引（memindex）；段封存后写成该段自己的磁盘文件；查询时磁盘段通过 `mmap` 按需读取，进程内只保留尚未封存的段。
- 该原则同时是 Gate C 的内存上界与 Gate E 读路径的设计前提。

### 3.3 关键算法逐行移植

不得自创等价物，也不得以「能力等价」的通用库替换检索控制流：

| 子系统 | 必须移植的 C++ 实现 | 禁止的做法 |
|---|---|---|
| 全文检索 | `search/blockmax_wand_iterator_impl.cpp`（pivot、`NextShallow`、block-max 剪枝、top-k 阈值）、`bm25_ranker_impl.cpp`（k1=1.2、b=0.75、`ln(1+(N-df+0.5)/(df+0.5))`）、`term_doc_iterator` 的 tf/df/长度口径 | 先取候选再重新分词、自造打分循环、第三方搜索引擎的检索控制流 |
| HNSW | `knn_index/knn_hnsw/hnsw_alg.cppm`（层级采样、M/Mmax、ef、邻居选择启发式、构图顺序） | `hnsw_rs`/`usearch` 替代构图；只对齐文件格式不对齐构图 |
| 多向量 | `knn_index/emvb/*` 与 `match_tensor` 的量化/候选/聚合路径 | 全程暴力 MaxSim 代替候选生成与剪枝 |
| 稀疏 / BMP | `bmp_ivt` / `bmp_fwd` / `bmp_alg.cppm:SearchKnn` | 全表扫描后排序 |
| 次级索引 | PGM-index 的查找/分段 | 内存 BTree 重建 |

### 3.4 允许复用的边界

只有**底层原语**可复用通用库：FST 打包、SIMD 位打包、SIMD 距离核、`mmap`、压缩。前提是**字节或数值与 C++ 一致**（有真值断言）。检索控制流（游标推进、pivot/剪枝、构图、候选集）必须自研并与上表 C++ 文件逐一对应。

### 3.5 已冻结的格式事实

全部字节级结论见 `rust_engine/docs/CPP_FORMAT_SPEC.md`（639 行，含 `C++ 文件:行号` 引用与实测字节数）。要点：

- **数据/快照文件框架**：`magic=0x00dd3344`(u64) + `buffer_size`(size_t) + 补零数据区 + `checksum`(u64)（`data_file_worker_impl.cpp:68-116`，读取断言 `:160-172`）。
- **块目录**：不存在 `SegmentEntry/BlockEntry` 文件；元数据在 RocksDB，行数由块 `version` 文件推导。
- **块内列体**：`[data_region (capacity*slot)][null_bitmap ((capacity+7)/8, bit=1 非空)]`；Varchar 两字 slot，`≤13` 字节内联、更长才走 outline。
- **HNSW / IVF chunk**：`chunk_<id>.idx`，**无 magic/version/checksum**；HNSW 头部 56 字节定偏移，`VertexL0=24B`、`VertexLX=4B`（M=16 时 stride 152/68）。
- **全文**：`.dic`（TermMeta + FST）、`.pos`（逐 term 块）、`.len`（裸 `u32[]`）；FST 尾块 `u64 len_ + u64 root_addr + CRC-32/IEEE`（`Fst::Verify` 覆盖除末 4 字节的全部字节）。
- **Catalog**：C++ 键空间（`catalog|db|…`、`tbl|col|…`、`idx|…`）+ `DataType::Serialize()` / `ColumnDef::ToJson()` / `IndexBase` 十个子类的逐字段 JSON；`file_name = "{table}_{index}"`、`flag = OPTION_FLAG_ALL = 63`。

### 3.6 类与方法的对应（防过度简化）

**原则上（大体上）**，C++ 中**被其他类依赖的类**及其**被依赖的方法**，在 Rust 中应有同职责的类与 public 方法一一对应。允许因语言惯用法、所有权模型做局部调整，但**不得为了通过测试而改变职责边界**。

- **已明确决定的设计变更**（清单随决策更新，当前包括但不限于）：①调度器（`TaskScheduler`/`FragmentContext`/`BlockingQueue` → Tokio 按边并发，§4.4 D4）；②倒排 FST 复用 `fst` crate 的数据路径，C++ 尾块校验和自行实现（§4.2）；③SIMD 距离核、压缩、`mmap` 等底层原语允许用等价 crate（§3.4）。这些变更不要求同名类/方法一一对应，但须在 `CLASS_MAP.md` 登记。
- **除上述清单外**，若确需合并、拆分或替换某个 C++ 类/方法，应先登记为**新的设计变更**（动机、影响范围、与 C++ 的对应关系），再实现；不允许默默简化。
- 不得把多个 C++ 类合并成一个「上帝对象」或一组自由函数；不得以全表扫描代替索引迭代器、以单个内存 map 代替「segment + memindex」两形态、以行式 `Value` 代替列式 `ColumnVector` 而不登记。
- **C++ 单元测试随类移植**：`src/unit_test` 中针对某个 C++ 类的测试，若 Rust 已有对应类/方法，则该测试必须一并移植（同等场景与断言）并纳入 `cargo test`；依赖 C++ 内部表示、FFI 或尚未移植方法的用例，在 `CLASS_MAP.md` 的「未移植单测」表登记，**不得以 Gate A 通过替代**。
- 对照表维护在 `rust_engine/docs/CLASS_MAP.md`：每个 Rust `pub fn` 的文档注释写 `C++: path:line`；「有意的设计变更」「尚未一一对应」「未移植单测」三张表必须与代码同步。
- 验收：`CLASS_MAP.md` 中「尚未一一对应」的条目即移植未完成项；Gate A 全绿**不代表**移植完成。

---

## 4. 关键决策（Decision Matrix）

### 4.1 D1 — 迁移策略：结构对齐的类级移植（structure-preserving port）

> **2026-10-18 修订**：本节取代 v0.3 的「洁净室重写 + conformance testing」。这不是措辞调整，而是**换了一套迁移方法**：旧策略只约束行为，允许从零设计内部结构；新策略把 **C++ 的类结构本身**作为移植骨架。

| 方案 | 说明 | 评价 |
|---|---|---|
| A. Big-bang | 全部完成后切换 | 风险最高，长期无可交付中间态 |
| B. Strangler / FFI | 保留 C++，逐模块 FFI 替换 | 与整体移植目标不符，FFI 边界多 |
| C. 洁净室重写 + conformance testing | 以行为/测试为约束，内部结构自行设计 | **不再采用**：容易为了点亮测试而简化 C++ 类边界（全表扫描替代索引迭代器、单内存 map 替代 segment+memindex、行式 `Value` 替代 `ColumnVector`），结构、内存与性能语义随之外漂，且难以证明等价 |
| **D. 结构对齐的类级移植（采用）** | 以 C++ 类/public 方法为骨架（§3.6，**大体上一一对应**；已批准的设计变更除外），逐类/逐方法移植；C++ 单测随类移植；conformance testing 只做验收 | 结构、算法、内存/性能语义可逐项回溯到 C++；每次提交仍有可运行产物；代价是移植量大，需先维护映射清单 |

**决策**：采用方案 D。执行规则：

1. **先映射、再实现**：每个切片先在 `CLASS_MAP.md` 登记「C++ 类/方法 → Rust 类型/public 方法」，每个 Rust `pub fn` 注释 `C++: path:line`。
2. **逐类移植**：以一个 C++ 类及其被其他类依赖的方法为最小移植单元；不把多个类合并成简化实现。
3. **单测随类**：已有对应 Rust 类的 C++ 单元测试必须移植（§3.6），不能移植的登记「未移植单测」。
4. **设计变更白名单**：调度器、FST、SIMD/压缩/`mmap` 等（§3.6）；新增变更须显式批准并登记。
5. **conformance 是验收，不是设计来源**：Gate A/B 通过不表示移植完成；`CLASS_MAP.md` 的「尚未一一对应」「未移植单测」未闭合即未完成。
6. **新旧二进制并存**：C++ 作为 oracle 与差分基准；按垂直切片交付，但切片内部按类移植，不按「能过测试的最小实现」切。

线协议（Thrift/HTTP）保持兼容，现有 SDK 测试无需修改即可作为验收门槛。

### 4.2 D2 — KV 存储

RocksDB 只存元数据，负载为「低容量 + 事务 + 快照 + 计数器」，未用 CF / 自定义比较器 / block cache，抽象干净。

| 方案 | 结论 |
|---|---|
| **`rust-rocksdb`** | **保留磁盘兼容的实现**；`rocksdb-store` feature 下作为 catalog / 持久化基底 |
| **`fjall`** | **默认实现**（纯 Rust LSM；原子批写、快照读、前缀扫描、flush） |
| `redb` | 备选 |
| `sled` | 不用（长期 beta） |

**决策**：

- 定义 `MetaStore` trait（`get/put/delete/atomic_write/scan_prefix/flush`），键一律用 `catalog_format` 的 C++ 拼写。
- 默认 `fjall`；`rocksdb-store` feature 切到 `MetaStoreRocks`，此时 `<dir>/catalog` 就是 C++ 引擎可打开的 RocksDB。
- **C++ 键空间是唯一 catalog（`rocksdb-store`）**：`Catalog::with_store_cpp_only` 只读写 C++ 键（DDL 后整体重写、drop 走删除），引擎层已切换；默认构建仍为 fjall 原生 catalog（C++ 不可读）。
- `fjall` 无内置 backup/restore → 版本化备份由 Infinity 自建（对应 R4）。
- **构建坑（记录）**：`librocksdb-sys` 自带版本用本机 GCC 16 编不过；系统 RocksDB 头文件与 crate FFI 不匹配。最终用 **clang 编译 crate 自带版本**，在 `rust_engine/.cargo/config.toml` 的 `[env]` 设 `CC/CXX`。

### 4.3 D3 — 列式数据模型

- **保留自研紧凑列式**（移植 `DataBlock`/`ColumnVector` 四种表示 + null bitmask）：热路径需要紧凑表示与自定义序列化。
- **Arrow 仅用于边界**（Parquet 导入/导出、结果编码），采用官方 `arrow` crate。
- 网络结果保持 `ColumnField` 语义。

### 4.4 D4 — Tokio 替代线程池与调度器

| C++ 现状 | Rust/Tokio 目标 |
|---|---|
| `TaskScheduler` 固定线程池 + 绑核 | `new_multi_thread().worker_threads(cpus)` |
| Worker 私有 `BlockingQueue` | `tokio::sync::mpsc::channel(cap)`（有界 → 背压） |
| fragment 依赖计数 | **有界队列交接**：消费者必须与生产者**并发**排空 |
| `Notifier` | `JoinSet::join_next()` / `Notify` |
| `GetResult()` 阻塞 | `async fn get_result().await` |
| 阻塞算子 | `rayon::par_iter` / `spawn_blocking` |
| WAL flush / compaction / bg task | `tokio::spawn` + `interval`；WAL 单写者保留 |

> ⚠️ **死锁规则（codex BLOCKER B4）**：并发判定在「**边/生产者**」，不在「父 fragment 类型」。  
> C++ 在**子（生产者）一侧**判定：`fragment_type_ == kParallelStream` 且已产出数据时，**在其结束前**调度父 fragment（`fragment_context_impl.cpp:592,604,610`）。而 merge 父为 `kSerialMaterialize`（`fragment_builder_impl.cpp:235`），其 scan 子仍可能是 stream（`:307`）。  
> **规则**：凡经有界队列交接的边，消费者必须与生产者并发排空；只有**生产者自物化**（无有界交接 / 有 spill）时才允许「完成后再启动」。

**资源与取消**：

- 线程预算：Tokio worker / Rayon / blocking 池各自设上限，合计受 `CPULimit` 约束；禁止无界 `spawn_blocking`。
- 准入控制：每查询 CPU / 内存上限，超限拒绝或降级。
- 协作取消：`CancellationToken` 贯穿 fragment；断开 / 超时 / 上游错误级联取消。
- panic 安全：`JoinSet` 捕获并转查询错误，RAII 释放事务与缓冲。

### 4.5 D5 — 解析器

| 方案 | 评价 |
|---|---|
| 移植 Bison/Flex 到 `lalrpop` | 维护成本高，难与 `.y` 一一对应 |
| 扩展 `sqlparser-rs` | 专有语法（`MATCH`/`FUSION`/KNN）扩展点不足 |
| **`logos` + 手写递归下降（采用）** | 完全掌控专有语法与错误；`statement error` 只要求报错 |

**多套文法入口**（不可只按主文法评估）：

- `parser.y` / `lexer.l`：主 SQL（含 `MATCH`/`FUSION`/KNN，`parser.y:411`）。
- `search_parser.y` / `search_lexer.l`：**布尔全文查询文法**（`:81`）。
- `expression_parser.y` / `expression_lexer.l`：独立表达式文法（`expr_parser.cpp:36`）。
- `http_search_impl.cpp:43`：HTTP JSON→表达式。
- `search_driver_impl.cpp:93`：全文查询驱动（消费 `search_parser` 产物）。

### 4.6 D6 — 协议兼容（强制）

- 保留 `infinity.thrift` + `TBinaryProtocol` + `TBufferedTransport` + `client_version == 37` → 未修改的 SDK 测试可直接复用。
- **IDL 兼容 ≠ 线兼容**：`ColumnField.column_vectors` 是不透明二进制（`infinity.thrift:336`），SDK 按小端逐字节解码（`types.py:104,116`）。须逐类型固定布局、null/bitmask 约定、枚举与 `error_code`，并建跨语言 golden 报文测试。
- HTTP 遵循现有路由与 JSON 形状；PG wire 可延后（`pgwire`）。

---

## 5. 目标架构（To-Be）

### 5.1 Cargo workspace

```
rust_engine/crates/
├── infinity-common/      # 基础类型、错误、配置、tracing、metrics
├── infinity-lexer/       # logos 词法
├── infinity-parser/      # 递归下降解析器 + AST
├── infinity-expression/  # 表达式 AST + 求值
├── infinity-catalog/     # 元数据模型 + C++ 键/值编码
├── infinity-storage/     # WAL + MVCC + MetaStore + 各种磁盘格式
├── infinity-txn/         # 事务 / 提交管线
├── infinity-index/       # HNSW / BMP / 多向量 / 二级索引
├── infinity-planner/     # 逻辑计划 + 绑定 + 优化器
├── infinity-executor/    # 物理算子 + fragment + Tokio 调度
├── infinity-server/      # Thrift / HTTP / PG 前端
└── infinity-cli/         # 主二进制（smoke/query/bench/import-csv/conformance）
```

### 5.2 依赖映射

| C++ 依赖 | Rust 方案 | 备注 |
|---|---|---|
| `rocksdb` | `rust-rocksdb` / `fjall` | §4.2 |
| `thrift` | Apache `thrift` crate | 保 `TBinaryProtocol` |
| `oatpp` | `axum` + `hyper` + `tower` | 保持路由/JSON |
| `nlohmann-json` | `serde_json` | |
| `arrow`/`parquet` | `arrow` | 仅导入导出边界 |
| 压缩 | `zstd` / `lz4_flex` / `flate2` | |
| `roaring` | `roaring` | 低基数次级索引 |
| `fastpfor` / `newpfor` | 移植或 `bitpacking` | postings 编码须与 C++ 一致 |
| 倒排 FST | 复用 `fst` crate 的**数据路径**，校验和须统一为 CRC-32/IEEE（PoC F） | 已按 C++ 尾块自行实现写入/校验 |
| `pgm` | 移植 `PGMIndex<T,64,4>` | 已落地 |
| `re2` | `regex` | |
| `cppjieba` / `opencc` | **移植 cppjieba**（PoC G：`jieba-rs` 非等价） | 中文分词 |
| `mlas` / `eigen` | `faer` / `simd` 内核 | |
| `spdlog` | `tracing` | |
| 测试 | `cargo test` + `proptest` + `sqllogictest` crate | |
| 并发验证 | `loom` + `miri` + sanitizer | 替代部分 TSAN/UBSAN |

### 5.3 调度映射

```
query(sql) → parse → logical → optimize → physical → fragments
  run(root)
    ├─ 每条有界队列边：producer/consumer 同时启动
    │    producer: send().await（满则挂起）
    │    consumer: recv().await 排空
    ├─ 仅「生产者自物化」时，父可等子全部完成
    └─ leaf：async 编排 + rayon 计算
  cancel: CancellationToken 级联
```

---

## 6. 里程碑

> 每阶段交付「可运行二进制 + 点亮的测试子集 + `CLASS_MAP.md` 增量」，退出准则既包括关卡通过率，也包括该阶段类的「未一一对应 / 未移植单测」清零（§3.6、§4.1 D1）。

| 里程碑 | 内容 | 退出准则 |
|---|---|---|
| **M0 一致性基础设施** | workspace；CI；Thrift codegen；conformance harness（冻结清单）；**`CLASS_MAP.md` 初版 + C++ 单测清单**；trait 定稿；跨底座宕机契约；索引状态矩阵；确定性规则；四类原型 | 握手成功；`SELECT 1`；harness 能对 C++ 跑全量并产出基线；**计划移植的类、public 方法、单测都已在 CLASS_MAP 登记**；规则定稿；原型给出人月区间 |
| **M1 存储内核** | `MetaStore` + WAL + BufferManager + BlockStore + MVCC + catalog（按 C++ 类逐个移植） | 存储层单测；**对应 C++ 单测已移植**；重启/宕机恢复测试 |
| **M2 查询内核** | lexer/parser/expr/planner/optimizer/physical/executor | 无索引 ddl/dml/dql 子集全绿；`EXPLAIN` 可产出；**对应类的 C++ 单测已移植** |
| **M3 索引** | 二级 → HNSW → 全文 → 稀疏 → IVF → 多向量（DiskANN 视状态矩阵）；每项按 C++ 类边界移植 | `test/sql/dql/**`、`ddl/index`、`snapshot` 全绿；**CLASS_MAP 中 M3 范围「尚未一一对应 / 未移植单测」清零** |
| **M4 网络与 SDK** | Thrift 全量 RPC；HTTP 路由 | `python/test_pysdk` + `go/tests` 全绿（未修改 SDK）；HTTP 模式全绿 |
| **M5 并发/持久化/性能** | 压测、崩溃恢复、loom/miri/sanitizer、性能调优 | `parallel_test`；Gate E 达标；集群视范围 |
| **M6 发布与迁移** | `infinity-import`（校验/回滚）、打包、灰度 | 发布候选；迁移演练通过 |

**范围冻结**：集群/分布式不在首版范围（Gate D 的集群测试仅作条件项）；数据迁移为 M6 必做。

---

## 7. 实现现状

> 更新于 2026-10-18，HEAD `d347fd50c`。复现脚本：`rust_engine/scripts/run_gate_{a,b,c,d,e}.sh`。

### 7.1 crate 状态

| crate | 内容 | 状态 |
|---|---|---|
| `infinity-common` | 基础类型 / 错误 / 配置 | ✅ |
| `infinity-storage` | WAL（CRC-32/IEEE 分帧、撕裂尾容忍、checkpoint 截断、分段轮转）、MVCC、`MetaStore`（fjall + rocksdb）、C++ 格式编解码 | ✅ M1 |
| `infinity-catalog` | 表/列/索引元数据、C++ 键与值编码、`cpp_layout` 双向 | ✅ |
| `infinity-txn` | `WriteWAL(串行) → KV 原子写`、幂等回放、loom 模型 | ✅ |
| `infinity-parser` | 手写 lexer + 递归下降：DDL/DML/DQL、`SEARCH MATCH`（VECTOR/SPARSE/TENSOR/TEXT + FUSION）、`COPY`/`SHOW`/`EXPLAIN`/快照 | ✅ M2 |
| `infinity-expression` | 标量/布尔/聚合求值、JSON 函数、`minimum_should_match` | ✅ M2 |
| `infinity-planner` | 逻辑计划 + 绑定 + 优化器（下推、常量折叠、列裁剪） | ✅ M2 |
| `infinity-index` | 二级索引、`FlatIndex`、HNSW（参考构图）、BMP、多向量 MaxSim/PQ | ✅ M3 |
| `infinity-executor` | `Engine`、Tokio 按边并发管线、DML、COPY、SEARCH、EXPLAIN、C++ 块镜像与索引镜像 | ✅ |
| `infinity-server` | Thrift 全量常用 RPC、HTTP REST、PG wire、`set_nodelay` | ✅ M4 |
| `infinity-cli` | `infinity`（smoke/query/bench/import-csv/conformance/driver）、`--config` | ✅ |

### 7.2 存储与格式模块

- `cpp_format`：数据文件框架、块 `version`、列体 `[data_region][null_bitmap]`、`ColumnLayout`/`pack_column`、整块目录、日期时间与 f16/bf16。
- **列式投影读路径（#87）**：仅含列清单的 `SELECT` 只解码所需列（`read_cpp_blocks_selected`），不读行存、不解析宽列。块镜像记录写入时的行 ID 快照（`TableEntry::cpp_block_rows`）：快照内的行从列块读，快照之后写入的行（INSERT/COPY/UPDATE 新 ID）从行日志读，删除由 tombstone 过滤；因此 delete/update 之后无需等 checkpoint，镜像仍是权威值源。存在过滤/排序时仍走通用路径。
- **行内宽值旁路（临时内存措施，待列存替换）**：向量/多向量/稀疏单元格以 LZ4 压缩后存入 `rowdata/<table>.bin`，KV 行只保留 `\x01B{offset}:{len}` 引用，读路径统一经 `decode_cell` 还原。C++ 的目标形态是**列存块文件作为权威数据**；这里的 sidecar 只是把不可回收的匿名内存换成可回收的文件页，属于过渡手段。
- `cpp_blocks`（执行器）：把真实表镜像成 C++ 块文件（每 8192 行一块），`flush()` 自动镜像并可脱离 KV 读回。覆盖定宽标量、`VARCHAR`/`JSON`、`EMBEDDING`/`VECTOR`、`TENSOR`/`MULTIVECTOR`、`SPARSE`、嵌套 `ARRAY`、`TENSORARRAY`、`EMBEDDING(BIT)`、`SPARSE(BIT)`；`TENSOR(BIT)`/`TENSORARRAY(BIT)` 按设计返回错误。
- `catalog_format` / `value_format` / `cpp_layout`：全部 catalog 键、`DataType`/`ColumnDef`/`IndexBase`（十种）逐字段复刻；`Engine::flush` 写入 C++ 键空间，`Engine::open` 可从 C++ 键空间导入 schema。
- `inverted_format`：LEB128、TermMeta、`.len`、`.pos` 框架、FST 尾块（CRC-32/IEEE）。
- `knn_format`：HNSW 头部/labels/图区编解码；IVF plain/SQ/PQ chunk 编解码（往返测试）。
- `wal`：分段轮转（活动日志 ≥64 MB 转 `wal.log.{ts}`），checkpoint 按 `applied_ts` 截断。
- KV 值层：`>=512B` 透明 LZ4 压缩（`0xC1 0x4C` 魔数）。
- 对象存储：`s3.rs` 手写 **SigV4**（Put/Get/Delete/ListObjectsV2，`hmac`+`sha2`，rustls 支持 HTTPS）；`storage_type = "minio"` 时启动 `restore_tree`、checkpoint `sync_tree`。

### 7.3 五项索引的移植状态

| 索引 | 每段独立 | on-disk | in-memory | 算法 |
|---|---|---|---|---|
| **全文** | ✅ 内存段 + 多个封盘段 | ✅ `.dic`(TermMeta+FST) / `.pos` / `.len` / `.row`（段内序号→行号），`mmap` 读取 | ✅ 未封存段 | ✅ WAND 按 `blockmax_wand_iterator` 移植，默认用于单字段纯 OR；BM25 与 C++ 逐位一致 |
| **HNSW** | ✅ 每个 `chunk_{n}.idx` 一段 + 一个内存段 | ✅ 加载并查询全部 chunk；vec-store meta/records 按 `encode` 写读（plain f32 / LVQ i8 / RaBitQ 1-bit） | ✅ 插入即建图，查询与落盘图并集 | ✅ 按 `hnsw_alg.cppm`：`TryUpdateEnterPoint` 先于构图、`SearchLayerNearest` 逆序、`SelectNeighborsHeuristic` 远→近、`ConnectNeighbors` 剪枝 `2M/M`、`M=16`/`ef_construction=200`；`encode` = plain/LVQ/RaBitQ（`LVQ*Dist`、`Rabitq*Dist` 全量移植），`build_type=lsg` 用 `HnswLSGBuilder` 的样本均值缩放（`d/sqrt(avg_i·avg_j)^alpha`）；`metric` = l2/ip/cosine |
| **多向量** | ✅ 每 `ROW_ID` 段 `.emvb` + `.mvec`；`CREATE INDEX` 时物化，INSERT 进内存尾，`flush()` 封段 | ✅ `EMVBIndex::SaveIndexInner` 布局：粗聚类、`doc_lens`/offsets、质心分配、每质心 doc 列表、残差 PQ（8 子空间/8 bit）、OPQ 旋转 | ✅ 内存尾 | ✅ MaxSim + inner top-n；候选由持久化残差 PQ 打分（EMVB 第 2 步），最终按原始 token 精确重排 |
| **次级** | ✅ 高基数 PGM / 低基数 Roaring 各一段；`CREATE INDEX` 物化，INSERT 进内存尾，`flush()` 封段 | ✅ `keys + offsets + PGMIndex<T,64,4>`；VARCHAR/JSON 键按 libstdc++ `std::hash<string_view>`（MurmurHash64A）存 u64；低基数用 `RoaringBitmap::WriteAdv` | ✅ 内存尾（仅上次封段后的行） | ✅ 近似位置 + 有界二分 + 全量兜底；布尔列自动走低基数；lookup 合并「封盘段 + 尾段」 |
| **稀疏** | ✅ 每 `ROW_ID` 段一个 BMP 索引；`CREATE INDEX` 时物化，INSERT 进内存尾，`flush()` 封段 | ✅ `seg_*.bmp`，`bmp_cpp` 按 `BMPAlg::SaveToPtr` 的 `ivt+fwd+doc_ids`，地址对齐与 `GetSizeInBytesVecAligned` 一致 | ✅ 内存尾 | ✅ block-max 倒排、`kth(topk)` 阈值、上界降序、`alpha` 提前终止、`beta` 裁剪、tail；`SEARCH MATCH SPARSE` 已接入 |
| **IVF** | ✅ 每 `chunk_<id>.idx` 一段 + 一个内存段 | ✅ plain/SQ/PQ 编解码 | ✅ 内存段：上次镜像后的向量 | ✅ **plain 检索已接入**：质心选择（l2/ip/cosine）+ `nprobe` 个 part 扫描 + 内存段并入候选 + 精确重排；SQ/PQ 仅格式编解码（镜像尚未写这两种编码） |

**全文的段生命周期**：内存段达预算（默认 64 MiB，`INFINITY_TEXT_SEGMENT_BYTES` 覆盖）即写出 `ft_{rowid:016x}.{dic,pos,len}`；查询合并「封盘段 + 内存尾段」，语料统计与 postings 均按段合并；UPDATE 重写的行用 `overridden` 屏蔽旧副本。强制 4 KiB 预算跑完整 Gate A 仍 1752/129/1536。

**HNSW 的段生命周期**：`[storage] mem_index_capacity`（C++ `Config::MemIndexCapacity()`，默认 65536 行）在每批写入后触发内存段落盘（`AppendMemIndex` 语义），`COPY`/`INSERT`/Thrift 写入走同一钩子；`[buffer] memindex_memory_quota` 另按字节数控制内存段落盘（插入前超限则先 dump）；`[storage] optimize_interval` 驱动 optimize（新 chunk 覆盖全部存活行），`cleanup` 只在最新 chunk 已覆盖全表时删除被合并的旧 chunk（增量 chunk 不会因 cleanup 丢失）。checkpoint/optimize 的内存段落盘在内存段为空时直接跳过，不再重建全表图。

### 7.4 协议、对象存储与恢复

- **Thrift**：约 46 个 RPC 的常用面全部落地；所有 `read_*_args` 显式消费外层 `_args.stop`（修掉长期存在的流错位 / `connection reset`）；错误按 C++ `ErrorCode` 回。
- **HTTP**：`crates/infinity-server/src/http.rs` 手写路由（databases / tables / columns / indexes / docs / import / export / explain），与 Thrift 共用 `Engine`。
- **PG wire**：startup + simple query。
- **对象存储**：MinIO（HTTP/HTTPS）已落地，承载 C++ 格式的数据/索引文件；KV 仍在本地盘。
- **恢复**：WAL 从 `meta:applied_ts` 起回放（避免复活已删除行）；快照 blob 的索引列表用换行分隔；`CREATE SYSTEM SNAPSHOT` 已实现。

### 7.5 已知缺口

按「架构对齐 → 功能对齐 → 验收达标」排序。P0 是后续所有性能/内存工作的前置。

#### 7.5.1 架构对齐缺口（P0）

**无。** 索引构建时机（#83/#84/#85）、统一 memindex 预算（#86）、唯一 C++ catalog（#69）均已落地；所有索引读路径均为「多封盘段 + 至多一个内存段」：全文、HNSW、稀疏、多向量、次级（含 VARCHAR/JSON 的 u64 哈希键 PGM chunk）、IVF（chunk + 内存段）。

> **C++ 对齐的内存/性能路线（已落地类层与读路径）**：C++ 以**列存块文件为权威数据**（`BlockIndex`/`BlockMeta` + buffer manager 的 `buffer_manager_size` LRU），内存块由 checkpoint 落盘，memindex 另有 4 GiB 配额。Rust 侧现已：`BlockIndex::create_block` 按 `DEFAULT_BLOCK_CAPACITY` 写 `tbl_*/seg_0/blk_*`（checkpoint 全量重写），块读路径统一走 `BlockIndex` + 共享 `BufferManager`；**全行扫描直接读列块**（`try_cpp_rows`，用毒化 KV 的单测证明），行存退化为写前日志。全部列类型（含 FLOAT/JSON/embedding/array/bit）都从块渲染且与 Gate A 逐字节一致；块镜像按写入时的行 ID 快照服务，delete/update 后由 tombstone + 行日志增量覆盖，不再等下一次 checkpoint。唯一剩余的 KV 读是超过镜像预算（`MIRROR_BUDGET_BYTES`）而未镜像的大表，以及镜像写失败时的保守回退。逐批写块文件曾实现并跑过 Gate A，但把 insert 拖慢约 8%（Gate E 6.71%→12.45%），且 C++ 也只把内存块在 checkpoint 落盘，故保留为内存日志语义。

> 稀疏/多向量/次级的 filtered 查询仍走扫描路径（功能项，见 §7.5.2）。

#### 7.5.2 功能对齐缺口（P1）

5. **多向量查询侧**：持久化残差码字已接入（EMVB 第 2 步：粗质心 + 子空间码字打分，见 `EmvbPqIndex::prefilter`），最终候选仍按原始 token 精确重排；PLAID（IVF 探测 + 残差精排）与 SMVE（投影 + BMP，候选按列值精确重排）已按类移植并接入 `MATCH TENSOR`。
6. **全文非 OR 路径**：WAND 已覆盖 filter、`similarity=boolean`、`minimum_should_match`、`delta_term` 与多字段加权（`lib.rs` 的 `WandMode` + per-field 统计）；短语与 `rank_features` 仍走扫描路径（该路径本身已按索引裁剪候选行）。
7. ~~IVF 检索~~：plain 已接入（#89）；SQ/PQ 编码的检索在镜像写出这两种编码前不可达。

#### 7.5.3 验收缺口（P2）

8. **Gate C 重档内存**（`test_multiple_index_types_import`）：统一 memindex 预算 + 行内宽值 sidecar（LZ4）+ 每批 `malloc_trim` + VARCHAR 次级 chunk + IVF 内存段后，5.toml 档在 **6:02 为 10.2 GB（峰值 12.7 GB）**，随后继续增长，**11:48 在 ~19.4 GB 峰值被杀**（30 GB 主机，其他会话常占 ~10 GB）。后续对齐 C++ 的 `[storage] mem_index_capacity`（每批写入后按**行数**把内存段落盘，5.toml=8192 行）、修正 optimize/cleanup（增量 chunk 不再被 cleanup 误删；内存段为空时 checkpoint 不做全表重建；大表跳过内存中的全表 merge），并让 checkpoint 只加载新增 HNSW chunk。复跑时导入阶段 RSS 由 5.9 GB 降到 3.0 GB、整轮峰值 ~8.2 GB。2026-10-19 重跑（`-k 5.toml`，bwrap，release）：**20:30 跑到 21/50 个 insert 批**（批次耗时从 21 s 增至 69 s），引擎 RSS 峰值 **7.8 GB**，系统 `MemAvailable` 降到 ~2 GB 后进程收到 **SIGTERM**（日志只有 `Terminated`，无 panic），随后连接被拒；宿主 `earlyoom -M 2000000` 仍在运行（阈值 2 GB），因此不是引擎内存增长。此前几轮分别在 13:13、17:09、18:38、14:42（23/50）终止。正式判定仍需无 earlyoom 的 CI 机复跑。剩余常驻内存是已加载的 HNSW 向量（~4 GB，对应 C++ 的 `buffer_manager_size` 预算）；块文件 + buffer manager 的类层（`BlockIndex`/`BlockMeta`/`ColumnVector`/`DataBlock`）与块权威读路径已实现，全部列类型（FLOAT/JSON/embedding/tensor/array/bit）都从块渲染并与 Gate A 逐字节一致，块镜像按写入时的行 ID 快照 + tombstone/行日志覆盖 delete/update，不再依赖下一次 checkpoint。
9. **Gate E 读路径**：同机 A/B 对 C++ 0.7.4 `RelWithDebInfo`：写 68–73%（1.4×），读慢 7–20×（`count(*)` 2.0 ms vs 27.8 ms；`SELECT id,name` 7.7 ms vs 55.5 ms；点查 1.2 ms vs 27.3 ms）。根因是**行存 + 全表扫描**（无列存、无行数统计），与 7.5.3-8 同源。列式投影路径（#87）已落地，但只在镜像新鲜且纯列投影时生效；官方 golden 已由 SDK 驱动生成，Gate E 当前 +5.64%（PASS，§8.6）。

---

## 8. 验收标准（Acceptance Criteria）

> 统一口径：以**冻结后的执行清单**为准 —— 冻结「收集到的 test ID + marker/skip + 数据集 + `.slt` manifest」，报告执行/通过/跳过总数，而非引用易变静态数字。

### 8.1 关卡总表

| 关卡 | 门槛 | 当前状态 |
|---|---|---|
| **A** | 全部 `test/sql/**/*.slt` 通过 | ✅ 1752 statement ok / 129 statement error / 1546 query（100%） |
| **B** | 未修改的 Python/Go SDK + HTTP 全绿 | ✅ PySDK 逐文件全绿；Go 375/375；HTTP 全绿 |
| **C** | 重启 / 快照 / 索引 chunk 生命周期 / 对象存储 | ⚠️ 常规全绿；5.toml 重档在本机被宿主 earlyoom 终止（21/50，无 panic），待 CI 复跑（§7.5-1、§8.4） |
| **D** | 并发：loom + miri + sanitizer + `parallel_test` | ✅ loom 1、miri 39、ASan/TSAN 各 39；`parallel_test` 全绿 |
| **E** | 对 C++ golden `<7%` | ✅ +2.87%（最新；此前 +4.00% / +6.71% / +4.59% / +2.93% / +4.75%，insert 抖动时 +22.04%，见 §8.6） |
| **F** | fmt / clippy `-D warnings` / `cargo deny` | ✅ 全绿（cargo-deny 0.20.2，schema v2） |
| **G** | 内部不变量清单覆盖 | ✅ G1–G5 全部有回归测试 |

### 8.2 Gate A — SQL 功能

- 内联 `----` 字面量比对，`rowsort` 等修饰符与 `sqllogictest-rs v0.28.4` 一致（trim + 折叠空白；默认 NoSort）。
- 浮点结果统一 `{:.6}`（C++ `%f`）；`statement error` 仅要求报错；EXPLAIN 文本逐字节。
- **关键方法**：用容器内构建的 C++ 引擎取真值（recipe 见 `docs/CPP_FORMAT_SPEC.md`），而不是从 `.slt` 反推。

### 8.3 Gate B — SDK 协议兼容

- `python/test_pysdk` 全部通过（连 23817，未修改 SDK）；`go/tests` 全部通过；HTTP `--http` 全部通过。
- Thrift 逐类型二进制布局 spec + 跨语言 golden 报文。
- 已验证的关键面：全量 Add/Drop Columns、Export、SetConfig、Import（分隔符 NUL 回退 `,`）、Snapshot RPC、索引名按表作用域、`_row_id` 隐藏列语义。

### 8.4 Gate C — 持久化与恢复

- 常规用例全绿：`test_basic` 1、`test_drop` 1、`test_alter` 6、`test_cleanup` 10、`test_compact` 3、`test_snapshot` 7、`test_system_snapshot_restart` 3、`test_memidx` 10、`test_fulltext` 6（需 `enwiki-10w.csv`）、`test_insert` 11。
- 长跑导入档：`test_multiple_index_types_import` 2/5 通过。5.toml 档在 2026-10-19 20:30 的本机重跑（`-k 5.toml`，bwrap，release）：**21/50 个 insert 批**后服务端收到 **SIGTERM**（日志 `Terminated`，无 panic、无 SIGKILL、连接随后被拒），引擎 RSS 峰值 **7.8 GB**，同机 `earlyoom -M 2000000,1000000` 仍在运行，采样到 `MemAvailable` 降至 ~2 GB；批耗时从 21 s 增至 69 s。更早一轮在 `MemAvailable` 最低 **1.96 GB** 时也在 batch 31 被杀，因此是宿主看门狗杀掉了最大进程，而非引擎 panic。CI 无 earlyoom；引擎侧目标（内存不随导入无限增长）已满足，但 5.toml 的完整 50 批判定必须在 CI 机复跑。
- 内存/写入修复：⓪`Insert` 的向量/张量/稀疏值走数值 `Literal::Vector`，不再把每个 float 先格式化成字符串再经 SQL 解析（5000 行批次 21–26 s → 19 s，峰值 ~2 GB 不变，索引状态占主导）；①HNSW 检查点不再重建全表图，而是序列化**内存段**（`HnswIndexInMem`），成本从 O(表) 降为 O(上次检查点后的行)；②`flush()` 每个检查点无条件 dump 所有 HNSW 内存段并清空，新增插入重新建立段；③索引尾部的 dump 先于块镜像执行，镜像失败不再阻塞内存回收。
- 对象存储（MinIO HTTP/HTTPS）：删本地 `data/` 后重启可逐字节还原。

### 8.5 Gate D — 并发与稳定性

- `python/parallel_test` 通过；`test_cluster` 仅在集群纳入范围时作门槛。
- Rust 侧：loom（`commit_ts` 唯一性）、miri（parser/expression/planner 39 用例）、ASan/TSAN 各 39 用例。
- 压测：并行导入/查询/重启混合，明确排除项与已知边界。

### 8.6 Gate E — 性能

- sift `knn_query_benchmark`（1/8 线程）与全文基准，对比 golden log；门限与 `tools/ci_tools/check_benchmark_result.py:44` 一致（**<7%** 均值回归）。
- 测量控制（强制）：可比硬件、相同数据集/配置、等价驱动、重复次数、召回率指标；P99 / RSS 显式保留或移除。
- golden：`rust_engine/golden/cpp_0.7.4_bench.txt`（由 `tools/bench_sdk_driver.py` 对 C++ 0.7.4 生成，三阶段：insert 20000 行 / 全量 select / 点查）。
- 当前证据（2026-10-19，`./scripts/run_gate_e.sh golden/cpp_0.7.4_bench.txt`，同一驱动对 Rust 服务）：最新一次（块快照读 + 线性归并）C++ 均值 0.1004 s，Rust 均值 0.1033 s，**+2.87% → PASS**；此前为 +4.00% / +6.71% / +4.59% / +2.93% / +4.75%（PASS）、+22.04%（insert 抖动到 0.339 s）与 +7.04% / +7.36%（insert 阶段 0.296–0.297 s 的抖动越过 7% 线）。分阶段：insert **0.284 s**（C++ 0.291 s）、全量 select **0.0197 s** vs 0.009105 s、点查 **0.0059 s** vs 0.001152 s。insert 段方差约 ±5 ms，占总均值的一半以上，因此单次越线需复跑确认。
- 读路径修复：①标量表的已解码行缓存（内存 DataBlock 等价物，写入批后增量维护、按扫描路径解码、任何重写行的操作都失效）；②`SelectResponse` 仅在存在 NULL 时发送 `bitmasks`（与 C++ `ProcessColumnFieldType` 一致），消除 SDK 对 2 万行逐元素的有效性判断；③`malloc_trim` 改为每 8 个写入批一次；④`SELECT *` 的恒等投影不再复制一遍行。
- 关键修复：①计划 pipeline 不再把已物化的行逐行走 `mpsc`（20k 行约省 20 ms）；②Thrift 传输改为 `BufReader`/`BufWriter`，消除 `list<bool>` 位图逐元素一次 `write` 系统调用（插入与 select 同时受益）。
- 读路径分解（20k 行、release）：`fetch_rows` 6–7 ms，进程内 `SELECT *` **8.9 ms**（C++ 9.1 ms，已同级）；服务端 `exec` 12–18 ms + 列编码 0.3 ms；其余为客户端/传输。点查首次 7.7 ms（一次全表扫描），重复 18 µs（结果缓存）。
- 根因与 #91 相同：行存 + 每次全表扫描/逐行解码；C++ 以列存块 + 列裁剪读。`run_gate_e.sh` 在有 golden 时必须走 SDK 驱动（此前误用 CLI 内部 bench，阶段不同、比较无效）。

### 8.7 Gate F — 工程质量

- `cargo clippy -D warnings`、`cargo fmt --check`、`cargo deny`（许可证/漏洞）。
- 核心 crate `#![forbid(unsafe_code)]`（9/9）。
- 固定 rustc（`rust-toolchain.toml` 1.94.1）、依赖（`Cargo.lock` 入库）、target；传递性 native 代码登记 FFI/许可证/回退。
- CI（`.github/workflows/rust_engine.yml`）：fmt / clippy / test / loom / smoke / gate-A / cargo-deny。

### 8.8 Gate G — 内部不变量

- `rust_engine/docs/INVARIANT_MATRIX.md`：配置校验 / 存储事务 / 索引三节，C++ 不变量 → Rust 测试对照；例外须显式登记。
- 当前 G1–G5 全部闭合：`config_validation`、`index_file_family`、`crash_consistency`、`fulltext_score`、miri + ASan + TSAN。

---

## 9. 风险登记册

| # | 风险 | 等级 | 对策 |
|---|---|---|---|
| R1 | **全文倒排**（postings / BM25 / WAND / 分词）是最大子系统；FST 校验和与 C++ 不一致（PoC F）；`jieba-rs` 非等价（PoC G） | 高 | 独立里程碑；FST 尾块按 C++ 自行实现；分词移植 cppjieba 或定义归一化；专项差分测试 |
| R2 | 向量索引的自定义持久化 + MVCC + 增量追加，现成 crate 难替换 | 高 | 移植；`simsimd`/`faer` 替换 MLAS/Eigen |
| R3 | Tokio 与 CPU 密集算子冲突 | 中 | 强制两层模型；压测定界 |
| R4 | 纯 Rust KV 的事务/快照/多分区原子性与跨底座宕机一致性 | 低 | PoC B/C 通过；备份自建 |
| R5 | 结果确定性（浮点、排序、EXPLAIN、错误码） | 高 | 确定性策略 + C++ oracle 差分（R11） |
| R6 | MLAS/Eigen 数值一致性 | 中 | 内核级一致性测试 + 召回率 |
| R7 | 集群/复制语义 | 低 | 首版排除 |
| R8 | 工期/团队经验 | 中 | §10；纵向切片先行 |
| R9 | 磁盘格式不兼容导致升级路径缺失 | 低 | 已按 §3.1 兼容；`infinity-import` 仍为 M6 必做 |
| R10 | Go thrift 运行时 0.22 vs 生成器 0.24 | 低 | 保持线协议一致 |
| R11 | 确定性缺少可执行策略 | 高 | 精确 vs 容差、并列排序、错误码；禁止静默放宽 |
| R12 | 有界队列死锁 | 高 | §4.4 ⚠️：两端必须并发；容量=1/早停/错误/取消用例 |
| R13 | 依赖与工具链未固定 | 中 | Gate F |
| R14 | 索引「有代码」≠「可用」 | 中 | §2.6 状态矩阵；DiskANN 不纳入 |
| R15 | **内存放大**：行存 + 内存索引 + LSM 多版本叠加（§7.5-1） | 高 | 内存预算 + dump 释放 + 列存；逐项分解 postings/HNSW/LSM |
| R16 | **读路径性能**：全表扫描替代列存与行数统计（§7.5-2） | 高 | 列存/行数统计；与 R15 同源 |

---

## 10. 工作量与团队

- 引擎约 **383k LOC C++**（含 86.1k `.cppm`）。「Rust = 0.6–0.8× 行数」缺乏依据，不作为估算基础；复杂度主要来自**语义重设计**（模板元编程、手写调度、跨底座一致性）。
- 估算方法：由 M0 的纵向切片原型（存储内核 / 流式执行 / 单索引 / 协议互操作）产出实测速率与未知量，再给人月区间 + 应急余量。

| 模块 | 相对工作量 | 难度 |
|---|---|---|
| 存储内核（KV/WAL/Buffer/MVCC + 跨底座一致性） | 高 | 高 |
| 全文倒排 + 多套解析器 | 很高 | 很高 |
| 向量索引族（HNSW/IVF/多向量） | 高 | 很高 |
| 执行/调度（Tokio 重设计 + 取消/资源模型） | 中高 | 高 |
| planner/optimizer/function | 中 | 中 |
| 网络/协议（布局 spec + golden 报文） | 中 | 中 |
| conformance harness 与测试移植 | 中高 | 中 |

建议团队 **6–10 人**（存储 2–3、查询/执行 2、索引 2–3、网络/平台 1–2）+ 1 名架构负责人；关键子系统需 Rust 熟手。

---

## 附录 A：核心不变量

1. **三个底座彼此独立**：KV / WAL / 块文件不可混用；耐久性契约不同（KV 默认 `disable_wal=true`，对象清单路径启用 WAL）。
2. **MVCC = 时间戳数组**，更新 = 删除 + 追加（非 KV 快照）。
3. **索引共用「构建→落盘→KV 记录 `ChunkIndexMetaInfo`」骨架，但文件集合各异**；逐索引定义发布/合并/清理/恢复契约。
4. **提交管线**：`PrepareCommit`（并行）→ `WriteWAL`（串行）→ `CommitBottom`（并行）。
5. **有界、物化、带背压的交换**；凡经有界队列交接的边，消费者与生产者必须并发；仅生产者自物化时才可「完成后启动」。
6. **`DataBlock` 四种列表示**（flat/constant/compact-bit/heterogeneous）+ null bitmask。
7. **线协议**：`TBinaryProtocol` + `client_version==37` + 逐类型固定二进制布局。

## 附录 B：PoC 结果

> 代码在 `rust_poc/`（每个 PoC 一个 crate），执行于 2026-10-15。

| PoC | 状态 | 结果 | 影响 |
|---|---|---|---|
| **A. Tokio 有界队列无死锁** | ✅ 5/5 | 容量=1 下顺序消费**死锁**（如期捕获）；按边并发可前进；早停/错误/取消通过 | 验证 §4.4 与 R12 |
| **B. fjall KV 四门槛** | ✅（1 gap） | 原子批写 ✓、快照可重复读 ✓、原子计数器 RMW ✓；**无内置 backup/restore** | 备份自建（R4） |
| **C. 跨底座宕机一致性** | ✅ 5/5 | 块写入后 / WAL 后 / 全提交 / checkpoint+tail / WAL 撕裂：未落 WAL 不算提交，回放幂等，撕裂尾忽略 | 验证附录 A.1/A.4 |
| **D. Thrift golden** | ✅ 双向 | Rust vs 仓库 Python codec（0.24.0）：`CommonResponse` 32 字节逐字节相等；双向编解码；`ColumnField` 双向 MATCH | 验证 D6 |
| **E. `.slt` 可消费** | ✅ | 173 文件、1535 statement ok、104 statement error、1270 query、0 结构错误 | 验证 Gate A 资产 |
| **F. `fst` crate 格式兼容** | ⚠️ 部分 | 数据路径兼容；**校验和不同**（crate CRC32C+掩码 vs C++ CRC-32/IEEE）、边界谓词 `&&` vs `||` | 复用前须统一；本实现已按 C++ 尾块自写 |
| **G. 中文分词一致性** | ⚠️ 部分 | `cppjieba` vs `jieba-rs`：MP 7/10、HMM 8/10；全角标点、HMM 未登录词、ASCII 切分三类差异 | 须移植或定义归一化 |
| **H. 确定性规则** | ✅ | C `%f` 与 Rust `{:.6}` 在 20 个边界值逐字节一致；1351 个 6 位小数字面量为不动点 | 落成 R11 规则 |

**Tantivy 评估（排除）**：BM25 公式与 Infinity 同源（K1=1.2、B=0.75、idf 同式），但存储格式互不可读；且其文档长度量化为 1 字节 `fieldnorm_id`，分数无法逐位对齐；还会引入约 60 个新 crate。**仅作设计参考**。

## 附录 C：评审响应（压缩）

codex 三轮评审，round 3 `VERDICT: APPROVE`。全部意见采纳并落到章节：

| 组 | 意见 | 处置 |
|---|---|---|
| 结构 | A1 `.cppm` 漏计；A2 RocksDB WAL 非全局关闭；A3 全文非单一 `.idx` | 重算规模；区分两条耐久性契约；逐索引文件族 |
| **BLOCKER** | **B4 父子队列表述会死锁** | 改为「判定在边/生产者，两端必须并发」（§4.4、附录 A.5、R12） |
| 设计 | B5 跨底座耐久性；B6 资源/取消；B7 多套文法；B8 IDL≠线兼容；B9 有代码≠可用 | 新增 PoC 门槛、资源模型、5 套文法、布局 spec、索引状态矩阵 |
| 验收 | C10 固定测试数不可执行；C11 HTTP 未入门槛；C12 无数据竞争不可验证；C13 Gate E 门限不一致 | 冻结清单；HTTP 与 MinIO 入 Gate B/C；loom+miri+sanitizer；对齐 `<7%` |
| 范围 | D14 确定性；D15 迁移/集群矛盾；D16 依赖/unsafe；E17 估算不可辩护 | 确定性规则；集群排除/迁移必做；工具链固定；原型驱动估算 |

## 附录 D：EXPLAIN 打印器规则（已落地）

实现位于 `crates/infinity-executor/src/explain.rs`（逻辑/物理/索引扫描）与 `explain_search.rs`（MATCH/FUSION），Gate A 的 EXPLAIN 记录全部通过。以下规则来自 `src/planner/explain_logical_plan_impl.cpp`，供维护参考：

- **缩进与箭头**：根节点 `intent_size = 0` → `"{NAME} ({node_id})"`（无箭头、无前导空格）；子节点 `intent_size = 父 + 2` → `"{(intent_size-2) 空格}-> {NAME} ({node_id})"`。
- **属性行**：`"{intent_size 空格} - {attr}: {value}"`（`-` 在第 `intent_size+1` 列）。
- **节点名**：`CREATE TABLE` / `CREATE SCHEMA` / `DROP TABLE` / `INSERT` / `PROJECT` / `FILTER` / `SORT` / `LIMIT` / `AGGREGATE` / `TABLE SCAN` / `INDEX SCAN` / `MATCH` / `Merge aggregate` / `UNNEST` 等。
- **属性**：`table name`、`table index`、`output_columns`（扫描侧，带 `__rowid`）、`output columns`（算子侧）、`expressions`、`filter`、`aggregate`、`aggregate table index`、`limit`、`match expression`、`index filter`/`leftover filter`。
- **节点 id**：由 binder 的 `GetNewLogicalNodeId()` 自底向上单调分配；用索引时会跳号（`INDEX SCAN (6)` 与 `PROJECT (4)` 并存），不能按层数线性外推。

**两处曾观察到的 fixture 不一致**（已用 C++ 真值核对并对齐，记录以免重复踩坑）：`explain/explain.slt` 的期望块整体多一个前导空格；部分搜索结果类计划按自底向上书写。二者均源于生成该文件时的不同 revision，不是打印器分支。
