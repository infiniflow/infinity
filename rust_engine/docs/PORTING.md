# 移植纲领

`rust_engine` 是 C++ 参考引擎（`src/`）的 Rust 重写版。本文是这次移植的纲领：**目标与判据**（§1）、**方法论**（§2）、**当前状态**（§3）、**当前缺口**（§4），以及把整件事拆成的**子目标**（§5）——每个子目标在 `docs/` 下有一份台账，记录它此刻的状态。

里程碑与排期、决策矩阵（D1–D6）、Gate 的定义、风险登记册在仓库根的 [`RUST_REWRITE_PLAN.md`](../../RUST_REWRITE_PLAN.md)，本文不重复，只记录"现在是什么样、为什么这样判"。

---

## 1 目标

### 1.1 完成的定义

三条同时成立才算移植完成：

1. **格式等价**——同一输入下，两个引擎写出的 `data/`、`catalog/`、`wal/` 逐字节相同（参考自身不确定的部分按内容比，清单见 `FORMAT_EQUIVALENCE_TESTS.md` §5）。
2. **行为等价**——同一 SQL / 协议输入，两个引擎的回答相同：`.slt` 全量（Gate A）、SDK 与 PG wire（Gate B）、重启与崩溃恢复（Gate C）、并发（Gate D）。
3. **可交接**——任一引擎能打开对方留下的目录继续读写，参考能 replay 重写版写的 WAL，重写版能从参考的 WAL 恢复。

判据是**参考**，不是"能跑通"。重写版自己前后一致、或者两版都能读同一份数据，都还不够：`data/` 里一个字节的差别意味着某处的布局、对齐、排序或填充与参考不同。

### 1.2 等价的三个层级

| 层级 | 断言 | 用途 |
|---|---|---|
| 字节 | 同一输入，两引擎写出的文件逐字节相同 | 最强的格式判据，能看见任何布局漂移 |
| 内容 | 不可字节比的文件（参考随机、未初始化内存）解码后比"索引说了什么" | HNSW 图、IVF 质心、catalog 字节 |
| 行为 | 两引擎回答同一批查询的结果相同 | 语义等价，但不保证格式 |

门禁按这个顺序排：能字节比的绝不只做内容比，能内容比的绝不只做行为比。三层各自的边界与"为什么只能到这一层"登记在 `FORMAT_EQUIVALENCE_TESTS.md` §1、§5。

### 1.3 非目标

- **不移植**：DiskANN、geo 列类型、远程对象存储的 `obj_key` 打包上传（`GAPS.md` §1）。
- **不复刻参考的不确定性**：HNSW 的 `thread_local` 随机图、IVF 的 `random_device` 采样、未初始化的 padding，按"参考实际写出什么"写或按内容比，不追求逐字节相等。
- **不"顺手改进"**：与参考逐行对应的实现不改写成 Rust 惯用法；确需设计变更（调度器、FST、SIMD 原语、RocksDB 事务型 KV）必须登记在 `CLASS_MAP.md`「有意的设计变更」，并说明为什么可以不等价。

---

## 2 方法论

### 2.1 参考是唯一真值，而且必须实测

- 任何格式结论都要能追到 `文件:行号`（`CPP_FORMAT_SPEC.md` 的规矩），不得凭推测实现；没核对的进"待核对清单"。
- **不拿黑盒拟合**：`.slt` 的期望值、SDK 的行为都是跨 revision 生成的黑盒，对不上时先去量参考（容器内构建 C++ 引擎，用 `psql`/SDK 取真值），不要调常数迁就。Gate A 从 1478 推到 1536/1536，靠的是量出 tokenizer 的逐字节分类，而不是拟合 `len/avg`。

### 2.2 先冻结格式，再写实现

磁盘格式（列块、`version`、outline、倒排三件套、catalog 键空间、WAL 帧）先写成带行号引用的规格（`CPP_FORMAT_SPEC.md`），再照规格实现。反过来做——先写实现、再对齐字节——会把"实现细节"固化成"格式"，且每次差异都要重新读参考源码。

