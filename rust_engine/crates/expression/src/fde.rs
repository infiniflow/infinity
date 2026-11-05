//! Fixed Dimensional Encoding (FDE) for tensor similarity.
//!
//! C++: `fixed_dimensional_encoding.cppm` / `_impl.cpp`
//! (`src/function/scalar`). A tensor (a point cloud) is partitioned with
//! SimHash, each point is optionally projected (AMS sketch / Count-Sketch), the
//! points are pooled per partition (sum or average), repetitions are
//! concatenated, and an optional final Count-Sketch projection fixes the
//! output dimension. The dot product of two FDEs approximates the original
//! tensor similarity (arXiv:2405.19504).

/// C++: `FixedDimensionalEncodingConfig`.
#[derive(Debug, Clone, PartialEq)]
pub struct FixedDimensionalEncodingConfig {
    /// Dimension of the input embeddings.
    pub dimension: i32,
    /// Independent repetitions of the encoding.
    pub num_repetitions: i32,
    /// SimHash projections per repetition (`2^n` partitions).
    pub num_simhash_projections: i32,
    /// Seed for the generator.
    pub seed: i32,
    pub encoding_type: EncodingType,
    /// Target dimension of the AMS projection.
    pub projection_dimension: i32,
    pub projection_type: ProjectionType,
    /// Fill empty partitions with the nearest point (document encoding).
    pub fill_empty_partitions: bool,
    /// Optional final Count-Sketch dimension.
    pub final_projection_dimension: Option<i32>,
}

impl Default for FixedDimensionalEncodingConfig {
    fn default() -> Self {
        Self {
            dimension: 0,
            num_repetitions: 1,
            num_simhash_projections: 0,
            seed: 1,
            encoding_type: EncodingType::DefaultSum,
            projection_dimension: 0,
            projection_type: ProjectionType::DefaultIdentity,
            fill_empty_partitions: false,
            final_projection_dimension: None,
        }
    }
}

impl FixedDimensionalEncodingConfig {
    /// C++: `has_final_projection_dimension`.
    pub fn has_final_projection_dimension(&self) -> bool {
        self.final_projection_dimension.is_some()
    }
}

/// C++: `EncodingType`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum EncodingType {
    #[default]
    DefaultSum,
    Average,
}

/// C++: `ProjectionType`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum ProjectionType {
    #[default]
    DefaultIdentity,
    AmsSketch,
}

/// C++: `Tensor`: a flat buffer of `point_count` points of `dimension` floats.
#[derive(Debug, Clone, PartialEq)]
pub struct Tensor {
    buffer: Vec<f32>,
    point_count: i32,
    dimension: i32,
}

impl Tensor {
    pub fn new(data: Vec<f32>, dimension: i32) -> Self {
        let point_count = if dimension > 0 {
            data.len() as i32 / dimension
        } else {
            0
        };
        Self {
            buffer: data,
            point_count,
            dimension,
        }
    }

    pub fn point_count(&self) -> i32 {
        self.point_count
    }
    pub fn dimension(&self) -> i32 {
        self.dimension
    }
    pub fn total_size(&self) -> usize {
        self.buffer.len()
    }
    pub fn raw_data(&self) -> &[f32] {
        &self.buffer
    }

    /// C++: `GetPoint`.
    pub fn point(&self, index: i32) -> Vec<f32> {
        let dim = self.dimension as usize;
        let at = index as usize * dim;
        self.buffer[at..at + dim].to_vec()
    }
}

/// C++: `RandomGenerator` (`std::mt19937` + normal/Bernoulli/uniform).
///
/// The generator is deterministic per seed; the concrete stream differs from
/// `mt19937` (the FDE tests assert determinism and properties, not the exact
/// C++ bit stream).
#[derive(Debug, Clone)]
pub struct RandomGenerator {
    state: u64,
    /// Cached second Box-Muller sample.
    spare: Option<f32>,
}

