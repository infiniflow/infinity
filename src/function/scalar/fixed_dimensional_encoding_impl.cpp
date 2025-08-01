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

#include <cmath>
#include <limits>
#include <random>

module infinity_core:fixed_dimensional_encoding.impl;

import :fixed_dimensional_encoding;

import :stl;
import :new_catalog;
import logical_type;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import internal_types;
import data_type;
import :column_vector;
import :data_block;
import :value;
import array_info;
import :third_party;
import embedding_info;
import :function_set;
import :base_expression;
import :function;

namespace infinity {

// RandomGenerator implementation
float RandomGenerator::GenerateGaussian() {
    return gaussian_dist_(engine_);
}

bool RandomGenerator::GenerateBernoulli() {
    return bernoulli_dist_(engine_);
}

u32 RandomGenerator::GenerateUniform(u32 max_val) {
    std::uniform_int_distribution<u32> dist(0, max_val - 1);
    return dist(engine_);
}

// HashPartitioner implementation
HashPartitioner::HashPartitioner(i32 dimension, i32 num_bits, RandomGenerator& rng)
    : num_bits_(num_bits) {
    projection_vectors_.resize(num_bits);
    for (i32 i = 0; i < num_bits; ++i) {
        projection_vectors_[i].resize(dimension);
        for (i32 j = 0; j < dimension; ++j) {
            projection_vectors_[i][j] = rng.GenerateGaussian();
        }
    }
}

u32 HashPartitioner::ComputePartition(const Vector<float>& point) const {
    u32 hash_code = 0;
    for (i32 bit = 0; bit < num_bits_; ++bit) {
        float dot_product = 0.0f;
        for (i32 dim = 0; dim < static_cast<i32>(point.size()); ++dim) {
            dot_product += point[dim] * projection_vectors_[bit][dim];
        }
        if (dot_product > 0.0f) {
            hash_code = (hash_code << 1) | 1;
        } else {
            hash_code = hash_code << 1;
        }
        // Apply Gray code transformation
        if (bit > 0) {
            hash_code ^= (hash_code >> 1);
        }
    }
    return hash_code;
}

u32 HashPartitioner::ComputeDistance(const Vector<float>& point, u32 target_partition) const {
    u32 actual_partition = ComputePartition(point);
    return __builtin_popcount(actual_partition ^ target_partition);
}



// SparseProjector implementation
SparseProjector::SparseProjector(i32 input_dim, i32 output_dim, RandomGenerator& rng)
    : input_dim_(input_dim), output_dim_(output_dim) {
    for (i32 i = 0; i < input_dim; ++i) {
        u32 target_idx = rng.GenerateUniform(output_dim);
        float sign = rng.GenerateBernoulli() ? 1.0f : -1.0f;
        projection_map_.emplace_back(target_idx, sign);
    }
}

Vector<float> SparseProjector::Project(const Vector<float>& input) const {
    Vector<float> output(output_dim_, 0.0f);
    for (i32 i = 0; i < static_cast<i32>(input.size()); ++i) {
        const auto& [target_idx, sign] = projection_map_[i];
        output[target_idx] += input[i] * sign;
    }
    return output;
}

// EncodingAccumulator implementation
EncodingAccumulator::EncodingAccumulator(i32 size, bool use_averaging)
    : accumulator_(size, 0.0f), counts_(size, 0), use_averaging_(use_averaging) {}

void EncodingAccumulator::AddToPartition(i32 partition_start, const Vector<float>& values) {
    for (i32 i = 0; i < static_cast<i32>(values.size()); ++i) {
        accumulator_[partition_start + i] += values[i];
        counts_[partition_start + i]++;
    }
}

void EncodingAccumulator::FillEmptyPartition(i32 partition_start, const Vector<float>& values) {
    for (i32 i = 0; i < static_cast<i32>(values.size()); ++i) {
        if (counts_[partition_start + i] == 0) {
            accumulator_[partition_start + i] = values[i];
            counts_[partition_start + i] = 1;
        }
    }
}

Vector<float> EncodingAccumulator::Finalize() const {
    Vector<float> result = accumulator_;
    if (use_averaging_) {
        for (i32 i = 0; i < static_cast<i32>(result.size()); ++i) {
            if (counts_[i] > 0) {
                result[i] /= static_cast<float>(counts_[i]);
            }
        }
    }
    return result;
}

// EncodingPipeline implementation
bool EncodingPipeline::ValidateInput(const Tensor& tensor) const {
    if (tensor.GetTotalSize() % config_.dimension != 0) return false;
    if (config_.num_simhash_projections >= 31 || config_.num_simhash_projections < 0) return false;
    return true;
}

Vector<float> EncodingPipeline::ProcessSingleRepetition(const Tensor& tensor, i32 rep_index) const {
    RandomGenerator rng(config_.seed + rep_index);

    // Setup partitioning
    HashPartitioner partitioner(config_.dimension, config_.num_simhash_projections, rng);
    i32 num_partitions = 1 << config_.num_simhash_projections;

    // Setup projection
    i32 proj_dim = (config_.projection_type == FixedDimensionalEncodingConfig::ProjectionType::DEFAULT_IDENTITY)
                   ? config_.dimension : config_.projection_dimension;

    Optional<SparseProjector> projector = None;
    if (config_.projection_type == FixedDimensionalEncodingConfig::ProjectionType::AMS_SKETCH) {
        if (config_.projection_dimension <= 0) return {};
        projector = SparseProjector(config_.dimension, proj_dim, rng);
    }

    // Setup accumulator
    bool use_averaging = (config_.encoding_type == FixedDimensionalEncodingConfig::EncodingType::AVERAGE);
    EncodingAccumulator accumulator(num_partitions * proj_dim, use_averaging);

    // Process each point
    for (i32 point_idx = 0; point_idx < tensor.GetPointCount(); ++point_idx) {
        Vector<float> point = tensor.GetPoint(point_idx);

        // Apply projection if needed
        Vector<float> projected_point = projector.has_value() ? projector->Project(point) : point;

        // Determine partition
        u32 partition_idx = (config_.num_simhash_projections > 0) ? partitioner.ComputePartition(point) : 0;
        i32 partition_start = partition_idx * proj_dim;

        // Add to accumulator
        accumulator.AddToPartition(partition_start, projected_point);
    }

    // Handle empty partitions for document encoding
    if (use_averaging && config_.fill_empty_partitions && config_.num_simhash_projections > 0) {
        for (u32 partition_idx = 0; partition_idx < static_cast<u32>(num_partitions); ++partition_idx) {
            i32 partition_start = partition_idx * proj_dim;

            // Find nearest point for empty partitions
            i32 best_point_idx = -1;
            u32 min_distance = std::numeric_limits<u32>::max();

            for (i32 point_idx = 0; point_idx < tensor.GetPointCount(); ++point_idx) {
                Vector<float> point = tensor.GetPoint(point_idx);
                u32 distance = partitioner.ComputeDistance(point, partition_idx);
                if (distance < min_distance) {
                    min_distance = distance;
                    best_point_idx = point_idx;
                }
            }

            if (best_point_idx >= 0) {
                Vector<float> best_point = tensor.GetPoint(best_point_idx);
                Vector<float> projected_best = projector.has_value() ? projector->Project(best_point) : best_point;
                accumulator.FillEmptyPartition(partition_start, projected_best);
            }
        }
    }

    return accumulator.Finalize();
}

Optional<Vector<float>> EncodingPipeline::Execute(const Tensor& tensor) const {
    if (!ValidateInput(tensor)) return None;

    // Special validation for query encoding
    if (config_.encoding_type == FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM &&
        config_.fill_empty_partitions) {
        return None;
    }

    Vector<Vector<float>> repetition_results;
    repetition_results.reserve(config_.num_repetitions);

    // Process each repetition
    for (i32 rep = 0; rep < config_.num_repetitions; ++rep) {
        Vector<float> rep_result = ProcessSingleRepetition(tensor, rep);
        if (rep_result.empty()) return None;
        repetition_results.push_back(std::move(rep_result));
    }

    // Concatenate all repetitions
    Vector<float> final_result;
    for (const auto& rep_result : repetition_results) {
        final_result.insert(final_result.end(), rep_result.begin(), rep_result.end());
    }

    // Apply final projection if specified
    if (config_.has_final_projection_dimension()) {
        RandomGenerator final_rng(config_.seed);
        SparseProjector final_projector(final_result.size(), config_.final_projection_dimension.value(), final_rng);
        final_result = final_projector.Project(final_result);
    }

    return final_result;
}

// Public API functions
Optional<Vector<float>> GenerateFixedDimensionalEncoding(
    const Vector<float>& tensor_data, const FixedDimensionalEncodingConfig& config) {
    Tensor tensor(tensor_data, config.dimension);
    EncodingPipeline pipeline(config);
    return pipeline.Execute(tensor);
}

Optional<Vector<float>> GenerateQueryFixedDimensionalEncoding(
    const Vector<float>& tensor_data, const FixedDimensionalEncodingConfig& config) {
    auto modified_config = config;
    modified_config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;
    return GenerateFixedDimensionalEncoding(tensor_data, modified_config);
}

Optional<Vector<float>> GenerateDocumentFixedDimensionalEncoding(
    const Vector<float>& tensor_data, const FixedDimensionalEncodingConfig& config) {
    auto modified_config = config;
    modified_config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::AVERAGE;
    return GenerateFixedDimensionalEncoding(tensor_data, modified_config);
}

// Legacy compatibility functions for testing
namespace internal {
    u32 SimHashPartitionIndex(const Vector<float>& input_vector) {
        RandomGenerator rng(1);  // Use default seed
        HashPartitioner partitioner(input_vector.size(), std::min(32, static_cast<i32>(input_vector.size())), rng);
        return partitioner.ComputePartition(input_vector);
    }

