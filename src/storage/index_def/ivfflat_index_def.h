#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/logical_type.h"
#include "index_def.h"
#include "parser/statement/statement_common.h"

namespace infinity {
class IVFFlatIndexDef : public IndexDef {
public:
    static SharedPtr<IndexDef>
    Make(String index_name, IndexMethod method_type, Vector<String> column_names, const Vector<InitParameter *> &index_para_list);

    IVFFlatIndexDef(String index_name, IndexMethod method_type, Vector<String> column_names, size_t centroids_count, MetricType metric_type)
        : IndexDef(std::move(index_name), method_type, std::move(column_names)), centroids_count_(centroids_count), metric_type_(metric_type) {}

    ~IVFFlatIndexDef() = default;

    virtual bool operator==(const IndexDef &other) const override;

    virtual bool operator!=(const IndexDef &other) const override;

public:
    // Note shenyushi: whether the virtual function of derive class needs to has "virtual" keyword? And what is "final" keyword?
    virtual int32_t GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual String ToString() const override;

    virtual nlohmann::json Serialize() const override;

    static SharedPtr<IVFFlatIndexDef> Deserialize(const nlohmann::json &serialized);

public:
    const size_t centroids_count_{};

    const MetricType metric_type_{kInvalid};
};

} // namespace infinity