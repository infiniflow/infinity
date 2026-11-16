# 全文基准：参考 `fulltext_benchmark` 的 Rust 移植与对比

> 参考实现：`benchmark/local_infinity/fulltext/fulltext_benchmark.cpp`
> 端口：`crates/cli/src/fulltext_bench.rs`（`infinity fulltext-bench …`）

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
- `--mode query-only` 复用上一次留下的目录，不建表不插入；`--terms N` / `--topn N` /
  `--algo auto|heap_bmw|bmw|batch|false` 把长查询和 `block_max` 透传给 `MATCH`
  （`false` 是无剪枝的 naive 基线，`bmw` 是旧的排序实现）。参考的行数上限是 8192，
  `--insert-batch` 超过它插入会被拒绝。

端口（Rust）：

```bash
cargo build --release -p cli
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
`FTBENCH_EXTRA_QUERY=…` 追加一条查询；`FTBENCH_TOPN` 改 `topn`（默认 10）；
`FTBENCH_BLOCK_MAX` 填参考的 `block_max` 值（`auto`/`heap_bmw`/`bmw`/`batch`/`false`，
`false` 就是无剪枝的 naive 基线）。`INFINITY_WAND_STATS=1` 让 WAND 往 stderr 打一行
`[WAND] scored=<精确打分次数> cursors=<游标数> df_total=<候选文档总数>`，用来看剪枝率。

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

**跨引擎、走服务端的那一组数字**（`INDEX fulltext build|query`）来自 `tools/bench_index_types.py`，
由 `tools/run_index_bench.sh index-types` 起停两端引擎；与上面的 `fulltext-bench`（进程内、参考
`fulltext_benchmark` 的移植）是两条不同的路径，数字不要混用。全索引类型的入口见
[`INDEX_PERF.md`](INDEX_PERF.md)。

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

## 3.1 1M 文档：索引构建与三路对比（C++ vs Rust）

语料由 `cargo run --release --bin gen-corpus -- --corpus <enwiki.tsv> --out <jsonl>` 生成：词表取
enwiki 10 万篇的词频前 20k（分词规则与参考 `LoadVocabulary` 一致），每篇按词频（zipf 形状）采样 40 词，
共 100 万篇，生成 2.8 s。两边读同一份 JSONL；查询词由参考自己的 `LoadVocabulary` 词表采样（固定种子），
先跑参考，再把它日志里的查询文本原样喂给端口（`FTBENCH_EXTRA_QUERY`），所以两边打的是同一批词。
`topn=10`，每条查询进程内 3 次取最小、3 个进程再取最小；两边都只测查询路径（参考这轮加了
`--mode query-only`，与端口的 `query-only` 对应）。

| 阶段 | 参考 C++ | 端口 Rust | 比值 |
|---|---|---|---|
| INSERT 100 万行 | 31 s（`--insert-batch 8192`，参考的行数上限） | 20.4 s（batch 100000） | 端口快 1.5× |
| 索引体积 | 1.2 GB | 1.2 GB（31 个 chunk） | 持平 |
| CREATE INDEX（DDL） | 33 ms | 12 ms | 端口快 2.8× |
| 查询 10 词 | 4.0 ms | **3.3 ms** | 0.8× |
| 查询 50 词 | 16 ms | **18.6 ms** | 1.2× |
| 查询 100 词 | 33 ms | **37.2 ms** | 1.1× |
| 查询 200 词 | 79 ms | **82.8 ms** | 1.0× |

三路对比（同一批查询，单位 ms，三条查询取中位数；「旧 sort」是端口按值重排整个游标数组的实现）：

| 查询词数 | C++ naive | C++ heap | C++ 旧 sort | Rust naive | Rust heap | Rust 旧 sort |
|---|---|---|---|---|---|---|
| 10 | 12.0 | 4.0 | 4.0 | 6.1 | 3.3 | 5.6 |
| 50 | 47 | 16 | 42 | 19.8 | 18.6 | 125.5 |
| 100 | 113 | 33 | 152 | 46.5 | 37.2 | 443.1 |
| 200 | 317 | 79 | 560 | 102.7 | 82.8 | 1290.7 |

- 堆对旧排序版：参考 1.0–7.1×，端口 1.1–12.7×（旧版排序要搬 `Cursor` 整个结构体，词一多就爆）。
- 端口的 naive 比参考快 1.2–1.9×（同一条 posting 的单价更低），堆已经追到 0.8–1.2×（10 词反超）。
- 端口的堆只比自己的 naive 快 1.2–1.3×，参考是 3.5×。剪枝本身是有效的：
  `INFINITY_WAND_STATS=1` 打印的 `[WAND] scored=288 cursors=100 df_total=277838` 说明 100 词只精确
  打分 288 次（候选 24.4 万），200 词 448 次（候选 50.4 万）；差的是控制结构上的轮数，见 §3.2。

### 3.2 端口的 heap 为什么还是慢 1.3–1.4×

两边都加了计数器（参考在 `Finish()` 打印 `BlockMaxWandStats`，端口在 `drive()` 结束打印），
同一条 100 词查询、`INFINITY_WAND_STATS=1`：

| 计数器 | 参考 C++ | 端口 Rust |
|---|---|---|
| 精确打分 | 394 | 288 |
| pivot 轮数 | 97,021 | 166,735 |
| 其中「前缀头不在 pivot 文档上、整体前移后重来」 | 无此分支 | 153,291（92%） |
| 子游标推进 | 256,096 | 449,481 |
| 堆操作 | 532,911 | 898,863 |
| 解码 posting 数 | — | 277,838（正好等于 df 总和） |

排除掉两个直觉上的怀疑：

- **不是解码量**：端口 2223 个块、解了 277,838 条 posting，正好等于 100 个词的 df 之和——
  按 128 条一块整块解并不浪费，每个 posting 只解一次。
- **不是叶子层跳过**：参考那边本轮加了 `INFINITY_WAND_PLAIN_SEEK=1` 切到
  `NextNoSkip`（只定位、不在叶子里按本地阈值跳文档），计数一条不差、耗时 30 → 31 ms，
  说明这条叶子跳过在这个负载上没有作用。

差的是端口还留着**排序版**的一条分支：前缀的第一个游标不在 pivot 文档上时，把整个前缀
`seek` 到 pivot 文档再 `continue` 重来一轮（`wand.rs` 里 `prefix[0].cur != Some(doc)` 那一支）。
heap 版参考没有这一步：它在候选文档上直接用「块覆盖候选就计块上界、块尾在候选之前就记 0」
求和，够阈值就把前缀推到候选并打分，不够就前进一个文档或跳块。端口 92% 的轮次花在这条
重来分支上，所以同一个查询多做 0.7 倍轮数、0.75 倍子游标推进、0.68 倍堆操作——单次操作
其实比参考便宜，才没有更慢。

已按参考的写法移植（`sum_at_doc` 判定 + 参考的三段式块跳过 `SkipUncompetitiveRange`）：
候选文档上的上界改成「块覆盖候选就计块上界、块尾在候选之前记 0」，够阈值才把前缀推到候选打分，
不够就跳到下一个能竞争的块。同一条 100 词查询：

| 计数器 | 参考 C++ | 端口（移植前） | 端口（移植后） |
|---|---|---|---|
| pivot 轮数 | 97,021 | 166,735 | 98,852 |
| 其中重来轮次 | 无 | 153,291 | 3,694 |
| 堆操作 | 532,911 | 898,863 | 540,310 |
| 子游标推进 | 256,096 | 449,481 | 526,039 |
| 耗时（3 条查询中位数） | 33 ms | 44.9 ms | 45.5 ms |

轮数和堆操作已经和参考持平，墙钟却没有变化：被省掉的 15 万次重来轮次本来就很便宜，而新分支
为了判定候选点上的上界要多解一部分游标——子游标推进从 44.9 万涨到 52.6 万，仍是参考的 2 倍。
剩下的差距在游标层：端口每次推进都要过 `MergeCursor`（内存尾 + spill run + 封存段多源归并），
并把 128 条 posting 整块解成三列；参考只是在单一 doc list 上走跳表。profile 分布很平
（`next` 26%、堆 pop 16%、`TermStream::ensure` 17%、`TermStream::load` 16%、
`MergeCursor` 17%），没有单点可以再砍一刀。

结果不变：四个查询长度、12 条查询的 `ROW_ID()/SCORE()` 与移植前逐字节相同。

再往下一层是取 posting 的调用次数：`TermStream::load` 每读一条 posting 要调
`merge.doc()` + `tf()` + `len()`，每个都要重新匹配一次
`Source`/`MergeCursor` 的枚举分支，段游标的 `tf()`/`len()` 还会各自重跑一次
`ensure()`。现在合并成一次 `MergeCursor::posting()`（单源直接转发给那唯一的
`Source`，段游标只 `ensure` 一次、只读一次 `.len`）。profile 里
`TermStream::load` 从 16% 掉到 8%、`MergeCursor::doc/advance` 直接掉出前 12；
端到端 100 词 41.1 → 39.0 ms、200 词 86.6 → 84.8 ms（最小值），中位数变化在噪声内。

取结果行一度看着像最大的一块（profile 里 `decode_rows_of_values` + UTF-8 约 20%），实测并不是：

| topn | 100 词查询 |
|---|---|
| 1 | 39.3 ms |
| 10 | 39.8 ms |
| 100 | 40.9 ms |

取 10 行只比取 1 行多 0.5 ms（约 1.3%），说明这条路径**没有整块解码**——`BlockCursor::rows` 把
row id 折算成槽位列表传进 `decode_rows_of_values` 的 `rows_wanted`，循环只走这些槽位，块里
8192 行不会碰。那 20% 是 `String::from_utf8_lossy`：它对每个文本单元都按 UTF-8 chunk 迭代，
即使字节本来就合法。现在先用 `str::from_utf8` 走快路径、只在真非法时才退回 lossy，
profile 里这块消失（墙钟在噪声内，省下的量被重新归到游标上）。

### 3.3 多词 WAND 剩下的 0.8–1.2× 在哪

把 §3.2 的计数换算成单价（100 词、1M 文档）：

| | 参考 C++ | 端口 Rust |
|---|---|---|
| pivot 轮数 | 97,021 | 98,852 |
| 每轮成本 | 340 ns | 376 ns |
| 子游标推进 | 256,096 | 约 260,000（`child_advances` 把 resolve 和 push_back 各记一次） |
| 每次推进成本 | 129 ns | 143 ns |
| 精确打分 | 394 | 288 |
| 解码的 posting | ≈推进次数（逐条走） | 277,838（正好每篇 posting 一次） |

轮数、堆操作、推进次数、解码量都在 3% 以内——**算法层面没有差的了**，差的是每次推进的常数：
端口每次推进要跨四层 `WandStream` → `TermStream` → `MergeCursor`（`single`/`concat` 分支 +
range 判断）→ `Source` → `TermDocCursor`（`ensure` + 块内扫描），每层一次枚举匹配；参考是一次虚调用。

游标层还剩两条开销（第 1 条本轮已吃掉）：

1. ~~**每条 posting 被物化两次**~~（已修）：`TermDocCursor::fill` 把 128 条解进 `rows/tfs/ordinals`，
   `TermStream::load` 再逐条通过 `posting()` 复制进自己的 `docs/tfs/lens`。现在 `load` 调
   `MergeCursor::extend_block`，段游标把已经解好的三列直接 `extend_from_slice` 过去（每个源一次调用，
   不是每条 posting 一次），长度用已有的 ordinal 读 `.len`，位置捕获的短语路径仍走逐条。
   100 词 43.5 → **37.2 ms**、200 词 98.6 → **82.8 ms**、10 词 4.6 → **3.3 ms**（比参考还快）。
2. **块上界每块重算**（量过，不值得动）：`block_stats` 每块扫 128 条 tf + 每块一次浮点除法
   （判最大比值改用交叉相乘，一次除法算出来，值完全不变）。整条路径的上限是 2223 块 × 128 条 ≈ 28 万次
   比较，实测关掉也省不到 2%：把倍率放宽 1.5 倍（等价于上界变松）只慢 3%（100 词 37.2 → 38.5 ms），
   说明这份负载对块上界的松紧不敏感；而走 skip list 的 `block_max_tf` 反而**更松**——这份语料每个
   chunk 只有 ~89 条 posting、不到一个 128 的块，合并块经常跨两个 chunk，取两个块的最大值等于把范围放大
   ~40%，按上面的敏感度算会慢 1–2%，抵掉省下的那 1.6%。

不是杠杆的东西：**跳表 seek**。看起来端口`TermDocCursor::advance` 是线性扫描（没有用 skip list 跳块），
但这个负载上参考自己 97k 轮只做了 2,247 次块级 seek——两个实现都在逐条推进，跳表省不到东西；
它只对「跳很远」的稀疏词有用，不在多词查询的路径上。

> **这条结论的适用范围**：只对「advance 跨度 ≤ 1~2 个块」的 WAND 多词负载成立，不能推广。
> 大跨度场景（高选择性 filter 下推、范围/前缀过滤、稀疏词与高频词求交、大 df 词的尾部跳过）
> 是数量级的差距：每跳过一个 128-posting 块就少解一次三列 SIMD 块。
>
> 因此 `TermDocCursor::advance` **已接上 doc skip list**（`SkipListReader::SkipTo`）：
> 块内仍是线性步进（同参考 `PostingIterator::SkipTo` 的短路），跨块直接落到目标块。
> `advance_hops_blocks_with_the_skip_list` 断言 600 篇的 term 跳到最后一篇只解 2 个块（而非 5 个），
> `advance_lands_where_a_linear_walk_does` 对 700 个 target 逐一核对落点与块数。
>
> WAND 走的是 `TermStream::advance`（`Cursor::seek` → `WandStream::seek_block`），它原来逐块
> `load`，根本不经过存储层的 `advance`，所以只改存储层对 WAND 没有收益。现在
> `TermStream::advance` 在 `target > 块尾` 时改为 `merge.advance(target)` 再重填一块
> （参考的 `TermDocIterator::NextShallow` → `PostingIterator::SkipTo`），块内仍线性。
> `a_shallow_seek_on_a_sealed_segment_hops_blocks` 断言 5000 篇的 sealed 段上跳到 4900 只重填一次。
> **注意**：seek 后块起点不再是 128 对齐（是第一个 ≥ target 的 posting），块上界仍只覆盖自己那
> 128 条，剪枝 sound，但覆盖范围与参考的固定 128 文档块不同。
>
> **filter 下推**：`WHERE` / `MATCH TEXT (…, WHERE …)` / `filter=…` 过去是"全表求值 → 失败行塞进
> `dead` 集合 → 打分后丢弃"，每个被拒的候选仍要解码块上界并打分一次。现在求值结果是一次性的
> 升序行列表（`DocFilter`），WAND 在 pivot 落在被拒行时把**所有**低于下一个允许行的游标 seek 过去
> （`skip_to`），streaming 路径对每个 merge cursor `advance`；`next()` 的 target 也先 `lift` 到
> 下一个允许行。`filter_pushdown_scores_only_admitted_rows` 断言 2000 篇里每 20 篇放行时打分次数
> ≤ 100（无下推是 2000），`filter_pushdown_matches_a_post_filtered_scan` 逐条比对"带下推"与
> "全量跑完再过滤"的文档序与分数。
>
> 仍未接的是「读 `block_max_tf` 当块上界」那一半，理由见 §4.9（合并流的块边界与段内块不对齐）。

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
  block_max_percentage。当时的观察是「参考在这套配置下写的是空 skip list（前 200 个 term 的
  doc_skip_len 全为 0）」，但这条与现有证据冲突、需复核：真实参考文件
  `ft_0000000000000000.pos`（8192 篇）的 doc region 夹具里含 **64 个 skip item**，
  而编码器在 `Dump` 前会 flush 掉不满一块的尾部，2 篇的 term 也会写出 1 个 item。
  最终**改成读存下来的上界**：sealed 段与 spill run 走 `GetBlockMaxInfo`
  （`SkipListReader` 的 `block_max_tf` / `block_max_percentage`），内存尾按自己那 128 条现场算；
  合并流取「最小块尾 + 覆盖它的源里最大 tf/pct」（参考 `MultiPostingDecoder`
  的 `GetBlockLastDocID` / `GetBlockMaxInfo`）。只有不满一个块、没有 skip item 的列表才回退到
  现场算。WAND 的上界直接用存下来的 `u16` percentage，与参考逐位一致。
  注意：这与**跳表 seek** 是两条独立收益——seek 只看 offset，不受块边界对齐影响，也已接上（§3.3）。
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
（`crates/executor/src/engine/statements.rs`）。**在同一张空表上**，这两次 checkpoint
一个是 3–5 ms（参考），一个是 9.7 ms（端口），差在各自的落盘步骤：

| 端口的 checkpoint 步骤 | 耗时 | 参考有这一步吗 |
|---|---|---|
| RocksDB flush()（catalog，memtable → SST） | 3.6 ms | 有，就是 `KVStore::Flush()`（`kv_store_impl.cpp:390`） |
| RocksDB flush()（persistence 行库） | 2.9 ms | 没有对应物：参考只有一份 KV，行在块文件里 |
| flush_blocks（块镜像 + 失效索引目录清理） | 2.2 ms | 有，写数据块 |

块文件与索引 chunk 写完即 fsync（对齐 `FileWorker::WriteToFile`，见 [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) §3）。20,000 行那次的 `flush_blocks`：2.495 s → 2.514 s（块文件，+19 ms）→ 2.584 s（再加上全文 chunk 的 `.dic`/`.pos`/`.len`，+70 ms）。

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
| 段由 `DumpMemIndex` 事务落盘 | `dump_index_transaction` → `seal_index_tail`（`index_build.rs`）：把 `ft_{首行:016x}.{dic,pos,len,row}` 落盘并 fsync（`write_segment_index_streaming` 走 `storage::definition::format::write_synced`）。**checkpoint（`flush`/`flush_storage`）不 dump 内存索引**，这一点照参考的 `NewTxn::CheckpointTable`（只刷块）；尾段在下次打开时从 chunk gap 之后重新分词建索引 |
| 容量/配额也会触发 | `should_seal`（`memindex_capacity` 行数或 `TEXT_INDEX_BUDGET_BYTES` 字节）在写路径上直接封存；超过统一 memindex 配额时排 `DumpMemIndexTask` |
| 打开时映射已有 chunk | `TextIndex::load_sealed()` 扫 `fulltext/<key>/ft_*.dic`，逐段 `SegmentIndexReader::open`（mmap），返回第一个未被覆盖的行号；`text_index_for` 只从那一行往后建索引 |
| 尾部只读增量 | `fetch_rows_with_ids_projected_from` → `read_blocks_including_deleted_from`：行号 `segment<<32 | block*8192` 之外的块整块跳过，不解码 |

实测（20k 文档，重开）：第一条查询 **2.41 s → 29 ms**，第二条 0.73 ms，命中行数与重启前
一致。这个读数只对「大部分行已封存、只有尾部要重建」的形状成立——封存由容量/配额触发，
**不是 checkpoint 做的**（当时记录的切分比例没有留存）。尾部越大第一条查询越贵，整表都
没封存时就是 2.41 s 那一档。
映射不成功的 chunk（崩溃时的半截写）会让 `load_sealed` 退回 0，即整表重建——「一半来自
磁盘、一半重建」会把同一批文档数两次。

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

产物不变：`cargo test -p executor`（707 + 集成）全绿，包括全文格式的字节门禁。

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

### 4.17 多词查询：子游标走文档序最小堆

10 个词以上的一条查询会有同样多的 WAND 游标，参考在每轮 pivot 之后都要把子游标数组重新排一遍。
端口把「排好序的数组」换成 `BinaryHeap<Reverse<(doc, ord)>>`（`wand.rs` 的 `heap`），加一个
`prefix` 缓冲保存本轮弹出的前缀：

- `FindPivotOptimized` 变成「从堆里弹出最短前缀，直到上界和 ≥ 阈值」，每次弹出 O(log n)，
  且只碰真正移动过的游标；旧写法每轮要 `ensure()` 全部游标再整体 `sort_by_key`。
- 阈值口径不变（第 k 名分数，`<` 而非 `<=`，平分再看 doc id）；`total_upper` 增量维护，
  游标耗尽（`ensure()` 拿到 `None`）时从总上界里减掉自己那一条。
- `skip_blocks` 里「块尾不越过下一篇」取 `heap.peek()`；跳完把前缀游标 `push_back` 回堆，
  `push_back` 顺带解析这次 `seek` 留下的待定文档。
- 打分仍按创建序（field-major，再按 term）累加，`score_prefix` 对 `prefix` 按 `ord` 就地排序，
  所以分数不随堆的次序变化，与扫描路径逐位一致。

20k 语料、`topn=10`、进程内 5 次取最小、4 个进程再取最小：

| 查询词数 | 每轮整表排序 | 文档序最小堆 | |
|---|---|---|---|
| 13 / 14 | 3.23 / 1.76 ms | 3.38 / 1.69 ms | 差异在噪声内 |
| 25 | 22.01 ms | 19.87 ms | 1.11x |
| 40 | 27.37 ms | 22.40 ms | 1.22x |
| 64 | 37.31 ms | 29.26 ms | 1.28x |

排序的代价是每轮 O(n log n)，游标一多才显出来；十几个游标以内它被 posting 解码和块推进盖住
（同一张表里 2–4 词的另外五条查询都在 ±4% 内）。

1M 文档上的索引构建与三路对比（参考与端口并排，含 naive 与旧排序版）见 §3.1：词一多差距就是
数量级的——旧写法每轮把整个游标数组按值排序，`Cursor` 里带着 `WandStream`（若干个 `Vec`），
一次排序要搬上百个 100 字节以上的结构体，而堆里只搬 16 字节的 `(doc, ord)`。

对拍：`wand_matches_full_scan_on_a_random_corpus`（2000 篇，k=1/10/100）和
`heap_wand_matches_naive_scan_on_many_corpora`（3 组语料、稠密与稀疏混排、k 从 1 到全量、
两种 `delta`/`min_should`、BM25 与 boolean）拿朴素 OR 全扫逐条比 row id 与分数；
`test/sql/dql/fulltext` 的 50 条语句、82 条查询在新旧实现下输出逐字节相同。

## 5. 顺带修掉的一个格式缺陷

`encode_fulltext_chunk`（`crates/executor/src/blocks/`）原来对
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
`crates/executor/tests/fulltext_format.rs::repeated_term_counts_one_document_and_every_occurrence`。

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


## 12. 段清单、目录无关的 chunk 发现、IO 由持久化管理器接管

这一节记录「参考写出的全文索引能被本端口直接读」这条路的形状，以及三个决定它成立的改动。

### 12.1 三个问题的答案

| 问题 | 结论 |
|---|---|
| 参考没有 `.ok` 清单，怎么读它的 chunk | `.ok` 是**可选**的：只有 `NotFound` 才算「没有清单」，此时只做结构校验（`.len % 4 == 0`、`.dic` 的 FST 尾部可解析），`live_docs`/`total_len` 从 `.len` 现场算（全部视为存活）。参考不因删除重写 chunk，所以这个回退与它的评分口径一致，不是妥协。其它读错误、截断、magic/版本不符**不是**「没有清单」：`ChunkManifest::read` 返回错误，`SegmentIndexReader::open` 随之失败，chunk 被判坏而不是绕过校验重建 |
| 参考的 chunk 在哪、怎么发现 | 清单在 catalog（`TableIndexMeta → segment → chunk → ChunkIndexMetaInfo`），文件在 `db_{db}/tbl_{tbl}/idx_{idx}/seg_{seg}/ft_{…}.{dic,pos,len}`。本端口运行时索引在 `<root>/fulltext/<db>.<table>.<column>/`，所以打开一个参考写出的目录时**没有任何 chunk 可扫**：`text_index_for` 先按 catalog 走一遍，把那些 chunk 拷贝进自己的目录（`TextIndex::adopt`），再走原来的加载路径 |
| 谁做 IO | 持久化管理器。`format::write_synced` 与 `MappedFile::open` 是仅有的两个收口点，它们都经 `storage::chunk_store`：装了管理器（`object_dir`）就发布对象 / 从对象区间映射，否则是直接落盘 |

### 12.2 实现

| 位置 | 内容 |
|---|---|
| `storage/src/io/chunk_store.rs` | `ChunkStore` 门面：`LocalChunkStore`（`write_synced_direct` / `MappedFile::open_path`）与 `PersistentChunkStore`（`pm.persist_keyed` + `MappedFile::open_range`）。两种发布：`write` 是参考的默认 `try_compose=true`（块列等小文件共用一个有界对象），`write_dedicated` 是 `try_compose=false`（全文 chunk 文件，与参考的 `memory_indexer_impl.cpp:679` / `column_index_merger_impl.cpp:162` 一致，一个文件一个对象），两者都在**同一把 per-path 锁**内做完本地替换与对象发布。管理器的键：`data/` 下的文件用参考的 `RemovePrefix` 规则（`db_1/tbl_2/…`），`fulltext/` 缓存用相对引擎根的键，同一个 `pm|object|` 命名空间 |
| `storage/src/buffer/mapped.rs` | `open_path`（裸文件）+ `open`（走 store）+ `open_range(obj, offset, len)`：组合对象里的一段按区间映射；零长文件仍是空映射 |
| `storage/src/persistence/persistence_manager.rs` | `persist_with(.., try_compose)`、`persist_keyed`/`lookup_key`/`cleanup_key`/`keys_under`；对象写完后 `fsync` 文件**与 workspace 目录项**（rename 本身是目录更新），`current_obj_finalize` 写 footer 后同样处理；`persist_directory` 只在**内容比对**确认已发布的字节与磁盘一致时跳过（大小相同不算证据：原地改写不改长度、截断成 0 也会留下记录） |
| `executor/src/engine/text_index.rs` | `CatalogChunk` + `TextIndex::adopt`；`load_sealed_scoped(live_segments, catalog)` 先扫自己的目录，空了才采纳 catalog 的 chunk。合并/坏段/弃用段的处理不变 |
| `executor/src/engine/statements.rs` | `fulltext_catalog_chunks(table, column)`：参考的 `ColumnIndexReader::Open` 那条路（catalog 走 chunk 记录 + 段目录）。两种表名拼写都试（索引记录里的 `table` 可能是裸名） |
| `executor/src/engine/admin.rs` | `optimize_fulltext_locked` 改 `async` + `spawn_blocking`，并把 `tombstone_set` 传给合并（死行过滤） |
| `executor/src/blocks/engine.rs` / `ddl.rs` / `statements.rs` | 删目录改走 `chunk_store::remove_tree`，被删文件的对象部分一并从 `pm|object|` 里去掉 |

`.ok` 只出现在端口自己的 `fulltext/<key>/` 里；给参考读的 `idx_{id}/seg_{s}/` 永远只写三个文件（`write_fulltext_index` 只写 `.dic/.pos/.len`），所以跨引擎的字节与文件清单对比不受它影响；`python/restart_test/fulltext_common.py::merged_files` 显式跳过 `.ok`。

**偏差（有意）**：`object_dir` 模式下端口保留本地副本作为缓存，参考会把临时文件移进对象、不留逻辑路径文件。代价是对象的字节与本地文件各一份；换来的是目录枚举、`restore_missing_files` 与 mmap 的局部性都照旧工作，读路径不必每次都从对象里拷一段。

### 12.3 验证

单测（`cargo test -p executor`，762 通过 / 38 ignored；`cargo test -p storage` 495 通过）：

| 用例 | 断言 |
|---|---|
| `a_chunk_written_in_the_reference_layout_is_adopted` | `flush` + `dump_index_transaction` 之后删掉端口自己的 `fulltext/` 目录，重开引擎查询仍然命中；端口目录里重新出现**同名**的 chunk（采纳，而不是重新分词） |
| `the_persistent_manager_publishes_every_data_file` | `data/` 下每个文件都有 `pm|object|` 记录、对象长度覆盖 `part_offset + part_size`；删掉块列文件与 chunk 的 `.dic` 之后 `MappedFile::open` 仍返回同样的字节（从对象区间映射） |
| `a_chunk_with_an_empty_length_array_maps` | `.pos`/`.len` 都是 0 字节时不报 `EINVAL`，`doc_count()==0`，`load_sealed` 跳过而不是当作缺口覆盖 |
| `a_sparse_chunk_keeps_its_span_and_merges_with_its_neighbour` | chunk 内部有洞（0,1,5,6）时 `docs` 是跨度 7、`live_docs` 是 4；与 7..9 的邻居仍可合并，合并后跨度 9、存活 6 |
| `a_seal_leaves_out_the_rows_deleted_before_it` / `a_merge_reclaims_the_postings_of_a_deleted_row` | 封段与合并都按 tombstone 过滤，`.ok` 记的 `live_docs/total_len` 与实际 posting 一致 |
| `writers_readers_deletes_and_merges_stay_consistent` | 8 写 × 20 批 × 100 行 = 16 000 行、6 读 × 持续查询、1 删（16 轮）、8 轮 dump+OPTIMIZE+flush 并发跑：读者看到的每个 id 都在已插入集合内；`count(*)` + 墓碑 = 16 000；全文查询行集合 == `SELECT id` 行集合；重开引擎后同一答案（参数与读数见 §12.6） |

跨引擎字节门禁仍由 `python/restart_test/test_cross_engine_fulltext_{scale,production}.py` 守。端口侧需要 `rust_engine/target/release/infinity`（本轮已构建），但门禁还要同一台机器上的**参考二进制**，本机没有，故仍未跑；10M 那一档改用进程内示例跑通（§13.5）。本轮没有改动任何编码器：`.dic/.pos/.len` 的字节只经过 IO 门面，内容不变。

### 12.4 IO 收口的完整清单

「持久化管理器接管所有 IO」在端口里落到三个收口点，其余全部经由它们：

| 收口点 | 覆盖 | 说明 |
|---|---|---|
| `definition::format::write_synced` | `version`、全部分块与索引 chunk（全文 `.dic/.pos/.len`、BMP/EMVB/multivector/PLAID/HNSW/IVF、二级索引 `.idx`） | 发布时写本地副本（缓存）+ 对象；全文 chunk 用 `write_dedicated`（`try_compose=false`，与参考的全文写入器一致），其余用 `write`（参考的默认，多个小文件共用有界对象） |
| `definition::format::write_synced_parts` / `write_parts_no_sync` | 块列 `{id}.col`、`col_{id}_out`（`write_block_column_synced`） | **不经 store**，直接落盘：`parts` 版本省掉一次整体拷贝，`no_sync` 版本给 import（参考的 `COPY` 也不 fsync 块，靠随后的 checkpoint）。`object_dir` 下这些文件的对象由 checkpoint 的 `persist_directory` 发布（内容比对后才跳过），不是写的时候就发布 |
| `MappedFile::open` / `open_range` | 块列、全文 chunk、BMP/EMVB/multivector、PLAID | 本地文件优先；只有对象时按 `ObjAddr` 映射区间 |
| `chunk_store::{read_file, write_file, open_file, remove_tree}` | 块 `version`、各索引 chunk 的探针读取（HNSW/IVF/PLAID 覆盖度、chunk 行数）、删表/删索引 | 上一轮还有 15 处直接 `std::fs` 的 chunk 读取，本轮全部改走 store |

**WAL / catalog / 导入源文件不在此列**，与参考一致（参考的 `NewTxn` 走 `VirtualStore` 与 KVStore，不经 `Persist`；`COPY` 的源文件是用户输入）。

**唯一未接的**：`index` crate 自己的 `SecondaryIndex::load` / `HnswIndex::load_sparse` / `chunk_file::load`。`storage` 依赖 `index`，反向加依赖会成环，所以这三处仍是 `std::fs::read`；但它们的调用者**全在 `#[cfg(test)]` 里**（见 §12.7 的核实），生产路径是 `HnswIndex::from_chunk_parts` 加上 executor 从 store 读出的字节，所以生产 IO 已经全部经 store，这条缝只影响该 crate 自己的测试。要彻底消除，得把「读字节」上移到 executor（`index` 只接受 `&[u8]`）。

