// =============================================================
// 此文件为 Matrix 类较长的具体实现
// =============================================================

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/* 使用 initializer_list 列表视图初始化矩阵的构造函数
 * - 必须严格按照矩阵格式来初始化，{{1}, {2, 3}} 这样子初始化会得到错误的矩阵
 * - 矩阵大小以第一行的列数为矩阵列数，后续列数少于第一行则补零，多则丢弃 */
Matrix::Matrix(std::initializer_list<std::initializer_list<double>> initList) {
    row_ = initList.size();
    col_ = initList.begin()->size(); // 第一个元素指向一个 initializer_list<double>
    matrix_ = std::vector<std::vector<double>>(row_, std::vector<double>(col_, 0.0)); // 仅仅按照传入列表的第一行的长度初始化矩阵，因此不严格按照矩阵形式初始化的话，生成的矩阵有缺陷

    auto rowIt = initList.begin();
    for (matrixSizet i = 0; i < row_; ++i, ++rowIt) {
        auto colIt = rowIt->begin(); // 如果不按照标准矩阵形式，这里开始，以后有可能会访问到尾后迭代器之外，行为未定义。因此下面的循环加了if语句保护内存，然而仍然无法保护矩阵完整，故依赖coder自觉点检查
        for (matrixSizet j = 0; j < col_; ++j, ++colIt) {
            if (colIt == rowIt->end()) // 简单保护内存而已
                break;
            matrix_[i][j] = *colIt;
        }
    }
}

/* 类型转换 */
Matrix::operator Vector() const {
    Vector output;
    if (col_ == 1) { // 如果为 1 * 1 的矩阵，此处直接处理为列向量（即为不执行转置）
        output.reconstruct(row_);
        for (Vector::vectorSizet i = 0; i < row_; ++i)
            output(i) = matrix_[i][0];
    }
    else if (row_ == 1) {
        output.reconstruct(col_);
        for (Vector::vectorSizet i = 0; i < col_; ++i)
            output(i) = matrix_[0][i];
        output = ~output; // 转为行向量
    }

    return output;
}

/* ============ 运算符重载 ============ */
/* 矩阵之间加法：matrix + matrix
 * - 只检测两个矩阵行列是否完全相等 */
Matrix operator+(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_)
        throw std::invalid_argument("Matrix::operator+(): Dimensional mismatch between the two matrices");

    Matrix output(matrix1.row_, matrix2.col_);
    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j)
            output.matrix_[i][j] += matrix2.matrix_[i][j];
    }
    return output;
}

/* 矩阵之间减法：matrix - matrix
 * - 只检测两个矩阵行列是否完全相等 */
Matrix operator-(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_)
        throw std::invalid_argument("Matrix::operator-(): Dimensional mismatch between the two matrices");

    Matrix output(matrix1);
    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j)
            output.matrix_[i][j] -= matrix2.matrix_[i][j];
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
 *
 * - anbn为向量an与bn的点积，矩阵的乘积即为以下矩阵：
 * [a1b1 a1b2 a1b3 ... a1bn]
 * [a2b1 ...           ... ]
 * [...                    ]
 * [amb1 ...           ambn] */
Matrix operator*(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.col_ != matrix2.row_)
        throw std::invalid_argument("Matrix::operator*(): Dimensional mismatch between the two matrices");

    Matrix output(matrix1.row_, matrix2.col_);
    std::vector<Vector> a(matrix1.row_); // row_ 个行向量
    std::vector<Vector> b(matrix2.col_); // col_ 个列向量

    for (Matrix::matrixSizet i = 0; i < matrix1.row_; ++i)
        a[i] = matrix1.getRow(i); // 获取每一行
    for (Matrix::matrixSizet i = 0; i < matrix2.col_; ++i)
        b[i] = matrix2.getCol(i); // 获取每一列

    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j)
            output.matrix_[i][j] = dot(a[i], b[j]);
    }

    return output;
}

/* 矩阵与标量乘法：matrix(left) * scalar(right)
 * - 无错误检查*/
Matrix operator*(const Matrix& matrix, double scalar) {
    Matrix output(matrix);
    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j)
            output.matrix_[i][j] *= scalar;
    }
    return output;
}

