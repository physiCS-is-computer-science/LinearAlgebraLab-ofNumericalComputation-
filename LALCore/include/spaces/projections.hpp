// projector
// orthogonal projection

#pragma once

#include "core/matrix.hpp"
#include "spaces/subspaces.hpp"

namespace subsp {
    
class Projection {
public:
    Projection() = default;
    ~Projection() = default;
    Projection(const Projection&) = default;
    Projection(Projection&&) = default;
    Projection& operator=(const Projection&) = delete;
    Projection& operator=(Projection&&) = delete;

    Projection(const core::dmtx& m) : origDmtx_(m) {}

    core::dmtx orig() const { return origDmtx_; }

    /* 投影矩阵 P（A(~AA)^-1~A 在 A 化为正交基矩阵 Q 时退化为 Q(~QQ)^-1~Q = QI~Q = Q~Q） */
    core::dmtx projmtx() const {
        subsp::SubSpace spcer(origDmtx_);
        core::dmtx colSpaceMtx = spcer.colBase();
        return colSpaceMtx * (~colSpaceMtx);
    }
    
private:
    const core::dmtx origDmtx_;
};

} // namespace subsp