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

`version` 文件：布局相同，ts 也相同。`created_` 的 `create_ts` 是填块那次 append 的**提交 ts**，删除槽里是删除事务的提交 ts，两个引擎对同一句脚本消耗同一条 ts 序列（启动引导、首个 checkpoint、`default_db`、每条 DDL、每次 append/delete/dump/checkpoint，见 §6.6），所以整个文件逐字节比较。唯一会破坏这个前提的是参考的**内存用量驱动 dump**：它是后台线程上的写事务，ts 在 worker 真正提交时才分配，实测会落在 append 与随后的 DELETE 之间或之后（同一脚本两次跑出不同的 `version`）。对齐用例因此把配置里的异步任务全部关掉：`[storage] optimize_interval / cleanup_interval / compact_interval` 与 `[wal] checkpoint_interval` 都写 `"0s"`（参考侧 0 = 关闭，`default_values.cppm:98-123`），并把 `[buffer] memindex_memory_quota` 设成内存索引永远达不到的值（`"2GB"`，即 buffer 大小；参考的字节数解析器拒绝 `"0"`）。

内存索引的**行容量触发**保留（`[storage] mem_index_capacity`，参考默认 65536 = 8 个块）：它按行数触发（`new_txn_index_impl.cpp:1381`），是脚本的性质而不是时钟的性质，因此可复现。于是对齐用例里只剩两类 dump：脚本显式的 `DumpIndex`，以及行数越过 `mem_index_capacity` 的那次。排期固定之后，两边的 `idx_chunk|…|{chunk_id}` 也落在同一批事务上，该值与其他记录一样逐字节比（§6.1）。

