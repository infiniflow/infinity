# `PersistenceManager` 子系统移植计划（C++ → Rust）

> 目标子系统：参考的**对象持久化层**。它决定数据文件在 `data/` 之外以什么形态存在、catalog 里记什么、读的时候怎么还原。
> 交付口径与 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) 一致：同一输入下两引擎写出的对象布局可互读（`test_the_object_directory_is_shared_with_the_reference` 已双向验证当前的简化实现），且对象文件与 catalog 记录在字节上逐步对齐。
>
> 移植分两个**不可合并**的阶段：**阶段 1 逐函数机械翻译**（保留 C++ 的名字、控制流、错误语义，禁止顺手抽象），**阶段 2 按 Rust 习惯重构**（状态收拢、错误类型化、依赖注入、无锁化）。阶段 1 的产物是阶段 2 的**行为基准**：先有 golden 字节与用例，再重构。

---

## 1 子系统清单

| 文件（C++） | 行数量级 | 职责 | 现状（Rust） |
|---|---|---|---|
| `src/storage/persistence/persistence_manager.cppm` / `_impl.cpp` | ~700 | `PersistenceManager`：`ObjAddr` 编解码、compose 状态机、`pm\|object\|` 记录、引用计数、cleanup | 只有 `infinity_storage::object_dir`（`unpack` + 简化 `pack`） |
| `obj_status.cppm` / `_impl.cpp` | ~150 | `ObjStat`（`obj_size/parts/ref_count/deleted_ranges`）、`ObjCached`、`Range` | 无（`pack` 里内联写一条 stat JSON） |
| `obj_stat_accessor.cppm` / `_impl.cpp` | ~200 | `ObjectStats`：`obj_key → ObjStat` 的内存表 + KV 持久化、`Get/GetNoCount/PutNew/PutNoCount/Invalidate/Release` | 无 |
| `persist_result_handler.cppm` / `_impl.cpp` | ~100 | `PersistWriteResult` 的落地动作：本地上传/删除、远端删除；读时按需下载并置缓存位 | 无 |
| `src/storage/io/virtual_store*.cppm` / `virtual_store_impl.cpp` | ~600 | `VirtualStore`：local / MinIO 两套同名 API（`Exists/Open/Read/Write/UploadObject/DownloadObject/RemoveObject/ListObjects`） | 部分：`infinity_storage::s3`（自有客户端 + `sync_tree/restore_tree`），**无**同名抽象 |
| `src/storage/io/object_storage_process*.cppm` / `object_storage_task.cppm` | ~250 | 单线程队列，执行 `Download/Upload/Copy/Remove/LocalDrop` | 无（同步调用） |
| 调用方：`buffer/file_worker/file_worker_impl.cpp`、`invertedindex/{memory_indexer,column_index_merger,column_index_iterator}_impl.cpp`、`catalog/meta/chunk_index_meta_impl.cpp`、`common/snapshot_info_impl.cpp` | — | `Persist`（写）、`GetObjCache`/`PutObjCache`（读时缓存与引用计数）、`Cleanup`（删） | 端口直接在 `data/` 上读写，checkpoint 时整体 `pack` |

`PersistenceManager` 的对外接口（`persistence_manager.cppm:76-140`）就是移植面：

```
Persist(file_path, tmp_file_path, try_compose = true) -> PersistWriteResult   [[nodiscard]]
CurrentObjFinalize(validate = false)                  -> PersistWriteResult   [[nodiscard]]
GetObjCache(local_path)                               -> PersistReadResult    [[nodiscard]]
PutObjCache(file_path)                                -> PersistWriteResult   [[nodiscard]]
Cleanup(file_path)                                    -> PersistWriteResult   [[nodiscard]]
GetFileSize(file_path)                                -> (size_t, Status)
GetObjPath(obj_key) / GetAllObjects() / GetAllFiles() / SetKvStore()
```

---

## 2 必须逐条保真的语义

下面每一条都是格式或可见行为，移植时**先照抄**，阶段 2 也不许改语义（只改表达）。

1. **`ObjAddr` JSON**（`persistence_manager_impl.cpp:41-56`）：
   `{"obj_key":"<key>","part_offset":<u64>,"part_size":<u64>}`，键序与无空格写法都会落进 RocksDB 的值里。
