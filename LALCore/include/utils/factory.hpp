// matrix builder
// matrix factory
// random generator

#include "core/matrix.hpp"

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
};
extern Factory factr;

} // namespace util