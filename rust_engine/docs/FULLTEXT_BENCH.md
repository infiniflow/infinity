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
| CREATE INDEX（DDL 本身，见 §4.11） | 3–5 ms（参考读数含它自己的 Flush） | 1.1–1.9 ms | 持平 |
| ReadJsonl（不计入） | 17–29 ms | 60–62 ms | — |
| INSERT 20000 行（batch=500） | ≈1 s（日志只到整秒） | 1.95–2.00 s | ≈2× 慢（计时口径见 §7） |
| IMPORT 20000 行 | ≈2 s | **1.876 s** | 反超 |
| Optimize（合并索引 chunk） | 360–512 µs | 299–397 µs | 同量级 |
| 查询 1 `harmful "social custom"` | 1–5 ms | **0.32 ms** | 反超 |
| 查询 2 `social custom "harmful chemical"` | 0.93–1 ms | **0.37 ms** | 反超 |
| 查询 3 `"annual American awards"`（0 行） | 0.27–0.73 ms | **0.27 ms** | 同量级 |
| 查询 4 `harmful chemical` | 0.58–0.97 ms | **0.32 ms** | 反超 |
| 查询 5 `"one of"` | 1–2 ms | **1.42 ms** | 反超/持平 |
| 查询 6（13 词） | 2–3 ms | **3.03 ms** | 同量级 |
| 查询 7（14 词） | 0.84–1 ms | **1.40 ms** | ≈1.5× 慢 |

写入从 3.2 s 降到 1.88 s（IMPORT 已快过参考，INSERT 的 2× 是 §7 说的计时口径）；查询从最初的 13–30 倍
降到「1–2 个词的查询全面反超，13/14 词的长查询在 1–1.5 倍以内」。

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

### 4.8 归并换拼接：段之间的 posting 直接首尾相接

多词/短语查询原来的最大单项是 MergeCursor::settle（一次查询里每个 term 要归并
2–3 个源：sealed segment + 内存尾段），13 词查询的 self time 里占 9.5%。

但段之间是按行区间切分的——第 1 段是 [0, N1)，第 2 段 [N1, N2)，尾段在其后——
区间互不重叠，**归并的结果就等于按区间顺序拼接**。所以 MergeCursor 新增一条
concat 路径：

- 判决条件：源数 ≥ 2、overridden 为空（没有新副本落在旧区间里）、每个源都能报出
  自己的行区间（段用 rows.first()/rows.last()，尾段用插入时维护的 min/max），
  且按区间起点排序后两两不相交；满足才把源按区间升序重排并走拼接；
- 拼接时 next 指向当前源，源走空就换下一个，**每篇 posting 只访问一个源**，
  没有比较、没有 k 路挑选；advance(target) 先整段跳过区间已经落在 target 之前的源；
- 不满足（有 UPDATE 造成的重复行、或 spill run 与尾段区间重叠）就退回原来的 k 路归并。

A/B（交替 3 轮，重复 20 次取最小；第 3 轮机器被别处占满，取前两轮）：

| 查询 | 拼接前 | 拼接后 | |
|---|---|---|---|
| 1 harmful "social custom" | 0.335 ms | 0.315 ms | −6% |
| 2 social custom "harmful chemical" | 0.398 ms | 0.365 ms | −8% |
| 3 "annual American awards" | 0.353 ms | **0.274 ms** | −22% |
| 4 harmful chemical | 0.328 ms | 0.317 ms | −3% |
| 5 "one of" | 1.95 ms | **1.42 ms** | −27% |
| 6（13 词） | 4.51 ms | **3.03 ms** | −33% |
| 7（14 词） | 1.77 ms | **1.40 ms** | −21% |

profile 里 MergeCursor::settle 从前三（9.5%）直接消失，剩下的最大项是
memmove / 分配器 / TermStream::load / TermDocCursor::ensure，都是填充块本身的工作。

回归测试：a_query_over_many_segments_matches_a_single_segment 用
set_memindex_capacity(2) 把 20 行切成约 10 个段，断言四条查询（单词、短语、两词 OR、
三词 OR）的结果与单段建索引**逐行逐序**相同。

### 4.9 试过但没走通的两条路

- **从 doc skip list 直接读块上界**：格式里确实有 block_max_tf /
  block_max_percentage，但参考在这套配置下写的是空 skip list——解析真实 .pos 的前
  200 个 term，doc_skip_len 全为 0（我们的文件与参考逐字节相同）。所以块上界只能像
  现在这样从 doc list 块现场算。
