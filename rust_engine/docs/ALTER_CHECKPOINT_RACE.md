# `ALTER TABLE` 与 checkpoint 竞争：块列错位

> 对应用例：`executor` `alter_ut::a_checkpoint_through_a_column_change_keeps_the_block_readable`
> （`#[ignore = "known-defect: …"]`）。本文记录现象、实测证据、根因与已排除的修法，
> 供后续收口时不再重走弯路。

## 1 现象

用例跑 50 轮，每轮：

```sql
CREATE TABLE tb1 (c1 INT, c2 INT, c3 VARCHAR);      -- 单块，1 行
INSERT INTO tb1 VALUES (1, 2, 'test');
-- 另一线程不停执行 checkpoint_wal() + flush()（`PeriodicTriggerThread::Run`）
ALTER TABLE tb1 ADD COLUMN (c4 VARCHAR DEFAULT 'tttt');
ALTER TABLE tb1 DROP COLUMN (c2);
ALTER TABLE tb1 ADD COLUMN (c2 FLOAT DEFAULT 0.0);
INSERT INTO tb1 VALUES (2, 'test2', 'tttt2', 1.0);
-- 关引擎、重开，块是唯一副本
SELECT c1, c3, c4 FROM tb1 ORDER BY c1;
```

失败时断言是：

```
left:  [["1", "2", "test"],     ["2", "test2", "tttt2"]]
right: [["1", "test", "tttt"],  ["2", "test2", "tttt2"]]
```

- **只有第一行错**，第二行（在全部 DDL 之后插入）永远正确。
- 第一行的 `c3` 读到 `"2"`——被 DROP 的 `c2` 的旧值；`c4` 读到 `"test"`——本属于 `c3` 的值。
  即整行**右移一列**。

实测失败率（**初测**：20 轮，debug 构建，本机 4 vCPU，带探针插桩，
`cargo test -p executor --lib … -- --ignored`）：16 通过 / 4 失败，约 20%。
另有一批复测（24 轮，无插桩）1 次失败；两批合并 44 轮 5 次 = 11%，见 §6。
该用例最初是在 arm64 runner 上被记录为 flaky 后加 `#[ignore]` 的，慢机器上窗口更大。

## 2 证据：错在磁盘，不在读取

在 `Engine::open` 之前直接 dump 块文件字节，并在 `open` 之后用三条不同路径读同一块。

**失败那一轮**（`seg_0/blk_0/`）：

| 文件 | `head` 前 28 字节 | 内容 | 结论 |
|---|---|---|---|
| `0.col` | `… 1,0,0,0 · 2,0,0,0 …` | `1`、`2` | c1，正确 |
| `2.col` | `… 1,0,0 · 50 …` | `"2"`（len=1） | **c3 的位置存了 c2 的旧值** |
| `3.col` | `… 4,0,0 · t,e,s,t …` | `"test"`（len=4） | **c4 的位置存了 c3 的值** |
| `4.col` | `… 0,0,0,0 · 0,0,128,63 …` | `0.0` | 新 c2（FLOAT），正确 |

正常那一轮只有 `2.col`/`3.col` 不同：`2.col="test"`、`3.col="tttt"`。

`open` 之后 catalog 与三条读路径：

```
ids   = [0, 2, 3, 4]
names = ["c1", "c3", "c4", "c2"]
types = ["INTEGER", "VARCHAR", "VARCHAR", "FLOAT"]
read_blocks      -> [["1", "2", "test", "0.0"], …]   ← 与磁盘一致
SELECT *         -> [["1", "2", "test", "0.000000"], …]
SELECT c1,c3,c4  -> [["1", "2", "test"], …]
```

三条路径一致，catalog 也正确 —— **读取端没有 bug**，是块文件被写错了。
（这也说明此前怀疑的「`wanted` 列投影 / layout stride 错位」不成立。）

## 3 根因

`write_blocks_mode_with`（`crates/executor/src/blocks/engine.rs:415`）里，编码列与行宽是两个
**不同来源**：

```rust
let meta = self.storage.catalog.get_table_by_name(&table)?;   // 当前 catalog
let snapshot_ids = self.storage.catalog
    .schema_ids_at(&table, checkpoint_ts)                     // 按 bound 的列 id
    .unwrap_or_else(|_| meta.ids.clone());
let encode_meta = meta.at_ids(&snapshot_ids);
```

行来自 append buffer（`TableEntry::pending_rows`），编码器按 `encode_meta` 的列序逐列取
`row[idx]`。当 **buffer 里的行比 `encode_meta` 宽**时，多出来的那一格会让其后所有值左移：

失败那轮 `snapshot_ids = [0, 2, 3]`（c2 已 DROP），行仍是 4 个值
`[1, 2, "test", "tttt"]`（c1, c2, c3, c4 的旧快照）。编码器按序号取：

| 编码列 | id | 取自 `row[idx]` | 落盘 |
|---|---|---|---|
| 第 0 列 | 0 | `row[0] = 1` | `0.col` ✓ |
| 第 1 列 | 2 | `row[1] = 2` | `2.col` ✗（c3 的位置） |
| 第 2 列 | 3 | `row[2] = "test"` | `3.col` ✗（c4 的位置） |

`4.col`（新 c2，FLOAT）由 `add_column_files` 单独写出，所以正确——这正是「只有前两列错位、
第四列对」的原因。

### 3.1 为什么这一行仍是 4 个值

这里**不是**「INSERT 延迟落 buffer」。`execute_blocking` 是同步的（`query.rs:415-426`：
`block_on(self.execute(sql))`），而 `insert_literals_inner` 会 `.await` 到
`run_commit_bottom`（`dml.rs:803`），`publish_encoded_rows` 在这一次 await 内跑完。
用例里第一条 `INSERT` 在 `ALTER` 开始前**已经完整进入 buffer**。探针
`patch-drop … rows=0` 也印证了这一点：DROP 的 patch 跑时 buffer 里的那行已被前一次
checkpoint 取走，不是「还没来」。

真正的窗口是**一个 checkpoint 内三次读取跨越了一次 DDL**。`write_blocks_mode_with` 依次读：

| 步 | 位置 | 读到什么 |
|---|---|---|
| ① | `blocks/engine.rs:443` `get_table_by_name` | `meta`：DROP 前的 `[0,1,2,3]` |
| ② | `blocks/engine.rs:455` `schema_ids_at(checkpoint_ts)` | DROP 后的 `[0,2,3]`（id 1 已退役） |
| ③ | `blocks/engine.rs:580` `entry.pending_rows.clone()` | buffer：仍是 4 格（patch 未跑或不在此临界区） |