### 2.3 台账驱动：不让"看起来实现了"蒙混

- `CLASS_AUDIT.md`：参考每个 `export class/struct` 一行，`mapped` / `partial` / `test` / `exempt` / `excluded`。
- `TEST_AUDIT.md`：参考每个 `TEST/TEST_F/TEST_P` 一行，`ported` / `excluded` / `exempt`。
- `CLASS_MAP.md`：关键类与方法的一一对应，以及登记过的设计变更。

`partial` 不是"差不多"，是"生产路径只走到一部分"，必须写清缺哪一段。三份台账是"覆盖"的唯一口径：没有行的功能不算覆盖，没有对应测试的类不算移植。

### 2.4 门禁分层，修一个差异沉一条测试

每个差异的收口动作固定：**先让门禁红**（能复现的用例）、**再改代码**、**最后让门禁绿**并留下那条用例。门禁分五档（SQL `.slt`、SDK/协议、重启与恢复、并发与稳定性、性能），格式与不变量另有两条（`FORMAT_EQUIVALENCE_TESTS.md`、`INVARIANT_MATRIX.md`），跑法见 §6。

单测与门禁的分工：单测钉住**机制**（一个函数的边界、一个布局的字节），门禁钉住**整体等价**（整目录、整协议、整恢复）。两者都要——缺一个就会出现"单测全绿，但目录读不出来"。

### 2.5 文档只写现状

文档描述"参考怎么工作、重写版怎么对应、哪里不同、怎么验证"。移植过程、修复顺序、评审轮次不写进文档，需要时看 `git log`。差异用对比表；缺口只写"未覆盖什么 + 为什么"，不写"原先如何、现已修复"。

---

## 3 当前状态

| 维度 | 状态 | 验证 |
|---|---|---|
| 磁盘格式（`data/`、`catalog/`、`wal/`） | 块列、`version`、索引 chunk、catalog 键值、WAL 帧（含 `txn_id` 与 CRC）与参考逐字节相同 | `python/restart_test/test_cross_engine_bytes_*.py`（按层分文件，见 `FORMAT_EQUIVALENCE_TESTS.md` §3） |
| WAL | `wal.log` 的文件名、checkpoint 换文件、帧头、帧体全部一致；两引擎可互相 replay | 同上 + `test_cross_engine_compat.py` |
| 跨引擎交接 | 参考读重写版的目录、重写版读参考的目录，数据与各索引的答案一致 | `test_cross_engine_compat.py` |
| SQL 行为 | `.slt` 全量一致（Gate A）；SDK/协议（Gate B）、重启/恢复（Gate C）由对应脚本驱动 | `rust_engine/scripts/run_gate_{a,b,c}.sh` |
| 内部不变量 | 配置校验、存储/事务、索引三组不变量逐条有回归测试（Gate G） | `INVARIANT_MATRIX.md` |
| 已知缺口 | 见 §4 | — |

---

## 4 当前缺口

唯一的缺口总表是 [`GAPS.md`](GAPS.md)，分四类：

| 类别 | 现在有什么 | 是否待办 |
|---|---|---|
| §1 有意不移植 | 对象存储 `obj_key` 打包、DiskANN、geo 类型 | 否，是决定 |
| §2 未实现 / 未接线 | **当前为空**：相关子查询、压缩/IMPORT 的 WAL 帧、`IndexScanBuilder`、`ResultCacheGetter`、`MATCH` 走索引、`fast_rough_filter`、分析器家族、稀疏检索选项、快照均已接线并有门禁 | — |
| §3 不可逐字节比对 | HNSW 图区、IVF 质心、`catalog/` 字节、NULL slot 与内联 padding、后台 dump 的 chunk 切分 | 否，参考自身不确定 |
| §4 覆盖不到的输入空间 | 带索引的 8M 行表、三段以上的容量切分、PLAID/EMVB/MULTIVECTOR chunk 字节 | 是，但代价高 |

