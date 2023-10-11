#include "ivfflat_index_def.h"

#include "common/utility/serializable.h"
#include "storage/index_def/index_def.h"

namespace infinity {

SharedPtr<IndexDef>
IVFFlatIndexDef::Make(String index_name, IndexMethod method_type, Vector<String> column_names, const Vector<InitParameter *> &index_para_list) {
    size_t centroids_count = 0;
    MetricType metric_type = MetricType::kInvalid;
    for (auto para : index_para_list) {
        if (para->para_name_ == "centroids_count") {
            centroids_count = std::stoi(para->para_value_);
        } else if (para->para_name_ == "metric") {
            metric_type = StringToMetricType(para->para_value_);
        }
    }
    if (centroids_count == 0 || metric_type == MetricType::kInvalid) {
        StorageException("Lack index parameters");
    }
    auto ptr = MakeShared<IVFFlatIndexDef>(std::move(index_name), method_type, std::move(column_names), centroids_count, metric_type);
    return std::static_pointer_cast<IndexDef>(ptr);
}

bool IVFFlatIndexDef::operator==(const IndexDef &other) const {
    try {
        auto other1 = dynamic_cast<const IVFFlatIndexDef &>(other);
        return IndexDef::operator==(other) && centroids_count_ == other1.centroids_count_ && metric_type_ == other1.metric_type_;
    } catch (std::bad_cast exception) {
        return false;
    }
}

bool IVFFlatIndexDef::operator!=(const IndexDef &other) const { return !(*this == other); }

int32_t IVFFlatIndexDef::GetSizeInBytes() const {
    size_t size = IndexDef::GetSizeInBytes();
    size += sizeof(int32_t) + sizeof(centroids_count_);
    size += sizeof(int32_t) + sizeof(metric_type_);
    return size;
}

void IVFFlatIndexDef::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, centroids_count_);
    WriteBufAdv(ptr, metric_type_);
}

String IVFFlatIndexDef::ToString() const {
    std::stringstream ss;
    ss << IndexDef::ToString() << ", " << centroids_count_ << ", " << MetricTypeToString(metric_type_);
    return ss.str();
}

nlohmann::json IVFFlatIndexDef::Serialize() const {
    nlohmann::json res = IndexDef::Serialize();
    res["centroids_count"] = centroids_count_;
    res["metric_type"] = MetricTypeToString(metric_type_);
    return res;
}

SharedPtr<IVFFlatIndexDef> IVFFlatIndexDef::Deserialize(const nlohmann::json &serialized) {
    // Hack to initialize with no parameter while default constructor is deleted
    auto ptr = new char[sizeof(IVFFlatIndexDef)];
    auto index_ptr = reinterpret_cast<IVFFlatIndexDef *>(ptr);
    index_ptr->InitBaseWithJson(serialized);
    // Hack to initialize const value after constructor
    const_cast<size_t &>(index_ptr->centroids_count_) = serialized["centroids_count_"];
    const_cast<MetricType &>(index_ptr->metric_type_) = serialized["metric_type_"];
    return std::shared_ptr<IVFFlatIndexDef>(index_ptr);
}

} // namespace infinity