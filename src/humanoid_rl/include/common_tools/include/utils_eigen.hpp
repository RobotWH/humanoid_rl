/**
 * Copyright (c) [2025] XinChengYang <vertax@foxmail.com> <yaphetys@gmail.com>
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __UTILS_EIGEN_HPP__
#define __UTILS_EIGEN_HPP__
#include <Eigen/Eigen>

#include "utils_colorprint.hpp"

// For the consideration of avoiding alignment error while running, we prefer
// using datatype of Eigen with no alignment. In addition, for higher realtime
// performance (don't expect too much), you can modify the option with
// alignment, but you might carefully be aware of the crash of the program.
#define EIGEN_DATA_TYPE_DEFAULT_OPTION Eigen::DontAlign
// #define EIGEN_DATA_TYPE_DEFAULT_OPTION Eigen::AutoAlign

template <int M, int N, int option = (EIGEN_DATA_TYPE_DEFAULT_OPTION | Eigen::RowMajor)>
using eigen_mat_d = Eigen::Matrix<double, M, N, option>;

template <int M, int N, int option = (EIGEN_DATA_TYPE_DEFAULT_OPTION | Eigen::RowMajor)>
using eigen_mat_f = Eigen::Matrix<float, M, N, option>;

template <typename T, int M, int N, int option = (EIGEN_DATA_TYPE_DEFAULT_OPTION | Eigen::RowMajor)>
using eigen_mat_t = Eigen::Matrix<T, M, N, option>;

template <int M, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
using eigen_vec_d = Eigen::Matrix<double, M, 1, option>;

template <int M, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
using eigen_vec_f = Eigen::Matrix<float, M, 1, option>;

template <typename T, int M, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
using eigen_vec_t = Eigen::Matrix<T, M, 1, option>;

template <typename T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
using eigen_q_t = Eigen::Quaternion<T, option>;

template <typename T>
using eigen_angleaxis_t = Eigen::AngleAxis<T>;

template <typename T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
using eigen_pose_t = Eigen::Transform<T, 3, Eigen::Isometry, option>;

template <int M, int N>
using eigen_mat = eigen_mat_d<M, N>;

template <int M>
using eigen_vec = eigen_vec_d<M>;

typedef eigen_vec<2> vec_2;
typedef eigen_vec<3> vec_3;
typedef eigen_vec<4> vec_4;
typedef eigen_vec<6> vec_6;
typedef eigen_vec<7> vec_7;
typedef eigen_vec<12> vec_12;
typedef eigen_mat<3, 3> mat_3_3;
typedef eigen_mat<4, 4> mat_4_4;
typedef eigen_mat<6, 6> mat_6_6;
typedef eigen_mat<12, 12> mat_12;
typedef eigen_mat<6, 12> mat_6_12;
typedef eigen_mat<12, 6> mat_12_6;
typedef eigen_angleaxis_t<double> eigen_angleaxis;
typedef Eigen::Quaternion<double, EIGEN_DATA_TYPE_DEFAULT_OPTION> eigen_q;
typedef Eigen::Transform<double, 3, Eigen::Isometry, EIGEN_DATA_TYPE_DEFAULT_OPTION> eigen_pose;
typedef std::vector<eigen_q> eigen_q_vec;

// namespace common_tools
// {

template <typename T>
inline T angle_refine(const T &rad)
{
    // Refine angle to [-pi, pi]
    T rad_afr_refined = (rad - (floor(rad / T(2 * M_PI)) * T(2 * M_PI)));
    if(rad_afr_refined > T(M_PI))
    {
        rad_afr_refined -= T(2 * M_PI);
    }
    return rad_afr_refined;
}

/*****
    Some operator based tools for Eigen::Vector<T>
    Example:
        a. Eigen::Vector<T> from array:                Eigen::Vector<T> <<
data_rhs b. Eigen::Vector<T> to array:                  Eigen::Vector<T> >>
data_rhs
*****/
template <typename T, int M, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const void operator<<(eigen_vec_t<T, M, option> &eigen_vec_lhs, const T *data_rhs)
{
    for(size_t i = 0; i < M; i++)
    {
        eigen_vec_lhs(i) = data_rhs[i];
    }
}

template <typename T, int M, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const void operator>>(const eigen_vec_t<T, M, option> &eigen_vec_lhs, T *data_rhs)
{
    for(size_t i = 0; i < M; i++)
    {
        data_rhs[i] = eigen_vec_lhs(i);
    }
}

template <typename T, int M, typename TT = T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const void operator<<(eigen_vec_t<T, M, option> &eigen_vec_lhs,
                             const std::pair<std::vector<TT> *, int> &std_vector_start)
{
    // Loading data from a std::vector, from the starting point
    // Example: eigen_vec_lhs << std::make_pair(&std::vector, starting_point)
    for(size_t i = 0; i < M; i++)
    {
        eigen_vec_lhs(i) = T((*std_vector_start.first)[std_vector_start.second + i]);
    }
}

