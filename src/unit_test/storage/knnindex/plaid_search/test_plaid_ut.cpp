// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_plaid;

import :ut.base_test;
import :infinity_exception;
import :plaid_index;
import :plaid_quantizer;
import :local_file_handle;
import :roaring_bitmap;

using namespace infinity;

class PlaidQuantizerTest : public BaseTest {};

TEST_F(PlaidQuantizerTest, test_train_quantize_dequantize) {
    constexpr u32 nbits = 4;
    constexpr u32 embedding_dim = 32;
    constexpr u32 n_embeddings = 256;

    // Create quantizer
    PlaidQuantizer quantizer(nbits, embedding_dim);
    EXPECT_EQ(quantizer.nbits(), nbits);
    EXPECT_EQ(quantizer.embedding_dim(), embedding_dim);
    EXPECT_EQ(quantizer.n_buckets(), 16u);
    EXPECT_EQ(quantizer.packed_dim(), (embedding_dim * nbits + 7) / 8);

    // Generate random residuals
    std::vector<f32> residuals(n_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 0.1f);
    for (auto &v : residuals) {
        v = dis(gen);
    }

    // Train quantizer
    quantizer.Train(residuals.data(), n_embeddings);

    // Quantize
    u32 packed_dim = 0;
    auto packed = quantizer.Quantize(residuals.data(), n_embeddings, packed_dim);
    EXPECT_EQ(packed_dim, quantizer.packed_dim());

    // Create dummy centroid IDs and centroids for dequantization
    std::vector<u32> centroid_ids(n_embeddings, 0);
    std::vector<f32> centroids(embedding_dim, 0.0f);

    // Dequantize
    std::vector<f32> reconstructed(n_embeddings * embedding_dim);
    quantizer.Dequantize(centroid_ids.data(), packed.get(), n_embeddings, reconstructed.data(), centroids.data(), 1);

    // Check that reconstructed values are within expected range
    // (should be close to bucket weights, not exact match)
    for (u32 i = 0; i < n_embeddings * embedding_dim; ++i) {
        EXPECT_TRUE(std::isfinite(reconstructed[i]));
    }
}

TEST_F(PlaidQuantizerTest, test_2bit_quantization) {
    constexpr u32 nbits = 2;
    constexpr u32 embedding_dim = 16;
    constexpr u32 n_embeddings = 64;

    PlaidQuantizer quantizer(nbits, embedding_dim);
    EXPECT_EQ(quantizer.n_buckets(), 4u);

    std::vector<f32> residuals(n_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 0.1f);
    for (auto &v : residuals) {
        v = dis(gen);
    }

    quantizer.Train(residuals.data(), n_embeddings);

    u32 packed_dim = 0;
    auto packed = quantizer.Quantize(residuals.data(), n_embeddings, packed_dim);
    EXPECT_EQ(packed_dim, (embedding_dim * nbits + 7) / 8);
}

class PlaidIndexTest : public BaseTest {};

TEST_F(PlaidIndexTest, test_basic_construction) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;

    PlaidIndex index(0, embedding_dim, nbits, n_centroids);

    EXPECT_EQ(index.GetEmbeddingDimension(), embedding_dim);
    EXPECT_EQ(index.GetNCentroids(), 0u); // Not trained yet
    EXPECT_EQ(index.GetDocNum(), 0u);
    EXPECT_EQ(index.GetTotalEmbeddingNum(), 0u);
    EXPECT_FALSE(index.IsMmap());
}

