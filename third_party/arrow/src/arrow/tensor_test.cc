// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// Unit tests for DataType (and subclasses), Field, and Schema

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "arrow/buffer.h"
#include "arrow/tensor.h"
#include "arrow/testing/gtest_util.h"
#include "arrow/type.h"

namespace arrow {

void AssertCountNonZero(const Tensor& t, int64_t expected) {
  ASSERT_OK_AND_ASSIGN(int64_t count, t.CountNonZero());
  ASSERT_EQ(count, expected);
}

TEST(TestComputeRowMajorStrides, ZeroDimension) {
  std::vector<int64_t> strides;

  std::vector<int64_t> shape1 = {0, 2, 3};
  ASSERT_OK(arrow::internal::ComputeRowMajorStrides(DoubleType(), shape1, &strides));
  EXPECT_THAT(strides,
              testing::ElementsAre(sizeof(double), sizeof(double), sizeof(double)));

  std::vector<int64_t> shape2 = {2, 0, 3};
  strides.clear();
  ASSERT_OK(arrow::internal::ComputeRowMajorStrides(DoubleType(), shape2, &strides));
  EXPECT_THAT(strides,
              testing::ElementsAre(sizeof(double), sizeof(double), sizeof(double)));

  std::vector<int64_t> shape3 = {2, 3, 0};
  strides.clear();
  ASSERT_OK(arrow::internal::ComputeRowMajorStrides(DoubleType(), shape3, &strides));
  EXPECT_THAT(strides,
              testing::ElementsAre(sizeof(double), sizeof(double), sizeof(double)));
}

TEST(TestComputeRowMajorStrides, MaximumSize) {
  constexpr uint64_t total_length =
      1 + static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  std::vector<int64_t> shape = {2, 2, static_cast<int64_t>(total_length / 4)};

  std::vector<int64_t> strides;
  ASSERT_OK(arrow::internal::ComputeRowMajorStrides(Int8Type(), shape, &strides));
  EXPECT_THAT(strides, testing::ElementsAre(2 * shape[2], shape[2], 1));
}

TEST(TestComputeRowMajorStrides, OverflowCase) {
  constexpr uint64_t total_length =
      1 + static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  std::vector<int64_t> shape = {2, 2, static_cast<int64_t>(total_length / 4)};

  std::vector<int64_t> strides;
  EXPECT_RAISES_WITH_MESSAGE_THAT(
      Invalid,
      testing::HasSubstr(
          "Row-major strides computed from shape would not fit in 64-bit integer"),
      arrow::internal::ComputeRowMajorStrides(Int16Type(), shape, &strides));
  EXPECT_EQ(0, strides.size());
}

TEST(TestComputeColumnMajorStrides, ZeroDimension) {
  std::vector<int64_t> strides;

  std::vector<int64_t> shape1 = {0, 2, 3};
  ASSERT_OK(arrow::internal::ComputeColumnMajorStrides(DoubleType(), shape1, &strides));
  EXPECT_THAT(strides,
              testing::ElementsAre(sizeof(double), sizeof(double), sizeof(double)));

  std::vector<int64_t> shape2 = {2, 0, 3};
  strides.clear();
  ASSERT_OK(arrow::internal::ComputeColumnMajorStrides(DoubleType(), shape2, &strides));
  EXPECT_THAT(strides,
              testing::ElementsAre(sizeof(double), sizeof(double), sizeof(double)));

  std::vector<int64_t> shape3 = {2, 3, 0};
  strides.clear();
  ASSERT_OK(arrow::internal::ComputeColumnMajorStrides(DoubleType(), shape3, &strides));
  EXPECT_THAT(strides,
              testing::ElementsAre(sizeof(double), sizeof(double), sizeof(double)));
}

TEST(TestComputeColumnMajorStrides, MaximumSize) {
  constexpr uint64_t total_length =
      1 + static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  std::vector<int64_t> shape = {static_cast<int64_t>(total_length / 4), 2, 2};

  std::vector<int64_t> strides;
  ASSERT_OK(arrow::internal::ComputeColumnMajorStrides(Int8Type(), shape, &strides));
  EXPECT_THAT(strides, testing::ElementsAre(1, shape[0], 2 * shape[0]));
}

TEST(TestComputeColumnMajorStrides, OverflowCase) {
  constexpr uint64_t total_length =
      1 + static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  std::vector<int64_t> shape = {static_cast<int64_t>(total_length / 4), 2, 2};

  std::vector<int64_t> strides;
  EXPECT_RAISES_WITH_MESSAGE_THAT(
      Invalid,
      testing::HasSubstr(
          "Column-major strides computed from shape would not fit in 64-bit integer"),
      arrow::internal::ComputeColumnMajorStrides(Int16Type(), shape, &strides));
  EXPECT_EQ(0, strides.size());
}

TEST(TestTensor, MakeRowMajor) {
  std::vector<int64_t> shape = {3, 6};
  std::vector<int64_t> strides = {sizeof(double) * 6, sizeof(double)};
  std::vector<double> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto data = Buffer::Wrap(values);

  // without strides and dim_names
  std::shared_ptr<Tensor> tensor1;
  ASSERT_OK_AND_ASSIGN(tensor1, Tensor::Make(float64(), data, shape));
  EXPECT_EQ(float64(), tensor1->type());
  EXPECT_EQ(shape, tensor1->shape());
  EXPECT_EQ(strides, tensor1->strides());
  EXPECT_EQ(std::vector<std::string>{}, tensor1->dim_names());
  EXPECT_EQ(data->data(), tensor1->raw_data());
  EXPECT_TRUE(tensor1->is_row_major());
  EXPECT_FALSE(tensor1->is_column_major());
  EXPECT_TRUE(tensor1->is_contiguous());

  // without dim_names
  std::shared_ptr<Tensor> tensor2;
  ASSERT_OK_AND_ASSIGN(tensor2, Tensor::Make(float64(), data, shape, strides));
  EXPECT_EQ(float64(), tensor2->type());
  EXPECT_EQ(shape, tensor2->shape());
  EXPECT_EQ(strides, tensor2->strides());
  EXPECT_EQ(std::vector<std::string>{}, tensor2->dim_names());
  EXPECT_EQ(data->data(), tensor2->raw_data());
  EXPECT_TRUE(tensor2->Equals(*tensor1));
  EXPECT_TRUE(tensor2->is_row_major());
  EXPECT_FALSE(tensor2->is_column_major());
  EXPECT_TRUE(tensor2->is_contiguous());

  // without strides
  std::vector<std::string> dim_names = {"foo", "bar"};
  std::shared_ptr<Tensor> tensor3;
  ASSERT_OK_AND_ASSIGN(tensor3, Tensor::Make(float64(), data, shape, {}, dim_names));
  EXPECT_EQ(float64(), tensor3->type());
  EXPECT_EQ(shape, tensor3->shape());
  EXPECT_EQ(strides, tensor3->strides());
  EXPECT_EQ(dim_names, tensor3->dim_names());
  EXPECT_EQ(data->data(), tensor3->raw_data());
  EXPECT_TRUE(tensor3->Equals(*tensor1));
  EXPECT_TRUE(tensor3->Equals(*tensor2));

  // supply all parameters
  std::shared_ptr<Tensor> tensor4;
  ASSERT_OK_AND_ASSIGN(tensor4, Tensor::Make(float64(), data, shape, strides, dim_names));
  EXPECT_EQ(float64(), tensor4->type());
  EXPECT_EQ(shape, tensor4->shape());
  EXPECT_EQ(strides, tensor4->strides());
  EXPECT_EQ(dim_names, tensor4->dim_names());
  EXPECT_EQ(data->data(), tensor4->raw_data());
  EXPECT_TRUE(tensor4->Equals(*tensor1));
  EXPECT_TRUE(tensor4->Equals(*tensor2));
  EXPECT_TRUE(tensor4->Equals(*tensor3));
}

TEST(TestTensor, MakeColumnMajor) {
  std::vector<int64_t> shape = {3, 6};
  std::vector<int64_t> strides = {sizeof(double), sizeof(double) * 3};
  std::vector<double> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto data = Buffer::Wrap(values);

  std::shared_ptr<Tensor> tensor;
  ASSERT_OK_AND_ASSIGN(tensor, Tensor::Make(float64(), data, shape, strides));
  EXPECT_FALSE(tensor->is_row_major());
  EXPECT_TRUE(tensor->is_column_major());
  EXPECT_TRUE(tensor->is_contiguous());
}

TEST(TestTensor, MakeStrided) {
  std::vector<int64_t> shape = {3, 6};
  std::vector<int64_t> strides = {sizeof(double) * 12, sizeof(double) * 2};
  std::vector<double> values = {1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0,
                                1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0};
  auto data = Buffer::Wrap(values);

  std::shared_ptr<Tensor> tensor;
  ASSERT_OK_AND_ASSIGN(tensor, Tensor::Make(float64(), data, shape, strides));
  EXPECT_FALSE(tensor->is_row_major());
  EXPECT_FALSE(tensor->is_column_major());
  EXPECT_FALSE(tensor->is_contiguous());
}

TEST(TestTensor, MakeZeroDim) {
  std::vector<int64_t> shape = {};
  std::vector<double> values = {355 / 113.0};
  auto data = Buffer::Wrap(values);
  std::shared_ptr<Tensor> tensor;

  ASSERT_OK_AND_ASSIGN(tensor, Tensor::Make(float64(), data, shape));
  EXPECT_EQ(1, tensor->size());
  EXPECT_EQ(shape, tensor->shape());
  EXPECT_EQ(shape, tensor->strides());
  EXPECT_EQ(data->data(), tensor->raw_data());
  EXPECT_EQ(values[0], tensor->Value<DoubleType>({}));
}

TEST(TestTensor, MakeFailureCases) {
  std::vector<int64_t> shape = {3, 6};
  std::vector<double> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto data = Buffer::Wrap(values);

  // null type
  ASSERT_RAISES(Invalid, Tensor::Make(nullptr, data, shape));

  // invalid type
  ASSERT_RAISES(Invalid, Tensor::Make(binary(), data, shape));

  // null data
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), nullptr, shape));

  // negative items in shape
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), data, {-3, 6}));

  // overflow in positive strides computation
  constexpr uint64_t total_length =
      1 + static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  EXPECT_RAISES_WITH_MESSAGE_THAT(
      Invalid,
      testing::HasSubstr(
          "Row-major strides computed from shape would not fit in 64-bit integer"),
      Tensor::Make(float64(), data, {2, 2, static_cast<int64_t>(total_length / 4)}));

  // negative strides are prohibited
  EXPECT_RAISES_WITH_MESSAGE_THAT(
      Invalid, testing::HasSubstr("negative strides not supported"),
      Tensor::Make(float64(), data, {18}, {-(int)sizeof(double)}));

  // invalid stride length
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), data, shape, {sizeof(double)}));
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), data, shape,
                                      {sizeof(double), sizeof(double), sizeof(double)}));

  // invalid stride values to involve buffer over run
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), data, shape,
                                      {sizeof(double) * 6, sizeof(double) * 2}));
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), data, shape,
                                      {sizeof(double) * 12, sizeof(double)}));

  // too many dim_names are supplied
  ASSERT_RAISES(Invalid, Tensor::Make(float64(), data, shape, {}, {"foo", "bar", "baz"}));
}