### 12.5 跨引擎门禁（本轮实跑）

| 用例 | 结果 |
|---|---|
| `test_realtime_concurrent_inserts_and_reads_agree` | 通过（8.2 s） |
| `test_offline_build_writes_the_reference_bytes` | 通过（15.9 s）：端口 spill + `OfflineDump` 的产物与参考逐字节相同 |
| `test_repeated_fulltext_merges_stay_byte_identical` | 通过（31.5 s）：3 轮 × 3 chunk × 4000 行，每轮合并产物与参考逐字节相同 |
| `test_the_port_reads_an_index_the_reference_wrote`（本轮新增） | 通过（4.8 s）：**参考写 + dump 的同一目录，端口只查询、不 dump**；断言端口自己的 `fulltext/<table>` 目录出现在查询之后，且其中的 `.dic/.pos/.len` 与参考的逐字节相同 —— 采纳（拷贝），不是重新分词 |
| `test_restart_after_dump_and_merge_keeps_the_answers`（8 个变体） | 通过（9 min 47 s） |
| `test_deletes_and_updates_leave_the_index_consistent` | 通过（17.7 s） |
| `test_writers_readers_and_maintenance_share_the_index` | 通过（13.0 s） |
| `test_kill_during_the_build_recovers_the_index`（3 个变体） | 通过（4 min 19 s）：spill / seal / merge 中途 `kill -9` 后重开仍一致 |

