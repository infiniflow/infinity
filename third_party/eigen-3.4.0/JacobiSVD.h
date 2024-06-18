// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2009-2010 Benoit Jacob <jacob.benoit.1@gmail.com>
// Copyright (C) 2013-2014 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef EIGEN_JACOBISVD_H
#define EIGEN_JACOBISVD_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

namespace Eigen {

class JacobiRotation;

struct Matrix {
    float *data_ = nullptr;
    int dimension_ = 0;
    Matrix(float *data, const int dimension) : data_(data), dimension_(dimension) {}
    [[nodiscard]] float coeff(const int i, const int j) const { return data_[i * dimension_ + j]; }
    inline void applyOnTheLeft(int p, int q, const JacobiRotation &j);
    inline void applyOnTheRight(int p, int q, const JacobiRotation &j);
};

// NOTICE: this class is copied from Eigen/src/Jacobi/Jacobi.h
class JacobiRotation {
public:
    /** Default constructor without any initialization. */
    JacobiRotation() = default;

    /** Construct a planar rotation from a cosine-sine pair (\a c, \c s). */
    JacobiRotation(const float c, const float s) : m_c(c), m_s(s) {}

    float &c() { return m_c; }
    float c() const { return m_c; }
    float &s() { return m_s; }
    float s() const { return m_s; }

    /** Concatenates two planar rotation */
    JacobiRotation operator*(const JacobiRotation &other) const {
        return JacobiRotation(m_c * other.m_c - m_s * other.m_s, m_c * other.m_s + m_s * other.m_c);
    }

    /** Returns the transposed transformation */
    JacobiRotation transpose() const { return JacobiRotation(m_c, -m_s); }

    /** Returns the adjoint transformation */
    JacobiRotation adjoint() const { return JacobiRotation(m_c, -m_s); }

    bool makeJacobi(const float x, const float y, const float z) {
        if (const float deno = 2.0f * std::abs(y); deno < std::numeric_limits<float>::min()) {
            m_c = 1.0f;
            m_s = 0.0f;
            return false;
        } else {
            const float tau = (x - z) / deno;
            const float w = std::sqrt(1.0f + tau * tau);
            const float t = 1.0f / ((tau > 0.0f) ? (tau + w) : (tau - w));
            const float n = 1.0f / std::sqrt(1.0f + t * t);
            m_s = std::signbit(y) ? (t * n) : (-t * n);
            m_c = n;
            return true;
        }
    }

    bool makeJacobi(const Matrix &m, const int p, const int q) { return makeJacobi(m.coeff(p, p), m.coeff(p, q), m.coeff(q, q)); }