TEST(TestTensor, ZeroDim) {
  const int64_t values = 1;
  std::vector<int64_t> shape = {};

  using T = int64_t;

  ASSERT_OK_AND_ASSIGN(std::shared_ptr<Buffer> buffer,
                       AllocateBuffer(values * sizeof(T)));

  Tensor t0(int64(), buffer, shape);

  ASSERT_EQ(1, t0.size());
}

TEST(TestTensor, BasicCtors) {
  const int64_t values = 24;
  std::vector<int64_t> shape = {4, 6};
  std::vector<int64_t> strides = {48, 8};
  std::vector<std::string> dim_names = {"foo", "bar"};

  using T = int64_t;

  ASSERT_OK_AND_ASSIGN(std::shared_ptr<Buffer> buffer,
                       AllocateBuffer(values * sizeof(T)));

  Tensor t1(int64(), buffer, shape);
  Tensor t2(int64(), buffer, shape, strides);
  Tensor t3(int64(), buffer, shape, strides, dim_names);

  ASSERT_EQ(24, t1.size());
  ASSERT_TRUE(t1.is_mutable());

  ASSERT_EQ(strides, t1.strides());
  ASSERT_EQ(strides, t2.strides());

  ASSERT_EQ(std::vector<std::string>({"foo", "bar"}), t3.dim_names());
  ASSERT_EQ("foo", t3.dim_name(0));
  ASSERT_EQ("bar", t3.dim_name(1));

  ASSERT_EQ(std::vector<std::string>({}), t1.dim_names());
  ASSERT_EQ("", t1.dim_name(0));
  ASSERT_EQ("", t1.dim_name(1));
}

