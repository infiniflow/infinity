#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/logical_type.h"
#include "index_def.h"
#include "parser/statement/statement_common.h"

namespace infinity {
class IVFFlatIndexDef : public IndexDef {
  public:
    explicit IVFFlatIndexDef() {}

    static SharedPtr<IndexDef>
    Make(IndexDefCommon common, const Vector<InitParameter *> &index_para_list);

    ~IVFFlatIndexDef() = default;

    String ToString() const override;

    nlohmann::json Serialize() const override;

    static SharedPtr<IVFFlatIndexDef>
    Deserialize(const nlohmann::json &serialized);

  public:
    [[nodiscard]] inline size_t centroids_count() const {
        return centroids_count_;
    }

    [[nodiscard]] inline MetricType metric_type() const { return metric_type_; }

  private:
    size_t centroids_count_{};

    MetricType metric_type_{kInvalid};
};

} // namespace infinity