# Codex × CodeBuddy 交叉评审记录

> 每轮由独立 codex reviewer（`codex exec --sandbox read-only`，Orca worker 因
> `agent-update-prompt` 无法启动）审查 `rust_engine` 的移植代码，本 session
> 逐条复核、修复或登记为 `partial`/设计变更。收敛条件：没有未处置的
> critical/high 发现，且 `CLASS_AUDIT`/`TEST_AUDIT` 不再有“有类无调用”或
> 不可追溯的映射。

## Round 1（HEAD 93db60e59 → 36755f7ec）

| # | 发现 | 级别 | 处置 |
|---|---|---|---|
| F1 | 标量/EXISTS 子查询求值为 NULL/false | critical | 已实现（`subquery_cte_ut` 11 例）；相关子查询仍不支持，见 Round 2 |
| F2 | AnalyzerPool 只构造 rag/standard/ngram | major | 已补齐可构造家族 + 29 名表驱动测试；japanese/korean/traditional 因缺 MeCab/OpenCC 拒绝 |
| F3 | HNSW 测试 top-10、roundtrip 不落盘 | major | plain/LVQ 改严格 top-1；新增写盘→重开测试；RaBitQ 保留参考 top-ef 并附 C++ 依据 |
| F4 | 23 个优化器规则类空映射 | major | 先标 `gap`，后实现并在 Round 2 处理接线 |
| F5 | peer/network 类无实现 | major | 同上 |
| F6 | CTE 未绑定、非字面 LIMIT 被忽略 | major | 已实现 CTE（别名/链式/遮蔽）与常量 LIMIT/OFFSET |
| F7 | 审计统计自相矛盾 | major | 计数重算；native 替代实现单独注明 |
| F8 | `^boost` 不参与打分、字段组按 token 数计分 | major | boost 进入查询树并乘入 BM25；字段组走 BM25 路径 |

## Round 2（HEAD 8509b2f49）

| # | 发现 | 级别 | 复核结论 |
|---|---|---|---|
| G1 | 生产路径完全绕过新优化器规则 | critical | 属实（22/23 个类模块外零引用）；`planner-10` 接线中，未接线前保持 `partial` |
| G2 | 相关子查询按常量求值 | critical | 属实；解析器丢弃表限定符，无法可靠判定相关列。登记为限制，`SubqueryUnnest` 保持 `partial`，不声称等价 |
| G3 | peer 为进程内模拟、无 Thrift/TCP | critical | 属实（设计选择）；`peer-10` 接入 `Server` 并让 `SHOW CURRENT NODE` 可观测，相关类保持 `partial` |
| G4 | `SubqueryUnnest` 返回 mark 列而非子计划首列 | high | 属实；要求 `planner-10` 修正 |
| G5 | 相关 IN/NOT IN 缺 mark/否定语义 | high | 属实；随相关子查询一并登记为限制 |
| G6 | 相关标量/聚合硬编码 `ColumnBinding(0,0)` | high | 属实；同上 |
| G7 | `DependentJoinFlattener` 缺逐节点相关表 | high | 属实；随相关子查询登记 |
| G8 | `ColumnRemapper` 仅套用外部位置映射 | high | 属实；保持 `partial`，待接入真实重映射 |
| G9 | `LazyLoad` 只收集元数据 | high | 属实；文档注明 analysis-only |
| G10 | `PGProtocolHandler` 不存在 | high | 已修正映射（`pg::handle` + `PgServer`），该行 `partial` |
| G11 | `RegexIndexRewrite` 输出 `index=` 而非 `column@index` | medium | 要求 `planner-10` 修正 |
| G12 | `ResultCacheGetter` 键/白名单不符 | medium | 要求 `planner-10` 修正 |
| G13 | `IndexScanBuilder` 候选未被执行器消费 | medium | 随 G1 接线处理 |
| G14 | 全文 evaluator 未实现参考语法/MSM | medium | 保持 `partial`，登记待补 |
| G15 | 字段组 BM25 未走 `parse_single_with_fields` | medium | 待复核（boost/短语解析差异） |
| G16 | HNSW 生产 chunk 路径未测试 | medium | **不成立**：`cpp_blocks.rs` 的 `hnsw_index_mirror_roundtrips_the_vectors` 与 `a_persisted_hnsw_index_answers_vector_search` 已覆盖生产 writer + flush/reopen/search |
| G17 | HNSW top-1 与 RaBitQ 指标不同 | medium | 部分不成立：RaBitQ 的 top-ef 与 C++ `KnnSearchSorted` 不截断一致，测试注释已说明 |
| G18 | `PeerClient` 为拉取式 | medium | 属实；进程内模拟范围内注明 |
| G19 | `SHOW CURRENT NODE` 启动身份硬编码 | medium | 已改由 `NodeInfo` 驱动；新增 `show_current_node_reflects_the_peer_manager` |
| G20 | `RingBufferIterator` 溢出处理差异 | medium | 可接受偏差，已在代码注释说明 |

## Round 3（进行中）

范围限定为无 worker 编辑的稳定区：HNSW 生产持久化与指标、analyzer 家族缺口、
字段组 boost 解析、审计一致性、`TestLoad` 语义。结果追加到本表。
