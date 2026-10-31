# Gate G — C++ 内部不变量 → Rust 测试对照

> **子目标 S5 · 内部不变量有测试兜住**｜纲领见 [`PORTING.md`](PORTING.md) §5

计划书 §6 Gate G 要求：产出一份「C++ 内部不变量 → Rust 测试」清单并纳入门槛，覆盖
①配置校验/拒绝行为、②存储/事务不变量、③索引不变量；**清单外的例外须显式登记并说明理由**。

Rust 侧只登记**已存在**的测试，名称与 `cargo test` 输出一致；C++ 侧以计划书「附录 A：核心不变量」
与 `src/unit_test/main/config_ut.cpp:175` 为锚点。还没被任何测试兜住的不变量见 §4。

## 1. 配置校验 / 拒绝行为

| C++ 不变量 / 用例 | Rust 对应 | 状态 |
|---|---|---|
| 参数越界必须**拒绝**而非静默钳制（`config_ut.cpp:175`） | `infinity-executor: config_validation`（`SET CONFIG` 对**已知键**做值域校验：`result_cache ∈ {on,off,clear,suspend}`、`cache_result_capacity` 正整数、`CHECKPOINT_INTERVAL` 非负整数；越界/非法即报错，未知键仍容忍） | ✅ |
| `Connect` 的 `client_version` 必须匹配（否则握手失败） | `infinity-server: handshake_policy`、`tcp_connect_success_and_mismatch` | ✅ |
| 校验和错误的 WAL 帧必须**停止读取**而非跳过 | `infinity-storage: corrupt_checksum_stops_reading` | ✅ |
| 撕裂尾（torn tail）必须被容忍并丢弃 | `infinity-storage: torn_tail_is_ignored` | ✅ |
| 空事务必须被拒绝 | `infinity-txn: empty_txn_rejected` | ✅ |
| 重名表 / 重名列必须被拒绝 | `infinity-catalog: duplicate_name_is_rejected`；executor `defaults_and_alter_add_column`（重复列） | ✅ |
| 张量维度/嵌套不合法必须报错 | executor：`TENSORARRAY` 三重嵌套与维度校验（`.slt` 的 `statement error` 记录） | ✅ |
| 列类型不在参考白名单时必须拒绝（`Decimal`/`HugeInt`/`Interval`/`UUID`，含 `ARRAY` 元素） | executor `the_reference_column_type_whitelist_is_enforced`；RPC 侧 `tcp_create_table_round_trip_with_engine` | ✅ |

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
| CRC-32/IEEE 与 C++ **逐字节一致** | `infinity-storage: standard_check_value`、`differs_on_one_bit`；WAL 帧级：`test_the_wal_entries_match_byte_for_byte`（整帧，含 CRC） | ✅ |
| 行墓碑（DELETE）对查询**不可见**，对聚合**不计入** | executor `delete_with_where_and_all` | ✅ |
| `DROP TABLE` 后同名重建必须**空表** | executor（`DROP TABLE` 清理行/墓碑/索引；`test_simple_agg` 由 38→77 通过） | ✅ |
| 快照/恢复（`CREATE/DROP/RESTORE TABLE SNAPSHOT`） | executor：快照序列化 + 恢复重建（`test/sql/snapshot/**` 10 文件全绿） | ✅ 基础 |
| **检查点/compaction 后恢复**（WAL 截断后仍可恢复；恢复后的写入再次重启仍在） | `infinity-executor: restart_checkpoint`（①WAL 全截断后重启仍完整；②删掉已 compact 的 WAL 文件仍完整；③恢复后的新写入再重启不丢） | ✅ |
| A1 三底座**彼此独立**且耐久性契约不同 | 代码结构（KV/WAL/块文件各自 crate 边界）；**跨底座宕机一致性**：`infinity-storage: crash_consistency`（5 用例，见 §4） | ✅ |
| WAL 帧与参考逐字节相同（含 `txn_id` 序列） | `test_the_wal_entries_match_byte_for_byte`、`test_the_ddl_wal_commands_match_the_reference` | ✅ |

## 3. 索引不变量

| C++ 不变量（附录 A3） | Rust 对应 | 状态 |
|---|---|---|
| 二级索引：插入后可查、范围扫描、**持久化后重建** | `infinity-index: insert_lookup`、`range_scan`、`persist_and_load`；executor `index_scan_matches_expected_and_survives_restart` | ✅ |
| 向量索引：Flat 精确 top-1 | `infinity-index/vector: flat_exact_top1` | ✅ |
| HNSW 能找到精确解，且**召回率**达标 | `infinity-index/vector: hnsw_finds_exact_vector`、`hnsw_recall_vs_flat` | ✅ |
| 全文倒排：分词、单词查询、AND 查询与排序、top-k 截断 | `infinity-index/fulltext: tokenize_basic`、`single_term_lookup`、`and_query_and_ranking`、`top_k_limits`；**打分单调性**：`infinity-executor: fulltext_score` | ✅ |
| A3 逐索引**文件集合/发布/合并/清理**契约（向量 `.idx`、全文 `.dic/.pos/.spill/.len`） | `infinity-storage/tests/index_file_family.rs` | ✅ |
| `DROP INDEX` 后重建 | `infinity-executor: index_drop_recreate`（DROP 前后与重建后查询结果一致；重建后写入的新行可见） | ✅ |

## 4. 显式登记的例外

| # | 项 | 说明 |
|---|---|---|
| E1 | `miri` / sanitizer 的 crate 范围 | `run_gate_d.sh` 跑 miri、AddressSanitizer、ThreadSanitizer（`nightly-2025-04-04`），只覆盖纯 Rust 核心（`infinity-parser`/`infinity-expression`/`infinity-planner`，各 38 用例）。`infinity-storage`/`infinity-catalog`/`infinity-executor` 的依赖 MSRV 高于该 pinned nightly，带 FFI/SIMD 的 crate 未纳入；要覆盖需把 Gate D 的 pin 升到工作区当前用的 toolchain（`rust-toolchain.toml` 因 `std::simd` 固定为 `nightly-2026-10-29`） |
| E2 | 索引文件族契约的测试形态 | `index_file_family.rs`（4 用例）固定：路径形状 `db_{db}/tbl_{tbl}/idx_{index}/seg_{seg}`、向量族 `chunk_{n}.idx`、块数据 `db_{db}/tbl_{tbl}/seg_{seg}/blk_{blk}` 与 `{col}.col`/`col_{id}_out`/`version`；全文族后缀 `.dic`/`.pos`/`.spill`（瞬时）/`.len`；并用真实编解码器把 `.dic`+`.pos`+`.len` 组成可回读的倒排索引 |
| E3 | 全文精确分值 | 单调性（tf、长度、命中）由 `fulltext_score.rs` 与 §2 的精确口径单测兜住；与参考的逐值一致性由 Gate A（`.slt` 全量）和 `FORMAT_EQUIVALENCE_TESTS.md` §6.2 承担，不在本清单里重复 |

## 5. 使用方式

```bash
# 全部登记项（Rust 侧）
cd rust_engine && cargo test --workspace
# Loom 模型（commit_ts 分配唯一性）
RUSTFLAGS="--cfg loom" cargo test -p infinity-txn --lib loom
```

本清单的例外只有 §4 三条：E1 是工具链限制，E2/E3 说明了测试的形态与归属。§1–§3 的每一行都有对应的
`cargo test` 用例，且都在 CI（`--workspace`）里跑。
