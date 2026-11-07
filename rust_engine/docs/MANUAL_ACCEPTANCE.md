# 大规模手工验收：全文 33M 与 SIFT/GIST 1M

> 适用范围：Rust 重写版与 C++ 参考版在同一台机器上的**大数据量性能、内存和字节兼容**验收。
> 自动门禁的规模上限是 200 行 / 2 万行 / 15 万行，8M 行全文用例需要显式打开；本文补的是自动门禁不跑的两档：
> **enwiki 全文 33,332,620 行** 与 **SIFT/GIST 各 1,000,000 行**（LSP-HNSW 与暴力搜索），
> 以及这些档位下的峰值内存和跨引擎互读。
> 其他索引（二级、BMP、张量）不在本文做大数据性能测试，只做门禁规模的字节/行为兼容；
> 口径见 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) 与 [`GAPS.md`](GAPS.md)。

## 1 判定口径

### 1.1 三层判据

| 层级 | 断言 | 本文在哪用 |
|---|---|---|
| 结果一致 | 同一份数据、同一条 SQL，两端的 `ROW_ID` / `SCORE` / `count(*)` 相同 | 33M 全文、SIFT/GIST 1M |
| 互读 | A 写目录，B 打开后能查、能继续写，答案不退化 | 33M 全文、SIFT/GIST 的 LSP-HNSW |
| 字节 | 同一输入，确定性文件逐字节相同 | 门禁套件 + 8.39M 全文 HUGE 用例 |

字节判据只对**确定性文件族**成立：

| 文件/数据 | 判定方式 | 依据 |
|---|---|---|
| 块列 `*.col`、变长 outline、`version` | 逐字节 | 只由行值和行序决定 |
| 全文 `.dic/.pos/.len` | 逐字节 | 同上；chunk 边界由 `mem_index_capacity` 决定 |
| 稀疏 BMP chunk | 逐字节 | `ToBlockFwd` 已按稳定序写 |
| 二级索引 chunk（整数键、浮点键） | 逐字节 | PGM 拟合确定 |
| HNSW chunk | 头 + 向量 + 标签逐字节，**图区按内容/召回比** | 参考用 `thread_local std::mt19937` 建图，同输入多次运行图不同 |
| IVF chunk | 按内容比 | 参考用 `random_device` 采样 k-means |
| `catalog/`、`wal/` | 只比逻辑内容（`SHOW CATALOG 'all'`、键值、WAL 帧） | 带 session id、LSN、提交 ts，参考自己也不可复现；`wal.log` 帧本身在门禁里逐字节比 |
| 对象存储桶内布局 | 不参与 | 重写版不移植 `obj_key` 打包上传 |

### 1.2 通过阈值

正确性、互读、字节是**硬判据**：任何一条不一致就是 FAIL，没有“性能换正确性”。
性能/内存是**观察判据**，建议按下表记录并评审；阈值按同一台机、同一数据、同一驱动、同一轮运行比较。

| 指标 | 通过 | 需评审 | 说明 |
|---|---|---|---|
| 建索引耗时（Rust/C++） | ≤ 1.5× | > 1.5× | 绝对值 < 50 ms 时看区间，不看单点 |
| 查询耗时（Rust/C++） | ≤ 1.5× | > 1.5× | 单次 < 5 ms 的查询，psql 启动开销会主导，改用进程内基准 |
| 峰值 RSS（Rust/C++） | ≤ 1.5× | > 1.5× | `VmHWM`；同时记录 `RssAnon`/`RssFile` |
| 稳态 RSS（建完索引空闲 10 s） | ≤ 1.3× | > 1.3× | `VmRSS` |
| 索引落盘体积 | ≤ 1.05×（全文/二级/BMP）；HNSW ≤ 1.1× | 超出 | `du -sb`，同一行数 |
| 稳定性 | 3 次重复无 crash/OOM；kill -9 后可恢复 | 任一次失败 | 见 §9 |

**不要把不同报告的绝对值横向比较。** 全文 1M、HNSW 200k、二级 10 万行的数字各有各的数据集和配置；
本文只比较同一用例内 C++ 与 Rust 的比值。上表的 1.5× 是本文 33M/1M 用例的评审线，
不要拿它去判 20k HNSW（那个规模的服务端 exec 有 5.5× 的已知常数差，见 [`HNSW_BENCH.md`](HNSW_BENCH.md)；那份报告的构建参数也不等于本文的 LSP 口径）。

### 1.3 用例矩阵

| 编号 | 用例 | 数据 | 主要工具 | 主要指标 |
|---|---|---|---|---|
| FT-1 | 全文进程内基准（冒烟/中档） | enwiki 1M / 8.39M JSONL | `fulltext-bench` / `fulltext_benchmark` | import、index、query、长查询、RSS |
| FT-2 | 全文 33M 服务端主用例 | enwiki 33,332,620 行 TSV | psql + 两引擎服务端 | COPY、CREATE INDEX、查询、RSS、索引体积 |
| KNN-1 | SIFT 1M + LSP-HNSW | SIFT base 1M（128 维） | `tools/perf_sift.py --build-type lsg` | index_s、qps、p50/p95、recall@100、RSS |
| KNN-2 | GIST 1M + LSP-HNSW | GIST base 1M（960 维） | 同上 `--dataset gist` | 同上 |
| KNN-3 | SIFT 1M 暴力搜索 | SIFT base 1M | `tools/perf_sift.py --no-index` | qps、p50/p95、recall@100、RSS |
| KNN-4 | GIST 1M 暴力搜索 | GIST base 1M | 同上 `--dataset gist --no-index` | 同上 |
| BY-1 | 全索引字节/互读门禁 | 200 / 2 万 / 15 万行 | `test_cross_engine_*` | 字节、catalog、各索引查询 |
| BY-2 | 8.39M 全文逐字节 | 8,388,700 行 | HUGE 用例 | `.dic/.pos/.len` 逐字节 |
| BY-3 | 33M / 1M 互读与增量写 | 33M 全文、SIFT/GIST 的 LSP-HNSW | 两引擎服务端 / `knn-bench` | count、ROW_ID、recall、可继续写 |
| RC-1 | 大数据导入/建索引崩溃 | 33M（可选） | kill -9 + 重启 | 不出现部分导入、索引可重建 |

## 2 环境与前置

### 2.1 机器与容量

| 用例 | 原始数据 | 运行目录估算 | 建议空闲磁盘 | 建议内存 |
|---|---|---|---|---|
| 全文 33M（FT-2） | 约 30 GB TSV | 数据块约 30 GB + 全文索引约 40 GB（按 1.2 GB/1M 文档估） | ≥ 150 GB | ≥ 64 GB |
| 全文 1M/8.39M（FT-1） | 0.1–8 GB JSONL | 索引 1.2–10 GB | ≥ 50 GB | ≥ 32 GB |
| SIFT 1M（KNN-1/3） | 493 MB fvecs | 数据 + 索引约 3–5 GB | ≥ 20 GB | ≥ 16 GB |
| GIST 1M（KNN-2/4） | 约 3.8 GB fvecs | 数据 + 索引约 8–12 GB | ≥ 40 GB | ≥ 32 GB |

先量一次，不要按估算硬跑：

```bash
nproc
free -g
df -h .
git rev-parse HEAD
uname -a
```

33M 全文在 16 GB 内存的机器上不要跑：参考的建索引和查询都会把页缓存和索引结构抬起来，
先 OOM 再谈比值没有意义。

### 2.2 二进制

```bash
# 重写版：cli 包才产出服务入口，只编 server 库不会更新这个二进制
cd rust_engine
cargo build --release -p cli
ls -l target/release/infinity
cd ..

# 参考版：构建产物是参考真值，不要在这条路径上做 ln -sf / cp
ls -l cmake-build-reldeb/src/infinity
```

参考版如果不存在，用 CI builder 镜像重编（`.github/workflows/tests.yml` 的 `reldeb_amd64_benchmark_build`），
不要拿 debug 版和 release 版比。两端的构建类型必须写进记录：参考 `RelWithDebInfo`，重写版 `--release`。

