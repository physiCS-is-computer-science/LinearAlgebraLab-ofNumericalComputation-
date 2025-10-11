// =========================================================================
// # Vector类，单个vector<double>存储向量对象，包含向量基本运算，与Matrix密切相关
// - 向量基本信息：尺寸、元素数量等
// - 向量状态判断：是否为标量、空向量
// - 向量基本运算：加、减、乘、逐元素乘、逐元素除、逐元素幂、元素求和、最值、转置等
// -------------------------------------------------------------------------
// - 通过枚举类型区分行列向量
// - *运算符重载只重载了向量数乘，向量之间的“乘法运算”用 dot() cross() 函数来进行
// =========================================================================

#pragma once

#include <vector>

class Matrix; // 向前声明，避免 vector.hpp 和 matrix.hpp 相互循环 include 导致报错

enum class VectorOrientation {
    ROW,
    COLUMN
};

class Vector {
  public:
    using vectorSizet = std::vector<double>::size_type;

    /* ======== friend ======== */
    friend Vector operator+(const Vector& vec1, const Vector& vec2);
    friend Vector operator-(const Vector& vec1, const Vector& vec2);
    friend Vector operator*(double scalar, const Vector& vec);
    friend bool operator==(const Vector& vec1, const Vector& vec2);
    friend bool operator!=(const Vector& vec1, const Vector& vec2);
    friend Vector operator~(const Vector& vec);
    friend Vector times(const Vector& vec1, const Vector& vec2);
    friend Vector power(const Vector& vec1, const Vector& vec2);
    friend Vector divide(const Vector& vec1, const Vector& vec2);
    friend double dot(const Vector& vec1, const Vector& vec2);
    friend Matrix cross(const Vector& vec1, const Vector& vec2);
    friend Matrix toMatrix(const Vector& vec);

    Vector() = default;
    Vector(vectorSizet dimension) : dimension_(dimension), vec_(dimension, 0.0), orientation_(VectorOrientation::COLUMN) {};
    Vector(const Vector& other) : dimension_(other.dimension_), vec_(other.vec_), orientation_(other.orientation_) {};
    Vector(std::initializer_list<double> initList);

    /* ======== 运算符重载 ======== */
    double& operator()(vectorSizet index) { return vec_[index]; } // x(1)访问向量的 第2个 元素。决定使用哪个版本的 () 取决于该向量对象是否为 const，与其他的无关
    const double& operator()(vectorSizet index) const { return vec_[index]; } // 同上，const版本，具体使用时，如果该向量对象为 const 则调用此函数，否则调用上面的 非const 版本

    /* ======== menber functions ======== */
    vectorSizet getSize() const { return dimension_; } // 向量维度大小
    VectorOrientation getOrientation() const { return orientation_; } // 获取 行/列 状态
    double getElementSum() const; // 获取所有元素和
    double getMean() const { return this->getElementSum() / this->getSize(); } // 获取所有元素平均值
    double getMax() const; // 获取最大值
    double getMin() const; // 获取最小值
    bool isScalar() const { return dimension_ == 1; } // 是否为常量
    bool isEmpty() const { return dimension_ == 0; } // 是否为空向量
    void reconstruct(vectorSizet dimension = 0); // 重构向量，同 matrix.hpp

  private:
    VectorOrientation orientation_ = VectorOrientation::COLUMN; // 默认为列向量
    vectorSizet dimension_ = 0;
    std::vector<double> vec_;
};

/* ======== 运算符重载 ======== */
Vector operator+(const Vector& vec1, const Vector& vec2); // +
Vector operator-(const Vector& vec1, const Vector& vec2); // -
Vector operator*(double scalar, const Vector& vec); // scalar(left) * vec(right)
inline Vector operator*(const Vector& vec, double scalar) { return scalar * vec; } // vec(left) * scalar(right)
bool operator==(const Vector& vec1, const Vector& vec2); // ==
bool operator!=(const Vector& vec1, const Vector& vec2); // !=
Vector operator~(const Vector& vec); // 转置

Vector times(const Vector& vec1, const Vector& vec2); // 逐元素乘
Vector power(const Vector& vec1, const Vector& vec2); // 逐元素幂
Vector divide(const Vector& vec1, const Vector& vec2); // 逐元素除

double dot(const Vector& vec1, const Vector& vec2); // 点积，动态识别列向量左乘行向量（故必须接受两个维度相等的向量，一个行一个列，参数左右无所谓）
Matrix cross(const Vector& vec1, const Vector& vec2); // 叉积，同上
Matrix toMatrix(const Vector& vec); // 将 1 * n 的 Vector 转换为 Matrix