impl RandomGenerator {
    /// C++: `RandomGenerator(u32 seed)`.
    pub fn new(seed: u32) -> Self {
        Self {
            state: (seed as u64) | 0x9E37_79B9_7F4A_7C15,
            spare: None,
        }
    }

    fn next_u64(&mut self) -> u64 {
        self.state ^= self.state << 13;
        self.state ^= self.state >> 7;
        self.state ^= self.state << 17;
        self.state
    }

    fn unit(&mut self) -> f32 {
        // (0, 1]
        (((self.next_u64() >> 11) as f64 + 1.0) / ((1u64 << 53) as f64)) as f32
    }

    /// C++: `GenerateGaussian` (normal, mean 0, stddev 1).
    pub fn gaussian(&mut self) -> f32 {
        if let Some(sample) = self.spare.take() {
            return sample;
        }
        let u = self.unit();
        let v = self.unit();
        let r = (-2.0 * u.ln()).sqrt();
        let theta = 2.0 * std::f32::consts::PI * v;
        self.spare = Some(r * theta.sin());
        r * theta.cos()
    }

    /// C++: `GenerateBernoulli` (p = 0.5).
    pub fn bernoulli(&mut self) -> bool {
        self.next_u64() & 1 == 1
    }

    /// C++: `GenerateUniform(max_val)` in `[0, max_val)`.
    pub fn uniform(&mut self, max_val: u32) -> u32 {
        if max_val == 0 {
            return 0;
        }
        (self.next_u64() % max_val as u64) as u32
    }
}

/// C++: `HashPartitioner`.
#[derive(Debug, Clone)]
pub struct HashPartitioner {
    projection_vectors: Vec<Vec<f32>>,
    num_bits: i32,
}

impl HashPartitioner {
    pub fn new(dimension: i32, num_bits: i32, rng: &mut RandomGenerator) -> Self {
        let mut projection_vectors = Vec::with_capacity(num_bits.max(0) as usize);
        for _ in 0..num_bits.max(0) {
            let vector = (0..dimension.max(0)).map(|_| rng.gaussian()).collect();
            projection_vectors.push(vector);
        }
        Self {
            projection_vectors,
            num_bits,
        }
    }

    /// C++: `ComputePartition` (Gray-code transformed sign bits).
    pub fn compute_partition(&self, point: &[f32]) -> u32 {
        let mut hash_code = 0u32;
        for bit in 0..self.num_bits {
            let vector = &self.projection_vectors[bit as usize];
            let mut dot = 0.0f32;
            for (dim, value) in point.iter().enumerate() {
                dot += value * vector[dim];
            }
            hash_code = (hash_code << 1) | u32::from(dot > 0.0);
            if bit > 0 {
                hash_code ^= hash_code >> 1;
            }
        }
        hash_code
    }

    /// C++: `ComputeDistance` (Hamming distance between partitions).
    pub fn compute_distance(&self, point: &[f32], target_partition: u32) -> u32 {
        (self.compute_partition(point) ^ target_partition).count_ones()
    }
}

/// C++: `SparseProjector`.
#[derive(Debug, Clone)]
pub struct SparseProjector {
    projection_map: Vec<(u32, f32)>,
    output_dim: i32,
}

impl SparseProjector {
    pub fn new(input_dim: i32, output_dim: i32, rng: &mut RandomGenerator) -> Self {
        let mut projection_map = Vec::with_capacity(input_dim.max(0) as usize);
        for _ in 0..input_dim.max(0) {
            let target = rng.uniform(output_dim.max(0) as u32);
            let sign = if rng.bernoulli() { 1.0 } else { -1.0 };
            projection_map.push((target, sign));
        }
        Self {
            projection_map,
            output_dim,
        }
    }

    /// C++: `Project`.
    pub fn project(&self, input: &[f32]) -> Vec<f32> {
        let mut output = vec![0.0f32; self.output_dim.max(0) as usize];
        for (i, value) in input.iter().enumerate() {
            let (target, sign) = self.projection_map[i];
            output[target as usize] += value * sign;
        }
        output
    }
}