TEST(TestTensor, IsContiguous) {
  const int64_t values = 24;
  std::vector<int64_t> shape = {4, 6};
  std::vector<int64_t> strides = {48, 8};

  using T = int64_t;

  ASSERT_OK_AND_ASSIGN(std::shared_ptr<Buffer> buffer,
                       AllocateBuffer(values * sizeof(T)));

  std::vector<int64_t> c_strides = {48, 8};
  std::vector<int64_t> f_strides = {8, 32};
  std::vector<int64_t> noncontig_strides = {8, 8};
  Tensor t1(int64(), buffer, shape, c_strides);
  Tensor t2(int64(), buffer, shape, f_strides);
  Tensor t3(int64(), buffer, shape, noncontig_strides);

  ASSERT_TRUE(t1.is_contiguous());
  ASSERT_TRUE(t2.is_contiguous());
  ASSERT_FALSE(t3.is_contiguous());
}

TEST(TestTensor, ZeroSizedTensor) {
  std::vector<int64_t> shape = {0};

  ASSERT_OK_AND_ASSIGN(std::shared_ptr<Buffer> buffer, AllocateBuffer(0));

  Tensor t(int64(), buffer, shape);
  ASSERT_EQ(t.strides().size(), 1);
}

TEST(TestTensor, CountNonZeroForZeroSizedTensor) {
  std::vector<int64_t> shape = {0};

  ASSERT_OK_AND_ASSIGN(std::shared_ptr<Buffer> buffer, AllocateBuffer(0));

  Tensor t(int64(), buffer, shape);
  AssertCountNonZero(t, 0);
}