只要 DROP 的 commit 落在 ① 与 ② 之间、而 patch 落在 ③ 之后，这次 checkpoint 就会用
**DROP 后的列集合**去编码**DROP 前的 4 格行**：`cells[1]`（c2 的值 `2`）被写到列 id 2 的
`2.col`。探针实测正是这一组：`ts=14 live=[0,1,2,3] snapshot=[0,2,3] encode=[0,2,3]
rows=1 seq=0 [1, 2, "test", "tttt"]`。

两个加剧因素：

- `at_ids` 对找不到的 id **静默 continue**（`catalog/src/lib.rs:127-129`），编码列数少于行宽
  时不报错；
- `parse_stored_row` 会用**当前** `defaults` 把行补齐到 `defaults.len()`
  （`statements.rs:5311-5317`）。所以「按 pre-drop schema 解析、按 post-drop 补齐」也能产出
  一个宽度对得上、内容错位的行 —— 不能只靠「长度相等」判断正常。

**尚缺的一条探针**：现有探针能证明「① ② ③ 不一致」，但还没钉住是哪一次 checkpoint、
在哪个线程上撞到了这个交错。下一步应在 ① ② ③ 各打一次 `(thread_id, monotonic ts, ids)`
并让 DDL 侧打 commit ts，用一次失败运行对齐三者，确认是「checkpoint 内部三次读」而不是
「上一次 checkpoint 的快照」。在那之前，本节结论应视为**高度可能**而非已证。

### 3.2 参考为什么没有这个窗口

参考不靠「DROP 也拿块锁」——**`DropColumnsData` 根本不碰块锁**。它从
`new_txn_data_impl.cpp:1374` 开始，整个函数体只做一件事：给每个 block 写
`DropBlockColumnKey` 标记（`:1399-1404`），既不重写列缓冲、也不改 `max_ts_`。

真正起作用的性质是**两者的不对称**：

| 操作 | 块缓冲 | 块锁 |
|---|---|---|
| `AddColumnsDataInBlock`（`:1279-1372`） | **物理安装**一个新列缓冲：新建 `ColumnVector`，`AppendValue(default)` 循环补齐全部行（`:1300-1308`），写 null 位图，`SetDataSize` | 末尾取 `unique_lock`，`max_ts_ = CommitTS()`（`:1367-1368`） |
| `DropColumnsData`（`:1374-1441`） | **完全不碰**：只写 marker，块文件按列 id 命名，所以被 drop 的 `{id}.col` 原地留着 | 不取 |

于是：

1. **ADD 之后块缓冲与 catalog 必然一致** —— 新列的值是当场按 default 铺好的，不需要任何
   后续 re-encode；`max_ts_` 的更新让 checkpoint（`:1470` 的 `shared_lock` +
   `checkpoint_ts_ < min(checkpoint_ts, max_ts_)`）知道这块变过、要重刷。
2. **DROP 之后也没有 re-encode 窗口** —— 因为 DROP 不改变任何一行已有的列值，也就不存在
   「行宽与列集合不匹配」这件事。列集合变窄只影响之后新写的行。

端口的差别正在这里：它把 DROP 实现成**逐行删格**
（`patch_buffered_drop_column`，`blocks/engine.rs:2603`：`cells.remove(position)`），
于是 buffer 里同时存在「已 patch 的行」和「未 patch 的行」，而 checkpoint 用一套列集合
统一编码 —— 这个状态在参考里不存在。`bottom_gate`（被 `flush_table`
`blocks/engine.rs:1432` 与 `run_commit_bottom` `statements.rs:1009` 共用）只保证互斥，
它既不能让 ① ② ③ 三次读取同源，也不能让 patch 与 checkpoint 的 buffer 快照排序。

### 3.3 附带的第二次读取撕裂

`meta`（`get_table_by_name`）与 `schema_ids_at` 是**两次独立读取**，中间可以夹进一次 DDL。
探针抓到过 `live_ids = [0,1,2,3]`（旧）而 `snapshot_ids = [0,2,3]`（新）并存的时刻。
即使 3.1 的行宽问题修好，这两次读取也应当合并成一次快照。仅把 id 列表合并还不够：
如果 bound 在 DROP 前、当前 `meta` 在 DROP 后，`meta.at_ids([0,1,2,3])` 仍会因为 id 1
不在当前表里而静默得到 `[0,2,3]`；随后 `types`、`layouts` 和 default 也都失去了 id 1。
所以需要的是完整的 **schema at `checkpoint_ts`**，至少同时包含顺序、id、类型和 default。

## 4 已排除的修法（实测通过率）

每条都跑 12–25 轮：

| 修法 | 改动位置 | 通过率 | 为何不够 |
|---|---|---|---|
| 保持原样 | — | ~50–80%（波动大） | 基线 |
| `snapshot_ids` 与 live `meta.ids` 求交 | `write_blocks_mode_with` | ~40–80%（波动） | 行仍是 live 宽度而编码列是交集，形状本就矛盾；且没解决 3.3 |
| patch 移到 `before_wal`（conflict 后、WAL 前） | `ddl_commit_with_effects` | ~80% | 位置对（对应参考 `PrepareCommit*Data`），但只挪走了「DDL 内部窗口」，3.1 的「checkpoint 三次读取跨 DDL」不受影响 |
| patch 移进 `effects` 并持 `bottom_gate` | 同上 | **~20%（更差）** | gate 只互斥不排序；且 `add_column_files_async` 会重入同一把 gate |
| `encode_meta` 直接用 live `meta.ids` | `write_blocks_mode_with` | **0%** | bound 之后新增的列也参与编码，破坏了别的场景 |
| 加「DDL 与在途 APPEND 双向冲突（`Retry`）」 | `txn/src/conflict.rs` | 无改善 | 规则本身正确且已独立提交，但只覆盖「INSERT 后提交」；本例是 INSERT **先**提交 |

结论：这些都是在**下游**（checkpoint 侧对齐行宽，或 commit 顺序）打补丁。真正的结构性前提是
「用一套列集合编码宽度各异的缓冲行」，它在 §3.1 的三次读取跨越一次 DDL 时必然失败 —— 根因与
修法见 §5。

