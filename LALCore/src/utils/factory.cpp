// matrix builder
// matrix factory
// random generator

#include "utils/factory.hpp"

namespace util {

Factory factr;

/* 判断是否为单位矩阵 */
bool Factory::iseye(const core::dmtx& mtx) {
    return mtx == eye(mtx.getRowSize());
}

/* 制造单位矩阵 */
core::dmtx Factory::eye(std::size_t dim) {
    core::dmtx output(dim);
    for (std::size_t i{0}; i < output.getRowSize(); ++i) {
        output(i, i) = 1;
    }
    return output;
}

} // namespace util