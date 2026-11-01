# 二级索引性能：参考 vs 重写版

> 复现：`tools/run_secondary_bench.sh lookup`（建索引 + 查询 + 一次维护）与
> `tools/run_secondary_bench.sh maint`（维护成本随索引数增长）。
> 数据落在 `.scratch/sec-{cpp,rust}/`，脚本见 `tools/bench_secondary_{lookup,maint}.py`。

## 1 口径

| 项 | 设置 |
|---|---|
| 机器 | 同一台，两个引擎**不同时**运行（各自独占一次） |
| 隔离 | 每个引擎跑在 `bwrap --unshare-net` 的网络命名空间里，23817 是本轮私有端口 |
| 客户端 | 同一个 `.venv` 的 Python SDK、同一段 SQL |
| 表 | `(id INT, name VARCHAR, score DOUBLE)`，10 万行，`flush_data()` 后全部封存 |
| 配置 | `optimize/cleanup/compact/checkpoint` 间隔全为 `0s`，`result_cache = off`，`memindex_memory_quota = 2GB`（行数触发的 dump 仍然开着，两侧一致） |
| 取值 | `lookup` 3 轮的中位数；`maint` 单轮（脚本可重跑） |
| 结果规模 | `score = v` → 100 行；`score ∈ [v, v+10)` → 1000 行；`name = 'n-x'` / `id = x` → 1 行 |

## 2 建索引与查询（10 万行已封存）

| 相位 | 参考 (s) | 重写版 (s) | 比值 |
|---|---:|---:|---:|
| 插入 10 万行（无索引） | 2.265 | 2.290 | 1.01x |
| 建二级索引 `score` (DOUBLE) | 0.0260 | 0.0549 | 2.1x |
| 建二级索引 `name` (VARCHAR) | 0.0366 | 0.0604 | 1.7x |
| 建二级索引 `id` (INT) | 0.0329 | 0.0552 | 1.7x |
| 无索引点查（200 次 × 100 行） | 0.1410 | 0.1293 | **0.92x** |
| 无索引范围（100 次 × 1000 行） | 0.1047 | 0.0819 | **0.78x** |
| 索引点查，唯一键（200 × 1 行） | 0.0712 | 0.0623 | **0.87x** |
| 索引点查，VARCHAR 唯一键（200 × 1 行） | 0.0601 | 0.0411 | **0.68x** |
| 索引点查，100 行（200 次） | 0.0638 | 0.0664 | **1.04x** |
| 索引范围，1000 行（100 次） | 0.0574 | 0.0552 | **0.96x** |
| 带 3 个索引插入 2 万行 | 0.4639 | 0.4980 | 1.07x |

两侧的物理计划相同：`EXPLAIN` 对 `=` 与 `>= … <` 都给 `INDEX SCAN`。**所有查询形态（走索引与不走索引）
现在都持平或更快**（0.68–1.04x）：全表扫描的那两行过去是 5.0x / 8.0x，原因是扫描把每一行都解码成 `Value`
再过滤；现在谓词直接在块的字节上判（见 §4）。

## 3 维护成本（同一次运行；5 000 行一条语句）

| 相位 | 参考 (s) | 重写版 (s) | 比值 |
|---|---:|---:|---:|
| UPDATE，3 个索引在线 | 0.0070 | 0.0170 | 2.42x |
| UPDATE，索引已删除 | 0.0104 | 0.0169 | 1.62x |
| DELETE（按索引列），3 个索引在线 | 0.0036 | 0.0041 | **1.14x** |
| DELETE，索引已删除 | 0.0067 | 0.0092 | **1.38x** |

**UPDATE 的索引边际成本已经落到噪声里**：3 个索引在线（17.0 ms）与不带索引（16.9 ms）几乎相同；
同口径的 `maint`（每一档扫描形状相同）从 1 个索引的 1.8 µs/行涨到 3 个索引的 2.7 µs/行，即 0.45 µs/行/索引。
2.42x 里主要是 DML 基线（不带索引 1.62x）与「参考在带索引时扫描更便宜」。DELETE 与参考同级。

随在线索引数量的斜率（`maint` 口径，2 万行表，5 000 行/档，µs/行）：

| 相位 | 参考 (µs/行) | 重写版 (µs/行，0/1/2/3 个索引) |
|---|---|---|
| UPDATE | 1.6–3.9 | 2.6–3.4 / 1.7–1.9 / 2.1–2.2 / 2.7 |
| DELETE | 0.8–1.0 | 8.0 / 7.6 / 7.7 / 7.8 |

