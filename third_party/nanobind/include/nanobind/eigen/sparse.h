/*
    nanobind/eigen/sparse.h: type casters for sparse Eigen matrices

    Copyright (c) 2023 Henri Menke and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/ndarray.h>
#include <nanobind/eigen/dense.h>
#include <Eigen/SparseCore>

#include <memory>
#include <type_traits>
#include <utility>

NAMESPACE_BEGIN(NB_NAMESPACE)

NAMESPACE_BEGIN(detail)

/// Detect Eigen::SparseMatrix
template <typename T> constexpr bool is_eigen_sparse_matrix_v =
    is_eigen_sparse_v<T> &&
    !std::is_base_of_v<Eigen::SparseMapBase<T, Eigen::ReadOnlyAccessors>, T>;


/// Caster for Eigen::SparseMatrix
template <typename T> struct type_caster<T, enable_if_t<is_eigen_sparse_matrix_v<T>>> {
    using Scalar = typename T::Scalar;
    using StorageIndex = typename T::StorageIndex;
    using Index = typename T::Index;
    using SparseMap = Eigen::Map<T>;

    static_assert(std::is_same_v<T, Eigen::SparseMatrix<Scalar, T::Options, StorageIndex>>,
                  "nanobind: Eigen sparse caster only implemented for matrices");

    static constexpr bool RowMajor = T::IsRowMajor;

    using ScalarNDArray = ndarray<numpy, Scalar, shape<-1>>;
    using StorageIndexNDArray = ndarray<numpy, StorageIndex, shape<-1>>;

    using ScalarCaster = make_caster<ScalarNDArray>;
    using StorageIndexCaster = make_caster<StorageIndexNDArray>;

    NB_TYPE_CASTER(T, const_name<RowMajor>("scipy.sparse.csr_matrix[",
                                           "scipy.sparse.csc_matrix[")
                   + make_caster<Scalar>::Name + const_name("]"))

    ScalarCaster data_caster;
    StorageIndexCaster indices_caster, indptr_caster;

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept {
        object obj = borrow(src);
        try {
            object matrix_type = module_::import_("scipy.sparse").attr(RowMajor ? "csr_matrix" : "csc_matrix");
            if (!obj.type().is(matrix_type))
                obj = matrix_type(obj);
        } catch (const python_error &) {
            return false;
        }

        if (object data_o = obj.attr("data"); !data_caster.from_python(data_o, flags, cleanup))
            return false;
        ScalarNDArray& values = data_caster.value;

        if (object indices_o = obj.attr("indices"); !indices_caster.from_python(indices_o, flags, cleanup))
            return false;
        StorageIndexNDArray& inner_indices = indices_caster.value;

        if (object indptr_o = obj.attr("indptr"); !indptr_caster.from_python(indptr_o, flags, cleanup))
            return false;
        StorageIndexNDArray& outer_indices = indptr_caster.value;

        object shape_o = obj.attr("shape"), nnz_o = obj.attr("nnz");
        Index rows, cols, nnz;
        try {
            if (len(shape_o) != 2)
                return false;
            rows = cast<Index>(shape_o[0]);
            cols = cast<Index>(shape_o[1]);
            nnz = cast<Index>(nnz_o);
        } catch (const python_error &) {
            return false;
        }

        value = SparseMap(rows, cols, nnz, outer_indices.data(), inner_indices.data(), values.data());

        return true;
    }

    static handle from_cpp(T &&v, rv_policy policy, cleanup_list *cleanup) noexcept {
        if (policy == rv_policy::automatic ||
            policy == rv_policy::automatic_reference)
            policy = rv_policy::move;

        return from_cpp((const T &) v, policy, cleanup);
    }

    static handle from_cpp(const T &v, rv_policy policy, cleanup_list *) noexcept {
        if (!v.isCompressed()) {
            PyErr_SetString(PyExc_ValueError,
                            "nanobind: unable to return an Eigen sparse matrix that is not in a compressed format. "
                            "Please call `.makeCompressed()` before returning the value on the C++ end.");
            return handle();
        }

        object matrix_type;
        try {
            matrix_type = module_::import_("scipy.sparse").attr(RowMajor ? "csr_matrix" : "csc_matrix");
        } catch (python_error &e) {
            e.restore();
            return handle();
        }

        const Index rows = v.rows(), cols = v.cols();
        const size_t data_shape[] = { (size_t) v.nonZeros() };
        const size_t outer_indices_shape[] = { (size_t) ((RowMajor ? rows : cols) + 1) };

        T *src = std::addressof(const_cast<T &>(v));
        object owner;
        if (policy == rv_policy::move) {
            src = new T(std::move(v));
            owner = capsule(src, [](void *p) noexcept { delete (T *) p; });
        }

        ScalarNDArray data(src->valuePtr(), 1, data_shape, owner);
        StorageIndexNDArray outer_indices(src->outerIndexPtr(), 1, outer_indices_shape, owner);
        StorageIndexNDArray inner_indices(src->innerIndexPtr(), 1, data_shape, owner);

        try {
            return matrix_type(nanobind::make_tuple(
                                   std::move(data), std::move(inner_indices), std::move(outer_indices)),
                               nanobind::make_tuple(rows, cols))
                .release();
        } catch (python_error &e) {
            e.restore();
            return handle();
        }
    }
};


/// Caster for Eigen::Map<Eigen::SparseMatrix>, still needs to be implemented.
template <typename T>
struct type_caster<Eigen::Map<T>, enable_if_t<is_eigen_sparse_matrix_v<T>>> {
    using Map = Eigen::Map<T>;
    using SparseMatrixCaster = type_caster<T>;
    static constexpr auto Name = SparseMatrixCaster::Name;
    template <typename T_> using Cast = Map;
    template <typename T_> static constexpr bool can_cast() { return true; }

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept = delete;

    static handle from_cpp(const Map &v, rv_policy policy, cleanup_list *cleanup) noexcept = delete;
};


/// Caster for Eigen::Ref<Eigen::SparseMatrix>, still needs to be implemented
template <typename T, int Options>
struct type_caster<Eigen::Ref<T, Options>, enable_if_t<is_eigen_sparse_matrix_v<T>>> {
    using Ref = Eigen::Ref<T, Options>;
    using Map = Eigen::Map<T, Options>;
    using MapCaster = make_caster<Map>;
    static constexpr auto Name = MapCaster::Name;
    template <typename T_> using Cast = Ref;
    template <typename T_> static constexpr bool can_cast() { return true; }

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept = delete;

    static handle from_cpp(const Ref &v, rv_policy policy, cleanup_list *cleanup) noexcept = delete;
};

NAMESPACE_END(detail)

NAMESPACE_END(NB_NAMESPACE)
