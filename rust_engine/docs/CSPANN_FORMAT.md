# CSPANN chunk 字节级规格（`chunk_{n}.idx`，version 1）

本文是 [`CSPANN_DESIGN.md`](CSPANN_DESIGN.md) §6 的**字节级**展开：每个字段的偏移、宽度、
字节序，以及读取端会拒绝什么。**唯一实现**是
`crates/storage/src/knn_index/cspann/mod.rs`（`write` / `Layout::open` / `Chunk`），
写入端与读取端的每一个偏移都能在这个文件里查到行号；本文与代码不一致时以代码为准并改本文。

设计取向（也是它和 HNSW chunk 最大的不同）：**chunk 自包含**。HNSW 的
`chunk_{n}.idx`（`knn_format.rs`）没有 magic、没有版本、没有校验和，维度要靠目录 JSON 提供，
所以「陈旧 / 截断 / 错配」只能靠解码失败来发现；cspann chunk 自带 magic、版本、分节表和
每节 CRC32C，损坏在 open 时就报出**是哪个节**坏了。

---

## 1. 不变量

| # | 不变量 | 为什么 |
|---|---|---|
| I1 | 一个 chunk = 一棵不可变的树，创建后不修改 | 发布是「整文件 rename」（§6.3），并发读者永远看到完整的一棵 |
| I2 | `partitions` 按 `key` 升序排列，`key ∈ [0, partitions)` 且无空洞 | 树用 DFS 先序编号，父 `key` 一定小于子 `key`；搜索按 key 找 slot 才不用建索引 |
| I3 | `root.key == 0`，`root.level == height`，`leaf.level == 1`，`parent.level == child.level + 1` | 下降只看 `level`，不读 children 也能判断是否到叶 |
| I4 | 叶的成员是**行号**（引擎全局 `u64`），枝的成员是**子 partition 的 key** | 一种节点类型，下降和叶扫描走同一段代码 |
| I5 | Section F 的 `row_ids` 覆盖本 chunk 的全部行，升序、无重复 | 覆盖度判定（§9.7）靠它求差集，重复会让「未覆盖行」算错 |
| I6 | 所有整数小端；所有 `f32` 是 IEEE-754 binary32 小端 | 与引擎其它落盘格式一致，读端可直接 `from_le_bytes` |
| I7 | 每节内容一字节不改地决定 CRC32C | 校验覆盖**节内容**，不覆盖头与节表（头有 magic/版本，错配在更早一步就被拒） |

---

## 2. 文件与生命周期

| 项 | 值 |
|---|---|
| 路径 | `<index_dir>/idx_{index_id}/seg_{seg_id}/chunk_{chunk_id}.idx` |
| 谁写 | checkpoint 的 `publish_index_chunks`（`engine/admin.rs::publish_cspann_chunks` → `blocks/engine.rs::cspann_chunk`） |
| 谁读 | `index_build.rs::load_cspann_index` → `FileIndex::open` → `Layout::open`（惰性：第一次查询才装载） |
| 写入方式 | 先整份编码进内存，再 `write_synced` 落盘；发布沿用目录记录的 chunk 表 |
| `chunk_id` | 目录分配、只增不复用（`Cleanup` 删文件但不回收 id，与 HNSW/全文一致） |
| 重建频率 | cspann 走 `§9.4 rebuild` 模式：没有 tail，checkpoint 时按整列重建一棵树并发布新 chunk |

---

## 3. 总体布局

```text
offset 0     ┌──────────────────────────────────────────┐
             │ header 128 B（§4）                        │
offset 32    │  └ section table: 6 × {off u64, len u64}  │
offset 128   ├──────────────────────────────────────────┤
             │ Section A  meta        32 B × partitions  │
             │ Section B  centroids   dims × 4 B × parts │
             │ Section C  children    4 B × child total  │
             │ Section D  slots       每 partition 页对齐 │
             │ Section E  vectors     v1 为空            │
             │ Section F  row_ids     8 B × covered rows │
end - 32     ├──────────────────────────────────────────┤
             │ trailer 32 B（magic + 6 × CRC32C）        │
             └──────────────────────────────────────────┘
```

- 节顺序固定为 A..F（`Section` 枚举的判别值就是节表下标）；
- 除 Section D 按 **4096 B** 对齐外，每节起始按 **8 B** 对齐，填充字节为 0；
- 节之间允许重叠吗？不允许，但没有显式检查「不重叠」——只有 `offset >= 128 &&
  end <= trailer_start` 的越界检查。写入端按顺序追加，天然不重叠。

