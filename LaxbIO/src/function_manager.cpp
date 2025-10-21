#include "function_manager.hpp"
#include "utils/interface.hpp"
#include "workspace.hpp"
#include <string>
#include <unordered_map>

FuncManager fnMgr;

void registerAllFunc() {
    fnMgr.fnReg("eye", eye);
    fnMgr.fnReg("zeros", zeros);
    fnMgr.fnReg("ones", ones);
    fnMgr.fnReg("random", random);
    fnMgr.fnReg("randn", randn);
    fnMgr.fnReg("diag", diag);
}

/* 仅仅查找函数是否存在 */
bool FuncManager::fnFind(const std::string& fnName) const {
    if (funcMap_.find(fnName) != funcMap_.end())
        return true;
    return false;
}