2. **空文件**：`obj_key = "KEY_EMPTY"`、offset 0、size 0；不产生对象、不写 `pm|object_stat|`（`obj_stat_accessor_impl.cpp:148-151` 对 `KEY_EMPTY` 直接返回）。
3. **对齐**：每段起点向上取整到 `ObjAlignment = 8`（`persistence_manager.cppm:70`），段间填充字节为 0（`CurrentObjAppendNoLock` 用 `seekp` + 补零，`persistence_manager_impl.cpp:455-460`）。
4. **compose 上限**：`object_size_limit_`（默认 128 MB，`default_values.cppm:218-219`；配置键 `persistence_object_size_limit`）。
   - 文件 `size >= limit` 或 `try_compose == false` → **独占对象**：文件整体搬到 `workspace/<uuid>`，`ObjAddr(key, 0, size)`（`persistence_manager_impl.cpp:148-172`）；
   - 否则若 `size >= room`（`room = limit - current_size`，`:434`）→ 先 `finalize` 当前对象，再开新对象（`:175-177`）；
   - 单个对象 `current_size` 一旦 `>= limit` 参考直接 `UnrecoverableError`（`CurrentObjAppendNoLock:474-478`），端口不应复制这个"不该发生"的崩溃，改为按 `room` 提前切分。
5. **footer**：`finalize` 时若 `parts > 1`，在对象**末尾**追加 `u32 = 1`（`compose_format`，`persistence_manager_impl.cpp:272-286`）。`ObjStat::obj_size_` **不含** footer（`obj_status.cppm:41` 注释），footer 只增文件长度。
6. **统计记录**：`pm|object_stat|<obj_key>` = `{"deleted_ranges":[...],"obj_size":<N>,"parts":<N>}`；参考**每次 append 后**都用 `(current_size, parts, ref_count)` 覆写一次（`:186`），`finalize` 再覆写一次（`:288`）。
7. **引用计数**：`ObjectStats::Get` 自增、`GetNoCount` 不动（`obj_stat_accessor_impl.cpp:51-68`）；`GetObjCache` 命中时 `Get`（自增），`PutObjCache` 在读完时自减（`persistence_manager_impl.cpp:396-430`）；`ref_count` 只存在于内存，不落 KV。
8. **删除回收**：`Cleanup(local_path)` 把该路径的段范围记进 `deleted_ranges`；当所有段都被删除时对象整体删除（`CleanupNoLock`，`persistence_manager_impl.cpp:487+`），本地的进 `drop_keys_`、远端进 `drop_from_remote_keys_`——这两个列表就是 `PersistResultHandler::HandleWriteResult` 要执行的动作。
9. **缓存位**：`ObjCached::{kNotCached,kDownloading,kCached}`，读时若本地文件不存在则 CAS 到 `kDownloading` → 下载 → `kCached`，其他读者等待（`persist_result_handler_impl.cpp:56-81`）。
10. **路径键**：`RemovePrefix` 去掉 `local_data_dir_` 前缀得到相对路径（`persistence_manager_impl.cpp:603-611`），键形如 `pm|object|db_1/tbl_0/seg_0/blk_0/version`（分隔符是 `/`，跨平台）。
11. **不可恢复错误**：参考在"记录缺失/尺寸超限/引用计数为负"处 `UnrecoverableError`（进程终止）。移植时这些位置在阶段 1 保持"立即失败"，阶段 2 再决定哪些降级为可返回错误（见 §5）。

---

## 3 阶段 0：先定数据类型与编解码（1 天）

阶段 1 之前只做**数据**，不做行为：

| Rust 类型 | 对应 | 备注 |
|---|---|---|
| `ObjAddr { obj_key: String, part_offset: u64, part_size: u64 }` | `ObjAddr` | 手写 JSON 读写（当前 `infinity-storage` 没有 `serde_json`；为字节保真沿用 `object_dir.rs` 的手写方式） |
| `ObjStat { obj_size: u64, parts: u64, ref_count: u64, deleted_ranges: BTreeSet<Range>, cached: CacheState }` | `ObjStat` | `ToString` 的键序固定 |
| `Range { start: u64, end: u64 }` | `Range` | `Ord` 按 `start`，实现 `cover/intersect` |
| `CacheState { NotCached, Downloading, Cached }` | `ObjCached` | `AtomicU8` 承载 |
| `WriteOutcome { addr: ObjAddr, uploaded: Vec<ObjectKey>, dropped_local: Vec<PathBuf>, dropped_remote: Vec<ObjectKey> }` | `PersistWriteResult` | 字段一一对应，先不合并 |
| `ReadOutcome { addr: ObjAddr, dropped_local: ..., dropped_remote: ..., stat: Option<Arc<ObjStat>> }` | `PersistReadResult` | 同上 |