浮点键 PGM 的截距依赖参考的**编译选项**，且只有一处真的被收缩。clang 默认 `-ffp-contract=on`；实测参考只把 `get_floating_point_segment` 的 `i_y - (i_x - origin) * slope` 编成 FMA，`get_intersection` 里 `a`、`b` 的两个叉积与 `i_x`/`i_y` 的 `p0.x + b * slope1.dx` 都按原式求值。判据是两组线性输入：都收缩时第一个截距是 31（65 行，步长 10）与 **32**（100 行，步长 0.5，与参考差 1 字节）；只收缩截距时是 31 与 31；都不收缩时是 32 与 31。重写版因此只对截距用 `f64::mul_add`，由 `float_intercept_matches_contracted_reference_fit` 与 `float_intercept_is_not_contracted_in_the_cross_products` 两条钉住。

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
| `::test_the_bytes_after_deletes_updates_and_a_restart` | 100 行 + 第二次运行 DELETE 20 / UPDATE 10 | 块仍逐字节相同（210 行、3 次 create 含 ts、30 个标记槽）、全文与浮点次级 chunk 相同、catalog 记录（键与值）相同；`idx_chunk|…` 记录整体不入比对（chunk 边界是排期产物），改断言它们盖住 210 行 |
| `::test_the_index_chunks_match_for_every_index_kind` | IVF + HNSW(cosine) + HNSW(ip) + 全文(chinese) + 次级(int) + 次级(date) | 六类索引按各自类别比对；`_wait_for_index_chunks` 等全部 chunk 落盘 |
| `::test_the_ivf_chunk_is_only_compared_by_content` | 上面的表，参考跑 2 次 | IVF chunk 的字节不是行值的函数，只能按内容比 |
| `::test_a_crash_leaves_a_replayable_directory` | 100 行 flush + 50 行未 flush 后 kill -9 | 另一引擎启动后看到 150 行（双向） |
| `::test_a_compaction_with_one_segment_is_a_no_op` | 8192 行 + `COMPACT` + 50 行 | 两个引擎都只有 `seg_0`（`NewCompactionAlg` 一个段挑不出候选，不开事务）；块与 `version` 逐字节相同；行数 8242 |
| `::test_a_multi_segment_table_matches_across_segments` | 两次 `IMPORT`（100 + 60 行，同一线程顺序导入）→ 两个段；七类索引（全文 / HNSW / IVF / 次级 varchar·float·int / 稀疏 BMP） | 每个索引在 `seg_0`、`seg_1` 各一套 chunk，chunk id 从 0 重新开始、`base_row_id` 以 `1 << 32` 起算；块、全文、次级、稀疏 chunk 逐字节相同，HNSW 头/向量/标签与 IVF `(offset, vector)` 按内容；catalog 键值相同的部分逐键比；参考读两边目录答案一致，跨段全文合并同时返回两个段的行 |
| `::test_a_segment_with_many_blocks_keeps_one_chunk_per_index` | 一次 `IMPORT` 20000 行 → 一段 3 块 | 每个索引在该段只有**一个** chunk（`row_count` 20000、`base_row_id` 0）——`PopulateIndex` 按 `mem_index_capacity`/段末 dumps 一次，不是每块一个；块、全文、次级、稀疏逐字节相同；参考读两边答案一致 |
| `::test_a_compacted_table_matches_and_keeps_its_sources` | 4 次 `IMPORT`（100+60+40+30）后 `COMPACT` | `NewCompactionAlg` 需要同层 4 个段才挑候选；合并段 id = 5、ts 是该压缩事务；四个源段保留文件与 catalog 记录、只多 `drop\|seg\|…` 与 `drop\|idx_seg\|…`，`(index, segment)` 配对记录被删（参考读目录时据此不看旧段）；新段每个索引一个 230 行 chunk |
| `::test_the_object_directory_is_shared_with_the_reference` | 100 行 + 次级索引；两个引擎各写一次 `persistence_dir`（`storage_type = "local"`，即桶在本地的那份布局），写完删掉 `data/`，由另一个引擎读 | 对面的引擎按 `pm|object|` 解析打包对象后：行数 100、`id = 7` 命中、次级范围查询命中同一批 id；两个方向一致 |
| `::test_the_port_round_trips_through_object_storage` | 200 行 + `storage_type="minio"` → 清空本地目录 → 读回 → 再写 50 行（在恢复出来的树上做 checkpoint）→ 再清空 → 读回 | 重写版从桶里恢复 `data/`+`catalog/`+`persistence/`，行数 200；第二次 checkpoint 后是 250（需要 `INFINITY_MINIO_URL`；每次跑用新桶，避免上一轮的桶污染） |
| `::test_the_secondary_and_sparse_indexes_survive_deletes_updates_and_a_restart` | INT / FLOAT / DOUBLE 三个次级键 + 稀疏 BMP；第二个进程 `DELETE` 前缀 + `UPDATE` 一段，再 flush/dump | 每个 chunk 逐字节相同（DOUBLE 键走 8 字节键 + `double` 斜率）；chunk 覆盖 110 个物理行；四个索引的答案相同 |
| `::test_the_vector_indexes_survive_deletes_updates_and_a_restart` | HNSW（`vec`）+ IVF（`vec2`）；第二个进程删除 + 改向量 + 追加 | HNSW 的 header/vectors/labels 与 IVF 的 header 计数、`(offset, vector)` 对相同；两个索引的 top-k 答案相同；行数相同 |
| `::test_the_quantized_ivf_indexes_mirror_and_answer` | IVF `scalar_quantization(8 bits)` + `product_quantization(2×8 bits)`；第二个进程删除前缀、改写一段、追加 40 行 | 两引擎都发布了 chunk、chunk 覆盖 260 个物理行；`nprobe=16` 查到 10 个合法 id 且**不含已删除的 id**（量化码依赖随机质心，邻居不作逐 id 比较，删除位图必须生效） |
| `::test_the_quantized_ivf_indexes_survive_a_crash` | 上一张表 + 30 行未 flush + 删除 10 行后 kill -9，双向交接 | 另一引擎 replay 后 220 行；SQ / PQ 各返回 10 个合法 id，且不含未 flush 阶段删除的 id |
| `::test_the_indexes_survive_a_crash` | 100 行 flush + dump → 50 行未 flush + 删除 10 行后 kill -9，双向交接；6 个索引（次级 INT / DOUBLE、BMP、HNSW、IVF、全文） | 另一引擎 replay 后 140 行；次级等值仍命中已 flush 的行；稀疏 / HNSW / IVF / 全文各返回 5 个合法 id |
| `::test_the_sparse_index_options_reach_the_chunk` | 300 行、300 个不同 term、列声明 `sparse,512,...`；同一列上两个 BMP 索引：`block_size = 4` 与默认（16） | 同一引擎里两个 chunk 必须**不同**（选项确实进了 chunk）；各自与另一引擎的同名 chunk 逐字节相同；chunk 盖住 300 行 |
| `::test_the_wal_entries_match_for_the_shared_commands` | 建表 + 100 行 append + 20 行 delete + 10 行 update，**不** flush（WAL 不被 checkpoint 截断） | 每条 `Append` / `Delete` 的提交 ts、命令类型、库名/表名逐字节相同；`Delete` 的行 id 列表逐字节相同；`Append` 的块头（`block_num/block_id/first_row_id/row_count`）逐字节相同 |
| `test_cross_engine_compat.py` | 200 行，4 类索引 | 双向交接：A 写 → B 读 → B 再写 |
| `infinity-storage/tests/s3_minio.rs` | 本地临时目录 + 桶 | `tree_sync_and_restore`（往返 + 二次同步上传 0 个文件）、`same_size_edit_is_uploaded`（同长度改写必须重传）、`put_get_list_delete_roundtrip`；需要 `INFINITY_MINIO_TEST=1` |