（`test_a_long_build_keeps_the_index_memory_flat` 未跑：16 万行 × 两档规模的匿名内存采样，本机 available 内存约 7 G，起服务有 OOM 风险。）

### 12.6 大规模并发（`cargo test`，debug 构建，约 55 s）

`writers_readers_deletes_and_merges_stay_consistent`：**8 写 × 20 批 × 100 行 = 16 000 行**，6 个读线程持续查询，1 个删除线程 16 轮（每 17 行删 1），主线程 8 轮 `dump_index → OPTIMIZE → flush` 与上面全部并发。不变量是「表里有什么」而不是「删线程请求了什么」（一个落在一个尚未提交的事务上的 DELETE 什么都不删，请求列表看不见）：

| 断言 | 结果 |
|---|---|
| 读线程看到的每个 id 都在已插入集合内 | 通过（无幽灵行） |

> 「无幽灵行」这一条曾偶发失败并报 `reader saw the ghost id`，原因在**测试自己的记账顺序**：写线程先 `INSERT`（提交即对其他线程可见）再把 id 写进 `inserted`，而读线程是查询**之后**才 clone 这份集合，于是「刚提交、还没记账」的那一批会短暂地不在集合里。修法是把 `inserted.extend(ids)` 挪到 `INSERT` 之前——记账失败不会发生（`INSERT` 本身 `unwrap`），而「可见即已记账」变成了构造性事实。索引侧没有返回过未提交的行。
| `SELECT count(*)` + 墓碑数 = 16 000 | 通过 |
| 全文查询（`beta`，topn=100000）的行集合 == `SELECT id FROM stress` | 通过（0 missing / 0 extra） |
| 删除真的发生了 | 通过（断言 live < 16 000） |
| flush 后重开引擎，同一问题同一答案 | 通过 |

