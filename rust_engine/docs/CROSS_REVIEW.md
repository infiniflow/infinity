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

## Round 4（HEAD bab0f5ef9，收敛检查）

| # | 上轮遗留 | 结论 |
|---|---|---|
| G1 | 生产 planner 未接入优化器规则 | **已解决**：`cc1843436` 接入 `plan_select`/`run_select`（QueryBinder、ColumnPruner/Remapper、LazyLoad、FastRoughFilter、IndexScanBuilder + evaluators、RegexIndexRewrite、SubqueryUnnest），`530b11691` 修正 residual 谓词挂载到 base scan；有 plan-shape 测试。Gate A 1546/1546、executor 503 tests |
| G2 | 相关子查询按常量求值 | **已解决（显式拒绝）**：`bind_subquery` 检测到 correlated_columns 后调用 `SubqueryUnnest`，随后返回 `correlated subqueries are not supported`，不再静默求值；`CLASS_MAP` 记录为限制 |
| G3 | peer 仅进程内传输 | **已明确登记**：11 个类经 `PeerManager` 挂到 `Server` 并有服务端测试；`CLASS_AUDIT` 的备注写明“进程内 InProcessPeerTransport，无 Thrift/TCP 线路”，不声称 wire-compatible |
| G4-G9 | SubqueryUnnest/IN/DependentJoinFlattener/ColumnRemapper/LazyLoad | 随 G1 接入执行路径；未被 C++ 覆盖的 correlated 执行路径以显式错误代替 |
| G10 | PGProtocolHandler 不存在 | **已解决**：`pg::PGProtocolHandler` 实现并路由每条连接，行 373 mapped |
| G11-G14 | Regex 输出、ResultCache 键、IndexScanBuilder 消费、全文 evaluator | Regex 已在 IndexScanBuilder 内接线；IndexScanBuilder 由执行器直接调用（规则无法携带 evaluator）；ResultCacheGetter 保留 API + 单测，执行器继续用语句级缓存；全文 MSM/语法差异保留为 `partial` 注记 |
| G16/G17 | HNSW 生产 codec、top-ef | G16 由 `knn_format` + `cpp_blocks` 生产路径测试覆盖；G17 已加 `search_candidates` 保持 C++ 不截断语义 |

**未接线但已登记**：`IndexScanBuilder` 作为 `OptimizerRule`（执行器直接调用，避免丢谓词）、`ResultCacheGetter` 默认管线（语句缓存已覆盖，plan-level 需写路径失效钩子）、块级 rough filter（无 block stats 层）、相关子查询执行（无 mark join 算子，显式拒绝）。

## Round 5（HEAD 563e07e10 + 本轮修复，收敛检查）

| # | 发现 | 级别 | 处置 |
|---|---|---|---|
| R5-1 | G2 新测试只断言去重后剩一个相关列，若 `s.a` 被误绑定为外层也只有一个（与 `o.a` 去重），无法区分 | medium | **已修**：`a_qualified_outer_reference_is_not_shadowed_by_a_local_column` 现在直接断言谓词两侧：`s.a` = `(@0, 0, depth 0)`、`o.a` = `(@0, 0, depth 1)` |
| R5-2 | 限定名为子查询自身表（`s.a`）的本地绑定分支无测试 | medium | **已修**：新增 `a_qualified_local_reference_binds_to_the_subquery_scan`，同时覆盖 `db.s.a` 的三段限定符归一化 |
| R5-3 | `plan_select` 可能产出带 `Expr::Subquery` 的不可执行计划 | medium | **部分不成立**：`infinity_expression::Expr` 没有 `Subquery` 变体。但确认了真实边界：未解析的 `EXISTS`/`SUBQUERY` 会编译成未知函数，而未知函数求值为 `NULL`（静默 false）。**已修**：`ExprAst/Predicate/SelectItem::contains_subquery`（parser）在 `plan_select` 入口拒绝，报 `subqueries must be resolved before planning`；执行器的三个同名 helper 改为复用这套方法 |
| R5-4 | Gate A 缺少 `statement error` 行时 `frac` 退回 `(0,0)`，比较退化为恒真 | low | **已修**：三个计数器任一缺失直接失败，且要求三者总数 > 0 |
| R5-5 | `CLASS_MAP` 仍把 subquery 类标 `✅`、把 peer 写成 `partial`，与 `CLASS_AUDIT` 不一致 | low | **已修**：subquery 行改 `⚠️ partial`，`ResultCacheGetter` 注明仅测试接入；「已知限制」重写为“解析器保留限定符 + binder 检出 + 执行器显式拒绝”，peer 说明与 `CLASS_AUDIT` 的 `mapped` 一致，且都不宣称线路兼容 |