UPDATE 第一档最慢是冷启动：第一句要打开块索引、暖缓存；从 1 个索引到 3 个索引是 1.8 → 2.7，
即每个索引 +0.45 µs/行。DELETE 从 8.0 到 7.8，与索引数无关。
`maint` 的 DELETE 绝对值比 `lookup` 高，是因为它按 `score` 逐值删除、每档命中 5 000 行而表只有 2 万行，
固定开销摊不开。

## 4 这一轮修掉的问题

1. **索引命中后逐行取数**：`decode_rows_of_values` / `decode_block_column` 从「扫整块窗口」改成「按
   索引给的 slot 列表解码」。
2. **列式批量路径的 slot 映射错位**（正确性问题）：`read_columns_batch_for_rows` 用 slot 下标去索引
   「已产出行的位置」，块里只要有一个隐藏 slot、或 slot 下标 ≥ 命中行数，后面的行就整片变成 NULL ——
   `CREATE INDEX` + checkpoint 之后 `WHERE indexed_col = v` 只返回 0–3 行。改为按同一份可见性映射，
   回归用例 `a_secondary_index_answers_equality_after_a_checkpoint`（旧代码在该用例上返回 0）。
3. **索引探测后的逐行回表校验**：`index_lookup` / `index_range_lookup` 为每个候选行解码一次整行文本再比对，
   而两个调用方本来都会对取回的行重新求值谓词；删掉这一趟。
4. **`SecondaryIndex::approx_bytes` 是 O(键数)**，而 memindex 配额每行都查：3 个 10 万键的索引下，
   一条 5 000 行 UPDATE 有一半时间在重复求和自己；改为增量维护。
5. **点读扫 append buffer**：`pending_row` 线性扫描 + `pending_rows_of` 整份克隆。改成 `seq -> position`
   映射，列式扫描原地遍历 buffer。
6. **点读克隆删除日志**：`BlockCursor::row` 每行 `deleted_ids(table)` 克隆一次删除日志，代价是日志长度，
   于是 DELETE 看起来随索引数变慢（其实是前面的 UPDATE 造出的删除更多）。改为游标构造时建一次 tombstone 集合。
7. **过滤扫描先物化再过滤**（`noindex_*` 的 5.0x / 8.0x）：列式扫描现在把「单列比较」的谓词压到块的字节上
   （`ColumnScan`，1–8 字节读 + 一次比较），只有命中的行才解码成 `Value`。10 万行点查/范围查询从
   3.5 ms / 8.1 ms 降到 0.61 ms / 0.80 ms（参考 0.68 / 1.00）。
8. **DML 的扫描与取行**：`UPDATE`/`DELETE` 先用同一套边界扫描拿行号（`row_ids_in_bounds`），再按块批量取行
   （`BlockCursor::rows`，同一块只解码一次）。
9. **提交路径的 O(n²)**：`commit_at_deferred_with` 对每个删除键做一次 `deferred_deletes.contains(key)`
   线性查找 —— 5 000 行 DELETE 就是 2 500 万次字符串比较。改成一次集合查找（全部延迟删除时直接跳过），
   DELETE 从 40 ms 降到 4.4 ms。
10. **索引喂入按批**：`update_indexes_on_insert_text` 每行都做一次 `table_key`（格式化+小写分配）、一次
    表锁、每个索引一次 schema 线性查找，调用方还先给整行渲染 `Vec<String>`。`UPDATE` 改走
    `update_secondary_indexes_batch`（键与列位置一次解析，只渲染被索引的那一格，varchar 直接用原字符串），
    并在表上没有全文/向量索引时跳过那两个逐行钩子。
11. **内存尾巴换成哈希表**：`SecondaryIndex` 的尾巴原本是 `BTreeMap<Vec<u8>, …>`，在 10 万键时一次插入要
    走 ~17 层 `memcmp`（profile 里 `__memcmp_avx2_movbe` 占 15%）。改成 `HashMap`：读者要么点查、要么全扫，
    顺序无人依赖；chunk 落盘自己排序，文件字节不变。UPDATE 的索引边际成本因此落到噪声（3 个索引 17.0 ms
    vs 不带索引 16.9 ms）。

## 5 剩余的差距

- `UPDATE`（不含索引）1.62x、含 3 个索引 2.42x：重写行要构造 WAL 的 `AppendV2` 载荷（`Value` → `Literal` →
  列式块）并逐行更新簿记；索引的边际成本已经落到噪声（0.1 ms / 5 000 行 / 3 个索引）。
- 建索引 1.7–1.8x，绝对值 53–60 ms（10 万行）。