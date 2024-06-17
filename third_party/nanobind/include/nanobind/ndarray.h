/*
    nanobind/ndarray.h: functionality to exchange n-dimensional arrays with
    other array programming frameworks (NumPy, PyTorch, etc.)

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.

    The API below is based on the DLPack project
    (https://github.com/dmlc/dlpack/blob/main/include/dlpack/dlpack.h)
*/
#pragma once

#include <nanobind/nanobind.h>
#include <initializer_list>

NAMESPACE_BEGIN(NB_NAMESPACE)

NAMESPACE_BEGIN(device)
#define NB_DEVICE(enum_name, enum_value)                              \
    struct enum_name {                                                \
        static constexpr auto name = detail::const_name(#enum_name);  \
        static constexpr int32_t value = enum_value;                  \
        static constexpr bool is_device = true;                       \
    }
NB_DEVICE(none, 0); NB_DEVICE(cpu, 1); NB_DEVICE(cuda, 2);
NB_DEVICE(cuda_host, 3); NB_DEVICE(opencl, 4); NB_DEVICE(vulkan, 7);
NB_DEVICE(metal, 8); NB_DEVICE(rocm, 10); NB_DEVICE(rocm_host, 11);
NB_DEVICE(cuda_managed, 13); NB_DEVICE(oneapi, 14);
#undef NB_DEVICE
NAMESPACE_END(device)

NAMESPACE_BEGIN(dlpack)

enum class dtype_code : uint8_t {
    Int = 0, UInt = 1, Float = 2, Bfloat = 4, Complex = 5, Bool = 6
};

struct device {
    int32_t device_type = 0;
    int32_t device_id = 0;
};

struct dtype {
    uint8_t code = 0;
    uint8_t bits = 0;
    uint16_t lanes = 0;

    bool operator==(const dtype &o) const {
        return code == o.code && bits == o.bits && lanes == o.lanes;
    }
    bool operator!=(const dtype &o) const { return !operator==(o); }
};

struct dltensor {
    void *data = nullptr;
    nanobind::dlpack::device device;
    int32_t ndim = 0;
    nanobind::dlpack::dtype dtype;
    int64_t *shape = nullptr;
    int64_t *strides = nullptr;
    uint64_t byte_offset = 0;
};

NAMESPACE_END(dlpack)

template <ssize_t... Is> struct shape {
    static_assert(
        ((Is >= 0 || Is == -1) && ...),
        "The arguments to nanobind::shape must either be positive or equal to -1"
    );
    static constexpr size_t size = sizeof...(Is);
};

struct c_contig { };
struct f_contig { };
struct any_contig { };
struct numpy { };
struct tensorflow { };
struct pytorch { };
struct jax { };
struct cupy { };
struct ro { };

template <typename T> struct ndarray_traits {
    static constexpr bool is_complex = detail::is_complex_v<T>;
    static constexpr bool is_float   = std::is_floating_point_v<T>;
    static constexpr bool is_bool    = std::is_same_v<std::remove_cv_t<T>, bool>;
    static constexpr bool is_int     = std::is_integral_v<T> && !is_bool;
    static constexpr bool is_signed  = std::is_signed_v<T>;
};

NAMESPACE_BEGIN(detail)

template <typename T>
constexpr bool is_ndarray_scalar_v =
    ndarray_traits<T>::is_float || ndarray_traits<T>::is_int ||
    ndarray_traits<T>::is_bool || ndarray_traits<T>::is_complex;

template <typename> struct ndim_shape;
template <size_t... S> struct ndim_shape<std::index_sequence<S...>> {
    using type = shape<((void) S, -1)...>;
};

NAMESPACE_END(detail)

template <size_t N> using ndim = typename detail::ndim_shape<std::make_index_sequence<N>>::type;

template <typename T> constexpr dlpack::dtype dtype() {
    static_assert(
        detail::is_ndarray_scalar_v<T>,
        "nanobind::dtype<T>: T must be a floating point or integer variable!"
    );

    dlpack::dtype result;

    if constexpr (ndarray_traits<T>::is_float)
        result.code = (uint8_t) dlpack::dtype_code::Float;
    else if constexpr (ndarray_traits<T>::is_complex)
        result.code = (uint8_t) dlpack::dtype_code::Complex;
    else if constexpr (ndarray_traits<T>::is_signed)
        result.code = (uint8_t) dlpack::dtype_code::Int;
    else if constexpr (std::is_same_v<std::remove_cv_t<T>, bool>)
        result.code = (uint8_t) dlpack::dtype_code::Bool;
    else
        result.code = (uint8_t) dlpack::dtype_code::UInt;

    result.bits = sizeof(T) * 8;
    result.lanes = 1;

    return result;
}


NAMESPACE_BEGIN(detail)

enum class ndarray_framework : int { none, numpy, tensorflow, pytorch, jax, cupy };

struct ndarray_req {
    dlpack::dtype dtype;
    uint32_t ndim = 0;
    size_t *shape = nullptr;
    bool req_shape = false;
    bool req_dtype = false;
    bool req_ro = false;
    char req_order = '\0';
    uint8_t req_device = 0;
};

template <typename T, typename = int> struct ndarray_arg {
    static constexpr size_t size = 0;
    static constexpr auto name = descr<0>{ };
    static void apply(ndarray_req &) { }
};

template <typename T> struct ndarray_arg<T, enable_if_t<ndarray_traits<T>::is_float>> {
    static constexpr size_t size = 0;

    static constexpr auto name =
        const_name("dtype=float") +
        const_name<sizeof(T) * 8>() +
        const_name<std::is_const_v<T>>(", writable=False", "");

    static void apply(ndarray_req &tr) {
        tr.dtype = dtype<T>();
        tr.req_dtype = true;
        tr.req_ro = std::is_const_v<T>;
    }
};

template <typename T> struct ndarray_arg<T, enable_if_t<ndarray_traits<T>::is_complex>> {
    static constexpr size_t size = 0;

    static constexpr auto name =
        const_name("dtype=complex") +
        const_name<sizeof(T) * 8>() +
        const_name<std::is_const_v<T>>(", writable=False", "");

    static void apply(ndarray_req &tr) {
        tr.dtype = dtype<T>();
        tr.req_dtype = true;
        tr.req_ro = std::is_const_v<T>;
    }
};

template <typename T> struct ndarray_arg<T, enable_if_t<ndarray_traits<T>::is_int>> {
    static constexpr size_t size = 0;

    static constexpr auto name =
        const_name("dtype=") +
        const_name<std::is_unsigned_v<T>>("u", "") +
        const_name("int") + const_name<sizeof(T) * 8>() +
        const_name<std::is_const_v<T>>(", writable=False", "");

    static void apply(ndarray_req &tr) {
        tr.dtype = dtype<T>();
        tr.req_dtype = true;
        tr.req_ro = std::is_const_v<T>;
    }
};

template <typename T> struct ndarray_arg<T, enable_if_t<ndarray_traits<T>::is_bool>> {
    static constexpr size_t size = 0;

    static constexpr auto name =
        const_name("dtype=bool") +
        const_name<std::is_const_v<T>>(", writable=False", "");

    static void apply(ndarray_req &tr) {
        tr.dtype = dtype<T>();
        tr.req_dtype = true;
        tr.req_ro |= std::is_const_v<T>;
    }
};

template<> struct ndarray_arg<ro> {
    static constexpr size_t size = 0;

    static constexpr auto name = const_name("writable=False");

    static void apply(ndarray_req &tr) {
        tr.req_ro = true;
    }
};

template <ssize_t... Is> struct ndarray_arg<shape<Is...>> {
    static constexpr size_t size = sizeof...(Is);
    static constexpr auto name =
        const_name("shape=(") +
        concat(const_name<Is == -1>(const_name("*"), const_name<(size_t) Is>())...) +
        const_name(")");

    static void apply(ndarray_req &tr) {
        size_t i = 0;
        ((tr.shape[i++] = (size_t) Is), ...);
        tr.ndim = (uint32_t) sizeof...(Is);
        tr.req_shape = true;
    }
};

template <> struct ndarray_arg<c_contig> {
    static constexpr size_t size = 0;
    static constexpr auto name = const_name("order='C'");
    static void apply(ndarray_req &tr) { tr.req_order = 'C'; }
};

template <> struct ndarray_arg<f_contig> {
    static constexpr size_t size = 0;
    static constexpr auto name = const_name("order='F'");
    static void apply(ndarray_req &tr) { tr.req_order = 'F'; }
};

template <> struct ndarray_arg<any_contig> {
    static constexpr size_t size = 0;
    static constexpr auto name = const_name("order='*'");
    static void apply(ndarray_req &tr) { tr.req_order = '\0'; }
};

template <typename T> struct ndarray_arg<T, enable_if_t<T::is_device>> {
    static constexpr size_t size = 0;
    static constexpr auto name = const_name("device='") + T::name + const_name('\'');
    static void apply(ndarray_req &tr) { tr.req_device = (uint8_t) T::value; }
};

template <typename... Ts> struct ndarray_info {
    using scalar_type = void;
    using shape_type = void;
    constexpr static auto name = const_name("ndarray");
    constexpr static ndarray_framework framework = ndarray_framework::none;
    constexpr static char order = '\0';
};

template <typename T, typename... Ts> struct ndarray_info<T, Ts...>  : ndarray_info<Ts...> {
    using scalar_type =
        std::conditional_t<ndarray_traits<T>::is_float || ndarray_traits<T>::is_int ||
                           ndarray_traits<T>::is_bool || ndarray_traits<T>::is_complex,
                           T, typename ndarray_info<Ts...>::scalar_type>;
};

template <ssize_t... Is, typename... Ts> struct ndarray_info<shape<Is...>, Ts...> : ndarray_info<Ts...> {
    using shape_type = shape<Is...>;
};

template <typename... Ts> struct ndarray_info<c_contig, Ts...> : ndarray_info<Ts...> {
    constexpr static char order = 'C';
};

template <typename... Ts> struct ndarray_info<f_contig, Ts...> : ndarray_info<Ts...> {
    constexpr static char order = 'F';
};

template <typename... Ts> struct ndarray_info<numpy, Ts...> : ndarray_info<Ts...> {
    constexpr static auto name = const_name("numpy.ndarray");
    constexpr static ndarray_framework framework = ndarray_framework::numpy;
};

template <typename... Ts> struct ndarray_info<pytorch, Ts...> : ndarray_info<Ts...> {
    constexpr static auto name = const_name("torch.Tensor");
    constexpr static ndarray_framework framework = ndarray_framework::pytorch;
};

template <typename... Ts> struct ndarray_info<tensorflow, Ts...> : ndarray_info<Ts...> {
    constexpr static auto name = const_name("tensorflow.python.framework.ops.EagerTensor");
    constexpr static ndarray_framework framework = ndarray_framework::tensorflow;
};

template <typename... Ts> struct ndarray_info<jax, Ts...> : ndarray_info<Ts...> {
    constexpr static auto name = const_name("jaxlib.xla_extension.DeviceArray");
    constexpr static ndarray_framework framework = ndarray_framework::jax;
};
template <typename... Ts> struct ndarray_info<cupy, Ts...> : ndarray_info<Ts...> {
    constexpr static auto name = const_name("cupy.ndarray");
    constexpr static ndarray_framework framework = ndarray_framework::cupy;
};


NAMESPACE_END(detail)

template <typename Scalar, typename Shape, char Order> struct ndarray_view {
    static constexpr size_t Dim = Shape::size;

    ndarray_view() = default;
    ndarray_view(const ndarray_view &) = default;
    ndarray_view(ndarray_view &&) = default;
    ndarray_view &operator=(const ndarray_view &) = default;
    ndarray_view &operator=(ndarray_view &&) noexcept = default;
    ~ndarray_view() noexcept = default;

    template <typename... Ts> NB_INLINE Scalar &operator()(Ts... indices) const {
        static_assert(
            sizeof...(Ts) == Dim,
            "ndarray_view::operator(): invalid number of arguments");

        const int64_t indices_i64[] { (int64_t) indices... };
        int64_t offset = 0;
        for (size_t i = 0; i < Dim; ++i)
            offset += indices_i64[i] * m_strides[i];

        return *(m_data + offset);
    }

    size_t ndim() const { return Dim; }
    size_t shape(size_t i) const { return m_shape[i]; }
    int64_t stride(size_t i) const { return m_strides[i]; }
    Scalar *data() const { return m_data; }

private:
    template <typename...> friend class ndarray;

    template <size_t... I1, ssize_t... I2>
    ndarray_view(Scalar *data, const int64_t *shape, const int64_t *strides,
                 std::index_sequence<I1...>, nanobind::shape<I2...>)
        : m_data(data) {

        /* Initialize shape/strides with compile-time knowledge if
           available (to permit vectorization, loop unrolling, etc.) */
        ((m_shape[I1] = (I2 == -1) ? shape[I1] : (int64_t) I2), ...);
        ((m_strides[I1] = strides[I1]), ...);

        if constexpr (Order == 'F') {
            m_strides[0] = 1;
            for (size_t i = 1; i < Dim; ++i)
                m_strides[i] = m_strides[i - 1] * m_shape[i - 1];
        } else if constexpr (Order == 'C') {
            m_strides[Dim - 1] = 1;
            for (Py_ssize_t i = (Py_ssize_t) Dim - 2; i >= 0; --i)
                m_strides[i] = m_strides[i + 1] * m_shape[i + 1];
        }
    }

    Scalar *m_data = nullptr;
    int64_t m_shape[Dim] { };
    int64_t m_strides[Dim] { };
};


template <typename... Args> class ndarray {
public:
    template <typename...> friend class ndarray;

    using Info = detail::ndarray_info<Args...>;
    using Scalar = typename Info::scalar_type;

    ndarray() = default;

    explicit ndarray(detail::ndarray_handle *handle) : m_handle(handle) {
        if (handle)
            m_dltensor = *detail::ndarray_inc_ref(handle);
    }

    template <typename... Args2>
    explicit ndarray(const ndarray<Args2...> &other) : ndarray(other.m_handle) { }

    ndarray(std::conditional_t<std::is_const_v<Scalar>, const void *, void *> data,
            size_t ndim,
            const size_t *shape,
            handle owner,
            const int64_t *strides = nullptr,
            dlpack::dtype dtype = nanobind::dtype<Scalar>(),
            int32_t device_type = device::cpu::value,
            int32_t device_id = 0) {
        m_handle = detail::ndarray_create(
            (void *) data, ndim, shape, owner.ptr(), strides, &dtype,
            std::is_const_v<Scalar>, device_type, device_id);
        m_dltensor = *detail::ndarray_inc_ref(m_handle);
    }

    ndarray(std::conditional_t<std::is_const_v<Scalar>, const void *, void *> data,
            std::initializer_list<size_t> shape,
            handle owner,
            std::initializer_list<int64_t> strides = { },
            dlpack::dtype dtype = nanobind::dtype<Scalar>(),
            int32_t device_type = device::cpu::value,
            int32_t device_id = 0) {

        if (strides.size() != 0 && strides.size() != shape.size())
            detail::fail("ndarray(): shape and strides have incompatible size!");

        m_handle = detail::ndarray_create(
            (void *) data, shape.size(), shape.begin(), owner.ptr(),
            (strides.size() == 0) ? nullptr : strides.begin(), &dtype,
            std::is_const_v<Scalar>, device_type, device_id);

        m_dltensor = *detail::ndarray_inc_ref(m_handle);
    }

    ~ndarray() {
        detail::ndarray_dec_ref(m_handle);
    }

    ndarray(const ndarray &t) : m_handle(t.m_handle), m_dltensor(t.m_dltensor) {
        detail::ndarray_inc_ref(m_handle);
    }

    ndarray(ndarray &&t) noexcept : m_handle(t.m_handle), m_dltensor(t.m_dltensor) {
        t.m_handle = nullptr;
        t.m_dltensor = dlpack::dltensor();
    }

    ndarray &operator=(ndarray &&t) noexcept {
        detail::ndarray_dec_ref(m_handle);
        m_handle = t.m_handle;
        m_dltensor = t.m_dltensor;
        t.m_handle = nullptr;
        t.m_dltensor = dlpack::dltensor();
        return *this;
    }

    ndarray &operator=(const ndarray &t) {
        detail::ndarray_inc_ref(t.m_handle);
        detail::ndarray_dec_ref(m_handle);
        m_handle = t.m_handle;
        m_dltensor = t.m_dltensor;
        return *this;
    }

    dlpack::dtype dtype() const { return m_dltensor.dtype; }
    size_t ndim() const { return (size_t) m_dltensor.ndim; }
    size_t shape(size_t i) const { return (size_t) m_dltensor.shape[i]; }
    int64_t stride(size_t i) const { return m_dltensor.strides[i]; }
    const int64_t* shape_ptr() const { return m_dltensor.shape; }
    const int64_t* stride_ptr() const { return m_dltensor.strides; }
    bool is_valid() const { return m_handle != nullptr; }
    int32_t device_type() const { return m_dltensor.device.device_type; }
    int32_t device_id() const { return m_dltensor.device.device_id; }
    detail::ndarray_handle *handle() const { return m_handle; }

    size_t size() const {
        size_t ret = is_valid();
        for (size_t i = 0; i < ndim(); ++i)
            ret *= shape(i);
        return ret;
    }

    size_t itemsize() const { return ((size_t) dtype().bits + 7) / 8; }
    size_t nbytes() const { return ((size_t) dtype().bits * size() + 7) / 8; }

    const Scalar *data() const {
        return (const Scalar *)((const uint8_t *) m_dltensor.data + m_dltensor.byte_offset);
    }

    template <typename T = Scalar, std::enable_if_t<!std::is_const_v<T>, int> = 1>
    Scalar *data() {
        return (Scalar *) ((uint8_t *) m_dltensor.data +
                           m_dltensor.byte_offset);
    }

    template <typename T = Scalar,
              std::enable_if_t<!std::is_const_v<T>, int> = 1, typename... Ts>
    NB_INLINE auto &operator()(Ts... indices) {
        return *(Scalar *) ((uint8_t *) m_dltensor.data +
                            byte_offset(indices...));
    }

    template <typename... Ts> NB_INLINE const auto & operator()(Ts... indices) const {
        return *(const Scalar *) ((const uint8_t *) m_dltensor.data +
                                  byte_offset(indices...));
    }

    template <typename... Extra> NB_INLINE auto view() const {
        using Info2 = typename ndarray<Args..., Extra...>::Info;
        using Scalar2 = typename Info2::scalar_type;
        using Shape2 = typename Info2::shape_type;

        constexpr bool has_scalar = !std::is_same_v<Scalar2, void>,
                       has_shape  = !std::is_same_v<Shape2, void>;

        static_assert(has_scalar,
            "To use the ndarray::view<..>() method, you must add a scalar type "
            "annotation (e.g. 'float') to the template parameters of the parent "
            "ndarray, or to the call to .view<..>()");

        static_assert(has_shape,
            "To use the ndarray::view<..>() method, you must add a shape<..> "
            "or ndim<..> annotation to the template parameters of the parent "
            "ndarray, or to the call to .view<..>()");

        if constexpr (has_scalar && has_shape) {
            return ndarray_view<Scalar2, Shape2, Info2::order>(
                (Scalar2 *) data(), shape_ptr(), stride_ptr(),
                std::make_index_sequence<Shape2::size>(), Shape2());
        } else {
            return nullptr;
        }
    }

private:
    template <typename... Ts>
    NB_INLINE int64_t byte_offset(Ts... indices) const {
        constexpr bool has_scalar = !std::is_same_v<Scalar, void>,
                       has_shape = !std::is_same_v<typename Info::shape_type, void>;

        static_assert(has_scalar,
            "To use ndarray::operator(), you must add a scalar type "
            "annotation (e.g. 'float') to the ndarray template parameters.");

        static_assert(has_shape,
            "To use ndarray::operator(), you must add a shape<> or "
            "ndim<> annotation to the ndarray template parameters.");

        if constexpr (has_scalar && has_shape) {
            static_assert(sizeof...(Ts) == Info::shape_type::size,
                          "ndarray::operator(): invalid number of arguments");

            size_t counter = 0;
            int64_t index = 0;
            ((index += int64_t(indices) * m_dltensor.strides[counter++]), ...);

            return (int64_t) m_dltensor.byte_offset + index * sizeof(typename Info::scalar_type);
        } else {
            return 0;
        }
    }

    detail::ndarray_handle *m_handle = nullptr;
    dlpack::dltensor m_dltensor;
};

inline bool ndarray_check(handle h) { return detail::ndarray_check(h.ptr()); }

NAMESPACE_BEGIN(detail)

template <typename... Args> struct type_caster<ndarray<Args...>> {
    NB_TYPE_CASTER(ndarray<Args...>, Value::Info::name + const_name("[") +
                                        concat_maybe(detail::ndarray_arg<Args>::name...) +
                                        const_name("]"))

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept {
        constexpr size_t size = (0 + ... + detail::ndarray_arg<Args>::size);
        size_t shape[size + 1];
        detail::ndarray_req req;
        req.shape = shape;
        (detail::ndarray_arg<Args>::apply(req), ...);
        if (src.is_none()) {
            value = ndarray<Args...>();
            return true;
        }
        value = ndarray<Args...>(ndarray_import(
            src.ptr(), &req, flags & (uint8_t) cast_flags::convert, cleanup));
        return value.is_valid();
    }

    static handle from_cpp(const ndarray<Args...> &tensor, rv_policy policy,
                           cleanup_list *cleanup) noexcept {
        return ndarray_wrap(tensor.handle(), Value::Info::framework, policy, cleanup);
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
