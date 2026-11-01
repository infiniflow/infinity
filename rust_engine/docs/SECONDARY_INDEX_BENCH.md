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
| 插入 10 万行（无索引） | 2.258 | 2.285 | 1.01x |
| 建二级索引 `score` (DOUBLE) | 0.0318 | 0.0554 | 1.75x |
| 建二级索引 `name` (VARCHAR) | 0.0386 | 0.0606 | 1.57x |
| 建二级索引 `id` (INT) | 0.0309 | 0.0557 | 1.80x |
| 无索引点查（200 次 × 100 行） | 0.1441 | 0.1292 | **0.90x** |
| 无索引范围（100 次 × 1000 行） | 0.1017 | 0.0814 | **0.80x** |
| 索引点查，唯一键（200 × 1 行） | 0.0730 | 0.0561 | **0.77x** |
| 索引点查，VARCHAR 唯一键（200 × 1 行） | 0.0490 | 0.0420 | **0.86x** |
| 索引点查，100 行（200 次） | 0.0727 | 0.0538 | **0.74x** |
| 索引范围，1000 行（100 次） | 0.0591 | 0.0530 | **0.90x** |
| 带 3 个索引插入 2 万行 | 0.4644 | 0.5043 | 1.09x |

两侧的物理计划相同：`EXPLAIN` 对 `=` 与 `>= … <` 都给 `INDEX SCAN`。**所有查询形态（走索引与不走索引）
现在都持平或更快**（0.74–0.90x）：全表扫描的那两行过去是 5.0x / 8.0x，原因是扫描把每一行都解码成 `Value`
再过滤；现在谓词直接在块的字节上判（见 §4）。

## 3 维护成本（同一次运行；5 000 行一条语句）

| 相位 | 参考 (s) | 重写版 (s) | 比值 |
|---|---:|---:|---:|
| UPDATE，3 个索引在线 | 0.0079 | 0.0252 | 3.2x |
| UPDATE，索引已删除 | 0.0117 | 0.0195 | 1.7x |
| DELETE（按索引列），3 个索引在线 | 0.0040 | 0.0044 | **1.09x** |
| DELETE，索引已删除 | 0.0070 | 0.0094 | **1.35x** |

DELETE 已经与参考同级；UPDATE 还剩 1.7–3.2x（重写行的 WAL `AppendV2` 载荷与逐行簿记，见 §5）。

随在线索引数量的斜率（`maint` 口径，2 万行表，5 000 行/档，µs/行）：

| 在线索引 | 参考 UPDATE | 重写版 UPDATE | 参考 DELETE | 重写版 DELETE |
|---|---:|---:|---:|---:|
| 无 | 1.6–2.7 | 11.8–13.0 | 0.8–1.0 | 8.0–8.1 |
| `score` | 1.6–2.7 | 9.3–9.7 | 0.8–1.0 | 7.5–7.6 |
| `score`+`name` | 1.6–2.7 | 10.1 | 0.8–1.0 | 7.6–7.7 |
| `score`+`name`+`id` | 1.6–2.7 | 10.7–11.0 | 0.8–1.0 | 7.7–7.8 |

两条斜率都是平的（0 → 3 个索引）。`maint` 的绝对值比 `lookup` 高，是因为它按 `score` 逐值删除、
每档都命中 5 000 行且表只有 2 万行，固定开销摊不开。

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

## 5 剩余的差距

- `UPDATE`（不含索引）1.7x、含 3 个索引 3.2x：重写行要构造 WAL 的 `AppendV2` 载荷（`Value` → `Literal` →
  列式块）并逐行更新簿记；索引本身只占其中约 6 ms（5 000 行，3 个索引）。
- 建索引 1.6–1.8x，绝对值 55–61 ms（10 万行）。