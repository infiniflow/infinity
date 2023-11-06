#pragma once

#include <analysis/analyzers.hpp>
#include <analysis/token_attributes.hpp>
#include <analysis/token_streams.hpp>
#include <index/index_writer.hpp>

namespace infinity {

struct IndexField {
    std::string_view name_;
    const irs::features_t features_;
    const irs::IndexFeatures index_features_;
    IndexField(const std::string_view &n, irs::IndexFeatures index_features, const irs::features_t &flags)
        : name_(n), features_(flags), index_features_(index_features) {}

    std::string_view Name() const noexcept { return name_; }

    const irs::features_t &Features() const noexcept { return features_; }

    irs::IndexFeatures IndexFeatures() const noexcept { return index_features_; }

    virtual irs::token_stream &GetTokens() const = 0;

    virtual bool Write(irs::data_output &out) const = 0;

    virtual ~IndexField() = default;
};

struct StringField : public IndexField {
    std::string f_;
    mutable irs::string_token_stream stream_;

    StringField(const std::string_view &n, irs::IndexFeatures index_features, const irs::features_t &flags) : IndexField(n, index_features, flags) {}

    StringField(const std::string_view &n, irs::IndexFeatures index_features, const irs::features_t &flags, const std::string &a)
        : IndexField(n, index_features, flags), f_(a) {}

    irs::token_stream &GetTokens() const override {
        stream_.reset(f_);
        return stream_;
    }

    bool Write(irs::data_output &out) const override {
        irs::write_string(out, f_.c_str(), f_.length());
        return true;
    }
};

struct TextField : public IndexField {
    std::string f_;
    mutable irs::analysis::analyzer::ptr stream_;

    TextField(const std::string_view &n, irs::IndexFeatures index_features, const irs::features_t &flags, irs::analysis::analyzer::ptr stream)
        : IndexField(n, index_features, flags), stream_(std::move(stream)) {}

    irs::token_stream &GetTokens() const override {
        stream_->reset(f_);
        return *stream_;
    }

    bool Write(irs::data_output &out) const override {
        irs::write_string(out, f_.c_str(), f_.length());
        return true;
    }
};

struct NumericField : public IndexField {
    mutable irs::numeric_token_stream stream_;
    int64_t value_;

    NumericField(const std::string_view &n, irs::IndexFeatures index_features, const irs::features_t &flags) : IndexField(n, index_features, flags) {}

    NumericField(const std::string_view &n, irs::IndexFeatures index_features, const irs::features_t &flags, uint64_t v)
        : IndexField(n, index_features, flags), value_(v) {}

    irs::token_stream &GetTokens() const override {
        stream_.reset(value_);
        return stream_;
    }

    bool Write(irs::data_output &out) const override {
        irs::write_zvlong(out, value_);
        return true;
    }
};

} // namespace infinity