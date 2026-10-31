# Catalog 后端 FAQ：RocksDB、磁盘兼容与 fsync

> **子目标 S6 · catalog 底座与互操作**｜纲领见 [`PORTING.md`](PORTING.md) §5

这份文档回答三个反复出现的问题：**Rust 侧用哪个 catalog 后端**、**它和参考实现的目录能不能互相读**、**在哪些点上真的做了 fsync**。

实现位置：`crates/infinity-storage/src/metastore.rs`（`MetaStore` trait）、`crates/infinity-storage/src/metastore_rocks.rs`（RocksDB，唯一实现）、`crates/infinity-catalog/src/cpp_layout.rs`（C++ 键空间/值编码）、`crates/infinity-storage/src/wal.rs`（重做日志）。

---

## 1. 只有一个后端：RocksDB

| | |
|---|---|
| 代码 | `RocksMetaStore`（rust-rocksdb 0.25） |
| catalog 目录 | `<dir>/catalog`（RocksDB；`catalog\| db\| tbl\| col\| idx\| seg\| blk\|` 与原生键空间同库） |
| 行/元数据目录 | `<dir>/persistence`（RocksDB） |
| 目录可被 C++ 打开 | ✅ 同一个底座、同一套键 |
| Catalog 语义 | `Catalog::with_store`（原生键空间 + C++ 布局镜像；checkpoint 时把布局写全） |

没有 feature、没有探测器、没有离线转换工具：底座就是参考用的那个，所以同一个二进制打开两边写的目录都是同一段代码。

---

## 2. 磁盘兼容性现状

| 内容 | 兼容情况 |
|---|---|
| 表数据（块镜像 `data/db_{id}/tbl_{id}/seg_{n}/blk_{m}/`） | ✅ 逐字节对齐参考布局（`cpp_format`），C++ 可读 |
| catalog 的表/列/索引记录 | ✅ 键与值都对齐 `KeyEncode`/`DataType::Serialize`（`cpp_layout.rs`） |
| 次级索引 chunk | ✅ 类型化键（`KeyKind::{U64,F32,F64,I32,I64}`），C++ 的 FLOAT/DOUBLE/DATE/TIME 块可读 |
| WAL | ✅ `wal/wal.log` 同时承载 checkpoint 帧与事务记录（checkpoint 时整文件改名成 `wal.log.<seal_ts>`，同 `WalManager::SwapWalFile`），帧体与参考逐字节相同 |
| HNSW / 全文 / 二级索引的索引目录 | ✅ 各自有 chunk 镜像并登记 `catalog\|idx_chunk\|…\|chunk_info` |

跨引擎门禁：`python/restart_test/test_cross_engine_compat.py`（C++ 写 → Rust 读，反之亦然，核对 catalog/数据/每个索引）。`rust_engine/scripts/run_gate_c.sh` 的跨引擎腿直接用它构建出来的 `infinity` 二进制。

## 3. fsync 在哪些点上

**总原则**：写路径上**一次 fsync 都没有**，与参考一致。参考的 WAL 线程只做 `ofs_.write` + `ofs_.flush()`（用户态 flush，不是 fsync，`wal_manager_impl.cpp:311,329-340`），换文件只是 close + rename（`:511-513`）；catalog 的 RocksDB 写入带 `disableWAL`（`kv_store_impl.cpp:311,441`），checkpoint 时一次 `Flush()`（`:390`）把 memtable 变成 SST。

| 位置 | 行为 | 参考对应 |
|---|---|---|
| `WalManager::append` | `write_all`，不 fsync（`File` 无用户态缓冲） | `WalManager::Flush` 的 `ofs_.write` |
| `WalManager::rotate` | 关文件 + rename，不 fsync | `WalManager::SwapWalFile` |
| checkpoint 帧（`append_checkpoint`） | 先 rotate 再 append，不 fsync | 同一个 `ofs_` 循环里的 `SwapWalFile` + 帧写入 |
| `RocksMetaStore::put` / `delete` / `atomic_batch` | `WriteOptions::disable_wal(true)`；只有 `pm|` 开头的对象记录开着 WAL，随后 `flush_wal(false)` | `KVStore::Put/Delete/Merge` 默认 `disable_wal = true`；`PersistenceManager` / `ObjStatAccessor` 传 `false` 并 `FlushWAL(false)` |
| `RocksMetaStore::flush`（checkpoint） | `db.flush()`：memtable → SST（RocksDB 自己 fsync SST 与 MANIFEST） | `KVStore::Flush()` = 一次 `transaction_db_->Flush()` |
| 提交（语句 / 事务） | append 一条 WAL 帧 + 写 KV，没有同步步骤 | `NewTxn::CommitTxn` 把帧交给 WAL 线程 |

