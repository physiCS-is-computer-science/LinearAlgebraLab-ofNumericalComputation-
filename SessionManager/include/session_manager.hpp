// ============================================
// 工作空间，由 LALCore 调用
// ============================================

#pragma once

#include "core/matrix.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace smr {

class SessionMgr {
public:
    SessionMgr() = default;
    ~SessionMgr() = default;
    SessionMgr(const SessionMgr&) = delete;
    SessionMgr(SessionMgr&&) = delete;
    SessionMgr& operator=(const SessionMgr&) = delete;
    SessionMgr& operator=(SessionMgr&&) = delete;

private:
    std::unordered_map<std::string, core::dmtx> dmVarMap{}; // 存储
    std::unordered_map<std::string, double> dVarMap{};
};

extern SessionMgr semgr; // 不可复制，仅仅作为接口

} // namespace smr