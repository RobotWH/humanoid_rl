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

#ifndef __UTILS_SERIALIZATION_HPP__
#define __UTILS_SERIALIZATION_HPP__

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/array.hpp>
#include <boost/serialization/split_free.hpp>

#include "tools_color_printf.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#ifndef SERIALIZATION_WITH_OPENCV
#define SERIALIZATION_WITH_OPENCV 1
#endif

#if SERIALIZATION_WITH_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#endif

namespace common_tools {
using std::cout;
using std::endl;
template <typename T>
inline void dump_obj_to_file(T *obj, std::string file_name, int if_bin = 1)
{
    // when set if_bin = 0, there might be crash with "terminate called after
    // throwing an instance of 'boost::archive::archive_exception'"
    std::ofstream ofs(file_name);
    if(ofs.is_open())
    {
        if(if_bin)
        {
            boost::archive::binary_oarchive oa(ofs);
            oa << *obj;
            ofs.close();
        } else
        {
            boost::archive::text_oarchive oa(ofs);
            oa << *obj;
            ofs.close();
        }
    } else
    {
        cout << ANSI_COLOR_RED_BOLD << "Dump obj to file [" << file_name << "] fail!, file can not open" << endl;
    }
}

template <typename T>
inline void load_obj_from_file(T *obj, std::string file_name, int if_bin = 1)
{
    // when set if_bin = 0, there might be crash with "terminate called after
    // throwing an instance of 'boost::archive::archive_exception'"
    std::ifstream ifs(file_name, std::ios_base::in);
    if(ifs.good())
    {
        if(if_bin)
        {
            boost::archive::binary_iarchive ia(ifs);
            ia >> *obj;
            ifs.close();
        } else
        {
            boost::archive::text_iarchive ia(ifs);
            ia >> *obj;
            ifs.close();
        }
    } else
    {
        // cout << ANSI_COLOR_RED_BOLD << "Load obj to file [" << file_name << "]
        // fail!, file can not open" << endl;
    }
}
} // namespace common_tools

