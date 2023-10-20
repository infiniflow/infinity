module;

import stl;
import third_party;
import index_def;
import parser;

export module ivfflat_index_def;

namespace infinity {
export class IVFFlatIndexDef : public IndexDef {
  public:
    inline explicit IVFFlatIndexDef() {}

    static SharedPtr<IndexDef>
    Make(IndexDefCommon common, const Vector<InitParameter *> &index_para_list);

    ~IVFFlatIndexDef() = default;

    String ToString() const override;

    Json Serialize() const override;

    static SharedPtr<IVFFlatIndexDef>
    Deserialize(const Json &serialized);

  public:
    [[nodiscard]] inline SizeT centroids_count() const {
        return centroids_count_;
    }

    [[nodiscard]] inline MetricType metric_type() const { return metric_type_; }

  private:
    SizeT centroids_count_{};

    MetricType metric_type_{MetricType::kInvalid};
};

} // namespace infinity