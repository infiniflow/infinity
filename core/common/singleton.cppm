//
// Created by jinhai on 23-10-14.
//
export module singleton;

namespace infinity {

export template <typename T>
class Singleton {
public:
    inline static T &instance() {
        static T instance;
        return instance;
    }

    virtual ~Singleton() = default;
};

} // namespace infinity
