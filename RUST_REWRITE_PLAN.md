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

- 不追求与旧版本**磁盘格式**兼容（新引擎使用新存储格式；提供导入工具或要求重灌数据，见 §3 D2）。
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
- **Gate A 基线**（`infinity conformance <test/sql>`，把现有 `.slt` 直接跑在 Rust 引擎上）：**173 文件**、`statement ok` **1744/1746**、`statement error` **34/129**、`query` **1111/1532**。即**当前一致率**（仍远未达标）——量化了与 §6 Gate A（要求 100%）的差距。
  - **里程碑轨迹**：`statement ok` **404 → 760 → 1696/1746**、`query` **71 → 136 → 1111/1532**。本轮（2026-10-16）新增能力：①**通用表达式层**（`ExprAst`：列/字面量/函数调用/**算术 `+ - * / %`（含 `*`，带词法歧义消解）**/**`CAST`/`TRY_CAST`**）+ 布尔谓词树（`AND/OR/NOT`、括号、列间比较、`IS [NOT] NULL`、`LIKE`、**`[NOT] IN (…)`（数值隐式转换）**）；②**函数/聚合扩容**（`ROUND/SQRT/ABS/CEIL/FLOOR/UPPER/LOWER/LENGTH`、`FIRST/LAST`、**`json_extract*`/`json_contains`/`json_exists_path`**（自研 JSON 解析 + 路径 + 紧凑序列化）；未知函数→`NULL`）；③**数组/稀疏/集合字面量**（`[…]`、`[i:v]`、`{…}`、嵌套 `[[…],[…]]`）、参数化类型、`DATE/TIMESTAMP '…'` 与 `TRUE/FALSE` 字面量；④**`SEARCH MATCH` 端到端**：`VECTOR`（`l2/ip/cosine`）+ **`SPARSE`（内积）** + **`TENSOR`（MaxSim；`TensorArray` 逐张量取 max；`bit` 用 `-hamming`）**，均支持 `WITH (threshold=…)`、`WHERE`（外层与括号内）**先过滤后 top-N**、`USING INDEX`，伪列 `ROW_ID()/SIMILARITY()/DISTANCE()/SCORE()`；⑤**`COPY … FROM`** 真实 CSV 装载 / `COPY … TO` 导出（fixture 路径映射、`DELIMITER '\t'`、向量/张量规范化）；⑥**DDL/DML**：`DEFAULT`/`NOT NULL`（校验）、`ALTER TABLE ADD/DROP COLUMN`、`INSERT` 列清单对齐 + 缺列补默认值、`DROP TABLE/INDEX [IF EXISTS]`、`SELECT … AS`、**常量 `SELECT`（无 FROM）**、`CREATE SNAPSHOT`/`ALTER <index>`/`SET/RESTORE/COMPACT/DUMP` 容错、**聚合表达式两阶段求值**（`Aggregate` → `Projection`，支持 `MAX(c1)+SUM(c2)`）、**`SELECT UNNEST(json_array)`** 展开；⑦**NULL 渲染统一为 `null`**（与 `.slt` 字面量一致）。
  - **一致性硬伤修正**：conformance harness 原先会**整条丢弃**以 `#` 注释开头的记录（导致注释紧邻的 `COPY` 未执行），已改为**逐行剔除注释**（记录总数由 1270→1532）；新增**按文件失败归因**输出，便于定位。
  - **仍待收窄**：`statement error` 由 69→34 是「宽松容错」的代价；`TEXT`/`FUSION` 打分、`EXPLAIN` 计划文本、float16 张量量化尚未实现。
  - **全文打分的调查结论（2026-10-16，负结果）**：以 `test/sql/dql/fulltext/fulltext_keyword.slt` 反推 `SCORE()` 模型——`doc^4.5` + `'multiple another'` 对 1/2 个命中词均给 **4.5**（疑似按字段封顶），而 `doc2` + `'second default text'`（`similarity=boolean`）对 1/2 个命中词给 **1.0 / 2.0**（疑似命中词数×boost）。两者互相矛盾；默认相似度下按 BM25（k1=1.2，见 `src/planner/bound_select_statement_impl.cpp:237`）手算 0.34，与实测 1.0 不符；`analyzer=keyword` 的分词边界与 block-max/WAND 影响亦无法从测试反推。**结论：必须逐行移植 iresearch 的相似度/分词（R1）才能对齐打分，本阶段不做近似**。已实现的是**非打分**全文路径：`filter_text`/`filter_fulltext` 词元匹配、`regex(col,pat)`（自研正则引擎）、`SEARCH MATCH TEXT` 的解析（不含打分排序）。
- **跨语言 wire 验证（十三项）**：①`Connect`；②`list<ColumnField>`；③`Select` REPLY；④`CreateTable`；⑤`Select *`；⑥`Insert`；⑦`Select` 列投影；⑧`Select` 过滤；⑨`Select` `ORDER BY…LIMIT`；⑩`ListDatabase`；⑪`Delete`；⑫`Update`；⑬**`ShowTable`**（`cols=2 rows=2`）。全部返回 `PY_*_OK`。
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

### Gate A — SQL 功能（硬门槛）
- **全部 `test/sql/**/*.slt` 通过**（内联 `----` 字面量比对，`rowsort` 等修饰符语义一致）；无静默放宽、无 skip 新增。
- 关键点：浮点格式化（如 `ROUND(SCORE(),4)`）、`rowsort` 排序稳定、`statement error`（仅需报错）、`explain` 计划文本结构（含 operator id/内部列，见 Gate D 与 §附录 C）。

### Gate B — SDK 协议兼容（硬门槛，核心验收面）
- **`python/test_pysdk` 全部用例通过**（连 23817，未修改 SDK）。
- **`go/tests` 全部用例通过**（`go test ./tests/...`，未修改 SDK）。
- **HTTP 模式（`--http`）全部用例通过**（`tools/run_http_api.py`，与 CI 一致）。
- 前置：Thrift 线兼容（`TBinaryProtocol` + `client_version==37` + **逐类型二进制布局 spec**）；**跨语言 golden 报文**测试通过。

### Gate C — 持久化与恢复
- `python/restart_test` 通过；`test/sql/snapshot` 通过；**WAL 崩溃恢复**用例通过。
- **对象存储 / MinIO** 用例通过（对应 CI 的 minio job）。
- **compaction 后恢复**与**恢复期间再次崩溃**用例通过（针对跨底座一致性契约）。

### Gate D — 并发与稳定性
- `python/parallel_test` 通过；`test_cluster` 仅在「集群纳入范围」时作为门槛（见 §5 范围冻结）。
- Rust 侧（**替代不可验证的「cargo test 无数据竞争」表述**）：
  - **有界 `loom` 模型**：覆盖调度/通道/取消的核心状态机（容量=1、早停、错误、取消）。
  - **`miri`**：跑**不依赖 native/FFI** 的纯 Rust 核心单测（明确排除 FFI 依赖项）。
  - **native sanitizer**：对含 FFI/SIMD 的模块保留 TSAN/ASAN 覆盖（与 C++ 现状对齐）。
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
