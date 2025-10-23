// =========================================================================
// # Vector类，单个vector<T>存储向量对象，包含向量基本运算，与Matrix密切相关
// - 向量基本信息：尺寸、元素数量等
// - 向量状态判断：是否为标量、空向量
// - 向量基本运算：加、减、乘、逐元素乘、逐元素除、逐元素幂、元素求和、最值、转置等
// -------------------------------------------------------------------------
// - 复数类型仅仅支持 complex<double>，故传入其他类型的复数时，不会实例化复数部分代码（事实上后果很严重！）
// - 通过枚举类型区分行列向量
// - 运算符重载只重载了向量数乘，向量之间的“乘法运算”用 dot() cross() 函数来进行
// =========================================================================

#pragma once

#include "core/matrix.hpp"
#include "core/vector.hpp"
// #include "utils/output.hpp"
#include <algorithm>
#include <cmath>
#include <complex>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace core {

// class Matrix; // 向前声明，避免 vector.hpp 和 matrix.hpp 相互循环 include 导致报错

enum class VectorOrientation {
    ROW,
    COLUMN
};

template <typename T>
class Vector {
public:
    using vecSizet = typename std::vector<T>::size_type;

    /* ==== friends ==== */
    template <typename U>
    friend Vector<U> operator+(const Vector<U>& vec1, const Vector<U>& vec2);
    template <typename U>
    friend Vector<U> operator-(const Vector<U>& vec1, const Vector<U>& vec2);

    Vector() = default;
    ~Vector() = default;
    Vector(const Vector<T>&) = default;
    Vector(Vector<T>&&) = default;
    Vector<T>& operator=(const Vector<T>&) = default;
    Vector<T>& operator=(Vector<T>&&) = default;

    /* ==== 构造函数们 ==== */
    Vector(const vecSizet dimension, const VectorOrientation orientation = VectorOrientation::COLUMN)
        : orientation_(orientation), dimension_(dimension) { initVec(dimension); }; // 构造维度为 demension 的向量
    Vector(std::initializer_list<T> il); // initializer_list 初始化

    /* ==== 运算符重载 ==== */
    T& operator()(vecSizet inx) { return vec_[inx]; } // x(0)访问向量的 第1个 元素。决定使用哪个版本的 () 取决于该向量对象是否为 const，与其他的无关
    const T& operator()(vecSizet inx) const { return vec_[inx]; }

    /* ==== 成员函数们 ==== */
    vecSizet getSize() const { return dimension_; } // 向量维度大小
    VectorOrientation getOrientation() const { return orientation_; } // 获取 行/列 状态
    T getElemSum() const; // 获取所有元素和
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type getMean() const { return (this->getElemSum() / this->getSize()); } // 获取实数向量所有元素平均值
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type getMax() const; // 获取最大值
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type getMin() const; // 获取最小值
    bool isScalar() const { return dimension_ == 1; } // 是否为常量
    bool isEmpty() const { return dimension_ == 0; } // 是否为空向量
    void reconstruct(vecSizet dimension = 0); // 重构向量，不改变行列特征

private:
    /* 一组初始化函数，利用 SFINAE 机制根据类型选择实例化哪种函数 */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, void>::type initVec(vecSizet dimension) {
        vec_ = std::vector<T>(dimension, 0);
    };
    template <typename U = T>
    typename std::enable_if<std::is_same<U, std::complex<double>>::value, void>::type initVec(vecSizet dimension) {
        vec_ = std::vector<std::complex<double>>(dimension, std::complex<double>(0.0, 0.0));
    };

    VectorOrientation orientation_{VectorOrientation::COLUMN}; // 默认为列向量
    vecSizet dimension_{0};
    std::vector<T> vec_{};
};

/* 打包好两种类型，目前 v0.x 到 v1.x 只支持 double，complex<double> 要等到 v2.x */
using dvec = Vector<double>;

/* ==== 类内实现 ==== */
/* 接受一个 initializer_list 视图，初始化为 Vector
 * - 默认生成的向量为列向量 */
template <typename T>
Vector<T>::Vector(std::initializer_list<T> il) {
    dimension_ = il.size();
    orientation_ = VectorOrientation::COLUMN; // 默认列向量
    auto pushIt = std::back_inserter(vec_);
    std::copy(il.begin(), il.end(), pushIt);
}

/* ==== 成员函数 ==== */
template <typename T>
T Vector<T>::getElemSum() const {
    T output{}; // 初始化为 0
    for (auto& i : vec_)
        output += i;
    return output;
}

/* 向量获取最大值 */
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type Vector<T>::getMax() const {
    if (this->getSize() == 1) // 一维
        return vec_[0];
    return *std::max_element(vec_.begin(), vec_.end());
}

/* 向量获取最小值 */
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type Vector<T>::getMin() const {
    if (this->getSize() == 1)
        return vec_[0];
    return *std::min_element(vec_.begin(), vec_.end());
}

/* 重构向量，同 matrix.hpp
 * - 默认不带维度则初始化为空向量
 * - 不改变行列特征 */
template <typename T>
void Vector<T>::reconstruct(vecSizet dimension) {
    dimension_ = dimension;
    initVec(dimension); // 不同类型不同初始化方式
}

/* ==== 运算符重载 ==== */
/* 向量格式化输出
 * - Vector 中重载了两个()运算符来访问，一个是 const 版本，访问 const 的 Vector，另一个是 非const 版本，访问普通 Vector
 * - std::complex<> 重载了输出运算符, 因而直接用就行(复数输出用圆括号包裹, 左为实部, 右为虚部) */