---

## 4. 头部（128 B）

| 偏移 | 宽度 | 字段 | 取值 / 含义 |
|---|---|---|---|
| 0 | 8 | `magic` | ASCII `"CSPANN\0\1"` |
| 8 | 2 | `version` u16 | 目前只接受 `1` |
| 10 | 2 | `header_len` u16 | 必须 `= 128`（读端强制，用来挡住未来加长头部又忘了 bump 版本） |
| 12 | 4 | `dims` u32 | 向量维数，`> 0`；与目录记录不符在 `FileIndex::open` 被拒 |
| 16 | 1 | `metric` u8 | `0 = l2`、`1 = ip`、`2 = cosine` |
| 17 | 1 | `encode` u8 | `0 = none`（预留）、`1 = rabitq`（v1 唯一发布值） |
| 18 | 1 | `rot_algo` u8 | `1 = 分块 Hadamard + 符号翻转（RHT）`；写端恒写 1 |
| 19 | 1 | 保留 | 0 |
| 20 | 8 | `seed` u64 | 旋转符号的种子；**码只在这个种子下有意义**，读端必须与目录一致 |
| 28 | 4 | `partitions` u32 | 节点数，`> 0` |
| 32 | 96 | `section table` | 6 × {`offset` u64, `len` u64}，按 A..F 顺序 |

读端在**头部**就拒掉的四种情况：文件短于 `128 + 32`、magic 不符、版本 ≠ 1、
`header_len ≠ 128`；随后是 `dims == 0`、`metric`/`encode` 越界、`partitions == 0`。

---

## 5. Section A：partition 元数据（32 B × partitions）

按 `key` 升序，`key` 即行号下标（I2）。

| 偏移 | 宽度 | 字段 | 含义 |
|---|---|---|---|
| 0 | 4 | `key` u32 | partition key，`[0, partitions)` |
| 4 | 2 | `level` u16 | `1` = 叶 |
| 6 | 2 | `count` u16 | 成员数（叶：行数；枝：子数）——**上限 65535** |
| 8 | 4 | `radius` f32 | `R_p = max‖o − c‖`，§8.3 的半径下界 |
| 12 | 4 | `centroid_off` u32 | 在 Section B 中的**字节**偏移 |
| 16 | 4 | `slots_off` u32 | 在 Section D 中的**字节**偏移（该 partition 列组的起点） |
| 20 | 4 | `children_off` u32 | 在 Section C 中的**元素**（u32 个数）偏移 |
| 24 | 2 | `children_len` u16 | 子数；叶为 0 |
| 26 | 2 | 保留 | 0 |
| 28 | 4 | 保留 | 0 |

> 注意两个 `off` 的单位不同：`centroid_off`/`slots_off` 是字节，`children_off` 是 u32 元素下标。
> `count` 与 `children_len` 是 u16 → 单叶最多 65535 行；`max_partition_size` 的默认 128 远低于此。

---

## 6. Section B / C / E / F

**B 质心**：每 partition `dims × 4` 字节（`f32` 连续存放，**原始空间**，未旋转）。1536d 时
一个质心 6 KB，256 个质心 ≈ 1.5 MB —— 这是常驻内存的「热元数据」。

**C 子 key**：每子 `u32`，同一个父的子 key 连续。只被枝节点使用。

**E 完整向量**：v1 恒为空节（`len = 0`）。`encode = none` 需要它存原始向量，
但 v1 的写入端只发布 rabitq（`write` 直接拒绝 `encode != Rabitq`），所以这个节是**为下一个
版本预留的位置**，长度字段存在但内容为空。读端不解释它。

**F 行号**：本 chunk 覆盖的每个行号 `u64`，升序（I5）。`FileIndex::row_ids()` 读的就是这一节，
覆盖度判定（§9.7）用 `live_rows − row_ids` 求「未覆盖行」。

---

## 7. Section D：slot 页（列组）

每个 partition 的成员按**列**而不是按行存放，起点 4 KB 对齐，长度**不**补齐
（下一个 partition 自己补齐到 4 KB）。页内列组顺序固定：

```text
row_ids  : u64 × count
norm     : f32 × count      ‖o − c‖（旋转后空间）
raw_norm : f32 × count      ‖o‖²（ip / cosine 用）
sum      : f32 × count      码的比特和（popcount，用作点积估计）
error    : f32 × count      ⟨u, û⟩，§8.3 的 1 − error 上界
codes    : u8  × count × ceil(dims / 8)
```

