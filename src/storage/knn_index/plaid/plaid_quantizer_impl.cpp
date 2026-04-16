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

module infinity_core:plaid_quantizer.impl;

import :plaid_quantizer;
import :local_file_handle;
import :infinity_exception;
import :logger;

import std;

namespace infinity {

PlaidQuantizer::PlaidQuantizer(u32 nbits, u32 embedding_dim)
    : nbits_(nbits), embedding_dim_(embedding_dim), n_buckets_(1u << nbits), packed_dim_((embedding_dim * nbits + 7) / 8) {
    if (nbits != 2 && nbits != 4) {
        UnrecoverableError("PlaidQuantizer: nbits must be 2 or 4");
    }
    if (embedding_dim == 0) {
        UnrecoverableError("PlaidQuantizer: embedding_dim must be > 0");
    }
    BuildLookupTables();
}

void PlaidQuantizer::BuildLookupTables() {
    // Build bit reversal map for unpacking
    const u32 nbits_mask = (1u << nbits_) - 1;
    byte_reversed_bits_map_ = std::make_unique<u8[]>(256);

    for (u32 i = 0; i < 256; ++i) {
        u32 val = i;
        u32 out = 0;
        i32 pos = 8;

        while (pos >= static_cast<i32>(nbits_)) {
            u32 segment = (val >> (pos - nbits_)) & nbits_mask;

            // Reverse bits within segment
            u32 rev_segment = 0;
            for (u32 k = 0; k < nbits_; ++k) {
                if (segment & (1u << k)) {
                    rev_segment |= 1u << (nbits_ - 1 - k);
                }
            }

            out |= rev_segment;
            if (pos > static_cast<i32>(nbits_)) {
                out <<= nbits_;
            }
            pos -= nbits_;
        }
        byte_reversed_bits_map_[i] = static_cast<u8>(out);
    }

    // Build lookup table for bucket weight indices
    const u32 keys_per_byte = 8 / nbits_;
    bucket_weight_indices_lookup_ = std::make_unique<u8[]>(256 * keys_per_byte);
    const u32 mask = (1u << nbits_) - 1;

    for (u32 byte_val = 0; byte_val < 256; ++byte_val) {
        for (u32 k = 0; k < keys_per_byte; ++k) {
            u32 shift = (keys_per_byte - 1 - k) * nbits_;
            u32 index = (byte_val >> shift) & mask;
            bucket_weight_indices_lookup_[byte_val * keys_per_byte + k] = static_cast<u8>(index);
        }
    }
}

void PlaidQuantizer::Train(const f32 *residuals, u64 n_embeddings) {
    std::unique_lock lock(rw_mutex_);

    if (n_embeddings == 0) {
        UnrecoverableError("PlaidQuantizer::Train: n_embeddings must be > 0");
    }

    // ── Memory-bounded Compress (P3) ──────────────────────────────────
    // Compute average residual first (streaming, O(1) extra memory)
    double sum = 0.0;
    const u64 total_values = n_embeddings * embedding_dim_;
    for (u64 i = 0; i < total_values; ++i) {
        sum += residuals[i];
    }
    avg_residual_ = static_cast<f32>(sum / static_cast<double>(total_values));

    // Determine how many residual values we can afford to process.
    // If total exceeds the limit, sample down to MAX_RESIDUAL_VALUES.
    const u64 max_values = MAX_RESIDUAL_VALUES;
    const u64 max_embeddings_for_limit = std::max<u64>(1, max_values / embedding_dim_);

    std::vector<f32> all_residuals;

    if (total_values <= max_values) {
        // Fast path: data fits within limit, copy all
        all_residuals.reserve(total_values);
        for (u64 i = 0; i < total_values; ++i) {
            all_residuals.push_back(residuals[i] - avg_residual_);
        }
    } else {
        // Memory-bounded path: randomly sample embeddings to fit within limit
        const u64 sample_count = max_embeddings_for_limit;
        LOG_INFO(fmt::format("PlaidQuantizer::Train: Memory-bounded sampling {} from {} embeddings (total {} values > limit {})",
                             sample_count, n_embeddings, total_values, max_values));

        // Reservoir sampling for embedding indices
        std::vector<u64> sample_indices;
        sample_indices.reserve(sample_count);
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<u64> dist(0, std::numeric_limits<u64>::max());

        for (u64 i = 0; i < sample_count; ++i) {
            sample_indices.push_back(i);
        }
        for (u64 i = sample_count; i < n_embeddings; ++i) {
            u64 j = dist(gen) % (i + 1);
            if (j < sample_count) {
                sample_indices[j] = i;
            }
        }

        // Collect sampled residuals
        all_residuals.reserve(sample_count * embedding_dim_);
        for (u64 i = 0; i < sample_count; ++i) {
            const u64 idx = sample_indices[i];
            const f32 *emb = residuals + idx * embedding_dim_;
            for (u32 d = 0; d < embedding_dim_; ++d) {
                all_residuals.push_back(emb[d] - avg_residual_);
            }
        }
    }

    // Sort for quantile computation
    std::sort(all_residuals.begin(), all_residuals.end());

    // ── Compute bucket cutoffs at quantile boundaries ────────────────
    // Equivalent to next-plaid's: quantiles at i/n_buckets for i = 1..n_buckets-1
    bucket_cutoffs_ = std::make_unique<f32[]>(n_buckets_ - 1);
    for (u32 i = 1; i < n_buckets_; ++i) {
        f64 quantile = static_cast<f64>(i) / n_buckets_;
        u64 idx = static_cast<u64>(quantile * all_residuals.size());
        idx = std::min(idx, all_residuals.size() - 1);
        bucket_cutoffs_[i - 1] = all_residuals[idx];
    }

    // ── Compute bucket weights using quantile midpoints (next-plaid style) ──
    // next-plaid: bucket_weights = quantiles at (i+0.5)/n_buckets
    // This is more robust than per-bucket averaging and avoids an extra pass.
    bucket_weights_ = std::make_unique<f32[]>(n_buckets_);
    for (u32 i = 0; i < n_buckets_; ++i) {
        f64 quantile = (static_cast<f64>(i) + 0.5) / n_buckets_;
        u64 idx = static_cast<u64>(quantile * all_residuals.size());
        idx = std::min(idx, all_residuals.size() - 1);
        bucket_weights_[i] = all_residuals[idx] + avg_residual_;
    }

    // Free the sorted residuals early to release memory before returning
    all_residuals.clear();
    all_residuals.shrink_to_fit();

    LOG_INFO(fmt::format("PlaidQuantizer::Train: Trained with {} residual values, avg_residual={:.6f}",
                         std::min(total_values, max_values), avg_residual_));
}

std::unique_ptr<u8[]> PlaidQuantizer::Quantize(const f32 *residuals, u64 n_embeddings, u32 &out_packed_dim) const {
    std::shared_lock lock(rw_mutex_);

    if (!bucket_cutoffs_) {
        UnrecoverableError("PlaidQuantizer::Quantize: must call Train first");
    }

    out_packed_dim = packed_dim_;
    auto packed = std::make_unique<u8[]>(n_embeddings * packed_dim_);
    std::fill_n(packed.get(), n_embeddings * packed_dim_, 0);

    for (u64 i = 0; i < n_embeddings; ++i) {
        const f32 *residual = residuals + i * embedding_dim_;
        u8 *packed_row = packed.get() + i * packed_dim_;
        u32 bit_idx = 0;

        for (u32 j = 0; j < embedding_dim_; ++j) {
            f32 val = residual[j] - avg_residual_;

            // Find bucket using binary search (equivalent to searchsorted)
            u32 bucket = 0;
            for (u32 k = 0; k < n_buckets_ - 1; ++k) {
                if (val > bucket_cutoffs_[k]) {
                    bucket = k + 1;
                } else {
                    break;
                }
            }

            // Pack bits into bytes
            for (u32 b = 0; b < nbits_; ++b) {
                u8 bit = (bucket >> b) & 1;
                u32 byte_idx = bit_idx / 8;
                u32 bit_pos = 7 - (bit_idx % 8);
                packed_row[byte_idx] |= bit << bit_pos;
                bit_idx++;
            }
        }
    }

    return packed;
}

void PlaidQuantizer::Dequantize(const u32 *centroid_ids,
                                const u8 *packed_residuals,
                                u64 n_embeddings,
                                f32 *output,
                                const f32 *centroids_data,
                                u32 n_centroids) const {
    std::shared_lock lock(rw_mutex_);

    if (!bucket_weights_) {
        UnrecoverableError("PlaidQuantizer::Dequantize: must call Train first");
    }

    [[maybe_unused]] const u32 keys_per_byte = 8 / nbits_;

    for (u64 i = 0; i < n_embeddings; ++i) {
        u32 centroid_id = centroid_ids[i];
        const u8 *packed = packed_residuals + i * packed_dim_;
        f32 *out = output + i * embedding_dim_;

        // Get centroid
        const f32 *centroid = centroids_data + centroid_id * embedding_dim_;

        // Unpack residuals
        u32 residual_idx = 0;
        for (u32 j = 0; j < packed_dim_; ++j) {
            u8 byte_val = packed[j];
            u8 reversed = byte_reversed_bits_map_[byte_val];
            const u8 *indices = &bucket_weight_indices_lookup_[reversed * keys_per_byte];

            for (u32 k = 0; k < keys_per_byte && residual_idx < embedding_dim_; ++k) {
                u32 bucket_idx = indices[k];
                out[residual_idx] = centroid[residual_idx] + bucket_weights_[bucket_idx];
                residual_idx++;
            }
        }

        // Normalize
        f32 norm_sq = 0.0f;
        for (u32 j = 0; j < embedding_dim_; ++j) {
            norm_sq += out[j] * out[j];
        }
        f32 norm = std::sqrt(norm_sq);
        if (norm > 1e-12f) {
            for (u32 j = 0; j < embedding_dim_; ++j) {
                out[j] /= norm;
            }
        }
    }
}

std::unique_ptr<f32[]> PlaidQuantizer::GetIPDistanceTable(const f32 *query) const {
    std::shared_lock lock(rw_mutex_);

    if (!bucket_weights_) {
        UnrecoverableError("PlaidQuantizer::GetIPDistanceTable: must call Train first");
    }

    // Compute IP table: [embedding_dim_, n_buckets_]
    auto table = std::make_unique<f32[]>(embedding_dim_ * n_buckets_);

    for (u32 d = 0; d < embedding_dim_; ++d) {
        for (u32 b = 0; b < n_buckets_; ++b) {
            table[d * n_buckets_ + b] = query[d] * bucket_weights_[b];
        }
    }

    return table;
}

f32 PlaidQuantizer::GetSingleIPDistance(u32 embedding_id,
                                        u32 query_id,
                                        u32 query_num,
                                        const f32 *ip_table,
                                        const u8 *packed_residuals,
                                        const u32 *centroid_ids,
                                        const f32 *centroids_data) const {
    // No lock needed: after Train(), all accessed members (packed_dim_, nbits_,
    // byte_reversed_bits_map_, bucket_weight_indices_lookup_) are immutable.
    // This function is called in tight inner loops during ExactScore, so
    // removing the lock avoids significant contention overhead.

    const u32 keys_per_byte = 8 / nbits_;
    u32 centroid_id = centroid_ids[embedding_id];
    const u8 *packed = packed_residuals + embedding_id * packed_dim_;
    const f32 *centroid = centroids_data + centroid_id * embedding_dim_;

    f32 result = 0.0f;
    u32 residual_idx = 0;

    for (u32 j = 0; j < packed_dim_; ++j) {
        u8 byte_val = packed[j];
        u8 reversed = byte_reversed_bits_map_[byte_val];
        const u8 *indices = &bucket_weight_indices_lookup_[reversed * keys_per_byte];

        for (u32 k = 0; k < keys_per_byte && residual_idx < embedding_dim_; ++k) {
            u32 bucket_idx = indices[k];
            // centroid[d] * query[d] + residual[d] * query[d]
            // = centroid[d] * query[d] + bucket_weight * query[d]
            result += centroid[residual_idx] * ip_table[residual_idx * n_buckets_ + bucket_idx];
            residual_idx++;
        }
    }

    return result;
}

void PlaidQuantizer::GetMultipleIPDistance(u32 embedding_offset,
                                           u32 n_embeddings,
                                           u32 query_id,
                                           u32 query_num,
                                           const f32 *ip_table,
                                           const u8 *packed_residuals,
                                           const u32 *centroid_ids,
                                           const f32 *centroids_data,
                                           f32 *output) const {
    for (u32 i = 0; i < n_embeddings; ++i) {
        output[i] = GetSingleIPDistance(embedding_offset + i, query_id, query_num, ip_table, packed_residuals, centroid_ids, centroids_data);
    }
}

void PlaidQuantizer::Save(LocalFileHandle &file_handle) const {
    std::shared_lock lock(rw_mutex_);

    file_handle.Append(&nbits_, sizeof(nbits_));
    file_handle.Append(&embedding_dim_, sizeof(embedding_dim_));
    file_handle.Append(&avg_residual_, sizeof(avg_residual_));

    // Save bucket cutoffs
    u32 n_cutoffs = n_buckets_ - 1;
    file_handle.Append(&n_cutoffs, sizeof(n_cutoffs));
    if (n_cutoffs > 0) {
        file_handle.Append(bucket_cutoffs_.get(), n_cutoffs * sizeof(f32));
    }

    // Save bucket weights
    file_handle.Append(&n_buckets_, sizeof(n_buckets_));
    file_handle.Append(bucket_weights_.get(), n_buckets_ * sizeof(f32));
}

void PlaidQuantizer::Load(LocalFileHandle &file_handle) {
    std::unique_lock lock(rw_mutex_);

    u32 nbits, embedding_dim;
    file_handle.Read(&nbits, sizeof(nbits));
    file_handle.Read(&embedding_dim, sizeof(embedding_dim));

    if (nbits != nbits_ || embedding_dim != embedding_dim_) {
        UnrecoverableError("PlaidQuantizer::Load: dimension mismatch");
    }

    file_handle.Read(&avg_residual_, sizeof(avg_residual_));

    // Load bucket cutoffs
    u32 n_cutoffs;
    file_handle.Read(&n_cutoffs, sizeof(n_cutoffs));
    if (n_cutoffs > 0) {
        bucket_cutoffs_ = std::make_unique<f32[]>(n_cutoffs);
        file_handle.Read(bucket_cutoffs_.get(), n_cutoffs * sizeof(f32));
    }

    // Load bucket weights
    u32 n_buckets;
    file_handle.Read(&n_buckets, sizeof(n_buckets));
    if (n_buckets != n_buckets_) {
        UnrecoverableError("PlaidQuantizer::Load: n_buckets mismatch");
    }
    bucket_weights_ = std::make_unique<f32[]>(n_buckets_);
    file_handle.Read(bucket_weights_.get(), n_buckets_ * sizeof(f32));

    // Rebuild lookup tables
    BuildLookupTables();
}

void PlaidQuantizer::SaveToPtr(void *ptr, size_t &offset) const {
    std::shared_lock lock(rw_mutex_);
    char *dst = static_cast<char *>(ptr);

    auto append = [&dst, &offset](const void *src, size_t len) {
        std::memcpy(dst + offset, src, len);
        offset += len;
    };

    // Save nbits_ and embedding_dim_ (consistent with Save() format)
    append(&nbits_, sizeof(nbits_));
    append(&embedding_dim_, sizeof(embedding_dim_));
    append(&avg_residual_, sizeof(avg_residual_));

    // Save bucket cutoffs
    u32 n_cutoffs = n_buckets_ - 1;
    append(&n_cutoffs, sizeof(n_cutoffs));
    if (n_cutoffs > 0) {
        append(bucket_cutoffs_.get(), n_cutoffs * sizeof(f32));
    }

    // Save bucket weights
    append(&n_buckets_, sizeof(n_buckets_));
    append(bucket_weights_.get(), n_buckets_ * sizeof(f32));
}

void PlaidQuantizer::LoadFromPtr(void *ptr, size_t &offset) {
    std::unique_lock lock(rw_mutex_);
    char *src = static_cast<char *>(ptr);

    auto read = [&src, &offset](void *dst, size_t len) {
        std::memcpy(dst, src + offset, len);
        offset += len;
    };

    // Read nbits_ and embedding_dim_ (consistent with Save() format)
    u32 nbits, embedding_dim;
    read(&nbits, sizeof(nbits));
    read(&embedding_dim, sizeof(embedding_dim));

    if (nbits != nbits_ || embedding_dim != embedding_dim_) {
        UnrecoverableError(
            fmt::format("PlaidQuantizer::LoadFromPtr: dimension mismatch: read nbits={}, embedding_dim={}, expected nbits={}, embedding_dim={}",
                        nbits,
                        embedding_dim,
                        nbits_,
                        embedding_dim_));
    }

    read(&avg_residual_, sizeof(avg_residual_));

    // Load bucket cutoffs
    u32 n_cutoffs;
    read(&n_cutoffs, sizeof(n_cutoffs));
    if (n_cutoffs > 0) {
        bucket_cutoffs_ = std::make_unique<f32[]>(n_cutoffs);
        read(bucket_cutoffs_.get(), n_cutoffs * sizeof(f32));
    }

    // Load bucket weights
    u32 n_buckets;
    read(&n_buckets, sizeof(n_buckets));
    if (n_buckets != n_buckets_) {
        UnrecoverableError(fmt::format("PlaidQuantizer::LoadFromPtr: n_buckets mismatch: read={}, expected={}", n_buckets, n_buckets_));
    }
    bucket_weights_ = std::make_unique<f32[]>(n_buckets_);
    read(bucket_weights_.get(), n_buckets_ * sizeof(f32));

    // Rebuild lookup tables
    BuildLookupTables();
}

size_t PlaidQuantizer::MemUsage() const {
    std::shared_lock lock(rw_mutex_);

    size_t size = sizeof(PlaidQuantizer);
    size += (n_buckets_ - 1) * sizeof(f32); // bucket_cutoffs_
    size += n_buckets_ * sizeof(f32);       // bucket_weights_
    size += 256 * sizeof(u8);               // byte_reversed_bits_map_
    size += 256 * sizeof(u8);               // bucket_weight_indices_lookup_

    return size;
}

void PlaidQuantizer::CopyFrom(const PlaidQuantizer &other) {
    std::unique_lock lock(rw_mutex_);
    std::shared_lock other_lock(other.rw_mutex_);

    if (other.nbits_ != nbits_ || other.embedding_dim_ != embedding_dim_) {
        UnrecoverableError("PlaidQuantizer::CopyFrom: dimension mismatch");
    }

    avg_residual_ = other.avg_residual_;

    // Copy bucket cutoffs
    if (other.bucket_cutoffs_) {
        bucket_cutoffs_ = std::make_unique<f32[]>(n_buckets_ - 1);
        std::copy_n(other.bucket_cutoffs_.get(), n_buckets_ - 1, bucket_cutoffs_.get());
    }

    // Copy bucket weights
    if (other.bucket_weights_) {
        bucket_weights_ = std::make_unique<f32[]>(n_buckets_);
        std::copy_n(other.bucket_weights_.get(), n_buckets_, bucket_weights_.get());
    }

    // Note: lookup tables are already built in constructor
}

} // namespace infinity