namespace boost {
namespace serialization {
template <class Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void save(Archive &ar, const Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &M,
                 const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Index rows = M.rows();
    typename Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Index cols = M.cols();

    ar << rows;
    ar << cols;

    ar << make_array(M.data(), M.size());
}

template <class Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void load(Archive &ar, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &M,
                 const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Index rows, cols;

    ar >> rows;
    ar >> cols;

    // if (rows=!_Rows) throw std::exception(/*"Unexpected number of rows"*/);
    // if (cols=!_Cols) throw std::exception(/*"Unexpected number of cols"*/);

    ar >> make_array(M.data(), M.size());
}

template <class Archive, typename _Scalar, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void load(Archive &ar, Eigen::Matrix<_Scalar, Eigen::Dynamic, _Cols, _Options, _MaxRows, _MaxCols> &M,
                 const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, Eigen::Dynamic, _Cols, _Options, _MaxRows, _MaxCols>::Index rows, cols;

    ar >> rows;
    ar >> cols;

    // if (cols=!_Cols) throw std::exception(/*"Unexpected number of cols"*/);

    M.resize(rows, Eigen::NoChange);

    ar >> make_array(M.data(), M.size());
}

template <class Archive, typename _Scalar, int _Rows, int _Options, int _MaxRows, int _MaxCols>
inline void load(Archive &ar, Eigen::Matrix<_Scalar, _Rows, Eigen::Dynamic, _Options, _MaxRows, _MaxCols> &M,
                 const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, _Rows, Eigen::Dynamic, _Options, _MaxRows, _MaxCols>::Index rows, cols;

    ar >> rows;
    ar >> cols;

    // if (rows=!_Rows) throw std::exception(/*"Unexpected number of rows"*/);

    M.resize(Eigen::NoChange, cols);

    ar >> make_array(M.data(), M.size());
}

template <class Archive, typename _Scalar, int _Options, int _MaxRows, int _MaxCols>
inline void load(Archive &ar, Eigen::Matrix<_Scalar, Eigen::Dynamic, Eigen::Dynamic, _Options, _MaxRows, _MaxCols> &M,
                 const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, Eigen::Dynamic, Eigen::Dynamic, _Options, _MaxRows, _MaxCols>::Index rows, cols;

    ar >> rows;
    ar >> cols;

    M.resize(rows, cols);

    ar >> make_array(M.data(), M.size());
}

template <class Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void serialize(Archive &ar, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &M,
                      const unsigned int file_version)
{
    split_free(ar, M, file_version);
}

template <class Archive, typename _Scalar, int _Dim, int _Mode, int _Options>
inline void serialize(Archive &ar, Eigen::Transform<_Scalar, _Dim, _Mode, _Options> &t, const unsigned int version)
{
    serialize(ar, t.matrix(), version);
}

template <class Archive, typename _Scalar>
void save(Archive &ar, const Eigen::Triplet<_Scalar> &m, const unsigned int /*version*/)
{
    ar << m.row();
    ar << m.col();
    ar << m.value();
}

template <class Archive, typename _Scalar>
void load(Archive &ar, Eigen::Triplet<_Scalar> &m, const unsigned int /*version*/)
{
    typename Eigen::Triplet<_Scalar>::Index row, col;
    _Scalar value;

    ar >> row;
    ar >> col;
    ar >> value;

    m = Eigen::Triplet<_Scalar>(row, col, value);
}

template <class Archive, class _Scalar>
void serialize(Archive &ar, Eigen::Triplet<_Scalar> &m, const unsigned int version)
{
    split_free(ar, m, version);
}

template <class Archive, typename _Scalar, int _Options, typename _Index>
void save(Archive &ar, const Eigen::SparseMatrix<_Scalar, _Options, _Index> &m, const unsigned int /*version*/)
{
    _Index innerSize = m.innerSize();
    _Index outerSize = m.outerSize();

    typedef typename Eigen::Triplet<_Scalar> Triplet;
    std::vector<Triplet> triplets;

    for(_Index i = 0; i < outerSize; ++i)
        for(typename Eigen::SparseMatrix<_Scalar, _Options, _Index>::InnerIterator it(m, i); it; ++it)
            triplets.push_back(Triplet(it.row(), it.col(), it.value()));

    ar << innerSize;
    ar << outerSize;
    ar << triplets;
}

template <class Archive, typename _Scalar, int _Options, typename _Index>
void load(Archive &ar, Eigen::SparseMatrix<_Scalar, _Options, _Index> &m, const unsigned int /*version*/)
{
    _Index innerSize;
    _Index outerSize;

    ar >> innerSize;
    ar >> outerSize;

    _Index rows = (m.IsRowMajor) ? outerSize : innerSize;
    _Index cols = (m.IsRowMajor) ? innerSize : outerSize;

    m.resize(rows, cols);

    typedef typename Eigen::Triplet<_Scalar> Triplet;
    std::vector<Triplet> triplets;

    ar >> triplets;

    m.setFromTriplets(triplets.begin(), triplets.end());
}

template <class Archive, typename _Scalar, int _Options, typename _Index>
void serialize(Archive &ar, Eigen::SparseMatrix<_Scalar, _Options, _Index> &m, const unsigned int version)
{
    split_free(ar, m, version);
}

template <class Archive, typename _Scalar>
void serialize(Archive &ar, Eigen::Quaternion<_Scalar> &q, const unsigned int /*version*/)
{
    ar &q.w();
    ar &q.x();
    ar &q.y();
    ar &q.z();
}

template <class Archive, typename _Scalar>
void serialize(Archive &ar, Eigen::Quaternion<_Scalar, Eigen::DontAlign> &q, const unsigned int /*version*/)
{
    ar &q.w();
    ar &q.x();
    ar &q.y();
    ar &q.z();
}

#if SERIALIZATION_WITH_OPENCV

// Serialization support for cv::Mat
template <class Archive>
void save(Archive &ar, const cv::Mat &m, const unsigned int /*version*/)
{
    const int elem_type = m.type();
    const size_t elem_size = m.elemSize();

    ar &m.cols;
    ar &m.rows;
    ar &elem_type;
    ar &elem_size;

    const size_t data_size = elem_size * m.cols * m.rows;
    if(m.isContinuous())
    {
        ar &boost::serialization::make_array(m.ptr(), data_size);
    } else
    {
        cv::Mat m_cont;
        m.copyTo(m_cont);
        ar &boost::serialization::make_array(m_cont.ptr(), data_size);
    }
}
template <class Archive>
void load(Archive &ar, cv::Mat &m, const unsigned int /*version*/)
{
    int cols, rows, elem_type;
    size_t elem_size;

    ar &cols;
    ar &rows;
    ar &elem_type;
    ar &elem_size;

    m.create(rows, cols, elem_type);

    const size_t data_size = elem_size * m.cols * m.rows;
    ar &boost::serialization::make_array(m.ptr(), data_size);
}
template <class Archive>
inline void serialize(Archive &ar, cv::Mat &m, const unsigned int version)
{
    split_free(ar, m, version);
}

// Serialization support for cv::Mat_
template <class Archive, typename _Tp>
void save(Archive &ar, const cv::Mat_<_Tp> &m, const unsigned int /*version*/)
{
    ar &m.cols;
    ar &m.rows;

    const size_t data_size = m.cols * m.rows;
    if(m.isContinuous())
    {
        ar &boost::serialization::make_array((const _Tp *)m.ptr(), data_size);
    } else
    {
        cv::Mat_<_Tp> m_cont;
        m.copyTo(m_cont);
        ar &boost::serialization::make_array((const _Tp *)m_cont.ptr(), data_size);
    }
}
template <class Archive, typename _Tp>
void load(Archive &ar, cv::Mat_<_Tp> &m, const unsigned int /*version*/)
{
    int cols, rows;
    ar &cols;
    ar &rows;

    m.create(rows, cols);

    const size_t data_size = m.cols * m.rows;
    for(size_t n = 0; n < data_size; ++n)
        new((_Tp *)m.data + n) _Tp;
    ar &boost::serialization::make_array((_Tp *)m.ptr(), data_size);
}
template <class Archive, typename _Tp>
inline void serialize(Archive &ar, cv::Mat_<_Tp> &m, const unsigned int version)
{
    split_free(ar, m, version);
}

// Serialization support for cv::Matx
template <class Archive, typename _Tp, int m, int n>
void serialize(Archive &ar, cv::Matx<_Tp, m, n> &_m, const unsigned int /*version*/)
{
    ar &_m.val;
}

// Serialization support for cv::Vec
template <class Archive, typename _Tp, int cn>
void serialize(Archive &ar, cv::Vec<_Tp, cn> &v, const unsigned int /*version*/)
{
    ar &boost::serialization::base_object<cv::Matx<_Tp, cn, 1> >(v);
}

// Serialization support for cv::Point_
template <class Archive, typename _Tp>
void serialize(Archive &ar, cv::Point_<_Tp> &pt, const unsigned int /*version*/)
{
    ar &pt.x &pt.y;
}

// Serialization support for cv::Point3_
template <class Archive, typename _Tp>
void serialize(Archive &ar, cv::Point3_<_Tp> &pt, const unsigned int /*version*/)
{
    ar &pt.x &pt.y &pt.z;
}

// Serialization support for cv::Size_
template <class Archive, typename _Tp>
void serialize(Archive &ar, cv::Size_<_Tp> &sz, const unsigned int /*version*/)
{
    ar &sz.width &sz.height;
}

// Serialization support for cv::Rect_
template <class Archive, typename _Tp>
void serialize(Archive &ar, cv::Rect_<_Tp> &rc, const unsigned int /*version*/)
{
    ar &rc.x &rc.y &rc.width &rc.height;
}

// Serialization support for cv::KeyPoint
template <class Archive>
void serialize(Archive &ar, cv::KeyPoint &k, const unsigned int /*version*/)
{
    ar &k.pt;
    ar &k.size;
    ar &k.angle;
    ar &k.response;
    ar &k.octave;
    ar &k.class_id;
}

// Serialization support for cv::DMatch
template <class Archive>
void serialize(Archive &ar, cv::DMatch &m, const unsigned int /*version*/)
{
    ar &m.queryIdx;
    ar &m.trainIdx;
    ar &m.imgIdx;
    ar &m.distance;
}
#endif
} // namespace serialization
} // namespace boost

#endif // __UTILS_SERIALIZATION_HPP__