// =====================================================================
// 所有命令都有相似的流程，调用史上最伟大的 LaxbIO、SessionManager 模块解析传递数据
// - 每个命令的流程：
//    1.判断 args 数量，不符合则返回 false
//    2.去除末尾分号，防止参数排序分析函数 sortArgs() 返回错误消息
//    3.显示指定参数顺序和种类，调用 sortArgs() 函数分析排序 args
//    4.逐项分析返回的有序 args，使用
//    5.根据 ';' 判断是否写入 smr::semgr::cptoup_
// =====================================================================

#include "commandIO.hpp"
#include "core/matrix.hpp"
#include "session_manager.hpp"
#include <iostream>
#include <string>
#include <vector>

/* eye {num}
 * eye ::A {num} */
bool eye(std::vector<std::string> args) {
    if (args.empty()) {
        smr::semgr.seterr("eye(): Args is empty");
        return false;
    }

    /* 末尾分号 */
    auto endIt = args.end() - 1;
    if (*endIt == ";") // 去除末尾分号防止 sortArgs() 函数排序出错
        args.erase(endIt);

    /* 排序 args */
    std::size_t type{0};
    if (laxb::cmdpr.sortArgs(std::vector<laxb::Cmdt>{laxb::Cmdt::CB}, args) == true) // eye {num}
        type = 1;
    else if (laxb::cmdpr.sortArgs(std::vector<laxb::Cmdt>{laxb::Cmdt::CB, laxb::Cmdt::OUT}, args) == true) // eye ::A {num}
        type = 2;
    else {
        smr::semgr.seterr("eye(): Argument(s) error");
        return false;
    }

    /* 分析、使用 */
    core::dmtx output{};
    std::vector<double> dimens = laxb::cmdpr.curlyArgs(args[0]);
    if (dimens.size() != 1) {
        smr::semgr.seterr("eye(): Number of arguments error in \"{" + args[0].substr(1, args[0].size() - 2) + "}\"");
        return false;
    }
    if (dimens[0] == 0) {
        smr::semgr.seterr("eye(): Invalid argument \"{0}\"");
        return false;
    }
    output = core::dmtx(dimens[0]); // 制造矩阵
    for (core::dmtx::mtxSizet i = 0; i < output.getRowSize(); ++i)
        output(i, i) = 1;

    if (type == 2) {
        std::string varn{args[1].substr(2)}; // ::A
        auto it = smr::semgr.finddmtx(varn); // 去掉标识符的子串
        if (it != smr::semgr.getdmtxEnd()) {
            smr::semgr.seterr("eye(): Variable \"" + varn + "\" exist");
            return false;
        }
        smr::semgr.adddmtx(varn, output);
    }

    /* 判断是否写入 smr::semgr::cptoup_ */
    if (*endIt != ";")
        smr::semgr << output;

    return true;
}

// bool zeros(std::vector<std::string> args) {
// }

// bool ones(std::vector<std::string> args) {
// }

// bool random(std::vector<std::string> args) {
// }

// bool randn(std::vector<std::string> args) {
// }

// bool diag(std::vector<std::string> args) {
// }