    float m_c = 0.0f;
    float m_s = 0.0f;
};

namespace internal {

// NOTICE: this function is copied from Eigen/src/Jacobi/Jacobi.h
/** \jacobi_module
 * Applies the clock wise 2D rotation \a j to the set of 2D vectors of coordinates \a x and \a y:
 * \f$ \left ( \begin{array}{cc} x \\ y \end{array} \right )  =  J \left ( \begin{array}{cc} x \\ y \end{array} \right ) \f$
 *
 * \sa MatrixBase::applyOnTheLeft(), MatrixBase::applyOnTheRight()
 */
inline void apply_rotation_in_the_plane(float *x, const int incrx, float *y, const int incry, const int size, const JacobiRotation &j) {
    const float c = j.c();
    const float s = j.s();
    if (c == 1.0f && s == 0.0f) {
        return;
    }
    for (int i = 0; i < size; ++i) {
        const float xi = *x;
        const float yi = *y;
        *x = c * xi + s * yi;
        *y = -s * xi + c * yi;
        x += incrx;
        y += incry;
    }
}

} // namespace internal

// NOTICE: this function is copied from Eigen/src/Jacobi/Jacobi.h
/** \jacobi_module
 * Applies the rotation in the plane \a j to the rows \a p and \a q of \c *this, i.e., it computes B = J * B,
 * with \f$ B = \left ( \begin{array}{cc} \text{*this.row}(p) \\ \text{*this.row}(q) \end{array} \right ) \f$.
 *
 * \sa class JacobiRotation, MatrixBase::applyOnTheRight(), internal::apply_rotation_in_the_plane()
 */
inline void Matrix::applyOnTheLeft(const int p, const int q, const JacobiRotation &j) {
    float *x_ptr = data_ + p * dimension_;
    float *y_ptr = data_ + q * dimension_;
    internal::apply_rotation_in_the_plane(x_ptr, 1, y_ptr, 1, dimension_, j);
}

// NOTICE: this function is copied from Eigen/src/Jacobi/Jacobi.h
/** \ingroup Jacobi_Module
 * Applies the rotation in the plane \a j to the columns \a p and \a q of \c *this, i.e., it computes B = B * J
 * with \f$ B = \left ( \begin{array}{cc} \text{*this.col}(p) & \text{*this.col}(q) \end{array} \right ) \f$.
 *
 * \sa class JacobiRotation, MatrixBase::applyOnTheLeft(), internal::apply_rotation_in_the_plane()
 */
inline void Matrix::applyOnTheRight(const int p, const int q, const JacobiRotation &j) {
    float *x_ptr = data_ + p;
    float *y_ptr = data_ + q;
    internal::apply_rotation_in_the_plane(x_ptr, dimension_, y_ptr, dimension_, dimension_, j.transpose());
}

namespace internal {

// NOTICE: this function is copied from Eigen/src/misc/RealSvd2x2.h
inline void real_2x2_jacobi_svd(const Matrix &matrix, const int p, const int q, JacobiRotation &j_left, JacobiRotation &j_right) {
    float inner_matrix[4] = {matrix.coeff(p, p), matrix.coeff(p, q), matrix.coeff(q, p), matrix.coeff(q, q)};
    Matrix m(inner_matrix, 2);
    JacobiRotation rot1;
    const auto t = m.coeff(0, 0) + m.coeff(1, 1);
    const auto d = m.coeff(1, 0) - m.coeff(0, 1);
    if (std::abs(d) < std::numeric_limits<float>::min()) {
        rot1.s() = 0.0f;
        rot1.c() = 1.0f;
    } else {
        // If d!=0, then t/d cannot overflow because the magnitude of the
        // entries forming d are not too small compared to the ones forming t.
        const float u = t / d;
        const float tmp = std::sqrt(1.0f + u * u);
        rot1.s() = 1.0f / tmp;
        rot1.c() = u / tmp;
    }
    m.applyOnTheLeft(0, 1, rot1);
    j_right.makeJacobi(m, 0, 1);
    j_left = rot1 * j_right.transpose();
}

} // end namespace internal

/** \ingroup SVD_Module
 *
 *
 * \class JacobiSVD
 *
 * \brief Two-sided Jacobi SVD decomposition of a rectangular matrix
 *
 * \tparam _MatrixType the type of the matrix of which we are computing the SVD decomposition
 * \tparam QRPreconditioner this optional parameter allows to specify the type of QR decomposition that will be used internally
 *                        for the R-SVD step for non-square matrices. See discussion of possible values below.
 *
 * SVD decomposition consists in decomposing any n-by-p matrix \a A as a product
 *   \f[ A = U S V^* \f]
 * where \a U is a n-by-n unitary, \a V is a p-by-p unitary, and \a S is a n-by-p real positive matrix which is zero outside of its main diagonal;
 * the diagonal entries of S are known as the \em singular \em values of \a A and the columns of \a U and \a V are known as the left
 * and right \em singular \em vectors of \a A respectively.
 *
 * Singular values are always sorted in decreasing order.
 *
 * This JacobiSVD decomposition computes only the singular values by default. If you want \a U or \a V, you need to ask for them explicitly.
 *
 * You can ask for only \em thin \a U or \a V to be computed, meaning the following. In case of a rectangular n-by-p matrix, letting \a m be the
 * smaller value among \a n and \a p, there are only \a m singular vectors; the remaining columns of \a U and \a V do not correspond to actual
 * singular vectors. Asking for \em thin \a U or \a V means asking for only their \a m first columns to be formed. So \a U is then a n-by-m matrix,
 * and \a V is then a p-by-m matrix. Notice that thin \a U and \a V are all you need for (least squares) solving.
 *
 * Here's an example demonstrating basic usage:
 * \include JacobiSVD_basic.cpp
 * Output: \verbinclude JacobiSVD_basic.out
 *
 * This JacobiSVD class is a two-sided Jacobi R-SVD decomposition, ensuring optimal reliability and accuracy. The downside is that it's slower than
 * bidiagonalizing SVD algorithms for large square matrices; however its complexity is still \f$ O(n^2p) \f$ where \a n is the smaller dimension and
 * \a p is the greater dimension, meaning that it is still of the same order of complexity as the faster bidiagonalizing R-SVD algorithms.
 * In particular, like any R-SVD, it takes advantage of non-squareness in that its complexity is only linear in the greater dimension.
 *
 * If the input matrix has inf or nan coefficients, the result of the computation is undefined, but the computation is guaranteed to
 * terminate in finite (and reasonable) time.
 *
 * The possible values for QRPreconditioner are:
 * \li ColPivHouseholderQRPreconditioner is the default. In practice it's very safe. It uses column-pivoting QR.
 * \li FullPivHouseholderQRPreconditioner, is the safest and slowest. It uses full-pivoting QR.
 *     Contrary to other QRs, it doesn't allow computing thin unitaries.
 * \li HouseholderQRPreconditioner is the fastest, and less safe and accurate than the pivoting variants. It uses non-pivoting QR.
 *     This is very similar in safety and accuracy to the bidiagonalization process used by bidiagonalizing SVD algorithms (since bidiagonalization
 *     is inherently non-pivoting). However the resulting SVD is still more reliable than bidiagonalizing SVDs because the Jacobi-based iterarive
 *     process is more reliable than the optimized bidiagonal SVD iterations.
 * \li NoQRPreconditioner allows not to use a QR preconditioner at all. This is useful if you know that you will only be computing
 *     JacobiSVD decompositions of square matrices. Non-square matrices require a QR preconditioner. Using this option will result in
 *     faster compilation and smaller executable code. It won't significantly speed up computation, since JacobiSVD is always checking
 *     if QR preconditioning is needed before applying it anyway.
 *
 * \sa MatrixBase::jacobiSvd()
 */

// NOTICE: this function is modified from Eigen/src/SVD/JacobiSVD.h
// extra_buffer size:
// if NEED_SORT_SINGULAR, need dimension * (2 * dimension + 1)

#ifdef NEED_SORT_SINGULAR
inline void JacobiSVDSolve(const int dimension, float *A, float *output_U, float *output_V, float *extra_buffer) {
    float *const U = extra_buffer;
    extra_buffer += dimension * dimension;
    float *const V = extra_buffer;
    extra_buffer += dimension * dimension;
    float *const S = extra_buffer;
    extra_buffer += dimension;
#else

inline void JacobiSVDSolve(const int dimension, float *A, float *U, float *V) {

#endif

    Matrix matrix_A(A, dimension);
    Matrix matrix_U(U, dimension);
    Matrix matrix_V(V, dimension);

    // currently we stop when we reach precision 2*epsilon as the last bit of precision can require an unreasonable number of iterations,
    // only worsening the precision of U and V as we accumulate more rotations
    constexpr auto precision = 2 * std::numeric_limits<float>::epsilon();

    // limit for denormal numbers to be considered zero in order to avoid infinite loops (see bug 286)
    constexpr auto considerAsZero = std::numeric_limits<float>::min();

    // Scaling factor to reduce over/under-flows
    float scale = std::accumulate(A, A + dimension * dimension, 0.0f, [](float acc, float val) -> float { return std::max(acc, std::abs(val)); });
    if (scale == 0.0f) [[unlikely]] {
        scale = 1.0f;
    }

    /*** step 1. The R-SVD step: we use a QR decomposition to reduce to the case of a square matrix */
    // Removed because we only consider square matrix

    {
        if (scale != 1.0f) [[likely]] {
            for (int i = 0; i < dimension * dimension; ++i) {
                A[i] /= scale;
            }
        }
        std::fill_n(U, dimension * dimension, 0.0f);
        for (int i = 0; i < dimension; i++) {
            U[i * dimension + i] = 1.0f;
        }
        std::fill_n(V, dimension * dimension, 0.0f);
        for (int i = 0; i < dimension; i++) {
            V[i * dimension + i] = 1.0f;
        }
    }

    /*** step 2. The main Jacobi SVD iteration. ***/
    float maxDiagEntry = 0.0f;
    for (int i = 0; i < dimension; ++i) {
        maxDiagEntry = std::max(maxDiagEntry, std::abs(matrix_A.coeff(i, i)));
    }

    bool finished = false;
    while (!finished) {
        finished = true;

        // do a sweep: for all index pairs (p,q), perform SVD of the corresponding 2x2 sub-matrix

        for (int p = 1; p < dimension; ++p) {
            for (int q = 0; q < p; ++q) {
                // if this 2x2 sub-matrix is not diagonal already...
                // notice that this comparison will evaluate to false if any NaN is involved, ensuring that NaN's don't
                // keep us iterating forever. Similarly, small denormal numbers are considered zero.
                if (const float threshold = std::max(considerAsZero, precision * maxDiagEntry);
                    std::abs(matrix_A.coeff(p, q)) > threshold || std::abs(matrix_A.coeff(q, p)) > threshold) {
                    finished = false;
                    // perform SVD decomposition of 2x2 sub-matrix corresponding to indices p,q to make it diagonal
                    {
                        JacobiRotation j_left, j_right;
                        internal::real_2x2_jacobi_svd(matrix_A, p, q, j_left, j_right);

                        // accumulate resulting Jacobi rotations
                        matrix_A.applyOnTheLeft(p, q, j_left);
                        matrix_U.applyOnTheRight(p, q, j_left.transpose());

                        matrix_A.applyOnTheRight(p, q, j_right);
                        matrix_V.applyOnTheRight(p, q, j_right);

                        // keep track of the largest diagonal coefficient
                        maxDiagEntry = std::max(maxDiagEntry, std::max(std::abs(matrix_A.coeff(p, p)), std::abs(matrix_A.coeff(q, q))));
                    }
                }
            }
        }
    }

#ifdef NEED_SORT_SINGULAR

    /*** step 3. The work matrix is now diagonal, so ensure it's positive so its diagonal entries are the singular values ***/

    std::vector<char> column_negative_U(dimension);
    std::vector<int> column_real_idx_U(dimension);
    std::iota(column_real_idx_U.begin(), column_real_idx_U.end(), 0);
    std::vector column_real_idx_V(column_real_idx_U);

    for (int i = 0; i < dimension; ++i) {
        // m_workMatrix.coeff(i,i) is already real, no difficulty:
        const float a = matrix_A.coeff(i, i);
        S[i] = std::abs(a);
        if (a < 0.0f) {
            column_negative_U[i] = 1;
        }
    }

    if (scale != 1.0f) {
        for (int i = 0; i < dimension; ++i) {
            S[i] *= scale;
        }
    }

    /*** step 4. Sort singular values in descending order and compute the number of nonzero singular values ***/

    // int nonzeroSingularValues = dimension;
    for (int i = 0; i < dimension; ++i) {
        const auto max_tail_ptr = std::max_element(S + i, S + dimension);
        if (*max_tail_ptr == 0.0f) {
            // nonzeroSingularValues = i;
            break;
        }
        if (const int pos = static_cast<int>(max_tail_ptr - S); pos != i) {
            std::swap(S[i], S[pos]);
            std::swap(column_real_idx_U[i], column_real_idx_U[pos]);
            std::swap(column_real_idx_V[i], column_real_idx_V[pos]);
        }
    }

    // output
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            const auto u_idx = column_real_idx_U[j];
            output_U[j] = column_negative_U[u_idx] ? -U[u_idx] : U[u_idx];
            output_V[j] = V[column_real_idx_V[j]];
        }
        output_U += dimension;
        output_V += dimension;
    }

#else

    /*** step 3. The work matrix is now diagonal, so ensure it's positive so its diagonal entries are the singular values ***/

    std::vector<int> U_negative_column;

    for (int i = 0; i < dimension; ++i) {
        // m_workMatrix.coeff(i,i) is already real, no difficulty:
        if (matrix_A.coeff(i, i) < 0.0f) {
            U_negative_column.push_back(i);
        }
    }

    /*** step 4. Sort singular values in descending order and compute the number of nonzero singular values ***/
    // NOTICE: we don't need to sort singular values

    // output
    if (!U_negative_column.empty()) {
        for (int i = 0; i < dimension; ++i) {
            for (int j : U_negative_column) {
                U[j] = -U[j];
            }
            U += dimension;
        }
    }

#endif
}

} // end namespace Eigen

#endif // EIGEN_JACOBISVD_H