/// C++: `EncodingAccumulator`.
#[derive(Debug, Clone)]
pub struct EncodingAccumulator {
    accumulator: Vec<f32>,
    counts: Vec<i32>,
    use_averaging: bool,
}

impl EncodingAccumulator {
    pub fn new(size: i32, use_averaging: bool) -> Self {
        Self {
            accumulator: vec![0.0; size.max(0) as usize],
            counts: vec![0; size.max(0) as usize],
            use_averaging,
        }
    }

    pub fn add_to_partition(&mut self, partition_start: i32, values: &[f32]) {
        for (i, value) in values.iter().enumerate() {
            self.accumulator[partition_start as usize + i] += value;
            self.counts[partition_start as usize + i] += 1;
        }
    }

    pub fn fill_empty_partition(&mut self, partition_start: i32, values: &[f32]) {
        for (i, value) in values.iter().enumerate() {
            if self.counts[partition_start as usize + i] == 0 {
                self.accumulator[partition_start as usize + i] = *value;
                self.counts[partition_start as usize + i] = 1;
            }
        }
    }

    pub fn finalize(&self) -> Vec<f32> {
        let mut result = self.accumulator.clone();
        if self.use_averaging {
            for (i, value) in result.iter_mut().enumerate() {
                if self.counts[i] > 0 {
                    *value /= self.counts[i] as f32;
                }
            }
        }
        result
    }
}

/// C++: `EncodingPipeline`.
#[derive(Debug, Clone)]
pub struct EncodingPipeline<'a> {
    config: &'a FixedDimensionalEncodingConfig,
}

impl<'a> EncodingPipeline<'a> {
    pub fn new(config: &'a FixedDimensionalEncodingConfig) -> Self {
        Self { config }
    }

    /// C++: `ValidateInput`.
    fn validate_input(&self, tensor: &Tensor) -> bool {
        if self.config.dimension <= 0 || (tensor.total_size() as i32) % self.config.dimension != 0 {
            return false;
        }
        (0..31).contains(&self.config.num_simhash_projections)
    }

    /// C++: `ProcessSingleRepetition`.
    fn process_single_repetition(&self, tensor: &Tensor, rep_index: i32) -> Vec<f32> {
        let mut rng = RandomGenerator::new((self.config.seed + rep_index) as u32);
        let partitioner = HashPartitioner::new(
            self.config.dimension,
            self.config.num_simhash_projections,
            &mut rng,
        );
        let num_partitions = 1i32 << self.config.num_simhash_projections;
        let proj_dim = if self.config.projection_type == ProjectionType::DefaultIdentity {
            self.config.dimension
        } else {
            self.config.projection_dimension
        };
        let projector = if self.config.projection_type == ProjectionType::AmsSketch {
            if self.config.projection_dimension <= 0 {
                return Vec::new();
            }
            Some(SparseProjector::new(
                self.config.dimension,
                proj_dim,
                &mut rng,
            ))
        } else {
            None
        };
        let use_averaging = self.config.encoding_type == EncodingType::Average;
        let mut accumulator = EncodingAccumulator::new(num_partitions * proj_dim, use_averaging);

        for point_idx in 0..tensor.point_count() {
            let point = tensor.point(point_idx);
            let projected = match &projector {
                Some(projector) => projector.project(&point),
                None => point.clone(),
            };
            let partition_idx = if self.config.num_simhash_projections > 0 {
                partitioner.compute_partition(&point)
            } else {
                0
            };
            accumulator.add_to_partition((partition_idx as i32) * proj_dim, &projected);
        }

        if use_averaging
            && self.config.fill_empty_partitions
            && self.config.num_simhash_projections > 0
        {
            for partition_idx in 0..num_partitions as u32 {
                let mut best_point_idx = -1i32;
                let mut min_distance = u32::MAX;
                for point_idx in 0..tensor.point_count() {
                    let point = tensor.point(point_idx);
                    let distance = partitioner.compute_distance(&point, partition_idx);
                    if distance < min_distance {
                        min_distance = distance;
                        best_point_idx = point_idx;
                    }
                }
                if best_point_idx >= 0 {
                    let best_point = tensor.point(best_point_idx);
                    let projected = match &projector {
                        Some(projector) => projector.project(&best_point),
                        None => best_point,
                    };
                    accumulator.fill_empty_partition((partition_idx as i32) * proj_dim, &projected);
                }
            }
        }
        accumulator.finalize()
    }