TEST(TestTensor, CountNonZeroForContiguousTensor) {
  std::vector<int64_t> shape = {4, 6};
  std::vector<int64_t> values = {1, 0,  2, 0,  0,  3, 0,  4, 5, 0,  6, 0,
                                 0, 11, 0, 12, 13, 0, 14, 0, 0, 15, 0, 16};
  std::shared_ptr<Buffer> buffer = Buffer::Wrap(values);

  std::vector<int64_t> c_strides = {48, 8};
  std::vector<int64_t> f_strides = {8, 32};
  Tensor t1(int64(), buffer, shape, c_strides);
  Tensor t2(int64(), buffer, shape, f_strides);

  ASSERT_TRUE(t1.is_contiguous());
  ASSERT_TRUE(t2.is_contiguous());
  AssertCountNonZero(t1, 12);
  AssertCountNonZero(t2, 12);
}

TEST(TestTensor, CountNonZeroForNonContiguousTensor) {
  std::vector<int64_t> shape = {4, 4};
  std::vector<int64_t> values = {
      1, 0,  2, 0,  0,  3, 0,  4, 5, 0,  6, 0,  7, 0,  8, 0,
      0, 11, 0, 12, 13, 0, 14, 0, 0, 15, 0, 16, 0, 15, 0, 16,
  };
  std::shared_ptr<Buffer> buffer = Buffer::Wrap(values);

  std::vector<int64_t> noncontig_strides = {64, 16};
  Tensor t(int64(), buffer, shape, noncontig_strides);

  ASSERT_FALSE(t.is_contiguous());
  AssertCountNonZero(t, 8);
}

TEST(TestTensor, ElementAccessInt32) {
  std::vector<int64_t> shape = {2, 3};
  std::vector<int32_t> values = {1, 2, 3, 4, 5, 6};
  std::vector<int64_t> c_strides = {sizeof(int32_t) * 3, sizeof(int32_t)};
  std::vector<int64_t> f_strides = {sizeof(int32_t), sizeof(int32_t) * 2};
  Tensor tc(int64(), Buffer::Wrap(values), shape, c_strides);
  Tensor tf(int64(), Buffer::Wrap(values), shape, f_strides);

  EXPECT_EQ(1, tc.Value<Int32Type>({0, 0}));
  EXPECT_EQ(2, tc.Value<Int32Type>({0, 1}));
  EXPECT_EQ(4, tc.Value<Int32Type>({1, 0}));

  EXPECT_EQ(1, tf.Value<Int32Type>({0, 0}));
  EXPECT_EQ(3, tf.Value<Int32Type>({0, 1}));
  EXPECT_EQ(2, tf.Value<Int32Type>({1, 0}));

  // Tensor::Value<T>() doesn't prohibit element access if the type T is different from
  // the value type of the tensor
  EXPECT_NO_THROW({
    int32_t x = 3;
    EXPECT_EQ(*reinterpret_cast<int8_t*>(&x), tc.Value<Int8Type>({0, 2}));

    union {
      int64_t i64;
      struct {
        int32_t first;
        int32_t second;
      } i32;
    } y;
    y.i32.first = 4;
    y.i32.second = 5;
    EXPECT_EQ(y.i64, tc.Value<Int64Type>({1, 0}));
  });
}

