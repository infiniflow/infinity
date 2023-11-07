module;

import stl;
import singleton;
import iresearch_analyzer;
import third_party;

export module iresearch_document;

namespace infinity {

export struct IndexField {
    StringView name_;
    const Features features_;
    const IndexFeatures index_features_;
    IndexField(const StringView &n, IndexFeatures index_features, const Features &flags)
        : name_(n), features_(flags), index_features_(index_features) {}

    StringView Name() const noexcept { return name_; }

    const Features &GetFeatures() const noexcept { return features_; }

    IndexFeatures GetIndexFeatures() const noexcept { return index_features_; }

    virtual TokenStream &GetTokens() const = 0;

    virtual bool Write(DataOutput &out) const = 0;

    virtual ~IndexField() = default;
};

export struct StringField : public IndexField {
    String f_;
    mutable StringTokenStream stream_;

    StringField(const StringView &n, IndexFeatures index_features, const Features &flags) : IndexField(n, index_features, flags) {}

    StringField(const StringView &n, IndexFeatures index_features, const Features &flags, const String &a)
        : IndexField(n, index_features, flags), f_(a) {}

    TokenStream &GetTokens() const override {
        stream_.reset(f_);
        return stream_;
    }

    bool Write(DataOutput &out) const override {
        IResearchWriteString(out, f_.c_str(), f_.length());
        return true;
    }
};

export struct TextField : public IndexField {
    String f_;
    mutable IResearchAnalyzer::ptr stream_;

    TextField(const StringView &n, IndexFeatures index_features, const Features &flags, IResearchAnalyzer::ptr stream)
        : IndexField(n, index_features, flags), stream_(Move(stream)) {}

    TokenStream &GetTokens() const override {
        stream_->reset(f_);
        return *stream_;
    }

    bool Write(DataOutput &out) const override {
        IResearchWriteString(out, f_.c_str(), f_.length());
        return true;
    }
};

export struct NumericField : public IndexField {
    mutable NumericTokenStream stream_;
    i64 value_;

    NumericField(const StringView &n, IndexFeatures index_features, const Features &flags) : IndexField(n, index_features, flags) {}

    NumericField(const StringView &n, IndexFeatures index_features, const Features &flags, u64 v) : IndexField(n, index_features, flags), value_(v) {}

    TokenStream &GetTokens() const override {
        stream_.reset(value_);
        return stream_;
    }

    bool Write(DataOutput &out) const override {
        IResearchWriteZVlong(out, value_);
        return true;
    }
};

export struct FieldIterator {
    FieldIterator();
};

} // namespace infinity