- **把长度取用改成懒解析**（Lens::Map/Segment，用时才按序号读 .len）：块上界本身就要
  遍历块内每一条 posting，把取用从 load() 挪到 block_stats() 只是换位置，实测反而慢
  3–4%（多一次 enum 匹配），已回退。

### 4.10 建索引/导入：把每 token 的分配和每 posting 的 Vec 去掉

先用两个开关把 3.09 s 的 IMPORT 拆开（现已移除）：整个导入里 **95% 是全文索引**
（跳过 index_row 后只剩 0.15 s），其中 analyze 1.33 s、postings 1.56 s。profile 也
对得上：snowball 词干 ~22%、分配器 ~22%、memcmp（词项哈希查表）~10%。

三处改动：

| 改动 | 内容 |
|---|---|
| 分词不再分配 | tokenize 原来给每个 token 建一个字节 Vec 再转 String（3M token 就是 6M 次分配）。改成 for_each_token：token 就是输入切片（只有 UNITE 字节被丢弃时才用一块复用的缓冲），tokenize 保留为收集所有权的薄包装 |
| 词干引擎复用 | Stemmer::stem 原来每个 token 都新建 Snowball 环境，现在每个算法一个 OnceLock；analyze 里小写化用 Cow（本来就是小写就借原文），词干没变就不分配 |
| 位置不再逐条分配 | 内存索引里每个 term 的位置原来是一条 posting 一个 Vec<u32>（一次导入约 600 万个分配）。现在每个 term 一个只追加的 arena，posting 只记 (start, count)；重写某行时新位置追加在尾部，旧偏移依然有效 |

实测（同机同语料）：

| 阶段 | 改前 | 改后 |
|---|---|---|
| IMPORT 20000 行 | 3.09 s | **2.45–2.46 s**（参考 2 s） |
| INSERT 20000 行（batch=500） | 3.24 s | **2.59 s** |
| CREATE INDEX（DDL 本身） | — | 1.1–1.9 ms（提交不再同步任何日志，见 §4.11） |

### 4.11 对比表里为什么只计 DDL

两边的 `Flush` 都是 checkpoint：参考的 `Flush()` 走 `PhysicalFlush`
（`src/executor/operator/physical_flush_impl.cpp:42`），即 kv store 的 `Flush()`
再加一个 `NewCheckpointTask` 并等它结束；端口 `Engine::flush` 走 `flush_storage`
（`crates/infinity-executor/src/lib.rs:21257`）。**在同一张空表上**，这两次 checkpoint
一个是 3–5 ms（参考），一个是 9.7 ms（端口），差在各自的落盘步骤：

| 端口的 checkpoint 步骤 | 耗时 | 参考有这一步吗 |
|---|---|---|
| RocksDB flush()（catalog，memtable → SST） | 3.6 ms | 有，就是 `KVStore::Flush()`（`kv_store_impl.cpp:390`） |
| RocksDB flush()（persistence 行库） | 2.9 ms | 没有对应物：参考只有一份 KV，行在块文件里 |
| flush_cpp_blocks（块镜像 + 失效索引目录清理） | 2.2 ms | 有，写数据块 |

块文件与索引 chunk 写完即 fsync（对齐 `FileWorker::WriteToFile`，见 [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) §3）。20,000 行那次的 `flush_cpp_blocks`：2.495 s → 2.514 s（块文件，+19 ms）→ 2.584 s（再加上全文 chunk 的 `.dic`/`.pos`/`.len`，+70 ms）。

catalog 的写入带 `disableWAL`（对齐 `KVStore::Put` 的默认值，`kv_store_impl.cpp:441`）；`wal.log` 只在 checkpoint 帧那一处 fsync（这是端口有意比参考多做的一步，理由见 [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) §3），commit 记录不 fsync（对齐 `ofs_.write` + `ofs_.flush()`）。
提交路径同样没有同步动作，所以 `CREATE INDEX` 的 DDL 是 1.1–1.9 ms（此前每条语句都要同步一次 catalog 日志）。
细节见 [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) §3。

这 9.7 ms 与全文索引无关（此时索引还是空的），它衡量的是各自的落盘实现，把它塞进
「建索引」那一行只会掩盖索引本身的成本——所以**对比测试里不计**：基准默认只计时
`CREATE INDEX`（1.1–1.9 ms），flush 需要显式打开（`FTBENCH_FLUSH=1`），打开后单独
打印、不并进读数。参考那边沿用它的 `BenchmarkCreateIndex`（CreateIndex + Flush 一起计），
所以表里 3–5 ms 是它 DDL 的上界。