### 2.3 依赖

| 工具 | 用途 | 检查 |
|---|---|---|
| `psql` | 走 PG 协议发 SQL、COPY、查询计时 | `command -v psql` |
| `bwrap` | 给参考/门禁一个可写的 `/var/infinity`，并隔离端口 | `command -v bwrap` |
| `.venv/bin/python` | SDK 驱动、pytest、门禁 | `.venv/bin/python -V` |
| `ss` / `pgrep` | 查端口、找引擎进程 | `ss -ltn` |
| `/usr/bin/time` | 粗测墙钟（不用它的 maxrss 代替引擎内存） | `command -v time` |

Python 环境按仓库根目录的 `uv` 环境来，不要用系统 python：

```bash
uv sync --python 3.11 --frozen --all-extras
```

### 2.4 数据

| 数据集 | 位置 | 行数 | 用途 |
|---|---|---|---|
| enwiki 33M | `$ENWIKI_33M`，默认 `benchmark/enwiki.33332620.csv` | 33,332,620 | FT-2 主用例 |
| enwiki 8.39M | `benchmark/enwiki.8388608.csv` | 8,388,608 | FT-1 中档、HUGE 字节门禁 |
| enwiki 10 万 | `test/data/benchmark/enwiki/enwiki-10w.csv` | 100,000 | 全流程冒烟 |
| SIFT 1M | `test/data/benchmark/sift_1m/` | 1,000,000 | KNN-1/3 |
| GIST 1M | `test/data/benchmark/gist_1m/` 或 `$GIST_DIR` | 1,000,000 | KNN-2/4 |

33M 文件不随 git 分发；仓库里登记的位置是 C++ 全文基准源码中的注释路径
`benchmark/enwiki.33332620.csv`（`fulltext_benchmark.cpp:443`）。现场用环境变量指向实际文件：

```bash
export ENWIKI_33M="${ENWIKI_33M:-$PWD/benchmark/enwiki.33332620.csv}"
test -f "$ENWIKI_33M" || { echo "missing $ENWIKI_33M"; exit 1; }
du -h "$ENWIKI_33M"
head -1 "$ENWIKI_33M" | awk -F'\t' '{print "fields=" NF}'
```

enwiki TSV 是 3 列 `title / docdate / text`，没有 `id`。服务端用例建 3 列表即可；
进程内基准 `fulltext-bench` 需要 `{id,title,text}` 的 JSONL，见 §5.1。

SIFT 的符号链接目标必须存在：

```bash
ls -l test/data/benchmark/sift_1m/
readlink -f test/data/benchmark/sift_1m/sift_base.fvecs
# GIST 没有数据时先确认现场目录，后面用 --data-dir 指向它
ls -l test/data/benchmark/gist_1m/ 2>/dev/null || echo "GIST_DIR not linked yet"
```

### 2.5 隔离与清理

同一台机上只跑一个引擎。手动起服务时不做网络命名空间隔离，所以必须先确认端口和进程：

```bash
pgrep -a -x infinity || true
ss -ltn '( sport = :23817 or sport = :5432 or sport = :5433 or sport = :54321 )'
```

只 kill 本次启动的 PID（`kill "$PID"`），不要用 `pkill -f infinity`：它会把别的会话和当前 shell 一起匹配掉。
如果使用 `bwrap --unshare-pid`，内部的 `pgrep` 只看到本命名空间，风险小；手动服务不加。

所有运行目录放到仓库的 `.scratch/manual/<case>/`，**不要放 `/tmp`，也不要用 tmpfs**。
内存/性能测量必须让数据目录落在磁盘上；RAM-backed 目录会把“引擎内存”和“页缓存容量”混在一起。

```bash
mkdir -p .scratch/manual/{ft,ft33,knn,bytes,bin}
```

## 3 工具与脚本

### 3.1 现有工具

| 工具 | 路径 | 用途 | 输出 |
|---|---|---|---|
| SIFT/GIST 1M 驱动 | `tools/perf_sift.py` | 复现 CI 的 `knn_import_benchmark` / `knn_query_benchmark`：COPY fvecs、建 HNSW（可 `--build-type lsg`）或不建索引（`--no-index`），查询条数可配 | JSON：`dataset`、`index`、`build_type`、`import_s`、`index_s`、`qps`、`lat_p50_ms`、`lat_p95_ms`、`recall_at_100` |
| 通用索引基准 | `tools/run_index_bench.sh` + `run_index_bench_one.sh` | 每个引擎独立网络命名空间，起停 + 跑驱动；本文只作小规模参考，不放大 | `INDEX <kind> build|query <s>`、`SEC <phase> <s>` |
| 全文进程内基准（重写版） | `rust_engine/target/release/infinity fulltext-bench <dir> <mode> <corpus.jsonl> [batch]` | `insert` / `import` / `merge` / `query` / `query-only` | 各阶段耗时、5 条短查询 + 13/14 词长查询耗时 |
| 全文进程内基准（参考） | `benchmark/local_infinity/fulltext/fulltext_benchmark`（或 `cmake-build-reldeb/benchmark/local_infinity/fulltext_benchmark`） | 同上的 C++ 版 | 同阶段耗时；查询会打印 `result_id[i]` |
| 跨引擎行为门禁 | `python/restart_test/test_cross_engine_compat.py` | C++ 写 → Rust 读、Rust 写 → C++ 读；catalog/数据/各索引各一条查询 | pytest |
| 跨引擎字节门禁 | `python/restart_test/test_cross_engine_bytes_*.py` | 分层比对文件、表、段、WAL、变更、存储 | pytest |
| 门禁共享工具 | `python/restart_test/bytes_common.py` | 配置模板、起停、文件遍历、比较器 | Python API |
| Gate C 总入口 | `rust_engine/scripts/run_gate_c.sh` | workspace 测试 + snapshot + restart + 跨引擎 + MinIO（有环境时） | PASS/FAIL |
| 门禁 shim | `rust_engine/scripts/infinity_rust_shim.sh` | 把 `--config=<toml>` 映射到重写版；设置 `MALLOC_ARENA_MAX=2` | 服务进程 |

### 3.2 自动门禁与 CI 的位置

`.github/workflows/rust_engine.yml` 的 job：

| job | 覆盖 | 与本文的关系 |
|---|---|---|
| `quality` | fmt / clippy / deny | 不涉及数据规模 |
| `unit` | `cargo test --workspace`、cli smoke、Gate A 基线 | 单测证明不了 33M 的资源和 IO 行为 |
| `build` | 参考版（builder 镜像，amd64/arm64）+ 重写版 | 产出 `bin-<engine>-<arch>` 制品，手工可直接用 |
| `restart` | Gate C 重启用例（`-m "not slow"`） | 手工补 33M 的长时间导入和内存 |
| `sdk` | 未改的 Python SDK 打两个引擎（nightly/手动） | 不涉及大数据索引 |
| `cross_engine` | `test_cross_engine_compat` + `test_cross_engine_bytes_*` | 手工跑同一套 + 8.39M HUGE + 33M 互读 |
| `slow` | miri / ASan / ignored / parallel / kill -9 循环；重导入 5.toml | 5.toml 仍是 `continue-on-error`，是手工 FT-2 的直接动因 |

`.github/workflows/tests.yml` 的 `reldeb_amd64_benchmark` 是参考版自己的 SIFT 黄金基线：
`knn_import_benchmark` + `knn_query_benchmark`，1 线程和 8 线程。`tools/perf_sift.py` 把同一组语句
改成走 SQL/PG 协议，因此同一台机上可以同时跑 C++ 和 Rust，而不是拿两套客户端比。

**CI 只有 SIFT 的 golden，没有 GIST 的 golden**：GIST 的 HNSW/暴力数字只能在同一轮里做
C++ 与 Rust 的相对比较，不能拿历史上的 SIFT 数字或 CI golden 当 GIST 的基线。

`RUST_DESELECT` 当前只列了对象存储往返一项（`test_cross_engine_bytes_60_storage.py::test_the_port_round_trips_through_object_storage`）；
手工验收不要把这一项记成回归，原因见 [`GAPS.md`](GAPS.md) §1。

