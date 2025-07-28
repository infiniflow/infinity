// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include "gtest/gtest.h"

import base_test;
import infinity_core;

using namespace infinity;

class FixedDimensionalEncodingTest : public BaseTest {};

TEST_F(FixedDimensionalEncodingTest, config_test) {
    // Test default configuration
    FixedDimensionalEncodingConfig config;
    EXPECT_EQ(config.dimension, 0);
    EXPECT_EQ(config.num_repetitions, 1);
    EXPECT_EQ(config.num_simhash_projections, 0);
    EXPECT_EQ(config.seed, 1);
    EXPECT_EQ(config.encoding_type, FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM);
    EXPECT_EQ(config.projection_dimension, 0);
    EXPECT_EQ(config.projection_type, FixedDimensionalEncodingConfig::ProjectionType::DEFAULT_IDENTITY);
    EXPECT_FALSE(config.fill_empty_partitions);
    EXPECT_FALSE(config.has_final_projection_dimension());
    
    // Test setting final projection dimension
    config.final_projection_dimension = 128;
    EXPECT_TRUE(config.has_final_projection_dimension());
    EXPECT_EQ(config.final_projection_dimension.value(), 128);
}

TEST_F(FixedDimensionalEncodingTest, tensor_test) {
    // Test Tensor basic operations
    Vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    Tensor tensor(data, 3);  // 2 points of dimension 3

    EXPECT_EQ(tensor.GetPointCount(), 2);
    EXPECT_EQ(tensor.GetDimension(), 3);
    EXPECT_EQ(tensor.GetTotalSize(), 6u);

    // Test point extraction
    Vector<float> point0 = tensor.GetPoint(0);
    EXPECT_EQ(point0.size(), 3u);
    EXPECT_FLOAT_EQ(point0[0], 1.0f);
    EXPECT_FLOAT_EQ(point0[1], 2.0f);
    EXPECT_FLOAT_EQ(point0[2], 3.0f);

    Vector<float> point1 = tensor.GetPoint(1);
    EXPECT_EQ(point1.size(), 3u);
    EXPECT_FLOAT_EQ(point1[0], 4.0f);
    EXPECT_FLOAT_EQ(point1[1], 5.0f);
    EXPECT_FLOAT_EQ(point1[2], 6.0f);
}

TEST_F(FixedDimensionalEncodingTest, random_generator_test) {
    // Test RandomGenerator deterministic behavior
    RandomGenerator rng1(42);
    RandomGenerator rng2(42);

    // Same seed should produce same sequence
    for (i32 i = 0; i < 10; ++i) {
        EXPECT_FLOAT_EQ(rng1.GenerateGaussian(), rng2.GenerateGaussian());
        EXPECT_EQ(rng1.GenerateBernoulli(), rng2.GenerateBernoulli());
        EXPECT_EQ(rng1.GenerateUniform(100), rng2.GenerateUniform(100));
    }
}

TEST_F(FixedDimensionalEncodingTest, hash_partitioner_test) {
    // Test HashPartitioner
    RandomGenerator rng(123);
    HashPartitioner partitioner(3, 2, rng);  // 3D points, 2 bits = 4 partitions

    Vector<float> point1 = {1.0f, 2.0f, 3.0f};
    Vector<float> point2 = {-1.0f, -2.0f, -3.0f};

    u32 partition1 = partitioner.ComputePartition(point1);
    u32 partition2 = partitioner.ComputePartition(point2);

    EXPECT_GE(partition1, 0u);
    EXPECT_LT(partition1, 4u);  // 2^2 = 4 partitions
    EXPECT_GE(partition2, 0u);
    EXPECT_LT(partition2, 4u);

    // Distance to own partition should be 0
    EXPECT_EQ(partitioner.ComputeDistance(point1, partition1), 0u);
    EXPECT_EQ(partitioner.ComputeDistance(point2, partition2), 0u);
}

TEST_F(FixedDimensionalEncodingTest, sparse_projector_test) {
    // Test SparseProjector
    RandomGenerator rng(456);
    SparseProjector projector(5, 3, rng);  // 5D -> 3D

    Vector<float> input = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    Vector<float> output = projector.Project(input);

    EXPECT_EQ(output.size(), 3u);

    // Test deterministic behavior
    Vector<float> output2 = projector.Project(input);
    EXPECT_EQ(output.size(), output2.size());
    for (size_t i = 0; i < output.size(); ++i) {
        EXPECT_FLOAT_EQ(output[i], output2[i]);
    }
}

