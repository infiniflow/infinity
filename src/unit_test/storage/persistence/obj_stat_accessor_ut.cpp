module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.obj_stat_accessor;

import :ut.base_test;

import :object_stats;
import :obj_status;
//
// using namespace infinity;
//
// class ObjectStatMapTest : public BaseTest {};
//
// TEST_F(ObjectStatMapTest, test1) {
//     size_t disk_capacity_limit = 10;
//     ObjectStatAccessor_ObjectStorage obj_map(disk_capacity_limit);
//
//     std::optional<ObjStat> obj_stat = obj_map.Get("key1");
//     EXPECT_EQ(obj_stat, std::nullopt);
//
//     std::vector<std::string> drop_keys;
//     obj_map.PutNew("key1", ObjStat(4, 1, 0), drop_keys);
//     EXPECT_EQ(drop_keys.size(), 0);
//
//     obj_map.PutNew("key2", ObjStat(4, 1, 0), drop_keys);
//     EXPECT_EQ(drop_keys.size(), 0);
//
//     std::optional<ObjStat> stat1 = obj_map.Get("key1");
//     EXPECT_EQ(stat1->obj_size_, 4);
//     EXPECT_EQ(stat1->ref_count_, 1);
//     stat1 = obj_map.Release("key1", drop_keys);
//     EXPECT_EQ(stat1->ref_count_, 0);
//     EXPECT_EQ(drop_keys.size(), 0);
//
//     obj_map.PutNew("key3", ObjStat(4, 1, 0), drop_keys);
//     EXPECT_EQ(drop_keys.size(), 1);
//     EXPECT_EQ(drop_keys[0], "key2");
//     drop_keys.clear();
//     EXPECT_EQ(obj_map.disk_used(), 8);
//
//     std::optional<ObjStat> stat2 = obj_map.GetNoCount("key2");
//     EXPECT_NE(stat2, std::nullopt);
//     EXPECT_EQ(stat2->cached_, ObjCached::kNotCached);
//     std::optional<ObjStat> stat2_opt = obj_map.Invalidate("key2");
//     EXPECT_TRUE(stat2_opt.has_value());
//     stat2 = obj_map.GetNoCount("key2");
//     EXPECT_EQ(stat2, std::nullopt);
// }