- `code_bytes(dims) = ceil(dims / 8)`（`quantize::code_bytes`）：一维一比特，`dims = 1536` → 192 B；
- 每成员总占用 `16 + ceil(dims/8)`（`quantize::record_bytes`，其中 16 = 4 个 f32）；
- 列组按 `[全部 row_ids][全部 norm][全部 raw_norm][全部 sum][全部 error][全部 codes]`
  分段，不是 `{row_id, norm, …, code}` 逐行交织——这是「页内列式」，也是 mmap 读路径可以
  只读 `codes` 一段就完成打分的依据（§8.4）。

| dims | `code_bytes` | 每成员字节 | 128 成员的页 |
|---|---|---|---|
| 128 | 16 | 32 | 4 KB |
| 768 | 96 | 112 | 14 KB |
| 1536 | 192 | 208 | 26 KB |

（`CSPANN_DESIGN.md` §6.2 的设计表写 208 B @1536d，与此一致；`row_id` 取 `u64` 而非设计的
`u32`，见 §10。）

---

## 8. trailer（32 B）

| 偏移 | 宽度 | 字段 |
|---|---|---|
| 0 | 8 | magic `"CSPEND\0\1"` |
| 8 | 4 × 6 | 每节一个 CRC32C（u32 小端），按 A..F 顺序 |

CRC 算法是 **CRC-32C（Castagnoli，多项式 0x1EDC6F41，反射）**：单测钉住
`crc32c("123456789") == 0xE306_9283`。校验范围是**节内容本身**（不含填充、不含节表）。

---

## 9. 读取端的校验清单

`Layout::open(bytes)` 依次检查（全部返回带名字的 `CspannError::Corrupt`）：

1. 总长度 ≥ `128 + 32`；
2. magic / version / `header_len`；
3. `dims > 0`、`metric ∈ {0,1,2}`、`encode ∈ {0,1}`、`partitions > 0`；
4. trailer magic 在文件末尾；
5. 每节 `offset >= 128 && offset + len <= trailer_start`（`checked_add` 防溢出）；
6. 每节 CRC32C 与 trailer 记录一致。

`FileIndex::open(path, opts)` 在此之上再比三项**目录与 chunk 的一致性**：
`metric`、`encode`、`seed`。不一致直接拒绝而不是「按 chunk 的来」——旋转种子对不上时码仍是
合法比特，但打分会全错，这类错误必须 fail-fast。维度取 chunk 头部的 `dims`（搜索前再与查询
向量长度比对，长度不符返回空结果而不是 panic）。

单测 `round_trip_preserves_the_tree_and_the_answers` / `slot_pages_are_page_aligned_and_complete`
/「翻 1 字节被 CRC 拒」/「截断、魔数错、越界被拒」/「metric、encode、seed 不符被拒」
覆盖上述每一条（`cargo test -p storage cspann`，8 passed）。

（v1 的 `encode` 只有 `rabitq` 被发布，`encode = none` 的 `nc` 分支不落盘；所以 Section E
在 v1 永远是 0 字节，读端也没有代码去解释它——将来启用时要同时 bump `version`。）

---

## 10. 与设计正文的偏差（字节层面）

| 设计 | 实际 | 原因 |
|---|---|---|
| §6.1 头部含 `partition_size` 等构建参数 | 只在目录 JSON 里 | chunk 只需要**读**得懂自己：dims/metric/encode/seed 决定怎么解，其余是构建期选项 |
| §6.2 slot 208 B @1536d 且 `row_id u32` | **216 B**（`row_id u64`） | 行号是引擎全局序，`u32` 会静默截断；4 B/行 的收益不值得一类静默错误 |
| §6.1 Section E 写原始向量 | v1 恒空 | v1 只发布 rabitq；`encode = none` 留在算法层用于「与暴力结果逐行对拍」，不落盘 |
| §6.1 版本字段 | `version = 1` + `header_len = 128` 双保险 | 未来加长头部时可以「同版本、不同长度」直接拒绝，避免误读 |

---

## 11. 复现与门禁

```bash
cd rust_engine
env -u CXX -u CC cargo test -p storage cspann --lib          # 格式层 7 个用例
env -u CXX -u CC cargo test -p index   cspann --lib          # 算法层（含树不变式 / 建树确定性）
env -u CXX -u CC cargo run -q -p cli --bin infinity -- \
    conformance ../test/sql/dql/knn/embedding/test_knn_cspann_l2.slt
```

格式层用例对「翻一个字节」是逐节验的：任取某节的第一个字节取反，`Layout::open` 必须报出
**那一节**的名字。
