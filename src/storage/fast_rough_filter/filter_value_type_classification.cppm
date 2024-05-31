//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module filter_value_type_classification;
import stl;
import internal_types;
import column_vector;

namespace infinity {

// for std::visit
export template <class... Fs>
struct Overload : Fs... {
    using Fs::operator()...;
};

// explicit deduction guide
export template <class... Fs>
Overload(Fs...) -> Overload<Fs...>;

export template <typename... Args>
struct TypeList;

template <typename L1, typename L2, typename... L>
struct AddTypeListT {
    using Type = typename AddTypeListT<L1, typename AddTypeListT<L2, L...>::Type>::Type;
};

template <typename... Arg1, typename... Arg2>
struct AddTypeListT<TypeList<Arg1...>, TypeList<Arg2...>> {
    using Type = TypeList<Arg1..., Arg2...>;
};

export template <typename... L>
using AddTypeList = typename AddTypeListT<L...>::Type;

template <typename T, typename U>
struct IsAnyOfListT;

template <typename T, typename... Args>
struct IsAnyOfListT<T, TypeList<Args...>> {
    static constexpr bool value = IsAnyOf<T, Args...>;
};

template <typename T, typename L>
concept IsAnyOfList = IsAnyOfListT<T, L>::value;

using TypeListIsVarchar = TypeList<VarcharT>;

using TypeListProbabilisticAndMinMaxFilterAcceptedTypesExceptVarchar =
    TypeList<TinyIntT, SmallIntT, IntegerT, BigIntT, HugeIntT, DateT, TimeT, DateTimeT, TimestampT>;

using TypeListOnlyMinMaxFilterAcceptedTypes = TypeList<FloatT, DoubleT>;

using TypeListOnlyProbabilisticFilterAcceptedTypes = TypeList<BooleanT, DecimalT>;

using TypeListProbabilisticAndMinMaxFilterAcceptedTypes =
    AddTypeList<TypeListProbabilisticAndMinMaxFilterAcceptedTypesExceptVarchar, TypeListIsVarchar>;

export using TypeListMinMaxFilterAcceptedTypes =
    AddTypeList<TypeListProbabilisticAndMinMaxFilterAcceptedTypes, TypeListOnlyMinMaxFilterAcceptedTypes>;

using typeListProbabilisticFilterAcceptedTypes =
    AddTypeList<TypeListProbabilisticAndMinMaxFilterAcceptedTypes, TypeListOnlyProbabilisticFilterAcceptedTypes>;

using TypeListMinMaxInnerValUnchanged =
    AddTypeList<TypeListProbabilisticAndMinMaxFilterAcceptedTypesExceptVarchar, TypeListOnlyMinMaxFilterAcceptedTypes>;

export template <typename T>
concept IsVarchar = IsAnyOfList<T, TypeListIsVarchar>;

export template <typename T>
concept CanBuildMinMaxFilterAndBloomFilter = IsAnyOfList<T, TypeListProbabilisticAndMinMaxFilterAcceptedTypes>;

export template <typename T>
concept CanBuildMinMaxFilter = IsAnyOfList<T, TypeListMinMaxFilterAcceptedTypes>;

export template <typename T>
concept CanOnlyBuildMinMaxFilter = IsAnyOfList<T, TypeListOnlyMinMaxFilterAcceptedTypes>;

export template <typename T>
concept CanBuildBloomFilter = IsAnyOfList<T, typeListProbabilisticFilterAcceptedTypes>;

export template <typename T>
concept CanOnlyBuildBloomFilter = IsAnyOfList<T, TypeListOnlyProbabilisticFilterAcceptedTypes>;

export template <typename T>
concept IsMinMaxInnerValUnchanged = IsAnyOfList<T, TypeListMinMaxInnerValUnchanged>;

export template <typename T>
concept IsMinMaxFilterAcceptedTypes = IsAnyOfList<T, TypeListMinMaxFilterAcceptedTypes>;

} // namespace infinity