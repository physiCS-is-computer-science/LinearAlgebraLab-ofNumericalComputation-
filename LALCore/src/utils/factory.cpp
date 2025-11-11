// matrix builder
// matrix factory
// random generator

#include "utils/factory.hpp"
#include <stdexcept>

namespace util {

Factory factr;

/* ==== 判断 ==== */
/* 判断是否为单位矩阵 */
bool Factory::iseye(const core::dmtx& mtx) {
    return mtx == eye(mtx.getRowSize());
}

/* ==== 制造 ==== */
/* 制造单位矩阵 */
core::dmtx Factory::eye(std::size_t dim) {
    core::dmtx output(dim);
    for (std::size_t i{0}; i < output.getRowSize(); ++i) {
        output(i, i) = 1;
    }
    return output;
}

/* ==== 功能 ==== */
/* 相同行数的两个矩阵的拼接
 * - 维度不匹配时返回空矩阵
 * - mtx1 在左，mtx2 在右 */
core::dmtx Factory::catRow(const core::dmtx& mtx1, const core::dmtx& mtx2) {
    if (mtx1.getRowSize() != mtx2.getRowSize()) {
        return {};
    }

    std::size_t row{mtx1.getRowSize()}, col{mtx1.getColSize() + mtx2.getColSize()};
    core::dmtx output(row, col);
    for (std::size_t r{0}; r < row; ++r) {
        std::size_t c{0};
        for (; c < mtx1.getColSize(); ++c) {
            output(r, c) = mtx1(r, c);
        }
        for (; c < col; ++c) {
            output(r, c) = mtx2(r, c - mtx1.getColSize());
        }
    }

    return output;
}

/* 相同列数的两个矩阵的拼接
 * - 维度不匹配时返回空矩阵
 * - mtx1 在上，mtx2 在下 */
core::dmtx Factory::catCol(const core::dmtx& mtx1, const core::dmtx& mtx2) {
    if (mtx1.getColSize() != mtx2.getColSize()) {
        return {};
    }

    std::size_t row{mtx1.getRowSize() + mtx2.getRowSize()}, col{mtx1.getColSize()};
    core::dmtx output(row, col);
    std::size_t r{0};
    for (; r < mtx1.getRowSize(); ++r) {
        for (std::size_t c{0}; c < col; ++c) {
            output(r, c) = mtx1(r, c);
        }
    }
    for (; r < row; ++r) {
        for (std::size_t c{0}; c < col; ++c) {
            output(r, c) = mtx2(r - mtx1.getRowSize(), c);
        }
    }

    return output;
}

} // namespace util