## 5 收口方向：`pending_rows` 改为列式 `PendingRows`（按列 id）

根本的修法不在 checkpoint 侧，而在**缓冲的表示**。现在 `TableEntry::pending_rows` 是
`Vec<(usize, Vec<Value>)>`，每一行的单元格按**该行写入时**的 schema 位置排列；而 checkpoint 用
**一套**列集合（`snapshot_ids`）去编码所有行。两个位置轴不相容，这是第 3 节错位的结构性原因，
也是第 4 节所有修法都不稳定的原因 —— 它们都还在「一套列集合套所有行」。

### 5.1 新表示

```rust
struct PendingRows {
    /// 列 id -> 每行一格，长度恒为 `len()`。
    /// **包含已退役列**：退役列的数据仍可能是「bound 早于 DROP」的
    /// checkpoint 的唯一副本（见 5.4）。
    columns: BTreeMap<u64, Vec<Value>>,
    /// 退役信息，只记元数据、不存值：
    /// `retired_at`（该批列退役的 commit ts）-> 退役的 column id。
    retired: BTreeMap<u64, Vec<u64>>,
    /// 这段缓冲行的**起始行号**。行索引 `k` 对应行 id `base_row_id + k`。
    base_row_id: RowId,
}
```

与初稿相比去掉了 `seqs`/`pos` 两个映射：它们的作用是用行 id 反查行索引，而 5.1.1 证明
这段缓冲行在 id 空间里始终是**连续区间**，`base_row_id + k` 就是那个反查，O(1) 且不需要任何
辅助结构。

三条不变量：

1. **行数恒等**：`columns` 里每个 vector 的长度都等于 `len()`；`len()` 是
   `columns.values().next().map(Vec::len).unwrap_or(0)`（空表为 0，此时无列）。
2. **列集 = live 列 ∪ 仍有旧 bound 需要的退役列**。后者由 `retired` 判定，清空时机见 5.4。
3. **格子按列 id 归属，不按位置**。写入时 `INSERT` 知道自己这一行每个值属于哪个 id，
   直接落位；不需要 `reorder_row_text`，也不需要每行另存一份 commit ts。

### 5.1.1 `base_row_id` 的前提

`base_row_id + k` 只有在「缓冲行是 `[base_row_id, base_row_id + len())` 这段连续区间」时
才等于行 id。这个性质**不是假设，而是由三条现有机制保证的**：

1. **行号在 commit-ts 顺序里分配**。`reserve_rows_at`（`dml.rs:748-767` 的 `before_wal`）
   在冲突检查之后、WAL 之前发号，而只有最小的 commit ts 能进这一段（`turn_wait`），
   所以发出的号序 == 提交序。
2. **checkpoint 把窗口裁成已提交前缀**。`committed_rows(table, checkpoint_ts)`
   （`statements.rs:1482-1499`）按 append journal 的 ts 累加，遇到 `stamp > bound` 就
   `break`，得到的是一个**前缀长度**；`blocks/engine.rs:683-698` 用它裁 `counts`。
   于是 checkpoint 写走的永远是「前缀」，buffer 剩下的是「后缀」，仍是连续区间。
3. **块内连续性有硬断言**。`blocks/engine.rs:849-855` 要求
   `seq == first_id + slot`，否则直接 `Err`。所以「checkpoint 写走了一堆不连续的行」
   这种状态不可能悄悄存在。

`checkpoint_ts` 取 `txn.current_ts()`（`admin.rs:10`），且该值单调不减，因此 2 成立的前提
（bound 之前提交的是前缀）在每次 checkpoint 上都成立。

**会破坏这个前提、必须显式处理的地方**：

| 位置 | 为什么 | 处理 |
|---|---|---|
| `blocks/engine.rs:1115` 的 `retain_buffered` | 移除的是 checkpoint 写走的前缀 | 安全，但 `base_row_id` 要随之**前移**到新的首行 |
| `engine/admin.rs:633` 的 `retain_buffered` | compaction 按**段**移除（`seq >> 32`），段 id 可能不连续（`reserve_rows_inline` 用 `(seg+1).max(floor)` 跳过已退役段） | 移除后必须**重算** `base_row_id = 剩余最小 seq`，并断言剩余仍连续；否则该表退回「分段存储」模式（见下） |
| `blocks/engine.rs:2598/2616` 的 `retain_buffered` | 只改列形状，不删行 | 安全，`base_row_id` 不变 |

如果断言失败（剩余行不连续），`PendingRows` 需要一个降级表示：保留一个
`segments: BTreeMap<u32, SegmentRows>`（每个段一段连续区间），而不是单一 `base_row_id`。
**当前所有路径都满足单区间**，所以先按单区间实现 + 断言，断言触发再引入降级表示 ——
这样任何新的不连续来源都会以一个明确错误暴露，而不是静默写错块。

### 5.2 为什么这能消除错位

列 id 在端口上**单调递增且永不复用**（`catalog/src/lib.rs:1517-1529`：`TableMeta::GetNextColumnID`
是计数器，注释明确「Deriving the id from the live columns instead handed a dropped column's id to
the next `ADD COLUMN`, so the new column inherited the old one's `{id}.col` files」）。块文件也按
`{column_id}.col` 命名，`drop_column` 只 `meta.ids.remove(idx)`、把 id 退役而不让位
（`catalog/src/lib.rs:1591-1594`）。

所以「列 id」是 buffer 与磁盘之间唯一稳定的键。错位的结构前提消失：**单元格在进入 buffer 的
那一刻就绑定了自己的 id**，此后无论 schema 怎么变，它都不会被搬到别的列上。checkpoint 不再需要
把行「按某套列集合重排」。

`write_blocks_mode_with` 里那段 `snapshot_ids` → `encode_meta` → `at_ids` 可以去掉，但
**不能整段消失**：写盘仍然必须知道「本次 checkpoint 的 bound 上有哪些列，以及这些列的完整
元数据」，理由见 5.7。准确的表述是：**编码不再按当前行位置摆放单元格**，但仍要按 bound
选择输出列、类型、layout 和 default。

### 5.3 写入语义

两个方向都要补齐，否则行数不守恒。退役 id 不是“一律丢弃”：DROP 不删除块里已有列值，
因此一个在 DROP 前已经进入 buffer 的值，仍可能是 bound 早于 DROP 的 checkpoint 的唯一副本。

