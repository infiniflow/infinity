//
// Created by jinhai on 23-8-26.
//


#pragma once

#include "executor/physical_operator.h"
#include "parser/statement/flush_statement.h"

#include <memory>

namespace infinity {

class PhysicalFlush final : public PhysicalOperator {
public:
    explicit
    PhysicalFlush(FlushType flush_type, u64 id)
        : PhysicalOperator(PhysicalOperatorType::kFlush, nullptr, nullptr, id),
        flush_type_(flush_type) { }

    ~PhysicalFlush() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    inline FlushType
    flush_type() const {
        return flush_type_;
    }

private:
    void
    FlushData(QueryContext* query_context);

    void
    FlushLog(QueryContext* query_context);

    void
    FlushBuffer(QueryContext* query_context);

private:
    FlushType flush_type_;

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

}
