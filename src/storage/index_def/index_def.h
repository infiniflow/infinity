#pragma once

#include "common/types/alias/containers.h"
#include "common/types/alias/strings.h"
#include "json.hpp"
namespace infinity {

enum class IndexMethod {
    kIVFFlat,
    kIVFSQ8,
    kHnsw,
    kInvalid,
};

enum class MetricType {
    kMerticInnerProduct,
    kMerticL2,
    kInvalid,
};

IndexMethod StringToIndexMethod(const String &str);

MetricType StringToMetricType(const String &str);

struct IndexDefCommon {
    String index_name_{};
    IndexMethod method_type_{IndexMethod::kInvalid};
    Vector<String> column_names_{};

    explicit IndexDefCommon() {}

    explicit IndexDefCommon(String index_name, IndexMethod method_type,
                            Vector<String> column_names)
        : index_name_(std::move(index_name)), method_type_(method_type),
          column_names_(std::move(column_names)) {}

    String ToString() const;

    void Serialize(nlohmann::json &res) const;

    static void Deserialize(const nlohmann::json &serialized,
                            IndexDefCommon &index_def_common);
};

class IndexDef {
  public:
    virtual ~IndexDef() = default;

    virtual String ToString() const = 0;

    virtual nlohmann::json Serialize() const = 0;

  public:
    [[nodiscard]] inline IndexMethod method_type() const {
        return common_.method_type_;
    }

    [[nodiscard]] inline const Vector<String> &column_names() const {
        return common_.column_names_;
    }

    [[nodiscard]] inline const String &index_name() const {
        return common_.index_name_;
    }

  protected:
    IndexDefCommon common_{};
};

} // namespace infinity