### 12.7 删除与恢复也归管理器（本轮补齐）

前一轮只把「写」和「读」收进了 store，**删除**仍是 `std::fs::remove_file`。在 `object_dir` 模式下这留下两件事：`pm|object|` 记录不清、对象字节不回收，而且 `restore_missing_files` 会在下次打开时把已删除的文件**复活**。本轮：

| 改动 | 内容 |
|---|---|
| 删除走 store | `chunk_store::remove_file` / `remove_tree`；`blocks/engine.rs`（块列、镜像 chunk、deprecated chunk）、`engine/ddl.rs`（DROP INDEX 的存储）、`engine/dml.rs`（spill 集合重写）、`engine/statements.rs`（multivector/bmp 重建前的清理）、`engine/index_build.rs`（HNSW/IVF 旧文件）全部改走它。`ChunkStore::remove` 与 `remove_file` 现在返回 `Result`：两个后端都把「文件不存在」当成功（保持旧行为），其它失败向上传播，所以弃用 chunk 的标记只在磁盘与对象都真的删掉之后才清 |
| 恢复的目标路径 | 参考的键都相对 `data/`；端口的全文 chunk 缓存在 `data/` 旁边的 `<root>/fulltext/`，所以 `PersistenceManager::local_path_of` 对 `STORAGE_ROOT_KEYS`（当前只有 `fulltext/`）用引擎根解析，其余仍用 `data/`。修之前恢复会把缓存写到 `<root>/data/fulltext/…`（`a_data_directory_comes_back_from_the_objects_alone` 抓到的真实缺陷） |
| `index` crate 的三处直读 | 核实过：`SecondaryIndex::load` / `HnswIndex::load_sparse` / `chunk_file::load` 的调用者**全在 `#[cfg(test)]` 里**（`index/src/lib.rs:605`、`vector.rs:2502`、两个 `file_roundtrip` 辅助），生产路径走的是 `HnswIndex::from_chunk_parts` + executor 从 store 读出的字节。所以生产 IO 已经 100% 经 store，这条缝只影响该 crate 自己的测试 |

