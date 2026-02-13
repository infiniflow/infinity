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

    // Add documents
    for (u32 doc = 0; doc < n_docs; ++doc) {
        std::vector<f32> doc_embeddings(embeddings_per_doc * embedding_dim);
        for (auto &v : doc_embeddings) {
            v = dis(gen);
        }
        index.AddOneDocEmbeddings(doc_embeddings.data(), embeddings_per_doc);
    }

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

    // Add documents
    for (u32 doc = 0; doc < n_docs; ++doc) {
        std::vector<f32> doc_embeddings(embeddings_per_doc * embedding_dim);
        for (auto &v : doc_embeddings) {
            v = dis(gen);
        }
        index.AddOneDocEmbeddings(doc_embeddings.data(), embeddings_per_doc);
    }

    // Create query
    std::vector<f32> query(embeddings_per_doc * embedding_dim);
    for (auto &v : query) {
        v = dis(gen);
    }

    // Search using GetQueryResult
    auto [n_result, scores, ids] = index.GetQueryResult(query.data(),
                                                        embeddings_per_doc,
                                                        n_centroids, // n_ivf_probe
                                                        -100.0f,     // centroid_score_threshold
                                                        n_docs,      // n_doc_to_score
                                                        10,          // n_full_scores
                                                        5            // top_k
    );

    EXPECT_LE(n_result, 5u);
    EXPECT_GT(n_result, 0u);

    // Results should be sorted by score (descending)
    for (u32 i = 1; i < n_result; ++i) {
        EXPECT_GE(scores[i - 1], scores[i]);
    }
}

TEST_F(PlaidIndexTest, test_reconstruct) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum required
    constexpr u32 n_docs = 5;
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

    // Add documents
    for (u32 doc = 0; doc < n_docs; ++doc) {
        std::vector<f32> doc_embeddings(embeddings_per_doc * embedding_dim);
        for (auto &v : doc_embeddings) {
            v = dis(gen);
        }
        index.AddOneDocEmbeddings(doc_embeddings.data(), embeddings_per_doc);
    }

    // Test reconstruct single document
    u32 doc_len = 0;
    auto reconstructed = index.ReconstructDocument(0, doc_len);
    EXPECT_EQ(doc_len, embeddings_per_doc);
    ASSERT_NE(reconstructed, nullptr);

    // Check all values are finite
    for (u32 i = 0; i < doc_len * embedding_dim; ++i) {
        EXPECT_TRUE(std::isfinite(reconstructed[i]));
    }

    // Test reconstruct multiple documents
    std::vector<u32> doc_ids = {0, 1, 2};
    std::vector<u32> doc_lens;
    auto reconstructed_docs = index.ReconstructDocuments(doc_ids, doc_lens);

    EXPECT_EQ(reconstructed_docs.size(), doc_ids.size());
    for (size_t i = 0; i < doc_ids.size(); ++i) {
        EXPECT_EQ(doc_lens[i], embeddings_per_doc);
        ASSERT_NE(reconstructed_docs[i], nullptr);
    }
}

TEST_F(PlaidIndexTest, test_expand_centroids) {
    constexpr u32 embedding_dim = 32;
    constexpr u32 nbits = 4;
    constexpr u32 n_centroids = 8;
    constexpr u32 n_train_embeddings = 512; // Minimum required
    constexpr u32 n_new_embeddings = 100;

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
    u32 initial_n_centroids = index.GetNCentroids();
    EXPECT_EQ(initial_n_centroids, n_centroids);

    // Generate new embeddings (with outliers)
    std::vector<f32> new_embeddings(n_new_embeddings * embedding_dim);
    // Make some embeddings as outliers by using very different distribution
    std::normal_distribution<f32> outlier_dis(10.0f, 2.0f);
    for (u32 i = 0; i < n_new_embeddings; ++i) {
        for (u32 d = 0; d < embedding_dim; ++d) {
            new_embeddings[i * embedding_dim + d] = outlier_dis(gen);
        }
    }

    // Expand centroids
    index.ExpandCentroids(new_embeddings.data(), n_new_embeddings);

    // Number of centroids may or may not increase depending on outliers
    u32 final_n_centroids = index.GetNCentroids();
    EXPECT_GE(final_n_centroids, initial_n_centroids);
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
    index1.AddOneDocEmbeddings(train_data.data(), 8);

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
    index.AddOneDocEmbeddings(train_data.data(), 8);

    size_t mem_usage = index.MemUsage();
    EXPECT_GT(mem_usage, 0u);

    size_t calc_size = index.CalcSize();
    EXPECT_EQ(mem_usage, calc_size);
}
