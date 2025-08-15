
#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.result_cache_manager;

import :result_cache_manager;
import :logical_node;
import :physical_operator;
import :cached_node_base;
import :column_binding;
import :logical_node_type;
import :physical_operator_type;
import :query_context;
import :operator_state;
import :data_block;
#endif

import data_type;
import logical_type;

using namespace infinity;

class MockCachedNode : public CachedNodeBase {
public:
    MockCachedNode(String key, std::shared_ptr<Vector<String>> output_names) : CachedNodeBase(LogicalNodeType::kMock, output_names), key_(std::move(key)) {}

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
    ResultCacheManager cache_manager(100);

    String key1 = "key1";
    auto output_names1 = std::make_shared<Vector<String>>(Vector<String>{"col1", "col2"});
    auto output_types1 = Vector<std::shared_ptr<DataType>>{std::make_unique<DataType>(LogicalType::kInteger), std::make_unique<DataType>(LogicalType::kFloat)};
    auto block1 = std::make_unique<DataBlock>();
    block1->Init(output_types1, 1);
    block1->Finalize();
    Vector<std::unique_ptr<DataBlock>> blocks1;
    blocks1.push_back(std::move(block1));
    auto cached_node1 = std::make_unique<MockCachedNode>(key1, output_names1);

    String key2 = key1;
    auto output_names2 = std::make_shared<Vector<String>>(Vector<String>{"col2"});
    auto output_types2 = Vector<std::shared_ptr<DataType>>{std::make_unique<DataType>(LogicalType::kFloat)};
    auto block2 = std::make_unique<DataBlock>();
    block2->Init(output_types2, 1);
    block2->Finalize();
    Vector<std::unique_ptr<DataBlock>> blocks2;
    blocks2.push_back(std::move(block2));
    auto cached_node2 = std::make_unique<MockCachedNode>(key2, output_names2);

    String key3 = key1;
    auto output_names3 = std::make_shared<Vector<String>>(Vector<String>{"col1", "col2", "col3"});
    auto output_types3 = Vector<std::shared_ptr<DataType>>{std::make_unique<DataType>(LogicalType::kInteger),
                                                     std::make_unique<DataType>(LogicalType::kFloat),
                                                     std::make_unique<DataType>(LogicalType::kFloat)};
    auto block3 = std::make_unique<DataBlock>();
    block3->Init(output_types3, 1);
    block3->Finalize();
    Vector<std::unique_ptr<DataBlock>> blocks3;
    blocks3.push_back(std::move(block3));
    auto cached_node3 = std::make_unique<MockCachedNode>(key3, output_names3);

    String key4 = key1;
    auto output_names4 = std::make_shared<Vector<String>>(Vector<String>{"col3"});
    auto cached_node4 = std::make_unique<MockCachedNode>(key4, output_names4);

    auto res1 = cache_manager.GetCache(*cached_node1);
    EXPECT_FALSE(res1.has_value());

    auto cached_node11 = std::make_unique<MockCachedNode>(key1, output_names1);
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
    EXPECT_FALSE(success3);
    auto res4 = cache_manager.GetCache(*cached_node4);
    EXPECT_TRUE(res4.has_value());
    EXPECT_EQ(res4->column_map_.size(), 1);
    EXPECT_EQ(res4->column_map_[0], 2);
}

TEST(ResultCacheManagerTest, test2) {
    ResultCacheManager cache_manager(2);
    String key1 = "key1";
    auto output_names1 = std::make_shared<Vector<String>>(Vector<String>{"col1"});
    auto cached_node1 = std::make_unique<MockCachedNode>(key1, output_names1);

    String key2 = "key2";
    auto output_names2 = std::make_shared<Vector<String>>(Vector<String>{"col2"});
    auto cached_node2 = std::make_unique<MockCachedNode>(key2, output_names2);

    String key3 = "key3";
    auto output_names3 = std::make_shared<Vector<String>>(Vector<String>{"col1", "col2", "col3"});
    auto cached_node3 = std::make_unique<MockCachedNode>(key3, output_names3);

    auto cached_node11 = std::make_unique<MockCachedNode>(key1, output_names1);
    bool success = cache_manager.AddCache(std::move(cached_node1), {});
    EXPECT_TRUE(success);

    auto cached_node21 = std::make_unique<MockCachedNode>(key2, output_names2);
    bool success2 = cache_manager.AddCache(std::move(cached_node2), {});
    EXPECT_TRUE(success2);

    auto res1 = cache_manager.GetCache(*cached_node11);
    EXPECT_TRUE(res1.has_value());

    bool success3 = cache_manager.AddCache(std::move(cached_node3), {});
    EXPECT_TRUE(success3);

    auto res2 = cache_manager.GetCache(*cached_node21);
    EXPECT_FALSE(res2.has_value());
}
