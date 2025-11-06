// LU 、QR、cholesky decomposition

#include "decompositions/basic_decomp.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "numerical.hpp" // 涉及 double 的相等比较都要用 const 常量 EPSILON 判断
#include <cmath>
#include <stdexcept>
#include <vector>

namespace decomp {

/* LU 分解 */
std::vector<core::dmtx> BaseDecomposer::lu() {
    if (!origDmtx_.isSquare()) {
        throw std::invalid_argument("BaseDecomposer::lu(): origDmtx_ is not a square matrix");
    }
    return gaussianElimination();
}

/* 高斯消元法（包含 LU分解，附加换行矩阵 P）
 * - 如果 origDmtx_ 为非方阵，得到的 P 很有可能为错误的 P（详情见行变换循环）！！！
 * - 返回 pair，分别为 L、U
 * - 判断每列主元位置之下是否为 0，不是则行变换
 * - 当前位置为 0 时，寻找下方是否存在非零，存在则行交换，不存在则跳到下一列
 * - 消元逻辑考虑数值稳定性，遇到 0、非最大主元都换行，以减小误差 */
std::vector<core::dmtx> BaseDecomposer::gaussianElimination() {
    core::dmtx copy(origDmtx_); // 不改变原矩阵

    int minDim = copy.getRowSize() < copy.getColSize() ? copy.getRowSize() : copy.getColSize();
    core::dmtx L(minDim), P(minDim); // 最小维度

    for (core::dmtx::mtxSizet i = 0; i < minDim; ++i) {
        P(i, i) = 1;
    }

    for (core::dmtx::mtxSizet c = 0; c < copy.getColSize() && c < copy.getRowSize() - 1; ++c) {
        core::dvec pivotCvec(copy.getCol(c)); // 获取当前列
        core::dvec pivotRvec(copy.getRow(c)); // 当前行
        double pivotElem; // 当前主元列的主元

        /* 当前元素为 0 则行变换 */
        if (std::abs(pivotElem = pivotCvec(c)) <= util::EPSILON) {
            for (std::size_t r = c + 1; r < copy.getRowSize(); ++r) {
                if (std::abs(pivotCvec(r)) > util::EPSILON) {
                    copy.exchangeRow(c, r); // 由于 c 是正方形右下角，因而可以作为行使用

                    /* 行交换后重新获取 */
                    pivotCvec = copy.getCol(c); // 获取当前列
                    pivotRvec = copy.getRow(c); // 当前行
                    pivotElem = pivotCvec(c); // 当前主元

                    if (r < minDim && c < minDim) { // 如果为非方阵，此处 P 不进行行变换，但是 U 进行了行变换！故 P 为错误矩阵
                        P.exchangeRow(c, r);
                    }

                    break;
                }
            }
        }

        /* 寻找最大主元减小误差 */
        int maxPivotRow{-1};
        for (std::size_t r = c + 1; r < copy.getRowSize(); ++r) {
            if (pivotCvec(r) > pivotElem) {
                maxPivotRow = r;
            }
        }
        if (maxPivotRow != -1) {
            copy.exchangeRow(c, maxPivotRow);

            /* 行交换后重新获取 */
            pivotCvec = copy.getCol(c); // 获取当前列
            pivotRvec = copy.getRow(c); // 当前行
            pivotElem = pivotCvec(c); // 当前主元

            if (maxPivotRow < minDim && c < minDim) { // 如果为非方阵，此处 P 不进行行变换，但是 U 进行了行变换！故 P 为错误矩阵
                P.exchangeRow(c, maxPivotRow);
            }
        }

        /* 行加减 */
        for (core::dmtx::mtxSizet r = c + 1; r < copy.getRowSize(); ++r) {
            if (std::abs(pivotCvec(r)) <= util::EPSILON) { // 判断当前行的元素是否接近 0
                continue;
            }

            /* r2 - r1 * b/a */
            double l = pivotCvec(r) / pivotElem; // l = b/a
            copy.replaceRow(r, copy.getRow(r) - pivotRvec * l); // r2 - r1*l

            /* 写入 L */
            if (r < minDim && c < minDim) { // 此消元法涉及非方阵，故不做此判断会访问未定义内存
                if (std::abs(l) <= util::EPSILON)
                    l = 0;
                L(r, c) = l; // L
            }
        }
    }

    /* 全面检查 U，过小元素视为 0。必须在此处进行判断，因为在消元时提前设定某些元素为 0 会导致比较大的结果误差！（三次消元操作后误差大于1！！！） */
    for (core::dmtx::mtxSizet r = 0; r < copy.getRowSize(); ++r) {
        for (core::dmtx::mtxSizet c = 0; c < copy.getColSize(); ++c) {
            if (std::abs(copy(r, c)) <= util::EPSILON)
                copy(r, c) = 0;
        }
    }

    /* 手动制造主对角线 ^v^/ */
    for (core::dmtx::mtxSizet i = 0; i < minDim; ++i) {
        L(i, i) = 1;
    }

    return {L, copy, P};
}

} // namespace decomp