**本轮顺带修掉的真 bug（评审未报，由 R5-3 的边界排查暴露）**：执行器 `bind_subquery` 与 `outer_columns` 都用裸表名查 catalog，而 catalog 的键是 `db.table`，导致 `table_schema("cs2")` 恒失败、`bind_subquery` 恒走 `Ok(())` 提前返回——相关子查询检测在 SQL 路径上从未真正跑到过。改为 `table_key` 归一化后，`a_correlated_subquery_is_rejected_instead_of_evaluated_as_a_constant` 端到端证明：未相关 `EXISTS` 正常返回全部行，相关 `EXISTS` 报 `correlated subqueries are not supported`。

验证：`cargo fmt --all -- --check`、`cargo clippy --workspace --all-targets -- -D warnings`、`cargo test --workspace --lib`（全绿）、Gate A `statement ok 1752/1752, statement error 129/129, query 1546/1546`。

结论：**converged**——无未处置的 critical/high。

## Round 6（HEAD a215f17cf，收敛检查）

评审在 `77a0bd0ee` 落盘前开始，因此结论钉在 `a215f17cf`；其中唯一 high 正是本轮已自查并修复的 CTE 遮蔽问题。

| # | 发现 | 级别 | 处置 |
|---|---|---|---|
| R6-1 | `bind_subquery` 用 `table_key` 归一化裸表名后，遮蔽基表的 CTE 别名会被当作基表绑定（换掉静默空转，换来错误 schema） | high | **已修** `77a0bd0ee`：`run_subquery` 携带 CTE 作用域，FROM 是 CTE 别名时跳过绑定。回归测试 `a_cte_alias_shadowing_a_table_is_not_bound_as_that_table`；去掉跳过即报 `correlated subqueries are not supported`，已实测 |
| R6-2 | 外层查询的 FROM 是 CTE 别名时 `outer_columns` 同样按基表查，相关列作用域为空/错误 | medium | **已修**：外层作用域优先取 `cte_scope` 的列。测试 `an_outer_cte_supplies_the_correlation_scope`（修复前报 `unknown column 'cs.id'`，修复后报 `correlated subqueries are not supported`） |
| R6-3 | `SEARCH MATCH (…) WHERE …` 的过滤器由搜索路径直接编译，既不经过 subquery 解析器也不经过规划器守卫，`EXISTS` 会当未知函数求值成 NULL（静默空结果） | medium | **已修**：`resolve_stmt_subqueries` 现在同时重写每个 match 的 args 与 filter（`search_has_subquery` 判定是否需要克隆）。测试 `a_subquery_in_a_match_filter_is_not_folded_to_a_constant`（修复前 `unwrap_err()` 直接 panic，说明返回了 Ok——正是静默折叠） |
| R6-4 | `SelectItem::Func` 只查参数、不查函数名 | low | **已修**：先判 `SUBQUERY`/`EXISTS` 名 |
| R6-5 | `run_gate_a.sh` 用命令替换取输出，忽略 `cargo run` 退出码 | low | **已修**：记录 `$?`，非 0 直接失败 |

