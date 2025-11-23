// matrix builder
// matrix factory
// random generator

#include "core/matrix.hpp"
#include <utility>

namespace util {

class Factory {
public:
    /* 判断 */
    static bool iseye(const core::dmtx& mtx);

    /* 制造 */
    static core::dmtx eye(std::size_t dim);

    /* 功能 */
    static core::dmtx catRow(const core::dmtx& mtx1, const core::dmtx& mtx2);
    static core::dmtx catCol(const core::dmtx& mtx1, const core::dmtx& mtx2);
    static std::pair<core::dmtx, core::dmtx> splitRow(const core::dmtx& mtx, std::size_t row);
    static std::pair<core::dmtx, core::dmtx> splitCol(const core::dmtx& mtx, std::size_t col);
};

} // namespace util