    u32 DistanceToSimHashPartition(const Vector<float>& input_vector, u32 index) {
        RandomGenerator rng(1);
        HashPartitioner partitioner(input_vector.size(), std::min(32, static_cast<i32>(input_vector.size())), rng);
        return partitioner.ComputeDistance(input_vector, index);
    }

    Vector<float> ApplyCountSketchToVector(Span<const float> input_vector, u32 final_dimension, u32 seed) {
        RandomGenerator rng(seed);
        Vector<float> input_vec(input_vector.begin(), input_vector.end());
        SparseProjector projector(input_vec.size(), final_dimension, rng);
        return projector.Project(input_vec);
    }
}

// FDE Scalar Function Implementation
void FDEFunction(const DataBlock &input, SharedPtr<ColumnVector> &output) {
    if (input.column_count() != 2) {
        String error_message = fmt::format("FDE function: input column count is {}, expected 2.", input.column_count());
        UnrecoverableError(error_message);
    }

    SizeT row_count = input.row_count();

    for (SizeT row_idx = 0; row_idx < row_count; ++row_idx) {
        // Get the dimension parameter
        Value dimension_value = input.GetValue(1, row_idx);
        BigIntT target_dimension = dimension_value.GetValue<BigIntT>();

        // Get the tensor data
        Value tensor_value = input.GetValue(0, row_idx);

        // Extract tensor data based on the tensor type
        Vector<float> tensor_data;

        if (tensor_value.type().type() == LogicalType::kTensor || tensor_value.type().type() == LogicalType::kEmbedding) {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(tensor_value.type().type_info().get());

            // Get tensor data from the value - for tensor values, the data is stored as embedding data
            Span<char> raw_data = tensor_value.GetEmbedding();

            // Calculate total elements based on raw data size and element size
            SizeT element_size = 0;
            if (embedding_info->Type() == EmbeddingDataType::kElemFloat) {
                element_size = sizeof(float);
            } else if (embedding_info->Type() == EmbeddingDataType::kElemDouble) {
                element_size = sizeof(double);
            } else {
                String error_message =
                    fmt::format("FDE function: unsupported tensor data type: {}", EmbeddingInfo::EmbeddingDataTypeToString(embedding_info->Type()));
                UnrecoverableError(error_message);
            }

            SizeT total_elements = raw_data.size() / element_size;
            tensor_data.reserve(total_elements);

            // Convert raw data to float vector
            if (embedding_info->Type() == EmbeddingDataType::kElemFloat) {
                const float *float_ptr = reinterpret_cast<const float *>(raw_data.data());
                for (SizeT i = 0; i < total_elements; ++i) {
                    tensor_data.push_back(float_ptr[i]);
                }
            } else if (embedding_info->Type() == EmbeddingDataType::kElemDouble) {
                const double *double_ptr = reinterpret_cast<const double *>(raw_data.data());
                for (SizeT i = 0; i < total_elements; ++i) {
                    tensor_data.push_back(static_cast<float>(double_ptr[i]));
                }
            }
        } else if (tensor_value.type().type() == LogicalType::kArray) {
            // Handle Array input (including SubArrayArray from [[0.0, -10.0], [9.2, 45.6]] syntax)
            const Vector<Value> &array_elements = tensor_value.GetArray();

            // Check if this is a nested array (SubArrayArray)
            if (!array_elements.empty() && array_elements[0].type().type() == LogicalType::kArray) {
                // Nested array case: [[1.0, 2.0], [3.0, 4.0]]
                const Vector<Value> &first_sub_array = array_elements[0].GetArray();
                SizeT sub_array_size = first_sub_array.size();

                // Validate all sub-arrays have the same size
                for (SizeT i = 1; i < array_elements.size(); ++i) {
                    if (array_elements[i].type().type() != LogicalType::kArray) {
                        String error_message = "FDE function: mixed array types not supported";
                        UnrecoverableError(error_message);
                    }
                    const Vector<Value> &sub_array = array_elements[i].GetArray();
                    if (sub_array.size() != sub_array_size) {
                        String error_message = "FDE function: all sub-arrays must have the same dimension";
                        UnrecoverableError(error_message);
                    }
                }

                // Flatten nested arrays
                tensor_data.reserve(array_elements.size() * sub_array_size); // Pre-allocate memory
                for (SizeT i = 0; i < array_elements.size(); ++i) {
                    const Vector<Value> &sub_array = array_elements[i].GetArray();
                    if (sub_array.size() != sub_array_size) {
                        String error_message =
                            fmt::format("FDE function: sub-array {} has size {}, expected {}", i, sub_array.size(), sub_array_size);
                        UnrecoverableError(error_message);
                    }
                    for (SizeT j = 0; j < sub_array.size(); ++j) {
                        const Value &element_value = sub_array[j];
                        if (element_value.type().type() == LogicalType::kFloat) {
                            tensor_data.push_back(element_value.GetValue<FloatT>());
                        } else if (element_value.type().type() == LogicalType::kDouble) {
                            tensor_data.push_back(static_cast<float>(element_value.GetValue<DoubleT>()));
                        } else if (element_value.type().type() == LogicalType::kBigInt) {
                            tensor_data.push_back(static_cast<float>(element_value.GetValue<BigIntT>()));
                        } else if (element_value.type().type() == LogicalType::kInteger) {
                            tensor_data.push_back(static_cast<float>(element_value.GetValue<IntegerT>()));
                        } else {
                            String error_message = fmt::format("FDE function: unsupported array element type: {}", element_value.type().ToString());
                            UnrecoverableError(error_message);
                        }
                    }
                }
            } else {
                // Single-level array case: [1.0, 2.0, 3.0, 4.0]
                for (SizeT i = 0; i < array_elements.size(); ++i) {
                    const Value &element_value = array_elements[i];
                    if (element_value.type().type() == LogicalType::kFloat) {
                        tensor_data.push_back(element_value.GetValue<FloatT>());
                    } else if (element_value.type().type() == LogicalType::kDouble) {
                        tensor_data.push_back(static_cast<float>(element_value.GetValue<DoubleT>()));
                    } else if (element_value.type().type() == LogicalType::kBigInt) {
                        tensor_data.push_back(static_cast<float>(element_value.GetValue<BigIntT>()));
                    } else if (element_value.type().type() == LogicalType::kInteger) {
                        tensor_data.push_back(static_cast<float>(element_value.GetValue<IntegerT>()));
                    } else {
                        String error_message = fmt::format("FDE function: unsupported array element type: {}", element_value.type().ToString());
                        UnrecoverableError(error_message);
                    }
                }
            }

            if (tensor_data.empty()) {
                String error_message = "FDE function: empty array input";
                UnrecoverableError(error_message);
            }
        } else {
            String error_message = fmt::format("FDE function: expected tensor, embedding, or array input, got: {}", tensor_value.type().ToString());
            UnrecoverableError(error_message);
        }

        // Validate input dimensions
        if (tensor_data.empty()) {
            String error_message = "FDE function: empty tensor data";
            UnrecoverableError(error_message);
        }

        if (tensor_data.size() > 100000) {
            String error_message = fmt::format("FDE function: input dimension {} is too large (max 100000)", tensor_data.size());
            UnrecoverableError(error_message);
        }

        if (target_dimension <= 0 || target_dimension > 65536) {
            String error_message = fmt::format("FDE function: target dimension {} is invalid (must be 1-10000)", target_dimension);
            UnrecoverableError(error_message);
        }

        // Create FDE configuration for simple dimensionality reduction
        FixedDimensionalEncodingConfig config;
        config.dimension = static_cast<i32>(tensor_data.size()); // Use actual data size
        config.seed = 42; // Default seed
        config.num_repetitions = 1;
        config.num_simhash_projections = 0; // No partitioning - direct projection
        config.projection_type = FixedDimensionalEncodingConfig::ProjectionType::AMS_SKETCH;
        config.projection_dimension = target_dimension;
        config.encoding_type = FixedDimensionalEncodingConfig::EncodingType::DEFAULT_SUM;
        config.fill_empty_partitions = false;

        // Apply FDE encoding
        Optional<Vector<float>> result = GenerateFixedDimensionalEncoding(tensor_data, config);

        if (!result.has_value()) {
            String error_message = "FDE function: encoding failed";
            UnrecoverableError(error_message);
        }

        // Extract the result and make an explicit copy to avoid any reference issues
        Vector<float> result_vector;
        result_vector.reserve(result.value().size());
        for (const auto &val : result.value()) {
            result_vector.push_back(val);
        }

        // Create embedding value with the actual target dimension
        auto target_embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, target_dimension);
        Value result_embedding = Value::MakeEmbedding(reinterpret_cast<const char *>(result_vector.data()), target_embedding_info);
        output->AppendValue(result_embedding);
    }
}

