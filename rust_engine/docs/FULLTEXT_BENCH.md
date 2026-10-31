# 全文基准：参考 `fulltext_benchmark` 的 Rust 移植与对比

> 参考实现：`benchmark/local_infinity/fulltext/fulltext_benchmark.cpp`
> 端口：`crates/infinity-cli/src/fulltext_bench.rs`（`infinity fulltext-bench …`）

## 1. 怎么跑

参考（C++，容器里）：

```bash
docker exec infinity_stable_build bash -c 'cd /infinity && \
  INFINITY_ROOT_PATH=/infinity/.scratch/ftbench/root \
  ./cmake-build-reldeb/benchmark/local_infinity/fulltext_benchmark --mode query'
```

- 数据目录写死 `/var/infinity`（启动时 `rm -rf` 掉），日志在 `/var/infinity/log/infinity.log`
  （`log_to_stdout: false`，stdout 上没有耗时行）。
- 语料路径是 `$(INFINITY_ROOT_PATH)/test/data/benchmark/dbpedia-entity/corpus.jsonl`；
  `INFINITY_ROOT_PATH` 默认是 `.`，所以在仓库根跑也能对上。

端口（Rust）：

```bash
cargo build --release -p infinity-cli
./target/release/infinity fulltext-bench <dir> <mode> <corpus.jsonl> [batch=500]
```

| 参数 | 含义 |
|---|---|
| `mode=insert` | 建索引 → 按 `batch` 行 `INSERT` |
| `mode=import` | 建索引 → 一次 `IMPORT`（一个段） |
| `mode=merge` | 插入 → `optimize_indexes()`（参考的 `Optimize`：合并索引 chunk，不是数据压缩） |
| `mode=query` | 插入 → optimize → 参考的五条查询 |
| `mode=query-only` | 不建表不插入，直接在已有目录上跑查询（给 profiler 用） |

环境变量：`FTBENCH_QUERY_REPEAT=n` 每条查询在进程内跑 n 次取最小；
`FTBENCH_EXTRA_QUERY=…` 追加一条查询。

前五条是参考里的查询（`fields="text"`、默认 `topn=10`、投影 `ROW_ID(), SCORE()`）；
后两条是本轮加进**两边**基准（`fulltext_benchmark.cpp` 与 `fulltext-bench`）的多词查询，
用来覆盖「一条查询里十几个 WAND 游标」这种形态：

```
harmful "social custom"
social custom "harmful chemical"
"annual American awards"
harmful chemical
"one of"

harmful chemical social custom annual American awards one of the first world war   (13 词)
national football league high school state university population family city game season team player  (14 词)
```

> 参考的日志把耗时打印成整毫秒（`2ms` / `837us`），多词查询的读数只有 1 ms 的粒度。

## 2. 数据集

参考写死 `test/data/benchmark/dbpedia-entity/corpus.jsonl`，**这个文件不在仓库里**。
本次用 `/home/zhichyu/benchmark/enwiki/enwiki-10w.csv`（10 万行，tab 分隔的
title/日期/正文）前 20000 行生成 JSONL（`{"id","title","text"}`），两个引擎读同一个文件。

> `ReadJsonl` 的耗时不可直接比：参考用 simdjson，端口是手写的字符串扫描；
> 这一项不进入查询对比。

## 3. 结果（本机，20k 文档，A/B 交替各 3 轮，取重复 20 次的最小值）

| 阶段 | 参考 C++ | 端口 Rust | 比值 |
|---|---|---|---|
| CREATE INDEX + Flush | 3–5 ms | 14.6–17.6 ms | ≈3.5× 慢 |
| ReadJsonl（不计入） | 17–29 ms | 60–62 ms | — |
| INSERT 20000 行（batch=500） | ≈1 s（日志只到整秒） | 3.24–3.28 s | ≈3× 慢 |
| IMPORT 20000 行 | ≈2 s | 3.14 s | ≈1.6× 慢 |
| Optimize（合并索引 chunk） | 360–512 µs | 299–397 µs | 同量级 |
| 查询 1 `harmful "social custom"` | 1–5 ms | **0.33–0.34 ms** | 反超 |
| 查询 2 `social custom "harmful chemical"` | 0.93–1 ms | **0.40 ms** | 反超 |
| 查询 3 `"annual American awards"`（0 行） | 0.27–0.73 ms | **0.35 ms** | 同量级 |
| 查询 4 `harmful chemical` | 0.58–0.97 ms | **0.33 ms** | 反超 |
| 查询 5 `"one of"` | 1–2 ms | **1.94 ms** | 同量级 |
| 查询 6（13 词） | 2–3 ms | **4.49–4.51 ms** | ≈1.8× 慢 |
| 查询 7（14 词） | 0.84–1 ms | **1.77 ms** | ≈2× 慢 |