新增/更新的验证：

| 用例 | 断言 |
|---|---|
| `a_data_directory_comes_back_from_the_objects_alone`（Rust） | `object_dir` 下 400 行 + 2 次 dump + merge：删掉 `data/` 与 `fulltext/` 两棵树后 `restore_missing_files` 把 **10 + 4** 个文件按原字节恢复，且查询答案与删除前一致 |
| `test_the_port_comes_back_from_its_objects_alone`（Python，新） | `persistence_format = "reference"`：写 → dump → 杀进程 → **删掉 `data/` 与 `fulltext/`** → 新进程重建两棵树并用 `uniq{i}`/`bucket{b}` 全量校验（6.4 s） |

## 13. 10M 规模实测（本机，debug 构建）

`python/restart_test/fulltext_scale_gate.py --rows 10000000`：一个进程里插 10 000 000 行
（`body` = 12 个词：共享词表 + `bucket{i%7}` + `uniq{i}`），flush → dump → 查询 → OPTIMIZE →
删 1% → OPTIMIZE → 重启，期间用「不变量」校验（`uniq{i}` 只能返回 `[i]`，
`bucket{b}` 的 `filter_fulltext` 计数必须精确）。

### 13.1 读数

| 阶段 | 结果 |
|---|---|
| 插入 10M（5000 行/语句） | **1231 s = 8123 行/s** |
| flush（块落盘） | 879.5 s |
| dump（封段成 chunk） | 1.5 s（13 个 chunk/100 万行，共 100+ 个 → 3 文件 each） |
| 首次校验（7 个 `uniq` 探针 + 3 个桶的 top-10 与精确计数 + 三词 top-10） | 全部通过，约 10 min |
| `OPTIMIZE`（把 100+ chunk 合成 1 个） | **363 s** |
| 引擎 RSS | 插入后 2.5 GB，校验时峰值 4.1 GB（可用内存 7 GB） |
| 磁盘 | `data/` 2.6 GB + `wal/` 1.3 GB + `fulltext/` 1.3 GB；合并峰值另需 ~1.3 GB（本机 7.4 GB 余量，勉强够） |

