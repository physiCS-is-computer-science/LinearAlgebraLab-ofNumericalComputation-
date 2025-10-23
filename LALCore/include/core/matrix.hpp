// // ==============================================================================
// // # Matrix类，存储矩阵对象，输出矩阵基本性质，进行矩阵自身或矩阵间的简单运算
// // - 矩阵基本信息：尺寸、元素数量等
// // - 矩阵状态判断：是否为标量、向量、空矩阵
// // - 矩阵基本运算：加、减、乘、逐元素乘、逐元素除、逐元素幂、元素求和、最值、转置等
// // ------------------------------------------------------------------------------
// // 为符合数学习惯，定义 vector<double> 为列，vector<vector<double>> 为行，即为：
// // vector<vector<double>> 1
// // vector<vector<double>> 2
// // ...
// // ------------------------------------------------------------------------------
// // # 矩阵基本运算
// // - 矩阵加法 A + B
// // - 矩阵减法 A - B
// // - 矩阵间、矩阵常量间乘法 A * B
// // - 逐元素乘 times(A, B)
// // - 矩阵幂 A^c
// // - 逐元素幂 power(A, B)
// // - 矩阵间左除右除 mLeftDivide(A, B) mRightDivide(A, B)
// // - 逐元素左除右除 leftDivide(A, B) rightDivide(A, B)
// // - 矩阵比较 A == B  A != B
// // - 转置 ~A
// // - 共轭转置 ctranspose(A)
// // - 逆 !A
// // ==============================================================================

// #pragma once

// #include <string>
// #include <vector>

// /* ========= 以下为模板重构 ========= */
// // template <typename T>
// // class  {

// // }

// /* ========= 以下为旧类 ========= */

// // class Vector; // 向前声明，避免 vector.hpp 和 matrix.hpp 相互循环 include 导致报错

// class Matrix {
// public:
//     using matrixSizet = std::vector<double>::size_type;

//     /* friend */
//     friend Matrix operator+(const Matrix& matrix1, const Matrix& matrix2);
//     friend Matrix operator-(const Matrix& matrix1, const Matrix& matrix2);
//     friend Matrix operator*(const Matrix& matrix1, const Matrix& matrix2);
//     friend Matrix operator*(const Matrix& matrix, double scalar);
//     friend Matrix operator*(double scalar, const Matrix& matrix); // 为提高性能，声明为友元直接访问 private 成员
//     friend bool operator==(const Matrix& matrix1, const Matrix& matrix2);
//     friend bool operator!=(const Matrix& matrix1, const Matrix& matrix2);
//     friend Matrix operator~(const Matrix& matrix);
//     friend Matrix times(const Matrix& matrix1, const Matrix& matrix2);
//     friend Matrix power(const Matrix& matrix1, const Matrix& matrix2);
//     friend Matrix divide(const Matrix& matrix1, const Matrix& matrix2);
//     friend Vector tovec(const Matrix& matrix);

//     /* 默认的六大特殊函数 */
//     Matrix() = default;
//     ~Matrix() = default;
//     Matrix(const Matrix&) = default;
//     Matrix(Matrix&&) = default;
//     Matrix& operator=(const Matrix&) = default;
//     Matrix& operator=(Matrix&&) = default;

//     /* 类型转换 */
//     operator Vector() const; // 将一行或一列的矩阵在需要时转换为 Vector

//     /* 初始化构造函数 */
//     Matrix(matrixSizet row, matrixSizet col) : row_(row), col_(col), matrix_(row, std::vector<double>(col, 0.0)) {} // r 行 c 列的矩阵，默认初始化为 0
//     Matrix(matrixSizet dimension) : row_(dimension), col_(dimension), matrix_(dimension, std::vector<double>(dimension, 0.0)) {} // dimension 维的方阵，初始化为 0
//     Matrix(std::initializer_list<std::initializer_list<double>> initList); // 初始化列表初始化，{{1, 2, 3}, {4, 5, 6};

//     /* 运算符重载 */
//     double& operator()(matrixSizet row, matrixSizet col) { return matrix_[row][col]; } // 用 M(0, 1) 访问矩阵第一行第二列元素
//     const double& operator()(matrixSizet row, matrixSizet col) const { return matrix_[row][col]; } // 同上，const 版本，详细说明在 Vector 的()重载函数

//     /* member functions */
//     matrixSizet getRowSize() const { return row_; } // 获取行大小
//     matrixSizet getColSize() const { return col_; } // 获取列大小
//     Vector getRow(matrixSizet row) const; // 获取 row 行
//     Vector getCol(matrixSizet col) const; // 获取 col 列
//     Vector getMainDiag() const; // get main diagonal elements
//     Vector getAntiDiag() const; // get anti-diagonal elements
//     double getTrace() const; // 迹
//     double getElementSum() const; // 获取所有元素和
//     double getMean() const; // 获取所有元素平均值
//     double getMax() const; // 获取最大值
//     double getMin() const; // 获取最小值
//     bool isSquare() const { return row_ == col_; } // 判断是否为方阵
//     bool isScalar() const { return row_ == col_ && row_ == 1; } // 判断是否为常量，it is a scalar if it has only 1 element
//     bool isVector() const { return row_ == 1 || col_ == 1; } // 判断是否为向量
//     bool isEmpty() const { return row_ == 0 && col_ == 0; } // 判断是否为空矩阵
//     void reconstruct(matrixSizet row = 0, matrixSizet col = 0); // 重新构建矩阵大小，默认初始化元素为0，默认重构大小为 0 * 0 的空矩阵

// private:
//     matrixSizet col_ = 0;
//     matrixSizet row_ = 0;
//     std::vector<std::vector<double>> matrix_;
// };

// /* 运算符重载 */
// Matrix operator+(const Matrix& matrix1, const Matrix& matrix2); // +
// Matrix operator-(const Matrix& matrix1, const Matrix& matrix2); // -
// Matrix operator*(const Matrix& matrix1, const Matrix& matrix2); // matrix * matrix
// Matrix operator*(const Matrix& matrix, double scalar); // matrix(left) * scalar(right)
// inline Matrix operator*(double scalar, const Matrix& matrix) { return matrix * scalar; } // scalar(left) * matrix(right)
// bool operator==(const Matrix& matrix1, const Matrix& matrix2); // ==
// bool operator!=(const Matrix& matrix1, const Matrix& matrix2); // !=
// Matrix operator~(const Matrix& matrix); // 转置
// std::ostream& operator<<(std::ostream& os, const Matrix& matrix); // 默认矩阵末尾输出一个 '\n'

// Matrix times(const Matrix& matrix1, const Matrix& matrix2); // 逐元素乘
// Matrix power(const Matrix& matrix1, const Matrix& matrix2); // 逐元素幂
// Matrix divide(const Matrix& matrix1, const Matrix& matrix2); // 逐元素除

// // void printMatrix(const Matrix& matrix, std::string messageBegin = "", std::string messageEnd = ""); // print a matrix formatly
// Vector tovec(const Matrix& matrix); // 转换为一个 Vector