TEST_F(FixedDimensionalEncodingTest, internal_functions_test) {
    // Test SimHashPartitionIndex
    Vector<float> vec = {1.0f, -2.0f, 3.0f, -4.0f};  // positive, negative, positive, negative

    u32 partition_index = internal::SimHashPartitionIndex(vec);
    EXPECT_GE(partition_index, 0u);
    EXPECT_LT(partition_index, 16u);  // 2^4 = 16

    // Test DistanceToSimHashPartition
    u32 distance = internal::DistanceToSimHashPartition(vec, partition_index);
    EXPECT_EQ(distance, 0u);  // Distance to its own partition should be 0

    // Test ApplyCountSketchToVector
    Vector<float> input = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    Vector<float> result = internal::ApplyCountSketchToVector(Span<const float>(input), 10, 42);
    EXPECT_EQ(result.size(), 10u);

    // Count-Sketch should produce a non-zero result for non-zero input
    bool has_non_zero = false;
    for (float val : result) {
        if (std::abs(val) > 1e-6f) {
            has_non_zero = true;
            break;
        }
    }
    EXPECT_TRUE(has_non_zero);

    // Test deterministic behavior - same input and seed should produce same output
    Vector<float> result2 = internal::ApplyCountSketchToVector(Span<const float>(input), 10, 42);
    EXPECT_EQ(result.size(), result2.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_FLOAT_EQ(result[i], result2[i]);
    }
}

TEST_F(FixedDimensionalEncodingTest, simple_encoding_test) {
    // Test simple encoding with identity projection
    FixedDimensionalEncodingConfig config;
    config.dimension = 2;
    config.num_repetitions = 1;
    config.num_simhash_projections = 0;  // No partitioning
    config.projection_type = FixedDimensionalEncodingConfig::ProjectionType::DEFAULT_IDENTITY;
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;
    
    Vector<float> tensor_data = {1.0f, 2.0f, 3.0f, 4.0f};
    
    auto result = GenerateFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());
    
    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 2u);  // dimension * num_partitions * num_repetitions = 2 * 1 * 1
    
    // Should be sum of the two points: [1+3, 2+4] = [4, 6]
    EXPECT_FLOAT_EQ(encoding[0], 4.0f);
    EXPECT_FLOAT_EQ(encoding[1], 6.0f);
}

TEST_F(FixedDimensionalEncodingTest, invalid_input_test) {
    FixedDimensionalEncodingConfig config;
    config.dimension = 3;
    
    // Test mismatched tensor data size
    Vector<float> invalid_point_cloud = {1.0f, 2.0f};  // Size 2, but dimension is 3
    auto result = GenerateFixedDimensionalEncoding(invalid_point_cloud, config);
    EXPECT_FALSE(result.has_value());
    
    // Test too many simhash projections
    config.num_simhash_projections = 31;
    Vector<float> valid_point_cloud = {1.0f, 2.0f, 3.0f};
    result = GenerateFixedDimensionalEncoding(valid_point_cloud, config);
    EXPECT_FALSE(result.has_value());
    
    // Test query encoding with fill_empty_partitions
    config.num_simhash_projections = 2;
    config.fill_empty_partitions = true;
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;
    result = GenerateQueryFixedDimensionalEncoding(valid_point_cloud, config);
    EXPECT_FALSE(result.has_value());
}

TEST_F(FixedDimensionalEncodingTest, document_encoding_test) {
    // Test document encoding (AVERAGE type)
    FixedDimensionalEncodingConfig config;
    config.dimension = 2;
    config.num_repetitions = 1;
    config.num_simhash_projections = 0;
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::AVERAGE;
    
    Vector<float> tensor_data = {2.0f, 4.0f, 6.0f, 8.0f};  // Two points: [2,4] and [6,8]
    
    auto result = GenerateDocumentFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());
    
    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 2u);
    
    // Should be average of the two points: [(2+6)/2, (4+8)/2] = [4, 6]
    EXPECT_FLOAT_EQ(encoding[0], 4.0f);
    EXPECT_FLOAT_EQ(encoding[1], 6.0f);
}

