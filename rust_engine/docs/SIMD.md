# SIMD：参考库用了哪些，端口用了哪些

参考实现的 SIMD 集中在 `src/common/simd/`（编译期按 `-march=native` 选
`__AVX512F__` / `__AVX2__` / `__SSE2__`，每个函数都有对应的 `*BF` 标量版本）：

| 参考文件 | 用途 | 端口对应 | 状态 |
|---|---|---|---|
| `hnsw_simd_func.cppm` | `F32L2*` / `F32IP*` / `F32Cos*`（HNSW 的距离内核），以及 `I8*` / `U8*` 量化版本 | `infinity-index/src/simd.rs` | ✅ F32；I8/U8 未移植（见 §3） |
| `distance_simd_functions_impl.cpp` | 按类型的通用距离（float / int8 / uint8 / 二值） | 同上（只有 f32 稠密路径） | 部分 |
| `simdbitpacking`（third_party fastpfor） | posting list 的 SIMD-BP128 编解码 | `infinity-storage/src/inverted_format.rs` | ✅ 见 `FULLTEXT_BENCH.md` §4.6 |
| `maxsim_simd_funcs.cppm` | 多向量（late-interaction）的 max-sim | `infinity-index/src/multivector.rs` 的 `dot` | ✅ |
| `batch_bm25_simd_funcs.cppm` | 批量 BM25 打分 | WAND 逐文档打分（`wand.rs`） | 结构不同，未做批量 |
| `search_top_k*.cppm` / `emvb_simd_funcs.cppm` / `diskann_simd_func.cppm` | top-k 选择、EMVB 张量、DiskANN | `emvb.rs` / `rabitq.rs` 等 | 未做 |

## 1. 距离内核

`simd.rs` 里是三个 kernel，形状与参考的 AVX 变体一致：`f32x8` 累加器、`dim % 8`
的标量尾巴、最后一次 `reduce_sum`（参考是 `_mm256_store_ps` + 8 项相加）。

```rust
while i + LANES <= n {
    let diff = f32x8::from_slice(&a[i..]) - f32x8::from_slice(&b[i..]);
    acc += diff * diff;          // 不是 mul_add：参考 F32L2AVX512 里 FMA 是注释掉的
    i += LANES;
}
```

- `l2_sq` ← `F32L2`；`ip` ← `F32IP`；`cos_parts` / `cosine` / `cos_dist` ← `F32Cos`
  （一次遍历同时累加 dot、|a|²、|b|²，对应 `F32CosAVX512` 的三累加器）。
- 调用点：`vector::l2_sq` / `vector::plain_distance`（HNSW 的 `distance_query`、
  `distance_nodes`）、`knn_flat::distance`、`ivf::{dot,cosine}`、`lsg::distance`、
  `multivector::dot`。
- 求和顺序按 lane 分组，所以与逐元素标量求和的最后几位可能不同——参考的 SIMD 路径
  同样如此。单测用相对误差 1e-5 覆盖 0..=300 各种长度（含每条尾巴），并单独验证零
  向量的 cosine 为 0、长度不一致时只比较公共前缀。

## 2. 实测（SIFT 200k × 128，M=16，ef_construction=200；2000 条查询，topk=100，ef=200）

同一台机器、同一份数据，标量版与 SIMD 版各建一次索引：

| | 标量 | SIMD | 变化 |
|---|---|---|---|
| HNSW 建索引（200k） | 32.9 s | 27.4 s | **−17%** |
| 查询 qps | 248 | 263 | **+6%** |
| 查询 p50 | 4.10 ms | 3.87 ms | −6% |
| 距离内核（dim=128，单次） | 51.9 ns | 22.9 ns | **2.3×** |

查询只涨 6% 是因为距离不占大头：`perf` 上 `plain_distance` 约 8%，其余是结果行
物化（`embedding_element_text` 的浮点转文本约 20%）与分配器开销（malloc/free + memmove
约 20%）。按 8% × (1 − 1/2.3)
算，与实测一致。

内核只有 2.3×（而不是 8×）是因为 128 维、每向量 512 B 的负载是**带宽**受限：22.9 ns
对应约 22 GB/s，正好是单核可用的内存带宽。因此 `-C target-cpu=native`（打开 AVX2）
在这份数据上没有稳定收益（实测 20.5–29.6 ns，噪声大于差异），默认的 SSE2 基线已经够用，
也就没有改构建参数。

## 3. 没做的

- **I8 / U8 距离内核**（参考 `I8L2AVX2` 等）：端口的稠密 store 只有 `f32`，
  `EMBEDDING(TINYINT, …)` 的查询目前走的是同一条 f32 路径（`test_knn_hnsw_i8/u8`
  是已知的未通过用例，与这里无关）。等 TINYINT 有自己的定长 store 再加。
- **批量 top-k 选择**（`search_top_k_sgemm`）：端口用 `BinaryHeap`/排序，在 ef≤几百的
  规模上不是瓶颈。
- **批量 BM25**：端口的全文打分是 WAND 逐文档推进，没有"批量打分"这一步。
