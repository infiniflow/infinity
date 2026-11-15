# issue.txt 逐条对照

[`issue.txt`](issue.txt) 是审查者针对 `beb2b6912718a1e05b23472aaaf5c9cfa10df9ae`（`rust_engine`）提交的问题清单，共 8 条。本文逐条给出核验结论、对应的改动或决定，以及证据所在位置。

复现用的参考二进制：`cmake-build-reldeb/src/infinity`（C++ 侧，`main` 同期）。数据：2000 行、128 维，逐维均值 ≈ `30, 18, 11, 11, 1.5 …`，`encode=lvq`，`M=16`/`ef_construction=200`/`metric=l2`。

---

## 一、结论总览

| # | issue 条目 | 核验结论 | 处理 |
|---|---|---|---|
| 1 | LSG 参数没有接入实际构图 | **参考同样未接线** | 保持现状，记录依据 |
| 2 | LVQ 存在编码但未执行均值训练 | **参考同样未训练** | 保持现状，记录依据 |
| 3 | LVQ 构图被强制串行执行 | 端口缺陷 | 已修 |
| 4 | 构图存在重复复制与转换开销 | 端口缺陷 | 部分修复（克隆已修；文本往返、查询重算、整批复制未修，见 §3.2） |
| 5 | `show_index` 成功但参数字段为空 | 端口缺陷 | 已修 |
| 6 | 仅靠 WAL 恢复时丢失索引参数 | 端口缺陷 | 已修 |
| 7 | 显式 LSG 配置拼写不兼容，C++ 读取 Rust checkpoint 时 abort | 端口缺陷 | 已修 |
| 8 | Rust 读取 C++ 的 LSG WAL 后，索引不存在 | 端口缺陷 | 已修 |
| — | *（修第 3 条时发现）* HNSW 的 bucket 划分与参考不一致 | 端口缺陷 | 已修（§3.7） |

第 1、2 条及第 1 条引出的“三份文件逐字节相同”判据，经实测确认是**参考实现自身的行为**，不是端口回归；本轮明确**保持现状**。理由与测量见下文第二节。

---

## 二、判定为“参考自身行为”、保持现状的三项

### 2.1 LSG 参数不进入构图距离（issue 第 1 条）

参考的源码路径：

- `HnswHandler::InsertSampleVecs` / `InsertLSAvg` / `SetLSGParam` 只在 `src/unit_test/storage/knnindex/knn_hnsw/test_lsg_build_ut.cpp` 与 `test_quantizer_performance_ut.cpp` 里被调用；`src/` 的生产路径一处也没有（`hnsw_alg.cppm:339/350/360` 是它们的定义）。
- `HnswHandler` 构造时确实对 `IndexT::LSG` 调 `InitLSGBuilder`（`hnsw_handler_impl.cpp:218`），但没有后续调用把 `avg_` 交给距离函数，`LSGDistWrapper::avg_` 恒为 `nullptr`，`operator()` 直接返回 plain 距离。
- `NewTxn::OptimizeVecIndex` 对 `build_type_ == HnswBuildType::kLSG` 直接 `UnrecoverableError("Not implemented yet")`（`new_txn_index_impl.cpp:2282`）。

端口因此按同一口径：接受并持久化 `build_type` / `lsg_config`，构图使用 plain 距离；`index::lsg` 的 `LsgIndex`（采样 + 前 `ls_k` 均距 + `d / sqrt(avg_i·avg_j)^alpha`）已完整实现，供 `quantizer_perf` 与单测使用。

### 2.2 建索引路径不训练 LVQ 均值（issue 第 2 条）

