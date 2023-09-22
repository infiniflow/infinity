#pragma once

#include "common/types/internal_types.h"

#include <roaring/roaring.hh>
#include <vector>
#include <memory>

namespace infinity{

using Roaring = roaring::Roaring;


class BitSlicedIndex{
public:
	BitSlicedIndex(u64 schema_id, u64 table_id, u64 column_id);

	~BitSlicedIndex();	

	bool DoPut(u32 id, u64 val, u64& old_val);

	void DoRemove(u32 id, u64 val);

	bool DoGet(u32 id, u64& val);

	int DoRangeLT(uint64_t expect, bool allow_eq, std::unique_ptr<Roaring>& filter);

	int DoRangeGT(uint64_t expect, bool allow_eq, std::unique_ptr<Roaring>& filter);

	int DoRangeEQ(uint64_t expect, std::unique_ptr<Roaring>& filter);

	int DoRangeNEQ(uint64_t expect, std::unique_ptr<Roaring>& filter);

	int DoRangeBetween(uint64_t min, uint64_t max, std::unique_ptr<Roaring>& filter);

private:
	u64 schema_id_;
	u64 table_id_;
	u64 column_id_;
	std::vector<std::shared_ptr<Roaring>> bitmaps_;
	u8 bit_depth_ = 0;
};
}