调用链：`Engine::flush_storage()` → `catalog.flush()`（SST）→ `txn.flush()`（store 的 SST）→ `append_checkpoint()`（必要时 rotate，再写帧）。

**语义**：承诺的持久化边界是 **checkpoint，不是 commit**——commit 只写页缓存，掉电可以把它丢掉。进程崩溃（页缓存还在）通常会保留到 WAL 尾部，但那不是承诺。

**掉电后持久的是什么**：两个 RocksDB 的 SST（RocksDB 在 flush 里自己 fsync SST 与 MANIFEST）**加上块文件**。块文件对齐参考的 `FileWorker::WriteToFile`（`file_worker_impl.cpp:167`）：`{id}.col` / `col_{id}_out` / `version` 每个文件写完就 fsync（`cpp_format::write_synced`），目录本身不 fsync（参考也不 fsync 目录）。行数据的权威副本仍是 `persistence` 库里的行记录：块是镜像，恢复时 `recover_cpp_blocks` → `reconcile_mirror_marks` 按 KV 与块文件的行数差重建，所以块文件落后不会丢数据，只会让这次恢复多干一点活。索引 chunk 文件各有各自的 writer，`infinity-index` 的持久化走 `sync_all`，`cpp_blocks` 里少数 chunk 镜像仍是普通 `std::fs::write`（参考对这些文件也做 fsync），这是还没对齐的一处。

**两个库的处理**：参考只有一份 KV，catalog 记录与 `applied` 标记天然同库，丢一起丢。端口有两个 RocksDB（`catalog` 与 `persistence`），所以 `applied` 标记**两边都写**、重放下界取两者**最小值**（`apply_split` / `TxnManager::applied_ts`）。否则 RocksDB 后台把其中一个 memtable 刷成 SST、另一个随崩溃丢掉时，重放会相信领先的那个标记，跳过落后的库丢掉的记录。

---

## 4. 常见坑

1. **同一个目录只能有一个进程打开**：RocksDB 在目录里加锁，上一个引擎没退干净再开就会 `lock hold by current process`。测试前 `pkill -x infinity`。
2. **不要留下 0 字节的 `wal/wal.log`**：参考把空日志当成坏条目（`Found unexpected bad wal entry`），删除它又会变成 `WAL replay: No checkpoint found in wal`。正确做法是写入一条合法的 checkpoint 条目。
3. **参考必须能在 WAL 里找到 checkpoint**：任何有数据的目录都要有 `kNewCheckpoint` 条目，`max_checkpoint_ts` 从它来。
4. **变长列必须写 `col_{id}_out`**：`ColumnMeta::FilePaths` 会为 kVarBuffer 类型列出这个 outline 文件，缺失即 abort 整个块读取。
5. **BMP 倒排表按列声明的 bound 定尺寸**（不是去重 term 数）：参考读该文件时段与段之间没有长度字段，term 数少一个就整体错位。
6. **端口 23817**：参考不设 `SO_REUSEADDR`，上一个引擎没退干净就会 `Address already in use`；另外 SDK 关闭连接后**服务端**会进 `TIME_WAIT`（Linux 60s），这期间参考绑不上 23817——所以端口等待要超过 60s。
7. **索引数量会影响参考自己**：一张有 6 个索引的表让参考在自己的 index-dump 任务里 abort（"Dump index ... Aborted"，正在把全文索引的 FST 并进字典）。只有 2 个索引、其中一个是 DATE 次级索引时正常，所以触发条件更像是索引个数而不是类型；这条还没查清，跨引擎测试因此不给 DATE 列建次级索引。
8. **数据目录必须落磁盘**，不能用 tmpfs（Gate C 的导入用例会把 tmpfs 撑爆并报 ENOSPC）。

---

## 5. 怎么验证

```bash
# 单元/集成
cd rust_engine && cargo test -p infinity-storage --lib
cargo test -p infinity-catalog --lib
cargo test -p infinity-txn --lib            # WAL / 恢复
cargo test -p infinity-storage --test crash_consistency

# 双引擎交接（C++ ↔ Rust），两个方向都要过
cd rust_engine && cargo build --release -p infinity-cli
INFINITY_CPP_BIN=$PWD/../cmake-build-debug/src/infinity \
  ../.venv/bin/python -m pytest -q ../python/restart_test/test_cross_engine_compat.py
```

相关文档：格式细节见 [`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md)；未覆盖与未实现项见 [`GAPS.md`](GAPS.md)。
