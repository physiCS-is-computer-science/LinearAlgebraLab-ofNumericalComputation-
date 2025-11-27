// linear solver
// cholesky solver
// LU solver
// QR solver
// conjugate gradient solver
// gauss seidel solver
// GMRES solver
// jacobi solver

// =================================
// 基础求解器，调用基础分解部分求解
// =================================

#include "core/matrix.hpp"
#include "core/vector.hpp"
namespace solve {

class Solver {
public:
    Solver() = default;
    ~Solver() = default;
    Solver(const Solver&) = default;
    Solver(Solver&&) = default;
    Solver& operator=(const Solver&) = default;
    Solver& operator=(Solver&&) = default;
    Solver(const core::dmtx &m) : decomper_(m), R_(decomper_.rref()) {}

    core::dmtx getorigMtx() { return decomper_.getorigMtx(); } // 获取原始矩阵

    core::dmtx homogeneousSolve() const; // 求解 Ax = 0（A 的零空间），返回零空间基拼接而成的矩阵 N
    core::dmtx solveAxb(const core::dvec &b) const; // 求解 Ax
    
private:
const decomp::BaseDecomposer decomper_; // 绑定分解器
const core::dmtx R_; // 原始矩阵化为行最简型
};

} // namespace solve