TEST(TestTensor, EqualsInt64) {
  std::vector<int64_t> shape = {4, 4};

  std::vector<int64_t> c_values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  std::vector<int64_t> c_strides = {32, 8};
  Tensor tc1(int64(), Buffer::Wrap(c_values), shape, c_strides);

  std::vector<int64_t> c_values_2 = c_values;
  Tensor tc2(int64(), Buffer::Wrap(c_values_2), shape, c_strides);

  std::vector<int64_t> f_values = {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};
  Tensor tc3(int64(), Buffer::Wrap(f_values), shape, c_strides);

  Tensor tc4(int64(), Buffer::Wrap(c_values), {8, 2}, {16, 8});

  std::vector<int64_t> f_strides = {8, 32};
  Tensor tf1(int64(), Buffer::Wrap(f_values), shape, f_strides);

  std::vector<int64_t> f_values_2 = f_values;
  Tensor tf2(int64(), Buffer::Wrap(f_values_2), shape, f_strides);

  Tensor tf3(int64(), Buffer::Wrap(c_values), shape, f_strides);

  std::vector<int64_t> nc_values = {1, 0, 5, 0, 9,  0, 13, 0, 2, 0, 6, 0, 10, 0, 14, 0,
                                    3, 0, 7, 0, 11, 0, 15, 0, 4, 0, 8, 0, 12, 0, 16, 0};
  std::vector<int64_t> nc_strides = {16, 64};
  Tensor tnc(int64(), Buffer::Wrap(nc_values), shape, nc_strides);

  ASSERT_TRUE(tc1.is_contiguous());
  ASSERT_TRUE(tc1.is_row_major());

  ASSERT_TRUE(tf1.is_contiguous());
  ASSERT_TRUE(tf1.is_column_major());

  ASSERT_FALSE(tnc.is_contiguous());

  // same object
  EXPECT_TRUE(tc1.Equals(tc1));
  EXPECT_TRUE(tf1.Equals(tf1));
  EXPECT_TRUE(tnc.Equals(tnc));

  // different memory
  EXPECT_TRUE(tc1.Equals(tc2));
  EXPECT_TRUE(tf1.Equals(tf2));
  EXPECT_FALSE(tc1.Equals(tc3));

  // different shapes but same data
  EXPECT_FALSE(tc1.Equals(tc4));

  // row-major and column-major
  EXPECT_TRUE(tc1.Equals(tf1));
  EXPECT_FALSE(tc3.Equals(tf1));

  // row-major and non-contiguous
  EXPECT_TRUE(tc1.Equals(tnc));
  EXPECT_FALSE(tc3.Equals(tnc));

  // column-major and non-contiguous
  EXPECT_TRUE(tf1.Equals(tnc));
  EXPECT_FALSE(tf3.Equals(tnc));

  // zero-size tensor
  ASSERT_OK_AND_ASSIGN(auto empty_buffer1, AllocateBuffer(0));
  ASSERT_OK_AND_ASSIGN(auto empty_buffer2, AllocateBuffer(0));
  Tensor empty1(int64(), std::move(empty_buffer1), {0});
  Tensor empty2(int64(), std::move(empty_buffer2), {0});
  EXPECT_FALSE(empty1.Equals(tc1));
  EXPECT_TRUE(empty1.Equals(empty2));
}

template <typename DataType>
class TestFloatTensor : public ::testing::Test {};

TYPED_TEST_SUITE_P(TestFloatTensor);

