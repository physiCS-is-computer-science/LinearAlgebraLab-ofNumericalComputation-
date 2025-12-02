// fundamental subspaces
// subspace angles

#include "core/matrix.hpp"
#include "decompositions/advanced_decomp.hpp"
#include "solvers/linear_solvers.hpp"
#include "spaces/subspaces.hpp"
#include "utils/factory.hpp"

namespace subsp {

core::dmtx SubSpace::colBase() const {
    decomp::AdvancedDecomposer decomper(origDmtx_);
    core::dmtx Q = decomper.householderQR().first;
    std::size_t rank = decomper.getrank();

    return util::Factory::splitCol(Q, rank).first; // 秩数个列即为列空间的一组基，右半部分为左零空间基
}

core::dmtx SubSpace::nullBase() const {
    solve::Solver solver(origDmtx_);
    return solver.homogeneousSolve();
}

} // namespace subsp