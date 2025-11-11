// matrix builder
// matrix factory
// random generator

#include "core/matrix.hpp"
#include <utility>

namespace util {

class Factory {
public:
    /* 判断 */
    bool iseye(const core::dmtx& mtx);

    /* 制造 */
    core::dmtx eye(std::size_t dim);

    /* 功能 */
    core::dmtx catRow(const core::dmtx& mtx1, const core::dmtx& mtx2);
    core::dmtx catCol(const core::dmtx& mtx1, const core::dmtx& mtx2);
    std::pair<core::dmtx, core::dmtx> splitRow(const core::dmtx& mtx, std::size_t row);
    std::pair<core::dmtx, core::dmtx> splitCol(const core::dmtx& mtx, std::size_t col);
};
extern Factory factr;

} // namespace util