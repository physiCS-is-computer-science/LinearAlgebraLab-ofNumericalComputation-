#include "utilities/matrix_vector_output.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <iostream>
#include <string>

MatrixVectorOutput mvout;

/* 矩阵格式化输出
 * - 末尾追加一个 '\n' */
MatrixVectorOutput& MatrixVectorOutput::operator<<(const Matrix& matrix) {
    if (this->getState() == false)
        return *this;
    if (matrix.getColSize() == 0 || matrix.getRowSize() == 0) {
        state_ = false;
        return *this;
    }

    for (Matrix::matrixSizet row = 0; row < matrix.getRowSize(); ++row) {
        Vector temp(matrix.getRow(row)); // 临时行
        for (Vector::vectorSizet i = 0; i < temp.getSize(); ++i) // 逐个输出临时行的元素
            std::cout << temp(i) << '\t';
        std::cout << '\n'; // put a '\n' at the end of matrix
    }
    return *this;
}

/* 向量格式化输出
 * - Vector 中重载了两个()运算符来访问，一个是 const 版本，访问 const 的 Vector，另一个是 非const 版本，访问普通 Vector */
MatrixVectorOutput& MatrixVectorOutput::operator<<(const Vector& vec) {
    if (this->getState() == false) {
        std::cout << "MatrixVectorOutput::operator<<(): mvout is error";
        return *this;
    }
    if (vec.getSize() == 0) {
        state_ = false;
        return *this;
    }

    if (vec.getOrientation() == VectorOrientation::ROW) {
        for (Vector::vectorSizet i = 0; i < vec.getSize(); ++i)
            std::cout << vec(i) << '\t';
        std::cout << '\n'; // put a '\n' at the end of vector
    }
    else {
        for (Vector::vectorSizet i = 0; i < vec.getSize(); ++i)
            std::cout << vec(i) << '\n';
    }
    return *this;
}

/* 字符串 */
MatrixVectorOutput& MatrixVectorOutput::operator<<(std::string str) {
    if (this->getState() == false) {
        std::cout << "MatrixVectorOutput::operator<<(): mvout is error";
        return *this;
    }

    std::cout << str;
    return *this;
}

/* 字符 */
MatrixVectorOutput& MatrixVectorOutput::operator<<(char ch) {
    if (this->getState() == false) {
        std::cout << "MatrixVectorOutput::operator<<(): mvout is error";
        return *this;
    }

    std::cout << ch;
    return *this;
}

/* 整型 */
MatrixVectorOutput& MatrixVectorOutput::operator<<(int val) {
    if (this->getState() == false) {
        std::cout << "MatrixVectorOutput::operator<<(): mvout is error";
        return *this;
    }

    std::cout << val;
    return *this;
}

/* 双精度浮点型 */
MatrixVectorOutput& MatrixVectorOutput::operator<<(double val) {
    if (this->getState() == false) {
        std::cout << "MatrixVectorOutput::operator<<(): mvout is error";
        return *this;
    }

    std::cout << val;
    return *this;
}