## 4. 覆盖矩阵

行是格式单元，列是场景。`✅` 已覆盖且为字节门禁，`◐` 按内容 / 互读比对，`⬜` 未纳入门禁（原因见 §5）。

| 格式单元 | 单块单 chunk | 多块 / 多 chunk | 多段 | 删除 / 更新 | 重启后再写 | 崩溃恢复 | 类型 / 选项矩阵 | 对象存储 |
|---|---|---|---|---|---|---|---|---|
| 数据文件头尾（magic / buffer_size / checksum） | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ 互读 | — | ◐ 跨引擎对象目录 |
| 块列 `*.col`（`[data_region][null_bitmap]`） | ✅ | ✅ | ✅ 非 0 段块 | ✅ | ✅ | ◐ 互读 | ✅ 16 类 | ◐ |
| 变长 outline `col_*_out` | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ | ✅ | ◐ |
| `version` 文件 | ✅（含 ts） | ✅ | ✅ | ✅ | ✅ | ◐ | — | ◐ |
| 全文 `.dic/.pos/.len` | ✅ | ✅ | ✅ 两段各自 chunk + 跨段合并 | ✅ | ✅ | ◐ 重放后查询 | ✅ analyzer / `with_positions` | ◐ |
| HNSW chunk | ◐ 图按内容 | ◐ | ◐ 两段，按内容 | ◐ 内容+答案 | ◐ 内容+答案 | ◐ 重放后查询 | ✅ metric / build_type | ◐ |
| IVF chunk（plain / SQ / PQ） | ◐ 按内容 | ◐ | ◐ 两段，按内容 | ◐ 内容+答案 | ◐ 内容+答案 | ◐ 重放后查询 | ◐ plain / SQ / PQ | ◐ |
| 次级索引 chunk（整数键） | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ 重放后查询 | ✅ int / date 键 | ◐ |
| 次级索引 chunk（浮点键） | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ 重放后查询 | ✅ float / double 键 | ◐ |
| 稀疏 BMP chunk | ✅ | ✅ | ✅ | ✅ | ✅ | ◐ 重放后查询 | ✅ block_size / 词表规模 | ◐ |
| 删除位图（catalog 内） | ◐ 逻辑 | ◐ | ◐ | ✅ | ✅ | ◐ | — | — |
| WAL 记录 | ◐ 帧头+删除 id 逐字节 | ◐ | ◐ | ✅ | ✅ | ✅ 跨引擎 replay | — | ◐ |
| Catalog KV | ◐ 逻辑 | ◐ | ◐ | ✅ | ✅ | ◐ | — | — |

