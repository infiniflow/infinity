#include "index_def.h"

#include "common/utility/infinity_assert.h"
#include "common/utility/serializable.h"
#include "storage/index_def/ivfflat_index_def.h"

#include <cstdint>
#include <sstream>

namespace infinity {
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
} // namespace infinity

//--------------------------------------------------

namespace infinity {

bool IndexDef::operator==(const IndexDef &other) const {
    return index_name_ == other.index_name_ && method_type_ == other.method_type_ && column_names_ == other.column_names_;
}

bool IndexDef::operator!=(const IndexDef &other) const { return !(*this == other); }

int32_t IndexDef::GetSizeInBytes() const {
    int32_t size = 0;
    size += sizeof(int32_t) + index_name_->length();
    size += sizeof(int32_t) + sizeof(method_type_);
    size += sizeof(int32_t);
    for (const String &column_name : column_names_) {
        size += column_name.length(); // Note shenyushi: whether to add sizeof(int32_t) ?
    }
    return size;
}

void IndexDef::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, index_name_);
    WriteBufAdv(ptr, method_type_);
    for (const String &column_name : column_names_) {
        WriteBufAdv(ptr, column_name);
    }
}

String IndexDef::ToString() const {
    std::stringstream ss;
    ss << "IndexDef(" << index_name_ << ", " << IndexMethodToString(method_type_) << ", [";
    for (size_t i = 0; i < column_names_.size(); ++i) {
        ss << column_names_[i];
        if (i != column_names_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "])";
    return ss.str();
}

nlohmann::json IndexDef::Serialize() const {
    nlohmann::json res;
    res["index_name"] = *index_name_;
    res["method_type"] = IndexMethodToString(method_type_);
    res["column_names"] = column_names_;
    return res;
}

SharedPtr<IndexDef> IndexDef::Deserialize(const nlohmann::json &index_def_json) {
    IndexMethod method_type = index_def_json["method_type"];
    switch (method_type) {
        case IndexMethod::kIVFFlat: {
            auto ptr = IVFFlatIndexDef::Deserialize(index_def_json);
            return std::static_pointer_cast<IndexDef>(ptr);
        }
        case IndexMethod::kInvalid: {
            StorageError("Error index method while deserializing");
        }
        default:
            NotImplementError("Not implemented");
    }
}

void IndexDef::InitBaseWithJson(const nlohmann::json &index_def_json) {
    const_cast<SharedPtr<String> &>(index_name_) = MakeShared<String>(index_def_json["index_name"]);
    const_cast<IndexMethod &>(method_type_) = index_def_json["method_type"];
    const_cast<Vector<String> &>(column_names_) = index_def_json["column_names"];
}

} // namespace infinity