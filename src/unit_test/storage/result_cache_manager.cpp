#include "gtest/gtest.h"

import stl;
import result_cache_manager;
import logical_node;
import physical_operator;
import data_type;
import cached_node_base;
import column_binding;
import logical_node_type;
import physical_operator_type;
import query_context;
import operator_state;
import logical_type;
import data_block;

using namespace infinity;

class MockCachedNode : public CachedNodeBase {
public:
    MockCachedNode(String key, SharedPtr<Vector<String>> output_names) : CachedNodeBase(LogicalNodeType::kMock, output_names), key_(std::move(key)) {}

    u64 Hash() const override {
        u64 h = CachedNodeBase::Hash();
        h ^= std::hash<String>{}(key_);
        return h;
    }

    bool Eq(const CachedNodeBase &other_base) const override {
        if (type() != other_base.type()) {
            return false;
        }
        const auto &other = static_cast<const MockCachedNode &>(other_base);
        return key_ == other.key_;
    }

private:
    String key_;
};

TEST(ResultCacheManagerTest, test1) {
    ResultCacheManager cache_manager;

    String key1 = "key1";
    auto output_names1 = MakeShared<Vector<String>>(Vector<String>{"col1", "col2"});
    auto output_types1 = Vector<SharedPtr<DataType>>{MakeUnique<DataType>(LogicalType::kInteger), MakeUnique<DataType>(LogicalType::kFloat)};
    auto block1 = MakeUnique<DataBlock>();
    block1->Init(output_types1, 1);
    block1->Finalize();
    Vector<UniquePtr<DataBlock>> blocks1;
    blocks1.push_back(std::move(block1));
    auto cached_node1 = MakeUnique<MockCachedNode>(key1, output_names1);

    String key2 = key1;
    auto output_names2 = MakeShared<Vector<String>>(Vector<String>{"col2"});
    auto output_types2 = Vector<SharedPtr<DataType>>{MakeUnique<DataType>(LogicalType::kFloat)};
    auto block2 = MakeUnique<DataBlock>();
    block2->Init(output_types2, 1);
    block2->Finalize();
    Vector<UniquePtr<DataBlock>> blocks2;
    blocks2.push_back(std::move(block2));
    auto cached_node2 = MakeUnique<MockCachedNode>(key2, output_names2);

    String key3 = key1;
    auto output_names3 = MakeShared<Vector<String>>(Vector<String>{"col1", "col2", "col3"});
    auto output_types3 = Vector<SharedPtr<DataType>>{MakeUnique<DataType>(LogicalType::kInteger),
                                                     MakeUnique<DataType>(LogicalType::kFloat),
                                                     MakeUnique<DataType>(LogicalType::kFloat)};
    auto block3 = MakeUnique<DataBlock>();
    block3->Init(output_types3, 1);
    block3->Finalize();
    Vector<UniquePtr<DataBlock>> blocks3;
    blocks3.push_back(std::move(block3));
    auto cached_node3 = MakeUnique<MockCachedNode>(key3, output_names3);

    String key4 = key1;
    auto output_names4 = MakeShared<Vector<String>>(Vector<String>{"col3"});
    auto cached_node4 = MakeUnique<MockCachedNode>(key4, output_names4);

    auto res1 = cache_manager.GetCache(*cached_node1);
    EXPECT_FALSE(res1.has_value());

    auto cached_node11 = MakeUnique<MockCachedNode>(key1, output_names1);
    bool success = cache_manager.AddCache(std::move(cached_node1), std::move(blocks1));
    EXPECT_TRUE(success);
    auto res11 = cache_manager.GetCache(*cached_node11);
    EXPECT_TRUE(res11.has_value());
    EXPECT_EQ(res11->column_map_.size(), 2);
    EXPECT_EQ(res11->column_map_[0], 0);
    EXPECT_EQ(res11->column_map_[1], 1);

    auto res2 = cache_manager.GetCache(*cached_node2);
    EXPECT_TRUE(res2.has_value());
    EXPECT_EQ(res2->column_map_.size(), 1);
    EXPECT_EQ(res2->column_map_[0], 1);

    auto res3 = cache_manager.GetCache(*cached_node3);
    EXPECT_FALSE(res3.has_value());
    bool success3 = cache_manager.AddCache(std::move(cached_node3), std::move(blocks3));
    EXPECT_TRUE(success3);
    auto res4 = cache_manager.GetCache(*cached_node4);
    EXPECT_TRUE(res4.has_value());
    EXPECT_EQ(res4->column_map_.size(), 1);
    EXPECT_EQ(res4->column_map_[0], 2);
}