"对象存储"列的 `◐` 现在指**跨引擎对象目录**：任一引擎写出的 `pm|object|` + 打包对象，另一个引擎删掉 `data/` 后也能读出来（§3 的对象目录用例）。MinIO 传输层仍只有重写版自身往返，原因见 §6.5。

## 5. 未纳入门禁的部分

### 5.1 不做字节比对（有实测理由）

1. **HNSW 图区**。参考用 `thread_local` 随机数，图本身不是唯一确定的；重写版的图是确定的，但两端不可能相同。
2. **IVF 训练**。参考的 k-means 采样走 `std::random_device`，质心与质心个数都不是行值的函数。
3. **`catalog/` 与 `wal/`**。带 ts / LSN / session id。
4. **MinIO 桶内布局**。对象布局（`pm|object|` + 打包对象）已经对齐并由 §3 的对象目录用例双向验证；桶只是这套布局的远端传输，见 §6.5。

前两项的结论是**不追求字节可比**，理由在 §6.4：要复刻 libstdc++ 的 `mt19937` + `uniform_int_distribution` + 线程插入顺序，等于把实现细节写进格式。

### 5.2 输入空间的缺口

1. **段的产生方式**：参考在 `DEFAULT_SEGMENT_CAPACITY`（8,388,608 行）之前不会因为写满而开第二段；`COMPACT` 的候选由 `NewCompactionAlg` 按 DBT 挑（同层至少 `DBT_COMPACTION_M = 4` 个段），少于四个时不开事务。多段门禁因此走 `IMPORT`：它按 `ApplySegmentIDs` 自己分配并封存一个段（`new_txn_data_impl.cpp:241`）。
2. **多段的规模**：`test_a_multi_segment_table_matches_across_segments` 是 3 段、每段一块一个 chunk；`test_a_segment_with_many_blocks_keeps_one_chunk_per_index` 是一段三块一个 chunk；`test_a_compacted_table_matches_and_keeps_its_sources` 是 4 段压成 1 段。**8,388,608 行以上的真实多段仍未跑过**（需要按行容量切分写入，见 §5.2.3）。
3. **IMPORT 的 `fast_rough_filter`**：参考在导入封存段时同步建一个 binary fuse 预过滤器并写 `seg|{db}|{tbl}|{seg}|fast_rough_filter`（段级）与 `blk|…|fast_rough_filter`（块级，`BuildFastRoughFilterTask::ExecuteOnNewSealedSegment`，`new_txn_data_impl.cpp:1739`），重写版不写这些记录。读者把"没有过滤器"当作"不过滤"（走次级索引/扫描），因此不影响答案；门禁把它们排除在 catalog 比对之外。
4. **WAL 的 Append 行载荷**：帧头（提交 ts、命令数、命令类型、库名/表名）、`Delete` 的行 id 列表、`Append` 的块头都逐字节比（`test_the_wal_entries_match_for_the_shared_commands`）；`Append` 的行数据段与 `Delete` 的结尾计数字是重写版自己的编码——两个方向都能 replay（`test_a_crash_leaves_a_replayable_directory` 与 `test_the_indexes_survive_a_crash`），但字节不同。DDL 不在比对内：参考写 `CreateTable`/`CreateIndex`，重写版写 `KvBatch`（它落盘的 catalog 键），是不同的命令而非同一命令的不同编码。

### 5.3 流程

