// linear solver
// cholesky solver
// LU solver
// QR solver
// conjugate gradient solver
// gauss seidel solver
// GMRES solver
// jacobi solver

#include "decompositions/basic_decomp.hpp"
#include "solvers/linear_solvers.hpp"
#include "utils/factory.hpp"
#include "utils/numerical.hpp"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <set>

namespace solve {

/* 求解齐次线性方程组 Ax = 0
 * - 返回一个零空间矩阵 N ，该矩阵以列向量拼接，即为 x 所有解之线性组合向量。
 *   换句话说，N 的列向量张成矩阵 A 的零空间。
 * - 无论矩阵 A 是否有自由变量，零空间一定存在（至少为一个零向量），故返回的 N 一定为非空矩阵。
 * - 使用 Solver 类初始化绑定的成员 origDmtx_ 作为 A。
 * - 有自由变量时（即列数大于秩）
 *    1.计算 当前行与求解向量 之点积，求解向量为被设计的向量，使得 0-点积 为当前主元解
 *    2.当前行与求解向量二者之列一一对应，当前行主元变量及其左边所有列，对应的求解向量列都为 0
 *    3.当前行与求解向量 计算点积（主元变量位置为 0，被消去，及其左边一切都为 0），得到一个值 a，
 *      计算 0-a 即得到当前主元变量解
 *    4.要求使用行最简型矩阵 R，否则结果未定义 */
core::dmtx Solver::homogeneousSolve() const {
    /* 无自由变量时 */
    if (decomper_.getrank() == R_.getColSize()) { // 秩等于列数时，零空间有唯一解 0
        return util::Factory::amtx(R_.getColSize(), 1); // n 行 1 列向量（初始化为 0）
    }

    /* 求自由变量列数集合 */
    std::set<std::size_t> pivotVarCols{}; // 主元变量列数集合
    std::set<std::size_t> freeVarCols{}; // 自由变量列数集合（与上方主列数集互补）
    std::set<std::size_t> completeSet{}; // 全集
    for (std::size_t i{0}; i < R_.getRowSize(); ++i) {
        std::size_t c = decomp::firstNonzero(R_.getRow(i));
        if (c < R_.getColSize()) { // 存在主元时
            pivotVarCols.insert(c); // 主元列数集合
        }
    }
    for (std::size_t i{0}; i < R_.getColSize(); ++i) { // 构造全集
        completeSet.insert(i);
    }
    std::set_difference(completeSet.begin(), completeSet.end(),
                        pivotVarCols.begin(), pivotVarCols.end(),
                        std::inserter(freeVarCols, freeVarCols.end())); // 集合求差

    /* 构造 N */
    auto pivotIt{pivotVarCols.rbegin()}, freeIt{freeVarCols.rbegin()};
    core::dmtx N{util::Factory::amtx(R_.getColSize(), 1)};
    for (; freeIt != freeVarCols.rend();) { // 循环次数为零空间维度
        core::dvec solutionVec = util::Factory::avec(R_.getColSize()); // 求解向量（初始化为全 0）

        /* 构造零空间一个基（N 的一列） */
        for (int i{R_.getRowSize() - 1}; i >= 0; --i) {
            if (decomp::firstNonzero(R_.getRow(i)) >= R_.getColSize()) { // 全零行时跳过
                continue;
            }
            
            if (decomp::firstNonzero(R_.getRow(i)) < *freeIt) { // 构造求解向量，只处理当前行主元右边的自由变量
                solutionVec(*freeIt) = 1;
            }

            double a = -core::dot(R_.getRow(i), solutionVec); // 当前主元变量的解
            if (std::abs(a) <= util::EPSILON) {
                a = -a;
            }
            solutionVec(decomp::firstNonzero(R_.getRow(i))) = a;
        }    

        N = util::Factory::catCol(N, core::tomtx(solutionVec));
        ++freeIt;
    }
    auto oup = util::Factory::splitCol(N, 1); // 第 0 列不要（初始化构造的全零列）

    return oup.second;
}

/* 求解 Ax = b（b 为列向量，否则返回空矩阵）
 * - 无解时返回空矩阵 []
 * - 唯一解时返回 [xp 0]
 * - 无穷解时返回 [xp xn] */
core::dmtx Solver::solveAxb(const core::dvec &b) const {
    if (R_.getRowSize() != b.getSize() ||
        b.getOrientation() != core::VecOrientation::COLUMN) { // 维度、方向错误
        return {};
    }
    
    decomp::BaseDecomposer decomperAb(util::Factory::catCol(decomper_.getorigMtx(),
                                                            core::tomtx(b))); // 增广矩阵
    core::dmtx Rd{decomperAb.rref()}; // 化 [A b] 为行最简型 [R d]
    core::dmtx N{homogeneousSolve()}; // A 的零空间解（使用 R_，非增广矩阵）
    core::dmtx xp(R_.getColSize(), 1); // 初始化特解向量
    std::size_t rank{decomper_.getrank()}; // 原始矩阵 A 的秩

    /* 行满秩时 */
    if (rank == R_.getRowSize()) {
        core::dvec temp{b};
        if (b.getOrientation() == core::VecOrientation::ROW) {
            temp = ~b;
        }

        for (std::size_t r{0}; r < R_.getRowSize(); ++r) { // 构造特解向量
            std::size_t c = decomp::firstNonzero(R_.getRow(r));
            xp(c, 0) = Rd(r, Rd.getColSize() - 1); // d 对应位置赋值给 xp，xp 其余位置都为 0
        }
        
        return util::Factory::catCol(xp, N); // [xp xn]
    }
    
    /* 非行满秩时。寻找是否有 0 = 非0 的行 */
    for (std::size_t r{rank}; r < R_.getRowSize(); ++r) {
        if (Rd(r, Rd.getColSize() - 1) != 0) { // 第 r 行，最后一列的元素（即为 [R d] 的 d）
            return {};
        }
    }

    /*  */
    for (std::size_t r{0}; r < rank; ++r) { // 构造特解向量
        std::size_t c = decomp::firstNonzero(R_.getRow(r));
        xp(c, 0) = Rd(r, Rd.getColSize() - 1); // d 对应位置赋值给 xp，xp 其余位置都为 0
    }

    return util::Factory::catCol(xp, N);
}


} // namespace solve