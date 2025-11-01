// =================================================
// 基础分解类
// - 分解部分：LU 、QR、cholesky
// - 辅助部分：高斯消元（非最简 R 形式）
// =================================================

#pragma once

#include "core/matrix.hpp"
#include <utility>

namespace decomp {

class BaseDecomposer {
public:
    BaseDecomposer() = default;
    ~BaseDecomposer() = default;
    BaseDecomposer(const BaseDecomposer&) = default;
    BaseDecomposer(BaseDecomposer&&) = default;
    BaseDecomposer& operator=(const BaseDecomposer&) = default;
    BaseDecomposer& operator=(BaseDecomposer&&) = default;
    BaseDecomposer(core::dmtx mtx) : origDmtx_(mtx) {} // 构造

    std::pair<core::dmtx, core::dmtx> gaussianElimination(); // 高斯消元法，化为行阶梯矩阵（非最简 R）

private:
    core::dmtx origDmtx_{}; // 被操作矩阵
};

} // namespace decomp