1. **CI**：`.github/workflows/rust_engine.yml` 的 `cross_engine` job 在 builder 镜像里编参考、用 `--features rocksdb-store` 编重写版，并在 bwrap 下跑两个 pytest 文件。**对象存储不在 CI 里**：job 没有设 `INFINITY_MINIO_URL` / `INFINITY_MINIO_TEST`，所以 `test_the_port_round_trips_through_object_storage` 与 `s3_minio.rs` 在 CI 中都是 skip，需要手工跑（§7）。
2. **重写版自身确定性**：由 `test_the_port_is_deterministic_too` 单独断言（3 次跑，`data/` 下每个文件都必须相同）。

## 6. 与 C++ 参考的差异

### 6.1 字节层面的差异

| 差异 | 重写版 | 参考 | 门禁处理 |
|---|---|---|---|
| HNSW 图区 | 确定（插入顺序即行序） | `thread_local std::mt19937`，节点层级取决于线程调度 | header / vectors / labels 字节比，图区与查询答案按内容比 |
| IVF 质心 | 确定 | `std::random_device` 采样，质心个数也不固定 | chunk 按内容比 |
| `catalog/`、`wal/` | RocksDB；redo 记录写 `wal/wal.rust`，replay 时同时读参考的 `wal/wal.log` | RocksDB；写 `wal/wal.log` | 只比逻辑内容与互读 |
| 对象存储：写出的布局 | `persistence_format = "reference"` 时写 `pm\|object\|` 记录 + 打包对象（与参考同一套，128 MB 上限 / footer / stat） | `ObjCreate` 命名打包 blob，本地路径映射在 KV 的 `pm\|object\|<path>` | 对象目录用例双向比（§3） |
| 对象存储：S3 传输 | 路径树 `data/`、`catalog/`、`persistence/` 逐文件上传 | 对象键即 `obj_key`，对象文件直接上传 | 只做重写版自身往返（无可用端点，§6.5） |
| 索引 chunk 的段目录 | 每个段一套 `idx_{id}/seg_{s}/`，chunk id 每段从 0 起 | 同左 | 多段用例逐文件比（§3） |
| 索引 chunk 的粒度 | `IMPORT` 的段：全文按 `mem_index_capacity` 切、次级/稀疏整段一个；`INSERT` 的段：按块 | 同左 | 一段三块用例（§3） |
| 压缩后的段布局 | 合并段 = 最大段号 + 1，块/版本/chunk 都带压缩事务的 ts；源段保留文件与记录，另写 `drop\|seg\|` 与 `drop\|idx_seg\|`、删 `(index, segment)` 配对；`Cleanup` 才回收 | 同左 | 4 段压缩用例（§3） |
| `seg\|…\|fast_rough_filter` | 不写 | `IMPORT` 时写二进制 fuse 过滤器 | 不比对；读者把缺失当作"不过滤"，答案不变（§5.2.3） |
| 段的产生时机 | 按 `DEFAULT_SEGMENT_CAPACITY = 8,388,608` 行切（与参考相同）；单段 `COMPACT` 不开事务 | 按 `DEFAULT_SEGMENT_CAPACITY = 8,388,608` 行切 | 见 §5.2 |
| WAL 的 `Append` 行载荷 / DDL 命令 | 行数据按 `row_id` + 文本编码，DDL 写 `KvBatch`（catalog 键值） | 行数据按块内列编码，DDL 写 `CreateTable`/`CreateIndex` | 帧头、命令类型、库/表名、`Delete` 行 id 逐字节比；行载荷与 DDL 只保证双向 replay |
| 不带 `rocksdb-store` feature 时 | catalog 落 fjall | 只认 RocksDB | 参考打开只剩 `system_tag`（`Database: default_db doesn't exist`）；门禁必须带 feature |

### 6.2 与参考逐字节一致的部分

