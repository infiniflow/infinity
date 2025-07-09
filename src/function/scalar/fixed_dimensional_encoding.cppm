// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include <random>

export module infinity_core:fixed_dimensional_encoding;

import :stl;

namespace infinity {

// Fixed Dimensional Encoding , "Fixed Dimensional Encoding" , this function take a tensor as input
// and output a single vector (the FDE), such that the dot product between a query FDE and a document
// FDE approximates the similarity between the original query Tensor and document Tensor. 
// See https://arxiv.org/abs/2405.19504 for further details.

export struct FixedDimensionalEncodingConfig {
    // Dimension of the input embeddings
    i32 dimension = 0;

    // Number of independent repetitions for FDE generation
    i32 num_repetitions = 1;

    // Number of SimHash projections used to partition space in each repetition
    i32 num_simhash_projections = 0;

    // Seed for the FDE generation process
    i32 seed = 1;

    // How embeddings are added to the FDE
    enum class EncodingType {
        DEFAULT_SUM = 0,
        AVERAGE = 1
    };
    EncodingType encoding_type = EncodingType::DEFAULT_SUM;

    // Dimension to which points are reduced via random projections
    i32 projection_dimension = 0;

    // The ProjectionType sets how the original embeddings are projected
    enum class ProjectionType {
        DEFAULT_IDENTITY = 0,
        AMS_SKETCH = 1
    };
    ProjectionType projection_type = ProjectionType::DEFAULT_IDENTITY;

    // If true, fill empty partitions with nearest point coordinates
    bool fill_empty_partitions = false;

    // Final projection dimension using Count-Sketch
    Optional<i32> final_projection_dimension = None;

    bool has_final_projection_dimension() const {
        return final_projection_dimension.has_value();
    }
};

// Tensor representation for multi-dimensional data
export class Tensor {
private:
    Vector<float> buffer_;
    i32 point_count_;
    i32 dimension_;

public:
    Tensor(const Vector<float>& data, i32 dimension)
        : buffer_(data), dimension_(dimension) {
        point_count_ = static_cast<i32>(data.size()) / dimension;
    }

    i32 GetPointCount() const { return point_count_; }
    i32 GetDimension() const { return dimension_; }

    Vector<float> GetPoint(i32 index) const {
        Vector<float> point(dimension_);
        for (i32 i = 0; i < dimension_; ++i) {
            point[i] = buffer_[index * dimension_ + i];
        }
        return point;
    }

    const float* GetRawData() const { return buffer_.data(); }
    SizeT GetTotalSize() const { return buffer_.size(); }
};

// Random number generator wrapper for reproducible results
export class RandomGenerator {
private:
    std::mt19937 engine_;
    std::normal_distribution<float> gaussian_dist_;
    std::bernoulli_distribution bernoulli_dist_;

public:
    explicit RandomGenerator(u32 seed) : engine_(seed), gaussian_dist_(0.0f, 1.0f), bernoulli_dist_(0.5) {}

    float GenerateGaussian();
    bool GenerateBernoulli();
    u32 GenerateUniform(u32 max_val);
};

// Hash-based partitioning system
export class HashPartitioner {
private:
    Vector<Vector<float>> projection_vectors_;
    i32 num_bits_;

public:
    HashPartitioner(i32 dimension, i32 num_bits, RandomGenerator& rng);
    u32 ComputePartition(const Vector<float>& point) const;
    u32 ComputeDistance(const Vector<float>& point, u32 target_partition) const;
};

// Dimensionality reduction through sparse random projections
export class SparseProjector {
private:
    Vector<Pair<u32, float>> projection_map_;
    i32 input_dim_;
    i32 output_dim_;

public:
    SparseProjector(i32 input_dim, i32 output_dim, RandomGenerator& rng);
    Vector<float> Project(const Vector<float>& input) const;
};

// Encoding accumulator for different aggregation strategies
export class EncodingAccumulator {
private:
    Vector<float> accumulator_;
    Vector<i32> counts_;
    bool use_averaging_;

public:
    EncodingAccumulator(i32 size, bool use_averaging);
    void AddToPartition(i32 partition_start, const Vector<float>& values);
    void FillEmptyPartition(i32 partition_start, const Vector<float>& values);
    Vector<float> Finalize() const;
};

// Main encoding pipeline orchestrator
export class EncodingPipeline {
private:
    const FixedDimensionalEncodingConfig& config_;

    bool ValidateInput(const Tensor& tensor) const;
    Vector<float> ProcessSingleRepetition(const Tensor& tensor, i32 rep_index) const;

public:
    explicit EncodingPipeline(const FixedDimensionalEncodingConfig& config) : config_(config) {}
    Optional<Vector<float>> Execute(const Tensor& tensor) const;
};

// Public API functions
export Optional<Vector<float>> GenerateFixedDimensionalEncoding(
    const Vector<float>& tensor_data, const FixedDimensionalEncodingConfig& config);

export Optional<Vector<float>> GenerateQueryFixedDimensionalEncoding(
    const Vector<float>& tensor_data, const FixedDimensionalEncodingConfig& config);

export Optional<Vector<float>> GenerateDocumentFixedDimensionalEncoding(
    const Vector<float>& tensor_data, const FixedDimensionalEncodingConfig& config);

// Legacy compatibility functions for testing
export namespace internal {
    u32 SimHashPartitionIndex(const Vector<float>& input_vector);
    u32 DistanceToSimHashPartition(const Vector<float>& input_vector, u32 index);
    Vector<float> ApplyCountSketchToVector(Span<const float> input_vector, u32 final_dimension, u32 seed);
}

}  // namespace infinity