**验收**：用参考写出的真实值做 fixture（`pm|object|db_1/tbl_0/seg_0/blk_0/version` 等 4 条 + 1 条 `pm|object_stat|`，见本次调查的对象目录），Rust 编解码逐字节还原；`KEY_EMPTY`、空 `deleted_ranges` 两种边界各一条。

---

## 4 阶段 1：字面机械翻译（不改结构）

规则：**一函数对一函数**，保留 `_no_lock` 命名、参数顺序、返回结构；只用 `Mutex` 复刻 `mutable std::mutex`；错误位置照抄（该 panic 的地方 panic）；不做抽象、不做合并、不引入 trait。

| C++ | Rust（阶段 1） |
|---|---|
| `PersistenceManager::Persist(file_path, tmp, try_compose)` | `fn persist(&self, file_path: &str, tmp_file_path: &Path, try_compose: bool) -> Result<WriteOutcome>` |
| `ObjCreate()` | `fn obj_create(&self) -> String`（UUIDv4 形态） |
| `CurrentObjRoomNoLock()` | `fn current_obj_room_no_lock(&self) -> i64` |
| `CurrentObjAppendNoLock(tmp, size)` | `fn current_obj_append_no_lock(&self, tmp: &Path, size: usize) -> Result<()>` |
| `CurrentObjFinalizeNoLock(keys)` | `fn current_obj_finalize_no_lock(&self, keys: &mut Vec<String>) -> Result<()>` |
| `CleanupNoLock(addr, persist_keys, drop_remote, check_ref)` | `fn cleanup_no_lock(&self, addr: &ObjAddr, persisted: &mut Vec<String>, remote: &mut Vec<String>, check_ref: bool) -> Result<()>` |
| `GetObjCache(local_path)` | `fn get_obj_cache(&self, local_path: &str) -> Result<ReadOutcome>` |
| `PutObjCache(file_path)` | `fn put_obj_cache(&self, file_path: &str) -> Result<WriteOutcome>` |
| `Cleanup(file_path)` / `CurrentObjFinalize(validate)` / `GetFileSize` | 同名方法 |
| `RemovePrefix` / `GetObjPath` / `CheckValid` / `GetAllFiles` | 同名方法（`String` 仍用 `String`，阶段 2 再换 `Path`） |
| `ObjectStats::{Get,GetNoCount,PutNew,PutNoCount,Invalidate,Release,Deserialize,CheckValid}` | 同名方法，`HashMap<String, Arc<ObjStat>>` |
| `PersistResultHandler::{HandleWriteResult,HandleReadResult}` | 同名方法（仍单独成结构） |

**状态字段照抄**：`workspace`、`local_data_dir`、`object_size_limit`、`mutex`、`object_stats`、`current_object_key/size/parts/ref_count`。

**验收**：

1. 新增 `object_dir` 之外的 `persistence` 模块，单测覆盖：空文件、单个小文件、`parts = 1` 边界（不写 footer）、`parts ≥ 2` 写 footer、`size == limit-1`、`size == limit`（独占对象）、`size > limit`、一次 persist 跨多个对象（用缩小的 `limit` 注入）。
2. golden 对比：同一文件集用 C++（`storage_type = local`）与本实现各写一遍，`persistence/` 下的对象**逐字节**相同（footer、对齐、打包顺序），`pm|object|` / `pm|object_stat|` 记录逐条相同。
3. `test_the_object_directory_is_shared_with_the_reference` 两个方向仍绿。
4. 端口既有门禁（字节 17 项）不回归。

---

## 5 阶段 2：按 Rust 习惯重构（行为不变）

阶段 2 每一小步都要求阶段 1 的 golden 用例继续通过。

