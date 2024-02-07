module;

export module column_inverter;
import stl;
import column_vector;
import internal_types;

namespace infinity {

class RefCount {
    std::mutex lock_;
    std::condition_variable cv_;
    u32 ref_count_;

public:
    RefCount();
    virtual ~RefCount();
    void Retain() noexcept;
    void Release() noexcept;
    void WaitForZeroRefCount();
    bool ZeroRefCount();
};

export struct ColumnCommitter {

    void Retain() { ref_count_.Retain(); }

    void Release() { ref_count_.Release(); }

    bool ZeroRefCount() { return ref_count_.ZeroRefCount(); }

    void WaitForZeroRefCount() { return ref_count_.WaitForZeroRefCount(); }

    RefCount &GetRefCount() { return ref_count_; }

    virtual void Commit() = 0;

    RefCount ref_count_;
};

export class ColumnInverter {
public:
    virtual void InvertColumn(SharedPtr<ColumnVector> column_vector, RowID start_row_id) = 0;

    virtual void InvertColumn(u32 doc_id, const String &val) = 0;

    virtual void Dump() = 0;
};
} // namespace infinity