### 4.12 重启后不再重建全文索引

原来 `TextIndex::new` 从空开始，`text_index_for` 一旦发现内存索引的文档数少于表的
行数，就把**整表重新分词建索引**：20k 文档重开目录后的第一条查询要 2.41 s。

| 改动 | 内容 |
|---|---|
| checkpoint dump 全文内存尾段 | `flush_storage` 里新增 `seal_text_indexes()`，与 secondary / IVF / HNSW / sparse 一致（参考由后台 `DumpMemIndexTask` 做）：把 `ft_{首行:016x}.{dic,pos,len,row}` 落盘并 fsync（`fulltext_segment::write_segment_index` 走 `cpp_format::write_synced`） |
| 打开时映射已有 chunk | `TextIndex::load_sealed()` 扫 `fulltext/<key>/ft_*.dic`，逐段 `SegmentIndexReader::open`（mmap），返回第一个未被覆盖的行号；`text_index_for` 只从那一行往后建索引 |
| 尾部只读增量 | `fetch_rows_with_ids_projected_from` → `read_cpp_blocks_including_deleted_from`：行号 `segment<<32 | block*8192` 之外的块整块跳过，不解码 |

实测（20k 文档，checkpoint 后重开）：第一条查询 **2.41 s → 29 ms**，第二条 0.73 ms，
命中行数与重启前一致。映射不成功的 chunk（崩溃时的半截写）会让 `load_sealed` 退回 0，
即整表重建——「一半来自磁盘、一半重建」会把同一批文档数两次。

回归测试：`a_reopened_engine_indexes_only_the_unsealed_tail`（重开后 sealed 段覆盖 12
行、内存尾 0 行）；`new_catalog_index_internal_ut::test_index` / `test_populate_index0`
里「full-text 从第 0 行重建」的断言改成 checkpoint dump 之后的形状，与其它索引一致。

### 4.13 词干 memo 与「排序分组」（写路径的常数项）

§8.1 的 profile 指出两处纯常数开销，都已改掉：

| 改动 | 内容 |
|---|---|
| 词干 memo | `Stemmer::stem_memo`：thread-local 的 `算法 -> (词 -> 词干)` 表（不跨线程加锁），命中重复词直接取结果；表满 64k 条就整表清空，不会随词表无限长。`analyze` 用它，查询侧同样受益 |
| 分组不再用 map | `MemoryIndexer::insert` 先把 token 按 `(term, position)` 排序，再走连续段把位置写进各词的 arena：省掉每文档一个 `FastMap` 和每词一个 `Vec` |

实测（同机同语料，各跑两轮）：

| 阶段 | 改前 | 改后 |
|---|---|---|
| IMPORT 20000 行 | 2.49 s | **2.26 s**（参考 ≈2 s） |
| INSERT 20000 行（batch=500） | 2.59 s | **2.37–2.39 s** |
| 查询（repeat=20 取最小） | 0.27 / 0.32 / 0.25 / 0.27 / 1.36 / 2.96 / 1.36 ms | 0.28 / 0.33 / 0.26 / 0.28 / 1.41 / 3.05 / 1.37 ms（无变化） |

产物不变：`cargo test -p infinity-executor`（707 + 集成）全绿，包括全文格式的字节门禁。

### 4.14 term 换内部 id

`MemoryIndexer` 原来用 `FastMap<String, TermPostings>`：每篇文档的每个词都要按字符串
哈希、比较一次。改成内部 id：

| 部分 | 内容 |
|---|---|
| 词典 | `term_ids: FastMap<String, u32>`（词 → id）+ `term_names: Vec<String>`（id → 词），只在词第一次出现时分配 |
| posting 表 | `Vec<TermPostings>`，按 id 下标访问，查表退化成整数索引 |
| 落盘 | **不变**：dump 时仍按词构造 `BTreeMap<String, …>`，FST 拿到的仍是词字节升序（`segment_payload` / `merged_segment`） |
| 查询 | 尾段按 `term_ids` 映射后取下标；封存段仍按词查 `.dic`；`load_sealed` 之后词典为空，未知词只会命中封存段 |

实测：IMPORT 2.26 s → **2.18 s**，INSERT 2.37 s → **2.29 s**（各 −3.5%），查询不变
（0.28/0.33/0.26/0.28/1.32/2.94/1.37 ms）。收益比预估的 180–250 ms 小，因为预估里
「每 token 一个 `String`」那 ~100 ms 属于分词器的输出形式，这次没动。