    /// C++: `Execute`.
    pub fn execute(&self, tensor: &Tensor) -> Option<Vec<f32>> {
        if !self.validate_input(tensor) {
            return None;
        }
        if self.config.encoding_type == EncodingType::DefaultSum
            && self.config.fill_empty_partitions
        {
            return None;
        }
        let mut result = Vec::new();
        for rep in 0..self.config.num_repetitions.max(0) {
            let part = self.process_single_repetition(tensor, rep);
            if part.is_empty() {
                return None;
            }
            result.extend_from_slice(&part);
        }
        if let Some(final_dim) = self.config.final_projection_dimension {
            let mut rng = RandomGenerator::new(self.config.seed as u32);
            let projector = SparseProjector::new(result.len() as i32, final_dim, &mut rng);
            result = projector.project(&result);
        }
        Some(result)
    }
}

/// C++: `GenerateFixedDimensionalEncoding`.
pub fn generate_fixed_dimensional_encoding(
    tensor_data: &[f32],
    config: &FixedDimensionalEncodingConfig,
) -> Option<Vec<f32>> {
    let tensor = Tensor::new(tensor_data.to_vec(), config.dimension);
    EncodingPipeline::new(config).execute(&tensor)
}

/// C++: `GenerateQueryFixedDimensionalEncoding`.
pub fn generate_query_fixed_dimensional_encoding(
    tensor_data: &[f32],
    config: &FixedDimensionalEncodingConfig,
) -> Option<Vec<f32>> {
    let mut modified = config.clone();
    modified.encoding_type = EncodingType::DefaultSum;
    generate_fixed_dimensional_encoding(tensor_data, &modified)
}

/// C++: `GenerateDocumentFixedDimensionalEncoding`.
pub fn generate_document_fixed_dimensional_encoding(
    tensor_data: &[f32],
    config: &FixedDimensionalEncodingConfig,
) -> Option<Vec<f32>> {
    let mut modified = config.clone();
    modified.encoding_type = EncodingType::Average;
    generate_fixed_dimensional_encoding(tensor_data, &modified)
}

/// C++: `internal::*` (legacy compatibility helpers).
pub mod internal {
    use super::*;

    /// C++: `SimHashPartitionIndex`.
    pub fn sim_hash_partition_index(input_vector: &[f32]) -> u32 {
        let mut rng = RandomGenerator::new(1);
        let bits = input_vector.len().min(32) as i32;
        HashPartitioner::new(input_vector.len() as i32, bits, &mut rng)
            .compute_partition(input_vector)
    }

    /// C++: `DistanceToSimHashPartition`.
    pub fn distance_to_sim_hash_partition(input_vector: &[f32], index: u32) -> u32 {
        let mut rng = RandomGenerator::new(1);
        let bits = input_vector.len().min(32) as i32;
        HashPartitioner::new(input_vector.len() as i32, bits, &mut rng)
            .compute_distance(input_vector, index)
    }

