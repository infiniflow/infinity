
module;

import stl;
import third_party;

export module index_def;

namespace infinity {

export enum class IndexMethod {
    kIVFFlat,
    kIVFSQ8,
    kHnsw,
    kInvalid,
};

// TODO shenyushi: use definition in knn_exprs.h
export enum class MetricType {
    kMerticInnerProduct,
    kMerticL2,
    kInvalid,
};

export String IndexMethodToString(IndexMethod method);

export String MetricTypeToString(MetricType metric_type);

IndexMethod StringToIndexMethod(const String &str);

export MetricType StringToMetricType(const String &str);

export class IndexDef {
protected:
    explicit IndexDef(SharedPtr<String> index_name, IndexMethod method_type, Vector<String> column_names)
        : index_name_(Move(index_name)), method_type_(method_type), column_names_(Move(column_names)){};

public:
    virtual ~IndexDef() = default;

    virtual bool operator==(const IndexDef &other) const;

    virtual bool operator!=(const IndexDef &other) const;

public:
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    virtual i32 GetSizeInBytes() const;

    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const;

    // Read char from buffer
    static SharedPtr<IndexDef> ReadAdv(char *&ptr, i32 maxbytes);

    virtual String ToString() const;

    virtual Json Serialize() const;

    static SharedPtr<IndexDef> Deserialize(const Json &index_def_json);

public:
    SharedPtr<String> index_name_{};
    const IndexMethod method_type_{IndexMethod::kInvalid};
    const Vector<String> column_names_{};
};
} // namespace infinity