### 3.3 内存采样脚本

引擎退出后 `/proc` 就没了，峰值只能运行中采。把下面这段存到
`.scratch/manual/bin/sample_rss.sh`：

```bash
#!/usr/bin/env bash
# 用法: sample_rss.sh <out.tsv> [进程名]
# 进程名默认 infinity；C++ 全文基准传 fulltext_benchm（Linux comm 截断到 15 字符）。
set -u
out=$1
name=${2:-infinity}
while :; do
  pid=$(pgrep -x "$name" | head -n1 || true)
  if [[ -n "$pid" ]]; then
    ts=$(date +%s.%N)
    awk -v ts="$ts" '
      /^VmHWM:/ || /^VmRSS:/ || /^RssAnon:/ || /^RssFile:/ {
        printf "%s %s %s\n", ts, $1, $2
      }' "/proc/$pid/status" 2>/dev/null
  fi
  sleep 0.2
done
```

```bash
chmod +x .scratch/manual/bin/sample_rss.sh
```

阶段标记由调用方写进同一个文件。先设置 `RSS` 指向采样脚本的输出，再定义：

```bash
mark() { echo "MARK $1 $(date +%s.%N)" >> "${RSS:?set RSS first}"; }
```

采样字段：

| 字段 | 含义 | 用途 |
|---|---|---|
| `VmHWM` | 进程启动以来的峰值 RSS | 整轮峰值，最关键的对比数 |
| `VmRSS` | 当前 RSS | 分阶段峰值/稳态 |
| `RssAnon` | 匿名页（堆、栈、分配器） | 区分“引擎自己分配的内存” |
| `RssFile` | 文件映射页（mmap 的块/索引） | 重写版 mmap 块和索引段；参考也有 buffer/mmap，必须一起看 |

**不要只看 `VmRSS` 总量**：重写版的封存块和索引段是 mmap，`RssFile` 会随着扫描/查询上涨；
参考的 BufferManager 也会把文件页算进 RSS。两端都记录 `RssAnon` 和 `RssFile`，否则会把
“页缓存命中”误判成“引擎泄漏”。

### 3.4 已有用例与手工用例的映射

| 已有用例 | 自动覆盖 | 本文补什么 |
|---|---|---|
| `test_cross_engine_compat.py` | 200 行、5 类索引的双向互读 | 33M 全文、SIFT/GIST 的 LSP-HNSW 互读与增量写 |
| `test_cross_engine_bytes_20_table.py` | 200 行单表、2 万行多块、各索引 chunk | 8.39M 全文逐字节（§8.3） |
| `test_cross_engine_bytes_30_segments.py` | 多段、压缩、cleanup、15 万行一次 COPY | 8.39M/16.7M HUGE、33M 的段布局 sanity |
| `test_cross_engine_bytes_40_wal.py` / `_50_mutations.py` | WAL 帧、删除/更新/崩溃后恢复 | 33M 导入/建索引中途 kill -9（§9） |
| `test_multiple_index_types_import.py`（5.toml） | 9,999 行 × 25 轮、8 列 6 索引，长导入 | 33M 全文的长时间导入/内存曲线 |
| `tools/perf_sift.py` + CI `reldeb_amd64_benchmark` | C++ SIFT 1M 黄金、SQL 口径 | SIFT/GIST 1M、LSP-HNSW/暴力、峰值内存、互读 |
| `tools/run_index_bench.sh`（`index-types`／`secondary-*`） | 2 万–20 万行的各索引 build/query | 不在本文放大；只作为小规模交叉核对 |

## 4 通用测量方法

### 4.1 计时

* 同一驱动、同一批 SQL、同一行数、同一 `queries`；先 warm 1–3 次，再连续跑 20–100 次。
* 至少 3 轮，取中位数；每轮记录最小、最大，别只留一个均值。
* 建索引是一次性墙钟，包含读列 + 构图 + 落 chunk；不要把它和“查询第一发”混在一起。
* 查询端到端包含 thrift/PG 往返、解析、序列化；服务端 exec 计时段只在 `INF_SERVER_TIMER=1` 时打印，
  两端同一插桩位置。大表上端到端和服务端 exec 的差距会变小，但不要把两者混用。
* `psql` 启动约 1–3 ms；查询在 5 ms 以下时用进程内基准或 SDK 长连接，不要用一行一个 `psql` 比较。

### 4.2 内存

* 两端都 `export MALLOC_ARENA_MAX=2`，否则 glibc 的 per-thread arena 会把“线程数差异”记成内存差异。
* 采样 200 ms 一次；峰值用 `VmHWM` 核对，分阶段用 `VmRSS` + `MARK`。
* 记录三个点：导入结束、建索引结束、查询结束；再加一个“空闲 10 s”的稳态。
* 页缓存冷/热必须两端一致：同一轮内要么都先预热一遍，要么都在新目录上冷跑。不要 C++ 冷跑、Rust 热跑。
* 如果机器开了 swap，记录 `swapon --show`；不要把 swap 当成可用内存。
* 参考在容器里跑时，用宿主机 `/proc/<pid>/status` 采样（容器共享内核），不要用 `docker stats` 的
  “内存使用”直接和宿主机 RSS 比——它包含 page cache。

### 4.3 落盘体积

```bash
# 总数据目录
du -sb .scratch/manual/ft33/cpp/data
# 某个索引的目录（db_/tbl_ 用实际编号）
du -sb .scratch/manual/ft33/cpp/data/db_*/tbl_*/idx_*
# 全文 chunk 个数
find .scratch/manual/ft33/cpp/data -name '*.dic' | wc -l
```

体积按**同一行数**比；索引 chunk 数受 `mem_index_capacity` 和后台 dump 影响，不单独作为格式判据。

### 4.4 公平性检查单

- [ ] 两端二进制都是 release/RelWithDebInfo，时间戳是本次构建。
- [ ] 同一份数据文件、同一行数、同一列类型、同一索引参数。
- [ ] 同一配置：`optimize/cleanup/compact/checkpoint` 全 `0s`，`result_cache=off`，`memindex_memory_quota` 相同。
- [ ] 同一 `MALLOC_ARENA_MAX`、同一 `buffer_manager_size`、同一 `mem_index_capacity`。
- [ ] 同一时间只跑一个引擎；没有别的会话在同一端口/同一目录跑。
- [ ] 同一页缓存状态（都冷或都热），记录 `free -g`。
- [ ] 记录 `git rev-parse HEAD`、二进制大小和时间、`nproc`、内存总量、磁盘型号。
- [ ] 3 轮重复；每轮目录是新的，不复用上一轮的索引/块。

### 4.5 结果记录模板

```markdown
| 用例 | 引擎 | 数据 | 阶段 | 耗时 s | 峰值 RSS GB | 稳态 RSS GB | RssAnon GB | RssFile GB | 落盘 GB | 备注 |
|---|---|---|---|---|---|---|---|---|---|---|
| FT-2 | C++ | enwiki 33M | COPY | | | | | | | |
| FT-2 | C++ | enwiki 33M | CREATE INDEX | | | | | | | |
| FT-2 | Rust | enwiki 33M | COPY | | | | | | | |
| FT-2 | Rust | enwiki 33M | CREATE INDEX | | | | | | | |
| KNN-1 | C++ | SIFT 1M | LSP-HNSW index | | | | | | | |
| KNN-1 | Rust | SIFT 1M | LSP-HNSW index | | | | | | | |
| KNN-2 | C++ | GIST 1M | LSP-HNSW index | | | | | | | |
| KNN-2 | Rust | GIST 1M | LSP-HNSW index | | | | | | | |
| KNN-3 | C++ | SIFT 1M | brute force | | | | | | | |
| KNN-3 | Rust | SIFT 1M | brute force | | | | | | | |
| KNN-4 | C++ | GIST 1M | brute force | | | | | | | |
| KNN-4 | Rust | GIST 1M | brute force | | | | | | | |
```

每轮另存原始输出：`stdout.log`、`engine.log`、`*.rss`、驱动 JSON、`du -sb` 输出。

