# Analyzer 的 C ABI（`crates/analyzer-ffi`）

把 `analyzer` crate 的分词器做成一个 C 动态库，供 Go 等宿主进程链接。目标是**让宿主的分词结果与建索引时一致**：RAGFlow 的 `conf/infinity_mapping.json` 用 `rag-coarse`/`rag-fine`/`whitespace-#`/`rankfeatures` 这些名字给 Infinity 建全文索引，客户端拿同一套名字、同一份词典分词，查询词才会落在倒排表里的 term 上。

## 1 产物

```bash
cd rust_engine && cargo build --release -p analyzer-ffi
# target/release/librag_analyzer.so   （cdylib，动态链接）
# target/release/librag_analyzer.a    （staticlib，静态链接）
```

头文件：`crates/analyzer-ffi/include/rag_analyzer_c_api.h`。
符号名、结构体布局与 RAGFlow 现有的 `internal/binding/cpp/rag_analyzer_c_api.h` 一一对应，宿主换掉链接产物和头文件路径即可，代码不用改。

**不含** `ThincNER_*` / `ThincParser_*` / `ThincTagger_*`（spaCy 模型推理），那部分只有 C++ 实现。

## 2 调用方怎么用它

RAGFlow（Go）现在通过 cgo 链接 C++ 的 `librag_tokenizer_c_api.a`，用法如下：

| 场景 | 调用的入口 |
|---|---|
| 进程启动 | `RAGAnalyzer_Create(dictPath)` + `RAGAnalyzer_Load()`；`dictPath` 取 `RAGFLOW_DICT_PATH`，默认 `/usr/share/infinity/resource` |
| 分析器池 | `RAGAnalyzer_Copy()` 每个池实例一份（词典共享、状态独立），淘汰时 `RAGAnalyzer_Destroy()` |
| 每请求 | `RAGAnalyzer_SetLanguage(lang)`（文档语言；空串回落到 English） |
| 建 `content_ltks` | `RAGAnalyzer_Tokenize(text)` → 空格连接的粗粒度 token |
| 建 `content_sm_ltks` | `RAGAnalyzer_FineGrainedTokenize(content_ltks)` |
| 高亮/位置 | `RAGAnalyzer_TokenizeWithPosition(text)` + `RAGToken_GetText/Offset/EndOffset` + `RAGAnalyzer_FreeTokenList` |
| 词权 | `RAGAnalyzer_GetTermFreq(term)`、`RAGAnalyzer_GetTermTag(term)`（`term_weight` 用词频与词性给 chunk 打分） |
| 调试 | `RAGAnalyzer_Analyze(text, callback)` |

`internal/tokenizer` 在 `engineType == "infinity"` 时**不**调用分词器：`Tokenize`/`FineGrainedTokenize` 直接返回原文，因为索引由服务端分词。只有 ES / OceanBase 这类引擎才在客户端算 token。

## 3 两种入口

```c
/* 与 C++ 版相同的用法：先建句柄、再 Load，然后用 setter 配置 */
RAGAnalyzerHandle h = RAGAnalyzer_Create("/usr/share/infinity/resource");
RAGAnalyzer_Load(h);
RAGAnalyzer_SetLanguage(h, "English");
RAGAnalyzer_SetFineGrained(h, false);
char *tokens = RAGAnalyzer_Tokenize(h, "Hello, world!");

/* 或者直接按索引名打开：fine/语言后缀由名字决定，与 CREATE INDEX 的 ANALYZER 同源 */
RAGAnalyzerHandle fine = RAGAnalyzer_Open("rag-fine", "/usr/share/infinity/resource");
```

`RAGAnalyzer_Open` 接受 `parse_analyzer_name` 的全部族：`rag[-<lang>][-fine]`、`standard-<language>`、`chinese`/`traditional`/`ik[-fine]`、`japanese`、`korean`、`ngram-<n>`、`whitespace-<delims>`（`keyword` 同义）、`sparsegram-…`、`rankfeatures`。非 `rag` 族的句柄：`Tokenize`/`Analyze`/`TokenizeWithPosition` 可用，`FineGrainedTokenize` 返回 NULL，`GetTermFreq`/`GetTermTag` 返回 0/NULL，`Copy` 返回 NULL（需要几个就 `Open` 几个）。

