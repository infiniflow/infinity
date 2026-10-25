# Catalog 后端 FAQ：RocksDB、磁盘兼容与 fsync

> **子目标 S7 · catalog 底座与互操作**｜纲领见 [`PORTING.md`](PORTING.md) §5

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

**总原则**：写入路径**不**做逐条 fsync；屏障只在 checkpoint / shutdown / FLUSH。这与参考一致——参考的 WAL 走 RocksDB 且 `sync = false`，即「进程崩溃不丢、掉电可能丢」。

| 位置 | 行为 | 说明 |
|---|---|---|
| `WalManager::append` | 写文件，**不 fsync** | 注释在 `wal.rs:70`：逐记录 fsync 每次插入约 10ms；屏障在 `flush()` |
| `WalManager::flush` | `file.sync_all()` | checkpoint / shutdown / FLUSH 的屏障 |
| `WalManager::rotate` | 先 `sync_all()` 再 rename | 封存日志段前 |
| `WalManager::truncate_all` | `set_len(0)` + `sync_all()` | 日志被清空时 |
| `RocksMetaStore::flush` | `db.flush()` + `db.flush_wal(true)` | `flush` 只把 memtable 写成 SST，**只有 `flush_wal(true)` 才让日志真正落盘**；两者都做才是持久化屏障 |
| checkpoint 条目 `wal/wal.log` | 写完 `sync_all()` | 这个文件是参考判断「目录有没有 checkpoint」的唯一依据，掉电丢了参考就会拒绝打开目录 |
| `MetaStore::put` / `delete` / `atomic_batch` | 不 fsync | 由上面的 `flush()` 统一兜底 |

调用链：`Engine::flush_storage()` → `txn.flush()` → （WAL `sync_all` + store `flush`）→ `append_checkpoint()`（写并 fsync `wal/wal.log`）。

**因此**：进程崩溃 → 未 checkpoint 的数据由 WAL 重放，catalog 由 `atomic_batch` 原子提交；掉电 → 最多丢最后一次 checkpoint 之后的写入，与参考的 `sync=false` 语义相同。

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