template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vec) {
    if (os.fail()) {
        throw std::ios::failure("vector.hpp: operator<<(): std::cout.fail() == true");
        return os;
    }
    if (vec.getSize() == 0) {
        os.setstate(std::ios::failbit); // failbit 置位
        return os;
    }

    if (vec.getOrientation() == VectorOrientation::ROW) { // 行向量
        for (typename Vector<T>::vecSizet i = 0; i < vec.getSize(); ++i)
            os << vec(i) << '\t';
        os << '\n'; // put a '\n' at the end of vector
    }
    else { // 列向量
        for (typename Vector<T>::vecSizet i = 0; i < vec.getSize(); ++i)
            os << vec(i) << '\n';
    }
    return os;
}

/* 相同类型向量之间相加 */
template <typename T>
Vector<T> operator+(const Vector<T>& vec1, const Vector<T>& vec2) { // +
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.hpp: operator+(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.hpp: operator+(): orientation of the two vectors do not match");

    Vector<T> output(vec1); // 初始化为与 vec1 一致的向量
    std::transform(vec1.vec_.begin(), vec1.vec_.end(), vec2.vec_.begin(), output.vec_.begin(), [](const T v1, const T v2) { return v1 + v2; });
    return output;
}

/* 相同类型向量之间相减 */
template <typename T>
Vector<T> operator-(const Vector<T>& vec1, const Vector<T>& vec2) { // -
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: operator-(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.cpp: operator-(): orientation of the two vectors do not match");

    Vector<T> output(vec1); // 初始化为与 vec1 一致的向量
    std::transform(vec1.vec_.begin(), vec1.vec_.end(), vec2.vec_.begin(), output.vec_.begin(), [](const T v1, const T v2) { return v1 - v2; });
    return output;
}

} // namespace core

////// class Vector {
////// public:
//////     using vectorSizet = std::vector<double>::size_type;

//////     /* friend */
//////     friend Vector operator+(const Vector& vec1, const Vector& vec2);
//////     friend Vector operator-(const Vector& vec1, const Vector& vec2);
//     friend Vector operator*(double scalar, const Vector& vec);
//     friend bool operator==(const Vector& vec1, const Vector& vec2);
//     friend bool operator!=(const Vector& vec1, const Vector& vec2);
//     friend Vector operator~(const Vector& vec);
//     friend Vector times(const Vector& vec1, const Vector& vec2);
//     friend Vector power(const Vector& vec1, const Vector& vec2);
//     friend Vector divide(const Vector& vec1, const Vector& vec2);
//     friend double dot(const Vector& vec1, const Vector& vec2);
//     friend Matrix cross(const Vector& vec1, const Vector& vec2);
//     friend Matrix tomtx(const Vector& vec);

//////     Vector() = default;
//////     ~Vector() = default;
//////     Vector(const Vector&) = default;
//////     Vector(Vector&&) = default;
//////     Vector& operator=(const Vector&) = default;
//////     Vector& operator=(Vector&&) = default;
////
//////     Vector(vectorSizet dimension) : dimension_(dimension), vec_(dimension, 0.0), orientation_(VectorOrientation::COLUMN) {};
//////     Vector(std::initializer_list<double> initList);

//////     /* 运算符重载 */
//////     double& operator()(vectorSizet index) { return vec_[index]; } // x(1)访问向量的 第2个 元素。决定使用哪个版本的 () 取决于该向量对象是否为 const，与其他的无关
//////     const double& operator()(vectorSizet index) const { return vec_[index]; } // 同上，const版本，具体使用时，如果该向量对象为 const 则调用此函数，否则调用上面的 非const 版本

//////     /* menber functions */
//////     vectorSizet getSize() const { return dimension_; } // 向量维度大小
//////     VectorOrientation getOrientation() const { return orientation_; } // 获取 行/列 状态
//////     double getElementSum() const; // 获取所有元素和
//////     double getMean() const { return this->getElementSum() / this->getSize(); } // 获取所有元素平均值
//////     double getMax() const; // 获取最大值
//////     double getMin() const; // 获取最小值
//////     bool isScalar() const { return dimension_ == 1; } // 是否为常量
//////     bool isEmpty() const { return dimension_ == 0; } // 是否为空向量
//////     void reconstruct(vectorSizet dimension = 0); // 重构向量，同 matrix.hpp

////// private:
//////     VectorOrientation orientation_ = VectorOrientation::COLUMN; // 默认为列向量
//////     vectorSizet dimension_ = 0;
//////     std::vector<double> vec_{};
////// };

// /* 运算符重载 */
////// Vector operator+(const Vector& vec1, const Vector& vec2); // +
////// Vector operator-(const Vector& vec1, const Vector& vec2); // -
// Vector operator*(double scalar, const Vector& vec); // scalar(left) * vec(right)
// inline Vector operator*(const Vector& vec, double scalar) { return scalar * vec; } // vec(left) * scalar(right)
// bool operator==(const Vector& vec1, const Vector& vec2); // ==
// bool operator!=(const Vector& vec1, const Vector& vec2); // !=
// Vector operator~(const Vector& vec); // 转置
////// std::ostream& operator<<(std::ostream& os, const Vector& vec); // 同上

// Vector times(const Vector& vec1, const Vector& vec2); // 逐元素乘
// Vector power(const Vector& vec1, const Vector& vec2); // 逐元素幂
// Vector divide(const Vector& vec1, const Vector& vec2); // 逐元素除

// double dot(const Vector& vec1, const Vector& vec2); // 点积，动态识别列向量左乘行向量（故必须接受两个维度相等的向量，一个行一个列，参数左右无所谓）
// Matrix cross(const Vector& vec1, const Vector& vec2); // 叉积，同上
// Matrix tomtx(const Vector& vec); // 将 1 * n 的 Vector 转换为 Matrix