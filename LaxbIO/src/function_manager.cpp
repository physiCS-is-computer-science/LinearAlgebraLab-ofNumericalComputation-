#include "function_manager.hpp"
#include "session_manager.hpp"
#include "utils/interface.hpp"
#include <string>
#include <unordered_map>

namespace laxb {

FuncManager fnmgr;

void registerAllFunc() {
    fnmgr.fnReg("eye", eye);
    // fnmgr.fnReg("zeros", zeros);
    // fnmgr.fnReg("ones", ones);
    // fnmgr.fnReg("random", random);
    // fnmgr.fnReg("randn", randn);
    // fnmgr.fnReg("diag", diag);
}

/* 仅仅查找函数是否存在 */
bool FuncManager::fnFind(const std::string& fnName) const {
    if (funcMap_.find(fnName) != funcMap_.end())
        return true;
    return false;
}

} // namespace laxb