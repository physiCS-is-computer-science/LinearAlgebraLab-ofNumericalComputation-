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

enum class VecOrientation {
    ROW,
    COLUMN
};

template <typename T>
class Vector;
template <typename T>
class Matrix;

/* 友元声明前 */
template <typename T>
Vector<T> operator+(const Vector<T>& vec1, const Vector<T>& vec2);
template <typename T>
Vector<T> operator-(const Vector<T>& vec1, const Vector<T>& vec2);
template <typename T>
Vector<T> operator*(double scalar, const Vector<T>& vec); // scalar(left) * vec(right)
template <typename T>
Vector<T> operator*(const Vector<T>& vec, double scalar); // vec(left) * scalar(right)
template <typename T>
bool operator==(const Vector<T>& vec1, const Vector<T>& vec2); // ==
template <typename T>
bool operator!=(const Vector<T>& vec1, const Vector<T>& vec2); // !=
template <typename T>
Vector<T> operator~(const Vector<T>& vec); // 转置
template <typename T>
Vector<T> times(const Vector<T>& vec1, const Vector<T>& vec2); // 逐元素乘
template <typename T>
Vector<T> power(const Vector<T>& vec1, const Vector<T>& vec2); // 逐元素幂
template <typename T>
Vector<T> divide(const Vector<T>& vec1, const Vector<T>& vec2); // 逐元素除
template <typename T>
T dot(const Vector<T>& vec1, const Vector<T>& vec2); // 点积，动态识别列向量左乘行向量（故必须接受两个维度相等的向量，一个行一个列，参数左右无所谓）
template <typename T>
Matrix<T> cross(const Vector<T>& vec1, const Vector<T>& vec2); // 叉积，同上
template <typename T>
Matrix<T> tomtx(const Vector<T>& vec); // Vector 转 Matrix

template <typename T>
class Vector {
public:
    using vecSizet = typename std::vector<T>::size_type;

    /* ==== friends ==== */
    friend Vector<T> operator+ <T>(const Vector<T>& vec1, const Vector<T>& vec2);
    friend Vector<T> operator- <T>(const Vector<T>& vec1, const Vector<T>& vec2);
    friend Vector<T> operator* <T>(double scalar, const Vector<T>& vec); // scalar(left) * vec(right)
    friend Vector<T> operator* <T>(const Vector<T>& vec, double scalar); // vec(left) * scalar(right)
    friend bool operator== <T>(const Vector<T>& vec1, const Vector<T>& vec2); // ==
    friend bool operator!= <T>(const Vector<T>& vec1, const Vector<T>& vec2); // !=
    friend Vector<T> operator~ <T>(const Vector<T>& vec); // 转置
    friend Vector<T> times<T>(const Vector<T>& vec1, const Vector<T>& vec2); // 逐元素乘
    friend Vector<T> power<T>(const Vector<T>& vec1, const Vector<T>& vec2); // 逐元素幂
    friend Vector<T> divide<T>(const Vector<T>& vec1, const Vector<T>& vec2); // 逐元素除
    friend T dot<T>(const Vector<T>& vec1, const Vector<T>& vec2); // 点积，动态识别列向量左乘行向量（故必须接受两个维度相等的向量，一个行一个列，参数左右无所谓）
    friend Matrix<T> cross<T>(const Vector<T>& vec1, const Vector<T>& vec2); // 叉积，同上
    friend Matrix<T> tomtx<T>(const Vector<T>& vec); // Vector 转 Matrix

    Vector() = default;
    ~Vector() = default;
    Vector(const Vector<T>&) = default;
    Vector(Vector<T>&&) = default;
    Vector<T>& operator=(const Vector<T>&) = default;
    Vector<T>& operator=(Vector<T>&&) = default;

    /* ==== 构造函数们 ==== */
    Vector(const vecSizet dimension, const VecOrientation orientation = VecOrientation::COLUMN) : orientation_(orientation), dimension_(dimension) { initVec(dimension); }; // 构造维度为 demension 的向量
    Vector(std::initializer_list<T> il); // initializer_list 初始化