| 情形 | 处理 |
|---|---|
| 写入的行有、buffer 没有该 id，且该 id **在 live schema 里** | 在 `columns` 里新建该 id 的 vector，**历史行全部补 default** |
| 写入的行有、buffer 没有该 id，且该 id **已退役** | **不变量错误**。退役列的数据只来自 DROP 时已在 buffer 的那批行；DROP 之后的新 append 不得把旧 id 送入写入路径 |
| buffer 有、写入的行没有该 id（live 列） | 该行在这些列上追加 **default** |
| buffer 有、写入的行没有该 id（退役列，仍在 `columns` 里） | 同样追加 **default**，保持行数恒等；该值不会被 bound ≥ `retired_at` 的 checkpoint 写出 |

第二行**不是**第二条冲突消解路径。当前的 commit 顺序由冲突矩阵按方向处理：

- append 第二个提交、前序已经 DROP 时，`Append → DropColumn` 为 `NoRetry`
  （`txn/src/conflict.rs:224`），语句直接拒绝；
- DROP 第二个提交、前序 append 仍在途时，`DropColumn → Append` 为 `Retry`
  （`txn/src/conflict.rs:464-469`），DDL 重跑时必须等 append 的 bottom 完成，再从完整
  buffer 迁移退役信息。

如果后续按参考把前一条改成 `Retry`，重试的 INSERT 也必须从新 schema 重新解析、重新生成
WAL 和 cells，不能把第一次执行的旧 cells 传进 `PendingRows`。所以无论这里最终选
`NoRetry` 还是 `Retry`，**DROP 之后的新 append 都不进入退役列的写入路径**。

第三种情形正是 `ADD COLUMN`：新列对所有历史行补 default，与参考的
`AddColumnsDataInBlock`（`:1300-1308` 的 `AppendValue(default)` 循环）语义一致，只是对象
从块缓冲换成了本结构。

### 5.4 `retired` 的生命周期

`retired` 只记「哪批列在哪个 ts 退役」，**不存值**；值留在 `columns` 里。这样既不复制一份
行副本，又保住了 pre-DROP checkpoint 需要的数据。

为什么不能 DROP 时直接删：

- append buffer 是这些行**唯一的未落盘副本**。参考的 `DropColumnsData` 也不删
  `{id}.col`（只写 marker，`:1399-1404`），文件由 `Cleanup` 才真正移除。
- 一个 bound `T < retired_at` 的 checkpoint 必须写出该列（在 T 时刻它是 live 的）。

**何时可以真正丢弃**：一次 bound `T >= retired_at` 的 checkpoint 完成之后。

论证：`checkpoint_ts = txn.current_ts()` 单调不减，所以之后所有 checkpoint 的 bound 都
`>= retired_at`；而 `T >= retired_at` 意味着该列在 T 上**不是 live**，`schema_at(T)`
不会选择它（5.7）。因此那次 checkpoint 写出的块不含该列，之后也不会再有人需要它的数据。
此时把该 id 从 `columns` 删除、并从 `retired` 中移除即可，内存不随反复 ADD/DROP 累积。

DROP 时要做的是：

- 把 `retired_at = commit_ts`、`columnId` 追加进 `retired`；
- `columns` 里该 id 的 vector **原地保留**（不复制、不移动）；
- 不对后续新行做任何特殊处理 —— 它们按 5.3 第四行补 default，值不会被写出。

### 5.5 删除位：不要新建第二份真相

`pending_rows` 上现有的 `retain_buffered`（`state.rs:4713`）**不是删除机制**，四处调用全都是
「腾空间或改形状」：

| 调用点 | 用途 |
|---|---|
| `blocks/engine.rs:1115` | checkpoint 写完块后，把已落盘的**已提交行**从缓冲移除（不是删除） |
| `blocks/engine.rs:2598` | ADD COLUMN 后重算 `pending_bytes`（形状变了） |
| `blocks/engine.rs:2616` | DROP COLUMN 后重算 `pending_bytes`（形状变了） |
| `engine/admin.rs:633` | compaction 后移除被合并段的缓冲副本（不是删除） |

真正的删除语义已经在墓碑上，且已是位图形态：

- 块内：`BlockVersionFile::deleted: Vec<TxnTimeStamp>`，每个 block slot 一项，`0` = live
  （`storage/src/definition/format.rs:131-136`）；
- 内存/日志：`deleted_ids` + `deleted_set` + `delete_ts`（`state.rs:4856-4870`）；
  `tombstone_set_at(read_ts)` 给出某快照可见的墓碑集合，`BlockCursor.dead` 据此过滤，
  **不从 buffer 删行**；
- `write_blocks_mode_with` 已经用它们构造 `deleted_at` 来写 `version.deleted`。

初稿提出在 `PendingRows` 上再加一个 `deleted: Vec<u64>`，这会造出**两份删除真相**，
且两者可以漂移（例如 `DELETE` 只更新了其中一份）。取舍：

- **优先**：不新增字段。行索引 `k` 直接对应行 id `base_row_id + k`（5.1.1），删除状态继续只由
  `deleted_ids`/`deleted_set` 决定；checkpoint 写 `version.deleted` 时按 `base_row_id + k` 查墓碑。
  代价是 dump 时要对每行做一次墓碑查询（可先取 `tombstone_set_at` 的快照，O(1) each）。
- **若实测那是热点**，才考虑在 `PendingRows` 里放一份**派生缓存**，且必须写明：
  它由 `note_deleted_ids` 单向写入、从不作为判据、checkpoint 后清空。

初稿没有区分这两种，读者会以为可以随便加一份。

### 5.6 代价

| 项 | 影响 |
|---|---|
| 点读 `buffered_row` | 现在返回 `Option<&Vec<Value>>`（`state.rs:4760` 的借用视图）。**列存无法再返回这个类型**：行视图要么物化一个 `Vec<Value>`（点读路径回归），要么引入 `RowView<'a>` 适配层（`&'a PendingRows` + `k`，逐列按需取）。这是本次改造最实质的成本，必须显式设计，不能默认「更紧凑所以更快」 |
| 存储 | 列式确实更紧凑（无 per-row `Vec` 头）；「省掉每行一次 Vec 分配」只在写入侧成立 |
| 改动面 | `pending_rows` 在 `crates/executor/src` 出现 **52** 次，其中 `blocks/engine.rs` **24** 次。`write_blocks_mode_with` 改完变简单；`blocks/misc.rs` 的 `BlockCursor` 是主要读者 |
| 反复 ADD/DROP | 按 5.4，退役列的数据留到「bound ≥ `retired_at` 的 checkpoint 跑完」才删，内存上界是「一次 PRE-DROP 周期内仍在缓冲的数据」；不随新行增长，也不会永久累积 |
| `base_row_id` 的失效面 | 按 5.1.1，所有现有路径都保持单区间，但新增任何一个「按段或按任意子集删 buffer 行」的路径都必须重算或断言，否则 `k = seq - base` 会静默错位 |

