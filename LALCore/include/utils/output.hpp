// ========================================
// 重载 << 从而用 std::cout 输出矩阵与向量类型
// ========================================

#pragma once

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <iostream>
#include <string>

class MatrixVectorOutput {
public:
    friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix); // 默认矩阵末尾输出一个 '\n'
    friend std::ostream& operator<<(std::ostream& os, const Vector& vec); // 同上

    MatrixVectorOutput() = default;
    ~MatrixVectorOutput() = default;
    MatrixVectorOutput(const MatrixVectorOutput&) = default;
    MatrixVectorOutput(MatrixVectorOutput&&) = default;
    MatrixVectorOutput& operator=(const MatrixVectorOutput&) = default;
    MatrixVectorOutput& operator=(MatrixVectorOutput&&) = default;
};

/* 借助 std::cout 的所有重载，使得矩阵输出可以直接使用 std::cout */
std::ostream& operator<<(std::ostream& os, const Matrix& matrix); // 默认矩阵末尾输出一个 '\n'
std::ostream& operator<<(std::ostream& os, const Vector& vec); // 同上