- 参考唯一的均值训练点是 `LVQVecStoreMeta::Optimize`（`lvq_vec_store.cppm:255`），触发链只有 `ALTER INDEX … WITH (lvq_avg)` → `DataStore::Optimize()`（`data_store.cppm:310`）。
- `OptAddVec`（会先 `Optimize` 再入列）只被 `CompressToLVQ` / `CompressToRabitq` 使用（`data_store.cppm:834/857`），即 `ALTER` 的压缩路径；`CREATE INDEX` 走的是 `DataStore::AddVec`。
- 实测：参考为三个索引（plain / 默认 LSG / 显式 LSG）dump 出的 chunk，`dim(8B) + 128 × double` 的均值区**逐字节全零**；对同一张表执行 `ALTER INDEX … (compress_to_lvq)` 后再 dump，均值仍全零。共 9 个 chunk 全部如此。

端口保持同一条路径：`train_store` 只在显式调用时训练，`LvqStore::train` / `build` 供 optimize 与单测使用。

### 2.3 “三份索引文件逐字节相同”不是有效判据（issue 第 1 条的论证方式）

参考的 HNSW 图区层次由 `thread_local std::mt19937` 决定（`hnsw_alg.cppm:107`），哪个线程构建哪个向量、因而哪个节点拿到哪一层由调度决定。实测：

- 三个参数不同的索引，chunk 的**头 + 向量区**逐字节相同，只有图区不同；
- 两个参数**完全相同**的 plain 索引，chunk hash 也不同；
- 两个参数**完全相同**的 LSG 索引，chunk hash 也不同。

所以“文件相同 ⇒ LSG 未生效”与“文件不同 ⇒ LSG 生效”两个方向都不成立。第 1、2 条的结论只能由源码路径（2.1、2.2）得出。

**为什么不改**：真正实现 LSG 采样与 LVQ 均值训练会让端口与参考分叉，而 `docs/MANUAL_ACCEPTANCE.md` §7 的 LSP-HNSW 验收数字就是参考这条未接线路径跑出来的；一旦分叉，字节门禁、互读门禁与 §7 的基线都要重做。本轮决定保持现状，依据记录在 `docs/GAPS.md` §5。

---

## 三、已修的端口缺陷

### 3.1 issue 第 3 条：量化索引被强制串行构建

`HnswIndex::add_batch` 此前对任何非 `Plain` 的 store 直接回退到逐条 `add()`，实测 LVQ 建索引只用单核（99% CPU）。

修法：去掉该分支。参考的 `HnswHandler::InsertVecs` 从不按 store 类型分支——它先 `StoreData` 存整批，再把 `[start, end)` 切成 bucket 并行 `Build`；量化 store 的 `Build` 从顶点自身的编码重新推导查询（`GetVecToQuery`），端口由 `QueryRef::for_node` 等价实现。

回归：`index::vector::tests::quantized_batch_build_matches_serial_answers`（LVQ + RaBitQ，4 线程，与串行构建的答案 recall ≥ 0.9，图结构过 `check()`）。

### 3.2 issue 第 4 条：每次点间比较都克隆 LVQ 编码（**部分修复**）

issue 列了四类开销，本轮只修了第一类；其余三类仍在，见下面的清单。

**已修：点间比较的克隆。** `Store::distance_nodes` 在 LVQ 上走 `store.query_at(a)`，这会克隆整条 `LvqData`（含 `Vec<i8>`）；而 `SelectNeighborsHeuristic` 与反向边裁剪对每个候选都调用它一次。修法：新增 `LvqStore::record()`（借用）与 `LvqStore::distance_between(&LvqData, &LvqData)`，`distance_nodes` 改成借用两侧记录——对应参考把两个 `const LVQData *` 交给 `LVQDist` 的形态。

回归：`lvq::tests::borrowed_distance_matches_the_query_path`（三种 metric 下与 `query_at` 路径逐值相等）。

**未修（issue 提到的其余开销）：**

| 开销 | 现状 |
|---|---|
| 同一候选与多个邻居比较时重复构造查询编码 | 仍然存在。`distance_to` 对每个 `(query, i)` 调 `Store::distance_query`，RaBitQ 走 `decompress` + `make_query`，每次重算查询向量，没有「外层构造一次、内层复用」的借用形态 |
| 定宽向量先转文本再解析回浮点 | 仍然存在。`read_block_column_including_deleted` 返回 `Vec<(usize, String)>`，`build_hnsw_index_of` 再 `parse_vec_str` 回 `f32`；索引构建的读路径没有按列直接解码 `f32` 的通道 |
| 多个调用层再次克隆整批向量 | 仍然存在。`Engine::build_hnsw_index_of` → `HnswMemIndexInMem::insert_batch` → `HnswIndex::add_batch` 各层都 `clone()` 一份 `Vec<Vec<f32>>` |

