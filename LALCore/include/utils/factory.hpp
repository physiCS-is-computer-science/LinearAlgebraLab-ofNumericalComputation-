// matrix builder
// matrix factory
// random generator

#include "core/matrix.hpp"

namespace util {

class Factory {
public:
    bool iseye(const core::dmtx& mtx);

    core::dmtx eye(std::size_t dim);
};
extern Factory factr;

} // namespace util