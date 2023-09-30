//
// Created by jinhai on 23-5-19.
//

#pragma once



namespace infinity {

class IDAllocator {
public:
    explicit
    IDAllocator(u64 start_id = 0) : id_(start_id) {}

    inline u64
    AllocateNewId() {
        ++id_;
        return id_;
    }

    inline u64
    GetCurrentID() {
        return id_;
    }

    inline void
    SetID(u64 new_id) {
        id_ = new_id;
    }
private:
    std::atomic_uint64_t id_{};
};

}

