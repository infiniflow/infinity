//
// Created by jinhai on 23-3-3.
//

#pragma once

#include "common/types/alias/containers.h"
#include "extra_ddl_info.h"

namespace infinity {

struct CreateIndexPara {
    String para_name_{};
    String para_value_{};
};

enum class IndexMethod {
    kFlat,
    kIVFFlat,
    kIVFSQ8,
    kHnsw,
    kInvalid,
};

inline String IndexMethodToString(IndexMethod method) {
    switch (method) {
        case IndexMethod::kFlat:
            return "FLAT";
        case IndexMethod::kIVFFlat:
            return "IVF_FLAT";
        case IndexMethod::kIVFSQ8:
            return "IVF_SQ8";
        case IndexMethod::kHnsw:
            return "HNSW";
        default:
            return "INVALID";
    }
}

class CreateIndexInfo final : public ExtraDDLInfo {
public:
    explicit CreateIndexInfo() : ExtraDDLInfo(DDLType::kIndex) {}

    ~CreateIndexInfo() final;

    [[nodiscard]] String ToString() const final;

    String index_name_{};
    String schema_name_{"default"};
    String table_name_{};
    IndexMethod method_type_{IndexMethod::kInvalid};

    Vector<String> *column_names_{nullptr};
    Vector<CreateIndexPara *> *index_para_list_{nullptr};
};

} // namespace infinity
