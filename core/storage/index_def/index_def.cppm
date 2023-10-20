//
// Created by jinhai on 23-10-17.
//
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

export enum class MetricType {
    kMerticInnerProduct,
    kMerticL2,
    kInvalid,
};

IndexMethod StringToIndexMethod(const String &str);

export MetricType StringToMetricType(const String &str);

export struct IndexDefCommon {
    String index_name_{};
    IndexMethod method_type_{IndexMethod::kInvalid};
    Vector<String> column_names_{};

    explicit IndexDefCommon() {}

    explicit IndexDefCommon(String index_name, IndexMethod method_type,
                            Vector<String> column_names)
            : index_name_(Move(index_name)), method_type_(method_type),
              column_names_(Move(column_names)) {}

    String ToString() const;

    void Serialize(Json &res) const;

    static void Deserialize(const Json &serialized,
                            IndexDefCommon &index_def_common);
};

export class IndexDef {
public:
    inline IndexDef() = default;

    virtual ~IndexDef() = default;

    virtual String ToString() const = 0;

    virtual Json Serialize() const = 0;

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