### 5.7 输出哪些列：`schema_ids_at` 不能直接删

`write_blocks_mode_with` 现在先删掉不在 live `meta.ids` 里的文件，再写 `block_columns`
（`blocks/engine.rs:918-941`）。如果 dump 改成「遍历 `pending.columns` 全部写出」，那么：

- 一个在 bound 之后才 ADD 的列，buffer 里可能已有它的格（该列的历史行补过 default），
  写出后会得到一个**bound 上不存在的 `{id}.col`** —— 参考按自己的 bound 不写这个文件，
  字节门禁会炸；
- 一个已退役的 id 若被无条件写出，就是 5.3 说的「复活」。判据是 `retired_at`（边界见 5.7.4）。

所以 dump 前必须有一个显式的列选择。**选 bound 上的完整 schema**（下面 5.7.1 说明它可
实现，5.7.2 说明 `keep` 必须同源，5.7.4 定死比较的边界）。

#### 5.7.1 `schema_at(T)` 可实现：重建链已经存在

不需要新写「列元数据的历史存储」。`catalog/src/layout.rs:1526-1568` 的
`table_meta_of(&TableLayout) -> TableMeta` 已经把一批 `ColumnDefValue` 完整还原成 `TableMeta`：

| 字段 | 来源 |
|---|---|
| `types` | `type_text_of(&c.column_type)`，再 `split_sparse_store_suffix` 拆掉 `WITH (SORTED)`（`:1534-1538`） |
| `sparse_stores` | `sparse_store_of(&c.column_type)`（`:1539`） |
| `defaults` | `c.default.as_ref().and_then(default_text_of)`（`:1553`） |
| `not_nulls` | `is_not_null(c)`（`:1555`） |
| `ids` | `c.column_id`（`:1559-1562`） |

`ColumnDefValue`（`catalog/src/value_format.rs:149-160`）持有 `column_type`（`DataType::Serialize()`
的 JSON）、`column_id`、`column_name`、`constraints`、`default`，所以**类型、默认值、NOT NULL、
comment 都能从列记录恢复**。`layout` 不必单独存：`layout_of(type_text, column_type)`
（`blocks/column.rs:275`）只依赖这两者。

缺的只是一个**按 ts 取**的入口：`read_layout` 现在固定读「现在」，要加
`read_layout_at(T)`，用 `column_timeline`（`catalog/src/layout.rs:1392`）已有的
「create ts 开窗 / `drop|tbl_col|` 的 value 是 DROP 的 commit ts」边界过滤。
`column_timeline` 已经在做同样的事，只是它只返回 id 列表（`:1440-1460`），
扩展成返回完整 `TableLayout` 即可。

**一个隐式耦合要注意**：`table_meta_of` 依赖 `read_columns` 的
`columns.sort_by_key(|c| c.column_id)`（`:1372`），即「按 id 升序」。live `TableMeta` 的顺序是
「创建顺序、去掉被删的位置」。因为 id 单调递增且不复用，两者**当前一致**，但这个等价关系应当
写成不变量（`schema_at` 的单测里断言 `ids` 升序）。

#### 5.7.2 `keep` 必须与 `schema_at(T)` 同源（否则写了立刻被删）

`blocks/engine.rs:918-941` 的清理逻辑是：

```rust
let mut keep = vec![VERSION_FILE];
for (i, id) in meta.ids.iter().enumerate() {   // ← live schema
    keep.push(column_file(*id));
    if is_var_buffer_type(types[i]) { keep.push(column_outline_file(*id)); }
}
for entry in read_dir(block_dir) {
    if !keep.contains(&name) { remove_file(entry.path()); }   // ← 删掉不在 keep 的
}
write_block_synced(...)   // ← 然后才写
```

注意顺序：**先删、后写**。所以如果按 `schema_at(T)` 写出了一个退役列 `{id}.col`，而 `keep`
是由 live `meta.ids` 构造的，这个文件**不在 keep 里**，会在同一次写盘里被删掉 —— 结果与
「不写」等价，§5.4 的「pre-DROP checkpoint 仍能写出该列」就落了空。

因此 `keep` 必须由**同一个** `TableSchemaAt` 构造，与 `blocks_columns` 的列集完全一致。
实现上应当让两者从同一个值派生，而不是各自查一次 catalog：

```rust
let schema_at_t: TableSchemaAt = self.schema_at(bound_ts)?;
let keep = schema_at_t.keep_list();          // {id}.col + outline
let block_columns = encode(&schema_at_t, rows);
```

#### 5.7.3 三处消费点必须共用同一个值

`write_blocks_mode_with` 里选择 layout、构造 `blocks_columns`、清理 `keep`、补 outline 文件
（`:942-955`）这四处，今天各自从 `meta` 取值。改造后必须全部消费同一个 `TableSchemaAt`；
否则「按旧 meta 取类型、按新 id 取列」仍会在相反方向重演 3.1 的撕裂。

#### 5.7.4 边界：`retired_at <= T` 即不写

`T` 是 checkpoint 事务的**可见性上界**（`begin_ts` 语义，见 5.9），而 `retired_at` 是 DROP
的 commit ts。用**闭区间**，即：

| 关系 | 该列在 T 上 | 是否写 |
|---|---|---|
| `create_ts > T` | 还没建 | 不写 |
| `create_ts <= T` 且（未退役 或 `retired_at > T`） | live | **写** |
| `retired_at <= T` | 已退役 | 不写 |

取 `<=` 而不是 `<`：`retired_at` 是 DROP 的 commit ts，而 `T` 是「已完成提交的最大 ts」。
当 `T == retired_at` 时 DROP 已经可见，该列在 T 上不是 live。用 `<` 会把 DROP 之后第一次
checkpoint 也写成「含该列」，与参考的可见性口径相反。

