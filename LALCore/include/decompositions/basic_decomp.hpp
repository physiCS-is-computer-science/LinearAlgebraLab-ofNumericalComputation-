// LU 、QR、cholesky decomposition

#pragma once

#include "core/matrix.hpp"

class Decomposer {
public:
    Decomposer() = default;
    ~Decomposer() = default;
    Decomposer(const Decomposer&) = default;
    Decomposer(Decomposer&&) = default;
    Decomposer& operator=(const Decomposer&) = default;
    Decomposer& operator=(Decomposer&&) = default;
    Decomposer(core::dmtx mtx) : originalDmtx_(mtx) {}

    core::dmtx gaussianElimination(); // 高斯消元法，化为行阶梯矩阵（非最简 R）

private:
    core::dmtx originalDmtx_{}; // 被操作矩阵
};