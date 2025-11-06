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

    core::dmtx gausElimi() { return gaussianElimination()[1]; } // 调用 gaussianElimination()，仅仅返回消元矩阵
    std::vector<core::dmtx> lu(); // 调用 gaussianElimination()，返回 L、U、P
    core::dmtx rref(); // 化为行最简型

private:
    std::vector<core::dmtx> gaussianElimination(); // 高斯消元法，化为行阶梯矩阵（非最简 R），返回 L、U、P

    const core::dmtx origDmtx_{}; // 被操作矩阵，初始化时写入，其他时候不能改变
};

} // namespace decomp