写入仍是同一个量级上慢 3 倍（每批插入都要更新全文索引）。查询从最初的 13–30 倍
降到「1–2 个词的查询比参考快或持平，长查询慢不到 2 倍」。

## 4. 这一轮改了什么

### 4.1 posting list 改成迭代器（核心）

原来 `TextIndex::merge` 把每个查询词的 posting list 物化成 `Vec<(doc,tf)>`，再为
命中的每个 doc 建 `doc_len` 哈希表——与 top-k 无关，df 越大越贵。现在整条读取链
都是游标：

| 层 | 新增 | 说明 |
|---|---|---|
| storage | `SegmentIndexReader::term_doc_cursor` | 一个 term 的 doc list 按 **128 篇一个 mini-block** 从 mmap 现场解码（参考的 `DocListReader`），`advance(target)` 只解码落到的那个 block；**位置表**（`.pos` 的后半段）按 128 个 delta 一条记录随文档游标一起消费，`positions()` 只返回当前文档的那一段 |
| executor | `DocSource` / `MergeCursor` / `TermStream` | 三个来源：内存尾段切片、spill run、sealed segment 游标；`MergeCursor` k 路归并成升序、同一 row 取最新副本，并给出该文档的 tf 与 positions；`TermStream` 在它之上按 128 条维护 block 窗口 |
| wand | `Cursor` 直接驱动 `TermStream` | `block_last()` / `block_max()` 用窗口算，块对齐与参考的 `MAX_DOC_PER_RECORD` 一致；`block_index` 作为 `BlockMaxBM25Score` 的缓存键 |
| 短语 | `search_text_terms` 的通用分支 | 每个 (field, term) 一个 `MergeCursor`，按 row id 升序逐文档打分；`phrase_freq_lists` 直接用各词的 position 列表二分匹配，不再建 `HashMap<position, terms>` |

内存尾段的 per-term 列表改成**插入时保持升序去重**（重写同一行时替换而不是追加），
于是查询侧不再需要排序/去重。长表在游标构造时只解码第一个 block，剪枝掉的文档
完全不读。

### 4.2 查询路径上的另外三处浪费

| 问题 | 位置 | 影响（20k 文档） | 改法 |
|---|---|---|---|
| 索引完整性检查每次都走 `row_count`，而 `row_count` 要读每个块的 `version` 文件 | `text_index_for` | 每次查询 ≈4.5 ms | 优先用写路径维护的 `live_rows` 计数器 |
| 取命中行时把**整个 append buffer（20k 行）克隆**一遍 | `BlockCursor::new` → `pending_rows_of` | 每次查询 ≈7 ms | `Engine::pending_row` 只取那一行 |
| 取命中行时解码命中所落的**整块**（8192 行，含 text 列） | `BlockCursor::row` | 一次 6–7 ms | 传 `rows_wanted`，只解码要的那个 slot |
| WAND 命中 0 条时落到通用 posting 路径把同样的活再干一遍 | `search_text_terms` | 0 命中查询 18 ms | 直接返回空 |

合起来，`harmful chemical` 从 13 ms 降到 0.38 ms，`zzzqqqxx`（一个词都不命中）
从 18 ms 降到 0.11 ms。

### 4.3 短语接进 WAND：一个单元一个游标，块上界取各词的最小值

短语不再是 WAND 之外的一条扫描路径，而是一个和词平级的**单元游标**
（`WandUnit` 里装 `TermStream` 或 `PhraseCursor`）：

- `PhraseCursor` 持有各词的 `TermStream`（打开位置捕获），`settle()` 把子游标对齐到
  同一个文档、用 `phrase_freq_lists` 验证相邻性，只停在**短语真的出现**的文档上；