template <typename T, int M, typename TT = T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const void operator<<(const eigen_vec_t<T, M, option> &eigen_vec_lhs,
                             std::pair<std::vector<TT> *, int> &std_vector_start)
{
    for(size_t i = 0; i < M; i++)
    {
        (*std_vector_start.first)[std_vector_start.second + i] = TT(eigen_vec_lhs(i));
    }
}

/*****
    Some operator based tools for Eigen::Quaternion, before using these tools,
make sure you are using the uniform quaternion, otherwise some of the unwanted
results will be happend. Example: a. Quaternion from array: Eigen::Quaternion <<
data_rhs b. Quaternion to array:                     Eigen::Quaternion >>
data_rhs c. Rotation angle multiply(*=) a scalar:    Eigen::Quaternion *= scalar
        d. Rotation angle multiply(*=) a scalar:    Eigen::Quaternion * scalar
*****/
template <typename T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const void operator<<(eigen_q_t<T, option> &eigen_q_lhs, const T *data_rhs)
{
    eigen_q_lhs.w() = data_rhs[0];
    eigen_q_lhs.x() = data_rhs[1];
    eigen_q_lhs.y() = data_rhs[2];
    eigen_q_lhs.z() = data_rhs[3];
}

template <typename T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const void operator>>(const eigen_q_t<T, option> &eigen_q_lhs, T *data_rhs)
{
    data_rhs[0] = eigen_q_lhs.w();
    data_rhs[1] = eigen_q_lhs.x();
    data_rhs[2] = eigen_q_lhs.y();
    data_rhs[3] = eigen_q_lhs.z();
}

template <typename T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const eigen_q_t<T, option> &operator*=(eigen_q_t<T, option> &eigen_q_lhs, const T &s)
{
    Eigen::AngleAxis<T> angle_axis(eigen_q_lhs);
    angle_axis *= s;
    eigen_q_lhs = eigen_q_t<T, option>(angle_axis);
    return eigen_q_lhs;
}

template <typename T, int option = EIGEN_DATA_TYPE_DEFAULT_OPTION>
inline const eigen_q_t<T, option> &operator*(const eigen_q_t<T, option> &eigen_q_lhs, const T &s)
{
    Eigen::AngleAxis<T> angle_axis(eigen_q_lhs);
    angle_axis *= s;
    return eigen_q_t<T, option>(angle_axis);
}

/*****
    Conversion between eigen angle_axis and data array
    Example:
        a. AngleAxis from array: Eigen::AngleAxis << data_rhs
        b. AngleAxis to array:   Eigen::AngleAxis >> data_rhs
        c. Rotation angle multiply(*=) a scalar:    Eigen::AngleAxis *= scalar
        d. Rotation angle multiply(*=) a scalar:    Eigen::AngleAxis * scalar
*****/
template <typename T>
inline const void operator<<(Eigen::AngleAxis<T> &eigen_axisangle_lhs, const T *data_rhs)
{
    T vec_norm = sqrt(data_rhs[0] * data_rhs[0] + data_rhs[1] * data_rhs[1] + data_rhs[2] * data_rhs[2]);
    if(vec_norm != T(0.0))
    {
        eigen_axisangle_lhs.angle() = vec_norm;
        eigen_axisangle_lhs.axis() << data_rhs[0] / vec_norm, data_rhs[1] / vec_norm, data_rhs[2] / vec_norm;
    } else
    {
        eigen_axisangle_lhs.angle() = vec_norm;
        eigen_axisangle_lhs.axis() << vec_norm * data_rhs[0], vec_norm * data_rhs[1],
            vec_norm * data_rhs[2]; // For the consideration of derivation
    }
}

template <typename T>
inline const void operator>>(const Eigen::AngleAxis<T> &eigen_axisangle_lhs, T *data_rhs)
{
    T vec_norm = eigen_axisangle_lhs.angle();
    data_rhs[0] = eigen_axisangle_lhs.axis()(0) * vec_norm;
    data_rhs[1] = eigen_axisangle_lhs.axis()(1) * vec_norm;
    data_rhs[2] = eigen_axisangle_lhs.axis()(2) * vec_norm;
}

template <typename T>
inline const Eigen::AngleAxis<T> operator*=(Eigen::AngleAxis<T> &eigen_axisangle_lhs, const T &s)
{
    eigen_axisangle_lhs.angle() *= s;
    return eigen_axisangle_lhs;
}

template <typename T>
inline const Eigen::AngleAxis<T> operator*(const Eigen::AngleAxis<T> &eigen_axisangle_lhs, const T &s)
{
    Eigen::AngleAxis<T> angle_axis(eigen_axisangle_lhs);
    angle_axis.angle() *= s;
    return angle_axis;
}

// } // namespace common_tools

#endif // UTILS_EIGEN_HPP