    /* ==== 运算符重载 ==== */
    T& operator()(vecSizet inx) { return vec_[inx]; } // x(0)访问向量的 第1个 元素。决定使用哪个版本的 () 取决于该向量对象是否为 const，与其他的无关
    const T& operator()(vecSizet inx) const { return vec_[inx]; }

    /* ==== 成员函数们 ==== */
    vecSizet getSize() const { return dimension_; } // 向量维度大小
    VecOrientation getOrientation() const { return orientation_; } // 获取 行/列 状态
    T getElemSum() const; // 获取所有元素和
    T getMean() const { return (this->getElemSum() / this->getSize()); } // 获取实数向量所有元素平均值，未定义 complex<double>
    T getMax() const; // 获取最大值，未定义 complex<double>
    T getMin() const; // 获取最小值，未定义 complex<double>
    bool isScalar() const { return dimension_ == 1; } // 是否为常量
    bool isEmpty() const { return dimension_ == 0; } // 是否为空向量
    void reconstruct(vecSizet dimension = 0); // 重构向量，不改变行列特征

private:
    void initVec(vecSizet dimension) { vec_ = std::vector<T>(dimension, 0); } // 向量初始化函数

    VecOrientation orientation_{VecOrientation::COLUMN}; // 默认为列向量
    vecSizet dimension_{0};
    std::vector<T> vec_{};
};

/* 打包好两种类型，目前 v0.x 到 v1.x 只支持 double，complex<double> 要等到 v2.x */
using dvec = Vector<double>;
// using cvec = Vector<std::complex<double>>;

/* ==== 特例化 ==== */
/* complex<double> 版本特例化 */
template <>
inline void Vector<std::complex<double>>::initVec(vecSizet dimension) {
    vec_ = std::vector<std::complex<double>>(dimension, std::complex<double>(0.0, 0.0));
}

/* ==== 构造函数 ==== */
/* 接受一个 initializer_list 视图，初始化为 Vector
 * - 默认生成的向量为列向量 */