/* 判断两个矩阵是否包括所有元素完全相等 */
bool operator==(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ == matrix2.row_ && matrix1.col_ == matrix2.col_) {
        for (Matrix::matrixSizet i = 0; i < matrix1.row_; ++i) {
            for (Matrix::matrixSizet j = 0; j < matrix1.col_; ++j) {
                if (matrix1.matrix_[i][j] != matrix2.matrix_[i][j])
                    return false;
            }
        }
        return true;
    }
    else
        return false;
}

/* 判断两个矩阵是否不完全相等 */
bool operator!=(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_)
        return true;
    else {
        for (Matrix::matrixSizet i = 0; i < matrix1.row_; ++i) {
            for (Matrix::matrixSizet j = 0; j < matrix1.col_; ++j) {
                if (matrix1.matrix_[i][j] != matrix2.matrix_[i][j])
                    return true;
            }
        }
        return false;
    }
}

/* 矩阵转置
 * - 不改变自身 */
Matrix operator~(const Matrix& matrix) {
    if (matrix.isEmpty())
        throw std::invalid_argument("Matrix::operator~(): the incomming matrix is empty");

    Matrix output(matrix.col_, matrix.row_);
    for (Matrix::matrixSizet i = 0; i < matrix.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < matrix.col_; ++j)
            output.matrix_[j][i] = matrix.matrix_[i][j];
    }
    return output;
}

/* ============= member functions ============= */
/* get a row of the matrix */
Vector Matrix::getRow(matrixSizet row) const {
    if (row >= row_ || row < 0)
        throw std::out_of_range("Matrix::getRow(): colume index " + std::to_string(row) + " out of range " + "[0, " + std::to_string(row_ - 1) + "]");

    Vector output(col_);
    for (Vector::vectorSizet i = 0; i < col_; ++i)
        output(i) = matrix_[row][i];
    output = ~output; // 转置为行向量

    return output;
}

/* get a column of the matrix
 * - if col out of range, throw out_of_range() */
Vector Matrix::getCol(matrixSizet col) const {
    if (col >= col_ || col < 0)
        throw std::out_of_range("Matrix::getCol(): colume index " + std::to_string(col) + " out of range " + "[0, " + std::to_string(col_ - 1) + "]");

    Vector output(row_);
    for (Vector::vectorSizet i = 0; i < row_; ++i)
        output(i) = matrix_[i][col];
    return output; // 默认列向量，无需转置
}

/* get a main diagonal of the matrix */
Vector Matrix::getMainDiag() const {
    if (!isSquare())
        throw std::domain_error("Matrix::getMainDiag(): expect a square matrix, but it is a non-square matrix"); // 域错误

    Vector output(row_);
    for (Vector::vectorSizet i = 0; i < row_; ++i)
        output(i) = matrix_[i][i];
    return output;
}

/* 获取主对角线元素
 * - 成功返回一个 Vector
 * - 失败抛出 domain_erroe()*/
Vector Matrix::getAntiDiag() const {
    if (!isSquare())
        throw std::domain_error("Matrix::getAntiDiag(): expect a square matrix, but it is a non-square matrix"); // 域错误

    Vector output(row_);
    for (Vector::vectorSizet i = 0; i < row_; ++i)
        output(i) = matrix_[i][row_ - 1 - i];
    return output;
}

/* 返回矩阵的迹 */
double Matrix::getTrace() const {
    if (!(this->isSquare()))
        throw std::domain_error("Matrix::getTrace(): expect a square matrix, but it is a non-square matrix"); // 域错误

    double output = 0.0;
    for (matrixSizet i = 0; i < row_; ++i)
        output += matrix_[i][i];
    return output;
}

/* 重构矩阵大小
 * - 默认所有元素初始化为 0
 * - 默认构造 0 * 0 的空矩阵*/
void Matrix::reconstruct(matrixSizet row, matrixSizet col) {
    row_ = row;
    col_ = col;
    matrix_ = std::vector<std::vector<double>>(row, std::vector<double>(col, 0.0));
}