已登记的性能/口径差异（相关子查询的作用域边界、`CREATE TABLE … AS SELECT`、视图、`INSERT … SELECT`、`standard-porter` 的词干）在 `GAPS.md` §5，不是待办。

---

## 5 子目标与步骤

移植拆成下面这些子目标。每个子目标一份台账，记录它**此刻**的状态（不是历史）；本文只留一句摘要，细节在各自的文件里。

| # | 子目标 | 台账 | 记录的状态 | 什么时候读 |
|---|---|---|---|---|
| S1 | 冻结磁盘格式 | [`CPP_FORMAT_SPEC.md`](CPP_FORMAT_SPEC.md) | 数据文件、`version`、列体、outline、倒排三件套、次级/稀疏索引、WAL、catalog 键的字节级规格，逐条带 `文件:行` | 要动格式，或想知道某段字节为什么长这样 |
| S2 | 做到逐字节等价 | [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) | 三层口径的边界、门禁的分层与覆盖矩阵、格式层面的例外及原因、复现步骤 | 跑/改/新增格式门禁 |
| S3 | 类级覆盖不漏 | [`CLASS_AUDIT.md`](CLASS_AUDIT.md) | 参考每个 `export class/struct` 的映射状态；`partial` 说明缺哪一段 | 查某个类有没有对应 |
| S4 | 用例级覆盖不漏 | [`TEST_AUDIT.md`](TEST_AUDIT.md) | 参考每个 `TEST/TEST_F/TEST_P` 在 Rust 的落点，或排除理由 | 查某个 C++ 单测对应哪条 |
| S5 | 结构与设计变更可追溯 | [`CLASS_MAP.md`](CLASS_MAP.md) | 类/方法的一一对应；「有意的设计变更」「尚未一一对应」「未移植单测」三张表 | 想知道某个 C++ 类型在 Rust 里是谁、某个差异是否故意 |
| S6 | 内部不变量有测试兜住 | [`INVARIANT_MATRIX.md`](INVARIANT_MATRIX.md) | C++ 不变量 → Rust 回归测试的对照；Gate G 的三组（配置、存储/事务、索引） | 想知道某条不变量由谁兜住 |
| S7 | catalog 底座与互操作 | [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) | 唯一后端（RocksDB）、与参考目录互读的条件、fsync 点 | 参考打不开目录、或想确认落盘点 |
| S8 | 缺口收口 | [`GAPS.md`](GAPS.md) | 唯一缺口总表：有意不移植 / 未实现 / 不可字节比 / 覆盖不到 | 想知道"还差什么" |

子目标之间的依赖：S1 是 S2 的输入；S2 的例外决定 S8 §3；S3/S4/S5 是"覆盖"的口径，S6 是"不变量"的口径；S7 是跨引擎互操作的前提（两引擎共用同一个 catalog 键空间）。

---

## 6 怎么跑

格式门禁（需要参考二进制 `cmake-build-reldeb/src/infinity`）：

```bash
cd rust_engine && cargo build --release -p infinity-cli
cd ..
.venv/bin/python -m pytest python/restart_test/test_cross_engine_bytes_*.py \
                              python/restart_test/test_cross_engine_compat.py -q
```

这两组用例把两个引擎的根目录都放在 `/var/infinity/bytes`，并且会用参考再读一次目录；非 root 用户需要可写的 `/var/infinity`（`scripts/run_gate_c.sh` 用 `bwrap` 提供，且必须是磁盘目录，不能是 tmpfs）。

单元测试：

```bash
cd rust_engine && cargo test --release --workspace --lib
```

`scripts/run_gate_{a,b,c,d,e}.sh` 分别对应 SQL、SDK/协议、持久化、并发、性能关卡；用法与各自最近一次实测结论（表内标日期）见 `rust_engine/scripts/README.md`。
