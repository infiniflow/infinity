module;

import stl;

export module dist_func;

namespace infinity {

export template <typename DataType>
using DistFunc = DataType(*)(const DataType *, const DataType *, SizeT);

export template <typename DataType>
class SpaceBase {
public:
    virtual DistFunc<DataType> DistFuncPtr() const = 0;

    virtual ~SpaceBase() = default;
};

} // namespace infinity
