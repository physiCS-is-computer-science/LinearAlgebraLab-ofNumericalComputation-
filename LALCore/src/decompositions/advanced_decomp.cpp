// SVD、schur eigen decomposition

#include "decompositions/advanced_decomp.hpp"
#include "utils/factory.hpp"
#include "utils/numerical.hpp"
#include <cmath>
#include <vector>
#include <utility>

namespace decomp {

/* 豪斯霍尔德反射矩阵实现 QR分解，对于 m*n 维矩阵 A：
 * - 分别返回 Q、R
 * - Q 一定为 m 阶方阵，当 A 秩为 n 时（即各列线性无关），R 为上三角矩阵，否则为不严格的上三角矩阵
 * - 不直接计算反射矩阵 H，而是以 u 存储（H = I - 2/dot(u, ~u) * outer(u, ~u)）
 * - 计算 temp 时进行了正负号判断，根据当前子列第一个元素的正负，可以完美避免 u 为零向量的情况，
 *   当前子列为 [a ... 0] 时无需反射，直接做减法会导致 u 为零向量，而判断正负后使得 u 永远为 2*子列x
 * - 流程：
 *    1、抽取当前列子向量 x，反射至一个标准基上
 *    2、计算反射向量：u = x + sign(x0)*e，其中 e 为第一元素为 1 的单位向量
 *    3、计算常量：beta = 2/dot(u, ~u)
 *    4、更新A：HA = A - beta * outpter(u, ~u) * A
 *    5、循环直至得到 R，后使用 u 生成 Q（由反射矩阵 H 得天独厚的性质，有：Q = H1H2...HkI） */
std::pair<core::dmtx, core::dmtx> AdvancedDecomposer::householderQR() {
    std::vector<std::pair<double, core::dvec>> H{}; // H = I - beta * outer(u, ~u)
    
    core::dmtx R(origDmtx_);
    std::size_t row{R.getRowSize()}, col{R.getColSize()};
    core::dmtx Q = util::Factory::eye(row);
    
    /* (r, c) 锁定当前子向量首元素，其下（包括自身）为当前抽出以构造 u 的向量 */
    for (std::size_t r{0}, c{0}; c < col - 1 && r != row - 1; ++c) { // 最后一列、行无需执行
        if (std::abs(R(r, c)) <= util::EPSILON) { // 0 时跳过此列，行不变
            continue;
        }

        core::dvec x = util::Factory::subvec(R, r, c); // 获取子向量
        core::dvec e(x.getSize());
        double temp{x(0) >= 0 ? x.norm() : -x.norm()};
        e(0) = 1;

        core::dvec u = x + temp * e; // 反射向量
        double beta = 2 / core::dot(u, ~u);

        /* 对 u 升维 */
        core::dvec fullu(row);
        for (std::size_t i{0}; i < u.getSize(); ++i) {
            fullu(i + r) = u(i);
        }

        R = R - beta * core::outer(fullu, ~fullu) * R; // 更新 R
        H.push_back(std::make_pair(beta, fullu)); // 存储 H

        ++r;
    }

    for (int i{H.size() - 1}; i >= 0; --i) { // Q = H1H2...HkI => Q - beta * outer(u) * Q
        Q = Q - H[i].first * core::outer(H[i].second, ~(H[i].second)) * Q;
    }

    return {Q, R};
}
    
} // namespace decomp