## 5 FT-1：全文 1M/8.39M 进程内基准

这一档用现有 `fulltext-bench` / `fulltext_benchmark`，目的是在进入 33M 之前确认
“同一份 JSONL、同一组查询、同一套阶段计时”两边能对上，并拿到长查询的缩放曲线。

**8.39M 这一档先看内存**：重写版的 `fulltext-bench` 会把整个 JSONL 物化成
`Vec<[String; 3]>`，8M 条三列字符串本身就占十几 GB；`free -g` 不够就只跑 1M，
33M 一律走 §6 的服务端 COPY。

### 5.1 语料

`gen-corpus` 从 enwiki 10 万行抽取词表，按 zipf 形状生成 1M 文档：

```bash
cd rust_engine
cargo run --release --bin gen-corpus -- \
  --corpus ../test/data/benchmark/enwiki/enwiki-10w.csv \
  --out ../.scratch/manual/ft/corpus-1m.jsonl \
  --vocab-out ../.scratch/manual/ft/vocab.txt \
  --docs 1000000 --words 40 --vocab 20000
cd ..
```

输出是 `{"id","title","text"}` 的 JSONL，两端读同一个文件。

### 5.2 参考版

参考的语料路径写死为 `$(INFINITY_ROOT_PATH)/test/data/benchmark/dbpedia-entity/corpus.jsonl`，
数据目录写死 `/var/infinity`。用 bwrap 把 `/var/infinity` 绑到磁盘目录：

```bash
export FT_ROOT="$PWD/.scratch/manual/ft/root"
export FT_DATA="$PWD/.scratch/manual/ft/cpp-data"
mkdir -p "$FT_ROOT/test/data/benchmark/dbpedia-entity" "$FT_DATA"
ln -sf "$PWD/.scratch/manual/ft/corpus-1m.jsonl" \
  "$FT_ROOT/test/data/benchmark/dbpedia-entity/corpus.jsonl"

export MALLOC_ARENA_MAX=2
bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
  --bind "$FT_DATA" /var/infinity \
  --unshare-net --unshare-pid --proc /proc -- \
  env INFINITY_ROOT_PATH="$FT_ROOT" \
  ./cmake-build-reldeb/benchmark/local_infinity/fulltext_benchmark \
    --mode import
```

`--mode import` 做一次 COPY（`--insert-batch` 只在 insert/merge 模式生效，import 时不要依赖它）；
随后重复查询用 `--mode query-only`（不重建表、不重建索引）：

```bash
bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
  --bind "$FT_DATA" /var/infinity \
  --unshare-net --unshare-pid --proc /proc -- \
  env INFINITY_ROOT_PATH="$FT_ROOT" \
  ./cmake-build-reldeb/benchmark/local_infinity/fulltext_benchmark \
    --mode query-only
```

长查询用 `--terms N --repeat N --topn 10 --algo heap_bmw`；如果改用 `insert` 模式，
参考的行数上限是 8192，`--insert-batch` 不要超过 8192。日志在 `/var/infinity/log/infinity.log`（绑定到 `$FT_DATA/log`），
stdout 上没有耗时行。

### 5.3 重写版

```bash
export MALLOC_ARENA_MAX=2
./rust_engine/target/release/infinity fulltext-bench \
  .scratch/manual/ft/rust-data import \
  .scratch/manual/ft/corpus-1m.jsonl 100000

# 复用同一目录，只跑查询
./rust_engine/target/release/infinity fulltext-bench \
  .scratch/manual/ft/rust-data query-only \
  .scratch/manual/ft/corpus-1m.jsonl
```

`FTBENCH_QUERY_REPEAT=20` 让每条查询在进程内跑 20 次取最小；
`FTBENCH_EXTRA_QUERY='...'` 追加参考日志里的长查询，保证两边打的是同一批词。

**注意**：重写版的 `fulltext-bench` 把整个 JSONL 读进内存（`read_jsonl_rows`），
33M 行需要非常大的内存；33M 不走这条路径，走 §6 的服务端 COPY。1M / 8.39M 可以，
但也要先看 `free -g`。

### 5.4 记录与判定

| 阶段 | 看什么 | 判据 |
|---|---|---|
| Import/Insert | 墙钟 | Rust/C++ 比值；注意 INSERT 的批量大小不同（参考 ≤8192，重写版可 100000），批次差异要写进备注 |
| Create index（空表） | 墙钟 | 空表 DDL 成本，绝对值很小，只做 sanity |
| Optimize/merge | 墙钟 | 合并 chunk 的成本，应与参考同量级 |
| 查询 | 5 条短查询 + 13/14 词长查询 | 1–2 词查询 Rust 应不慢于参考；长查询比值记录曲线 |
| 结果 | 参考日志的 `result_id[i]`；重写版只打 `rows` 数 | 逐 id 比对走 §6 的服务端 SQL（同一目录起 server），或只把 `rows` 数和 §6 的 `ROW_ID` 作为正确性判据 |

`ReadJsonl` 的耗时不要直接比：参考用 simdjson，重写版是手写扫描；这一项不进查询对比。

### 5.5 内存

在启动前开采样，进程名 C++ 用 `fulltext_benchm`（comm 截断），Rust 用 `infinity`：

```bash
.scratch/manual/bin/sample_rss.sh .scratch/manual/ft/cpp.rss fulltext_benchm &
SAMPLER=$!
# 跑 §5.2 的 bwrap 命令
kill "$SAMPLER"
```

记录峰值、导入结束、建索引结束、查询结束四个点；`ReadJsonl` 阶段的 RSS 单独标出来，
它属于驱动侧内存，不属于索引内存。

## 6 FT-2：全文 33M 服务端主用例

33M 不使用进程内基准，原因是重写版的驱动会把整个 JSONL 物化进内存。这里走服务端：
两端都用 PG 协议发同一条 `CREATE TABLE` / `COPY` / `CREATE INDEX` / `SELECT`，
数据目录在磁盘上，内存由 §3.3 采样。

### 6.1 预检

```bash
export ENWIKI_33M="${ENWIKI_33M:-$PWD/benchmark/enwiki.33332620.csv}"
test -f "$ENWIKI_33M" || { echo "missing $ENWIKI_33M"; exit 1; }
du -h "$ENWIKI_33M"
head -1 "$ENWIKI_33M" | awk -F'\t' '{print "fields=" NF}'
df -h .
free -g
```

如果 33M 文件不在本机，先用 `test/data/benchmark/enwiki/enwiki-10w.csv`（10 万行）把 §6.2–6.6 走通，
再用 `benchmark/enwiki.8388608.csv`（8.39M）跑一遍，最后才上 33M。

### 6.2 生成配置

用门禁同一份配置模板，保证 `optimize/cleanup/compact/checkpoint` 全关、`mem_index_capacity=65536`、
`memindex_memory_quota=2GB`：

```bash
.venv/bin/python - <<'PY'
import sys
sys.path.insert(0, "python")
from restart_test import bytes_common as bc
for engine, port in (("cpp", 5432), ("rust", 5433)):
    root = f".scratch/manual/ft33/{engine}"
    bc._write_config(engine, root, port, quota="2GB")
    print(engine, root)
PY
```

**Rust 的 PG 端口不要用 23817**：23817 同时是 Rust 的 Thrift 默认端口，PG 先绑上后 Thrift 会
`Address already in use`，进程直接退出。PG 用 5433，Thrift 留在 23817。C++ 的 PG 用 5432。

配置里 C++ 的 `persistence_dir` 为空（参考把它当对象存储开关，非空会找不到块文件），
Rust 的 `persistence_dir` 指向 `<root>/persistence`。两份配置的 `data_dir/catalog_dir/wal_dir`
各自独立，行数、列、SQL 完全相同。

### 6.3 启动与就绪

一次只启动一个引擎：