（参考里对应的比较是 `new_txn_data_impl.cpp:1470-1474` 的
`block_lock->checkpoint_ts_ < std::min(option.checkpoint_ts_, block_lock->max_ts_)`，
那是**块是否要重刷**的判据；列选择由 `GetColumnDefs()` 在该事务的 `begin_ts` 上完成，
两者不能混用。）

### 5.8 实施顺序

1. `catalog`：`read_layout_at(T)` + `TableSchemaAt`（5.7.1），单测断言 `ids` 升序；
2. `PendingRows` 容器（`columns` + `retired` + `base_row_id`）+ 写入语义（5.3 四种情形）+
   “post-DROP 新行不得携带退役 id”的不变量检查 + 5.1.1 的 `base_row_id` 断言；
3. `write_blocks_mode_with` 改为消费 `TableSchemaAt`，`keep` 与编码同源（5.7.2、5.7.3），
   边界按 5.7.4 的 `retired_at <= T`；
4. 同一次改动里完成 `checkpoint_ts` → `bound_ts` 改名（5.9），避免两次触碰这 67 处；
5. 删除状态沿用墓碑（5.5 的首选），不新增字段；
6. 跑 `alter_ut` 30 轮、`test_cross_engine_bytes_*` 字节门禁（写盘路径唯一可信的判据）；
7. 再把 `BlockCursor` 的点读切到 `RowView`（5.6），按 profile 决定是否需要「只取投影列」。

第 3.3 节说的「两次读取撕裂」在这个形态下**影响变小但不消失**：编码不再取
`schema_ids_at` 来摆放单元格，但 5.7 仍要取一次完整的 `schema_at(T)`，且 buffer 快照、
列选择与 schema 元数据必须在同一顺序切面取得。

### 5.9 `checkpoint_ts` 改名 `bound_ts`

现在的形参名 `checkpoint_ts` 会让人读成「checkpoint 事务自己的 ts」，但它实际是
`self.txn.current_ts()`（`admin.rs:10-12`），即**已完成提交的最大 commit ts**，用作**可见性
上界**。改名为 `bound_ts: u64`，类型不变。

四个调用点，两种来历：

| 调用方 | 传的值 | 含义 |
|---|---|---|
| `flush_storage` → `flush_blocks`（`admin.rs:802-815`） | `current_ts()` | 可见性上界（**checkpoint 事务**） |
| `dump_all_sealed_blocks`（`index_build.rs:3569`） | `current_ts()` | 同上 |
| `write_blocks_commit` / `write_blocks_import`（`blocks/engine.rs:383/392`） | 该提交自己的 `ts` | **非 checkpoint 事务**：自己就是行写入者 |
| `admin.rs:656` 的 compaction | `compact_ts` | 同上 |

后两行传进来的值**恰好等于调用方自己的 commit ts** —— 这正是「它不是 checkpoint 事务」的
标志：没有别人写的行需要按上界挡在外面，它要写的就是自己刚写的那些。这一点写进形参的文档
注释，而不是再拆一个参数：

```rust
/// The visibility bound of this write: only rows and deletes stamped at or
/// before it are written.
///
/// A **checkpoint** passes `self.checkpoint_ts()` (`current_ts()`), i.e. the
/// newest commit that has finished - rows of a transaction still in flight
/// must stay out of the files.
///
/// A **non-checkpoint** caller (a commit writing its own blocks:
/// `write_blocks_commit`, `write_blocks_import`, `compact`) passes its own
/// commit ts. When the value handed here happens to equal the caller's own
/// commit ts, that is exactly the signal that this is not a checkpoint
/// transaction: there are no foreign rows to bound out, only the caller's own.
bound_ts: u64,
```

为什么保持一个参数而不拆成 `bound_ts` + `commit_ts`：`:645` 的
`if record < bound_ts.min(ts)` 里，`ts` 已经是块变更自己的 ts（`append_ts_of` /
`deleted_at` 取到的），所以「块是否需要重刷」的两个因子本来就都已就位；`bound_ts` 只在
`min` 里承担上界那一侧。改名后这条语句的读法变成「块上次写盘的 ts 早于（可见性上界与本块
变更时刻的较小者）」，与参考 `NewTxn::CheckpointTable` 的
`record_ts < min(checkpoint_ts, block_max_ts)` 一一对应，不需要额外参数。

`:514`、`:683` 的 `u64::MAX` 分支保持原样：它是「无上界」的哨兵，改名后读作
`bound_ts == u64::MAX`，语义更直白（这一档只有 checkpoint 之外的调用才会走到，
与上面「恰好等于自己的 ts」是同一件事的另一种写法）。

改名波及 67 处，按 §5.8 的第 4 步与 `TableSchemaAt` 改造一起做，避免两次触碰同一批代码。

### 5.10 跨段：`segments` 不应切在列这一层

`base_row_id` 的前提（5.1.1）在跨段时会失效：`reserve_rows_inline` 在
`off >= SEGMENT_ROW_CAPACITY`（`statements.rs:5384`，`1024 * 8192 = 8,388,608`）时开新段，
行 id 从 `0x…_00000000` 重新起跳，`base_row_id + k` 在段边界处不连续。

一种直接的想法是把列本身按段分开：

```rust
// ✗ 不推荐
segments: BTreeMap<u32, BTreeMap<u64, Vec<Value>>>
```

**列是表级概念，不是段级**。`schema_at(T)` / `TableSchemaAt` 按表给出列集合，同一个
`column_id` 在所有段里是同一列。按段切分这一层会带来三个具体代价：

1. **写盘要拼接**：`{id}.col` 是一个文件的全部行。跨段时要把 `segments[0][id]` 与
   `segments[1][id]` 首尾相接 —— 一次 memcpy，正是列存想省掉的拷贝。
2. **列级操作变成 O(段数)**：`DROP COLUMN` 要对每个段各删一次；
   `ADD COLUMN` 要对每段各补一遍 default。
3. **`retired` 语义错位**：退役是「某个 `retired_at` 之后这**列**不再 live」
   （5.4），与段无关；按段分层后，同一列会有多份退役状态。

正确的分层是**把段放在行索引上**，列保持表级：

```rust
struct PendingRows {
    /// 表级：column id -> 全部行，长度 = `len()`。
    /// 行按「段内连续、段间跳号」的顺序排列（即行 id 升序）。
    columns: BTreeMap<u64, Vec<Value>>,
    /// 行索引空间的段边界：段 id -> (起始行索引 k, 行数)。
    /// 段数极少（8.4M 行才开一个新段），二分查找足够。
    segment_spans: BTreeMap<u32, SegmentSpan>,
    retired: BTreeMap<u64, Vec<u64>>,
}
```

