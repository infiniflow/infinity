#include "key_encoder.h"

#include <unordered_map>

namespace infinity {
template <typename KeyEncoderTraits>
KeyEncoder::KeyEncoder(KeyEncoderTraits traits)
    : encode_func_(traits.Encode){}

struct FastHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

class KeyEncoderIntializer{
public:
    ~KeyEncoderIntializer() {
        for(auto iter : encoders_) {
            delete iter.second;
        }
    }

	static KeyEncoderIntializer * GetInstance() {
        static KeyEncoderIntializer instance;
        return &instance;
	}

	KeyEncoder* GetKeyEncoder(LogicalType type) const {
        auto it = encoders_.find(type);
        if(it != encoders_.end()) {
            return it->second;
        }
        return nullptr;
    }
private:
    KeyEncoderIntializer() {
    	AddEntry<LogicalType::kBoolean>();
    	AddEntry<LogicalType::kTinyInt>();
    	AddEntry<LogicalType::kInteger>();
    	AddEntry<LogicalType::kBigInt>();
    	AddEntry<LogicalType::kHugeInt>();
    	AddEntry<LogicalType::kDecimal>();
    	AddEntry<LogicalType::kVarchar>();
    	AddEntry<LogicalType::kDate>();
    	AddEntry<LogicalType::kTime>();
    	AddEntry<LogicalType::kDateTime>();
    	AddEntry<LogicalType::kTimestamp>();
    }

    template<LogicalType data_type>
    void AddEntry() {
    	encoders_.emplace(data_type, new KeyEncoder(KeyEncoderTraits<data_type>()));
    }

	std::unordered_map<LogicalType, KeyEncoder*, FastHash> encoders_;
};

}