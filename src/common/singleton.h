//
// Created by JinHai on 2022/7/19.
//

#pragma once

namespace infinity {

template<typename T>
class Singleton {
public:
    inline static T& instance() {
        static T instance;
        return instance;
    }

    virtual ~Singleton() {}
};

}
