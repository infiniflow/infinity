#include <nanobind/stl/complex.h>
#include <nanobind/eigen/dense.h>
#include <nanobind/eigen/sparse.h>
#include <nanobind/trampoline.h>

namespace nb = nanobind;

using namespace nb::literals;

NB_MODULE(test_eigen_ext, m) {
    m.def("addV3i",
          [](const Eigen::Vector3i &a,
             const Eigen::Vector3i &b) -> Eigen::Vector3i { return a + b; },
          "a"_a, "b"_a.noconvert());

    m.def("addR3i",
          [](const Eigen::RowVector3i &a,
             const Eigen::RowVector3i &b) -> Eigen::RowVector3i { return a + b; },
          "a"_a, "b"_a.noconvert());

    m.def("addRefCnstV3i",
          [](const Eigen::Ref<const Eigen::Vector3i> &a,
             const Eigen::Ref<const Eigen::Vector3i> &b) -> Eigen::Vector3i { return a + b; },
          "a"_a, "b"_a.noconvert());

    m.def("addRefCnstR3i",
          [](const Eigen::Ref<const Eigen::RowVector3i>& a,
             const Eigen::Ref<const Eigen::RowVector3i>& b) -> Eigen::RowVector3i { return a + b; },
          "a"_a, "b"_a.noconvert());

    m.def("addA3i",
          [](const Eigen::Array3i &a,
             const Eigen::Array3i &b) -> Eigen::Array3i { return a + b; },
          "a"_a, "b"_a.noconvert());

    m.def("addA3i_retExpr",
          [](const Eigen::Array3i &a,
             const Eigen::Array3i &b) { return a + b; },
          "a"_a, "b"_a.noconvert());

    m.def("addVXi",
          [](const Eigen::VectorXi &a,
             const Eigen::VectorXi &b) -> Eigen::VectorXi { return a + b; });

    using Matrix4uC = Eigen::Matrix<uint32_t, 4, 4, Eigen::ColMajor>;
    using Matrix4uR = Eigen::Matrix<uint32_t, 4, 4, Eigen::RowMajor>;
    using MatrixXuC = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;
    using MatrixXuR = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    m.def("addM4uCC",
          [](const Matrix4uC &a,
             const Matrix4uC &b) -> Matrix4uC { return a + b; });
    m.def("addMXuCC",
          [](const MatrixXuC &a,
             const MatrixXuC &b) -> MatrixXuC { return a + b; });
    m.def("addMXuCC_nc",
          [](const MatrixXuC &a,
             const MatrixXuC &b) -> MatrixXuC { return a + b; },
          "a"_a.noconvert(), "b"_a.noconvert());


    m.def("addM4uRR",
          [](const Matrix4uR &a,
             const Matrix4uR &b) -> Matrix4uR { return a + b; });
    m.def("addMXuRR",
          [](const MatrixXuR &a,
             const MatrixXuR &b) -> MatrixXuR { return a + b; });
    m.def("addMXuRR_nc",
          [](const MatrixXuR &a,
             const MatrixXuR &b) -> MatrixXuR { return a + b; },
          "a"_a.noconvert(), "b"_a.noconvert());

    m.def("addM4uCR",
          [](const Matrix4uC &a,
             const Matrix4uR &b) -> Matrix4uC { return a + b; });
    m.def("addMXuCR",
          [](const MatrixXuC &a,
             const MatrixXuR &b) -> MatrixXuC { return a + b; });

    m.def("addM4uRC",
          [](const Matrix4uR &a,
             const Matrix4uC &b) -> Matrix4uR { return a + b; });
    m.def("addMXuRC",
          [](const MatrixXuR &a,
             const MatrixXuC &b) -> MatrixXuR { return a + b; });

    m.def("addMapMXuCC",
          [](const Eigen::Map<MatrixXuC>& a,
             const Eigen::Map<MatrixXuC>& b) -> MatrixXuC { return a + b; });

    m.def("addMapCnstMXuCC",
          [](const Eigen::Map<const MatrixXuC>& a,
             const Eigen::Map<const MatrixXuC>& b) -> MatrixXuC { return a + b; });

    m.def("addMapMXuRR",
          [](const Eigen::Map<MatrixXuR>& a,
             const Eigen::Map<MatrixXuR>& b) -> MatrixXuC { return a + b; });

    m.def("addMapCnstMXuRR",
          [](const Eigen::Map<const MatrixXuR>& a,
             const Eigen::Map<const MatrixXuR>& b) -> MatrixXuC { return a + b; });

    m.def("addRefMXuCC",
          [](const Eigen::Ref<MatrixXuC>& a,
             const Eigen::Ref<MatrixXuC>& b) -> MatrixXuC { return a + b; });

    m.def("addRefCnstMXuCC",
          [](const Eigen::Ref<const MatrixXuC>& a,
             const Eigen::Ref<const MatrixXuC>& b) -> MatrixXuC { return a + b; });

    m.def("addRefCnstMXuCC_nc",
          [](const Eigen::Ref<const MatrixXuC>& a,
             const Eigen::Ref<const MatrixXuC>& b) -> MatrixXuC { return a + b; },
          "a"_a.noconvert(), "b"_a.noconvert());

    m.def("addRefMXuRR",
          [](const Eigen::Ref<MatrixXuR>& a,
             const Eigen::Ref<MatrixXuR>& b) -> MatrixXuC { return a + b; });

    m.def("addRefCnstMXuRR",
          [](const Eigen::Ref<const MatrixXuR>& a,
             const Eigen::Ref<const MatrixXuR>& b) -> MatrixXuC { return a + b; });

    m.def("addRefCnstMXuRR_nc",
          [](const Eigen::Ref<const MatrixXuR>& a,
             const Eigen::Ref<const MatrixXuR>& b) -> MatrixXuC { return a + b; },
          "a"_a.noconvert(), "b"_a.noconvert());

    m.def("addDRefMXuCC_nc",
          [](const nb::DRef<MatrixXuC> &a,
             const nb::DRef<MatrixXuC> &b) -> MatrixXuC { return a + b; },
          "a"_a.noconvert(), "b"_a.noconvert());

    m.def("addDRefMXuRR_nc",
          [](const nb::DRef<MatrixXuR>& a,
             const nb::DRef<MatrixXuR>& b) -> MatrixXuC { return a + b; },
          "a"_a.noconvert(), "b"_a.noconvert());

    m.def("mutate_DRefMXuC", [](nb::DRef<MatrixXuC> a) { a *= 2; }, nb::arg().noconvert());

    m.def("updateRefV3i", [](Eigen::Ref<Eigen::Vector3i> a) { a[2] = 123; });
    m.def("updateRefV3i_nc", [](Eigen::Ref<Eigen::Vector3i> a) { a[2] = 123; }, nb::arg().noconvert());
    m.def("updateRefVXi", [](Eigen::Ref<Eigen::VectorXi> a) { a[2] = 123; });
    m.def("updateRefVXi_nc", [](Eigen::Ref<Eigen::VectorXi> a) { a[2] = 123; }, nb::arg().noconvert());

    using SparseMatrixR = Eigen::SparseMatrix<float, Eigen::RowMajor>;
    using SparseMatrixC = Eigen::SparseMatrix<float>;
    Eigen::MatrixXf mat(5, 6);
    mat <<
	     0, 3,  0, 0,  0, 11,
	    22, 0,  0, 0, 17, 11,
	     7, 5,  0, 1,  0, 11,
	     0, 0,  0, 0,  0, 11,
	     0, 0, 14, 0,  8, 11;
    m.def("sparse_r", [mat]() -> SparseMatrixR {
        return Eigen::SparseView<Eigen::MatrixXf>(mat);
    });
    m.def("sparse_c", [mat]() -> SparseMatrixC {
        return Eigen::SparseView<Eigen::MatrixXf>(mat);
    });
    m.def("sparse_copy_r", [](const SparseMatrixR &m) -> SparseMatrixR { return m; });
    m.def("sparse_copy_c", [](const SparseMatrixC &m) -> SparseMatrixC { return m; });
    m.def("sparse_r_uncompressed", []() -> SparseMatrixR {
        SparseMatrixR m(2,2);
        m.coeffRef(0,0) = 1.0f;
        assert(!m.isCompressed());
        return m.markAsRValue();
    });
    m.def("sparse_complex", []() -> Eigen::SparseMatrix<std::complex<double>> { return {}; });

    /// issue #166
    using Matrix1d = Eigen::Matrix<double,1,1>;
    try {
        m.def(
            "default_arg", [](Matrix1d a, Matrix1d b) { return a + b; },
            "a"_a = Matrix1d::Zero(), "b"_a = Matrix1d::Zero());
    } catch (...) {
        // Ignore (NumPy not installed, etc.)
    }

    struct Buffer {
        uint32_t x[30] { };

        using Map = Eigen::Map<Eigen::Array<uint32_t, 10, 3>>;
        using DMap = Eigen::Map<Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic>>;

        Map map() { return Map(x); }
        DMap dmap() { return DMap(x, 10, 3); }
    };

    nb::class_<Buffer>(m, "Buffer")
        .def(nb::init<>())
        .def("map", &Buffer::map, nb::rv_policy::reference_internal)
        .def("dmap", &Buffer::dmap, nb::rv_policy::reference_internal);


    struct ClassWithEigenMember {
        Eigen::MatrixXd member = Eigen::Matrix2d::Ones();
    };

    nb::class_<ClassWithEigenMember>(m, "ClassWithEigenMember")
        .def(nb::init<>())
        .def_rw("member", &ClassWithEigenMember::member);

    m.def("castToMapVXi", [](nb::object obj) {
        return nb::cast<Eigen::Map<Eigen::VectorXi>>(obj);
    });
    m.def("castToMapCnstVXi", [](nb::object obj) {
      return nb::cast<Eigen::Map<const Eigen::VectorXi>>(obj);
      });
    m.def("castToRefVXi", [](nb::object obj) -> Eigen::VectorXi {
        return nb::cast<Eigen::Ref<Eigen::VectorXi>>(obj);
    });
    m.def("castToRefCnstVXi", [](nb::object obj) -> Eigen::VectorXi {
        return nb::cast<Eigen::Ref<const Eigen::VectorXi>>(obj);
    });
    m.def("castToDRefCnstVXi", [](nb::object obj) -> Eigen::VectorXi {
        return nb::cast<nb::DRef<const Eigen::VectorXi>>(obj);
    });
    m.def("castToRef03CnstVXi", [](nb::object obj) -> Eigen::VectorXi {
        return nb::cast<Eigen::Ref<const Eigen::VectorXi, Eigen::Unaligned, Eigen::InnerStride<3>>>(obj);
    });

    struct Base {
        virtual ~Base() = default;
        virtual void modRefData(Eigen::Ref<Eigen::VectorXd>) { };
        virtual void modRefDataConst(Eigen::Ref<const Eigen::VectorXd>) { };
    };

    struct PyBase : Base {
        NB_TRAMPOLINE(Base, 2);
        void modRefData(Eigen::Ref<Eigen::VectorXd> a) override {
            NB_OVERRIDE_PURE(modRefData, a);
        }
        void modRefDataConst(Eigen::Ref<const Eigen::VectorXd> a) override {
            NB_OVERRIDE_PURE(modRefDataConst, a);
        }
    };

    nb::class_<Base, PyBase>(m, "Base")
        .def(nb::init<>())
        .def("modRefData", &Base::modRefData)
        .def("modRefDataConst", &Base::modRefDataConst);

    m.def("modifyRef", [](Base* base) {
        Eigen::Vector2d input(1.0, 2.0);
        base->modRefData(input);
        return input;
    });
    m.def("modifyRefConst", [](Base* base) {
        Eigen::Vector2d input(1.0, 2.0);
        base->modRefDataConst(input);
        return input;
    });
}
