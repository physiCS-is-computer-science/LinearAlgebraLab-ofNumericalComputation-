#include "function_manager.hpp"
#include "session_manager.hpp"
#include "utils/interface.hpp"
#include <string>
#include <unordered_map>

namespace laxb {

FuncManager fnmgr;

void registerAllFunc() {
    /* ==== 基本命令 ==== */
    fnmgr.fnReg("quit", quit);
    fnmgr.fnReg("show", show);
    fnmgr.fnReg("var", var);
    // fnmgr.fnReg("modify", modify);
    fnmgr.fnReg("del", del);
    // fnmgr.fnReg("save", save);
    // fnmgr.fnReg("load", load);
    fnmgr.fnReg("cls", cls);
    // fnmgr.fnReg("help", help);

    /* ==== 运算 ==== */
    fnmgr.fnReg("plus", plus);
    // fnmgr.fnReg("minus", minus);
    // fnmgr.fnReg("mtimes", mtimes);
    // fnmgr.fnReg("times", times);
    // fnmgr.fnReg("divide", divide);
    // fnmgr.fnReg("mpower", mpower);
    // fnmgr.fnReg("power", power);

    /* ==== 矩阵创建与操作 ==== */
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