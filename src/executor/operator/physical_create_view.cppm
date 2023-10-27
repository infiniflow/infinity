//
// Created by JinHai on 2022/7/28.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import index_def;

export module physical_create_view;

namespace infinity {

export class PhysicalCreateView final : public PhysicalOperator {
public:
    explicit inline PhysicalCreateView(u64 id,
                                       SharedPtr<Vector<String>> names_ptr,
                                       SharedPtr<Vector<SharedPtr<DataType>>> types_ptr,
                                       SharedPtr<CreateViewInfo> create_view_info)
        : PhysicalOperator(PhysicalOperatorType::kCreateView, nullptr, nullptr, id), output_names_(Move(names_ptr)), output_types_(Move(types_ptr)),
          create_view_info_(Move(create_view_info)) {}

    ~PhysicalCreateView() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline const SharedPtr<CreateViewInfo> &bound_select_statement() const { return create_view_info_; };

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline const SharedPtr<CreateViewInfo> &create_view_info() const { return create_view_info_; }

private:
    SharedPtr<CreateViewInfo> create_view_info_{nullptr};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