/* ============ out of class ============ */
//// ------------------------------------------------------------------
//// # if want to print some messages at screen, pass a string object
//// - 默认参数为""，统一标明于声明
//// - 末尾不追加一个 '\n'，末尾什么都不加，而 mvout 在矩阵或向量末尾追加一个 '\n'
//// ------------------------------------------------------------------
// void printMatrix(const Matrix& matrix, string messageBegin, string messageEnd) {
//	if (!messageBegin.empty())
//		cout << messageBegin;
//
//	for (Matrix::matrixSizet row = 0; row < matrix.getRowSize(); ++row) {
//		for (const auto& i : matrix.getRow(row))
//			cout << i << '\t';
//		if (row != matrix.getRowSize() - 1) // not put last one '\n'
//			cout << '\n';
//	}
//
//	if (!messageEnd.empty())
//		cout << messageEnd;
// }

/* 矩阵逐元素相乘 */
Matrix times(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_)
        throw std::invalid_argument("Matrix::times(): Dimensional mismatch between the two matrices");

    Matrix output(matrix1);
    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j)
            output.matrix_[i][j] *= matrix2.matrix_[i][j];
    }
    return output;
}

/* 矩阵逐元素幂 */
Matrix power(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_)
        throw std::invalid_argument("Matrix::power(): Dimensional mismatch between the two matrices");

    Matrix output(matrix1);
    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j)
            output.matrix_[i][j] = pow(output.matrix_[i][j], matrix2.matrix_[i][j]);
    }
    return output;
}

/* 矩阵逐元素除
 * - 区分左右
 * - 留意除零错误，有一个 invalid_argument() 抛出 */
Matrix divide(const Matrix& matrix1, const Matrix& matrix2) {
    if (matrix1.row_ != matrix2.row_ || matrix1.col_ != matrix2.col_)
        throw std::invalid_argument("Matrix::divide(): Dimensional mismatch between the two matrices");

    Matrix output(matrix1);
    for (Matrix::matrixSizet i = 0; i < output.row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < output.col_; ++j) {
            if (matrix2.matrix_[i][j] == 0)
                throw std::invalid_argument("Matrix::divide(): Divide by zero error");
            output.matrix_[i][j] /= matrix2.matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵元素求和 */
double Matrix::getElementSum() const {
    double output = 0.0;
    for (matrixSizet i = 0; i < row_; ++i) {
        for (matrixSizet j = 0; j < col_; ++j)
            output += matrix_[i][j];
    }
    return output;
}

/* 矩阵平均值 */
double Matrix::getMean() const {
    return this->getElementSum() / (this->getRowSize() * this->getColSize());
}

/* 矩阵最大值 */
double Matrix::getMax() const {
    double output{matrix_[0][0]};
    for (Matrix::matrixSizet i = 0; i < row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < col_; ++j) {
            if (matrix_[i][j] > output)
                output = matrix_[i][j];
        }
    }
    return output;
}

/* 矩阵最小值 */
double Matrix::getMin() const {
    double output{matrix_[0][0]};
    for (Matrix::matrixSizet i = 0; i < row_; ++i) {
        for (Matrix::matrixSizet j = 0; j < col_; ++j) {
            if (matrix_[i][j] < output)
                output = matrix_[i][j];
        }
    }
    return output;
}

/* 接受 1*n 或 n*1 的 Matrix，返回对应方向的 Vector */
Vector tovec(const Matrix& matrix) {
    if (matrix.row_ != 1 && matrix.col_ != 1)
        throw std::invalid_argument("matrix.cpp: tovec(): expected a 1*n or n*1 matrix, but passed that is not");

    Vector output;

    if (matrix.row_ == 1) { // 此时为行向量
        output.reconstruct(matrix.col_);
        output = ~output; // 默认为列向量，故转置
        for (Vector::vectorSizet i = 0; i < matrix.col_; ++i)
            output(i) = matrix(0, i);
    }
    else {
        output.reconstruct(matrix.row_); // 此时为列向量
        for (Vector::vectorSizet i = 0; i < matrix.row_; ++i)
            output(i) = matrix(i, 0);
    }

    return output;
}