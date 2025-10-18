// matrix_vector_output.hpp

#include "utils/output.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

/* 矩阵格式化输出
 * - 末尾追加一个 '\n' */
std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    if (os.fail()) {
        throw std::ios::failure("matrix_vector_output.cpp: operator<<(): std::cout.fail() == true");
        return os;
    }

    if (matrix.getColSize() == 0 || matrix.getRowSize() == 0) {
        os.setstate(std::ios::failbit); // failbit 置位
        return os;
    }

    for (Matrix::matrixSizet row = 0; row < matrix.getRowSize(); ++row) {
        Vector temp(matrix.getRow(row)); // 临时行
        for (Vector::vectorSizet i = 0; i < temp.getSize(); ++i) // 逐个输出临时行的元素
            std::cout << temp(i) << '\t';
        std::cout << '\n'; // put a '\n' at the end of matrix
    }
    return os;
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
