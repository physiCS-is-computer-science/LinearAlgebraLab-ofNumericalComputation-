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
    /* friend */

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
    core::dmtx inv(); // 矩阵求逆
    double det(); // 行列式

private:
    std::vector<core::dmtx> gaussianElimination(); // 高斯消元法，化为行阶梯矩阵（非最简 R），返回 L、U、P

    const core::dmtx origDmtx_{}; // 被操作矩阵，初始化时写入，其他时候不能改变
};

// std::vector<std::pair<std::size_t, std::size_t>> findPivot(const core::dmtx& mtx); // 寻找所有主元坐标
core::dmtx exchangeRowToLadderMtx(core::dmtx& mtx); // 行交换来构造行阶梯矩阵，返回 P
std::size_t firstNonzero(const core::dvec& vec); // 从左向右（从上到下）寻找当前向量的第一个非 0 元素，返回索引
} // namespace decomp