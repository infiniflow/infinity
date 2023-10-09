#include "ivfflat_index_def.h"
#include "common/utility/exception.h"
#include "storage/index_def/index_def.h"
#include <memory>

namespace infinity {
SharedPtr<IndexDef>
IVFFlatIndexDef::Make(IndexDefCommon common,
                      const Vector<InitParameter *> &index_para_list) {
    auto ptr = MakeShared<IVFFlatIndexDef>();

    ptr->common_ = std::move(common);
    for (auto para : index_para_list) {
        if (para->para_name_ == "centroids_count") {
            ptr->centroids_count_ = std::stoi(para->para_value_);
        } else if (para->para_name_ == "metric") {
            ptr->metric_type_ = StringToMetricType(para->para_value_);
        }
    }
    if (ptr->centroids_count_ == 0 ||
        ptr->metric_type_ == MetricType::kInvalid) {
        StorageException("Invalid index parameters");
    }
    return std::static_pointer_cast<IndexDef>(ptr);
}

String IVFFlatIndexDef::ToString() const {
    std::stringstream ss;
    ss << "IVFFlatIndexDef(" << common_.ToString()
       << ", centroids_cnt = " << centroids_count_ << ")";
    return ss.str();
}

nlohmann::json IVFFlatIndexDef::Serialize() const {
    nlohmann::json res;
    common_.Serialize(res);
    res["centroids_count"] = centroids_count_;
    return res;
}

SharedPtr<IVFFlatIndexDef>
IVFFlatIndexDef::Deserialize(const nlohmann::json &serialized) {
    auto res = MakeShared<IVFFlatIndexDef>();
    IndexDefCommon::Deserialize(serialized, res->common_);
    res->centroids_count_ = serialized["centroids_count"];
    return res;
}
} // namespace infinity