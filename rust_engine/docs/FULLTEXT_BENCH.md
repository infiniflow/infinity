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

五条查询与参考一致（`fields="text"`、默认 `topn=10`、投影 `ROW_ID(), SCORE()`）：

```
harmful "social custom"
social custom "harmful chemical"
"annual American awards"
harmful chemical
"one of"
```

## 2. 数据集

参考写死 `test/data/benchmark/dbpedia-entity/corpus.jsonl`，**这个文件不在仓库里**。
本次用 `/home/zhichyu/benchmark/enwiki/enwiki-10w.csv`（10 万行，tab 分隔的
title/日期/正文）前 20000 行生成 JSONL（`{"id","title","text"}`），两个引擎读同一个文件。

> `ReadJsonl` 的耗时不可直接比：参考用 simdjson，端口是手写的字符串扫描；
> 这一项不进入查询对比。

## 3. 结果（本机，20k 文档，各跑 3 次）

| 阶段 | 参考 C++ | 端口 Rust | 比值 |
|---|---|---|---|
| CREATE INDEX + Flush | 3–5 ms | 14.6–17.6 ms | ≈3.5× 慢 |
| ReadJsonl（不计入） | 17–29 ms | 60–62 ms | — |
| INSERT 20000 行（batch=500） | ≈1 s（日志只到整秒） | 3.24–3.28 s | ≈3× 慢 |
| IMPORT 20000 行 | ≈2 s | 3.14 s | ≈1.6× 慢 |
| Optimize（合并索引 chunk） | 360–512 µs | 299–397 µs | 同量级 |
| 查询 1 `harmful "social custom"` | 1–5 ms | **0.40 ms**（18–20 ms） | 反超 |
| 查询 2 `social custom "harmful chemical"` | 0.93–1 ms | **0.49–0.50 ms**（13.9–15.3 ms） | 反超 |
| 查询 3 `"annual American awards"`（0 行） | 0.27–0.73 ms | **0.43–0.44 ms**（14.8–18.4 ms） | 同量级 |
| 查询 4 `harmful chemical` | 0.58–0.97 ms | **0.40 ms**（13.2–14.9 ms） | 反超 |
| 查询 5 `"one of"` | 1–2 ms | **2.49–2.51 ms**（30.8–33.3 ms） | ≈2× |

括号里是改成流式读取之前的数字。写入仍是同一个量级上慢 3 倍（每批插入都要
更新全文索引）；查询从 13–30 倍降到「四条比参考快，一条约 2 倍」。

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

要再逼近，需要把「按块定位」和「求下一处匹配」拆成两个动作（参考的
`NextShallow` / `Next`），让被剪掉的块根本不进位置验证；这是下一步。

参考侧是块级 WAND + 跳表流式推进（命中 k 个就停），只投影 `row_id`/score 两列，
所以耗时基本与语料规模无关（20k 文档仍在 1 ms 量级）。

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