- 它的块上界是**各词块上界的最小值**（`max_tf` 与 `tf/len` 比都取 min）——短语的出现
  次数不会超过其中任何一个词，这正是参考 `BlockMaxPhraseScore` 给的界；块尾取各词
  当前块尾的最小值，于是 `skip_block_range` 可以整块跳过一个没有任何短语出现的区间；
- 短语的 `idf` 是各词 `idf` 之和、`delta` 用 `delta_phrase`（词的用 `delta_term`），
  所以 `Cursor` 现在按单元记 `idf`/`delta`，`Bm25Ranker::score_with` 逐单元套用。

两个不能混的地方：`similarity=boolean` 语义是「命中的**词**数」而不是单元数，所以
boolean 模式下短语仍按词拆成多个游标（权重取字段 boost）；`keyword` 分析器和
`rank_features` 仍走原来的通用分支。

效果：`"one of"` 30.8 → 2.5 ms；0 命中的短语（`"annual American awards"`）14.8 → 0.43 ms，
因为上界一旦低于阈值整块被跳过。`phrase_unit_prunes_blocks_without_phrase_matches`
（`wand.rs`）用 2000 篇里只有前 128 篇含短语的语料验证：只打了 ≤128 次精确分、
最深块号 0。

`merge_positions` 已经删掉；`merge` 还留给 `filter_fulltext` 的候选集路径。

### 4.4 `"one of"` 还剩的 2.5 ms 是什么

同一语料上单独查 `of`（不构成短语）要 **2.9 ms**，比 `"one of"` 更慢——说明这 2.5 ms
不是短语验证的开销，而是「走一遍 2 万篇的 posting list」本身的单价：约 130 ns/篇。
短语反而更快，是因为它的块上界取了两个词的最小值，剪得比单词更多。

参考侧同样要遍历这批文档，1–2 ms 的差别在每次推进的成本，不在剪枝：

- `PhraseDocIterator::NextShallow`（`phrase_doc_iterator_impl.cpp:139`）只解 skip list、
  按块定位（注释明确写 "decode skip_list only"），只有块的 `BlockMaxBM25Score()`
  超过阈值才调用 `GetExactPhraseMatchData` 去解位置；我们现在的 `PhraseCursor::advance`
  必须解出「短语真的出现的下一篇」才能回答 `doc()`，所以块内每篇都要验证相邻性。
- 参考的短语上界其实更**松**：它取 `min_i(f1/tf_i + f3/pct_i)`，我们取
  `1 + max_i(f1/tf_i) + max_i(f3/pct_i)`（先对各词取 min tf / min pct 再套公式），
  后者不小于前者，所以剪枝只会更多、不会更少。

拆成两个动作已经做了：`PhraseCursor::seek_block`（对应 `NextShallow`，只把子游标
移到目标文档，不解位置）和 `PhraseCursor::resolve`（对应 `Next`，验证相邻性直到找到
下一处匹配），`WandStream` 上的 `seek_block` 默认等于 `advance`（单词没有可延迟的
东西），`Cursor::seek` 改成调用它并把 `cur` 标记为待解析，`next()` 每轮开头再 `ensure`。
`phrase_seek_block_defers_the_offset_check` 用 `verified` 计数器证明 `seek_block`
确实没有解位置。

**实测这一版没有变快**（仍是 2.48 ms）：因为 WAND 的循环紧接着就要 `ensure()` 拿
`cur` 去排序/找 pivot，延迟的那次解析在下一步就发生了。

浅层循环也搬过来了（`BlockMaxWandIterator::skip_blocks`）：在上界够不到阈值时，用
各游标的 `block_last` 连续把前缀游标推到块尾并重算块上界，全程不 `ensure()`。
但**这个负载下的数字仍然不动**（`"one of"` 2.47–2.63 ms、单独 `of` 2.90 ms）：
两个词在每个块里的最大 tf 都很高，块上界 `min_i(...)` 始终高于第 10 名的分数，
一次都不会剪。参考在同样数据上也是同样的界，所以它也不会剪——它那 1–2 ms 的差别
只能来自「每篇文档走一遍」的单价，不是剪枝。