```bash
export MALLOC_ARENA_MAX=2

# C++
./cmake-build-reldeb/src/infinity \
  --config=.scratch/manual/ft33/cpp/engine.toml \
  > .scratch/manual/ft33/cpp/stdout.log 2>&1 &
CPP_PID=$!

# 等到 PG 端口真正能查询（TCP connect 成功不代表存储初始化完）
for i in $(seq 1 180); do
  psql -h 127.0.0.1 -p 5432 -U admin -d default_db -Atc 'show databases' \
    >/dev/null 2>&1 && break
  sleep 1
done
```

Rust 用同样的命令，配置换成 `.../rust/engine.toml`，psql 端口换 5433。启动失败先看
`.../stdout.log` 和 `.../log/infinity.log`，不要直接重跑。

### 6.4 建表与导入（B2：先灌数据，再建索引）

下面以 C++ / PG 5432 为例；跑 Rust 时把 `-p 5432` 换成 `-p 5433`，其余 SQL 不变。

```bash
psql -h 127.0.0.1 -p 5432 -U admin -d default_db -c "
  CREATE TABLE ft (title VARCHAR, docdate VARCHAR, text VARCHAR);
"

# SQL 文件用 quoted heredoc 生成，避免 shell 再吃一层反斜杠；
# 文件里保留 '\t'（两个字符），与 test/sql 的 COPY 写法一致。
cat > .scratch/manual/ft33/copy.sql <<'SQL'
COPY ft FROM '__ENWIKI__' WITH (DELIMITER '\t', FORMAT CSV);
SQL
sed -i "s|__ENWIKI__|$ENWIKI_33M|" .scratch/manual/ft33/copy.sql

TIMEFORMAT='COPY %3R s'
time psql -h 127.0.0.1 -p 5432 -U admin -d default_db -f .scratch/manual/ft33/copy.sql

psql -h 127.0.0.1 -p 5432 -U admin -d default_db -Atc "SELECT count(*) FROM ft;"
```

`count(*)` 必须等于 33,332,620（或实际文件行数）。如果不等于，先停，不要继续建索引。
`COPY` 的路径用绝对路径；CSV 的 `DELIMITER '\\t'` 与 enwiki 的 3 列对应。

### 6.5 建索引

```bash
TIMEFORMAT='CREATE INDEX %3R s'
time psql -h 127.0.0.1 -p 5432 -U admin -d default_db -c "
  CREATE INDEX ft_idx ON ft (text) USING FULLTEXT;
"
```

这一阶段是全文索引的主指标：读 33M 行的 `text`、分词、建 posting / 位置 / 长度、落 chunk。
记录耗时、峰值 RSS、`du -sb` 的索引目录。

**B1（可选）在线维护**：另起一个空目录，先 `CREATE INDEX` 再 `COPY`，测的是“写入时维护索引”的路径。
它和 `fulltext_benchmark` 的 `insert` 同序，但磁盘要再占一份；33M 不建议默认跑，空间够再补。

### 6.6 查询

固定查询集用 `FULLTEXT_BENCH.md` 的五条短查询 + 两条长查询：

```sql
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', 'harmful "social custom"', 'topn=10');
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', 'social custom "harmful chemical"', 'topn=10');
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', '"annual American awards"', 'topn=10');
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', 'harmful chemical', 'topn=10');
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', '"one of"', 'topn=10');
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', 'harmful chemical social custom annual American awards one of the first world war', 'topn=10');
SELECT ROW_ID(), SCORE() FROM ft SEARCH MATCH TEXT ('text', 'national football league high school state university population family city game season team player', 'topn=10');
```

把上面 7 条写进 `.scratch/manual/ft33/queries.sql`（开头 `\timing on`），每条重复 23 次（3 次 warmup + 20 次计时）。
连接一次 psql 跑完，取每条查询 20 次计时的中位数：

```bash
psql -h 127.0.0.1 -p 5432 -U admin -d default_db -A -F $'\t' -t \
  -f .scratch/manual/ft33/queries.sql > .scratch/manual/ft33/cpp-query.log
```

结果比对用一次不带 `\timing` 的输出：

```bash
psql ... -A -F $'\t' -t -c "<query>" > cpp.tsv
psql ... -A -F $'\t' -t -c "<query>" > rust.tsv   # 换成 -p 5433
diff <(cut -f1 cpp.tsv) <(cut -f1 rust.tsv)      # ROW_ID 必须完全一致
paste cpp.tsv rust.tsv | awk -F'\t' '{d=$2-$4; if(d<0)d=-d; if(d>m)m=d} END{print "max score delta", m}'
```

`ROW_ID` 的顺序也必须一致（全文打分是确定的）；如果只是 score 的文本格式不同，比较数值差，
不要直接 diff 字符串。

**psql 的启动开销**：33M 的查询通常在毫秒到几十毫秒，psql 的 1–3 ms 固定开销会让比值偏向“看不出差异”。
如果某条查询绝对值 < 5 ms，改用进程内 1M 基准（§5）看相对性能。

### 6.7 内存与落盘

```bash
RSS=.scratch/manual/ft33/cpp/run.rss
mark() { echo "MARK $1 $(date +%s.%N)" >> "$RSS"; }
.scratch/manual/bin/sample_rss.sh "$RSS" infinity &
SAMPLER=$!

mark "IMPORT"          # 在 §6.4 前
# ... COPY ...
mark "INDEX"           # 在 §6.5 前
# ... CREATE INDEX ...
mark "QUERY"           # 在 §6.6 前
# ... 查询 ...
kill "$SAMPLER"

# 分阶段 VmRSS 峰值（kB -> GB）
awk '
  $1=="MARK" { phase=$2; next }
  $2=="VmRSS:" { if ($3+0 > max[phase]) max[phase]=$3+0 }
  END { for (p in max) printf "%s %.2f GB\n", p, max[p]/1024/1024 }
' "$RSS"

# 整轮 VmHWM
awk '$2=="VmHWM:"{v=$3} END{printf "VmHWM %.2f GB\n", v/1024/1024}' "$RSS"
```

`MARK` 行的第二列是阶段名，采样行第二列是 `VmRSS:`；采样脚本本身见 §3.3。

落盘：

```bash
du -sb .scratch/manual/ft33/cpp/data
du -sb .scratch/manual/ft33/cpp/data/db_*/tbl_*/idx_*
find .scratch/manual/ft33/cpp/data -name '*.dic' | wc -l
```

33M 的全文索引约 40 GB（按 1.2 GB/1M 文档估），chunk 数应按 `mem_index_capacity=65536` 切：
33,332,620 / 65,536 ≈ 509 个 chunk。chunk 数只作 sanity，不作为格式判据。

### 6.8 停止与清理

```bash
kill "$CPP_PID"
wait "$CPP_PID" 2>/dev/null || true
```

等端口释放后再启动 Rust：

```bash
for i in $(seq 1 60); do
  ss -ltn '( sport = :23817 or sport = :5432 or sport = :5433 )' | grep -q LISTEN || break
  sleep 1
done
```

删除 33M 目录前确认已经记录了 `du -sb` 和 RSS；单份 33M 目录约 70 GB，
不要在两份目录还没对比完时先删。

## 7 KNN：SIFT 1M / GIST 1M × LSP-HNSW / 暴力搜索

### 7.1 版本口径：当前 HNSW 是 LSP

本文的 HNSW 指**吴文龙修改后的 LSP 版本**，不是上游原始 HNSW；不要拿旧的 HNSW 报告或上游
基准数字直接当基线。仓库里与之对应的可选项是 `build_type='lsg'`（Local Scaling Graph，
`lsg_config` 默认 `sample_ratio=0.01, ls_k=10, alpha=1.0`）：

- `perf_sift.py --build-type lsg` 会把 `build_type = 'lsg'` 写进 `CREATE INDEX`；
- 只写 `USING Hnsw` 而不带 `build_type` 是 **plain** 构建，不是本文要验收的实现；
- 跑之前用 SDK 的 `table_obj.show_index("hnsw_index")`（或等价方式）记录实际的
  `build_type`、`lsg_config`、`encode`，两端必须是同一组参数。现场 LSP 版本如果不通过
  `build_type` 暴露，以索引元数据的实际输出为准。

### 7.2 数据与预检