template <typename T>
Vector<T>::Vector(std::initializer_list<T> il) {
    dimension_ = il.size();
    orientation_ = VecOrientation::COLUMN; // 默认列向量
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
T Vector<T>::getMax() const {
    if (this->getSize() == 1) // 一维
        return vec_[0];
    return *std::max_element(vec_.begin(), vec_.end());
}

/* 向量获取最小值 */
template <typename T>
T Vector<T>::getMin() const {
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

    if (vec.getOrientation() == VecOrientation::ROW) { // 行向量
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

/* scalar(left) * vec(right) */
template <typename T>
Vector<T> operator*(double scalar, const Vector<T>& vec) {
    Vector output(vec);
    for (typename Vector<T>::vecSizet i = 0; i < output.dimension_; ++i)
        output.vec_[i] *= scalar;
    return output;
}

/* vec(right) * scalar(left) */
template <typename T>
Vector<T> operator*(const Vector<T>& vec, double scalar) {
    return scalar * vec;
}

/* 判断两个向量是否为“完全相等” */
template <typename T>
bool operator==(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.dimension_ == vec2.dimension_ && vec1.orientation_ == vec2.orientation_) {
        for (typename Vector<T>::vecSizet i = 0; i < vec1.dimension_; ++i) {
            if (vec1.vec_[i] != vec2.vec_[i])
                return false;
        }
        return true;
    }
    else
        return false;
}

/* 判断两个向量是否为“非完全相等” */
template <typename T>
bool operator!=(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.dimension_ != vec2.dimension_ || vec1.orientation_ != vec2.orientation_)
        return true;
    else {
        for (typename Vector<T>::vecSizet i = 0; i < vec1.dimension_; ++i) {
            if (vec1.vec_[i] != vec2.vec_[i])
                return true;
        }
        return false;
    }
}

/* 向量转置
 * - 不改变自身 */
template <typename T>
Vector<T> operator~(const Vector<T>& vec) {
    Vector<T> output(vec);
    if (output.orientation_ == VecOrientation::COLUMN)
        output.orientation_ = VecOrientation::ROW;
    else
        output.orientation_ = VecOrientation::COLUMN;

    return output;
}

/* ==== 普通函数 ==== */
/* 向量逐元素乘 */
template <typename T>
Vector<T> times(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.hpp: times(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.hpp: times(): orientation of the two vectors do not match");

    Vector output(vec1);
    std::transform(vec1.vec_.begin(), vec1.vec_.end(), vec2.vec_.begin(), output.vec_.begin(), [](T v1, T v2) { return v1 * v2; });
    return output;
}

/* 向量逐元素幂 */
template <typename T>
Vector<T> power(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.hpp: power(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.hpp: power(): orientation of the two vectors do not match");

    Vector output(vec1);
    std::transform(vec1.vec_.begin(), vec1.vec_.end(), vec2.vec_.begin(), output.vec_.begin(), [](T v1, T v2) { return pow(v1, v2); });
    return output;
}

/* 向量逐元素除
 * - 注意除零错误，有一个 invalid_argument() 抛出 */
template <typename T>
Vector<T> divide(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.hpp: power(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.hpp: power(): orientation of the two vectors do not match");

    Vector output(vec1);
    for (typename Vector<T>::vecSizet i = 0; i < output.dimension_; ++i) {
        if (vec2.vec_[i] == 0)
            throw std::invalid_argument("vector.hpp: divide(): Divide by zero error");
        output.vec_[i] /= vec2.vec_[i];
    }
    return output;
}

/* 点积
 * - 两个向量参数无需确认顺序，只要保证一个为行向量，一个为列向量即可
 * - 矩阵间乘法的实现方式 */
template <typename T>
T dot(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.hpp: dot(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() == vec2.getOrientation()) // 点积要求行和列
        throw std::invalid_argument("vector.hpp: dot(): orientation of the two vectors do not match");

    Vector temp(~vec1); // 用转置初始化为与 vec2 相同方向的向量
    return times(temp, vec2).getElemSum(); // 逐元素积求和
}

/* 叉积
 * - 两个向量参数无需确认顺序，只要保证一个为行向量，一个为列向量即可 */
template <typename T>
Matrix<T> cross(const Vector<T>& vec1, const Vector<T>& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.hpp: cross(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() == vec2.getOrientation()) // 叉积要求行和列
        throw std::invalid_argument("vector.hpp: cross(): orientation of the two vectors do not match");

    Matrix<T> output(vec1.getSize()); // 方阵
    if (vec1.getOrientation() == VecOrientation::ROW) { // 此时 vec2(col) 左乘 vec1(row)
        for (typename Vector<T>::vecSizet i = 0; i < vec1.getSize(); ++i) { // vec1 的每列都为一个标量，分别乘 vec2 构成一个矩阵，即为叉积
            Vector<T> temp(vec2 * vec1.vec_[i]);
            for (typename Vector<T>::vecSizet j = 0; j < vec1.getSize(); ++j)
                output(j, i) = temp(j); // temp 为列向量
        }
    }
    else { // 此时 vec1(col) 左乘 vec2(row)
        for (typename Vector<T>::vecSizet i = 0; i < vec2.getSize(); ++i) {
            Vector<T> temp(vec1 * vec2.vec_[i]);
            for (typename Vector<T>::vecSizet j = 0; j < vec2.getSize(); ++j)
                output(j, i) = temp(j);
        }
    }

    return output;
}

/* 接受 1 * n 或者 n * 1 的 Vector 返回对应维度方向的 Matrix */
template <typename T>
Matrix<T> tomtx(const Vector<T>& vec) {
    Matrix<T> output{};
    if (vec.getOrientation() == VecOrientation::ROW) {
        output.reconstruct(1, vec.getSize()); // 行向量
        for (typename Matrix<T>::mtxSizet i = 0; i < vec.getSize(); ++i)
            output(0, i) = vec(i);
    }
    else {
        output.reconstruct(vec.getSize(), 1); // 列向量
        for (typename Matrix<T>::mtxSizet i = 0; i < vec.getSize(); ++i)
            output(i, 0) = vec(i);
    }

    return output;
}

} // namespace core