    /// C++: `ApplyCountSketchToVector`.
    pub fn apply_count_sketch_to_vector(
        input_vector: &[f32],
        final_dimension: u32,
        seed: u32,
    ) -> Vec<f32> {
        let mut rng = RandomGenerator::new(seed);
        let projector =
            SparseProjector::new(input_vector.len() as i32, final_dimension as i32, &mut rng);
        projector.project(input_vector)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// C++: `config_test`.
    #[test]
    fn config_test() {
        let mut config = FixedDimensionalEncodingConfig::default();
        assert_eq!(config.dimension, 0);
        assert_eq!(config.num_repetitions, 1);
        assert_eq!(config.num_simhash_projections, 0);
        assert_eq!(config.seed, 1);
        assert_eq!(config.encoding_type, EncodingType::DefaultSum);
        assert_eq!(config.projection_dimension, 0);
        assert_eq!(config.projection_type, ProjectionType::DefaultIdentity);
        assert!(!config.fill_empty_partitions);
        assert!(!config.has_final_projection_dimension());
        config.final_projection_dimension = Some(128);
        assert!(config.has_final_projection_dimension());
        assert_eq!(config.final_projection_dimension, Some(128));
    }

    /// C++: `tensor_test`.
    #[test]
    fn tensor_test() {
        let tensor = Tensor::new(vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0], 3);
        assert_eq!(tensor.point_count(), 2);
        assert_eq!(tensor.dimension(), 3);
        assert_eq!(tensor.total_size(), 6);
        assert_eq!(tensor.point(0), vec![1.0, 2.0, 3.0]);
        assert_eq!(tensor.point(1), vec![4.0, 5.0, 6.0]);
    }

    /// C++: `random_generator_test`.
    #[test]
    fn random_generator_test() {
        let mut a = RandomGenerator::new(42);
        let mut b = RandomGenerator::new(42);
        for _ in 0..10 {
            assert_eq!(a.gaussian(), b.gaussian());
            assert_eq!(a.bernoulli(), b.bernoulli());
            assert_eq!(a.uniform(100), b.uniform(100));
        }
    }

    /// C++: `hash_partitioner_test`.
    #[test]
    fn hash_partitioner_test() {
        let mut rng = RandomGenerator::new(123);
        let partitioner = HashPartitioner::new(3, 2, &mut rng);
        let point1 = vec![1.0, 2.0, 3.0];
        let point2 = vec![-1.0, -2.0, -3.0];
        let partition1 = partitioner.compute_partition(&point1);
        let partition2 = partitioner.compute_partition(&point2);
        assert!(partition1 < 4);
        assert!(partition2 < 4);
        assert_eq!(partitioner.compute_distance(&point1, partition1), 0);
        assert_eq!(partitioner.compute_distance(&point2, partition2), 0);
    }

    /// C++: `sparse_projector_test`.
    #[test]
    fn sparse_projector_test() {
        let mut rng = RandomGenerator::new(456);
        let projector = SparseProjector::new(5, 3, &mut rng);
        let input = vec![1.0, 2.0, 3.0, 4.0, 5.0];
        let output = projector.project(&input);
        assert_eq!(output.len(), 3);
        assert_eq!(output, projector.project(&input));
    }

    /// C++: `internal_functions_test`.
    #[test]
    fn internal_functions_test() {
        let vec = vec![1.0, -2.0, 3.0, -4.0];
        let partition = internal::sim_hash_partition_index(&vec);
        assert!(partition < 16);
        assert_eq!(internal::distance_to_sim_hash_partition(&vec, partition), 0);

        let input = vec![1.0, 2.0, 3.0, 4.0, 5.0];
        let result = internal::apply_count_sketch_to_vector(&input, 10, 42);
        assert_eq!(result.len(), 10);
        assert!(result.iter().any(|v| v.abs() > 1e-6));
        assert_eq!(
            result,
            internal::apply_count_sketch_to_vector(&input, 10, 42)
        );
    }

