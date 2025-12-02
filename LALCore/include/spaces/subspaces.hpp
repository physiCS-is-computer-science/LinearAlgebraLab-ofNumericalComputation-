// fundamental subspaces
// subspace angles

#pragma once

#include "core/matrix.hpp"

namespace subsp {

class SubSpace {
public:
    SubSpace() = default;
    ~SubSpace() = default;
    SubSpace(const SubSpace&) = default;
    SubSpace(SubSpace&&) = default;
    SubSpace& operator=(const SubSpace&) = default;
    SubSpace& operator=(SubSpace&&) = default;

    SubSpace(const core::dmtx& m) : origDmtx_(m) {}

    core::dmtx orig() const { return origDmtx_; }

    core::dmtx colBase() const;
    core::dmtx nullBase() const;
    
private:
    const core::dmtx origDmtx_;
};

} // namespace subsp