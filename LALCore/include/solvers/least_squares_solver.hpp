// 普通最小二乘
// 正则化最小二乘
// 加权最小二乘

#pragma once

#include "core/matrix.hpp"
#include "core/vector.hpp"

namespace solve {

class LeastSquaresSolver {
public:
    LeastSquaresSolver() = default;
    ~LeastSquaresSolver() = default;
    LeastSquaresSolver(const LeastSquaresSolver&) = default;
    LeastSquaresSolver(LeastSquaresSolver&&) = default;
    LeastSquaresSolver& operator=(const LeastSquaresSolver&) = delete;
    LeastSquaresSolver& operator=(LeastSquaresSolver&&) = delete;

    LeastSquaresSolver(const core::dmtx& m) : origDmtx_(m) {}
    
    core::dmtx solveAxhb(const core::dvec& b) const;
    
private:
    const core::dmtx origDmtx_;
};

} // namespace solve