10 万行同脚本的对照（用来外推）：插入 11.5 s（8668 行/s）、flush 6.2 s、dump 0.6 s、
optimize 2.6 s、`data/` 27 MB、`fulltext/` 13 MB、RSS 0.19 GB。

> 上表是 **debug 构建 + 门禁脚本（5000 行/语句）** 的读数；同一份数据用 release 构建走进程内
> 示例（§13.5）是插入 63–72k 行/s、flush 406–455 s、merge 70–79 s。差别在构建类型，不在索引
> 路径——下面 §13.2 那个「合并后 3 小时」的悬崖与构建类型无关，它在 release 下同样会发作。

### 13.2 发现的性能悬崖（已修：位置流的 seek 重放了整个前缀）

合并成**一个** 10M 行的 chunk 之后，同一组校验跑了 3 小时仍未结束（CPU 104%，单线程、
非死锁）。根因不在内存表示，而在**跨块 seek 时的位置流**：

`TermDocCursor::seek_block_of` 用文档 skip list 跳到目标块之后，原来调
`reset_positions(prev_ttf)`——把位置流**倒回起点**、再把跳过的那些 occurrence 一条条丢掉
（`disk_segment_reader.rs` 的 `reset_positions` / `next_delta` 里的 `pos_skip`）。于是
**一次 seek 的代价 ∝ 该 term 在段内已走过的 occurrence 数**，整表扫描就退化成
`O(N² / (2·块大小·tf))`。参考实现不是这样的：它写 `.pos` 时就为每个 128 位置记录写了
位置 skip list（`[u32 total_pos][u32 offset]`，`SkipListWriter::add_item_2`），
`PositionIterator::SkipTo` 直接定位到记录；本移植写了这份 skip list 却没有任何读取代码用它。