| 数据集 | 目录 | 维度 | base / query / groundtruth |
|---|---|---|---|
| SIFT 1M | `test/data/benchmark/sift_1m/` | 128 | `sift_base.fvecs` / `sift_query.fvecs` / `sift_groundtruth.ivecs` |
| GIST 1M | `test/data/benchmark/gist_1m/` 或 `$GIST_DIR` | 960 | `gist_base.fvecs` / `gist_query.fvecs` / `gist_groundtruth.ivecs` |

```bash
ls -lh test/data/benchmark/sift_1m/ test/data/benchmark/gist_1m/ 2>/dev/null
readlink -f test/data/benchmark/sift_1m/sift_base.fvecs
readlink -f test/data/benchmark/gist_1m/gist_base.fvecs
df -h .
free -g
```

GIST 1M 的 base 约 3.8 GB（1M × 960 × 4）、query/groundtruth 各几 MB。驱动默认读
`test/data/benchmark/sift_1m` / `test/data/benchmark/gist_1m`；没有链接时用 `SIFT_DIR` /
`GIST_DIR`，或每次传 `--data-dir` 指向现场目录：

```bash
export GIST_DIR=/path/to/gist_1m
# 或者每次传 --data-dir /path/to/gist_1m
```

### 7.3 驱动

`tools/perf_sift.py` 现在同时覆盖 SIFT 和 GIST，以及 HNSW / 暴力两种模式：

| 参数 | 作用 |
|---|---|
| `--dataset sift|gist` | 选数据文件和维度（128/960） |
| `--build-type plain|lsg` | `lsg` 是本文的 LSP 版本；不传是 plain |
| `--no-index` | 不建索引，走精确全表扫描（暴力搜索） |
| `--rows 0` | 整个 1M base |
| `--queries N` / `--times N` | 查询条数 / 重复轮数 |
| `--port N` | PG 端口；Rust 必须用 5433 |
| `--data-dir DIR` | 覆盖数据集目录 |

每次运行会清空自己的 `.scratch/perf-<engine>`；四组用例连跑时，互读操作要在该组跑完后立刻做，
或者先把目录 `mv` 成带数据集后缀的名字。Rust 仍然必须 `--port 5433`：默认的 23817 会和 Thrift 抢端口。

### 7.4 四组用例

一次一个引擎；先 C++ 后 Rust。下面每条命令的 `> .scratch/manual/knn/...json` 保存原始结果。

```bash
mkdir -p .scratch/manual/knn
export MALLOC_ARENA_MAX=2

# KNN-1：SIFT 1M + LSP-HNSW
.venv/bin/python tools/perf_sift.py cpp  --dataset sift --build-type lsg \
  --rows 0 --queries 100 --ef 200 --times 3 > .scratch/manual/knn/sift-hnsw-cpp.json
.venv/bin/python tools/perf_sift.py rust --port 5433 --dataset sift --build-type lsg \
  --rows 0 --queries 100 --ef 200 --times 3 > .scratch/manual/knn/sift-hnsw-rust.json

# KNN-2：GIST 1M + LSP-HNSW（数据不在默认目录时加 --data-dir）
.venv/bin/python tools/perf_sift.py cpp  --dataset gist --build-type lsg \
  --rows 0 --queries 100 --ef 200 --times 3 > .scratch/manual/knn/gist-hnsw-cpp.json
.venv/bin/python tools/perf_sift.py rust --port 5433 --dataset gist --build-type lsg \
  --rows 0 --queries 100 --ef 200 --times 3 > .scratch/manual/knn/gist-hnsw-rust.json

# KNN-3：SIFT 1M 暴力搜索（不建索引）
.venv/bin/python tools/perf_sift.py cpp  --dataset sift --no-index \
  --rows 0 --queries 100 --times 1 > .scratch/manual/knn/sift-brute-cpp.json
.venv/bin/python tools/perf_sift.py rust --port 5433 --dataset sift --no-index \
  --rows 0 --queries 100 --times 1 > .scratch/manual/knn/sift-brute-rust.json

# KNN-4：GIST 1M 暴力搜索（GIST 全扫较慢，先跑 20 条查询）
.venv/bin/python tools/perf_sift.py cpp  --dataset gist --no-index \
  --rows 0 --queries 20 --times 1 > .scratch/manual/knn/gist-brute-cpp.json
.venv/bin/python tools/perf_sift.py rust --port 5433 --dataset gist --no-index \
  --rows 0 --queries 20 --times 1 > .scratch/manual/knn/gist-brute-rust.json
```

如果只做 GIST 的暴力搜索，仍需跑一次 HNSW 作为速度/召回对照；反过来也一样。四组都跑完再比较
“索引带来的加速比”（KNN-1/3、KNN-2/4 的 query 耗时比）。

### 7.5 指标与判定

| 字段 | 含义 | 判据 |
|---|---|---|
| `import_s` | COPY 1M fvecs 墙钟 | 记录，不单独设阈值；SIFT/GIST 分开比 |
| `index_s` | `CREATE INDEX ... USING Hnsw` 墙钟；暴力模式为 `0.0` | HNSW 的 Rust/C++ ≤ 1.5× |
| `qps` / `lat_p50_ms` / `lat_p95_ms` | 端到端吞吐/延迟 | Rust/C++ ≤ 1.5×；p95 比 p50 更能看出长尾 |
| `recall_at_100` | 与各自 groundtruth 的 top-100 召回 | **整表暴力模式必须 = 1.0**；HNSW 的 Rust 与 C++ 差 < 0.01 |
| 索引体积 | HNSW 的 `.scratch/perf-<engine>/data/.../idx_*` | Rust/C++ ≤ 1.1×；暴力模式没有索引目录 |
| LSP 参数 | `show_index` 的 `build_type/lsg_config/encode` | 两端一致；记录到报告，不能只写 “HNSW” |

暴力搜索是正确性基准：在 `--rows 0`（整表 1M）下它精确扫描全部 base，top-100 应与
groundtruth 完全一致（`recall_at_100 = 1.0`）。如果整表下不是 1.0，先查数据文件/维度/groundtruth
是否配对，不要先怀疑 HNSW。用 `--rows N` 子集冒烟时，groundtruth 仍是全表 top-100，recall 会偏低，
不能拿子集结果做正确性判据。
HNSW 与暴力同批查询的耗时比就是索引加速比；这个比值在 SIFT 和 GIST 上分别记录，不要混在一起。

HNSW 的图区不逐字节比：参考用 `thread_local std::mt19937`，5000 行时 5 次运行能出 5 张不同的图。
1M 下更不要把“两次运行文件不同”当成格式回归；要看 `recall_at_100` 和互读。LSP 的缩放因子是
索引内容的一部分，但图本身仍受建图随机性影响。

### 7.6 内存

在 driver 外面套采样；进程名是 `infinity`（两端二进制同名，一次只跑一个）：

```bash
RSS=.scratch/manual/knn/sift-hnsw-cpp.rss
.scratch/manual/bin/sample_rss.sh "$RSS" infinity &
SAMPLER=$!
.venv/bin/python tools/perf_sift.py cpp --dataset sift --build-type lsg \
  --rows 0 --queries 100 --ef 200 --times 3 > .scratch/manual/knn/sift-hnsw-cpp.json
kill "$SAMPLER"
```

其余三组把 `--dataset`、`--build-type`、`--no-index`、`--queries` 换成 §7.4 对应命令即可。
峰值通常出现在 HNSW 的 `CREATE INDEX`；暴力模式没有建索引阶段，峰值主要来自扫描时的
页缓存/结果集，记录 `RssFile` 并与 HNSW 分开看。分阶段采样时用 §3.3 的 `MARK`。

### 7.7 互读（仅 HNSW）

HNSW 的 chunk 是“头 + 向量 + 标签 + 图”，参考和重写版都能读对方的布局，但图本身不同。
互读判据是**能打开、能查、recall 不塌**，不是 top-100 完全一致。暴力模式没有索引 chunk，
数据块的互读由 §8 的字节门禁覆盖，不在这里重复。