| # | 现状（阶段 1） | 重构后 | 理由 |
|---|---|---|---|
| 1 | `&self` + `Mutex` 包住全部可变状态；4 个 `current_object_*` 字段散落 | `&mut self`；`struct Composer { key: String, size: usize, parts: usize, refs: usize }`，用 `Option<Composer>` 表示"没有未封口的对象" | "未封口对象" 是一个状态，不是 4 个互相约束的字段；`Option` 让 `finalize/seal` 变成 `fn seal(&mut self) -> Option<Sealed>`，非法状态不可表示 |
| 2 | `current_obj_finalize_no_lock(&mut Vec<String>)` 通过出参返回"产生了哪些对象" | `fn seal(&mut self) -> Result<Option<ObjectKey>>` | 出参改返回值；调用方（checkpoint）一次拿全 |
| 3 | `unreachable`/`panic!` 复刻 `UnrecoverableError` | `Error::Persistence { .. }`（I/O）/`Error::Corrupt { .. }`（记录缺失）/`Error::LimitExceeded`（尺寸超限）；只有真正"继续跑会写坏数据"的点保留 `debug_assert!` | 进程崩溃不是 Rust 的错误模型；调用方（`file_worker`）能拿到 `Result` 决定回滚 |
| 4 | `std::set<Range>` | `BTreeSet<Range>`，`Range` 实现 `Ord`（按 `start`）+ `cover/intersect` | 直接对应，去掉自定义比较器样板 |
| 5 | 路径 `String`（`workspace`、`tmp`、`file_path`） | `Path`/`PathBuf`；只有 catalog 键保留 `String`（`pm\|object\|db_1/...` 是键，不是路径） | 路径拼接不再靠 `format!`；分隔符只在键生成处出现一次 |
| 6 | `ObjectStats` 里再查全局 `InfinityContext::instance().persistence_manager()` 拿 `KVStore` | 构造时注入 `Arc<dyn MetaStore>`；`ObjectStats::put_new(&self, ...)` 直接用 | 去掉全局单例与反向指针（端口已有 `Arc<dyn MetaStore>`） |
| 7 | `PersistResultHandler` 只是 2 个方法转调 | 合并为 `PersistenceManager::apply_write(outcome)` / `resolve_read(addr)`（或保留结构但只做 `impl` 转发） | 消除 2 层空壳；`HandleWrite` 的动作本来就是 manager 的职责 |
| 8 | `PersistWriteResult` 的 3 个 `Vec<String>` 平铺 | `WriteOutcome { addr, uploaded: Vec<ObjectKey>, dropped_local: Vec<PathBuf>, dropped_remote: Vec<ObjectKey> }`，`ObjectKey` newtype | 三个列表语义不同，newtype 防串 |
| 9 | `GetObjCache` 里用 `obj_stat.cached_` + `expect/wait` 手写三态 | `enum CacheState` + `Mutex<CacheState>` + `Condvar`，封装成 `fn acquire(&self, key) -> Guard` | 三态转移与等待集中在一处 |
| 10 | `_no_lock` 后缀 | 删除后缀；不变式由 `&mut self` 保证 | 命名反映真实约束 |
| 11 | `[[nodiscard]]` 靠约定 | `#[must_use]` 标在返回值上 | 编译期检查 |
| 12 | `Storage*` 反向指针 | 不移植：端口用 `Arc<dyn MetaStore>` + `PathBuf` 显式依赖 | 参考用它拿全局配置，端口不需要 |

**验收**：阶段 1 的 golden 用例逐字节仍相同；`cargo clippy` 无新增告警；`object_dir` 收敛为 `persistence` 的 `unpack`/`pack` 两个入口（`pack` 变成 `persist` 的批量封装）。

---

## 6 与端口现有代码的接合点

