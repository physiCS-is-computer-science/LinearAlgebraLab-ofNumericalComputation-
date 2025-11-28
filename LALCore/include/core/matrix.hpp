// ================================================================================================================
// # Matrix<T> 模板类，存储矩阵对象，输出矩阵基本性质，进行矩阵自身或矩阵间的简单运算
// - 矩阵基本信息：尺寸、元素数量等
// - 矩阵状态判断：是否为标量、向量、空矩阵
// - 矩阵基本运算：加、减、乘、逐元素乘、逐元素除、逐元素幂、元素求和、最值、转置等
// - 为符合数学习惯，定义 vector<double> 为列，vector<vector<double>> 为行，访问可以直接按照数学约定顺序[row][col]访问
// ================================================================================================================

#pragma once

#include "core/vector.hpp"
#include "utils/numerical.hpp"
#include <algorithm>
#include <cmath>
#include <complex>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace core {

template <typename T>
class Vector;
template <typename T>
class Matrix;

/* 友元向前声明 */
template <typename T>
Matrix<T> operator+(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Matrix<T> operator-(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Matrix<T> operator*(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Matrix<T> operator*(const Matrix<T>& matrix, double scalar);
template <typename T>
Matrix<T> operator*(double scalar, const Matrix<T>& matrix);
template <typename T>
bool operator==(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
bool operator!=(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Matrix<T> operator~(const Matrix<T>& matrix);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix);
template <typename T>
Matrix<T> times(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Matrix<T> power(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Matrix<T> divide(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
template <typename T>
Vector<T> tovec(const Matrix<T>& matrix);
template <typename T>
bool dimeq(const Matrix<T>& mtx1, const Matrix<T>& mtx2);

template <typename T>
class Matrix {
public:
    using mtxSizet = typename std::vector<std::vector<T>>::size_type;

    /* ==== friend ==== */
    friend Matrix<T> operator+ <T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Matrix<T> operator- <T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Matrix<T> operator* <T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Matrix<T> operator* <T>(const Matrix<T>& matrix, double scalar);
    friend Matrix<T> operator* <T>(double scalar, const Matrix<T>& matrix);
    friend bool operator== <T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend bool operator!= <T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Matrix<T> operator~ <T>(const Matrix<T>& matrix);
    friend std::ostream& operator<< <T>(std::ostream& os, const Matrix<T>& matrix); // 默认矩阵末尾输出一个 '\n'
    friend Matrix<T> times<T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Matrix<T> power<T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Matrix<T> divide<T>(const Matrix<T>& matrix1, const Matrix<T>& matrix2);
    friend Vector<T> tovec<T>(const Matrix<T>& matrix);
    friend bool dimeq<T>(const Matrix<T>& mtx1, const Matrix<T>& mtx2);

    Matrix() = default;
    ~Matrix() = default;
    Matrix(const Matrix<T>&) = default;
    Matrix(Matrix<T>&&) = default;
    Matrix<T>& operator=(const Matrix<T>&) = default;
    Matrix<T>& operator=(Matrix<T>&&) = default;

    /* ==== type exchange ==== */
    operator Vector<T>() const; // 将一行或一列的矩阵在需要时转换为 Vector

    /* ==== 构造函数们 ==== */
    Matrix(mtxSizet row, mtxSizet col)
        : row_(row), col_(col) { // r 行 c 列的矩阵，默认初始化为 0
        initMtx(row, col);
    }
    Matrix(mtxSizet dimension)
        : row_(dimension), col_(dimension) { // dimension 维的方阵，初始化为 0
        initMtx(dimension, dimension);
    }
    Matrix(std::initializer_list<std::initializer_list<T>> il); // 初始化列表初始化，{{1, 2, 3}, {4, 5, 6}};

    /* ==== 运算符重载 ==== */
    T& operator()(mtxSizet row, mtxSizet col) { // 用 M(0, 1) 访问矩阵第一行第二列元素
        return matrix_[row][col];
    }
    const T& operator()(mtxSizet row, mtxSizet col) const { // 同上，const 版本，详细说明在 Vector 的()重载函数
        return matrix_[row][col];
    }

    /* ==== member functions ==== */
    mtxSizet getRowSize() const { return row_; } // 获取行大小
    mtxSizet getColSize() const { return col_; } // 获取列大小
    Vector<T> getRow(mtxSizet row) const; // 获取 row 行
    Vector<T> getCol(mtxSizet col) const; // 获取 col 列
    void exchangeRow(mtxSizet r1, mtxSizet r2); // 行交换
    void exchangeCol(mtxSizet c1, mtxSizet c2); // 列交换
    void replaceRow(mtxSizet pos, Vector<T> vec); // 覆盖某行
    void replaceCol(mtxSizet pos, Vector<T> vec); // 覆盖某列
    // void insertRow(mtxSizet pos); // pos 行之前插入一行
    // void insertCol(mtxSizet pos); // pos 列之前插入一列
    Vector<T> getMainDiag() const; // get main diagonal elements
    Vector<T> getAntiDiag() const; // get anti-diagonal elements
    double getTrace() const; // 迹
    double getElementSum() const; // 获取所有元素和
    double getMean() const; // 获取所有元素平均值
    double getMax() const; // 获取最大值
    double getMin() const; // 获取最小值
    bool isSquare() const { return row_ == col_; } // 判断是否为方阵
    bool isScalar() const { return row_ == col_ && row_ == 1; } // 判断是否为常量，it is a scalar if it has only 1 element
    bool isVector() const { return row_ == 1 || col_ == 1; } // 判断是否为向量
    bool isEmpty() const { return row_ == 0 && col_ == 0; } // 判断是否为空矩阵
    bool iszero() const;
    void reconstruct(mtxSizet row = 0, mtxSizet col = 0); // 重新构建矩阵大小，默认初始化元素为0，默认重构大小为 0 * 0 的空矩阵

private:
    void initMtx(mtxSizet row, mtxSizet col) { // 初始化 matrix_
        matrix_ = std::vector<std::vector<T>>(row, std::vector(col, T{}));
    }

    mtxSizet row_{};
    mtxSizet col_{};
    std::vector<std::vector<T>> matrix_{};
};

/* 开箱即用 */
using dmtx = Matrix<double>;
// using cmtx = Matrix<std::complex<double>>; // 等 v2.x

/* ==== 特例化 ==== */
/* complex<double> 版本特例化 */
template <>
inline void Matrix<std::complex<double>>::initMtx(mtxSizet row, mtxSizet col) {
    row_ = row;
    col_ = col;
    matrix_ = std::vector<std::vector<std::complex<double>>>(
        row,
        std::vector<std::complex<double>>(col, std::complex<double>(0.0, 0.0))
    );
}

/* ==== type exchange ==== */
/* Matrix<T> 转换为 Vector<T> */
template <typename T>
Matrix<T>::operator Vector<T>() const {
    Vector<T> output{};
    if (col_ == 1) { // 如果为 n * 1 的矩阵，此处直接处理为列向量（即为不执行转置）
        output.reconstruct(row_);
        for (typename Vector<T>::vecSizet i = 0; i < row_; ++i) {
            output(i) = matrix_[i][0];
        }
    }
    else if (row_ == 1) {
        output.reconstruct(col_);
        for (typename Vector<T>::vecSizet i = 0; i < col_; ++i) {
            output(i) = matrix_[0][i];
        }
        output = ~output; // 转为行向量
    }

    return output;
}

/* ==== 构造函数们 ==== */
/* 使用 initializer_list 列表视图初始化矩阵的构造函数
 * - 必须严格按照矩阵格式来初始化，{{1}, {2, 3}} 这样子初始化会得到错误的矩阵
 * - 矩阵大小以第一行的列数为矩阵列数，后续列数少于第一行则补零，多则丢弃 */
template <typename T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> il) {
    row_ = il.size();
    col_ = il.begin()->size(); // 第一个元素指向一个 initializer_list<T>
    initMtx(row_, col_); // 仅仅按照传入列表的第一行的维度初始化矩阵，因此不严格按照矩阵形式初始化的话，生成的矩阵有缺陷

    auto rowIt = il.begin();
    for (mtxSizet i = 0; i < row_; ++i, ++rowIt) {
        auto colIt = rowIt->begin(); // 如果不按照标准矩阵形式，这里开始，以后有可能会访问到尾后迭代器之外，行为未定义。因此下面的循环加了if语句保护内存，然而仍然无法保护矩阵完整，故依赖coder自觉点检查
        for (mtxSizet j = 0; j < col_; ++j, ++colIt) {
            if (colIt == rowIt->end()) { // 简单保护内存而已
                break;
            }
            matrix_[i][j] = *colIt;
        }
    }
}

/* ==== 运算符重载 ==== */
/* 矩阵之间加法：matrix + matrix
 * - 只检测两个矩阵行列是否完全相等 */
template <typename T>
Matrix<T> operator+(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_) {
        throw std::invalid_argument(
            "matrix.hpp: operator+(): "
            "Dimensional mismatch between the two matrices"
        );
    }

    Matrix<T> output(matrix1);
    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            output.matrix_[i][j] += matrix2.matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵之间减法：matrix - matrix
 * - 只检测两个矩阵行列是否完全相等 */
template <typename T>
Matrix<T> operator-(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_) {
        throw std::invalid_argument(
            "matrix.hpp: operator-(): "
            "Dimensional mismatch between the two matrices"
        );
    }

    Matrix output(matrix1);
    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            output.matrix_[i][j] -= matrix2.matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵之间乘法：matrix * matrix
 * - 计算方式：
 * 矩阵 A * B 拆分为：
 * | A = [a1]        | B = [b1 b2 ... bn] |
 * |     [a2]        |                    |
 * |     ...         |                    |
 * |     [am]        |                    |
 * 其中 am 为行向量，bn 为列向量
 * -------------------------------------------
 * - anbn为向量an与bn的点积，矩阵的乘积即为以下矩阵：
 * [a1b1 a1b2 a1b3 ... a1bn]
 * [a2b1 ...           ... ]
 * [...                    ]
 * [amb1 ...           ambn] */
template <typename T>
Matrix<T> operator*(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.col_ != matrix2.row_) {
        throw std::invalid_argument(
            "matrix.hpp: operator*(): "
            "Dimensional mismatch between the two matrices"
        );
    }
    Matrix<T> output(matrix1.row_, matrix2.col_);
    std::vector<Vector<T>> a(matrix1.row_); // row_ 个行向量
    std::vector<Vector<T>> b(matrix2.col_); // col_ 个列向量

    for (typename Matrix<T>::mtxSizet i = 0; i < matrix1.row_; ++i) {
        a[i] = matrix1.getRow(i); // 获取每一行
    }
    for (typename Matrix<T>::mtxSizet i = 0; i < matrix2.col_; ++i) {
        b[i] = matrix2.getCol(i); // 获取每一列
    }

    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            output.matrix_[i][j] = dot(a[i], b[j]);
        }
    }

    return output;
}

/* 矩阵与标量乘法：matrix(left) * scalar(right)
 * - 无错误检查*/
template <typename T>
Matrix<T> operator*(const Matrix<T>& matrix, double scalar) {
    Matrix<T> output(matrix);
    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            output.matrix_[i][j] *= scalar;
        }
    }
    return output;
}

/* 矩阵与标量乘法：scalar(left) * matrix(left)
 * - 调用 matrix(left) * scalar(right) */
template <typename T>
Matrix<T> operator*(double scalar, const Matrix<T>& matrix) {
    return matrix * scalar;
}

/* 判断两个矩阵是否包括所有元素完全相等 */
template <typename T>
bool operator==(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ == matrix2.row_ && matrix1.col_ == matrix2.col_) {
        for (typename Matrix<T>::mtxSizet i = 0; i < matrix1.row_; ++i) {
            for (typename Matrix<T>::mtxSizet j = 0; j < matrix1.col_; ++j) {
                if (matrix1.matrix_[i][j] != matrix2.matrix_[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
    else {
        return false;
    }
}

/* 判断两个矩阵是否不完全相等 */
template <typename T>
bool operator!=(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_) {
        return true;
    }
    else {
        for (typename Matrix<T>::mtxSizet i = 0; i < matrix1.row_; ++i) {
            for (typename Matrix<T>::mtxSizet j = 0; j < matrix1.col_; ++j) {
                if (matrix1.matrix_[i][j] != matrix2.matrix_[i][j]) {
                    return true;
                }
            }
        }
        return false;
    }
}

/* 矩阵转置
 * - 不改变自身 */
template <typename T>
Matrix<T> operator~(const Matrix<T>& matrix) {
    if (matrix.isEmpty()) {
        throw std::invalid_argument(
            "matrix.hpp: operator~(): "
            "the incomming matrix is empty"
        );
    }

    Matrix<T> output(matrix.col_, matrix.row_);
    for (typename Matrix<T>::mtxSizet i = 0; i < matrix.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < matrix.col_; ++j) {
            output.matrix_[j][i] = matrix.matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵格式化输出 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
    if (os.fail()) {
        throw std::ios::failure(
            "matrix.hpp: operator<<(): "
            "std::cout.fail() == true"
        );
        return os;
    }
    if (matrix.getColSize() == 0 || matrix.getRowSize() == 0) {
        os.setstate(std::ios::failbit); // failbit 置位
        return os;
    }

    os << "\n\n";
    for (typename Matrix<T>::mtxSizet row = 0; row < matrix.getRowSize(); ++row) {
        Vector<T> temp(matrix.getRow(row)); // 临时行
        for (typename Vector<T>::vecSizet i = 0; i < temp.getSize(); ++i) { // 逐个输出临时行的元素
            os << std::setw(12) << temp(i); // 规定每个数字宽度为 12
        }
        os << '\n'; // put a '\n' at the end of matrix
    }

    return os;
}

/* ==== 普通函数 ==== */
/* 矩阵逐元素相乘 */
template <typename T>
Matrix<T> times(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_) {
        throw std::invalid_argument(
            "matrix.hpp: times(): "
            "Dimensional mismatch between the two matrices"
        );
    }
    Matrix<T> output(matrix1);
    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            output.matrix_[i][j] *= matrix2.matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵逐元素幂 */
template <typename T>
Matrix<T> power(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_) {
        throw std::invalid_argument(
            "matrix.hpp: power(): "
            "Dimensional mismatch between the two matrices"
        );
    }
    Matrix<T> output(matrix1);
    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            output.matrix_[i][j] = pow(output.matrix_[i][j], matrix2.matrix_[i][j]);
        }
    }
    return output;
}

/* 矩阵逐元素除
 * - 区分左右
 * - 留意除零错误，有一个 invalid_argument() 抛出 */
template <typename T>
Matrix<T> divide(const Matrix<T>& matrix1, const Matrix<T>& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_) {
        throw std::invalid_argument(
            "matrix.hpp: divide(): "
            "Dimensional mismatch between the two matrices"
        );
    }
    Matrix<T> output(matrix1);
    for (typename Matrix<T>::mtxSizet i = 0; i < output.row_; ++i) {
        for (typename Matrix<T>::mtxSizet j = 0; j < output.col_; ++j) {
            if (matrix2.matrix_[i][j] == 0) {
                throw std::invalid_argument(
                    "matrix.hpp: divide(): "
                    "Divide by zero error"
                );
            }
            output.matrix_[i][j] /= matrix2.matrix_[i][j];
        }
    }
    return output;
}

/* 接受 1*n 或 n*1 的 Matrix，返回对应方向的 Vector */
template <typename T>
Vector<T> tovec(const Matrix<T>& matrix) {
    if (matrix.row_ != 1 && matrix.col_ != 1) {
        throw std::invalid_argument(
            "matrix.hpp: tovec(): "
            "expected a 1*n or n*1 matrix, but passed that is not"
        );
    }

    Vector<T> output{};

    if (matrix.row_ == 1) { // 此时为行向量
        output.reconstruct(matrix.col_);
        output = ~output; // 默认为列向量，故转置
        for (typename Vector<T>::vecSizet i = 0; i < matrix.col_; ++i) {
            output(i) = matrix(0, i);
        }
    }
    else {
        output.reconstruct(matrix.row_); // 此时为列向量
        for (typename Vector<T>::vecSizet i = 0; i < matrix.row_; ++i) {
            output(i) = matrix(i, 0);
        }
    }

    return output;
}

/* 判断两个矩阵的维度是否相等，仅仅判断维度 */
template <typename T>
bool dimeq(const Matrix<T>& mtx1, const Matrix<T>& mtx2) {
    return mtx1.row_ == mtx2.row_ && mtx1.col_ == mtx2.col_;
}

/* ==== member functions ==== */
/* get a row of the matrix */
template <typename T>
Vector<T> Matrix<T>::getRow(mtxSizet row) const {
    if (row >= row_ || row < 0) {
        throw std::out_of_range(
            "matrix.hpp: getRow(): "
            "colume index " + std::to_string(row) + " out of range " + 
            "[0, " + std::to_string(row_ - 1) + "]"
        );
    }

    Vector<T> output(col_);
    for (typename Vector<T>::vecSizet i = 0; i < col_; ++i) {
        output(i) = matrix_[row][i];
    }
    output = ~output; // 转置为行向量
    return output;
}

/* get a column of the matrix
 * - if col out of range, throw out_of_range() */
template <typename T>
Vector<T> Matrix<T>::getCol(mtxSizet col) const {
    if (col >= col_ || col < 0) {
        throw std::out_of_range(
            "matrix.hpp: getCol(): "
            "colume index " + std::to_string(col) + " out of range " +
            "[0, " + std::to_string(col_ - 1) + "]"
        );
    }

    Vector<T> output(row_);
    for (typename Vector<T>::vecSizet i = 0; i < row_; ++i) {
        output(i) = matrix_[i][col];
    }
    return output; // 默认列向量，无需转置
}

/* 交换行 r1 和行 r2 */
template <typename T>
void Matrix<T>::exchangeRow(mtxSizet r1, mtxSizet r2) {
    if (r1 >= row_ || r2 >= row_) {
        throw std::invalid_argument(
            "matrix.hpp: exchangeRow(): "
            "Dimension not match"
        );
    }

    T temp;
    for (mtxSizet i = 0; i < col_; ++i) {
        temp = matrix_[r1][i];
        matrix_[r1][i] = matrix_[r2][i];
        matrix_[r2][i] = temp;
    }
}

/* 交换列 r1 和列 r2 */
template <typename T>
void Matrix<T>::exchangeCol(mtxSizet c1, mtxSizet c2) {
    if (c1 >= col_ || c2 >= col_) {
        throw std::invalid_argument(
            "matrix.hpp: exchangeCol(): "
            "Dimension not match"
        );
    }

    T temp;
    for (mtxSizet i = 0; i < row_; ++i) {
        temp = matrix_[i][c1];
        matrix_[i][c1] = matrix_[i][c2];
        matrix_[i][c2] = temp;
    }
}

/* 覆盖某一行
 * - 接收行位置、行向量，任意一个不匹配则抛出 std::invalid_argument */
template <typename T>
void Matrix<T>::replaceRow(mtxSizet pos, Vector<T> vec) {
    /* 运算匹配性检查 */
    if (pos >= row_) {
        throw std::invalid_argument(
            "matrix.hpp: replaceRow(): "
            "Max dimension is:" + std::to_string(row_) + 
            " but pass: " + std::to_string(pos)
        );
    }
    if (vec.getOrientation() != VecOrientation::ROW || vec.getSize() != col_) {
        throw std::invalid_argument(
            "matrix.hpp: replaceRow(): "
            "Orientation or dimension error"
        );
    }

    /* 逐项覆盖 */
    for (mtxSizet i = 0; i < col_; ++i) {
        matrix_[pos][i] = vec(i);
    }
}

/* 覆盖某一列
 * - 接收列位置、列向量，任意一个不匹配则抛出 std::invalid_argument */
template <typename T>
void Matrix<T>::replaceCol(mtxSizet pos, Vector<T> vec) {
    /* 运算匹配性检查 */
    if (pos >= col_) {
        throw std::invalid_argument(
            "matrix.hpp: replaceRow(): "
            "Max dimension is:" + std::to_string(col_) +
            " but pass: " + std::to_string(pos)
        );
    }
    if (vec.getOrientation() != VecOrientation::COLUMN || vec.getSize() != row_) {
        throw std::invalid_argument(
            "matrix.hpp: replaceRow(): "
            "Orientation or dimension error"
        );
    }

    /* 逐项覆盖 */
    for (mtxSizet i = 0; i < row_; ++i) {
        matrix_[i][pos] = vec(i);
    }
}

/* 获取主对角线元素
 * - 成功返回一个 Vector
 * - 失败抛出 domain_erroe()*/
template <typename T>
Vector<T> Matrix<T>::getMainDiag() const {
    if (!isSquare()) {
        throw std::domain_error(
            "matrix.hpp: getMainDiag(): "
            "expect a square matrix, but it is a non-square matrix"
        ); // 域错误
    }

    Vector<T> output(row_);
    for (typename Vector<T>::vecSizet i = 0; i < row_; ++i) {
        output(i) = matrix_[i][i];
    }
    return output;
}

/* 副对角线，同上 */
template <typename T>
Vector<T> Matrix<T>::getAntiDiag() const {
    if (!isSquare()) {
        throw std::domain_error( // 域错误
            "matrix.hpp: getAntiDiag(): "
            "expect a square matrix, but it is a non-square matrix"
        );
    }

    Vector output(row_);
    for (typename Vector<T>::vecSizet i = 0; i < row_; ++i) {
        output(i) = matrix_[i][row_ - 1 - i];
    }
    return output;
}

/* 返回矩阵的迹 */
template <typename T>
double Matrix<T>::getTrace() const {
    if (!(this->isSquare())) {
        throw std::domain_error( // 域错误
            "matrix.hpp: getTrace(): "
            "expect a square matrix, but it is a non-square matrix"
        );
    }

    double output = 0.0;
    for (mtxSizet i = 0; i < row_; ++i) {
        output += matrix_[i][i];
    }
    return output;
}

/* 矩阵元素求和 */
template <typename T>
double Matrix<T>::getElementSum() const {
    double output{};
    for (mtxSizet i = 0; i < row_; ++i) {
        for (mtxSizet j = 0; j < col_; ++j) {
            output += matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵平均值 */
template <typename T>
double Matrix<T>::getMean() const {
    return (
        this->getElementSum() /
        (this->getRowSize() * this->getColSize())
    );
}

/* 矩阵最大值 */
template <typename T>
double Matrix<T>::getMax() const {
    double output{matrix_[0][0]};
    for (mtxSizet i = 0; i < row_; ++i) {
        for (mtxSizet j = 0; j < col_; ++j) {
            if (matrix_[i][j] > output) {
                output = matrix_[i][j];
            }
        }
    }
    return output;
}

/* 矩阵最小值 */
template <typename T>
double Matrix<T>::getMin() const {
    double output{matrix_[0][0]};
    for (mtxSizet i = 0; i < row_; ++i) {
        for (mtxSizet j = 0; j < col_; ++j) {
            if (matrix_[i][j] < output) {
                output = matrix_[i][j];
            }
        }
    }
    return output;
}

/* 判断是否为全零矩阵 */
template<typename T>
bool Matrix<T>::iszero() const {
    for (std::size_t r{0}; r < this->row_; ++r) {
        for (std::size_t c{0}; c < this->col_; ++c) {
            if (std::abs(this->matrix_[r][c]) > util::EPSILON) {
                return false;
            }
        }
    }
    return true;
}

/* 重构矩阵大小
 * - 默认所有元素初始化为 0
 * - 默认构造 0 * 0 的空矩阵*/
template <typename T>
void Matrix<T>::reconstruct(mtxSizet row, mtxSizet col) {
    row_ = row;
    col_ = col;
    initMtx(row, col);
}

} // namespace core
