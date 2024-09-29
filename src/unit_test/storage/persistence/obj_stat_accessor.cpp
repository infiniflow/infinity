#include "gtest/gtest.h"

import base_test;
import stl;
import obj_stat_accessor;
import obj_status;

using namespace infinity;

class ObjectStatMapTest : public BaseTest {};

TEST_F(ObjectStatMapTest, test1) {
    SizeT disk_capacity_limit = 10;
    ObjectStatAccessor_ObjectStorage obj_map(disk_capacity_limit);

    ObjStat *obj_stat = obj_map.Get("key1");
    EXPECT_EQ(obj_stat, nullptr);

    Vector<String> drop_keys;
    obj_map.PutNew("key1", ObjStat(4, 1, 0), drop_keys);
    EXPECT_EQ(drop_keys.size(), 0);

    obj_map.PutNew("key2", ObjStat(4, 1, 0), drop_keys);
    EXPECT_EQ(drop_keys.size(), 0);

    ObjStat *stat1 = obj_map.Get("key1");
    EXPECT_EQ(stat1->obj_size_, 4);
    EXPECT_EQ(stat1->ref_count_, 1);
    obj_map.Release("key1", drop_keys);
    EXPECT_EQ(stat1->ref_count_, 0);
    EXPECT_EQ(drop_keys.size(), 0);

    obj_map.PutNew("key3", ObjStat(4, 1, 0), drop_keys);
    EXPECT_EQ(drop_keys.size(), 1);
    EXPECT_EQ(drop_keys[0], "key2");
    drop_keys.clear();
    EXPECT_EQ(obj_map.disk_used(), 8);

    ObjStat *stat2 = obj_map.GetNoCount("key2");
    EXPECT_NE(stat2, nullptr);
    EXPECT_FALSE(stat2->cached_);
    Optional<ObjStat> stat2_opt = obj_map.Invalidate("key2");
    EXPECT_TRUE(stat2_opt.has_value());
    stat2 = obj_map.GetNoCount("key2");
    EXPECT_EQ(stat2, nullptr);
}
