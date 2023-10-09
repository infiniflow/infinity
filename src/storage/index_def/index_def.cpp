#include "index_def.h"
#include <sstream>

namespace infinity {

namespace {

String IndexMethodToString(IndexMethod method) {
    switch (method) {
    case IndexMethod::kIVFFlat: {
        return "IVFFlat";
    }
    case IndexMethod::kIVFSQ8: {
        return "IVFSQ8";
    }
    case IndexMethod::kHnsw: {
        return "HNSW";
    }
    case IndexMethod::kInvalid: {
        return "Invalid";
    }
    }
}

String MetricTypeToString(MetricType metric_type) {
    switch (metric_type) {
    case MetricType::kMerticInnerProduct: {
        return "ip";
    }
    case MetricType::kMerticL2: {
        return "l2";
    }
    case MetricType::kInvalid: {
        return "Invalid";
    }
    }
}

} // namespace

IndexMethod StringToIndexMethod(const String &str) {
    if (str == "IVFFlat") {
        return IndexMethod::kIVFFlat;
    } else if (str == "IVFSQ8") {
        return IndexMethod::kIVFSQ8;
    } else if (str == "HNSW") {
        return IndexMethod::kHnsw;
    } else {
        return IndexMethod::kInvalid;
    }
}

MetricType StringToMetricType(const String &str) {
    if (str == "ip") {
        return MetricType::kMerticInnerProduct;
    } else if (str == "l2") {
        return MetricType::kMerticL2;
    } else {
        return MetricType::kInvalid;
    }
}

String IndexDefCommon::ToString() const {
    std::stringstream ss;
    ss << "IndexDefCommon(" << index_name_ << ", "
       << IndexMethodToString(method_type_) << ", [";
    for (size_t i = 0; i < column_names_.size(); ++i) {
        ss << column_names_[i];
        if (i != column_names_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "])";
    return ss.str();
}

void IndexDefCommon::Serialize(nlohmann::json &res) const {
    res["index_name"] = index_name_;
    res["method_type"] = IndexMethodToString(method_type_);
    res["column_names"] = column_names_;
}

void IndexDefCommon::Deserialize(const nlohmann::json &serialized,
                                 IndexDefCommon &index_def_common) {
    index_def_common.index_name_ = serialized["index_name"];
    index_def_common.method_type_ = serialized["method_type"];
    index_def_common.column_names_ = serialized["column_names"];
}

} // namespace infinity