后果与「合并」的关系：合并把 k 个短列表拼成一个长列表，这份前缀重放的成本正好乘以 k。
300k 行（`mem_index_capacity` 65536 → 5 个 chunk）上的实测：

| 状态 | 修复前 | 修复后 |
|---|---|---|
| dump 后（新进程，5 chunk） | 10 398 ms | 222 ms |
| optimize 后（新进程，1 chunk） | 40 073 ms | 177 ms |

同一时刻三词 `alpha beta gamma` 前后都是 620-627 ms，`(id, SCORE())` 逐位相同
→ 数据与答案都没变，纯粹是读侧代价。外推 10M：合并前 ≈6 min，合并后 ≈十几小时，
正好对上 §13.1 的「首次校验 10 min」与这次的「3 小时未完」。

修复（纯读侧，写出的字节不变）：`TermDocCursor` 增加 `pos_skiplist`，`seek_positions`
用 `SkipTo(prev_ttf + 1)` 拿到记录起始 offset 与该记录之前的 occurrence 数，
`pos_skip` 只剩**一个记录内**的前缀。回归测试
`a_hop_lands_on_the_position_record_it_needs` 断言一次跳到 2000 篇列表末尾只解 ≤8 个位置
记录（`pos_records_decoded` 计数器），而不是 46 个。

顺带纠正：门禁里的 `"alpha beta"` 走的是 **WAND + `PhraseCursor`** 快路径
（`has_query_syntax` 明确把引号留给快路径），不是 `PhraseDocIterator` 那条全量物化路径，
所以早期把它归因到 `PhraseDocIterator` 是错的（§13.4 讲的是另一条路径的改进）。

### 13.3 10M 的字节兼容与持久化

- **字节兼容**：已在本机跑过并通过的是 10000 / 40000 / 36000 行三档（offline 构建、
  两轮反复 merge）与「参考写、端口读」的采纳门禁；10M 这一档的 `--compare-cpp` 需要在同一台
  机器上先跑参考再跑端口（各 ~3 GB 数据），本机 7.4 GB 余量下**尚未跑完**，脚本已按
  「顺序构建、构建完即删」实现，换一台磁盘宽裕的机器可直接跑。
- **持久化**：10M 这一轮跑的是本地 profile；`object_dir` profile 的门禁（`data/` 与
  `fulltext/` 全删后从对象重建）在 30 万行以内已验证通过（§12.7）。10M + 对象 profile 会把
  磁盘占用翻倍（本地副本 + 对象），本机放不下，属于同一脚本的 `--persistence reference`
  加更大磁盘即可覆盖。

### 13.4 短语查询的内存表示已改（查询串语法那条路径）

> 注意：这条路径只在查询串语法（`a AND "b c"`、`col:term`、`a OR b`）时才会走到；
> 门禁里的 `'"alpha beta"'` 走的是 `PhraseCursor` 快路径，10M 的问题不在它（见 §13.2）。

`PhraseDocIterator` 的每条 posting 原来是 `(u64, Vec<u32>)`：一个文档一次堆分配。
10M 行 × 2 个词 ≈ 2000 万次分配，debug 下光构造就要几十分钟，常驻堆约 1 GB。

改成 `PhrasePostings`（`crates/executor/src/query_builder.rs`）：

```rust
pub struct PhrasePostings {
    docs: Vec<(u64, u32, u32)>, // (doc id, 位置起点, 位置个数)，升序
    positions: Vec<u32>,        // 一个词一块 arena
}
```

- 每个词两次分配（`docs` + `positions`）；位置仍全部在内存（精确短语匹配需要），但内存
  从「每文档 24 字节头 + 独立分配」降到「每文档 16 字节 + 每位置 4 字节」，10M × 2 词大约
  480 MB 而不是 1 GB，也不再触发 2000 万次 `malloc`。
- `exact_match` 直接对着 arena 切片做二分（原来每文档 clone 一次头词的 `Vec<u32>`，现在零拷贝）；
  `sloppy_match` 只在匹配到的文档上建临时 `Vec<i32>`。
- 语义不变：`phrase` / `sloppy_phrase_frequency_matches_the_reference` /
  `phrase_boost_scales_the_phrase_score` / `a_phrase_match_answers_from_the_index` /
  `search_honours_the_phrase_slop` 等 7 个用例全绿；`cargo test -p executor --lib`
  762 passed / 38 ignored。

仍未做的是把它变成**流式** positional cursor（按文档前进时解码当前位置，不再一次性物化整列）：
那需要让 `TermStream` 跨 chunk 持有解码状态，属于下一步。