TYPED_TEST_P(TestFloatTensor, Equals) {
  using DataType = TypeParam;
  using c_data_type = typename DataType::c_type;
  const int unit_size = sizeof(c_data_type);

  std::vector<int64_t> shape = {4, 4};

  std::vector<c_data_type> c_values = {1, 2,  3,  4,  5,  6,  7,  8,
                                       9, 10, 11, 12, 13, 14, 15, 16};
  std::vector<int64_t> c_strides = {unit_size * shape[1], unit_size};
  Tensor tc1(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(c_values), shape,
             c_strides);

  std::vector<c_data_type> c_values_2 = c_values;
  Tensor tc2(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(c_values_2), shape,
             c_strides);

  std::vector<c_data_type> f_values = {1, 5, 9,  13, 2, 6, 10, 14,
                                       3, 7, 11, 15, 4, 8, 12, 16};
  Tensor tc3(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(f_values), shape,
             c_strides);

  std::vector<int64_t> f_strides = {unit_size, unit_size * shape[0]};
  Tensor tf1(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(f_values), shape,
             f_strides);

  std::vector<c_data_type> f_values_2 = f_values;
  Tensor tf2(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(f_values_2), shape,
             f_strides);

  Tensor tf3(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(c_values), shape,
             f_strides);

  std::vector<c_data_type> nc_values = {1,  0,  5, 0,  9, 0, 13, 0, 2,  0,  6,
                                        0,  10, 0, 14, 0, 3, 0,  7, 0,  11, 0,
                                        15, 0,  4, 0,  8, 0, 12, 0, 16, 0};
  std::vector<int64_t> nc_strides = {unit_size * 2, unit_size * 2 * shape[0]};
  Tensor tnc(TypeTraits<DataType>::type_singleton(), Buffer::Wrap(nc_values), shape,
             nc_strides);

  ASSERT_TRUE(tc1.is_contiguous());
  ASSERT_TRUE(tc1.is_row_major());

  ASSERT_TRUE(tf1.is_contiguous());
  ASSERT_TRUE(tf1.is_column_major());

  ASSERT_FALSE(tnc.is_contiguous());

  // same object
  EXPECT_TRUE(tc1.Equals(tc1));
  EXPECT_TRUE(tf1.Equals(tf1));
  EXPECT_TRUE(tnc.Equals(tnc));

  // different memory
  EXPECT_TRUE(tc1.Equals(tc2));
  EXPECT_TRUE(tf1.Equals(tf2));
  EXPECT_FALSE(tc1.Equals(tc3));

  // row-major and column-major
  EXPECT_TRUE(tc1.Equals(tf1));
  EXPECT_FALSE(tc3.Equals(tf1));

  // row-major and non-contiguous
  EXPECT_TRUE(tc1.Equals(tnc));
  EXPECT_FALSE(tc3.Equals(tnc));

  // column-major and non-contiguous
  EXPECT_TRUE(tf1.Equals(tnc));
  EXPECT_FALSE(tf3.Equals(tnc));

  // signed zeros
  c_values[0] = -0.0;
  c_values_2[0] = 0.0;
  EXPECT_TRUE(tc1.Equals(tc2));
  EXPECT_FALSE(tc1.Equals(tc2, EqualOptions().signed_zeros_equal(false)));

  // tensors with NaNs
  const c_data_type nan_value = static_cast<c_data_type>(NAN);
  c_values[0] = nan_value;
  EXPECT_TRUE(std::isnan(tc1.Value<DataType>({0, 0})));
  EXPECT_FALSE(tc1.Equals(tc1));                                  // same object
  EXPECT_TRUE(tc1.Equals(tc1, EqualOptions().nans_equal(true)));  // same object
  EXPECT_FALSE(std::isnan(tc2.Value<DataType>({0, 0})));
  EXPECT_FALSE(tc1.Equals(tc2));                                   // different memory
  EXPECT_FALSE(tc1.Equals(tc2, EqualOptions().nans_equal(true)));  // different memory

  c_values_2[0] = nan_value;
  EXPECT_TRUE(std::isnan(tc2.Value<DataType>({0, 0})));
  EXPECT_FALSE(tc1.Equals(tc2));                                  // different memory
  EXPECT_TRUE(tc1.Equals(tc2, EqualOptions().nans_equal(true)));  // different memory
}

REGISTER_TYPED_TEST_SUITE_P(TestFloatTensor, Equals);

INSTANTIATE_TYPED_TEST_SUITE_P(Float32, TestFloatTensor, FloatType);
INSTANTIATE_TYPED_TEST_SUITE_P(Float64, TestFloatTensor, DoubleType);

