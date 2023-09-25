#pragma once

#include <cstdint>
#include <vector>

namespace infinity {

struct PostingValue {
    virtual ~PostingValue(){}
    virtual size_t GetSize() const = 0;
    uint32_t location_;
    uint32_t offset_;
};

template<typename T>
struct TypedPostingValue : public PostingValue {
    size_t GetSize() const override {
        return sizeof(T);
    }
};

struct PostingValues {
    ~PostingValues() {
        for(size_t i = 0 ; i < values_.size() ; ++i) {
            delete values_[i];
        }
    }

    PostingValue* GetValue(size_t index) const {
        return values_[index];
    }

    size_t GetSize() const {
        return values_.size();
    }

    size_t GetTotalSize() const {
        return values_.size() * values_[0]->GetSize();
    }

    void AddValue(PostingValue* value) {
        values_.push_back(value);
    }

    std::vector<PostingValue*> values_;
};

}

