// 普通最小二乘
// 正则化最小二乘
// 加权最小二乘

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "solvers/least_squares_solver.hpp"
#include "solvers/linear_solvers.hpp"

namespace solve {

/* 最小二乘求解 A\hat{x} = b */
core::dmtx LeastSquaresSolver::solveAxhb(const core::dvec& b) const {
    solve::Solver solver1(origDmtx_);
    core::dmtx Np = solver1.solveAxb(b);
    if (!(Np.isEmpty())) { // 有解时无需最小二乘，直接返回解
        return Np;
    }

    /* 求解 ~AAxh = ~Ab（Bxh = d） */
    core::dmtx temp{~origDmtx_};
    core::dmtx B{temp * origDmtx_};
    core::dvec d{temp * core::tomtx(b)};
    solve::Solver solver2(B);
    Np = solver2.solveAxb(d);

    return Np;
}

} // namespace solve