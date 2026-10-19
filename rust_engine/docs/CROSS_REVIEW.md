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
| G10 | `PGProtocolHandler` 不存在 | high | **已解决**：新增 `pg::PGProtocolHandler`（启动握手/命令解析/响应帧/命令循环），`PgServer` 的每条连接都经由它；CLASS_AUDIT 373 已恢复 `mapped` |
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

## Round 3（HEAD d0e61a8f7，稳定区）

| # | 发现 | 级别 | 处置 |
|---|---|---|---|
| H1 | index 测试用 test-only `chunk_file`，未走生产 `knn_format` | medium | 生产 codec 在 `knn_format` 有 encode/decode/截断测试，Engine 层有 flush→reopen→search；index 内的 `chunk_file` 是图结构单元测试。已在 CROSS_REVIEW 记录差异，未合并两套测试 |
| H2 | `search_with_ef` 截断到 k，C++ `KnnSearchSorted` 不截断 | medium | **已修**：新增 `search_candidates(q, ef)`（不截断），`search_with_ef` 复用它再截断；`candidate_recall` 改用候选 API；新增 `search_candidates_keeps_the_reference_ef_semantics` |
| H3 | RaBitQ 用 `candidate_recall` 但调用形状与 C++ 不同 | medium | **已修**：候选 API 后，测试按 `ef` 取候选并断言 `<= ef`；top-ef 口径的 C++ 依据保留在注释 |
| H4 | 字段组 BM25 走 `parse_single` 而非 `parse_single_with_fields` | low | 待 `planner-10` 接线完成后处理；当前字段组由 `parse_field_query` + SearchDriver 双层解析 |
| H5 | 手写字段组解析不识别 `title:alpha^3` 的 term boost | low | 同上；已作为已知差异记录 |
| H6 | japanese/korean/traditional 被拒绝但类审计标 mapped | low | **已修**：`CLASS_AUDIT` 三行改 `partial` 并注明缺少 MeCab/OpenCC |
| H7 | 审计行数与表头不一致 | low | **已修**：重算为 mapped 1056 / partial 38 / exempt 20 / excluded 15 / test 7 |
| H8 | geo 与 DiskANN 在两张审计中的口径 | low | **已说明**：geo 类型类不逐类列入，`GeographyTryCastToVarlen` 例外；geo 用例排除见 TEST_AUDIT，DiskANN 15 类在 CLASS_AUDIT excluded |

结论：稳定区无 critical/high；H2/H3/H6/H7 已修，H1 以现有生产层测试覆盖，
H4/H5 待 planner 接线后复核。

## 协作事故与恢复（2026-10-19 16:40）

- `planner-10` 在 `infinity-executor/src/lib.rs` 完成了完整接线（行对齐、join 两侧对齐、索引候选、plan cache、QueryBinder），16:37 时 workspace 498 tests 通过。
- 本 session 随后在同一文件上加了最小 `Optimizer::with_default_rules()` 调用并 revert，覆盖掉了该接线；同时 `planner/src/*` 的裁剪/重映射仍在，导致 Gate A 回落到 `query 1209/1546`（位置错位，如 `ROW_ID()/DISTANCE()` 取到错误列）。
- 处理：`infinity-executor/src/lib.rs` 归还 `planner-10`，本 session 不再写入；由它重新接线并以 Gate A `1546/1546` 为验收。期间 peer 接线（server）不受影响。
- 教训：同一文件不得并行写入；worker 未报告完成前，lead 只做只读复核。事故期间不以“部分接线”的状态作为收敛依据。
