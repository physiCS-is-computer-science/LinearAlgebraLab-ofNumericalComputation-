// ==================================
// 模仿cout行为的矩阵向量输出类
// ==================================

#pragma once

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <string>

class MatrixVectorOutput {
  public:
    MatrixVectorOutput() = default;

    MatrixVectorOutput& operator<<(const Matrix& matrix); // 默认矩阵末尾输出一个 '\n'
    MatrixVectorOutput& operator<<(const Vector& vec); // 同上
    MatrixVectorOutput& operator<<(std::string str);
    MatrixVectorOutput& operator<<(char ch);
    MatrixVectorOutput& operator<<(int val);
    MatrixVectorOutput& operator<<(double val);
    void clear() { state_ = true; }; // 恢复初始状态
    bool getState() const { return state_; }

  private:
    bool state_ = true;
};

extern MatrixVectorOutput mvout;