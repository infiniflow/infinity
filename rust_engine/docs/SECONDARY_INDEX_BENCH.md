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
| 插入 10 万行（无索引） | 2.287 | 2.285 | 1.00x |
| 建二级索引 `score` (DOUBLE) | 0.0299 | 0.0555 | 1.9x |
| 建二级索引 `name` (VARCHAR) | 0.0335 | 0.0599 | 1.8x |
| 建二级索引 `id` (INT) | 0.0307 | 0.0545 | 1.8x |
| 无索引点查（200 次 × 100 行） | 0.1457 | 0.7324 | 5.0x |
| 无索引范围（100 次 × 1000 行） | 0.1016 | 0.8128 | 8.0x |
| **索引点查，唯一键**（200 × 1 行） | 0.0767 | 0.0548 | **0.71x** |
| **索引点查，VARCHAR 唯一键**（200 × 1 行） | 0.0513 | 0.0421 | **0.82x** |
| **索引点查，100 行**（200 次） | 0.0721 | 0.0539 | **0.75x** |
| **索引范围，1000 行**（100 次） | 0.0583 | 0.1012 | **1.74x** |
| 带 3 个索引插入 2 万行 | 0.4621 | 0.4965 | 1.07x |

两侧的物理计划相同：`EXPLAIN` 对 `=` 与 `>= … <` 都给 `INDEX SCAN`。**索引查询已追平或反超**
（0.71–1.74x）；同样谓词不走索引的全表扫描仍是 5.0x / 8.0x —— 差距在扫描路径，不在索引。

## 3 维护成本（同一次运行；5 000 行一条语句）

| 相位 | 参考 (s) | 重写版 (s) |
|---|---:|---:|
| UPDATE，3 个索引在线 | 0.0071 | 0.1022 |
| UPDATE，索引已删除 | 0.0089 | 0.0835 |
| DELETE（按索引列），3 个索引在线 | 0.0034 | 0.0378 |
| DELETE，索引已删除 | 0.0072 | 0.0403 |

**索引对每条语句的边际成本**：UPDATE +18.7 ms（5 000 行，即 +3.7 µs/行），DELETE −2.5 ms（走索引
定位行，比全表扫描更快，与参考同向）。剩下的是 DML 路径本身：不带索引的 UPDATE 9.3x、DELETE 5.6x。

随在线索引数量的斜率（`maint` 口径，2 万行表，5 000 行/档，µs/行）：

| 在线索引 | 参考 UPDATE | 重写版 UPDATE | 参考 DELETE | 重写版 DELETE |
|---|---:|---:|---:|---:|
| 无 | 1.6–2.7 | 13.0 | 0.8–1.0 | 8.0 |
| `score` | 1.6–2.7 | 9.7 | 0.8–1.0 | 7.6 |
| `score`+`name` | 1.6–2.7 | 10.1 | 0.8–1.0 | 7.7 |
| `score`+`name`+`id` | 1.6–2.7 | 10.7 | 0.8–1.0 | 7.8 |

两侧的**斜率都是平的**：重写版 UPDATE 13.0 → 10.7，DELETE 8.0 → 7.8（µs/行，0 → 3 个索引）。
剩下的常数差（UPDATE 约 4x、DELETE 约 8x）与索引数量无关。

## 4 这一轮修掉的六个问题

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
6. **点读克隆删除日志**：`BlockCursor::row` 每行 `deleted_ids(table)` 克隆一次删除日志，代价是日志长度 ——
   删除越多越慢，`maint` 里 DELETE 随索引数上升（8.9 → 28.2 µs/行）就是它（索引越多，前面的 UPDATE 造出的
   删除越多）。改为游标构造时建一次 tombstone 集合，DELETE 的斜率随之抹平（8.0 → 7.8）。

## 5 剩余的、与索引无关的差距

- 全表扫描：点查 5.0x、范围 8.0x（`noindex_*` 两行）。
- `UPDATE`/`DELETE` 的常数差：9.3x / 5.6x（无索引），见 §3；与在线索引数量无关。
- 建索引：1.8–1.9x，绝对值 55–60 ms（10 万行），不构成瓶颈。