- 块列 `*.col`（16 类列，含 NULL 位图、`-0.0`、极值、outline 分界）与变长 outline；
- `version` 文件整体（`[create_size]` + 每次 create 的 `[ts][累计行数]` + `[capacity]` + 每槽 u64，DELETE 标槽，UPDATE = 删除 + 追加）：布局一致，且 `create_ts` 与删除槽 ts 就是事务自己的提交 ts，两边逐值相同（§6.6）；
- 全文 `.dic/.pos/.len`、次级索引 chunk（整数键与浮点键，含 PGM 模型）、稀疏 BMP chunk；
- 块的 NULL 位图，包括 `IMPORT` 写出的块的空位图覆盖规则：参考在一次导入里按**导入行数**建位图（最后一字节的尾位是 0），`INSERT` 写的块按字节填满（尾位是 1），两条规则都复刻（`ColumnBody::build_with`）；
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

表中的差异来自**配额驱动的后台 dump**（`memindex_memory_quota` 很小、又和其他引擎抢 CPU）；对齐门禁现在把它关掉（§2），只剩显式 `DumpIndex` 与行容量触发，切分是确定的。因此 chunk 的数量/边界仍不能作为通用的字节门禁（参考在别的配置下会变）：重写版只保证切分确定且不重不漏，200 行规模上参考稳定，那里整文件比对。`_seed` 在 `flush_data()` 之后对每个索引显式 `dump_index()`（参考的 `DumpMemIndex`），再用 `_wait_for_index_chunks` 轮询到全部 chunk 落盘，不依赖固定 sleep。

### 6.4 为什么不做 HNSW 图的字节可比

要让两引擎产出同一个图，重写版必须逐位复刻 `std::mt19937`、`std::uniform_int_distribution` 的取模方式、以及**线程插入顺序**。随机数和线程调度都不是格式的一部分。

### 6.5 对象存储：同一套对象布局

参考的持久化层（`PersistenceManager`，`src/storage/persistence/persistence_manager_impl.cpp`）不把数据文件留在 `data/`，而是：

- 把文件按 8 字节对齐拼进**对象**，或整个搬过去；对象文件的文件名就是 `obj_key`；
- 在 catalog 里写 `pm|object|<相对 data_dir 的路径>` → `{"obj_key","part_offset","part_size"}`，空文件记 `KEY_EMPTY`；
- 另写 `pm|object_stat|<obj_key>` → `{"deleted_ranges":[],"obj_size","parts"}`；
- 对象目录就是 `persistence_dir`（本地模式），换成 `storage_type = "minio"` 时同一份布局在桶里（`VirtualStore` 只换传输）。

重写版现在读写同一套布局：`[storage] persistence_format = "reference"`（参考会拒绝 `[storage]` 里的未知键，所以这个键只写进重写版的配置）让引擎在打开时按 `pm|object|` 把缺的 `data/` 文件从对象里取出来，并在每次 checkpoint 后把 `data/` 重新打包成一个对象、重写这些记录。`test_the_object_directory_is_shared_with_the_reference` 双向验证：任一引擎写出的对象目录，删掉 `data/` 后另一个引擎都能按对象答出同样的行与索引结果。

剩下的差异只有传输层：重写版的 S3 客户端（`infinity_storage::s3`）仍按路径树上传播 `data/`、`catalog/`、`persistence/`，没有把打包对象与 `pm|object|` 记录发到桶里。要覆盖这一段需要一个可用的 S3 端点（用例需要 `INFINITY_MINIO_URL`），而 MinIO 开源版已归档、本机也没有可用的替代端点，所以这一列在门禁里仍是重写版自身的往返。

参考的 compose 规则（128 MB 上限、独占对象、`parts > 1` 的 4 字节 footer、逐次 append 的统计记录、引用计数与 `deleted_ranges`）已由 `infinity_storage::persistence` 实现，逐条单测与参考写出的记录值对照；移植过程与剩余的纯形状重构见 [`PERSISTENCE_MANAGER_PORT_PLAN.md`](PERSISTENCE_MANAGER_PORT_PLAN.md)。

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

