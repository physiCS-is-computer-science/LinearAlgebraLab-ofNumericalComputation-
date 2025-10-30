// LU 、QR、cholesky decomposition

#pragma once

#include "core/matrix.hpp"

class BaseDecomposer {
public:
    BaseDecomposer() = default;
    ~BaseDecomposer() = default;
    BaseDecomposer(const BaseDecomposer&) = default;
    BaseDecomposer(BaseDecomposer&&) = default;
    BaseDecomposer& operator=(const BaseDecomposer&) = default;
    BaseDecomposer& operator=(BaseDecomposer&&) = default;
    BaseDecomposer(core::dmtx mtx) : originalDmtx_(mtx) {}

    core::dmtx gaussianElimination(); // 高斯消元法，化为行阶梯矩阵（非最简 R）

private:
    core::dmtx originalDmtx_{}; // 被操作矩阵
};