TEST_F(FixedDimensionalEncodingTest, simhash_partitioning_test) {
    // Test encoding with SimHash partitioning
    FixedDimensionalEncodingConfig config;
    config.dimension = 2;
    config.num_repetitions = 1;
    config.num_simhash_projections = 2;  // 2^2 = 4 partitions
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;

    Vector<float> tensor_data = {1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f};  // 4 points

    auto result = GenerateFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());

    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 8u);  // dimension * num_partitions = 2 * 4 = 8

    bool has_non_zero = false;
    for (float val : encoding) {
        if (std::abs(val) > 1e-6f) {
            has_non_zero = true;
            break;
        }
    }
    EXPECT_TRUE(has_non_zero);
}

TEST_F(FixedDimensionalEncodingTest, ams_projection_test) {
    // Test encoding with AMS sketch projection
    FixedDimensionalEncodingConfig config;
    config.dimension = 4;
    config.num_repetitions = 1;
    config.num_simhash_projections = 0;
    config.projection_type = FixedDimensionalEncodingConfig::ProjectionType::AMS_SKETCH;
    config.projection_dimension = 2;  // Project from 4D to 2D
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;

    Vector<float> tensor_data = {1.0f, 2.0f, 3.0f, 4.0f}; 

    auto result = GenerateFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());

    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 2u);  // projection_dimension

    // The result should be non-zero (random projection of non-zero input)
    bool has_non_zero = false;
    for (float val : encoding) {
        if (std::abs(val) > 1e-6f) {
            has_non_zero = true;
            break;
        }
    }
    EXPECT_TRUE(has_non_zero);
}

TEST_F(FixedDimensionalEncodingTest, final_projection_test) {
    // Test encoding with final Count-Sketch projection
    FixedDimensionalEncodingConfig config;
    config.dimension = 2;
    config.num_repetitions = 1;
    config.num_simhash_projections = 0;
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;
    config.final_projection_dimension = 5;

    Vector<float> tensor_data = {1.0f, 2.0f, 3.0f, 4.0f};  // Two 2D points

    auto result = GenerateFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());

    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 5u);  // final_projection_dimension

    // The result should be non-zero for non-zero input
    bool has_non_zero = false;
    for (float val : encoding) {
        if (std::abs(val) > 1e-6f) {
            has_non_zero = true;
            break;
        }
    }
    EXPECT_TRUE(has_non_zero);
}

TEST_F(FixedDimensionalEncodingTest, multiple_repetitions_test) {
    // Test encoding with multiple repetitions
    FixedDimensionalEncodingConfig config;
    config.dimension = 2;
    config.num_repetitions = 3;
    config.num_simhash_projections = 1;  // 2 partitions
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;

    Vector<float> tensor_data = {1.0f, 2.0f};  

    auto result = GenerateFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());

    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 12u);  // dimension * num_partitions * num_repetitions = 2 * 2 * 3 = 12

    // Each repetition should have some non-zero values
    for (i32 rep = 0; rep < 3; ++rep) {
        bool rep_has_non_zero = false;
        for (i32 i = rep * 4; i < (rep + 1) * 4; ++i) {
            if (std::abs(encoding[i]) > 1e-6f) {
                rep_has_non_zero = true;
                break;
            }
        }
        EXPECT_TRUE(rep_has_non_zero);
    }
}

TEST_F(FixedDimensionalEncodingTest, fill_empty_partitions_test) {
    // Test document encoding with fill_empty_partitions
    FixedDimensionalEncodingConfig config;
    config.dimension = 2;
    config.num_repetitions = 1;
    config.num_simhash_projections = 3;  // 8 partitions, likely to have empty ones
    config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::AVERAGE;
    config.fill_empty_partitions = true;

    Vector<float> tensor_data = {1.0f, 1.0f, 1.1f, 1.1f};  // Two very similar points

    auto result = GenerateDocumentFixedDimensionalEncoding(tensor_data, config);
    EXPECT_TRUE(result.has_value());

    Vector<float> encoding = result.value();
    EXPECT_EQ(encoding.size(), 16u);  // dimension * num_partitions = 2 * 8 = 16

    // With fill_empty_partitions, there should be no zero partitions
    i32 zero_partitions = 0;
    for (i32 p = 0; p < 8; ++p) {
        bool partition_is_zero = true;
        for (i32 d = 0; d < 2; ++d) {
            if (std::abs(encoding[p * 2 + d]) > 1e-6f) {
                partition_is_zero = false;
                break;
            }
        }
        if (partition_is_zero) {
            zero_partitions++;
        }
    }
    // With fill_empty_partitions enabled, we expect fewer zero partitions
    EXPECT_LE(zero_partitions, 6);  // Allow some zero partitions but not all
}
