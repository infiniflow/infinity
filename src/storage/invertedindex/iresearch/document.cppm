// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import singleton;
import iresearch_analyzer;
import third_party;

export module iresearch_document;

namespace infinity {

export struct IndexField {
    String name_;
    const Features features_;
    const IndexFeatures index_features_;
    IndexField(const String &name, IndexFeatures index_features, const Features &flags)
        : name_(name), features_(flags), index_features_(index_features) {}

    std::string_view name() const noexcept { return name_.c_str(); }

    const Features &features() const noexcept { return features_; }

    IndexFeatures index_features() const noexcept { return index_features_; }

    virtual TokenStream &get_tokens() const = 0;

    virtual bool write(DataOutput &out) const = 0;

    virtual ~IndexField() = default;
};

export struct StringField : public IndexField {
    String f_;
    mutable StringTokenStream stream_;

    StringField(const String &name, IndexFeatures index_features, const Features &flags) : IndexField(name, index_features, flags) {}

    StringField(const String &name, IndexFeatures index_features, const Features &flags, const String &a)
        : IndexField(name, index_features, flags), f_(a) {}

    TokenStream &get_tokens() const override {
        stream_.reset(f_);
        return stream_;
    }

    bool write(DataOutput &out) const override {
        IRSWriteString(out, f_.c_str(), f_.length());
        return true;
    }
};

export struct TextField : public IndexField {
    String f_;
    mutable IRSAnalyzer *stream_;

    TextField(const String &name, IndexFeatures index_features, const Features &flags, IRSAnalyzer *stream)
        : IndexField(name, index_features, flags), stream_(std::move(stream)) {}

    TokenStream &get_tokens() const override {
        stream_->reset(f_);
        return *stream_;
    }

    bool write(DataOutput &out) const override {
        IRSWriteString(out, f_.c_str(), f_.length());
        return true;
    }
};

export template <typename T>
struct NumericField : public IndexField {
    mutable NumericTokenStream stream_;
    T value_;

    NumericField(const String &name, IndexFeatures index_features, const Features &flags) : IndexField(name, index_features, flags) {}

    NumericField(const String &name, IndexFeatures index_features, const Features &flags, u64 v)
        : IndexField(name, index_features, flags), value_(v) {}

    TokenStream &get_tokens() const override {
        stream_.reset(value_);
        return stream_;
    }

    bool write(DataOutput &out) const override {
        IRSWriteZVlong(out, value_);
        return true;
    }
};

} // namespace infinity