验证：`cargo fmt --check`、`clippy -D warnings`、`cargo test --workspace --lib`（executor 508 tests）、Gate A `1752/1752 + 129/129 + 1546/1546`。

**Round 6 结论：not converged（1 high）**，该 high 与 4 条 medium/low 已在本节全部处置；R7 复核确认。

## Round 7（HEAD 0d9d07243，收敛检查）

| # | 发现 | 级别 | 处置 |
|---|---|---|---|
| R7-1 | `SEARCH` 选项串里的 `filter=…` 是事后由 `parse_filter_predicate` 解析的不透明文本，新解析器覆盖不到；其中 `EXISTS` 会当未知函数求值成 NULL，静默丢弃所有匹配 | high | **已修**：`parse_filter_predicate` 对含子查询的过滤器显式报 `subqueries are not supported in a search filter`。测试 `a_subquery_in_a_search_option_filter_is_rejected`；去掉拒绝后实测返回 `Rows{rows: []}`（静默空结果），正是该 bug |
| R7-2 | `expand_views` 只认本语句的 CTE 列表，外层 `WITH` 绑定的别名可能被当作同名视图展开 | high | **未复现**：视图存于独立的 `view:` 键空间且名字是 `db.view`，而 CTE 别名在 `run_select_with_ctes` 路径中保持未限定，`get_view_by_name("cte")` 恒查不到。仍把 `inherited` CTE 作用域传给 `expand_views`，与 `qualify_tables_in` 的口径对齐，不再依赖这个「名字没归一化」的巧合。测试 `an_inherited_cte_alias_is_not_expanded_as_a_view`（改前改后都通过，故不作为回归证据） |
| R7-3 | 子查询自带 `WITH`（`EXISTS (WITH x AS … SELECT …)`）的别名不在 `from_is_cte` 检查范围，仍会按同名基表绑定 | medium | **已修**：`from_is_cte` 同时看子查询自身的 `ctes`。测试 `a_subquery_local_cte_shadows_a_base_table` |
| R7-4 | 解析器里重复的注释块 | low | **已删除** |

验证：`cargo fmt --check`、`clippy -D warnings`、`cargo test --workspace --lib`（executor 511 tests）、Gate A `1752/1752 + 129/129 + 1546/1546`。

**Round 7 结论：not converged（2 high）**；其中 R7-1 为真实高危并已修复，R7-2 未能复现但仍做了口径对齐。

## Round 8（HEAD 75907c2d9，收敛确认）

只审 `75907c2d9` 的 diff 及其触及的函数。

| 项 | 结论 | 证据 |
|---|---|---|
| A：新拒绝是否误杀正常过滤器 | **无问题** | `parse_filter_predicate` 用 `Predicate::contains_subquery`，仅对名为 `EXISTS`/`SUBQUERY` 的 `Func` 为真；解析器只在这三处产出这些名字（`IN (SELECT …)` 等）。没有同名标量函数。向量/knn 的 `filter=` 走同一函数 |
| B：子查询自带 `WITH` | **无问题** | `from_is_cte` 用大小写不敏感的别名匹配并入 `local_ctes`；真实基表仍走 `bind_subquery`。跳过绑定只丢掉相关列检查，未被识别的外层限定符会由 `resolve_output_column` 报错，不会静默成 NULL |
| C：`expand_views` 新参数 | **行为中性** | `&[]` 时 inherited 分支为死分支，非 CTE 调用点不变；只有子查询与 CTE 查询传入非空作用域，与 `qualify_tables_in` 一致 |
| D：新引入的 critical/high | **无** | 仅改三个函数、删一段注释、加测试 |

遗留一条 low（不阻塞，未改）：`from_is_cte` 与 `run_select_with_ctes` 内的 `cte_scope` 判断有重复，无害。

**Round 8 结论：converged——无未处置的 critical/high。循环收敛于 `75907c2d9`。**
