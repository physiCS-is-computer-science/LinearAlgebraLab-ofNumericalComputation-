// ===========================================================================
// Laxb 与 LALCore 的接口，此类中注册所有 LALCore 中的函数并重载以便调用
// - Laxb 只负责消息传递、调用，不进行任何写入读取操作
// - 存取变量全部发生在 LALCore 与 Workspace 的联系中
// - 在启动时，函数全部自动注册，后只需对应字符串的值即可调用函数
// ===========================================================================
#pragma once

#include "session_manager.hpp"
#include <functional>
#include <string>
#include <unordered_map>

namespace laxb {

class FuncManager {
public:
    /* 全部使用默认合成版本 */
    FuncManager() = default;
    ~FuncManager() = default;
    FuncManager(const FuncManager&) = delete;
    FuncManager(FuncManager&&) = delete;
    FuncManager& operator=(const FuncManager&) = delete;
    FuncManager& operator=(FuncManager&&) = delete;

    void fnReg(std::string cmd, std::function<bool(std::vector<std::string>)> f) { funcMap_[cmd] = f; } // 注册函数
    bool fnFind(const std::string& fnName) const; // 函数查找
    std::function<bool(std::vector<std::string>)> call(const std::string& cmd) { return funcMap_[cmd]; } // 函数调用，没有查找检查，意味着没有对应命令则创建一个新的，需要检查有没有该命令再使用

private:
    std::unordered_map<std::string, std::function<bool(std::vector<std::string>)>> funcMap_; // 函数映射
};
extern FuncManager fnmgr; // 唯一对象，管理函数们

void registerAllFunc(); // 注册所有函数

} // namespace laxb