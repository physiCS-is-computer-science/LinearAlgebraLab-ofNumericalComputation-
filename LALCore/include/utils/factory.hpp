// matrix builder
// matrix factory
// random generator

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include <utility>

namespace util {

class Factory {
public:
    /* 判断 */
    static bool iseye(const core::dmtx& mtx);

    /* 制造 */
    static core::dmtx eye(std::size_t dim);
    static core::dvec avec(const core::dvec& v) { return core::dvec(v); }
    static core::dvec avec(
        std::size_t dim,
        double elem = 0.0,
        core::VecOrientation orien = core::VecOrientation::COLUMN
    );
    static core::dmtx amtx(const core::dmtx& m) { return core::dmtx(m); }
    static core::dmtx amtx(std::size_t row, std::size_t col, double elem = 0.0);

    /* 功能 */
    static core::dmtx catRow(const core::dmtx& mtx1, const core::dmtx& mtx2);
    static core::dmtx catCol(const core::dmtx& mtx1, const core::dmtx& mtx2);
    static std::pair<core::dmtx, core::dmtx> splitRow(const core::dmtx& mtx, std::size_t row);
    static std::pair<core::dmtx, core::dmtx> splitCol(const core::dmtx& mtx, std::size_t col);
    static core::dvec subvec(const core::dmtx& m, std::size_t r, std::size_t c);
};

} // namespace util