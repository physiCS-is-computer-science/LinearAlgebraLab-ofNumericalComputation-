#include "core/vector.hpp"
#include "core/matrix.hpp"
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

/* 接受一个 initializer_list 视图，初始化为 Vector
 * - 默认生成的向量为列向量 */
Vector::Vector(std::initializer_list<double> initList) {
    dimension_ = initList.size();
    orientation_ = VectorOrientation::COLUMN; // 默认列向量
    for (auto i : initList)
        vec_.push_back(i);
}

/* ======== menber functions ======== */
/* 向量元素求和 */
double Vector::getElementSum() const {
    double output = 0.0;
    for (auto& i : vec_)
        output += i;
    return output;
}

/* 向量获取最大值 */
double Vector::getMax() const {
    if (this->getSize() == 1)
        return vec_[0];

    double output(vec_[0]);
    for (auto& i : vec_) {
        if (i > output)
            output = i;
    }

    return output;
}

/* 向量获取最小值 */
double Vector::getMin() const {
    if (this->getSize() == 1)
        return vec_[0];

    double output(vec_[0]);
    for (auto& i : vec_) {
        if (i < output)
            output = i;
    }

    return output;
}

/* 向量之间相加 */
Vector operator+(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: operator+(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.cpp: operator+(): orientation of the two vectors do not match");

    Vector output(vec1);
    for (Vector::vectorSizet i = 0; i < output.dimension_; ++i)
        output.vec_[i] += vec2.vec_[i];
    return output;
}

/* 向量之间相减 */
Vector operator-(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: operator-(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.cpp: operator-(): orientation of the two vectors do not match");

    Vector output(vec1);
    for (Vector::vectorSizet i = 0; i < output.dimension_; ++i)
        output.vec_[i] -= vec2.vec_[i];
    return output;
}

/* 向量与标量相乘：scalar(left) * vec(right) */
Vector operator*(double scalar, const Vector& vec) {
    Vector output(vec);
    for (Vector::vectorSizet i = 0; i < output.dimension_; ++i)
        output.vec_[i] *= scalar;
    return output;
}

/* 判断两个向量是否为“完全相等” */
bool operator==(const Vector& vec1, const Vector& vec2) {
    if (vec1.dimension_ == vec2.dimension_ && vec1.orientation_ == vec2.orientation_) {
        for (Vector::vectorSizet i = 0; i < vec1.dimension_; ++i) {
            if (vec1.vec_[i] != vec2.vec_[i])
                return false;
        }
        return true;
    }
    else
        return false;
}

/* 判断两个向量是否为“非完全相等” */
bool operator!=(const Vector& vec1, const Vector& vec2) {
    if (vec1.dimension_ != vec2.dimension_ || vec1.orientation_ != vec2.orientation_)
        return true;
    else {
        for (Vector::vectorSizet i = 0; i < vec1.dimension_; ++i) {
            if (vec1.vec_[i] != vec2.vec_[i])
                return true;
        }
        return false;
    }
}

/* 向量转置
 * - 不改变自身 */
Vector operator~(const Vector& vec) {
    Vector output(vec);

    if (output.orientation_ == VectorOrientation::COLUMN)
        output.orientation_ = VectorOrientation::ROW;
    else
        output.orientation_ = VectorOrientation::COLUMN;

    return output;
}

/* 向量格式化输出
 * - Vector 中重载了两个()运算符来访问，一个是 const 版本，访问 const 的 Vector，另一个是 非const 版本，访问普通 Vector */
std::ostream& operator<<(std::ostream& os, const Vector& vec) {
    if (os.fail()) {
        throw std::ios::failure("matrix_vector_output.cpp: operator<<(): std::cout.fail() == true");
        return os;
    }
    if (vec.getSize() == 0) {
        os.setstate(std::ios::failbit); // failbit 置位
        return os;
    }

    if (vec.getOrientation() == VectorOrientation::ROW) {
        for (Vector::vectorSizet i = 0; i < vec.getSize(); ++i)
            os << vec(i) << '\t';
        os << '\n'; // put a '\n' at the end of vector
    }
    else {
        for (Vector::vectorSizet i = 0; i < vec.getSize(); ++i)
            os << vec(i) << '\n';
    }
    return os;
}

/* 向量逐元素乘 */
Vector times(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: times(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.cpp: times(): orientation of the two vectors do not match");

    Vector output(vec1);
    for (Vector::vectorSizet i = 0; i < output.dimension_; ++i)
        output.vec_[i] *= vec2.vec_[i];
    return output;
}

/* 向量逐元素幂 */
Vector power(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: power(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.cpp: power(): orientation of the two vectors do not match");

    Vector output(vec1);
    for (Vector::vectorSizet i = 0; i < output.dimension_; ++i)
        output.vec_[i] = pow(output.vec_[i], vec2.vec_[i]);
    return output;
}

/* 向量逐元素除
 * - 注意除零错误，有一个 invalid_argument() 抛出 */
Vector divide(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: power(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() != vec2.getOrientation())
        throw std::invalid_argument("vector.cpp: power(): orientation of the two vectors do not match");

    Vector output(vec1);
    for (Vector::vectorSizet i = 0; i < output.dimension_; ++i) {
        if (vec2.vec_[i] == 0)
            throw std::invalid_argument("vector.cpp: divide(): Divide by zero error");
        output.vec_[i] /= vec2.vec_[i];
    }
    return output;
}

/* 点积
 * - 两个向量参数无需确认顺序，只要保证一个为行向量，一个为列向量即可
 * - 矩阵间乘法的实现方式 */
double dot(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: dot(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() == vec2.getOrientation()) // 点积要求行和列
        throw std::invalid_argument("vector.cpp: dot(): orientation of the two vectors do not match");

    Vector temp(~vec1); // 用转置初始化为与 vec2 相同方向的向量
    return times(temp, vec2).getElementSum(); // 逐元素积求和
}

/* 叉积
 * - 两个向量参数无需确认顺序，只要保证一个为行向量，一个为列向量即可 */
Matrix cross(const Vector& vec1, const Vector& vec2) {
    if (vec1.getSize() != vec2.getSize())
        throw std::invalid_argument("vector.cpp: cross(): dimensions of the two vectors do not match, vec1 is " + std::to_string(vec1.getSize()) + ", vec2 is " + std::to_string(vec2.getSize()));
    if (vec1.getOrientation() == vec2.getOrientation()) // 叉积要求行和列
        throw std::invalid_argument("vector.cpp: cross(): orientation of the two vectors do not match");

    Matrix output(vec1.getSize()); // 方阵
    if (vec1.getOrientation() == VectorOrientation::ROW) { // 此时 vec2(col) 左乘 vec1(row)
        for (Vector::vectorSizet i = 0; i < vec1.getSize(); ++i) { // vec1 的每列都为一个标量，分别乘 vec2 构成一个矩阵，即为叉积
            Vector temp(vec2 * vec1.vec_[i]);
            for (Vector::vectorSizet j = 0; j < vec1.getSize(); ++j)
                output(j, i) = temp(j); // temp 为列向量
        }
    }
    else { // 此时 vec1(col) 左乘 vec2(row)
        for (Vector::vectorSizet i = 0; i < vec2.getSize(); ++i) {
            Vector temp(vec1 * vec2.vec_[i]);
            for (Vector::vectorSizet j = 0; j < vec2.getSize(); ++j)
                output(j, i) = temp(j);
        }
    }

    return output;
}

/* 接受 1 * n 或者 n * 1 的 Vector 返回对应维度方向的 Matrix */
Matrix tomtx(const Vector& vec) {
    Matrix output;

    if (vec.getOrientation() == VectorOrientation::ROW) {
        output.reconstruct(1, vec.getSize()); // 行向量
        for (Matrix::matrixSizet i = 0; i < vec.getSize(); ++i)
            output(0, i) = vec(i);
    }
    else {
        output.reconstruct(vec.getSize(), 1); // 列向量
        for (Matrix::matrixSizet i = 0; i < vec.getSize(); ++i)
            output(i, 0) = vec(i);
    }

    return output;
}

/* 重构向量，同 matrix.hpp
 * - 默认不带维度则初始化为空向量 */
void Vector::reconstruct(vectorSizet dimension) {
    dimension_ = dimension;
    vec_ = std::vector<double>(dimension, 0.0);
}