### 13.5 进程内大规模探针（不占端口）

`crates/executor/examples/fulltext_scale.rs`：直接用 `Engine` 驱动，不经过服务、不占端口，
所以可以和占着固定 Thrift 端口的门禁并存。位置参数是行数与目录，开关是
`--probes <n>`（探针数）、`--no-phrase`（跳过短语）、`--delete-pct <p>`（删除比例，默认 1）；
同名的 `FT_SCALE_PROBES` / `FT_SCALE_PHRASE` / `FT_SCALE_DELETE_PCT` 在缺省开关时仍生效。

| 规模 | 阶段 | 结果 |
|---|---|---|
| 100k | 插入 6.8 s（14 610 行/s）、dump、optimize、删 1%、再 merge、重启 | 全部不变量通过 |
| 300k | 同上（含短语） | 全部不变量通过；`uniq` 计数 1.73 s、三词 top-10 663 ms、短语 top-10 39.5 s → **修复位置 seek 后 0.08–0.22 s**（§13.2，dump 后 / 合并后分别为 222 / 177 ms） |
| 1M | 同上（`FT_SCALE_PHRASE=0`，`FT_SCALE_PROBES=2`） | **全部不变量通过**；optimize 30.5 s、`uniq` 计数 5.3 s、桶计数 6.8 s、三词 top-10 2.19 s、删 1%（1 万行）**406 s**、重开 13.9 s、RSS 309 MB |
| 10M | `--probes 2 --delete-pct 0`（release 二进制，含短语） | **全部不变量通过**；插入 157.9 s（63 314 行/s）、flush 406.5 s、dump 0.3 s、`uniq` 计数 5.0 / 9.8 s、桶计数 7.0–7.2 s、三词 top-10 2.92 s → 合并后 1.84 s、**短语 top-10 1.70 s → 合并后 1.76 s**、optimize 79.1 s、重开 20.5 s、RSS 2.2 GB（重开后 3.6 GB）、目录 5.1 GB |

> 读数取自合并 `upstream/main` **之前**的 release 构建。合并在 IO 原语上做的改动（块列改走
> `write_synced_parts`、省掉一次整体拷贝）只会让 flush 更快；review 之后的 per-path 锁只加在
> 对象 profile 的写路径上，marker 读取改成传播错误也只影响失败分支。所以上表对这条
> 「直接落盘 + 合并 + 查询」的路径仍然成立，没有被后续改动拉慢。

三条与规模相关的结论（都能从上面的读数直接读出来）：

1. **短语查询曾经是超线性的，原因是位置流的 seek 重放了整个前缀（已修）**：110k → 300k 行，
   `"alpha beta"` top-10 从 2.58 s 涨到 39.5 s（数据 3×，时间 15×），而三词严格线性
   （0.22 → 0.66 → 2.19 s）。根因与修法见 §13.2：`seek_block_of` 每次跨块都把位置流倒回
   起点再逐条丢弃（`reset_positions` + `pos_skip`），代价 ∝ 已走过的 occurrence 数，
   合并把短列表变长列表正好乘以 chunk 数。修好后 300k 短语 80–222 ms，与三词同量级，
   也不再随合并放大。**这条原来被误记为「必须改成流式 positional cursor」**——`PhraseCursor`
   本来就是流式的，缺的是位置 skip list 的 seek。
2. **`filter_fulltext` 是谓词、按行求值**：`SELECT count(*) … WHERE filter_fulltext(…)`
   在 100k / 300k / 1M 上是 0.58 / 1.73 / 5.3-5.7 s，与行数成正比而与命中数无关。
   门禁拿它做「精确计数」是对的，但它测的不是索引的检索延迟；要测检索延迟得看
   `SEARCH MATCH TEXT … topn`（上面三词那一行才是索引路径）。
3. **无索引上的 `DELETE … WHERE id IN (…)` 也是全表扫描**：删 1 万行在 1M 表上 406 s
   （100k 表上删 1 千行 4.4 s），因为 `id` 上没有二级索引，规划器只能扫。生产里这类删除
   应当走 `_row_id` 或带二级索引的列——这不是全文索引的问题，但会决定「删除 + merge」
   这条链在大表上的吞吐。

> **10M 复跑（已完成，进程内）**：上一轮的 10M 进程是人工 kill 的——它卡在合并后的短语校验
> 上（3h17m，单线程 CPU 100%，`engine.log` 无 panic、内核无 OOM 记录），原因即 §13.2 的
> 位置 seek。修复后 300k 已复测通过（dump 222 ms / 合并 177 ms，`(id, SCORE())` 与修复前
> 逐位相同），10M 这一档现在也跑通了（release 二进制，两轮，上表最后一行）：
>
> - **短语不再随合并放大**：dump 后 1.70 s、合并成一个 chunk 后 1.67 s，与 300k 的
>   0.08–0.22 s 只差数据量本身的常数；三词 2.9–3.2 s → 合并后 1.8 s；`uniq` 探针 5–8 s、
>   桶精确计数 7–8 s（这两个是 `filter_fulltext` 逐行谓词，O(行数)，不是索引延迟）。
> - **删除**：第二轮带 `--delete-pct 0.001`（100 行，一条 `DELETE … WHERE id IN (…)`）通过，
>   删除语句本身 14.5 s（`id` 无二级索引 → 全表扫描），删除后的全部不变量（含短语）通过；
>   紧随的 `optimize after delete` 是 0 ms——删除只改 tombstones，没有新 chunk 被封，没有可
>   合并的段，所以 10M 上「删除 → merge」这条链本身没有被这一轮覆盖（300k / 1M 上覆盖过）。
> - 开销：merge 69.5 s、flush 406–455 s、重开 20–22 s、RSS 2.2–2.5 GB、目录 5.1 GB。
>
> 仍缺的口径只有两个，都需要环境而不是代码：
>
> ```bash
> cargo build --release -p cli --bin infinity -j 4
> .venv/bin/python python/restart_test/fulltext_scale_gate.py --rows 10000000 --compare-cpp
> .venv/bin/python python/restart_test/fulltext_scale_gate.py --rows 1000000 --persistence reference
> # 进程内（不占端口，上面两轮就是这个）：
> rust_engine/target/release/examples/fulltext_scale 10000000 /tmp/ft-10m --probes 2 --delete-pct 0
> ```
>
> 即 **10M 的 `--compare-cpp` 字节门禁**（要先在同一台机器上构建并跑参考，各约 3 GB 数据，
> 本机 C++ 构建产物不存在）与 **10M + `object_dir` profile**（磁盘占用翻倍）。