格式没变：`fulltext_format` 的字节门禁与跨引擎用例照旧通过。

### 4.15 分词器直接吐 `&str`（写路径最后一个常数项）

§4.14 之后剩下的开销是「每个 term 一个 `String`」：`tokenize_index_positions` 先建
`Vec<(String, u32)>`，`insert` 再拿它去查表。现在改成流式：

| 部分 | 内容 |
|---|---|
| 分析器 | `StandardAnalyzer::for_each_term`：边分词边把 `TermView { text: &str, … }` 交给回调；`analyze` 现在是它的薄包装（逐项 `to_string()`），两条路径不会再跑偏 |
| 词干 memo | `Stemmer::with_stem` 把缓存里的 stem **按引用**交给回调（命中时零分配）；原来的 `stem_memo` 返回 `Cow::Owned`，命中也要克隆一次 |
| 索引 | `for_each_index_term`：标准分析器走流式，analyzer/delims 两种配置仍回落 `tokenize_index_positions`；`insert` 边分词边 intern，直接产出 `Vec<(u32 id, u32 pos)>` |

实测：IMPORT 2.18 s → **1.876 s**，INSERT 2.29 s → **1.95–2.00 s**（各再降 14%）。
相对参考：IMPORT 已经反超（参考 ≈2 s），INSERT 的 2× 是 §7 说的计时口径。查询不变
（0.30/0.33/0.26/0.28/1.39/2.92/1.37 ms），checkpoint 不变（≈10 ms）。

一个副作用要记住：intern 发生在预算检查之前，被预算拒掉的文档可能留下一个空的
词典项——它的 posting 列表为空，所有消费者（dump / merged_segment / 查询）本来就跳过空列表。

### 4.16 ngram / sparsegram 也走流式

这两个分析器产出的 term 比 standard 多得多（ngram-n 每个码点一个，sparsegram 更多），
所以「每 term 一个 `String`」的浪费被放大。现在它们也有 `for_each_term`，`analyze` 是
它的薄包装；`Analyzer::for_each_term` 统一分发，词典切分/转换类（chinese、ik、japanese、
korean、rag、traditional、rank_features）返回 `false`，仍走原来的收集路径。

bench 新增 `FTBENCH_ANALYZER=ngram-3` / `sparsegram-2-3`（同一个语料建对应索引），
A/B 用临时开关 `INF_FT_NO_STREAM`（已移除）关掉流式：

| 分析器 | 流式 | 收集（改前语义） | 差 |
|---|---|---|---|
| standard | 1.86–1.90 s | 2.09 s | −9% |
| ngram-3 | 2.76–2.86 s | 3.38 s | **−18%** |
| sparsegram-2-3 | 4.50 s | 5.28 s | **−15%** |

收益随 term 数放大，与预期一致；standard 的绝对值不变（1.86 s）。词典类分析器要等
各自的切分是否能借出 `&str` 单独确认（`traditional` 走 opencc 转换，天生要新串）。

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
- 重启后的第一条查询已从 2.41 s 降到 29 ms（§4.12）；`import` 之后的查询仍未进对比表。
- 参考的 `sleep(10)` 之后才查询（后台 flush 已完成），端口是 optimize 后立刻查询。

## 7. 取舍记录

| 项 | 决定 | 理由 |
|---|---|---|
| 重启加载已有 chunk（§4.12） | **做** | 20k 文档 2.41 s → 29 ms，是重启/跨引擎交接后第一条查询的必经路径；改动局部，不碰提交路径 |
| 写路径流水线化（参考的 `AppendMemIndexTask`） | **不做** | INSERT 的 2.6 倍主要是**计时口径**：参考把全文 mem-index 追加交给后台 appender（`new_txn_index_impl.cpp:892` → `mem_index_appender_impl.cpp`），insert 返回时索引还没建完，`sleep(10)` 之后才查询；端口是同步建索引。真实吞吐看 IMPORT（1.25×）。要做就得重做提交路径的时序与错误处理（dump/checkpoint 前 `WaitForCompletion`），风险高于收益 |
| 词干 memo / 词项表（IMPORT 还差 1.25×） | 待分析 | §4.10 的 profile：词干 ~22%、分配器 ~22%、词项查表（memcmp）~10% |
| 14 词查询（1.36 ms vs 0.84–1 ms） | 待分析 | 先 profile：每单元 `ensure()`、块窗口、`score_with` 的逐单元循环 |