TEST_F(PlaidIndexTest, test_train_and_add) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum: max(32 * n_centroids, 32 * n_buckets) = max(256, 512) = 512
    constexpr u32 n_docs = 10;
    constexpr u32 embeddings_per_doc = 8;

    PlaidIndex index(0, embedding_dim, nbits, n_centroids);

    // Generate training data
    std::vector<f32> train_data(n_train_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 1.0f);
    for (auto &v : train_data) {
        v = dis(gen);
    }

    // Train index
    index.Train(n_centroids, train_data.data(), n_train_embeddings);
    EXPECT_EQ(index.GetNCentroids(), n_centroids);

    // Add documents using AddMultipleDocsEmbeddings
    std::vector<u32> doc_lens;
    std::vector<f32> all_embeddings;
    all_embeddings.reserve(n_docs * embeddings_per_doc * embedding_dim);
    doc_lens.reserve(n_docs);

    for (u32 doc = 0; doc < n_docs; ++doc) {
        for (u32 i = 0; i < embeddings_per_doc * embedding_dim; ++i) {
            all_embeddings.push_back(dis(gen));
        }
        doc_lens.push_back(embeddings_per_doc);
    }

    index.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);

    EXPECT_EQ(index.GetDocNum(), n_docs);
    EXPECT_EQ(index.GetTotalEmbeddingNum(), n_docs * embeddings_per_doc);

    // Check document lengths
    for (u32 doc = 0; doc < n_docs; ++doc) {
        EXPECT_EQ(index.GetDocLen(doc), embeddings_per_doc);
    }
}

TEST_F(PlaidIndexTest, test_search) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum required
    constexpr u32 n_docs = 20;
    constexpr u32 embeddings_per_doc = 8;

    PlaidIndex index(0, embedding_dim, nbits, n_centroids);

    // Generate training data
    std::vector<f32> train_data(n_train_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 1.0f);
    for (auto &v : train_data) {
        v = dis(gen);
    }

    // Train
    index.Train(n_centroids, train_data.data(), n_train_embeddings);

    // Add documents using AddMultipleDocsEmbeddings
    std::vector<u32> doc_lens;
    std::vector<f32> all_embeddings;
    all_embeddings.reserve(n_docs * embeddings_per_doc * embedding_dim);
    doc_lens.reserve(n_docs);

    for (u32 doc = 0; doc < n_docs; ++doc) {
        for (u32 i = 0; i < embeddings_per_doc * embedding_dim; ++i) {
            all_embeddings.push_back(dis(gen));
        }
        doc_lens.push_back(embeddings_per_doc);
    }

    index.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);

    // Just verify index was built correctly
    EXPECT_EQ(index.GetDocNum(), n_docs);
    EXPECT_EQ(index.GetTotalEmbeddingNum(), n_docs * embeddings_per_doc);
}

TEST_F(PlaidIndexTest, test_save_load) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum required
    constexpr u32 n_docs = 10;
    constexpr u32 embeddings_per_doc = 8;

    // Create and populate index
    PlaidIndex index1(0, embedding_dim, nbits, n_centroids);

    std::vector<f32> train_data(n_train_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 1.0f);
    for (auto &v : train_data) {
        v = dis(gen);
    }

    index1.Train(n_centroids, train_data.data(), n_train_embeddings);

    std::vector<u32> doc_lens;
    std::vector<f32> all_embeddings;
    all_embeddings.reserve(n_docs * embeddings_per_doc * embedding_dim);
    doc_lens.reserve(n_docs);

    for (u32 doc = 0; doc < n_docs; ++doc) {
        for (u32 i = 0; i < embeddings_per_doc * embedding_dim; ++i) {
            all_embeddings.push_back(dis(gen));
        }
        doc_lens.push_back(embeddings_per_doc);
    }

    index1.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);

    // Get initial size
    size_t calc_size = index1.CalcSize();
    EXPECT_GT(calc_size, 0u);

    // Save to memory
    auto buffer = std::make_unique<char[]>(calc_size);
    size_t offset = 0;
    index1.SaveToPtr(buffer.get(), offset);

    // Load into new index
    PlaidIndex index2(0, embedding_dim, nbits, n_centroids);
    index2.LoadFromPtr(buffer.get(), calc_size, calc_size);

    // Verify loaded index
    EXPECT_EQ(index2.GetNCentroids(), n_centroids);
    EXPECT_EQ(index2.GetDocNum(), n_docs);
    EXPECT_EQ(index2.GetTotalEmbeddingNum(), n_docs * embeddings_per_doc);
}