这三项需要改读列接口与 `insert_batch` 的所有权传递，属于跨层的接口改动，不在本轮范围内；issue 自己也注明「尚未完成独立耗时剖析，不能认定它是主要瓶颈」。

### 3.3 issue 第 5 条：`show_index` 的参数字段为空

`ShowIndex` 响应里 `other_parameters` / `store_dir` / `segment_index_count` 固定填空串，`index_column_ids` 同样。

修法：新增 `Engine::show_index_info` 与 `other_params_of`，按参考的 `IndexBase::BuildOtherParamsString` 与 `TableMeta::...index_entry_dir_` 口径填充；Thrift 与 HTTP 两条路都接上。实测输出已与参考同形：

```
other_parameters   = metric = l2, encode_type = lvq, M = 16, ef_construction = 200,
                     lsg_config = sample_raito = 0.25, ls_k = 7, alpha = 0.5
store_dir          = db_1/tbl_0/idx_1
segment_index_count= 1
index_column_ids   = 1
```

### 3.4 issue 第 6 条：WAL 恢复丢失索引参数

`wal_bridge::index_options` 只回放 `M` / `EF_CONSTRUCTION` / `BLOCK_SIZE` / `METRIC`，丢了 `ENCODE`、`BUILD_TYPE`、`LSG_CONFIG`，于是崩溃恢复后的索引静默变成 `plain + plain` 且 LSG 配置丢失。

修法：这三个选项一并回放。实测：新建目录、32 行内建 LSG+LVQ 索引、不 `FLUSH DATA` 直接换进程打开副本，恢复后的 catalog 记录为

```json
{"M":16,"build_type":"lsg","encode_type":"lvq","lsg_config":"sample_raito=0.25,ls_k=7,alpha=0.5", ...}
```

回归：`wal_bridge::tests::round_trips_the_hnsw_options_through_a_wal_replay`。

### 3.5 issue 第 7 条：`lsg_config` 拼写不兼容导致参考 abort

参考的 `LSGConfig::FromString` 只认 `sample_raito`（拼写错误，`index_hnsw_impl.cpp:115`），`ToString` 也写这个；端口只认 `sample_ratio` 并写出 `sample_ratio=…`，参考读 Rust 写的 catalog 时 `Invalid index parameter type: sample_ratio` → `LSGConfig::FromString` → abort。

修法：`LsgConfig::parse` 两种拼写都接受（历史语句继续可用），`Display` 改为参考的 `sample_raito = 0.25, ls_k = 7, alpha = 0.5`（含空格、整数不带 `.0`）。

回归：`lsg::tests::config_parses_the_reference_format`。

### 3.6 issue 第 8 条：Rust 读 C++ 的 LSG WAL 后索引不存在

根因是 `IndexDef::read_adv` 遇到 `has_lsg` 直接返回 `Error::Storage("HNSW LSG config body is not ported yet")`。该错误让整帧 `CreateIndexV2` 解码失败（`decode_cmd` 返回 `None`），索引记录根本没被重放，所以 `show_index` 报 `index 'idx' does not exist`。

修法：`IndexDef::Hnsw` 的 `has_lsg: bool` 换成 `lsg: Option<LsgConfig>`，按参考的 `f32 sample_ratio` + `size_t ls_k` + `f32 alpha` 读回 body。

实测：Rust 打开参考写出的目录后，三条 HNSW 索引（plain / 默认 LSG / 显式 LSG）全部恢复，`show_index` 与向量查询都正常（`count(*) = 2000`，查询返回 3 行）。

