# 跨引擎持久化格式一致性：测试设计、覆盖矩阵与缺口清单

> 目的：把「C++ 参考与 Rust 端口写出的磁盘格式一致」这件事变成可执行、可复现、可度量的门禁，并诚实记录**还没覆盖到**的部分。
>
> 判定口径与 [`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md) 一致（C++ 能打开 Rust 的目录，反之亦然，结果一致），这里再加一层更强的**字节**口径。审计结论（哪些差异是简化、哪些不可复刻）记在 [`SIMPLIFICATION_AUDIT.md`](SIMPLIFICATION_AUDIT.md)。

---

## 1. 三个等价层级

| 层级 | 断言 | 能发现什么 | 不能发现什么 |
|---|---|---|---|
| **字节门禁** | 同一输入，两引擎写出的文件逐字节相同 | 布局、对齐、编码、排序、填充的任何漂移 | 两边以同样方式写错的语义（不会发生，参考是基准） |
| **内容门禁** | 不可字节比的文件，解码后比「索引说了什么」 | 索引内容错、行序错、统计量错 | 文件头/元数据区的布局漂移 |
| **互读门禁** | A 写 → B 打开 → 查询答案一致 → B 继续写 | 读路径、replay、增量写 | 只有纯字节层面的漂移 |

三层缺一不可：互读能过而字节不同，说明两端对同一份数据的编码不同（历史上真实发生过：次级索引的 PGM 模型、稀疏索引的 term 内行序）。

## 2. 文件分类（**实测**，不是假设）

`test_the_reference_is_deterministic_where_the_test_assumes_it` 把参考在**同一输入**上跑多次，统计哪些文件的字节在变。分类靠这个测量支撑。

| 类别 | 文件 | 依据 |
|---|---|---|
| **确定性** | 块列 `*.col`、变长 outline `col_*_out`、全文 `.dic/.pos/.len`、稀疏 BMP chunk、**整数键**次级索引 chunk、`version`（去掉 ts 后） | 只由行值/行序决定；参考自身多次运行字节相同 |
| **参考也不确定** | HNSW chunk 的**图区** | 参考用 `thread_local std::mt19937`（`src/storage/knn_index/hnsw/hnsw_alg.cppm:107`），哪个线程插入哪条向量、因而哪个节点拿到哪一层由调度决定；5000 行时参考 5 次跑出 5 个不同的图 |
| **构建产物** | **浮点键**次级索引 chunk 的 PGM 模型 | 见 `SIMPLIFICATION_AUDIT.md` 第 32 行：`size_t(inf)` 是 UB（clang++-20 两段 / g++ 三段），拟合在 x87 80 位 `long double` 里做且 intercept 被截断（51 组随机键里 18 组与 f64 不同） |
| **不可比** | `catalog/`（RocksDB）、`wal/` | 带 session id、LSN、提交 ts；参考对自己都不可复现。只比**逻辑内容**（`SHOW CATALOG 'all'`）与互读 |

`version` 文件：布局相同，只有 `create_ts` 不同（参考写事务提交 ts，端口写镜像 ts），比较前把 ts 置零。

## 3. 测试清单

| 测试 | 数据集 | 断言 |
|---|---|---|
| `test_cross_engine_bytes.py::test_the_two_engines_write_the_same_bytes` | 200 行 → 1 块 / 1 段 / 每索引 1 chunk（22 个文件），5 类索引 | 文件集合相同；确定性文件逐字节相同；HNSW 比 header/vectors/labels；浮点次级比 postings；目录逻辑记录相同；索引答案相同 |
| `::test_the_reference_is_deterministic_where_the_test_assumes_it` | 200 行，参考跑 3 次 | 只有 HNSW chunk 允许变；其余必须自复现 |
| `::test_the_columns_match_for_every_type` | 每类列一行，含 NULL / 极值 | 每个 `*.col` 与 outline 逐字节相同 |
| `::test_writes_after_deletes_and_restarts` | 插入 → flush → 重启 → 再插入 / 更新 / 删除 → flush | `data/` 逐字节相同（删除状态在 catalog，比逻辑内容） |
| `::test_the_index_chunks_match_for_every_index_kind` | 每类索引一张表 | 按索引种类套用对应口径（字节 / postings / 内容） |
| `::test_a_crash_leaves_a_replayable_directory` | 写入后 **kill -9**（不 flush），换引擎打开 | 另一个引擎能 replay，行数与内容一致 |
| `test_cross_engine_compat.py` | 200 行，4 类索引 | 双向交接：A 写 → B 读 → B 再写 |
| `test_cross_engine_multiblock.py::test_multi_block_and_multi_chunk_bytes` | 20000 行 → 3 块 / 3 chunk | 多块列、每块 `version`、多 chunk 索引逐字节相同 |

## 4. 覆盖矩阵

行是格式单元，列是场景。`✅` 已覆盖且为字节门禁，`◐` 按内容比对，`⬜` 未覆盖。**本表随本节第 6 项的推进更新**。

| 格式单元 | 单块单 chunk | 多块 / 多 chunk | 多段 | 删除 / 更新 | 重启后再写 | 崩溃恢复 | 类型 / 选项矩阵 | 对象存储 |
|---|---|---|---|---|---|---|---|---|
| 数据文件头尾（magic / buffer_size / checksum） | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | — | ⬜ |
| 块列 `*.col`（`[data_region][null_bitmap]`） | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ |
| 变长 outline `col_*_out` | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ |
| `version` 文件 | ✅（ts 归一） | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | — | ⬜ |
| 全文 `.dic/.pos/.len` | ✅ | ✅ | ⬜ 跨段合并未冻结 | ⬜ | ⬜ | ⬜ | ⬜ analyzer / `with_positions` | ⬜ |
| HNSW chunk | ◐ 图按内容 | ◐ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ metric / build_type | ⬜ |
| IVF chunk（plain / SQ / PQ） | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ |
| 次级索引 chunk（整数键） | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ date/datetime/uint 键 | ⬜ |
| 次级索引 chunk（浮点键） | ◐ postings | ◐ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ double 键 | ⬜ |
| 稀疏 BMP chunk | ✅ | ✅ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ 词表规模 / block_size | ⬜ |
| 删除位图（catalog 内） | ◐ 逻辑 | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | — | — |
| WAL 记录 | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | ⬜ | — | ⬜ |
| Catalog KV | ◐ 逻辑 | ◐ | ⬜ | ⬜ | ⬜ | ⬜ | — | — |

## 5. 缺口清单（本轮记录）

### 5.1 明确不做字节比对（有实测理由）

1. **HNSW 图区**。参考用 `thread_local` 随机数，图本身不是唯一确定的。
2. **浮点键 PGM 模型**。`size_t(inf)` UB + 80 位 `long double`，是构建产物。
3. **`catalog/` 与 `wal/`**。带 ts / LSN / session id。

前两项「能不能改成可比」在 §6 的第 6 项里评估：结论是**不追求字节可比**，理由写在那一项里（要复刻 libstdc++ 的 `mt19937` + uniform_int_distribution + 线程插入顺序，正是我们在稀疏索引上刚刚删掉的坏味道）。

### 5.2 输入空间

1. **块层**：原先只有 1 块（200 行 << 8192），多块 `.col`、`blk_N/version`、跨块 outline 未验证 → 第 1 项补。
2. **段 / chunk 层**：原先只有 1 段、每索引 1 chunk；多段（周期性 dump）、多 chunk、跨段全文合并（`CPP_FORMAT_SPEC.md §9.5` 仍标 UNKNOWN）未验证 → 第 1 项补多 chunk，多段仍未覆盖。
3. **类型**：原先只有 `int/varchar/float/vector(4,f32)/sparse(8,f32,i32)` → 第 2 项补全类型。
4. **索引种类**：全文只测默认 analyzer；向量只测 HNSW plain f32 + l2；次级只测 varchar(u64 hash) + f32；**IVF 一次没测**（端口 `cpp_blocks.rs:2717` 已在写 `IvfChunkHeader`）→ 第 4 项补。
5. **写序列**：原先只有「一次 bulk insert + flush」→ 第 3 项补增量写 / 更新 / 删除 / 重启后再写。
6. **崩溃恢复**：只测干净关停 → 第 7 项补。
7. **对象存储**（`storage_type = "minio"`）：端口的 `s3::restore_tree` 与上传从未与参考比对 → 仍未覆盖，见 §6。

### 5.3 流程

1. **没有 CI**：`.github/workflows` 里没有任何 `test_cross_engine_*` 入口，字节门禁只在本机 `rust_engine/scripts/run_gate_c.sh` 跑 → 第 5 项。
2. **端口自身确定性**没有单独断言，由「参考确定 + 两边相等」间接推出。

## 6. 推进项与验收口径

| # | 项目 | 状态 | 验收口径 |
|---|---|---|---|
| 1 | 多块 / 多 chunk（20000 行） | ✅ | 3 块 × 6 列 + 3 块 version + 每索引 3 chunk 逐字节相同（`2 passed in 36.6s`） |
| 2 | 全类型列矩阵 | ⬜ | 每类列的 `*.col` / outline 逐字节相同 |
| 3 | 删除 / 更新 / 重启后再写 | ⬜ | 操作后 `data/` 逐字节相同，catalog 逻辑记录相同 |
| 4 | 索引种类矩阵（IVF / metric / analyzer / 整数键 / NULL / 重复键） | ⬜ | 每类按对应口径通过 |
| 5 | CI 接入 | ⬜ | workflow 在本机可复现的步骤上跑通字节 + 互读两个测试 |
| 6 | HNSW 图 / 浮点 PGM 的「改成可比」评估 | ✅ 结论：不做 | 见下方 |
| 7 | WAL 崩溃恢复（跨引擎 replay） | ⬜ | kill -9 后另一个引擎能 replay，行数与内容一致 |

### 第 1 项发现的真实缺陷（已修）

把数据集从 200 行提到 20000 行立刻暴露了两件事，都不是测试写法问题：

1. **端口每个索引只写一个 chunk**。参考的索引 chunk 粒度是**块**（8192 行）：`MemoryIndexer`/`SecondaryIndexInMemT`/`BMPIndexInMem` 每次只拿一个块，chunk 文件与 `chunk_info` 记录都是每块一条（全文的文件名是 `ft_{块首行号:016x}`）。端口此前把整个段的索引写成一个 chunk / 一个 `ft_0000000000000000`，20000 行时参考写 3 个、端口写 1 个，参考读目录时还会漏掉后面 2 块。
   修法：`write_cpp_fulltext_index` 按物理块（`row_id / 8192`）写 `ft_*`；`write_secondary_pgm_chunks` 与 `SparseIndex::seal` 按块切分（offets/doc id 仍是段内绝对行号，只有切分点变）；`publish_cpp_index_chunks` 改为发布**所有** chunk（此前只发布 `chunk_0.idx`，HNSW 的 chunk_1/2 在盘上但不在 catalog 里，参考根本看不到）。
2. **HNSW chunk 的 `max_layer` 不是格式参数**。它是建图结果（图有几层），和 enterpoint 一样属于图；200 行时两个引擎恰好一致，8192 行时一个是 2、一个是 3。测试里把它当成 header 的一部分来比是错的，已归入图区。
3. **`index_size` 属于建图产物**。参考在 `chunk_info` 里存 `GetSizeInBytes()`，HNSW 是对**图的邻居数**求和（`GraphStoreInner::GetSizeInBytes`，`graph_store.cppm:221`），BMP 是内存 `BMPAlg` 的容量，都与图内容有关；端口写 0（读侧把 0 当"用文件自身大小"）。catalog 比对因此对该两类索引归一化 `index_size`。

### 第 6 项的结论（为什么不做字节可比）

- **HNSW 图**：要让两引擎产出同一个图，端口必须逐位复刻 `std::mt19937`、`std::uniform_int_distribution` 的取模方式、以及**线程插入顺序**。随机数和线程调度都不是格式的一部分，复刻它们等于把「libstdc++ 的实现细节」写进格式——这正是稀疏索引 `TailFwd::ToBlockFwd` 改用 `std::stable_sort` 时否掉的做法。图用内容门禁：header / vectors / labels 字节相同，图区与查询答案按内容比。
- **浮点 PGM**：修掉 UB（哨兵不用 `inf`）能让**参考自己**跨编译器稳定，但端口仍要逐位复刻 PGM 的浮点拟合（`get_intersection`/`get_slope_range` 的运算顺序与截断）。而 PGM 只是加速定位的**索引结构**，读侧本来就会精确扫描校正（`GetResultCnt`），所以正确性不依赖模型字节。结论：postings 字节可比（已做到），模型不比。

## 7. 怎么跑

```bash
# 参考（容器内）与端口
docker exec infinity_builder cmake --build /infinity/cmake-build-reldeb --target infinity
cd rust_engine && cargo build --release -p infinity-cli --features rocksdb-store

# 字节 / 全类型 / 索引矩阵 / 写序列（需要可写的 /var/infinity）
work=$(mktemp -d)
bwrap --dev-bind / / --tmpfs /var --dir /var/infinity --bind "$work" /var/infinity \
      --unshare-pid --proc /proc -- \
      .venv/bin/python -m pytest -q python/restart_test/test_cross_engine_bytes.py

# 互读
bwrap ... -- .venv/bin/python -m pytest -q python/restart_test/test_cross_engine_compat.py \
      --infinity_path=rust_engine/scripts/infinity_rust_shim.sh
```

环境变量：

| 变量 | 默认 | 作用 |
|---|---|---|
| `INFINITY_CPP_BIN` | `cmake-build-reldeb/src/infinity` | 参考二进制 |
| `INFINITY_RUST_BIN` | `rust_engine/scripts/infinity_rust_shim.sh` | 端口入口 |
| `INFINITY_BYTES_ROOT` | `/var/infinity/bytes` | 两引擎的数据根 |
| `INFINITY_BYTES_ROWS` | `20000` | 主测试的数据集行数（>8192 才是多块） |
| `INFINITY_BYTES_DET_ROWS` | `500` | 确定性测量的行数（参考在小规模上图才稳定） |

测试要占 `23817`（Thrift）与 `5432/5433`（PG 线），参考不设 `SO_REUSEADDR`，所以用例之间会等端口真正释放。