**Rust 读 C++ 目录**（`perf_sift.py` 跑完后目录保留在 `.scratch/perf-cpp`；最后一个参数是表名）：

```bash
# SIFT（表名 sift）
./rust_engine/target/release/infinity knn-bench \
  .scratch/perf-cpp \
  test/data/benchmark/sift_1m/sift_query.fvecs \
  test/data/benchmark/sift_1m/sift_groundtruth.ivecs \
  200 100 sift

# GIST（表名 gist；数据目录换实际路径）
./rust_engine/target/release/infinity knn-bench \
  .scratch/perf-cpp \
  test/data/benchmark/gist_1m/gist_query.fvecs \
  test/data/benchmark/gist_1m/gist_groundtruth.ivecs \
  200 100 gist
```

看输出的 `qps`、`lat p50/p95`、`recall@100`；和 §7.5 的 Rust 自建索引比，
recall 不应明显下降。

**C++ 读 Rust 目录**：复制 `.scratch/perf-rust/engine.toml`，把 `persistence_dir` 改成空串、
`postgres_port` 改成 5432，其余 `data_dir/catalog_dir/wal_dir` 保持指向 `.scratch/perf-rust`：

```bash
sed -e 's|^persistence_dir.*|persistence_dir = ""|' \
    -e 's|^postgres_port.*|postgres_port = 5432|' \
    .scratch/perf-rust/engine.toml > .scratch/manual/knn/cpp-read.toml
./cmake-build-reldeb/src/infinity --config=.scratch/manual/knn/cpp-read.toml \
  > .scratch/manual/knn/cpp-read.log 2>&1 &
```

然后 `psql -p 5432` 查 `SELECT count(*) FROM sift;`（或 `gist`）和几条 `SEARCH MATCH VECTOR`，
把 top-100 与 Rust 自己的结果比 recall。参考读重写版写的目录时 `persistence_dir` 必须为空，
否则它按对象存储找块文件，直接 abort。

## 8 BY：字节兼容与跨引擎互读

### 8.1 标准门禁（全索引）

先跑完整套件，确认门禁规模（200 行、2 万行、15 万行、多段、压缩、WAL、变更/崩溃）全绿：

```bash
mkdir -p .scratch/manual/bytes
export INFINITY_CPP_BIN="$PWD/cmake-build-reldeb/src/infinity"
export INFINITY_SERVER_BIN="$PWD/rust_engine/target/release/infinity"

bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
  --bind "$PWD/.scratch/manual/bytes" /var/infinity \
  --unshare-net --unshare-pid --proc /proc -- \
  .venv/bin/python -m pytest -q \
    python/restart_test/test_cross_engine_compat.py \
    python/restart_test/test_cross_engine_bytes_*.py \
    --infinity_path=rust_engine/scripts/infinity_rust_shim.sh
```

或者直接跑 `./rust_engine/scripts/run_gate_c.sh`；它额外跑 workspace 测试、snapshot 和重启套件，
时间更长。`run_gate_c.sh` 的跨引擎腿会自己 `cargo build --release -p cli` 并套同一个 bwrap。

### 8.2 字节判据的分类

| 文件族 | 门禁里的比法 | 大数据下的做法 |
|---|---|---|
| 块列 `.col`、outline、`version` | 逐字节 | 8.39M HUGE 用例逐字节 |
| 全文 `.dic/.pos/.len` | 逐字节 | 8.39M HUGE 用例逐字节；33M 只互读 + 抽样 |
| BMP 稀疏 chunk | 逐字节 | 门禁规模，不放大 |
| 二级索引 chunk（整数/浮点键） | 逐字节 | 门禁规模，不放大 |
| HNSW chunk | 头 + 向量 + 标签逐字节，图不比较 | 1M 比 recall 与互读 |
| IVF chunk | 内容 | 门禁规模，不放大 |
| `catalog/` | 逻辑内容（键值、`SHOW CATALOG 'all'`） | 33M 只读 `count` 和 `SHOW TABLES` |
| `wal/` | `wal.log` 逐帧；`wal.rust` 合并后比 | 不放大 |
| 对象存储 | 不参与（重写版不移植） | 不参与 |

### 8.3 8.39M 全文逐字节（HUGE）

这是自动门禁里**直接覆盖大数据全文**的用例：`IMPORT` 一次灌 8,388,700 行（2 个段），
全文索引按 `mem_index_capacity=65536` 切 chunk，两端的 `.dic/.pos/.len` 逐字节比。
默认 `INFINITY_BYTES_HUGE_SEGMENTS=3` 是 16.7M 行；先用 2 跑 8.39M，时间和磁盘都减半。

```bash
mkdir -p .scratch/manual/bytes-huge

bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
  --bind "$PWD/.scratch/manual/bytes-huge" /var/infinity \
  --unshare-net --unshare-pid --proc /proc -- \
  env INFINITY_BYTES_HUGE=1 \
      INFINITY_BYTES_HUGE_SEGMENTS=2 \
      INFINITY_CPP_BIN="$PWD/cmake-build-reldeb/src/infinity" \
      INFINITY_SERVER_BIN="$PWD/rust_engine/target/release/infinity" \
  .venv/bin/python -m pytest -q \
    python/restart_test/test_cross_engine_bytes_30_segments.py::test_a_table_larger_than_a_segment_starts_a_second_one
```

判定：pytest 绿；用例内部已经断言每段 128 个 chunk（满段）、尾段 1 个，且每个块/每个全文 chunk
逐字节相同。磁盘不够会先在 `COPY` 阶段失败，先看 `df -h`，不要怀疑格式。

### 8.4 33M / 1M 互读

33M 的**全量逐字节**不作为默认项：509 个全文 chunk、两份约 70 GB 的目录、数小时的运行时，
而且 `memindex_memory_quota` 触发的后台 dump 会让 chunk 边界带上调度成分。默认做：

1. C++ 写 33M 目录（§6），记录 `count(*)`、5 条查询的 `ROW_ID/SCORE`、`du -sb`。
2. 停 C++，用 Rust 配置指向**同一份** `data/catalog/wal`（新建 `persistence` 目录），
   启动 Rust，跑同样的 `count(*)` 和 5 条查询；`ROW_ID` 必须一致，`SCORE` 差 < 1e-6。
3. 用 Rust 插入 1,000 行，执行 `FLUSH DATA;` 和 `DUMP INDEX ft_idx ON ft;`，停 Rust。
4. 用 C++ 原配置重开同一目录（`persistence_dir` 为空），`count(*)` = 33,333,620，
   再查一次同样的 5 条查询；新插入的行在结果里可见，旧结果不退化。

Rust 读 C++ 目录的配置只需要改端口和 `persistence_dir`，`data_dir/catalog_dir/wal_dir` 保持指向 C++ 的目录：

```bash
cp .scratch/manual/ft33/cpp/engine.toml .scratch/manual/ft33/rust-read.toml
# 重写版用 persistence_dir 的父目录作为根；把它放在 cpp 根下，data/catalog/wal 就仍然指向
# C++ 写的那一份。C++ 配置里的 persistence_dir 仍为空，它不会去读这个目录。
sed -i \
  -e 's|^postgres_port.*|postgres_port = 5433|' \
  -e "s|^persistence_dir.*|persistence_dir = \"$PWD/.scratch/manual/ft33/cpp/persistence\"|" \
  .scratch/manual/ft33/rust-read.toml
mkdir -p .scratch/manual/ft33/cpp/persistence
```

SIFT/GIST 的 LSP-HNSW 互读按 §7.7；字节只比头/向量/标签，图区不比。暴力搜索没有索引 chunk，不参与这一项。

### 8.5 不做全量字节的项

| 项 | 原因 |
|---|---|
| 33M 全量逐字节 | 运行时/磁盘成本高；后台 dump 让 chunk 边界带调度成分 |
| HNSW 图区（含 LSP 缩放后的图） | 参考 RNG 不确定，两次运行图不同 |
| IVF chunk | k-means 随机采样 |
| `catalog/`、`wal/` 文件 | session id / LSN / ts，参考自己也不可复现；只比逻辑内容与 WAL 帧 |
| 对象存储桶内布局 | 重写版不移植 `obj_key` 打包上传 |

