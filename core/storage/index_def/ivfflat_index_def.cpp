module;

#include <memory>

import stl;
import index_def;
import parser;
import third_party;
import infinity_assert;
import infinity_exception;

module ivfflat_index_def;

namespace infinity {
SharedPtr<IndexDef> IVFFlatIndexDef::Make(IndexDefCommon common, const Vector<InitParameter *> &index_para_list) {
    auto ptr = MakeShared<IVFFlatIndexDef>();

    ptr->common_ = Move(common);
    SizeT para_count = index_para_list.size();
    for(SizeT idx = 0; idx < para_count; ++ idx) {
        InitParameter* para = index_para_list[idx];
        if (IsEqual(para->para_name_, "centroids_count")) {
            ptr->centroids_count_ = StrToInt(para->para_value_);
        } else if (IsEqual(para->para_name_, "metric")) {
            ptr->metric_type_ = StringToMetricType(para->para_value_);
        }
    }
    if (ptr->centroids_count_ == 0 || ptr->metric_type_ == MetricType::kInvalid) {
        Error<StorageException>("Invalid index parameters", __FILE_NAME__, __LINE__);
    }
    return std::static_pointer_cast<IndexDef>(ptr);
}

String IVFFlatIndexDef::ToString() const {
//    std::stringstream ss;
//    ss << "IVFFlatIndexDef(" << common_.ToString() << ", centroids_cnt = " << centroids_count_ << ")";
//    return ss.str();
}

Json IVFFlatIndexDef::Serialize() const {
    Json res;
    common_.Serialize(res);
    res["centroids_count"] = centroids_count_;
    return res;
}

SharedPtr<IVFFlatIndexDef> IVFFlatIndexDef::Deserialize(const Json &serialized) {
    auto res = MakeShared<IVFFlatIndexDef>();
    IndexDefCommon::Deserialize(serialized, res->common_);
    res->centroids_count_ = serialized["centroids_count"];
    return res;
}
} // namespace infinity