## 8. 分析：IMPORT 的 1.25× 与 14 词查询的 1.4×

（插桩是临时的 `INF_FT_PROF`，已移除；数字为本机 20k 文档、单次运行的累计值。）

### 8.1 IMPORT（参考 ≈2 s）

370 万 token / 2 万文档，`MemoryIndexer::insert` 及其调用链的拆分。下表是**改动前**（2.49 s）的
快照；其中的 analyze 与 posmap 两块已经按下面的 1/3 条改掉，现在是 2.26 s：

| 阶段 | 耗时 | 占比 | 内容 |
|---|---|---|---|
| analyze | 861 ms | 34% | `tokenize_index_positions`：分词、小写、Snowball 词干，**每个 token 一个 `String`** |
| seal | 877 ms | 35% | 触发预算后把内存段写成 `ft_*.{dic,pos,len,row}`：FST 构建 + posting 编码 + 逐文件 fsync |
| postings | 321 ms | 13% | `HashMap<String, TermPostings>` 查表 + arena 追加 + 升序列表插入 |
| posmap | 238 ms | 10% | 每文档一个 `FastMap<&str, Vec<u32>>`（每词一个 `Vec`，每个文档一个 map） |
| 其余 | ~180 ms | 7% | 校验、行编码、`index_rows`、`publish_encoded_rows`、提交 |

每行的固定开销可以忽略（收集索引句柄 2.1 ms、`index_delete_row` 1.9 ms、向量缓存 6.3 ms），
所以这一项不是「每行太重」，而是上面四块。按性价比排序（前两条已实现，见 §4.13）：

1. **analyze 861 ms**：3.7M 次 `String` 分配 + 词干。**已做**：词干 memo（§4.13）与流式分词
   + intern（§4.14 / §4.15），现在这条路径既不为每个 term 建 `String`，也不按字符串查表。
2. **posmap 238 ms**：**已做**——每文档的哈希 map 与每词一个 `Vec` 换成「按 (term, position) 排序后
   走连续段」，一次分配都不做（`MemoryIndexer::insert`）。
3. **seal 877 ms**：FST 与 posting 编码是实打实的活，posting 编解码已经吃过 SIMD（隔离 1.2–3.5×）。
   真正能省的是把它挪到后台（参考的 `DumpMemIndexTask`）——那就是 §7 里决定**不做**的第 2 项。
4. **postings 321 ms**：**已做**——term 换成内部 id：`term_ids: FastMap<String,u32>` + `term_names: Vec<String>`，postings 变成按 id 下标的 `Vec<TermPostings>`，查表从字符串哈希/比较变成整数下标。剩下的「每 token 一个 String」还没动（需要分词器直接吐 `&str`，另算约 100 ms）。

### 8.2 14 词查询 1.36–1.50 ms（参考 0.84–1 ms）

同一次运行里把「搜索前的准备」与 WAND 本体拆开，并读 WAND 的剪枝计数：

| 查询 | prep | WAND | 精确打分次数 | 最深块号 |
|---|---|---|---|---|
| `harmful "social custom"` | 175 µs | 57 µs | 70 | 0 |
| `"annual American awards"`（0 命中） | 287 µs | 2.7 µs | 0 | 0 |
| `"one of"` | 82 µs | 1.17 ms | 73 | 977 |
| 13 词 | 132 µs | 2.80 ms | 494 | 140 |
| 14 词 | 153 µs | 1.12 ms | 269 | 25 |

- **剪枝没问题**：0 命中的短语 0 次精确打分；`"one of"` 只精确打分 73 次却推进了 977 个块——上界够就整块跳过。
- 14 词的 1.12 ms 摊在 **25 个块 × 14 个 cursor** 上，约 45 µs/块；13 词是 140 块、约 20 µs/块。
  精确打分只有 269 次，所以钱花在「块级游标推进 + 每块重算上界」而不是打分。
- 另外约 300 µs 是 `search_text_terms` **之外**的固定开销（SQL 解析、计划、命中行的取回/投影）：
  `harmful "social custom"` 的 `execute` 是 533 µs = prep 175 + wand 57 + ≈300。

结论：14 词这 0.35 ms 的差距一半来自那 300 µs 的固定开销（对所有查询生效，受益面最大），
另一半是块循环的常数因子。块循环与参考同构（skip list 为空，两边都现场算块上界），
要继续压只能动「每块每 cursor 解 128 条 posting」这条路径，收益上限不大。