要用上浅层循环，得让块上界真正掉到阈值以下（短语里带一个稀有词、或 top-k 分数差异
大）；`phrase_unit_prunes_blocks_without_phrase_matches` 和
`top_k_in_the_first_block_never_scores_later_blocks` 就是这种构造，断言精确打分次数
被压在首块内。

### 4.5 每篇 posting 的固定开销：三处

§4.4 说了剩下的差距是「每篇文档走一遍」的单价。逐个消掉之后：

| 问题 | 改法 | `of` 的耗时 |
|---|---|---|
| 短语子游标给每条 posting 各分配一个 `Vec<u32>` 存位置 | 每条只记 `(pos_start, pos_len)`，位置写进每个块一块复用的 `pos_buf`，块边界清空 | 2.91 → 2.82 ms（短语 2.48 → 2.01） |
| SIMD-BP128 解码是**逐位**循环（每个值 `w` 次带边界检查的 32 位读取） | 按 fastpfor 的**纵向**布局整字解（先 64 位窗口，后改成 portable SIMD，见 §4.6） | 2.82 → 2.78 ms |
| 文档长度用 `HashMap<u64, u32>` 查，默认 SipHash；每个 posting 要查两次（块统计 + 打分） | `DocLenMap` + 恒等哈希（key 本身就是行号） | 2.78 → 2.64 ms |

参考侧是块级 WAND + 跳表流式推进（命中 k 个就停），只投影 `row_id`/score 两列，
所以耗时基本与语料规模无关（20k 文档仍在 1 ms 量级）。

### 4.6 posting 编解码：真正的 SIMD（nightly）

参考的 posting 编解码走 `fastpfor`（`src/storage/common/fastpfor_impl.cpp` 的
`SIMDBinaryPacking`），是一组 SSE2 intrinsics。端口此前的**格式**与它逐字节一致
（`encode_u32_block` 写的就是同一套纵向位平面布局），但指令层面一直是标量。

现在 `unpack_mini_block` / `pack_mini_block` 都用 `std::simd::u32x4`：四个 lane 的
数据逐字交错，所以一个 128 位寄存器正好装「四个 lane 的同一个字」，每个 4 值组只需
一次移位对 + 一次 OR + 一次 store（解码），编码是对称的
`words[i] |= v << off; words[i + 1] |= v >> (32 - off)`。

**为什么是 nightly**：`std::arch` 的 intrinsics 早已 stable，但全是 `unsafe`，而这个
crate 是 `#![deny(unsafe_code)]`（只对 mmap / FFI 逐点 `allow`）；`std::simd` 是安全
API，但仍是 nightly-only。因此 `rust-toolchain.toml` 固定到
`nightly-2026-10-29`（仍按日期 pin，保持「同一台机器/CI 用同一个编译器」）。

**代码生成**（`rustc --emit asm` 核对，不是"看起来像 SIMD"）：`movdqu` / `psrld` /
`pslld` / `por` / `pandn`。

**字节不变**：编码仍与 C++ 逐字节相同——既有固定向量
`u32_block_matches_the_cpp_bytes`（1/2/7/9/14/17 位 + VB 尾）之外，新增
`packed_blocks_match_the_bit_at_a_time_packer`（1..=32 全宽度，与逐位写法的输出比对）
和 `simd_decoder_matches_the_lane_by_lane_decoder`（1..=32 全宽度，与旧解码比对）。

**收益**：codec 本身 1.2–3.5×（`w=16` 典型 3.1×，解码约 0.48 ns/值）；但**端到端没有
可测变化**：20k 语料上 `of` 2.64 → 2.65 ms，其余四条仍是 0.37 / 0.46 / 0.32 / 0.37 /
1.79–1.80 ms，INSERT 3.17–3.21 s。原因是 doc list 解码只占 `of` 的百分之几——
那 2.6 ms 花在「每篇 posting 的游标推进 + 打分」，不在 codec。

### 4.7 游标层重建：序号贯通、去 trait object、块不再物件化

§4.4 的结论是「长查询慢在每篇 posting 的框架开销」。三处一起改：

