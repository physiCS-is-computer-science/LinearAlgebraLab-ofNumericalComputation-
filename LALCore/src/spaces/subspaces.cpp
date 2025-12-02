// fundamental subspaces
// subspace angles

#include "core/matrix.hpp"
#include "decompositions/advanced_decomp.hpp"
#include "solvers/linear_solvers.hpp"
#include "spaces/subspaces.hpp"
#include "utils/factory.hpp"

namespace subsp {

/* 计算 m*n 矩阵 A 列空间的一组基
 * - 豪斯霍尔德方法进行 QR 分解，得到的 Q 的左半部分（1 ~ r 列，r 为 A 的秩）一定为列空间基
 *   右半部分（r ~ m）一定为左零空间基
 * - 直接取 Q 左半部分即可得到 A 列空间一组基 */
core::dmtx SubSpace::colBase() const {
    decomp::AdvancedDecomposer decomper(origDmtx_);
    core::dmtx Q = decomper.householderQR().first;
    std::size_t rank = decomper.getrank();

    return util::Factory::splitCol(Q, rank).first; // 秩数个列即为列空间的一组基，右半部分为左零空间基
}

/* 计算 m*n 矩阵 A 零空间一组基
 * - 解线性方程组 Ax = 0 得到（也可用 Householder QR 分解中，Q 的右半部分） */
core::dmtx SubSpace::nullBase() const {
    solve::Solver solver(origDmtx_);
    return solver.homogeneousSolve();
}

} // namespace subsp