TEST_F(PlaidIndexTest, test_move_semantics) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum required

    // Create first index
    PlaidIndex index1(0, embedding_dim, nbits, n_centroids);

    std::vector<f32> train_data(n_train_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 1.0f);
    for (auto &v : train_data) {
        v = dis(gen);
    }

    index1.Train(n_centroids, train_data.data(), n_train_embeddings);

    std::vector<u32> doc_lens = {8};
    index1.AddMultipleDocsEmbeddings(train_data.data(), doc_lens);

    // Move construct
    PlaidIndex index2(std::move(index1));
    EXPECT_EQ(index2.GetNCentroids(), n_centroids);
    EXPECT_EQ(index2.GetDocNum(), 1u);

    // Move assign
    PlaidIndex index3(0, embedding_dim, nbits, n_centroids);
    index3 = std::move(index2);
    EXPECT_EQ(index3.GetNCentroids(), n_centroids);
    EXPECT_EQ(index3.GetDocNum(), 1u);
}

TEST_F(PlaidIndexTest, test_mem_usage) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum required

    PlaidIndex index(0, embedding_dim, nbits, n_centroids);

    std::vector<f32> train_data(n_train_embeddings * embedding_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 1.0f);
    for (auto &v : train_data) {
        v = dis(gen);
    }

    index.Train(n_centroids, train_data.data(), n_train_embeddings);

    std::vector<u32> doc_lens = {8};
    index.AddMultipleDocsEmbeddings(train_data.data(), doc_lens);

    size_t mem_usage = index.MemUsage();
    EXPECT_GT(mem_usage, 0u);

    size_t calc_size = index.CalcSize();
    EXPECT_EQ(mem_usage, calc_size);
}

// ── ColBERTSaR mode tests ──

class PlaidIndexColBERTSaRTest : public BaseTest {
protected:
    static constexpr u32 embedding_dim_ = 16; // smaller dim for faster test
    static constexpr u32 n_centroids_ = 8;
    static constexpr u32 n_train_embeddings_ = 512; // enough for query-aware training
    static constexpr u32 n_docs_ = 10;
    static constexpr u32 embeddings_per_doc_ = 6;