| 改动 | 内容 |
|---|---|
| 段内序号贯通 | `TermDocCursor` 解码时把每篇的段内序号和 row id 一起存下，`Source::len()` 直接读 `.len[ordinal]`；`TextIndex::doc_len_of`（row → 二分的 `ordinal` → len）从打分路径上删掉，`WandField.doc_len` 那个 `Box<dyn Fn>` 也不再存在 |
| 去 trait object | `DocSource` trait 换成 `Source` enum（Mem / Owned / Segment），`WandStream` 也从 trait 换成 enum（Term / Phrase）。只有 1 个源且没有过期副本要过滤时 `MergeCursor` 走 `single` 直达，不跑 k 路归并 |
| 块不再物件化 | `TermStream` 的块改成三列 `docs/tfs/lens`（不再有 `Posting` 结构体），位置只在短语子游标里进 per-block arena；块上界（max tf / max tf÷len）改成第一次 `block_stats()` 时才从这三列算，被整块跳过的块完全不付 |

A/B 交替（同一份 20k 语料，各 3 轮，重复 20 次取最小）：

| 查询 | 改动前 | 改动后 | |
|---|---|---|---|
| 1 `harmful "social custom"` | 0.34 ms | 0.33 ms | −2% |
| 2 `social custom "harmful chemical"` | 0.45 ms | 0.40 ms | −11% |
| 3 `"annual American awards"` | 0.31 ms | 0.35 ms | +13% |
| 4 `harmful chemical` | 0.37 ms | 0.33 ms | −9% |
| 5 `"one of"` | 1.78 ms | 1.94 ms | +9% |
| 6（13 词） | 6.20 ms | **4.50 ms** | **−28%** |
| 7（14 词） | 2.17 ms | **1.77 ms** | **−19%** |

长查询（本轮的目标）好了 20–30%；两条**短语**查询反而慢了 9–13%。原因：短语子游标
现在在 `load()` 里给每条 posting 都取一次长度，而旧代码只在真要块上界时才对块内
128 条取一次——两个高频词的短语要扫 4 万条 posting，多出来的取用把别处的收益吃掉了
（把它改成常量做消融会让剪枝变松，反而更慢，所以这个数字是干净 A/B 对出来的）。
下一步是把长度也做成按序号的懒取：`TermStream` 存 ordinal 加
`&SegmentIndexReader` / `&DocLenMap`，只在 `len()` 和 `block_stats()` 时才读。

## 5. 顺带修掉的一个格式缺陷

`encode_fulltext_chunk`（`crates/infinity-executor/src/cpp_blocks.rs`）原来对
**每个位置**调用一次 `end_document`，而参考的 `GeneratePosting`
（`column_inverter_impl.cpp:253-257`）是**每个文档**一次：位置的累加走
`AddPosition`。后果是：

- `df` 变成"出现的次数"而不是"文档数"，`total_tf` 成了每项 1；
- doc list 里同一个 doc id 重复出现，skip list 的严格递增断言在
  `MAX_DOC_PER_RECORD`（128）个条目后触发——**500 行一批插入 + flush 直接 panic**
  （`AddItem failed, last_doc_id: 286`）。

既有的门禁没抓到它，是因为 Gate A 的全文用例都在 128 篇以内、且走内存索引；
字节门禁的全文用例每篇文档的 token 都不重复。现在按文档分组后
`df`/`total_tf` 与参考一致，回归测试见
`crates/infinity-executor/tests/fulltext_format.rs::repeated_term_counts_one_document_and_every_occurrence`。

## 6. 这一轮没覆盖的

- 只跑了 20k 文档（dbpedia 语料缺失）。插入/查询的量级关系已经稳定，但绝对值会随语料变。
- 短语查询（§4.3）已接进 WAND 并有块级剪枝；只剩「两个词都很高频」的短语约比参考慢 2 倍（上界被高频词撑松，剪不掉多少）。
- `import` 之后的查询、以及重启后的查询没进对比表：端口重开目录时会**从行数据重建
  全文索引**（20k 文档约 3.1 s），而不是加载落盘的 `.dic`/`.pos`，这是另一条待查的路径。
- 参考的 `sleep(10)` 之后才查询（后台 flush 已完成），端口是 optimize 后立刻查询。