回归：`storage::wal::index_def::tests::hnsw_lsg_body_roundtrips_and_has_the_reference_layout`。

### 3.7 修 item 3 时发现的附加缺陷：HNSW 的 bucket 划分与参考不一致

`HnswIndex::add_batch` 把 `[base, entry)` 与 `[entry + 1, n)` **分别**切 bucket，再把两个列表并发执行。参考的 `HnswHandler::InsertVecs` 是对整个 `[start, end)` 切一次：

```
bucket_size = max(kBuildBucketSize, (n - 1) / threads + 1)
```

差别只在小批量上显现：

| 行数 | 参考 | 端口（修前） |
|---|---|---|
| 200 | 1 个 bucket，单线程 | 2 个 bucket，并发 |

两个并发 bucket 会写彼此的反向边：`connect_neighbors` 在 `list.len() < Mmax` 时直接 `push`，列表内容因此取决于谁先到。200 行的图逐次不同。

item 3 把量化 store 也送进这条路径后，这个偏差第一次变得可观测：`hnsw_encodings_answer_vector_search`（200 行）在 CI 的 arm64 上失败（amd64 通过，两边线程调度不同）。

修法三步（前两步不够，第三步才是关键）：

1. 按参考对整个 `[base, n)` 统一切分；
2. builder 里两处 `sort_unstable_by` 加上按顶点 id 的 tiebreak，与 serial 路径 `search_layer_filtered` 已有的做法一致。量化 store 的距离是估计值，并列很常见，没有 tiebreak 时 `sort_unstable` 对等值元素的顺序取决于输入布局；
3. **entry 顶点必须在它所属的 bucket 内部跳过，而不是把它从区间里切掉。** 第一版把含 entry 的 bucket 拆成前后两段，结果这两段又被并发调度——相邻区间同时飞，反向边的写入顺序依然不定（30 次构建 19 种图）。最终版本让每个 bucket 保持**连续**区间，`build_serial` 在遍历时跳过 entry。

验证：plain / LVQ / RaBitQ 各 30 次构建都只得到 **1** 种图（修前分别是 19 / 2 / 2 种）；该用例连续 25 次通过，再加 12 次 CPU 满载下也全部通过；把跳过逻辑去掉后测试立即失败。

---

## 四、遗留项（已决定保持现状）

**从参考目录恢复时 `segment_index_count` 读 0，参考读 1。**

根因：参考把索引 chunk 打包进 `persistence/` 的 `pm|object|` 条目（一个 blob 按 `part_offset`/`part_size` 切片），端口按已登记的“不移植对象存储”决定只读解包后的 `data/` 树，因此这些 chunk 在端口侧没有 `chunk_*.idx` 文件，`catalog_index_files` 返回空，段计数为 0。

影响范围：仅限“参考写了但没解包的目录”这一种输入；索引定义、参数、查询结果都正常（见 3.6 的实测）。

决定：**保持“不移植对象存储”**。追平需要移植参考的 `obj_key` 打包上传 + `VirtualStore` 布局，与 `docs/GAPS.md` §1 已登记的范围决定冲突，不在本轮范围内。

---

## 五、验证

- `cargo fmt`、`cargo clippy --workspace --all-targets` 干净。
- 单测：`index` 139 passed、`storage` 496 passed、`executor` 789 passed，0 failed。
- 新增回归：`quantized_batch_build_matches_serial_answers`、`borrowed_distance_matches_the_query_path`、`hnsw_lsg_body_roundtrips_and_has_the_reference_layout`、`round_trips_the_hnsw_options_through_a_wal_replay`。
- pysdk（bwrap 隔离、每轮独立数据目录）：`test_index` 961 passed；`test_show` 24 passed / 1 skipped（排除 40 分钟级的 `test_show_big_databases`）；`test_knn` 373 passed / 6 skipped；`test_explain` 1 passed。
- 跨引擎抽查：Rust 写 → Rust 崩溃恢复，参数完整；参考写 → Rust 打开，三条 LSG 索引可枚举、可查询。