`k -> 行 id` 的映射于是有两种形态：

| 情形 | 映射 | 成本 |
|---|---|---|
| 单段（绝对常态） | `base_row_id + k` | O(1)，`base_row_id` 仍然有用 |
| 跨段 | 在 `segment_spans` 上二分，再 `段起始 id + (k - 段起始 k)` | O(log 段数)，段数 ≤ 几 |

这样 `{id}.col` 天然是整列，不需要拼接；`ADD`/`DROP COLUMN` 仍是 O(1) 的列操作加一次
O(行数) 的 default 填充（与今天等价）；`retired` 也保持表级语义。

**实现建议**：`segment_spans` 作为成员加上，但**先按单段实现**
（`base_row_id` 快路径 + `debug_assert!(segment_spans.len() <= 1)`），跨段时再激活二分。
理由是 8.4M 行的缓冲在两次 checkpoint 之间几乎不可能出现，为一个罕见情形引入所有路径的
分支不划算；而断言会让「真的跨段了」以一个明确错误暴露，而不是静默算错行号。
激活条件写进单测：构造一个跨段的 buffer，断言二分给出的 id 与 `base_row_id + k` 在各自
段内一致。

### 5.11 `pending_bytes`：按列累加，不是瓶颈

先澄清它**不是性能瓶颈**。三个累加点，没有一个在每行的热路径上：

| 位置 | 频率 | 成本 |
|---|---|---|
| `buffer_rows` 的 `row_bytes` 累加（`state.rs:4689`） | 每行一次 | `size_of_val` + 遍历该行的 `Value` 找 `Str`。5000 行/批是 5000 次短循环 |
| `retain_buffered` 的全量重算（`state.rs:4717-4722`） | checkpoint / compaction / ADD / DROP 后 | O(剩余行数)，一次性 |
| `budget_bytes` 的 `sum()`（`index_build.rs:1909-1914`） | 每条**语句**一次 | O(表数)，与行数无关 |

列存改造后，第 1 项才是需要注意的地方：`row_bytes(&values)` 是「遍历一行的所有格」，而列存
里一行的格分散在各列 vector 中，逐行跨列取值会变成「每行 N 次随机访问」。

**改法：按列累加**。

```rust
/// 每个列的值体字节，running total。
column_bytes: BTreeMap<u64, usize>,
```

- `push` 一行：`for (id, v) in row { *column_bytes.entry(id) += value_body_bytes(v) }`
- `drop_column`：`column_bytes.remove(id)` —— O(1)，不需要重算
- `retain_buffered`：仍全量重算，但**按列**遍历（顺序访问，缓存友好）
- `unsealed_bytes()` = `column_bytes.values().sum()` + 结构开销

关键是 `value_body_bytes` **只看单个值**：

```rust
fn value_body_bytes(v: &Value) -> usize {
    std::mem::size_of::<Value>() + if let Value::Str(s) = v { s.len() } else { 0 }
}
```

列存反而让这件事更简单：值天然按列聚集，同一个 `Value` 变体相邻，分支预测与缓存都更好。
`pending_pos` 那一项结构开销（`state.rs:4744`）在列存下换成按 `len()` 计的少量固定开销，
不再依赖哈希表大小。

### 5.12 `append_row_cache`：保持行存，不与 `PendingRows` 合并

调查结论：**不要跟着列存改**。它是另一种东西。

| 维度 | `PendingRows` | `RowCache`（`state.rs:4985-4990`） |
|---|---|---|
| 生命周期 | 从提交到下一次 checkpoint | 从第一次全表扫描到下一次写/DDL，随时可丢 |
| 内容 | 只有未落盘的行 | **全表**的已解码行（含已落盘的） |
| 上限 | 无（受 quota 管） | `ROW_CACHE_BUDGET_BYTES = 64 MiB`，超了整体丢弃 |
| 准入条件 | 所有表 | 只有 `scalar_only(&types)` 的表（`statements.rs:3218`） |
| 读者 | 点读、窗口读、checkpoint | `fetch_rows_with_ids`（`statements.rs:3167-3192`），且只在 `read_ts() == u64::MAX` 且 `live_rows == rows.len()` 时命中 |
| 失效 | 由 checkpoint 前缀裁剪 | 任何写/DDL 调 `invalidate_row_cache`（`statements.rs:1127`） |

两个关键事实：

1. **`append_row_cache` 会重复解析一遍文本**（`dml.rs:176`：`parse_stored_row` 逐行）。它与
   `publish_encoded_rows`（`statements.rs:2079`）解析的是**同一批 `encoded` 文本**，两份解析
   确实是浪费。
2. 但它**不是列存的对象**：`RowCache` 缓存的是全表、行视图、给全表扫描用的东西。把它改成
   列存，等于把「已经落盘的行」也在内存里再存一份列副本，与 64 MiB 上限的初衷相反。

**建议**（与列存正交，可以单独做）：

- 让 `publish_encoded_rows` 把已解析的 `Vec<Value>` 顺手交给 `append_row_cache`，省掉第二次
  解析。两者在同一个 bottom 闭包内（`dml.rs:887` 的 `append_row_cache` 与 `:903` 的
  `publish_encoded_rows`），但**顺序相反**：`append_row_cache` 现在跑在
  `publish_encoded_rows` 之前，所以要改成「先 `publish` 拿到 `Vec<Value>`，再把它传给
  `append_row_cache`」——两次调用的先后需要调整，不是简单加个参数。`replay` 路径
  （`state.rs:5623`）同样要跟着调序。
- `RowCache` 保持 `Vec<(usize, Vec<Value>)>` 不变。列存改造**不触碰**它。

这样 §5.6 代价表里「两份数据」的担忧可以消掉一半：重复的是**解析**，不是长期存储。

### 5.13 compaction 与并发 append：`bound_ts` 必须保留上界

上一轮讨论 `bound_ts` 时留了一个前提没查：compaction 在提交内写块（`admin.rs:656` 的
`write_blocks_bounded_locked(table, compact_ts)`）时，是否可能出现「比它更晚提交的行混进
`pending_rows`，被它一起写进块」。查完的结论是：**不会，但靠的不是 `bound_ts`，而是
`bottom_gate`**。