| 端口位置 | 现状 | 移植后 |
|---|---|---|
| `infinity-storage/src/object_dir.rs` | `unpack` + 简化 `pack`（单对象、无上限、无 footer、无 stat/ref） | 由 `persistence` 模块取代；`unpack` 保留为"打开时把对象还原成 `data/`" |
| `EngineOptions::object_dir` | `Option<PathBuf>` | 换成 `Option<PersistenceConfig { workspace, object_size_limit }>` |
| `Engine::open_with_options` | 打开 catalog 后 `unpack` | `PersistenceManager::new(...)` + `pm.restore_missing_files()`（内部读 `pm\|object\|`） |
| `Engine::flush_storage` | checkpoint 末尾 `pack` | `pm.seal()` + `pm.apply_write(...)`（IMPORT/COMPACT/OPTIMIZE/DUMP 后按参考显式 `CurrentObjFinalize`） |
| `infinity-cli` 配置 | `persistence_format = "reference"` | 加 `persistence_object_size_limit`（参考同名键；只在端口配置里写，参考会拒绝 `[storage]` 的未知键） |
| `infinity-storage/src/s3.rs` | 路径树 `sync_tree/restore_tree` | 传输层换成 `VirtualStore` 的两个实现（local / minio），对象键与 `pm\|object\|` 记录由 manager 产生 |

---

## 7 任务拆分

| 里程碑 | 内容 | 完成判据 |
|---|---|---|
| **M0** | 数据类型与 JSON 编解码（§3） | fixture 往返逐字节一致 |
| **M1** | 字面移植 `Persist` + `CurrentObj{Append,Finalize,Room}` + `ObjCreate` + `RemovePrefix` | 单测：空/小/独占/footer/跨对象；C++ golden 逐字节相同 |
| **M2** | 字面移植 `ObjectStats` + `GetObjCache`/`PutObjCache` + `Cleanup{NoLock}` | 单测：引用计数、deleted_ranges 合并、对象整体删除；重开目录后计数一致 |
| **M3** | `PersistResultHandler` 的落地动作 + 接进 `Engine::open_with_options`/`flush_storage` | 对象目录门禁双向绿；字节门禁 17 项不回归 |
| **M4** | 阶段 2 的 12 项重构（一次 1–2 项，每步跑 golden） | golden 不变、clippy 干净 |
| **M5** | 文档与门禁更新（`FORMAT_EQUIVALENCE_TESTS.md` §6.5、覆盖矩阵） | 文档如实描述 compose 规则 |

---

## 8 风险与取舍

1. **footer 的位置**：`u32` 追加在最后一个段之后，读者按 `[part_offset, part_offset+part_size)` 读不会碰到它；但对象文件的**长度**与 `obj_size_` 差 4 字节。若将来做"对象文件逐字节比对"，这 4 字节必须一致。
2. **一次 persist 产生多个对象**：现有端口实现恒定写一个对象。改成按 `room` 切分后，`persist_keys_` 会变长，`HandleWriteResult` 需要逐个上传/统计——阶段 1 必须一起移植，否则"小对象"场景两边对象数不同。
3. **`ref_count` 不落盘**：参考重启后从 0 开始；端口不能把它写进 `pm|object_stat|`，否则记录与参考不同。
4. **`GetFileSize/CheckValid/GetAllObjects`** 目前没有调用方（只有 `unittest`），阶段 1 可以先不做，标 `todo!()` 并在 M2 补齐。
5. **`UnrecoverableError` → `Result` 的降级**：`Persist` 在"记录缺失"处放弃进程，改成 `Err` 后调用方（`file_worker`）若忽略返回值，会从"崩溃"变成"数据静默丢失"。阶段 2 要先给调用方加 `?` 或显式日志，再改错误类型。
6. **`Storage*`/全局单例**：不移植。端口已有的 `Arc<dyn MetaStore>` 是更干净的依赖；代价是 `ObjectStats` 的 KV 写入需要一个 `MetaStore` 句柄，`persistence` 构造时注入即可。
7. **多进程共享对象目录**：参考假设单写者（`current_object_*` 只在内存）。端口若同时开两个进程写同一目录会互相覆盖对象；这与现有 checkpoint 的 `flush_lock` 是同一类问题，移植时保持"单写者"并在文档写明。

---

## 9 不做什么

- 不移植 `Storage*` 反向指针与 `InfinityContext::instance()` 全局查找。
- 不移植 `object_storage_process` 的单线程队列：端口的 `VirtualStore` 实现直接同步调用即可（S3 客户端本身自带重试）。
- 不追求"对象文件字节与参考相同"以外的目标（例如对象内段的排列顺序）：排列顺序已经是可观察格式（见 `test_the_object_directory_is_shared_with_the_reference`），按 §2.4/§2.5 保真即可。
- 不改造 `file_worker` 的缓冲管理；端口已有自己的镜像写出路径，`persist` 只在 checkpoint 边界被调用。