所以同一句脚本的 trace 逐值相同。实测（200 行、5 个索引的字节用例，异步任务按 §2 关闭）：

```
参考   : 4=checkpoint(启动) 6=CreateDatabase 8=CreateTable 10/12/14/16/18=CreateIndex
         20=Append 22=checkpoint(flush) 24/26/28/30/32=DumpIndex 36/40/44=checkpoint
重写版 : 4=checkpoint(启动) 6=CreateDatabase 8=CreateTable 10/12/14/16/18=CreateIndex
         20=Append 22=checkpoint(flush) 24/26/28/30/32=DumpIndex
```

（文件级读数会误导：重写版的 redo 记录在 `wal.rust`，`wal.log` 只保留最新一个 checkpoint 帧，后写的那帧由 `cover_with_checkpoint` 覆盖到最后一次 dump，所以 `wal.log` 里读到的是 32；参考会截断自己的 `wal.log`。逐值比较看的是两边实际消耗的提交 ts。）

第二个进程接着写同一目录（append 100 / delete 20 / update 10 / flush / dump ×2）：

```
参考   : 24=checkpoint(启动) 26=Append 28=Delete 30=Update 32=checkpoint 34/36=DumpIndex
重写版 : 同一串
```

这些 ts 落到字节里的地方因此全部逐字节相同：块 `version` 的 `create_ts` 与删除槽、`catalog|seg|…` / `catalog|blk|…`（开段/开块的 append）、`idx_seg|…`（把索引与该段配对的事务 = `max(index 创建 ts, segment 创建 ts)`）、`catalog|tbl|…|{ts}` / `catalog|idx|…|{ts}` / `tbl|col|…|{ts}` 键里的 ts、`seg|…|first_delete_ts`（第一次删到该段的事务；不写它参考就不看块的删除标记，已删的行会被读回来）。

前提是**没有"什么时候跑"的写事务**，所以对齐配置把异步任务全关掉（§2）：`optimize/cleanup/compact_interval` 与 `checkpoint_interval` 都是 `0s`，`memindex_memory_quota` 取内存索引达不到的值。剩下的 dump 只有脚本显式的 `DumpIndex` 与行数越过 `mem_index_capacity` 的那次，两者都由脚本决定，因此 `idx_chunk|…|{chunk_id}` 也落在同一批事务上，和其它记录一样逐字节比。

`idx_chunk` 的值由重写版在 `DumpIndex` 时记下（`dump_ts:{表}:{索引}`），盖到已发布的 id 记录上，并把 `wal.log` 的 checkpoint 帧推进到覆盖它——参考读目录时按 `chunk_ts > begin_ts` 隐藏 chunk（`LoadChunkIDs1`），帧不覆盖就会被藏起来。

`IMPORT` 的段是另一种情况：参考在导入事务里就把索引写进 chunk（`PopulateIndex` → `AddNewChunkIndex1`），所以那个段的 chunk 记录带的是**导入事务**的提交 ts（多段用例里两段分别是 22 与 24）。重写版为此记下每段被导入时的 ts（`import_ts:{表}:{段}`），发布 chunk 时取它与 `dump_ts` 的较大者——用 `INSERT` 建的表没有这条记录，行为不变。

手动 `COMPACT` 两边做的也是同一件事：`ManualCompactTask` 把 `NewCompactionAlg` 挑出的候选段交给 `PhysicalCompact`（`physical_compact_impl.cpp:36`），只有一个段时挑不出候选、**不开事务**；重写版的 `compact_table` 现在同样在 `segment_ids(table).len() < 2` 时直接返回，所以 8192 行的脚本两边都留在 `seg_0`，trace 与字节完全一致（`test_a_compaction_with_one_segment_is_a_no_op`）。

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
