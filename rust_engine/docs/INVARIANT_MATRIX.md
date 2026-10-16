# Gate G — C++ 内部不变量 → Rust 测试 对照清单

> 计划书 §6 Gate G 要求：**必须**产出一份「C++ 内部不变量 → Rust 测试」清单并纳入门槛，
> 覆盖 ①配置校验/拒绝行为、②存储/事务不变量、③索引不变量；
> **清单外的例外须显式登记并说明理由**，不得默认遗漏。
>
> 本文件为 **v0.1**：Rust 侧只登记**已存在**的测试（名称与 `cargo test` 输出的完全一致），
> 未覆盖项在「缺口」列写明状态。C++ 侧以计划书「附录 A：核心不变量」与 CI 的
> `src/unit_test/main/config_ut.cpp:175` 为锚点。最后更新：2026-10-17（G1、G3、G5 闭合）。

## 1. 配置校验 / 拒绝行为

| C++ 不变量 / 用例 | Rust 对应 | 状态 |
|---|---|---|
| 参数越界必须**拒绝**而非静默钳制（`config_ut.cpp:175`） | `infinity-executor: config_validation`（`SET CONFIG` 对**已知键**做值域校验：`result_cache ∈ {on,off,clear,suspend}`、`cache_result_capacity` 正整数、`CHECKPOINT_INTERVAL` 非负整数；越界/非法即报错，未知键仍容忍） | ✅ |
| `Connect` 的 `client_version` 必须匹配（否则握手失败） | `infinity-server: handshake_policy`、`tcp_connect_success_and_mismatch` | ✅ |
| 校验和错误的 WAL 帧必须**停止读取**而非跳过 | `infinity-storage: corrupt_checksum_stops_reading` | ✅ |
| 撕裂尾（torn tail）必须被容忍并丢弃 | `infinity-storage: torn_tail_is_ignored` | ✅ |
| 空事务必须被拒绝 | `infinity-txn: empty_txn_rejected` | ✅ |
| 重名表 / 重名列必须被拒绝 | `infinity-catalog: duplicate_name_is_rejected`；executor `defaults_and_alter_add_column`（重复列） | ✅ |
| 张量维度/嵌套不合法必须报错 | executor：`TENSORARRAY` 三重嵌套校验（`statement error`） | ✅ 部分（见 §4 缺口） |

## 2. 存储 / 事务不变量

| C++ 不变量（附录 A） | Rust 对应 | 状态 |
|---|---|---|
| A2 **MVCC = 时间戳数组**，更新 = 删除 + 追加（非 KV 快照） | `infinity-storage: append_then_visibility`、`update_is_delete_plus_append`、`rollback_delete_restores_row`、`delete_respects_snapshot`；executor `update_is_delete_plus_append` | ✅ |
| A4 提交管线 `Prepare→WriteWAL(串行)→CommitBottom` | `infinity-txn: concurrent_commits_are_serialized_and_durable`、`commit_applies_to_kv` | ✅ |
| WAL 回放**幂等**（重复回放不产生副作用） | `infinity-txn: recovery_replays_wal_after_reopen` | ✅ |
| WAL 截断后恢复**恰好**有效前缀 | `infinity-txn: truncated_wal_recovers_exactly_the_valid_prefix`；`infinity-storage: replay_since_filters_by_commit_ts`、`truncate_keeps_tail` | ✅ |
| `commit_ts` 分配在任意交错下**唯一无缺口** | `infinity-txn: commit_ts_allocation_is_unique`（+ `--cfg loom` 的 `loom_model`） | ✅ |
| 元数据 KV 原子批写「全有或全无」 | `infinity-storage: atomic_write_is_all_or_nothing` | ✅ |
| 元数据跨重开**持久**，前缀扫描**有序** | `infinity-storage: survives_reopen_after_flush`、`scan_prefix_is_ordered`；catalog `survives_reopen_with_columns`；executor `persists_across_restart` | ✅ |
| CRC-32/IEEE 与 C++ **逐字节一致** | `infinity-storage: standard_check_value`、`differs_on_one_bit` | ✅ |
| 行墓碑（DELETE）对查询**不可见**，对聚合**不计入** | executor `delete_with_where_and_all` | ✅ |
| `DROP TABLE` 后同名重建必须**空表** | executor（`DROP TABLE` 清理行/墓碑/索引；`test_simple_agg` 由 38→77 通过） | ✅ |
| 快照/恢复（`CREATE/DROP/RESTORE TABLE SNAPSHOT`） | executor：快照序列化 + 恢复重建（`test/sql/snapshot/**` 10 文件全绿） | ✅ 基础 |
| A1 三底座**彼此独立**且耐久性契约不同 | 代码结构（KV/WAL/块文件各自 crate 边界）；**跨底座宕机一致性**：`infinity-storage: crash_consistency`（5 用例，见 §4） | ✅ |