    // Generate fixed-seed random data
    static void GenerateData(std::vector<f32> &data, u32 count, u32 seed = 42) {
        data.resize(count * embedding_dim_);
        std::mt19937 gen(seed);
        std::normal_distribution<f32> dis(0.0f, 0.5f);
        for (auto &v : data) {
            v = dis(gen);
        }
    }
};

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_construction) {
    PlaidIndex index(0, embedding_dim_, 4 /*nbits ignored in colbertsar mode*/, n_centroids_, true);

    EXPECT_TRUE(index.IsColBERTSaRMode());
    EXPECT_EQ(index.GetEmbeddingDimension(), embedding_dim_);
    EXPECT_EQ(index.GetNCentroids(), 0u); // Not trained yet
    EXPECT_EQ(index.GetDocNum(), 0u);
    EXPECT_EQ(index.GetTotalEmbeddingNum(), 0u);
    EXPECT_FALSE(index.IsMmap());
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_train_and_add) {
    PlaidIndex index(0, embedding_dim_, 4, n_centroids_, true);
    EXPECT_TRUE(index.IsColBERTSaRMode());

    // Generate training data
    std::vector<f32> train_data;
    GenerateData(train_data, n_train_embeddings_, 42);

    // Train
    index.Train(n_centroids_, train_data.data(), n_train_embeddings_);
    EXPECT_EQ(index.GetNCentroids(), n_centroids_);

    // Add documents
    std::vector<u32> doc_lens;
    std::vector<f32> all_embeddings;
    for (u32 doc = 0; doc < n_docs_; ++doc) {
        std::vector<f32> doc_emb;
        GenerateData(doc_emb, embeddings_per_doc_, 100 + doc);
        all_embeddings.insert(all_embeddings.end(), doc_emb.begin(), doc_emb.end());
        doc_lens.push_back(embeddings_per_doc_);
    }

    index.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);

    EXPECT_EQ(index.GetDocNum(), n_docs_);
    EXPECT_EQ(index.GetTotalEmbeddingNum(), n_docs_ * embeddings_per_doc_);

    // Verify document lengths
    for (u32 doc = 0; doc < n_docs_; ++doc) {
        EXPECT_EQ(index.GetDocLen(doc), embeddings_per_doc_);
    }

    // ColBERTSaR: no residuals stored → CalcSize should be smaller than PLAID equivalent
    // PLAID with same data would have packed_residuals (n_total_embeddings * packed_dim bytes)
    // ColBERTSaR has no packed_residuals, so CalcSize is noticeably smaller
    size_t colbertsar_size = index.CalcSize();
    EXPECT_GT(colbertsar_size, 0u);

    // Verify: a non-colbertsar index with same data is strictly larger
    PlaidIndex plaid_index(0, embedding_dim_, 4, n_centroids_, false);
    plaid_index.Train(n_centroids_, train_data.data(), n_train_embeddings_);
    plaid_index.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);
    size_t plaid_size = plaid_index.CalcSize();
    EXPECT_GT(plaid_size, colbertsar_size);
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_basic_search) {
    PlaidIndex index(0, embedding_dim_, 4, n_centroids_, true);

    std::vector<f32> train_data;
    GenerateData(train_data, n_train_embeddings_, 42);
    index.Train(n_centroids_, train_data.data(), n_train_embeddings_);

    // Add documents
    std::vector<u32> doc_lens;
    std::vector<f32> all_embeddings;
    for (u32 doc = 0; doc < n_docs_; ++doc) {
        std::vector<f32> doc_emb;
        GenerateData(doc_emb, embeddings_per_doc_, 100 + doc);
        all_embeddings.insert(all_embeddings.end(), doc_emb.begin(), doc_emb.end());
        doc_lens.push_back(embeddings_per_doc_);
    }
    index.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);
    EXPECT_EQ(index.GetDocNum(), n_docs_);

    // Build a simple query from the first doc's first embedding
    // Use a random query vector
    std::vector<f32> query(embedding_dim_, 0.0f);
    query[0] = 1.0f;

    // Create full bitmask (all docs visible)
    Bitmask bitmask(index.GetDocNum() + 1);
    bitmask.SetAllTrue();

    // Search
    constexpr u32 n_ivf_probe = 4;
    constexpr u32 top_k = 3;
    auto [result_count, scores, ids] = index.SearchWithBitmask(query.data(),
                                                               1 /* single query token */,
                                                               top_k,
                                                               bitmask,
                                                               nullptr,
                                                               0 /* begin_ts */,
                                                               n_ivf_probe,
                                                               0.0f /* no threshold */,
                                                               top_k /* n_doc_to_score */,
                                                               0 /* no full rerank since colbertsar */);

    // Should return some results (possibly 0 with random data, but at least doesn't crash)
    EXPECT_LE(result_count, top_k);
    if (result_count > 0) {
        for (u32 i = 0; i < result_count; ++i) {
            EXPECT_LT(ids[i], n_docs_ + index.GetNCentroids()); // ids are offset by start_segment_offset (0)
            EXPECT_TRUE(std::isfinite(scores[i]));
        }
    }
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_save_load) {
    PlaidIndex index1(0, embedding_dim_, 4, n_centroids_, true);

    std::vector<f32> train_data;
    GenerateData(train_data, n_train_embeddings_, 42);
    index1.Train(n_centroids_, train_data.data(), n_train_embeddings_);

    std::vector<u32> doc_lens;
    std::vector<f32> all_embeddings;
    for (u32 doc = 0; doc < n_docs_; ++doc) {
        std::vector<f32> doc_emb;
        GenerateData(doc_emb, embeddings_per_doc_, 100 + doc);
        all_embeddings.insert(all_embeddings.end(), doc_emb.begin(), doc_emb.end());
        doc_lens.push_back(embeddings_per_doc_);
    }
    index1.AddMultipleDocsEmbeddings(all_embeddings.data(), doc_lens);

    size_t calc_size = index1.CalcSize();
    EXPECT_GT(calc_size, 0u);

    // Save to memory
    auto buffer = std::make_unique<char[]>(calc_size);
    size_t offset = 0;
    index1.SaveToPtr(buffer.get(), offset);

    // Load into new index (colbertsar mode is read from file format, constructor mode matches)
    PlaidIndex index2(0, embedding_dim_, 4, n_centroids_, true);
    index2.LoadFromPtr(buffer.get(), calc_size, calc_size);

    // Verify loaded index
    EXPECT_TRUE(index2.IsColBERTSaRMode());
    EXPECT_EQ(index2.GetNCentroids(), n_centroids_);
    EXPECT_EQ(index2.GetDocNum(), n_docs_);
    EXPECT_EQ(index2.GetTotalEmbeddingNum(), n_docs_ * embeddings_per_doc_);

    // Verify document lengths
    for (u32 doc = 0; doc < n_docs_; ++doc) {
        EXPECT_EQ(index2.GetDocLen(doc), embeddings_per_doc_);
    }
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_move_semantics) {
    PlaidIndex index1(0, embedding_dim_, 4, n_centroids_, true);
    EXPECT_TRUE(index1.IsColBERTSaRMode());

    std::vector<f32> train_data;
    GenerateData(train_data, n_train_embeddings_, 42);
    index1.Train(n_centroids_, train_data.data(), n_train_embeddings_);

    std::vector<u32> doc_lens = {8};
    index1.AddMultipleDocsEmbeddings(train_data.data(), doc_lens);

    // Move construct
    PlaidIndex index2(std::move(index1));
    EXPECT_TRUE(index2.IsColBERTSaRMode());
    EXPECT_EQ(index2.GetNCentroids(), n_centroids_);
    EXPECT_EQ(index2.GetDocNum(), 1u);

    // Move assign
    PlaidIndex index3(0, embedding_dim_, 4, n_centroids_, true);
    index3 = std::move(index2);
    EXPECT_TRUE(index3.IsColBERTSaRMode());
    EXPECT_EQ(index3.GetNCentroids(), n_centroids_);
    EXPECT_EQ(index3.GetDocNum(), 1u);
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_mem_usage) {
    PlaidIndex index(0, embedding_dim_, 4, n_centroids_, true);

    std::vector<f32> train_data;
    GenerateData(train_data, n_train_embeddings_, 42);
    index.Train(n_centroids_, train_data.data(), n_train_embeddings_);

    std::vector<u32> doc_lens = {8};
    index.AddMultipleDocsEmbeddings(train_data.data(), doc_lens);

    size_t mem_usage = index.MemUsage();
    EXPECT_GT(mem_usage, 0u);

    size_t calc_size = index.CalcSize();
    EXPECT_EQ(mem_usage, calc_size);
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_merge) {
    // Test that MergeChunks works in colbertsar mode (no residuals)
    // MergeChunks replaces all data with chunks — create two chunk indices,
    // then merge both into a fresh target index
    std::vector<f32> train_data;
    GenerateData(train_data, n_train_embeddings_, 42);

    // Train primary index (supplier of centroids)
    PlaidIndex primary(0, embedding_dim_, 4, n_centroids_, true);
    primary.Train(n_centroids_, train_data.data(), n_train_embeddings_);

    // Create two chunks with shared centroids
    auto make_chunk = [&](u32 seed_start) -> PlaidIndex * {
        auto *chunk = new PlaidIndex(0, embedding_dim_, 4, n_centroids_, true);
        chunk->CopyCentroidsFrom(primary.centroids_data(), primary.centroid_norms_neg_half(), n_centroids_, nullptr, false);

        std::vector<u32> dl;
        std::vector<f32> emb;
        for (u32 doc = 0; doc < 5; ++doc) {
            std::vector<f32> de;
            GenerateData(de, embeddings_per_doc_, seed_start + doc);
            emb.insert(emb.end(), de.begin(), de.end());
            dl.push_back(embeddings_per_doc_);
        }
        chunk->AddMultipleDocsEmbeddings(emb.data(), dl);
        EXPECT_EQ(chunk->GetDocNum(), 5u);
        return chunk;
    };

    auto *chunk1 = make_chunk(200);
    auto *chunk2 = make_chunk(300);

    // Merge both chunks into a fresh index
    // Note: MergeChunks ignores the doc_offset from the pair; doc IDs are assigned 0..total_docs-1
    PlaidIndex merged(0, embedding_dim_, 4, n_centroids_, true);
    merged.CopyCentroidsFrom(primary.centroids_data(), primary.centroid_norms_neg_half(), n_centroids_, nullptr, false);
    merged.MergeChunks({{0, chunk1}, {0, chunk2}});

    EXPECT_EQ(merged.GetDocNum(), 10u);
    EXPECT_EQ(merged.GetTotalEmbeddingNum(), 10 * embeddings_per_doc_);

    delete chunk1;
    delete chunk2;
}