## 4 所有权与线程

- 所有返回的 `char*` 都是 `malloc` 出来的，用 `free()` 释放（或 `RAGAnalyzer_FreeString`）。`RAGTokenList` 用 `RAGAnalyzer_FreeTokenList` 释放。
- `RAGAnalyzer_Analyze` 回调里的 token 文本是 NUL 结尾的，只在回调期间有效。
- 一个句柄不支持多线程并发使用；`RAGAnalyzer_Copy` 出来的句柄共享词典（`Arc`），复制很便宜——这正是池的用法。
- 每个入口都 `catch_unwind`，panic 不会穿过 FFI；失败原因用 `RAGAnalyzer_LastError()`（线程局部）读取。
- 输入必须是合法 UTF-8，否则返回 NULL 并在 `LastError` 里说明。

## 5 与 C++ 版的一致性（实测）

同一份 `/usr/share/infinity/resource`、同一段驱动（`crates/analyzer-ffi/parity/driver.c`），分别链接 C++ 的 `librag_tokenizer_c_api.a` 与本 crate 的 `librag_analyzer.a`，比对 217 行输出（10 段中英混排文本 × 粗粒度/位置/细粒度，加细粒度串、7 种语言、6 个词频/词性、Copy）：

| 用例 | 结果 |
|---|---|
| 默认（English）粗粒度 token | 逐字节相同 |
| `TokenizeWithPosition` 的 token 与 offset | 相同 |
| 细粒度（`SetFineGrained` + `FineGrainedTokenize`） | 相同 |
| `GetTermFreq` / `GetTermTag` | 相同 |
| `RAGAnalyzer_Copy` 之后的行为 | 相同 |
| `SetLanguage` 为荷兰语/德语/斯洛伐克语/捷克语，且文本含非 ASCII token | **不同** |

差异只出现在非英语语言上，原因在于 RAGFlow 仓库里那份 `rag_analyzer.cpp` 与本仓库参考版（`src/common/analyzer/`）不是同一版本：

- 参考版只在 `slovak`/`czech` 上折叠变音（`fold_diacritics_`）并关闭词干；那份副本没有这个开关，却对德语/荷兰语的非 ASCII token 做折叠与词干（`h ä user` 对 `h a user`）。
- 参考版的词干入口用 `[a-zA-Z_-]+$` 判断词尾，非 ASCII 结尾的 token 原样保留；本 crate 用同一判据。

结论：本 crate 与**引擎**的分词一致（索引端也走它），与 RAGFlow 当前链接的那份旧 C++ 在非英语上有出入。切换前需要确认 RAGFlow 是否依赖那些语言的旧 token——`content_ltks` 只在 ES/OceanBase 路径上由客户端计算。

复跑方式：

```bash
cc -O1 -o cpp_driver driver.c -I<ragflow>/internal/binding/cpp \
   <ragflow>/internal/binding/cpp/cmake-build-release/librag_tokenizer_c_api.a \
   -lstdc++ -lm -lpthread -lpcre2-8
cc -O1 -o rust_driver driver.c -I../../crates/analyzer-ffi/include \
   -L../../target/release -lrag_analyzer -lm -lpthread
LD_LIBRARY_PATH=../../target/release ./rust_driver /usr/share/infinity/resource | grep '^R ' > rust.out
./cpp_driver /usr/share/infinity/resource | grep '^R ' > cpp.out
diff cpp.out rust.out
```

## 6 构建与链接

Go（cgo）静态链接：

```go
// #cgo LDFLAGS: ${SRCDIR}/../../../target/release/librag_analyzer.a -lm -lpthread -ldl
// #include "rag_analyzer_c_api.h"
```

动态链接时把 `librag_analyzer.so` 放进 `ldconfig` 路径，或加 `-Wl,-rpath,<dir>`。crate 的依赖只有 `analyzer` 和 `libc`，不会被 RocksDB / tokio 拖进来。
