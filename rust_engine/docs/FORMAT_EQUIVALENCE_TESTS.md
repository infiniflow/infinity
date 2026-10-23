# Rust 重写版的持久化格式：现状、与 C++ 参考的差异、测试覆盖

> 判定口径与 [`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md) 一致：C++ 参考能打开重写版的目录、重写版能打开参考的目录，查询结果一致。本文在此之上记录更强的**字节**口径、当前的覆盖范围，以及重写版与参考仍然存在的差异。差异的成因分类见 [`SIMPLIFICATION_AUDIT.md`](SIMPLIFICATION_AUDIT.md)。
>
> **现状**：同一输入下，重写版写出的块文件（`.col` / outline / `version`）、全文索引、次级索引（整数键与浮点键）、稀疏 BMP 索引与参考逐字节相同；HNSW、IVF 按内容比对；`catalog/`、`wal/` 与对象存储只保证互读。逐条差异见 §6。

---

## 1. 三个等价层级

| 层级 | 断言 | 能发现什么 | 不能发现什么 |
|---|---|---|---|
| **字节门禁** | 同一输入，两引擎写出的文件逐字节相同 | 布局、对齐、编码、排序、填充的任何漂移 | 两边以同样方式写错的语义（不会发生，参考是基准） |
| **内容门禁** | 不可字节比的文件，解码后比「索引说了什么」 | 索引内容错、行序错、统计量错 | 文件头/元数据区的布局漂移 |
| **互读门禁** | A 写 → B 打开 → 查询答案一致 → B 继续写 | 读路径、replay、增量写 | 只有纯字节层面的漂移 |

三层缺一不可：互读全过而字节不同，说明两端对同一份数据的编码不同；字节相同而互读失败，说明解码路径没跟上编码。

## 2. 文件分类（实测）

`test_the_reference_is_deterministic_where_the_test_assumes_it` 让参考在**同一输入**上跑多次，统计哪些文件的字节在变；`test_the_port_is_deterministic_too` 对重写版做同一件事。下表由这两个测量支撑。

| 类别 | 文件 | 依据 |
|---|---|---|
| **确定性** | 块列 `*.col`、变长 outline `col_*_out`、全文 `.dic/.pos/.len`、稀疏 BMP chunk、次级索引 chunk（整数键与浮点键）、`version`（含 ts） | 只由行值/行序决定；参考自身多次运行字节相同。次级索引的 PGM 拟合也是确定的：整数键走 `__int128` 精确算术，浮点键走 `double` 算术（`LargeSigned<T>` 对浮点 `T` 就是 `double`，`piecewise_linear_model.hpp:40`）；浮点拟合的截距对**编译选项**敏感（见下），重写版按参考的默认编译口径复刻。每层顶部的守卫段取 `nextafter(last, +inf)`（`pgm_index.hpp:104-130`） |
| **参考不确定** | HNSW chunk 的**图区** | 参考用 `thread_local std::mt19937`（`src/storage/knn_index/hnsw/hnsw_alg.cppm:107`），哪个线程插入哪条向量、因而哪个节点拿到哪一层由调度决定；5000 行时参考 5 次跑出 5 个不同的图 |
| **参考不确定** | IVF chunk | 参考用 `std::random_device` + `std::mt19937` 采样 k-means 训练集（`ivf_index_data_impl.cpp:212-216`），质心、连同质心个数都不是行值的函数；同一输入两次跑出两块同尺寸不同内容的 chunk |
| **不可比** | `catalog/`（RocksDB）、`wal/`、MinIO 桶内布局 | 带 session id、LSN、提交 ts；参考对自己都不可复现。只比**逻辑内容**（`SHOW CATALOG 'all'`）与互读 |

`version` 文件：布局相同，ts 也相同。`created_` 的 `create_ts` 是填块那次 append 的**提交 ts**，删除槽里是删除事务的提交 ts，两个引擎对同一句脚本消耗同一条 ts 序列（启动引导、首个 checkpoint、`default_db`、每条 DDL、每次 append/delete/dump/checkpoint，见 §6.6），所以整个文件逐字节比较。唯一会破坏这个前提的是参考的**配额驱动 dump**：它是后台线程上的写事务，ts 在 worker 真正提交时才分配，实测会落在 append 与随后的 DELETE 之间或之后（同一脚本两次跑出不同的 `version`）。因此比较 ts 的用例把 `[buffer] memindex_memory_quota` 设成内存索引永远达不到的值（`"2GB"`；参考的字节数解析器拒绝 `"0"`），只保留脚本里显式的 `DumpIndex`。`idx_chunk` 记录里的 ts 仍只有参考能命名（见 §6.1），门禁只对该值做归一化。

浮点键 PGM 的截距依赖参考的**编译选项**。clang 默认 `-ffp-contract=on`，参考以 `-O3` 编译时会把 `piecewise_linear_model.hpp` 里 5 处乘加（`get_intersection` 的 `a`、`b`、`i_x`、`i_y` 与 `get_floating_point_segment` 的截距）收缩成 FMA；同一份源码用 `-ffp-contract=off` 编出来，`FLOAT` 键 65 行线性输入的第一个截距是 32 而不是 31。重写版用 `f64::mul_add` 复刻收缩后的舍入，因此对齐的是**参考默认编译口径**下的字节；这条差异由 `float_intercept_matches_contracted_reference_fit` 钉住。

重写版的 **HNSW 图与 IVF 质心都确定**（插入顺序即行序，无随机源），所以重写版的自复现测量里**所有**文件都必须相同——包括参考自己都复现不了的那一块。

## 3. 测试清单

下表就是门禁的全部入口，每条都必须通过。

| 测试 | 数据集 | 断言 |
|---|---|---|
| `test_cross_engine_bytes.py::test_the_two_engines_write_the_same_bytes` | 200 行 → 1 块，5 类索引 | 块文件逐字节相同；每个索引按其类别比对（全文/次级/稀疏整文件，HNSW 头+向量+标签）；目录逻辑记录相同；索引答案相同 |
| `::test_the_blocks_match_across_many_blocks` | 20000 行、无索引 → 3 块（8192/8192/3616） | 每块每列的 `.col`、outline、每块 `version` 逐字节相同 |
| `::test_the_chunk_boundaries_are_timing_dependent` | 100 行 + 100 行两次 flush（一个块） | chunk 覆盖行区间无空洞、无重叠、总数 200；**不**断言固定的 chunk 数 |
| `::test_the_columns_match_for_every_type` | 16 类列 × 200 行，含 NULL 与极值 | 每个 `.col` 与 outline 逐字节相同（NULL 槽按位图屏蔽） |
| `::test_the_reference_is_deterministic_where_the_test_assumes_it` | 200 行，参考跑 3 次 | 只有 HNSW chunk 允许变；其余必须自复现 |
| `::test_the_port_is_deterministic_too` | 200 行，重写版跑 3 次 | `data/` 下**每个**文件都必须逐字节相同（含 HNSW 图与 IVF chunk） |
| `::test_the_bytes_after_deletes_updates_and_a_restart` | 100 行 + 第二次运行 DELETE 20 / UPDATE 10 | 块仍逐字节相同（210 行、3 次 create 含 ts、30 个标记槽）、全文与浮点次级 chunk 相同、catalog 记录（键与值，`idx_chunk` 的值除外）相同 |
| `::test_the_index_chunks_match_for_every_index_kind` | IVF + HNSW(cosine) + HNSW(ip) + 全文(chinese) + 次级(int) + 次级(date) | 六类索引按各自类别比对；`_wait_for_index_chunks` 等全部 chunk 落盘 |
| `::test_the_ivf_chunk_is_only_compared_by_content` | 上面的表，参考跑 2 次 | IVF chunk 的字节不是行值的函数，只能按内容比 |
| `::test_a_crash_leaves_a_replayable_directory` | 100 行 flush + 50 行未 flush 后 kill -9 | 另一引擎启动后看到 150 行（双向） |
| `::test_a_compacted_table_keeps_its_segments_apart` | 8192 行 + `COMPACT` + 50 行 | 重写版把行写进 `seg_1`/`seg_2`；非 0 段的块与参考同行的块逐字节相同；两个方向互读 8242 行；重写版重启不重复 |
| `::test_the_port_round_trips_through_object_storage` | 200 行 + `storage_type="minio"` → 清空本地目录 → 读回 → 再写 50 行（在恢复出来的树上做 checkpoint）→ 再清空 → 读回 | 重写版从桶里恢复 `data/`+`catalog/`+`persistence/`，行数 200；第二次 checkpoint 后是 250（需要 `INFINITY_MINIO_URL`；每次跑用新桶，避免上一轮的桶污染） |
| `test_cross_engine_compat.py` | 200 行，4 类索引 | 双向交接：A 写 → B 读 → B 再写 |
| `infinity-storage/tests/s3_minio.rs` | 本地临时目录 + 桶 | `tree_sync_and_restore`（往返 + 二次同步上传 0 个文件）、`same_size_edit_is_uploaded`（同长度改写必须重传）、`put_get_list_delete_roundtrip`；需要 `INFINITY_MINIO_TEST=1` |

## 4. 覆盖矩阵

行是格式单元，列是场景。`✅` 已覆盖且为字节门禁，`◐` 按内容 / 互读比对，`⬜` 未纳入门禁（原因见 §5）。

| 格式单元 | 单块单 chunk | 多块 / 多 chunk | 多段 | 删除 / 更新 | 重启后再写 | 崩溃恢复 | 类型 / 选项矩阵 | 对象存储 |
|---|---|---|---|---|---|---|---|---|
| 数据文件头尾（magic / buffer_size / checksum） | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ 互读 | — | ◐ 重写版自身往返 |
| 块列 `*.col`（`[data_region][null_bitmap]`） | ✅ | ✅ | ✅ 非 0 段块 | ✅ | ✅ | ◐ 互读 | ✅ 16 类 | ◐ |
| 变长 outline `col_*_out` | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ | ✅ | ◐ |
| `version` 文件 | ✅（含 ts） | ✅ | ✅ | ✅ | ✅ | ◐ | — | ◐ |
| 全文 `.dic/.pos/.len` | ✅ | ✅ | ⬜ 跨段合并 | ✅ | ✅ | ⬜ | ✅ analyzer / `with_positions` | ◐ |
| HNSW chunk | ◐ 图按内容 | ◐ | ⬜ | ⬜ | ⬜ | ⬜ | ✅ metric / build_type | ◐ |
| IVF chunk（plain / SQ / PQ） | ◐ 按内容 | ◐ | ⬜ | ⬜ | ⬜ | ⬜ | ◐ plain only | ◐ |
| 次级索引 chunk（整数键） | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | ✅ int / date 键 | ◐ |
| 次级索引 chunk（浮点键） | ✅ | ✅ | ⬜ | ✅ | ⬜ | ⬜ | ⬜ double 键 | ◐ |
| 稀疏 BMP chunk | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ 词表规模 / block_size | ◐ |
| 删除位图（catalog 内） | ◐ 逻辑 | ◐ | ◐ | ✅ | ✅ | ◐ | — | — |
| WAL 记录 | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ✅ 跨引擎 replay | — | ◐ |
| Catalog KV | ◐ 逻辑 | ◐ | ◐ | ✅ | ✅ | ◐ | — | — |

"对象存储"列的 `◐` 一律读作**重写版自身往返**（写 → 清空 → 读 → 再写 → 再读），不是跨引擎比对；跨引擎不成立的原因见 §6.5。

## 5. 未纳入门禁的部分

### 5.1 不做字节比对（有实测理由）

1. **HNSW 图区**。参考用 `thread_local` 随机数，图本身不是唯一确定的；重写版的图是确定的，但两端不可能相同。
2. **IVF 训练**。参考的 k-means 采样走 `std::random_device`，质心与质心个数都不是行值的函数。
3. **`catalog/` 与 `wal/`**。带 ts / LSN / session id。
4. **MinIO 桶内布局**。键名由两端各自的持久化层决定（见 §6.5）。

前两项的结论是**不追求字节可比**，理由在 §6.4：要复刻 libstdc++ 的 `mt19937` + `uniform_int_distribution` + 线程插入顺序，等于把实现细节写进格式。

### 5.2 输入空间的缺口

1. **跨段全文合并**：重写版把每个索引的 chunk 都写在 `idx_{id}/seg_0/`，不按段拆；参考在测试规模上只有 `seg_0`，所以没有可观测差异。要冻结这条需要 8,388,608 行级别的数据（每引擎约 1024 个块目录），本机内存与磁盘放不下两端。
2. **多段下的索引类**（矩阵"多段"列的 ⬜）：HNSW / IVF / 次级 / 稀疏都只有单段覆盖，原因同上。
3. **HNSW / IVF 的删除、更新、重启、崩溃恢复**：图区/质心不可字节比，这几个场景需要按内容比，目前没有用例。
4. **次级 / 稀疏索引的删除、更新、重启、崩溃恢复**：可字节比，目前没有用例。
5. **WAL 的字节级比对**：只做跨引擎 replay，不比字节。

### 5.3 流程

1. **CI**：`.github/workflows/rust_engine.yml` 的 `cross_engine` job 在 builder 镜像里编参考、用 `--features rocksdb-store` 编重写版，并在 bwrap 下跑两个 pytest 文件。
2. **重写版自身确定性**：由 `test_the_port_is_deterministic_too` 单独断言（3 次跑，`data/` 下每个文件都必须相同）。

## 6. 与 C++ 参考的差异

### 6.1 字节层面的差异

| 差异 | 重写版 | 参考 | 门禁处理 |
|---|---|---|---|
| `idx_chunk\|…\|{chunk_id}` 的值 | 封住该 chunk 的那次 `DumpIndex` 的提交 ts（`dump_ts:{表}:{索引}` 记录，checkpoint 镜像与 dump 当次都会写） | 同上。参考的 dump 由后台 `DumpIndexProcessor` 执行、全文索引再转到构建 worker，且 `flush_data` 的 checkpoint 异步提交，实测同一脚本两次给出 22/24/26/28/32（checkpoint 在 30）与 22/24/26/28/30（checkpoint 在 32）；重写版同步按调用序 dump（24/26/28/30/32） | 只把这个值归一化成 `<ts>`；键与其它记录逐字节比。两边都是真实的 dump 提交 ts，差异是 dump 排期 |
| HNSW 图区 | 确定（插入顺序即行序） | `thread_local std::mt19937`，节点层级取决于线程调度 | header / vectors / labels 字节比，图区与查询答案按内容比 |
| IVF 质心 | 确定 | `std::random_device` 采样，质心个数也不固定 | chunk 按内容比 |
| `catalog/`、`wal/` | RocksDB；redo 记录写 `wal/wal.rust`，replay 时同时读参考的 `wal/wal.log` | RocksDB；写 `wal/wal.log` | 只比逻辑内容与互读 |
| 对象存储桶内键 | 路径树 `data/`、`catalog/`、`persistence/` | `ObjCreate` 打包 blob，本地路径映射在 KV 的 `pm\|object\|<path>` | 只做重写版自身往返 |
| 索引 chunk 的段目录 | 全部在 `idx_{id}/seg_0/` | 路径同样带 `seg_{id}`；多段时是否按段拆分未验证 | 8M 行以下无差异，跨段未验证 |
| 段的产生时机 | `COMPACT` 即写出 `seg_1`/`seg_2` | 按 `DEFAULT_SEGMENT_CAPACITY = 8,388,608` 行切 | 见 §5.2 |
| 不带 `rocksdb-store` feature 时 | catalog 落 fjall | 只认 RocksDB | 参考打开只剩 `system_tag`（`Database: default_db doesn't exist`）；门禁必须带 feature |

### 6.2 与参考逐字节一致的部分

- 块列 `*.col`（16 类列，含 NULL 位图、`-0.0`、极值、outline 分界）与变长 outline；
- `version` 文件整体（`[create_size]` + 每次 create 的 `[ts][累计行数]` + `[capacity]` + 每槽 u64，DELETE 标槽，UPDATE = 删除 + 追加）：布局一致，且 `create_ts` 与删除槽 ts 就是事务自己的提交 ts，两边逐值相同（§6.6）；
- 全文 `.dic/.pos/.len`、次级索引 chunk（整数键与浮点键，含 PGM 模型）、稀疏 BMP chunk；
- JSON 列按 BSON 落盘（`{"data": <值>}` 外层、键排序、小整数 `int32`）；
- 浮点列按精确值落盘，只有 SELECT 输出层按 6 位小数格式化。

### 6.3 参考的非格式行为（决定门禁怎么写）

`MemIndexTracer::MakeDumpTask`（`memindex_tracer_impl.cpp:130`）每轮只提交"全部 EMVB 索引 + **至多一个**非 EMVB 索引"，dump 又在后台线程，所以**同一输入在不同负载下 chunk 的切分不同**：

| 场景 | 全文 / 次级 / BMP | HNSW |
|---|---|---|
| 100 行 flush + 100 行 flush（单独跑 3 次） | 1 个 chunk（200 行） | 2 个 chunk（各 100 行） |
| 同样的输入在整轮测试里跑（机器上还有别的引擎） | 2 个 chunk（各 100 行） | 2 个 chunk |
| 20000 行单独跑 | 3 个 chunk（8192/8192/3616） | 3 个 chunk |
| 20000 行在整轮测试里跑 | 1 个合并 chunk（`ft_..._4e20`，20000 行）+ 4 个次级 chunk | 3 个 chunk |

因此 chunk 的数量/边界不能作为字节门禁：重写版只保证切分确定且不重不漏（200 行规模上参考稳定，那里仍整文件比对）。`_seed` 在 `flush_data()` 之后对每个索引显式 `dump_index()`（参考的 `DumpMemIndex`），再用 `_wait_for_index_chunks` 轮询到全部 chunk 落盘，不依赖固定 sleep。

### 6.4 为什么不做 HNSW 图的字节可比

要让两引擎产出同一个图，重写版必须逐位复刻 `std::mt19937`、`std::uniform_int_distribution` 的取模方式、以及**线程插入顺序**。随机数和线程调度都不是格式的一部分。

### 6.5 对象存储的跨引擎差异（有测量）

- 参考的桶内键是**打包对象**：`ObjCreate()` 命名的 blob（`object_size_limit_` 触发的 8 MB compose 包），本地路径与 `(obj_key, offset, size)` 的映射写在 KV 的 `pm|object|<path>` 记录里（`KeyEncode::PMObjectKey`，`persistence_manager_impl.cpp:123`），对象统计另记 `pm|object_stat|`。
- 重写版的桶内键是**路径树**：`data/<相对路径>`、`catalog/<相对路径>`、`persistence/<相对路径>`（`infinity_storage::s3::sync_tree`）。
- 参考**没有恢复路径**：`VirtualStore::DownloadObject` 在 `src/` 里没有调用者；把参考的本地文件清空后重启，它记的是 `Init a new catalog` + 空表（日志里只有 `Uploading object meta/...`）。参考的对象存储是"备份"，不是"主存储"。

所以这一列只能覆盖重写版自己的往返；跨引擎互读需要一个重写版的打包对象存储实现（`ObjCreate` + `pm|object|` + `pm|object_stat|` + 对齐/合包），且在参考能自恢复之前没有可比的另一端。

### 6.6 时间戳：两个引擎消耗同一条序列

时钟按参考的**双指针偶数制**走（`new_txn_manager_impl.cpp`）：

| 项 | 参考 | 重写版 |
|---|---|---|
| 指针 | `current_ts_`（最后一个完成提交的 ts）+ `prepare_commit_ts_`（提交者取值的高水位） | 同（两个 `AtomicU64`） |
| 开事务 | `begin_ts = current_ts_ + 1` | 同 |
| 写事务提交 | `prepare_commit_ts_ += 2`，提交 ts 即新值 | 同 |
| 只读事务提交 | `commit_ts = current_ts_ + 1`，两个指针都不动 | 同 |
| 冲突 | 先取 ts 再查冲突，回滚也照样占掉这个 ts | 同 |
| 提交顺序 | `CommitBottom` 按提交 ts 推进 `current_ts_` | 同 |
| 重放 | `CommitReplayTxn` 把两个指针置成该 entry 的提交 ts | 同 |
| 起始值 | `start_ts` = 最新 checkpoint frame 的提交 ts | 同（`max(checkpoint 提交 ts, KV 的 applied 标记)`） |
| checkpoint | 一个写事务：命令带 `begin_ts`（奇数，`NewTxn::Checkpoint` 拒绝偶数），frame 的提交 ts 是它 +1 | 同 |
| DDL | 每条 DDL 一个写事务（`kDDL`） | 同：catalog 写收集成批，提交前按提交 ts 重盖（`Catalog::restamp`），typed WAL 一条 |
| 启动 | 引导（ts 2）、首个 checkpoint（ts 4）、首次连接建 `default_db`（ts 6）各占一个写事务；重启时 `SetNewSystemTS` + `BeginRecoveryTxn`（`prepare_commit_ts_ = current_ts_ + 2`）再跑一次 checkpoint | 同 |
| `DumpIndex` | 每次一个写事务（`DumpIndexProcessor::DoDump` 的 `kDumpMemIndex`），索引没有内存尾巴也照样占一个 | 同（`Engine::dump_index_transaction`） |
| UPDATE | 删除 + 追加，同一个事务 | 同（`commit_with_deletes(…, appended = 1, …)` 把两个 ts 都记进 `version`） |

所以同一句脚本的 trace 现在**逐值相同**。实测（200 行、5 个索引的字节用例，WAL 的提交 ts）：

```
参考   : 4=checkpoint(启动) 6=CreateDatabase 8=CreateTable 10/12/14/16/18=CreateIndex
         20=Append 22/24/26/28=DumpIndex 30=checkpoint 32=DumpIndex 36/40=checkpoint
重写版 : 同一串
```

第二个进程接着写同一目录（append 100 / delete 20 / update 10 / flush / dump ×2）：

```
参考   : 24=checkpoint(启动) 26=Append 28=Delete 30=Update 32=checkpoint 34/36=DumpIndex
重写版 : 同一串
```

这些 ts 落到字节里的地方因此全部逐字节相同：块 `version` 的 `create_ts` 与删除槽、`catalog|seg|…` / `catalog|blk|…`（开段/开块的 append）、`idx_seg|…`（把索引与该段配对的事务 = `max(index 创建 ts, segment 创建 ts)`）、`catalog|tbl|…|{ts}` / `catalog|idx|…|{ts}` / `tbl|col|…|{ts}` 键里的 ts。

两处例外：

* **参考的配额驱动 dump**（§2）：`BGMemIndexTracer::IncreaseMemoryUsage` 在后台线程上开写事务，ts 在 worker 真正提交时才分配，实测会落在 append 与随后的 DELETE 之间或之后。这不是格式，比较 ts 的用例把 `memindex_memory_quota` 设成内存索引达不到的值，只保留脚本里显式的 `DumpIndex`。
* **`idx_chunk|…|{chunk_id}` 的值**：两边都写产出该 chunk 的 dump 事务的提交 ts（重写版在 `DumpIndex` 时记下并盖到已发布的 id 记录上，同时把 `wal.log` 的 checkpoint 帧推进到覆盖它，否则参考读目录时会按 `chunk_ts > begin_ts` 把 chunk 藏起来）。差异只在**哪个索引拿到哪个 ts**：参考的 dump 走后台队列、全文索引再延后，`flush_data` 的 checkpoint 又是异步的；实测同一脚本两次得到 22/24/26/28/32（checkpoint 30）与 22/24/26/28/30（checkpoint 32），重写版同步按调用序得到 24/26/28/30/32。门禁只把这个值归一化成 `<ts>`，键与其它记录仍逐字节比。

还有一种情况不是分配差异，而是两边做的事不同：手动 `COMPACT`。参考的 `ManualCompactTask` 交给 `NewCompactionAlg` 挑候选段（`physical_compact_impl.cpp:36`），一个段时挑不出东西、**不开事务**；重写版的 `compact_table` 无条件把行搬进下一个段，因此多出一个写事务。多段用例里的表只有 8192 行（两边都是一个段），所以参考的 trace 是 `10=Append 12=checkpoint(flush) 14=Append(尾批) 16=checkpoint`，重写版在 12 与 14 之间多一个 compaction 事务。该用例只比尾块的**布局**（`_version_without_ts`），因为它的目的是验证行 id 携带段号、块落在段目录下，而不是比较 compaction 本身。

## 7. 怎么跑

```bash
# 参考（容器内）与重写版
docker exec infinity_builder cmake --build /infinity/cmake-build-reldeb --target infinity
cd rust_engine && cargo build --release -p infinity-cli --features rocksdb-store

# 字节 / 全类型 / 多块 / 写序列 / 索引矩阵 / 崩溃 / 多段（需要可写的 /var/infinity）
work=$(mktemp -d)
bwrap --dev-bind / / --tmpfs /var --dir /var/infinity --bind "$work" /var/infinity \
      --unshare-pid --proc /proc -- \
      .venv/bin/python -m pytest -q python/restart_test/test_cross_engine_bytes.py

# 对象存储往返（需要一个可用的 S3/MinIO 端点，桶名不能带下划线）
INFINITY_MINIO_URL=127.0.0.1:9000 \
INFINITY_MINIO_ACCESS_KEY=... INFINITY_MINIO_SECRET_KEY=... \
bwrap ... -- .venv/bin/python -m pytest -q \
      python/restart_test/test_cross_engine_bytes.py::test_the_port_round_trips_through_object_storage

# 互读
bwrap ... -- .venv/bin/python -m pytest -q python/restart_test/test_cross_engine_compat.py \
      --infinity_path=rust_engine/scripts/infinity_rust_shim.sh

# 重写版的 S3 测试（含"同长度改写也要上传"）
cd rust_engine && INFINITY_MINIO_TEST=1 INFINITY_MINIO_ENDPOINT=127.0.0.1:9000 \
      INFINITY_MINIO_ACCESS_KEY=... INFINITY_MINIO_SECRET_KEY=... \
      cargo test -p infinity-storage --test s3_minio
```

环境变量：

| 变量 | 默认 | 作用 |
|---|---|---|
| `INFINITY_CPP_BIN` | `cmake-build-reldeb/src/infinity` | 参考二进制 |
| `INFINITY_RUST_BIN` | `rust_engine/scripts/infinity_rust_shim.sh` | 重写版入口（shim 跑 `rust_engine/target/release/infinity`，编它必须带 `--features rocksdb-store`） |
| `INFINITY_BYTES_ROOT` | `/var/infinity/bytes` | 两引擎的数据根 |
| `INFINITY_BYTES_ROWS` | `200` | 字节测试的数据集行数 |
| `INFINITY_BYTES_BLOCK_ROWS` | `20000` | 多块测试的行数（>8192 才是多块） |
| `INFINITY_BYTES_DET_ROWS` | `200` | 确定性测量的行数 |
| `INFINITY_MINIO_URL` | 空 | 对象存储测试的端点；为空则跳过该用例 |
| `INFINITY_MINIO_ACCESS_KEY` / `_SECRET_KEY` | 空 | 对象存储凭据 |
| `INFINITY_MINIO_BUCKET_PREFIX` | `infinity-bytes` | 桶名前缀（桶名不能含 `_`，重写版会拒绝并退回本地）；每次跑再拼一个时间戳后缀，上一轮的桶不会污染这一轮 |
| `INFINITY_MINIO_TEST` | 空 | `1` 时 `cargo test --test s3_minio` 才真的连 MinIO，否则是 no-op |
| `INFINITY_MINIO_ENDPOINT` | `127.0.0.1:9005` | 上面那组 Rust 测试的端点（与 Python 门禁的 `INFINITY_MINIO_URL` 分开） |

测试要占 `23817`（Thrift）与 `5432/5433`（PG 线），参考不设 `SO_REUSEADDR`，所以用例之间会等端口真正释放。

**环境注意**：`bwrap` 只隔离 PID/mount，**不隔离网络**。同一台机器上若有另一个进程（另一个 agent、上一次没退干净的引擎）也在跑这些用例，它会连到本测试正在监听的 `23817` 上，把自己的表/行写进本次的目录——表现为"多了一张不属于本测试的表"或"参考只 dump 了前几个索引"。CI 用独立 runner 不会这样；本机并发跑时只看单条用例：`pytest ...::test_xxx`。