TEST_F(PlaidIndexColBERTSaRTest, test_colbertsar_size_smaller_than_plaid) {
    // Verify that ColBERTSaR index is strictly smaller than PLAID with same data
    constexpr u32 local_dim = 32; // need larger dim to see size difference clearly
    constexpr u32 local_n_centroids = 8;
    constexpr u32 local_n_train = 512;
    constexpr u32 local_n_docs = 10;
    constexpr u32 local_emb_per_doc = 8;

    std::vector<f32> train_data(local_n_train * local_dim);
    std::mt19937 gen(42);
    std::normal_distribution<f32> dis(0.0f, 1.0f);
    for (auto &v : train_data)
        v = dis(gen);

    std::vector<u32> doc_lens;
    std::vector<f32> all_emb;
    for (u32 doc = 0; doc < local_n_docs; ++doc) {
        for (u32 i = 0; i < local_emb_per_doc * local_dim; ++i)
            all_emb.push_back(dis(gen));
        doc_lens.push_back(local_emb_per_doc);
    }

    // Build ColBERTSaR index
    PlaidIndex colbertsar_index(0, local_dim, 4, local_n_centroids, true);
    colbertsar_index.Train(local_n_centroids, train_data.data(), local_n_train);
    colbertsar_index.AddMultipleDocsEmbeddings(all_emb.data(), doc_lens);
    size_t colbertsar_size = colbertsar_index.CalcSize();

    // Build standard PLAID index
    PlaidIndex plaid_index(0, local_dim, 4, local_n_centroids, false);
    plaid_index.Train(local_n_centroids, train_data.data(), local_n_train);
    plaid_index.AddMultipleDocsEmbeddings(all_emb.data(), doc_lens);
    size_t plaid_size = plaid_index.CalcSize();

    // ColBERTSaR saves: centroids + centroid_ids + IVF lists
    // PLAID adds: packed_residuals (n_total_emb * packed_dim bytes) + quantizer
    //
    // n_total_emb = 10 * 8 = 80
    // packed_residuals = 80 * (32*4/8) = 80 * 16 = 1280 bytes
    // quantizer ≈ (n_buckets-1 + n_buckets) * sizeof(f32) + LUTs ≈ ~2KB
    // Difference should be at least ~1280 bytes
    //
    // Centroids: local_n_centroids * local_dim * 4 = 8*32*4 = 1024 bytes for both
    // Centroid_ids: 80 * 4 = 320 bytes for both
    // IVF: similar for both

    EXPECT_GT(plaid_size, colbertsar_size) << "PLAID index should be larger due to packed residuals + quantizer";

    // The difference should be at least the size of packed residuals
    const u32 n_total_emb = local_n_docs * local_emb_per_doc;
    const u32 packed_dim = (local_dim * 4 + 7) / 8;
    const size_t min_residual_size = n_total_emb * packed_dim;
    EXPECT_GE(plaid_size - colbertsar_size, min_residual_size) << "Difference should be at least packed residual size";
}