## 9 RC-1：崩溃恢复（可选）

大数据量下最值得补的崩溃场景是“33M 导入/建索引中途 kill -9”：

1. 按 §6 启动引擎，开始 `COPY` 或 `CREATE INDEX`。
2. 等 30–60 s（确认已经在写盘，`du -sb` 在涨），`kill -9 "$PID"`，`wait`。
3. 用同一配置重启，先 `SELECT count(*) FROM ft;`：
   - `COPY` 是事务，重启后要么 0 行（回滚），要么完整行数；**不能是部分行**。
   - 索引要么不存在（重建前），要么完整可用；不能出现“有 catalog 记录、没有 chunk”。
4. 再跑 5 条查询；和崩溃前的结果比 `ROW_ID`。

门禁里对应的轻量用例：
`python/restart_test/test_cross_engine_bytes_50_mutations.py::test_a_crash_leaves_a_replayable_directory`、
`::test_the_indexes_survive_a_crash`。33M 这一档只做手工补充，不替代门禁。

## 10 结果归档

建议目录：

```text
.scratch/manual/2026-11-07/
  env.txt                 # nproc / free / df / HEAD / 二进制 ls -l
  ft33/
    cpp/{engine.toml,stdout.log,log/,run.rss,copy.time,index.time,queries.tsv}
    rust/{...}
    rust-read.toml
  knn/{sift-hnsw-cpp.json,gist-hnsw-cpp.json,sift-brute-cpp.json,gist-brute-cpp.json,...}
  bytes/{pytest.log}
  report.md               # §4.5 的表 + 结论
```

`env.txt` 至少包含：

```bash
{ echo "date: $(date -Is)"; echo "head: $(git rev-parse HEAD)";
  echo "nproc: $(nproc)"; free -g; df -h .;
  ls -l cmake-build-reldeb/src/infinity rust_engine/target/release/infinity;
  uname -a; } > .scratch/manual/2026-11-07/env.txt
```

报告里每个用例写三行：**结论（PASS/FAIL/REVIEW）**、**比值**、**必须记录的异常**。
异常包括：crash、OOM、端口占用、chunk 数不符合预期、recall 下降、score 差 > 1e-6。

## 11 已知偏差与不计入项

| 项 | 现状 | 对本文的影响 |
|---|---|---|
| 对象存储（`obj_key` 打包 + VirtualStore） | 不移植，桶内布局不同 | 跨引擎“同一桶互读”不成立；CI `RUST_DESELECT` 已排除 |
| DiskANN、geo 类型 | 不移植 | 不在本文测试范围 |
| `EMBEDDING(TINYINT)` 的 I8/U8 HNSW | 重写版稠密 store 只有 f32 | Gate A 有已知不通过项，HNSW 1M 用 FLOAT |
| PLAID 召回 | 两端都与精确解差距大 | 张量索引不放大；需要单独排查 |
| 参考 sparse scan 宽表 | 参考自身会 abort（`Cannot find segment with id: 0`） | 稀疏不放大 |
| 重写版 `fulltext-bench` | 整个 JSONL 读进内存 | 33M 走服务端 COPY，不走进程内基准 |
| HNSW 图 / IVF 质心 | 参考随机，重写版确定 | 只比 recall/内容，不比字节 |
| HNSW 版本 | 当前实现是吴文龙修改后的 LSP，不是上游原始 HNSW | HNSW 用例必须显式 `build_type='lsg'` 并记录 `lsg_config`，不要拿上游/旧 HNSW 数字直接比 |

## 12 附录：一键命令清单

```bash
# 0) 环境
cd /path/to/infinity
uv sync --python 3.11 --frozen --all-extras
(cd rust_engine && cargo build --release -p cli)
export MALLOC_ARENA_MAX=2
mkdir -p .scratch/manual/{ft,ft33,knn,bytes,bytes-huge,bin}
# 先把 §3.3 的 sample_rss.sh 写进 .scratch/manual/bin/，再 chmod
chmod +x .scratch/manual/bin/sample_rss.sh

# 1) 全文 1M 进程内（先冒烟）
(cd rust_engine && cargo run --release --bin gen-corpus -- \
  --corpus ../test/data/benchmark/enwiki/enwiki-10w.csv \
  --out ../.scratch/manual/ft/corpus-1m.jsonl \
  --vocab-out ../.scratch/manual/ft/vocab.txt \
  --docs 1000000 --words 40 --vocab 20000)

# 2) 全文 33M 服务端：生成配置 -> 起 C++ -> COPY -> CREATE INDEX -> 查询
.venv/bin/python - <<'PY'
import sys; sys.path.insert(0, "python")
from restart_test import bytes_common as bc
for engine, port in (("cpp", 5432), ("rust", 5433)):
    bc._write_config(engine, f".scratch/manual/ft33/{engine}", port, quota="2GB")
PY

# 3) KNN：SIFT/GIST × LSP-HNSW/暴力
mkdir -p .scratch/manual/knn
knn() {
  case_name=$1; dataset=$2; shift 2
  for engine in cpp rust; do
    port=""
    [ "$engine" = rust ] && port="--port 5433"
    .venv/bin/python tools/perf_sift.py "$engine" $port --dataset "$dataset" "$@" \
      > ".scratch/manual/knn/$case_name-$engine.json"
  done
}
# GIST 数据不在默认目录时，在 gist 命令后加 --data-dir /path/to/gist_1m
knn sift-hnsw  sift --build-type lsg --rows 0 --queries 100 --ef 200 --times 3
knn gist-hnsw  gist --build-type lsg --rows 0 --queries 100 --ef 200 --times 3
knn sift-brute sift --no-index      --rows 0 --queries 100 --times 1
knn gist-brute gist --no-index      --rows 0 --queries 20  --times 1

# 4) 字节/互读门禁
export INFINITY_CPP_BIN="$PWD/cmake-build-reldeb/src/infinity"
export INFINITY_SERVER_BIN="$PWD/rust_engine/target/release/infinity"
bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
  --bind "$PWD/.scratch/manual/bytes" /var/infinity \
  --unshare-net --unshare-pid --proc /proc -- \
  .venv/bin/python -m pytest -q \
    python/restart_test/test_cross_engine_compat.py \
    python/restart_test/test_cross_engine_bytes_*.py \
    --infinity_path=rust_engine/scripts/infinity_rust_shim.sh

# 5) 8.39M 全文逐字节
bwrap --dev-bind / / --tmpfs /var --dir /var/infinity \
  --bind "$PWD/.scratch/manual/bytes-huge" /var/infinity \
  --unshare-net --unshare-pid --proc /proc -- \
  env INFINITY_BYTES_HUGE=1 INFINITY_BYTES_HUGE_SEGMENTS=2 \
      INFINITY_CPP_BIN="$PWD/cmake-build-reldeb/src/infinity" \
      INFINITY_SERVER_BIN="$PWD/rust_engine/target/release/infinity" \
  .venv/bin/python -m pytest -q \
    python/restart_test/test_cross_engine_bytes_30_segments.py::test_a_table_larger_than_a_segment_starts_a_second_one
```

## 13 相关文档

- [`INDEX_PERF.md`](INDEX_PERF.md)：所有索引基准的入口、各报告的数字与口径。
- [`FULLTEXT_BENCH.md`](FULLTEXT_BENCH.md)：全文 1M 的三路对比、WAND 计数、`fulltext-bench` 参数。
- [`HNSW_BENCH.md`](HNSW_BENCH.md)：20k/200k HNSW 的 build/query/exec 对比；其构建参数以报告为准，不要默认它等于本文的 LSP 口径。
- [`SECONDARY_INDEX_BENCH.md`](SECONDARY_INDEX_BENCH.md)：二级索引的 lookup/maint 口径。
- [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md)：字节门禁的分层、覆盖矩阵和差异。
- [`GAPS.md`](GAPS.md)：不移植项、已知偏差和覆盖不到的范围。
- [`PORTING.md`](PORTING.md)：重写纲领与验收关卡。
