#include "function_manager.hpp"
#include "session_manager.hpp"
#include "utils/interface.hpp"
#include <string>
#include <unordered_map>

namespace laxb {

FuncManager fnmgr;

void registerAllFunc() {
    /* ==== 矩阵创建与操作 ==== */
    fnmgr.fnReg("eye",   eye);
    fnmgr.fnReg("zeros", zeros);
    fnmgr.fnReg("ones",  ones);
    // fnmgr.fnReg("random", random);
    // fnmgr.fnReg("randn", randn);
    fnmgr.fnReg("diag",    diag);
    fnmgr.fnReg("horzcat", horzcat);
    fnmgr.fnReg("vertcat", vertcat);

    /* ==== 矩阵基本信息 ==== */
    fnmgr.fnReg("size",    size);
    fnmgr.fnReg("isequal", isequal);
    
    /* ==== 矩阵运算 ==== */
    fnmgr.fnReg("transpose", transpose);
    fnmgr.fnReg("plus",      plus);
    fnmgr.fnReg("minus",     minus);
    fnmgr.fnReg("mtimes",    mtimes);
    fnmgr.fnReg("times",     times);
    fnmgr.fnReg("divide",    divide);
    // fnmgr.fnReg("mpower", mpower);
    fnmgr.fnReg("power",  power);
    fnmgr.fnReg("sum",    sum);
    fnmgr.fnReg("mean",   mean);
    fnmgr.fnReg("max",    max);
    fnmgr.fnReg("min",    min);

    /* ==== 矩阵分解 ==== */
    fnmgr.fnReg("lu", lu);
    fnmgr.fnReg("qr", qr);
    
    /* ==== 线性方程组求解 ==== */
    fnmgr.fnReg("inv",      inv);
    fnmgr.fnReg("linsolve", linsolve);
    fnmgr.fnReg("rref",     rref);
    fnmgr.fnReg("null",     null);
    fnmgr.fnReg("orth",     orth);
    fnmgr.fnReg("lsq",      lsq);

    /* ==== 特征值与奇异值 ==== */

    /* ==== 矩阵分析与条件数 ==== */
    fnmgr.fnReg("det", det);
    fnmgr.fnReg("rank", rank);
    fnmgr.fnReg("trace", trace);

    /* ==== 特殊矩阵 ==== */

    /* ==== 基本命令 ==== */
    fnmgr.fnReg("quit", quit);
    fnmgr.fnReg("show", show);
    fnmgr.fnReg("var",  var);
    fnmgr.fnReg("del",  del);
    fnmgr.fnReg("save", save);
    fnmgr.fnReg("load", load);
    fnmgr.fnReg("cls",  cls);
    fnmgr.fnReg("help", help);
}

/* 仅仅查找函数是否存在 */
bool FuncManager::fnFind(const std::string& fnName) const {
    if (funcMap_.find(fnName) != funcMap_.end()) {
        return true;
    }
    return false;
}

} // namespace laxb