`compact_ts` 的取得顺序（`admin.rs:474-480`）：

```rust
let turn = self.txn.reserve_commit_ts();
let compact_ts = turn.ts();
let compact_gate = self.bottom_gate(table);
let compact_turn = compact_gate.lock().await;   // ← 先拿 gate，再进 commit
```

它**先取表的 bottom gate，再进 commit**，并把这个 guard 一直持有到块写完之后
（`:656` 在 `commit_at_deferred_with` 的回调里，仍在 `compact_turn` 的作用域内）。而
append 的 bottom 也要拿同一把 gate（`dml.rs:801` 的 `gate.lock()`，那是 `batch_gate`；
`run_commit_bottom` 里的 `BottomWork.gates` 是 `bottom_gate`）。所以 compaction 写块期间，
同表没有 append 能把行塞进 buffer。

**因此 `bound_ts` 在 compaction 这一档传 `compact_ts` 是对的**：它既是「自己的 commit ts」，
也恰好是「此刻 buffer 里最新的行不会晚于它」的上界 —— 因为 gate 把更晚的 append 挡在外面。
这与 §5.9 说的「恰好等于自己的 ts 表示自己不是 checkpoint 事务」是同一件事的两面：

- checkpoint：**有**别的行可能晚于上界 → 必须按 `bound_ts` 裁；
- commit（compaction / import）：**没有**别的行能越过 gate → 上界取自己的 ts 即完备。

一个前提要写进实现注释：**compaction 必须在持有 bottom gate 的情况下写块**。若将来有人把
`compact_turn` 的持有范围缩短到 commit 之前，这个论证就失效，`bound_ts` 会真的漏掉并发行。
建议在 `write_blocks_bounded_locked` 的文档里加一句「调用方必须持有该表的 bottom gate」，
并在 compaction 的 gate 获取处加注释指向这一节。

## 6 已实施：bound 上的完整 schema

§5.7（A1 + A2）与 §5.9 已经落地。这一节记的是**做了什么**与**怎么验的**；§5.1–5.5 的
`PendingRows` 列存尚未开始，仍按 §5.8 的顺序待办。

### 6.1 改动

| 项 | 位置 | 内容 |
|---|---|---|
| `layout::schema_at` | `catalog/src/layout.rs` | 新增。按 ts 返回 `Vec<ColumnDefValue>`，窗口规则与 `column_timeline` 同源：create ts 开窗、`drop\|tbl_col\|` 的 value 是 DROP 的 commit ts、**闭区间**（`d <= ts` 即不 live）、同名的新 incarnation 覆盖旧的 |
| `Catalog::schema_at` | `catalog/src/lib.rs` | 新增。把上面的列记录经 `table_meta_of` 还原成完整 `TableMeta`（type / layout / default / not-null / sparse hint 全在），表不存在或 bound 前无记录时返回 `None` |
| `write_blocks_mode_with` | `blocks/engine.rs` | `encode_meta` 从 `meta.at_ids(snapshot_ids)`（当前表 + bound 的 id 列表）改为 `catalog.schema_at(table, bound_ts)`。`bound_ts == u64::MAX` 仍取 live |
| `keep` 列表 | `blocks/engine.rs` | 由 `meta.ids` 改为 `encode_meta.ids`。这是 A2：清理**先于**写盘，若 `keep` 用 live 列，本 pass 刚写出的 pre-DROP 列文件会在同一次调用里被删掉 |
| outline 文件补全 | `blocks/engine.rs` | `meta.ids[i]` → `encode_meta.ids[i]`。`types` 早已来自 `encode_meta`，用 live 的 ids 配对是相反方向的撕裂 |
| 改名 | `blocks/engine.rs`、`engine/admin.rs` | `checkpoint_ts` → `bound_ts`（28 + 1 处），并在 `write_blocks_mode_with` 的形参上写明「checkpoint 传 `current_ts()`；非 checkpoint 传自己的 commit ts，两者恰好相等即是它不是 checkpoint 事务的信号」 |
| gate 前提 | `blocks/engine.rs` | `write_blocks_bounded_locked` 的文档写明「调用方必须持有该表的 bottom gate」，指向 §5.13 |

### 6.2 验证

**单元测试**（`catalog/src/lib.rs` 的 `tests` 模块，5 个）：

- `schema_at_sees_a_column_from_its_create_ts`：create/ADD 两个 ts 的窗口；
- `a_dropped_column_is_gone_from_its_drop_ts_inclusive`：DROP 的 ts 上**已经**不可见（`<=`），
  且 pre-DROP 一侧 `types` 仍可取到被删列（这正是 A1 要证明的）；
- `a_recreated_column_has_a_new_id_and_both_incarnations_resolve`：同名重建得到新 id、
  新类型，两侧都能解析到各自那一份；
- `schema_at_returns_ids_ascending`：`table_meta_of` 依赖的排序不变量；
- `schema_at_agrees_with_schema_ids_at`：两个视图在 5 个 ts 上给出一致的 id 集合。

**回归**（`alter_ut::a_checkpoint_through_a_column_change_keeps_the_block_readable`）：

| 批次 | 轮数 | 失败 |
|---|---|---|
| 改动前（两批合计，见旧记录） | 44 | 5 |
| 改动后 | 40 | 0 |

40 轮由两批各 20 轮组成，都是 debug 构建、4 vCPU、
`cargo test -p executor --lib … -- --ignored`。用例因此从
`known-defect:` 升级为 `slow:`，并已从 `.github/workflows/rust_engine.yml` 的
`Slow reference cases` skip 名单与 `scripts/README.md` 的复现命令里移除 —— 它现在是门禁。

**全量**：`cargo test --workspace --lib` 全绿（`executor` 797 passed / 50 ignored，
`catalog` 62，`index` 137，`storage` 495，`txn` 32），`cargo clippy --workspace
--all-targets` 无告警。

### 6.3 仍然待办

- §5.1–5.5 的 `PendingRows` 列存（含 `base_row_id`、`retired`、跨段 `segment_spans`）：
  **未开始**。本次只修了「用哪个 schema 编码」，没有改缓冲的表示；§3.1 说的「buffer 里
  同一列集合套宽度各异的行」这个结构前提仍在。它现在的表现是被 `schema_at` 的 bound
  一致性挡住了，而不是消失。
- §5.10 跨段、§5.11 `pending_bytes`、§5.12 `append_row_cache`、§3.1 的探针。
