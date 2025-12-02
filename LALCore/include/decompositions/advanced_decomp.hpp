// SVD、schur eigen decomposition

// ===================================================================
// AdvancedDecomposer 不仅完成矩阵分解，也完成矩阵正交化（Householder QR 分解）
// ===================================================================

#pragma once

#include "decompositions/basic_decomp.hpp"
#include <utility>

namespace decomp {

class AdvancedDecomposer : protected BaseDecomposer {
public:
    AdvancedDecomposer() = default;
    ~AdvancedDecomposer() override = default;
    AdvancedDecomposer(const AdvancedDecomposer&) = default;
    AdvancedDecomposer(AdvancedDecomposer&&) = default;
    AdvancedDecomposer& operator=(const AdvancedDecomposer&) = delete;
    AdvancedDecomposer& operator=(AdvancedDecomposer&&) = delete;

    AdvancedDecomposer(const BaseDecomposer& other) : BaseDecomposer(other) {}

    core::dmtx origmtx() { return getorigMtx(); }
    
    std::pair<core::dmtx, core::dmtx> householderQR(); // 返回 Q R
};

} // namespace decomp