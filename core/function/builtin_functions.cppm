//
// Created by JinHai on 2022/9/14.
//

module;

import stl;

export module builtin_functions;

namespace infinity {

class NewCatalog;

export class BuiltinFunctions : public EnableSharedFromThis<BuiltinFunctions> {
public:
    explicit BuiltinFunctions(UniquePtr<NewCatalog> &catalog_ptr);

    void Init();

private:
    UniquePtr<NewCatalog> &catalog_ptr_;

private:
    void RegisterAggregateFunction();

    void RegisterScalarFunction();

    void RegisterTableFunction();
};

} // namespace infinity
