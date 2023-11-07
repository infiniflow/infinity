module;

import stl;
import singleton;
import third_party;

export module iresearch;

namespace infinity {

export using AnalyzerFactory = StdFunction<iresearch_analyzer_t::ptr>;

export class AnalyzerPool : public Singleton<AnalyzerPool> {
public:
    using CacheType = flat_hash_map<StringView, iresearch_analyzer_t::ptr>;

    iresearch_analyzer_t::ptr Get(const String &name);

    void Set(const String &name, const String &args);

private:
    CacheType cache_;
};

export struct IndexField {
    StringView name_;
    const features_t features_;
    const IndexFeatures index_features_;
    IndexField(const StringView &n, IndexFeatures index_features, const features_t &flags)
        : name_(n), features_(flags), index_features_(index_features) {}

    StringView Name() const noexcept { return name_; }

    const features_t &Features() const noexcept { return features_; }

    IndexFeatures GetIndexFeatures() const noexcept { return index_features_; }

    virtual token_stream &GetTokens() const = 0;

    virtual bool Write(data_output &out) const = 0;

    virtual ~IndexField() = default;
};

export struct StringField : public IndexField {
    String f_;
    mutable string_token_stream stream_;

    StringField(const StringView &n, IndexFeatures index_features, const features_t &flags) : IndexField(n, index_features, flags) {}

    StringField(const StringView &n, IndexFeatures index_features, const features_t &flags, const String &a)
        : IndexField(n, index_features, flags), f_(a) {}

    token_stream &GetTokens() const override {
        stream_.reset(f_);
        return stream_;
    }

    bool Write(data_output &out) const override {
        write_string(out, f_.c_str(), f_.length());
        return true;
    }
};

export struct TextField : public IndexField {
    String f_;
    mutable iresearch_analyzer_t::ptr stream_;

    TextField(const StringView &n, IndexFeatures index_features, const features_t &flags, iresearch_analyzer_t::ptr stream)
        : IndexField(n, index_features, flags), stream_(Move(stream)) {}

    token_stream &GetTokens() const override {
        stream_->reset(f_);
        return *stream_;
    }

    bool Write(data_output &out) const override {
        write_string(out, f_.c_str(), f_.length());
        return true;
    }
};

export struct NumericField : public IndexField {
    mutable numeric_token_stream stream_;
    i64 value_;

    NumericField(const StringView &n, IndexFeatures index_features, const features_t &flags) : IndexField(n, index_features, flags) {}

    NumericField(const StringView &n, IndexFeatures index_features, const features_t &flags, u64 v)
        : IndexField(n, index_features, flags), value_(v) {}

    token_stream &GetTokens() const override {
        stream_.reset(value_);
        return stream_;
    }

    bool Write(data_output &out) const override {
        write_zvlong(out, value_);
        return true;
    }
};

} // namespace infinity