TEST(TestNumericTensor, Make) {
  std::vector<int64_t> shape = {3, 6};
  std::vector<int64_t> strides = {sizeof(double) * 6, sizeof(double)};
  std::vector<double> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto data = Buffer::Wrap(values);

  // without strides and dim_names
  std::shared_ptr<NumericTensor<DoubleType>> tensor1;
  ASSERT_OK_AND_ASSIGN(tensor1, NumericTensor<DoubleType>::Make(data, shape));
  EXPECT_EQ(float64(), tensor1->type());
  EXPECT_EQ(shape, tensor1->shape());
  EXPECT_EQ(strides, tensor1->strides());
  EXPECT_EQ(data->data(), tensor1->raw_data());
  EXPECT_EQ(std::vector<std::string>{}, tensor1->dim_names());

  // without dim_names
  std::shared_ptr<NumericTensor<DoubleType>> tensor2;
  ASSERT_OK_AND_ASSIGN(tensor2, NumericTensor<DoubleType>::Make(data, shape, strides));
  EXPECT_EQ(float64(), tensor2->type());
  EXPECT_EQ(shape, tensor2->shape());
  EXPECT_EQ(strides, tensor2->strides());
  EXPECT_EQ(std::vector<std::string>{}, tensor2->dim_names());
  EXPECT_EQ(data->data(), tensor2->raw_data());
  EXPECT_TRUE(tensor2->Equals(*tensor1));

  // without strides
  std::vector<std::string> dim_names = {"foo", "bar"};
  std::shared_ptr<NumericTensor<DoubleType>> tensor3;
  ASSERT_OK_AND_ASSIGN(tensor3,
                       NumericTensor<DoubleType>::Make(data, shape, {}, dim_names));
  EXPECT_EQ(float64(), tensor3->type());
  EXPECT_EQ(shape, tensor3->shape());
  EXPECT_EQ(strides, tensor3->strides());
  EXPECT_EQ(dim_names, tensor3->dim_names());
  EXPECT_EQ(data->data(), tensor3->raw_data());
  EXPECT_TRUE(tensor3->Equals(*tensor1));
  EXPECT_TRUE(tensor3->Equals(*tensor2));

  // supply all parameters
  std::shared_ptr<NumericTensor<DoubleType>> tensor4;
  ASSERT_OK_AND_ASSIGN(tensor4,
                       NumericTensor<DoubleType>::Make(data, shape, strides, dim_names));
  EXPECT_EQ(float64(), tensor4->type());
  EXPECT_EQ(shape, tensor4->shape());
  EXPECT_EQ(strides, tensor4->strides());
  EXPECT_EQ(dim_names, tensor4->dim_names());
  EXPECT_EQ(data->data(), tensor4->raw_data());
  EXPECT_TRUE(tensor4->Equals(*tensor1));
  EXPECT_TRUE(tensor4->Equals(*tensor2));
  EXPECT_TRUE(tensor4->Equals(*tensor3));
}

TEST(TestNumericTensor, ElementAccessWithRowMajorStrides) {
  std::vector<int64_t> shape = {3, 4};

  std::vector<int64_t> values_i64 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  std::shared_ptr<Buffer> buffer_i64(Buffer::Wrap(values_i64));
  NumericTensor<Int64Type> t_i64(buffer_i64, shape);

  ASSERT_TRUE(t_i64.is_row_major());
  ASSERT_FALSE(t_i64.is_column_major());
  ASSERT_TRUE(t_i64.is_contiguous());
  ASSERT_EQ(1, t_i64.Value({0, 0}));
  ASSERT_EQ(5, t_i64.Value({1, 0}));
  ASSERT_EQ(6, t_i64.Value({1, 1}));
  ASSERT_EQ(11, t_i64.Value({2, 2}));

  std::vector<float> values_f32 = {1.1f, 2.1f, 3.1f, 4.1f,  5.1f,  6.1f,
                                   7.1f, 8.1f, 9.1f, 10.1f, 11.1f, 12.1f};
  std::shared_ptr<Buffer> buffer_f32(Buffer::Wrap(values_f32));
  NumericTensor<FloatType> t_f32(buffer_f32, shape);

  ASSERT_TRUE(t_f32.is_row_major());
  ASSERT_FALSE(t_f32.is_column_major());
  ASSERT_TRUE(t_f32.is_contiguous());
  ASSERT_EQ(1.1f, t_f32.Value({0, 0}));
  ASSERT_EQ(5.1f, t_f32.Value({1, 0}));
  ASSERT_EQ(6.1f, t_f32.Value({1, 1}));
  ASSERT_EQ(11.1f, t_f32.Value({2, 2}));
}