    /// C++: `simple_encoding_test`.
    #[test]
    fn simple_encoding_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 2,
            num_repetitions: 1,
            num_simhash_projections: 0,
            projection_type: ProjectionType::DefaultIdentity,
            encoding_type: EncodingType::DefaultSum,
            ..Default::default()
        };
        let encoding = generate_fixed_dimensional_encoding(&[1.0, 2.0, 3.0, 4.0], &config).unwrap();
        assert_eq!(encoding, vec![4.0, 6.0]);
    }

    /// C++: `invalid_input_test`.
    #[test]
    fn invalid_input_test() {
        let mut config = FixedDimensionalEncodingConfig {
            dimension: 3,
            ..Default::default()
        };
        assert!(generate_fixed_dimensional_encoding(&[1.0, 2.0], &config).is_none());

        config.num_simhash_projections = 31;
        assert!(generate_fixed_dimensional_encoding(&[1.0, 2.0, 3.0], &config).is_none());

        config.num_simhash_projections = 2;
        config.fill_empty_partitions = true;
        config.encoding_type = EncodingType::DefaultSum;
        assert!(generate_query_fixed_dimensional_encoding(&[1.0, 2.0, 3.0], &config).is_none());
    }

    /// C++: `document_encoding_test`.
    #[test]
    fn document_encoding_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 2,
            num_repetitions: 1,
            num_simhash_projections: 0,
            encoding_type: EncodingType::Average,
            ..Default::default()
        };
        let encoding =
            generate_document_fixed_dimensional_encoding(&[2.0, 4.0, 6.0, 8.0], &config).unwrap();
        assert_eq!(encoding, vec![4.0, 6.0]);
    }

    /// C++: `simhash_partitioning_test`.
    #[test]
    fn simhash_partitioning_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 2,
            num_repetitions: 1,
            num_simhash_projections: 2,
            encoding_type: EncodingType::DefaultSum,
            ..Default::default()
        };
        let data = [1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0];
        let encoding = generate_fixed_dimensional_encoding(&data, &config).unwrap();
        assert_eq!(encoding.len(), 8);
        assert!(encoding.iter().any(|v| v.abs() > 1e-6));
    }

    /// C++: `ams_projection_test`.
    #[test]
    fn ams_projection_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 4,
            num_repetitions: 1,
            num_simhash_projections: 0,
            projection_type: ProjectionType::AmsSketch,
            projection_dimension: 2,
            encoding_type: EncodingType::DefaultSum,
            ..Default::default()
        };
        let encoding = generate_fixed_dimensional_encoding(&[1.0, 2.0, 3.0, 4.0], &config).unwrap();
        assert_eq!(encoding.len(), 2);
        assert!(encoding.iter().any(|v| v.abs() > 1e-6));
    }

    /// C++: `final_projection_test`.
    #[test]
    fn final_projection_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 2,
            num_repetitions: 1,
            num_simhash_projections: 0,
            encoding_type: EncodingType::DefaultSum,
            final_projection_dimension: Some(5),
            ..Default::default()
        };
        let encoding = generate_fixed_dimensional_encoding(&[1.0, 2.0, 3.0, 4.0], &config).unwrap();
        assert_eq!(encoding.len(), 5);
        assert!(encoding.iter().any(|v| v.abs() > 1e-6));
    }

    /// C++: `multiple_repetitions_test`.
    #[test]
    fn multiple_repetitions_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 2,
            num_repetitions: 3,
            num_simhash_projections: 1,
            encoding_type: EncodingType::DefaultSum,
            ..Default::default()
        };
        let encoding = generate_fixed_dimensional_encoding(&[1.0, 2.0], &config).unwrap();
        assert_eq!(encoding.len(), 12);
        for rep in 0..3 {
            assert!(
                encoding[rep * 4..(rep + 1) * 4]
                    .iter()
                    .any(|v| v.abs() > 1e-6),
                "repetition {rep}"
            );
        }
    }

    /// C++: `fill_empty_partitions_test`.
    #[test]
    fn fill_empty_partitions_test() {
        let config = FixedDimensionalEncodingConfig {
            dimension: 2,
            num_repetitions: 1,
            num_simhash_projections: 3,
            encoding_type: EncodingType::Average,
            fill_empty_partitions: true,
            ..Default::default()
        };
        let encoding =
            generate_document_fixed_dimensional_encoding(&[1.0, 1.0, 1.1, 1.1], &config).unwrap();
        assert_eq!(encoding.len(), 16);
        let zero_partitions = (0..8)
            .filter(|p| encoding[p * 2..p * 2 + 2].iter().all(|v| v.abs() <= 1e-6))
            .count();
        assert!(zero_partitions <= 6, "zero partitions {zero_partitions}");
    }
}