## 3. 索引不变量

| C++ 不变量（附录 A3） | Rust 对应 | 状态 |
|---|---|---|
| 二级索引：插入后可查、范围扫描、**持久化后重建** | `infinity-index: insert_lookup`、`range_scan`、`persist_and_load`；executor `index_scan_matches_expected_and_survives_restart` | ✅ |
| 向量索引：Flat 精确 top-1 | `infinity-index/vector: flat_exact_top1` | ✅ |
| HNSW 能找到精确解，且**召回率**达标 | `infinity-index/vector: hnsw_finds_exact_vector`、`hnsw_recall_vs_flat` | ✅ |
| 全文倒排：分词、单词查询、AND 查询与排序、top-k 截断 | `infinity-index/fulltext: tokenize_basic`、`single_term_lookup`、`and_query_and_ranking`、`top_k_limits`；**打分单调性**：`infinity-executor: fulltext_score`（见 §4 G5） | ✅ |
| A3 逐索引**文件集合/发布/合并/清理**契约（向量 `.idx`、全文 `.dic/.pos/.spill/.len`） | — | **缺口**：Rust 侧索引未落盘为独立的块文件族，尚无「文件集合/发布/合并/清理」契约测试 |
| `DROP INDEX` 后重建 | executor：`DROP INDEX` + 元数据重建路径 | ⚠️ 无专项测试 |

## 4. 显式登记的例外与缺口（Gate G 要求）

| # | 缺口 | 理由 / 影响 |
|---|---|---|
| G1 | ~~**配置项的范围校验**（`config_ut.cpp` 类）~~ | **已闭合（2026-10-17）**：`SET CONFIG` 对已知键（`result_cache`/`cache_result_capacity`/`CHECKPOINT_INTERVAL`）做值域校验并**拒绝**越界/非法值（不静默钳制）；未知键保持容忍。回归测试：`infinity-executor/tests/config_validation.rs`（4 用例）。 |
| G2 | **索引文件族契约**（A3） | Rust 侧索引当前在内存 + KV 元数据中维护，尚未按 C++ 的「块文件族 + `ChunkIndexMetaInfo` 发布」落地；需在磁盘驻留索引（M4+）阶段补测。 |
| G3 | ~~**跨底座宕机一致性**（KV+WAL+块文件同时崩溃）~~ | **已闭合（2026-10-17）**：`infinity-storage/tests/crash_consistency.rs` 把 PoC C 的边界场景落成回归测试 —— ①块写后崩溃→事务不可见；②WAL 落盘后崩溃→回放可见；③完整提交→无需回放即可见；④块文件丢失→由 WAL 载荷重建；⑤WAL 撕裂尾→回放在有效前缀处停止。 |
| G4 | **`miri` / sanitizer** 覆盖 | **部分闭合（2026-10-17）**：miri 已在 `nightly-2025-04-04` 上对纯 Rust 核心（`infinity-parser`/`infinity-expression`/`infinity-planner`，共 38 用例）跑通，`run_gate_d.sh` 已接上；**sanitizer（TSAN/ASAN）** 仍需带 FFI/SIMD 的 nightly 与工具链，未覆盖。 |
| G5 | ~~全文**打分**不变量（相似度单调性等）~~ | **已闭合（2026-10-17）**：`infinity-executor/tests/fulltext_score.rs`（BM25 近似实现的单调性不变量）：①更高 tf 不打低于低 tf；②等 tf 下更短文档不打低于更长文档；③不含查询词的行不出现在结果里。精确分值与 C++/iresearch 的对齐仍属打分移植任务（见 §5.1）。 |

## 5. 使用方式

```bash
# 全部登记项（Rust 侧）
cd rust_engine && cargo test --workspace
# Loom 模型（commit_ts 分配唯一性）
RUSTFLAGS="--cfg loom" cargo test -p infinity-txn --lib loom
```

> 结论：Gate G 的**清单已产出**；配置校验（G1）、索引文件族（G2）、跨底座崩溃回归（G3）、
> miri/sanitizer（G4）为**已登记缺口**，尚未满足「纳入门槛」的条件。
