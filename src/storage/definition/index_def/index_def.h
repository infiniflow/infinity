#pragma once

#include "common/types/alias/containers.h"
#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "json.hpp"
#include "storage/knnindex/knn_distance.h"

namespace infinity {

enum class IndexMethod {
    kIVFFlat,
    kIVFSQ8,
    kHnsw,
    kInvalid,
};

// TODO shenyushi: use definition in knn_exprs.h
enum class MetricType {
    kMerticInnerProduct,
    kMerticL2,
    kInvalid,
};

String IndexMethodToString(IndexMethod method);

String MetricTypeToString(MetricType metric_type);

IndexMethod StringToIndexMethod(const String &str);

MetricType StringToMetricType(const String &str);

class IndexDef {
protected:
    explicit IndexDef(SharedPtr<String> index_name, IndexMethod method_type, Vector<String> column_names)
        : index_name_(std::move(index_name)), method_type_(method_type), column_names_(std::move(column_names)){};

public:
    virtual ~IndexDef() = default;

    virtual bool operator==(const IndexDef &other) const;

    virtual bool operator!=(const IndexDef &other) const;

public:
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    virtual int32_t GetSizeInBytes() const;

    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const;

    // Read char from buffer
    static SharedPtr<IndexDef> ReadAdv(char *&ptr, int32_t maxbytes);

    virtual String ToString() const;

    virtual nlohmann::json Serialize() const;

    static SharedPtr<IndexDef> Deserialize(const nlohmann::json &index_def_json);

public:
    // TODO shenyushi, use shared_ptr here
    const SharedPtr<String> index_name_{};
    const IndexMethod method_type_{IndexMethod::kInvalid};
    const Vector<String> column_names_{};
};
} // namespace infinity