TEST(TestNumericTensor, ElementAccessWithColumnMajorStrides) {
  std::vector<int64_t> shape = {3, 4};

  const int64_t i64_size = sizeof(int64_t);
  std::vector<int64_t> values_i64 = {1, 5, 9, 2, 6, 10, 3, 7, 11, 4, 8, 12};
  std::vector<int64_t> strides_i64 = {i64_size, i64_size * 3};
  std::shared_ptr<Buffer> buffer_i64(Buffer::Wrap(values_i64));
  NumericTensor<Int64Type> t_i64(buffer_i64, shape, strides_i64);

  ASSERT_TRUE(t_i64.is_column_major());
  ASSERT_FALSE(t_i64.is_row_major());
  ASSERT_TRUE(t_i64.is_contiguous());
  ASSERT_EQ(1, t_i64.Value({0, 0}));
  ASSERT_EQ(2, t_i64.Value({0, 1}));
  ASSERT_EQ(4, t_i64.Value({0, 3}));
  ASSERT_EQ(5, t_i64.Value({1, 0}));
  ASSERT_EQ(6, t_i64.Value({1, 1}));
  ASSERT_EQ(11, t_i64.Value({2, 2}));

  const int64_t f32_size = sizeof(float);
  std::vector<float> values_f32 = {1.1f, 5.1f, 9.1f,  2.1f, 6.1f, 10.1f,
                                   3.1f, 7.1f, 11.1f, 4.1f, 8.1f, 12.1f};
  std::vector<int64_t> strides_f32 = {f32_size, f32_size * 3};
  std::shared_ptr<Buffer> buffer_f32(Buffer::Wrap(values_f32));
  NumericTensor<FloatType> t_f32(buffer_f32, shape, strides_f32);

  ASSERT_TRUE(t_f32.is_column_major());
  ASSERT_FALSE(t_f32.is_row_major());
  ASSERT_TRUE(t_f32.is_contiguous());
  ASSERT_EQ(1.1f, t_f32.Value({0, 0}));
  ASSERT_EQ(2.1f, t_f32.Value({0, 1}));
  ASSERT_EQ(4.1f, t_f32.Value({0, 3}));
  ASSERT_EQ(5.1f, t_f32.Value({1, 0}));
  ASSERT_EQ(6.1f, t_f32.Value({1, 1}));
  ASSERT_EQ(11.1f, t_f32.Value({2, 2}));
}

TEST(TestNumericTensor, ElementAccessWithNonContiguousStrides) {
  std::vector<int64_t> shape = {3, 4};

  const int64_t i64_size = sizeof(int64_t);
  std::vector<int64_t> values_i64 = {1, 2, 3, 4, 0,  0,  5,  6, 7,
                                     8, 0, 0, 9, 10, 11, 12, 0, 0};
  std::vector<int64_t> strides_i64 = {i64_size * 6, i64_size};
  std::shared_ptr<Buffer> buffer_i64(Buffer::Wrap(values_i64));
  NumericTensor<Int64Type> t_i64(buffer_i64, shape, strides_i64);

  ASSERT_FALSE(t_i64.is_contiguous());
  ASSERT_FALSE(t_i64.is_row_major());
  ASSERT_FALSE(t_i64.is_column_major());
  ASSERT_EQ(1, t_i64.Value({0, 0}));
  ASSERT_EQ(2, t_i64.Value({0, 1}));
  ASSERT_EQ(4, t_i64.Value({0, 3}));
  ASSERT_EQ(5, t_i64.Value({1, 0}));
  ASSERT_EQ(6, t_i64.Value({1, 1}));
  ASSERT_EQ(11, t_i64.Value({2, 2}));

  const int64_t f32_size = sizeof(float);
  std::vector<float> values_f32 = {1.1f, 2.1f,  3.1f,  4.1f,  0.0f, 0.0f,
                                   5.1f, 6.1f,  7.1f,  8.1f,  0.0f, 0.0f,
                                   9.1f, 10.1f, 11.1f, 12.1f, 0.0f, 0.0f};
  std::vector<int64_t> strides_f32 = {f32_size * 6, f32_size};
  std::shared_ptr<Buffer> buffer_f32(Buffer::Wrap(values_f32));
  NumericTensor<FloatType> t_f32(buffer_f32, shape, strides_f32);

  ASSERT_FALSE(t_f32.is_contiguous());
  ASSERT_FALSE(t_f32.is_row_major());
  ASSERT_FALSE(t_f32.is_column_major());
  ASSERT_EQ(1.1f, t_f32.Value({0, 0}));
  ASSERT_EQ(2.1f, t_f32.Value({0, 1}));
  ASSERT_EQ(4.1f, t_f32.Value({0, 3}));
  ASSERT_EQ(5.1f, t_f32.Value({1, 0}));
  ASSERT_EQ(6.1f, t_f32.Value({1, 1}));
  ASSERT_EQ(11.1f, t_f32.Value({2, 2}));
}

}  // namespace arrow
