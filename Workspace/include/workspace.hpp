// ============================================
// 工作空间，由 LALCore 调用
// ============================================

#pragma once

class Workspace {
public:
    /* 仅仅被声明一次，不可复制 */
    Workspace() = default;
    ~Workspace() = default;
    Workspace(const Workspace&) = delete;
    Workspace(Workspace&&) = delete;
    Workspace& operator=(const Workspace&) = delete;
    Workspace& operator=(Workspace&&) = delete;
};

extern Workspace ws; // 不可复制，仅仅作为接口