void RegisterFDEFunction(NewCatalog *catalog_ptr) {
    String func_name = "FDE";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    // Use a generic output type that will be resolved at runtime
    // We'll register with dimension 1 as a placeholder - the actual dimension will be determined by the target_dimension parameter
    auto result_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 1); // Placeholder dimension
    auto result_type = DataType(LogicalType::kEmbedding, result_info);

    // Register for arrays (multi-dimensional arrays like [[0.0, -10.0], [9.2, 45.6]])
    auto array_type = DataType(LogicalType::kArray);
    ScalarFunction fde_array_function(func_name, {array_type, DataType(LogicalType::kBigInt)}, result_type, &FDEFunction);
    function_set_ptr->AddFunction(fde_array_function);

    // Register representative functions for embedding and tensor types
    Vector<EmbeddingDataType> data_types = {EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemDouble};

    for (auto data_type : data_types) {
        // Register one representative function per data type - the matching logic will handle any dimension
        auto tensor_embedding_info = EmbeddingInfo::Make(data_type, 1); // Use dimension 1 as representative
        auto tensor_type = DataType(LogicalType::kTensor, tensor_embedding_info);
        ScalarFunction fde_tensor_function(func_name, {tensor_type, DataType(LogicalType::kBigInt)}, result_type, &FDEFunction);
        function_set_ptr->AddFunction(fde_tensor_function);

        // Register for embedding types
        auto embedding_type = DataType(LogicalType::kEmbedding, tensor_embedding_info);
        ScalarFunction fde_embedding_function(func_name, {embedding_type, DataType(LogicalType::kBigInt)}, result_type, &FDEFunction);
        function_set_ptr->AddFunction(fde_embedding_function);
    }

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

}  // namespace infinity
