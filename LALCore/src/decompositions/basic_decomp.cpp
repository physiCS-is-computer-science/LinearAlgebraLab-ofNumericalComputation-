// LU 、QR、cholesky decomposition

#include "decompositions/basic_decomp.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "numerical.hpp" // 涉及 double 的相等比较都要用 const 常量 EPSILON 判断
#include "utils/factory.hpp"
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

namespace decomp {

/* LU 分解 */
std::vector<core::dmtx> BaseDecomposer::lu() {
    if (!origDmtx_.isSquare()) {
        throw std::invalid_argument("BaseDecomposer::lu(): origDmtx_ is not a square matrix");
    }
    return gaussianElimination();
}

/* 化为行最简型
 * - 调用 gaussianElimination() 返回的 U，返回最简型
 * - 失败返回空矩阵 */
core::dmtx BaseDecomposer::rref() {
    if (origDmtx_.isEmpty()) {
        return {};
    }

    core::dmtx R = this->gaussianElimination()[1]; // 返回的 U
    for (std::size_t row{R.getRowSize() - 1}; row > 0; --row) { // 第 0 行不需要消元
        core::dvec pivotRow{R.getRow(row)}; // 获取当前行作为主元行
        std::size_t col{firstNonzero(pivotRow)}; // 当前主元行第一个非 0 元素的列坐标（主元列）
        if (col >= R.getColSize()) { // 返回大于最大列下标的索引表示全 0 行
            continue;
        }

        /* 向上消元 */
        R.replaceRow(row, pivotRow * (1 / R(row, col))); // 主元化为 1
        pivotRow = R.getRow(row); // 更新主元行
        for (int r{row - 1}; r >= 0; --r) {
            double up{R(r, col)}, pivot{R(row, col)}, l{}; // 当前主元之上的元素、当前主元、乘数
            core::dvec upRow{R.getRow(r)}; // 待消元行

            l = up / pivot; // 此处主元一定为 1。代码逻辑后续可能更改
            R.replaceRow(r, upRow - pivotRow * l); // 行加减
        }
    }
    R.replaceRow(0, R.getRow(0) * (1 / R(0, firstNonzero(R.getRow(0))))); // 第 0 行主元化为 1

    return R;
}

/* 方阵求逆
 * - 方阵右侧插入同维度单位矩阵，对长方阵化为行最简型，右侧矩阵即为逆
 * - 维度错误、矩阵奇异都返回空矩阵 */
core::dmtx BaseDecomposer::inv() {
    if (!origDmtx_.isSquare()) {
        return {};
    }

    core::dmtx U{this->lu()[1]}; // 返回 U，检验是否为奇异矩阵
    std::size_t inx{decomp::firstNonzero(U.getRow(U.getRowSize() - 1))}; // 获取矩阵最后一行
    if (inx >= U.getColSize()) { // 返回的索引大于最大索引即为全 0 行，故矩阵奇异
        return {};
    }

    core::dmtx AE = util::factr.catCol(origDmtx_, util::factr.eye(origDmtx_.getRowSize()));
    decomp::BaseDecomposer temp{AE};
    AE = temp.rref(); // 返回的矩阵右侧为逆

    std::pair<core::dmtx, core::dmtx> E_A = util::factr.splitCol(AE, AE.getColSize() / 2); // 左为单位矩阵，右为逆

    return E_A.second;
}

// /* 找到行阶梯矩阵的所有主元坐标对
//  * - 非行阶梯矩阵返回空 vector */
// std::vector<std::pair<std::size_t, std::size_t>> findPivot(const core::dmtx& mtx) {
//     core::dmtx testMtx(mtx);
//     core::dmtx P = exchangeRowToLadderMtx(testMtx); // 尝试换行为行阶梯矩阵
//     if (P != util::factr.eye(P.getRowSize())) { // P 如果不等于单位矩阵则表示进行了换行，因而 A 不是标准行阶梯矩阵
//         return {};
//     }

//     std::vector<std::pair<std::size_t, std::size_t>> output{};

//     for (std::size_t scanRow = 0; scanRow < AP[0].getRowSize(); ++scanRow) { // 当前扫描的行
//         for (std::size_t col = 0; col < AP[0].getColSize(); ++col) { // 从左往右扫描第一个非零元素
//             if (std::abs(AP[0](scanRow, col)) <= util::EPSILON) { // 接近 0 时直接跳过
//                 continue;
//             }
//             for (std::size_t row = AP[0].getRowSize() - 1; row >= scanRow; --row) { // 从下往上扫描第一个非 0 元素
//                 if (std::abs(AP[0](row, col)) > util::EPSILON && row == scanRow) { // 遇到非 0 元素且是主元时
//                     output.push_back(std::make_pair(scanRow, col));
//                     break;
//                 }
//                 else if (std::abs(AP[0](row, col)) > util::EPSILON && row != scanRow) { // 不是主元
//                     break;
//                 }
//             }
//             break; // 如果执行到此处，必须强制结束循环
//         }
//     }

//     return output;
// }

/* 行交换得到行阶梯矩阵
 * - 返回行交换矩阵 P（只针对方阵）
 * - 如果传递非方阵，换行矩阵 P 是错误的 */
core::dmtx exchangeRowToLadderMtx(core::dmtx& mtx) {
    if (mtx.getRowSize() == 1) {
        return util::factr.eye(1);
    }

    std::vector<std::size_t> firstNon0s(mtx.getRowSize()); // 索引为行，存储的元素为此行第一个非 0 元素的列数
    for (std::size_t i{0}; i < mtx.getRowSize(); ++i) { // 填充首位非 0 元素数组，内容为“第 inx 行第一个非 0 元素为 fi..s[inx]”
        firstNon0s[i] = firstNonzero(mtx.getRow(i));
    }

    std::size_t minDim = mtx.getRowSize() < mtx.getColSize() ? mtx.getRowSize() : mtx.getColSize(); // 最小维度
    core::dmtx P(util::factr.eye(minDim)); // 换行矩阵

    /* 插入排序，此处效率极其低下，因为只要遇到小于 slow 的则换行。如果找最大元素再换行会提升很多效率。我好累，以后再改这里，希望那时你看得到吧梦奇 */
    for (std::size_t slow{0}; slow < firstNon0s.size() - 1; ++slow) { // 函数开头以保证矩阵至少有两行
        for (std::size_t fast{slow + 1}; fast < firstNon0s.size(); ++fast) {
            if (firstNon0s[slow] > firstNon0s[fast]) { // slow 小于等于 fast 的话就不动，是正确的行阶梯
                std::swap(firstNon0s[slow], firstNon0s[fast]);
                mtx.exchangeRow(slow, fast); // 矩阵同步换行
                if (slow < minDim && fast < minDim) { // 非方阵时此处 P 不记录，因而导致错误的 P
                    P.exchangeRow(slow, fast); // P 记录
                }
            }
        }
    }

    return P;
}

/* 返回向量第一个非 0 元素的索引
 * - 从左向右、从上向下
 * - 全 0 向量时返回一个大于向量维度的数 */
std::size_t firstNonzero(const core::dvec& vec) {
    std::size_t i{0};
    while (i < vec.getSize()) {
        if (std::abs(vec(i)) > util::EPSILON) {
            break;
        }
        ++i;
    }
    return i;
}

/* 高斯消元法（包含 LU分解，附加换行矩阵 P）
 * - 返回 vector<>，分别为 L、U、P
 * - 如果 origDmtx_ 为非方阵，得到的 P 为错误的 P（详情见行变换循环）！！！
 * - 当前主元位置为 0 时，寻找下方是否存在非 0，存在则行交换，不存在则跳到下一列（即全 0 列、主元位置之下的全 0 列都不进行任何操作）
 * - 消元逻辑考虑数值稳定性，遇到 0、非最大主元都换行，以减小误差（MATLAB） */
std::vector<core::dmtx> BaseDecomposer::gaussianElimination() {
    core::dmtx copy(origDmtx_); // 不改变原矩阵

    const std::size_t minDim = copy.getRowSize() < copy.getColSize() ? copy.getRowSize() : copy.getColSize();
    core::dmtx L(minDim), P(util::factr.eye(minDim)); // 最小维度

    /* LU 分解，每一行开始消元时，都先执行行变换，使得矩阵为行阶梯型，再进行消元算法 */
    for (std::size_t row = 0; row < copy.getColSize() && row < copy.getRowSize() - 1; ++row) { // 逐行扫描，
        core::dmtx correctP = exchangeRowToLadderMtx(copy); // 每次行加减时，先化为行阶梯
        L = correctP * L; // 对 L 同步对 U 的交换

        core::dvec pivotRvec(copy.getRow(row)); // 获取当前主元行
        std::size_t col = firstNonzero(pivotRvec); // 当前主元行的第一个非 0 元素，即为当前主元的列坐标
        if (col >= copy.getColSize()) { // 全 0 行时（全 0 行则返回超出范围的正整数）直接退出消元，因为上方已化为行阶梯，全 0 行后必然还是全 0 行
            P = P * correctP; // 同步更正行互换矩阵
            break;
        }

        core::dvec pivotCvec(copy.getCol(col)); // 获取当前主元列，为第一个非 0 元素所在列 col
        double pivotElem = copy(row, col); // 当前主元列的主元

        /* 寻找最大主元（绝对值最大）减小误差，全 0 列不进行任何操作 */
        std::size_t maxPivotRow{copy.getRowSize()}; // 初始化此处索引为矩阵之外，用于判断是否找到比当前主元更大的元素
        for (std::size_t r = row + 1; r < copy.getRowSize(); ++r) { // 从当前元素 (row, row) 之下的 (row+1, row) 开始找
            if (std::abs(pivotCvec(r)) > std::abs(pivotElem)) { // 当前主元列中，有绝对值大于当前主元的元素时，最大主元行刷新，后续进行交换
                maxPivotRow = r;
            }
        }
        if (maxPivotRow != copy.getRowSize()) { // 找到比当前主元更大的元素时
            copy.exchangeRow(row, maxPivotRow);

            /* 行交换后重新获取 */
            pivotRvec = copy.getRow(row); // 获取当前主元行
            pivotCvec = copy.getCol(col); // 获取当前主元列
            pivotElem = pivotCvec(row); // 当前主元 (row, col)

            if (row < minDim && maxPivotRow < minDim) { // 如果为非方阵，此处 P 不进行行变换，但是 U 进行了行变换！故 P 为错误矩阵
                P.exchangeRow(row, maxPivotRow);
                L.exchangeRow(row, maxPivotRow); // L 同步 U 的行交换
            }
        }

        P = P * correctP; // 化为行阶梯阵时的 correctP 更正矩阵 P，且 P 左乘之

        /* 行加减，全 0 列时不进行任何操作 */
        for (std::size_t r = row + 1; r < copy.getRowSize(); ++r) {
            if (std::abs(pivotCvec(r)) <= util::EPSILON) { // 判断主元列的当前行的元素是否接近 0
                continue;
            }

            /* r2 - r1 * b/a */
            double l = pivotCvec(r) / pivotElem; // l = b/a
            copy.replaceRow(r, copy.getRow(r) - pivotRvec * l); // r2 - r1*l

            /* 写入 L */
            if (r < minDim && col < minDim) { // 此消元法涉及非方阵，故不做此判断会访问未定义内存
                if (std::abs(l) <= util::EPSILON) {
                    l = 0;
                }
                L(r, row) = l; // L
            }
        }
    }

    /* 全面检查 U，过小元素视为 0。必须在此处进行判断，因为在消元时提前设定某些元素为 0 会导致比较大的结果误差！（三次消元操作后误差大于1！！！） */
    for (std::size_t i{0}; i < copy.getRowSize(); ++i) {
        for (std::size_t j{0}; j < copy.getColSize(); ++j) {
            if (std::abs(copy(i, j)) <= util::EPSILON)
                copy(i, j) = 0;
        }
    }

    for (std::size_t i{0}; i < minDim; ++i) { // 手动构造 1 主对角线
        L(i, i) = 1;
    }

    return {L, copy, P};
}

} // namespace decomp